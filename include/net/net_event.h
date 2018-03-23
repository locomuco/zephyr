/*
 * Copyright (c) 2016 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Network Events code public header
 */

#ifndef __NET_EVENT_H__
#define __NET_EVENT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <net/net_ip.h>

/**
 * @addtogroup net_mgmt
 * @{
 */

/* Network Interface events */
#define _NET_IF_LAYER		NET_MGMT_LAYER_L1
#define _NET_IF_CORE_CODE	0x001
#define _NET_EVENT_IF_BASE	(NET_MGMT_EVENT_BIT |			\
				 NET_MGMT_IFACE_BIT |			\
				 NET_MGMT_LAYER(_NET_IF_LAYER) |	\
				 NET_MGMT_LAYER_CODE(_NET_IF_CORE_CODE))

enum net_event_if_cmd {
	NET_EVENT_IF_CMD_DOWN = 1,
	NET_EVENT_IF_CMD_UP,
};

#define NET_EVENT_IF_DOWN				\
	(_NET_EVENT_IF_BASE | NET_EVENT_IF_CMD_DOWN)

#define NET_EVENT_IF_UP					\
	(_NET_EVENT_IF_BASE | NET_EVENT_IF_CMD_UP)

/* IPv6 Events */
#define _NET_IPV6_LAYER		NET_MGMT_LAYER_L3
#define _NET_IPV6_CORE_CODE	0x060
#define _NET_EVENT_IPV6_BASE	(NET_MGMT_EVENT_BIT |			\
				 NET_MGMT_IFACE_BIT |			\
				 NET_MGMT_LAYER(_NET_IPV6_LAYER) |	\
				 NET_MGMT_LAYER_CODE(_NET_IPV6_CORE_CODE))

enum net_event_ipv6_cmd {
	NET_EVENT_IPV6_CMD_ADDR_ADD	= 1,
	NET_EVENT_IPV6_CMD_ADDR_DEL,
	NET_EVENT_IPV6_CMD_MADDR_ADD,
	NET_EVENT_IPV6_CMD_MADDR_DEL,
	NET_EVENT_IPV6_CMD_PREFIX_ADD,
	NET_EVENT_IPV6_CMD_PREFIX_DEL,
	NET_EVENT_IPV6_CMD_MCAST_JOIN,
	NET_EVENT_IPV6_CMD_MCAST_LEAVE,
	NET_EVENT_IPV6_CMD_ROUTER_ADD,
	NET_EVENT_IPV6_CMD_ROUTER_DEL,
	NET_EVENT_IPV6_CMD_ROUTE_ADD,
	NET_EVENT_IPV6_CMD_ROUTE_DEL,
	NET_EVENT_IPV6_CMD_DAD_SUCCEED,
	NET_EVENT_IPV6_CMD_DAD_FAILED,
	NET_EVENT_IPV6_CMD_NBR_ADD,
	NET_EVENT_IPV6_CMD_NBR_DEL,
};

#define NET_EVENT_IPV6_ADDR_ADD					\
	(_NET_EVENT_IPV6_BASE | NET_EVENT_IPV6_CMD_ADDR_ADD)

#define NET_EVENT_IPV6_ADDR_DEL					\
	(_NET_EVENT_IPV6_BASE |	NET_EVENT_IPV6_CMD_ADDR_DEL)

#define NET_EVENT_IPV6_MADDR_ADD				\
	(_NET_EVENT_IPV6_BASE |	NET_EVENT_IPV6_CMD_MADDR_ADD)

#define NET_EVENT_IPV6_MADDR_DEL				\
	(_NET_EVENT_IPV6_BASE | NET_EVENT_IPV6_CMD_MADDR_DEL)

#define NET_EVENT_IPV6_PREFIX_ADD				\
	(_NET_EVENT_IPV6_BASE |	NET_EVENT_IPV6_CMD_PREFIX_ADD)

#define NET_EVENT_IPV6_PREFIX_DEL				\
	(_NET_EVENT_IPV6_BASE |	NET_EVENT_IPV6_CMD_PREFIX_DEL)

#define NET_EVENT_IPV6_MCAST_JOIN				\
	(_NET_EVENT_IPV6_BASE |	NET_EVENT_IPV6_CMD_MCAST_JOIN)

#define NET_EVENT_IPV6_MCAST_LEAVE				\
	(_NET_EVENT_IPV6_BASE |	NET_EVENT_IPV6_CMD_MCAST_LEAVE)

#define NET_EVENT_IPV6_ROUTER_ADD				\
	(_NET_EVENT_IPV6_BASE | NET_EVENT_IPV6_CMD_ROUTER_ADD)

