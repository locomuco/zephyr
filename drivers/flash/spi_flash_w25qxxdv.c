/*
 * Copyright (c) 2016 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>

#include <flash.h>
#include <spi.h>
#include <init.h>
#include <string.h>
#include "spi_flash_w25qxxdv_defs.h"
#include "spi_flash_w25qxxdv.h"
#include "flash_priv.h"

static int spi_flash_wb_access(struct spi_flash_data *ctx,
			       u8_t cmd, bool addressed, off_t offset,
			       void *data, size_t length, bool write)
{
	u8_t access[4];
	struct spi_buf buf[2] = {
		{
			.buf = access
		},
		{
			.buf = data,
			.len = length
		}
	};
	struct spi_buf_set tx = {
		.buffers = buf,
	};

	access[0] = cmd;

	if (addressed) {
		access[1] = (u8_t) (offset >> 16);
		access[2] = (u8_t) (offset >> 8);
		access[3] = (u8_t) offset;

		buf[0].len = 4;
	} else {
		buf[0].len = 1;
	}

	tx.count = length ? 2 : 1;

	if (!write) {
		const struct spi_buf_set rx = {
			.buffers = buf,
			.count = 2
		};

		return spi_transceive(ctx->spi, &ctx->spi_cfg, &tx, &rx);
	}

	return spi_write(ctx->spi, &ctx->spi_cfg, &tx);
}

static inline int spi_flash_wb_id(struct device *dev)
{
#if CONFIG_FLASH_RDID_VERIFY
	struct spi_flash_data *const driver_data = dev->driver_data;
	u32_t temp_data;
	u8_t buf[3];

	if (spi_flash_wb_access(driver_data, W25QXXDV_CMD_RDID,
				false, 0, buf, 3, false) != 0) {
		return -EIO;
	}

	temp_data = ((u32_t) buf[0]) << 16;
	temp_data |= ((u32_t) buf[1]) << 8;
	temp_data |= (u32_t) buf[2];

	if (temp_data != CONFIG_SPI_FLASH_W25QXXDV_RDID) {
		return -ENODEV;
	}
#endif /* CONFIG_FLASH_RDID_VERIFY */

	return 0;
}

static u8_t spi_flash_wb_reg_read(struct device *dev, u8_t reg)
{
	struct spi_flash_data *const driver_data = dev->driver_data;

	if (spi_flash_wb_access(driver_data, reg,
				false, 0, &reg, 1, false)) {
		return 0;
	}

	return reg;
}

static inline void wait_for_flash_idle(struct device *dev)
{
	u8_t reg;

	do {
		reg = spi_flash_wb_reg_read(dev, W25QXXDV_CMD_RDSR);
	} while (reg & W25QXXDV_WIP_BIT);
}

static int spi_flash_wb_reg_write(struct device *dev, u8_t reg)
{
	struct spi_flash_data *const driver_data = dev->driver_data;

	if (spi_flash_wb_access(driver_data, reg, false, 0,
				NULL, 0, true) != 0) {
		return -EIO;
	}

	return 0;
}

static int spi_flash_wb_read(struct device *dev, off_t offset, void *data,
			     size_t len)
{
	struct spi_flash_data *const driver_data = dev->driver_data;
	int ret;

	if (len > CONFIG_SPI_FLASH_W25QXXDV_MAX_DATA_LEN || offset < 0) {
		return -ENODEV;
	}

	k_sem_take(&driver_data->sem, K_FOREVER);

	wait_for_flash_idle(dev);

	ret = spi_flash_wb_access(driver_data, W25QXXDV_CMD_READ,
				  true, offset, data, len, false);

	k_sem_give(&driver_data->sem);

	return ret;
}

