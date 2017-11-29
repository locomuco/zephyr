/*
 * Copyright (c) 2016 Matthias Boesl
 * Copyright (c) 2017 Linaro Limited.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __INC_BOARD_H
#define __INC_BOARD_H

#include <soc.h>

/* Onboard LED */
#define LED0_GPIO_PORT  "GPIOB"
#define LED0_GPIO_PIN   0

#define LED1_GPIO_PORT  "GPIOB"
#define LED1_GPIO_PIN   1

#define LED2_GPIO_PORT  "GPIOB"
#define LED2_GPIO_PIN   5

#define LED3_GPIO_PORT  "GPIOB"
#define LED3_GPIO_PIN   8

#define ENC_RST_GPIO_PORT  "GPIOC"
#define ENC_RST_GPIO_PIN   14

#define ENC_CS_GPIO_PORT  "GPIOB"
#define ENC_CS_GPIO_PIN   12


#endif /* __INC_BOARD_H */
