/*
 * Copyright (c) 2018 Endre Karlson
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <init.h>
#include <soc.h>

static int particle_xenon_init(struct device *dev)
{
	ARG_UNUSED(dev);

	return 0;
}

SYS_INIT(particle_xenon_init, PRE_KERNEL_2,
	 CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);