static int spi_flash_wb_write(struct device *dev, off_t offset,
			      const void *data, size_t len)
{
	struct spi_flash_data *const driver_data = dev->driver_data;
	u8_t reg;
	int ret;

	if (len > CONFIG_SPI_FLASH_W25QXXDV_MAX_DATA_LEN || offset < 0) {
		return -ENOTSUP;
	}

	k_sem_take(&driver_data->sem, K_FOREVER);

	wait_for_flash_idle(dev);

	reg = spi_flash_wb_reg_read(dev, W25QXXDV_CMD_RDSR);
	if (!(reg & W25QXXDV_WEL_BIT)) {
		k_sem_give(&driver_data->sem);
		return -EIO;
	}

	wait_for_flash_idle(dev);

	/* Assume write protection has been disabled. Note that w25qxxdv
	 * flash automatically turns on write protection at the completion
	 * of each write or erase transaction.
	 */
	ret = spi_flash_wb_access(driver_data, W25QXXDV_CMD_PP,
				  true, offset, (void *)data, len, true);

	k_sem_give(&driver_data->sem);

	return ret;
}

static int spi_flash_wb_write_protection_set(struct device *dev, bool enable)
{
	struct spi_flash_data *const driver_data = dev->driver_data;
	u8_t reg = 0;
	int ret;

	k_sem_take(&driver_data->sem, K_FOREVER);

	wait_for_flash_idle(dev);

	if (enable) {
		reg = W25QXXDV_CMD_WRDI;
	} else {
		reg = W25QXXDV_CMD_WREN;
	}

	ret = spi_flash_wb_reg_write(dev, reg);

	k_sem_give(&driver_data->sem);

	return ret;
}

static inline int spi_flash_wb_erase_internal(struct device *dev,
					      off_t offset, size_t size)
{
	struct spi_flash_data *const driver_data = dev->driver_data;
	u8_t erase_opcode;
	u32_t len;

	if (offset < 0) {
		return -ENOTSUP;
	}

	wait_for_flash_idle(dev);

	/* write enable */
	spi_flash_wb_reg_write(dev, W25QXXDV_CMD_WREN);

	wait_for_flash_idle(dev);

	switch (size) {
	case W25QXXDV_SECTOR_SIZE:
		erase_opcode = W25QXXDV_CMD_SE;
		len = W25QXXDV_LEN_CMD_ADDRESS;
		break;
	case W25QXXDV_BLOCK32K_SIZE:
		erase_opcode = W25QXXDV_CMD_BE32K;
		len = W25QXXDV_LEN_CMD_ADDRESS;
		break;
	case W25QXXDV_BLOCK_SIZE:
		erase_opcode = W25QXXDV_CMD_BE;
		len = W25QXXDV_LEN_CMD_ADDRESS;
		break;
	case CONFIG_SPI_FLASH_W25QXXDV_FLASH_SIZE:
		erase_opcode = W25QXXDV_CMD_CE;
		len = 1;
		break;
	default:
		return -EIO;

	}

	/* Assume write protection has been disabled. Note that w25qxxdv
	 * flash automatically turns on write protection at the completion
	 * of each write or erase transaction.
	 */
	return spi_flash_wb_access(driver_data, erase_opcode,
				   true, offset, NULL, len, true);
}