#define NET_EVENT_IPV6_ROUTER_DEL				\
	(_NET_EVENT_IPV6_BASE | NET_EVENT_IPV6_CMD_ROUTER_DEL)

#define NET_EVENT_IPV6_ROUTE_ADD				\
	(_NET_EVENT_IPV6_BASE | NET_EVENT_IPV6_CMD_ROUTE_ADD)

#define NET_EVENT_IPV6_ROUTE_DEL				\
	(_NET_EVENT_IPV6_BASE | NET_EVENT_IPV6_CMD_ROUTE_DEL)

#define NET_EVENT_IPV6_DAD_SUCCEED				\
	(_NET_EVENT_IPV6_BASE | NET_EVENT_IPV6_CMD_DAD_SUCCEED)

#define NET_EVENT_IPV6_DAD_FAILED				\
	(_NET_EVENT_IPV6_BASE | NET_EVENT_IPV6_CMD_DAD_FAILED)

#define NET_EVENT_IPV6_NBR_ADD					\
	(_NET_EVENT_IPV6_BASE | NET_EVENT_IPV6_CMD_NBR_ADD)

#define NET_EVENT_IPV6_NBR_DEL					\
	(_NET_EVENT_IPV6_BASE | NET_EVENT_IPV6_CMD_NBR_DEL)

/* IPv4 Events*/
#define _NET_IPV4_LAYER		NET_MGMT_LAYER_L3
#define _NET_IPV4_CORE_CODE	0x004
#define _NET_EVENT_IPV4_BASE	(NET_MGMT_EVENT_BIT |			\
				 NET_MGMT_IFACE_BIT |			\
				 NET_MGMT_LAYER(_NET_IPV4_LAYER) |	\
				 NET_MGMT_LAYER_CODE(_NET_IPV4_CORE_CODE))

enum net_event_ipv4_cmd {
	NET_EVENT_IPV4_CMD_ADDR_ADD	= 1,
	NET_EVENT_IPV4_CMD_ADDR_DEL,
	NET_EVENT_IPV4_CMD_ROUTER_ADD,
};

#define NET_EVENT_IPV4_ADDR_ADD					\
	(_NET_EVENT_IPV4_BASE | NET_EVENT_IPV4_CMD_ADDR_ADD)

#define NET_EVENT_IPV4_ADDR_DEL					\
	(_NET_EVENT_IPV4_BASE |	NET_EVENT_IPV4_CMD_ADDR_DEL)

#define NET_EVENT_IPV4_ROUTER_ADD				\
	(_NET_EVENT_IPV4_BASE |	NET_EVENT_IPV4_CMD_ROUTER_ADD)

#ifdef CONFIG_NET_MGMT_EVENT_INFO
/**
 * @brief Network Management event information structure
 * Used to pass information on network events like
 *   NET_EVENT_IPV6_ADDR_ADD,
 *   NET_EVENT_IPV6_ADDR_DEL,
 *   NET_EVENT_IPV6_MADDR_ADD and
 *   NET_EVENT_IPV6_MADDR_DEL
 * when CONFIG_NET_MGMT_EVENT_INFO enabled and event generator pass the
 * information.
 */
struct net_event_ipv6_addr {
	struct in6_addr addr;
};

/**
 * @brief Network Management event information structure
 * Used to pass information on network events like
 *   NET_EVENT_IPV6_NBR_ADD and
 *   NET_EVENT_IPV6_NBR_DEL
 * when CONFIG_NET_MGMT_EVENT_INFO enabled and event generator pass the
 * information.
 * @Note: idx will be '-1' in case of NET_EVENT_IPV6_NBR_DEL event.
 */
struct net_event_ipv6_nbr {
	struct in6_addr addr;
	int idx; /* NBR index*/
};

/**
 * @brief Network Management event information structure
 * Used to pass information on network events like
 *   NET_EVENT_IPV6_ROUTE_ADD and
 *   NET_EVENT_IPV6_ROUTE_DEL
 * when CONFIG_NET_MGMT_EVENT_INFO enabled and event generator pass the
 * information.
 */
struct net_event_ipv6_route {
	struct in6_addr nexthop;
	struct in6_addr addr; /* addr/prefix */
	u8_t prefix_len;
};

/* Maximum size of "struct net_event_ipv6_addr" or
 * "struct net_event_ipv6_nbr" or "struct net_event_ipv6_route".
 * NOTE: Update comments here and calculate which struct occupies max size.
 */
#define NET_EVENT_INFO_MAX_SIZE sizeof(struct net_event_ipv6_route)

#endif

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* __NET_EVENT_H__ */
