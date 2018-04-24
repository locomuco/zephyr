/*
 * Copyright (c) 2016 Linaro Limited.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <device.h>
#include <init.h>
#include <pinmux.h>
#include <gpio.h>
#include <sys_io.h>
#include <board.h>

#include <pinmux/stm32/pinmux_stm32.h>

/* pin assignments for NUCLEO-F411RE board */
static const struct pin_config pinconf[] = {
#ifdef CONFIG_UART_STM32_PORT_1
	// TODO clean up this workaround (there seems to be something odd in the dts at the moment)
	{STM32_PIN_PA9, (STM32_PINMUX_ALT_FUNC_7 | STM32_PUSHPULL_PULLUP)},
	{STM32_PIN_PA10, (STM32_PINMUX_ALT_FUNC_7 | STM32_PUSHPULL_PULLUP)},
#endif	/* CONFIG_UART_STM32_PORT_1 */
#ifdef CONFIG_UART_STM32_PORT_2
	{STM32_PIN_PA2, STM32F4_PINMUX_FUNC_PA2_USART2_TX},
	{STM32_PIN_PA3, STM32F4_PINMUX_FUNC_PA3_USART2_RX},
#endif	/* CONFIG_UART_STM32_PORT_2 */
#ifdef CONFIG_SPI_1
	{STM32_PIN_PA4, STM32F4_PINMUX_FUNC_PA4_SPI1_NSS},
	{STM32_PIN_PA5, STM32F4_PINMUX_FUNC_PA5_SPI1_SCK},
	{STM32_PIN_PA6, STM32F4_PINMUX_FUNC_PA6_SPI1_MISO},
	{STM32_PIN_PA7, STM32F4_PINMUX_FUNC_PA7_SPI1_MOSI},
#endif	/* CONFIG_SPI_1 */
#ifdef CONFIG_SPI_2
	{STM32_PIN_PB12, STM32F4_PINMUX_FUNC_PB12_SPI2_NSS},
	{STM32_PIN_PB13, STM32F4_PINMUX_FUNC_PB13_SPI2_SCK},
	{STM32_PIN_PB14, STM32F4_PINMUX_FUNC_PB14_SPI2_MISO},
	{STM32_PIN_PB15, STM32F4_PINMUX_FUNC_PB15_SPI2_MOSI},
#endif	/* CONFIG_SPI_1 */
	{STM32_PIN_PC14, STM32_PUSHPULL_PULLUP},
};

static int pinmux_stm32_init(struct device *port)
{
	ARG_UNUSED(port);

	stm32_setup_pins(pinconf, ARRAY_SIZE(pinconf));

	/* bring enc28j60 out of reset */
	struct device *gpioc = device_get_binding(ENC_RST_GPIO_PORT);
	gpio_pin_configure(gpioc, ENC_RST_GPIO_PIN, GPIO_DIR_OUT);
	gpio_pin_write(gpioc, ENC_RST_GPIO_PIN, 1);
	/* configure cs pin as PD */
	struct device *gpiob = device_get_binding(ENC_CS_GPIO_PORT);
	gpio_pin_configure(gpiob, ENC_CS_GPIO_PIN, (GPIO_DIR_OUT | GPIO_PUD_PULL_DOWN));

	gpio_pin_configure(gpiob, LED1_GPIO_PIN, GPIO_DIR_OUT);
	gpio_pin_write(gpiob, LED1_GPIO_PIN, 1);

	return 0;
}

SYS_INIT(pinmux_stm32_init, PRE_KERNEL_1,
		CONFIG_PINMUX_STM32_DEVICE_INITIALIZATION_PRIORITY);