static int spi_flash_wb_erase(struct device *dev, off_t offset, size_t size)
{
	struct spi_flash_data *const driver_data = dev->driver_data;
	int ret = 0;
	u32_t new_offset = offset;
	u32_t size_remaining = size;
	u8_t reg;

	if ((offset < 0) || ((offset & W25QXXDV_SECTOR_MASK) != 0) ||
	    ((size + offset) > CONFIG_SPI_FLASH_W25QXXDV_FLASH_SIZE) ||
	    ((size & W25QXXDV_SECTOR_MASK) != 0)) {
		return -ENODEV;
	}

	k_sem_take(&driver_data->sem, K_FOREVER);

	reg = spi_flash_wb_reg_read(dev, W25QXXDV_CMD_RDSR);

	if (!(reg & W25QXXDV_WEL_BIT)) {
		k_sem_give(&driver_data->sem);
		return -EIO;
	}

	while ((size_remaining >= W25QXXDV_SECTOR_SIZE) && (ret == 0)) {
		if (size_remaining == CONFIG_SPI_FLASH_W25QXXDV_FLASH_SIZE) {
			ret = spi_flash_wb_erase_internal(dev, offset, size);
			break;
		}

		if (size_remaining >= W25QXXDV_BLOCK_SIZE) {
			ret = spi_flash_wb_erase_internal(dev, new_offset,
							  W25QXXDV_BLOCK_SIZE);
			new_offset += W25QXXDV_BLOCK_SIZE;
			size_remaining -= W25QXXDV_BLOCK_SIZE;
			continue;
		}

		if (size_remaining >= W25QXXDV_BLOCK32K_SIZE) {
			ret = spi_flash_wb_erase_internal(dev, new_offset,
							  W25QXXDV_BLOCK32K_SIZE);
			new_offset += W25QXXDV_BLOCK32K_SIZE;
			size_remaining -= W25QXXDV_BLOCK32K_SIZE;
			continue;
		}

		if (size_remaining >= W25QXXDV_SECTOR_SIZE) {
			ret = spi_flash_wb_erase_internal(dev, new_offset,
							  W25QXXDV_SECTOR_SIZE);
			new_offset += W25QXXDV_SECTOR_SIZE;
			size_remaining -= W25QXXDV_SECTOR_SIZE;
			continue;
		}
	}

	k_sem_give(&driver_data->sem);

	return ret;
}

#if defined(CONFIG_FLASH_PAGE_LAYOUT)
/* Equally-sized memory blocks for W25QXXDV and similar chips, e.g. MX25R8035F */
const static struct flash_pages_layout dev_layout = {
	.pages_count = (CONFIG_SPI_FLASH_W25QXXDV_FLASH_SIZE / W25QXXDV_SECTOR_SIZE),
	.pages_size = W25QXXDV_SECTOR_SIZE,
};

static void spi_flash_pages_layout(struct device *dev,
				     const struct flash_pages_layout **layout,
				     size_t *layout_size)
{
	*layout = &dev_layout;
	*layout_size = 1;
}
#endif /* CONFIG_FLASH_PAGE_LAYOUT */

static const struct flash_driver_api spi_flash_api = {
	.read = spi_flash_wb_read,
	.write = spi_flash_wb_write,
	.erase = spi_flash_wb_erase,
	.write_protection = spi_flash_wb_write_protection_set,
#if defined(CONFIG_FLASH_PAGE_LAYOUT)
	.page_layout = spi_flash_pages_layout,
#endif /* CONFIG_FLASH_PAGE_LAYOUT */
	.write_block_size = 1,
};

static int spi_flash_wb_configure(struct device *dev)
{
	struct spi_flash_data *data = dev->driver_data;

	data->spi = device_get_binding(CONFIG_SPI_FLASH_W25QXXDV_SPI_NAME);
	if (!data->spi) {
		return -EINVAL;
	}

	data->spi_cfg.frequency = CONFIG_SPI_FLASH_W25QXXDV_SPI_FREQ_0;
	data->spi_cfg.operation = SPI_WORD_SET(8);
	data->spi_cfg.slave = CONFIG_SPI_FLASH_W25QXXDV_SPI_SLAVE;

	return spi_flash_wb_id(dev);
}

static int spi_flash_init(struct device *dev)
{
	struct spi_flash_data *data = dev->driver_data;
	int ret;

	k_sem_init(&data->sem, 1, UINT_MAX);

	ret = spi_flash_wb_configure(dev);
	if (ret) {
		dev->driver_api = &spi_flash_api;
	}

	return ret;
}

static struct spi_flash_data spi_flash_memory_data;

DEVICE_INIT(spi_flash_memory, CONFIG_SPI_FLASH_W25QXXDV_DRV_NAME,
	    spi_flash_init, &spi_flash_memory_data, NULL, POST_KERNEL,
	    CONFIG_SPI_FLASH_W25QXXDV_INIT_PRIORITY);
