/******************************************************************************
 * File Name:   network.c
 *
 * Description: Ethernet initialization.
 *
 ********************************************************************************
 * Copyright 2022, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 *******************************************************************************/

/* The Infineon Ethernet Connection Manager does not work well for a
 * target that needs runtime configurable IP settings. It also does
 * not work well for a link that could go up and down at runtime.
 *
 * By using the lower-level Network Interface integration module
 * instead, we can support link changes as we would normally
 * expect. We still use Ethernet Connection Manager for bringing up
 * the Ethernet driver, and that unfortunately requires an established
 * link to complete.
 */

#include "network.h"
#include "shell.h"
#include "cycfg.h"

/* Ethernet connection manager header files */
#include "cy_ecm.h"

/* Network interface integration header files */
#include "cy_network_mw_core.h"

/* Ethernet PHY driver header files */
#include "cy_eth_phy_driver.h"

#include "lwip/netif.h"
#include "lwip/dns.h"

/* Standard C header files */
#include <inttypes.h>
#include <stdio.h>

/* Ethernet interface ID */
#ifdef XMC7100D_F176K4160
#define INTERFACE_ID CY_ECM_INTERFACE_ETH0
#else
#define INTERFACE_ID CY_ECM_INTERFACE_ETH1
#endif

static cy_ecm_phy_callbacks_t phy_callbacks = {
   .phy_init = cy_eth_phy_init,
   .phy_configure = cy_eth_phy_configure,
   .phy_enable_ext_reg = cy_eth_phy_enable_ext_reg,
   .phy_discover = cy_eth_phy_discover,
   .phy_get_auto_neg_status = cy_eth_phy_get_auto_neg_status,
   .phy_get_link_partner_cap = cy_eth_phy_get_link_partner_cap,
   .phy_get_linkspeed = cy_eth_phy_get_linkspeed,
   .phy_get_linkstatus = cy_eth_phy_get_linkstatus,
   .phy_reset = cy_eth_phy_reset};

static cy_ecm_t ecm_handle = NULL;
static cy_network_interface_context *iface;

static void ecm_link_cb (
   cy_ecm_event_t event,
   cy_ecm_event_data_t * event_data)
{
   switch (event)
   {
   case CY_ECM_EVENT_CONNECTED:
      printf ("Ethernet connected.\n");
      cy_network_ip_up (iface);
      break;
   case CY_ECM_EVENT_DISCONNECTED:
      printf ("Ethernet disconnected.\n");
      cy_network_ip_down (iface);
      break;
   default:
      break;
   }
}

static void iface_status_cb (cy_network_interface_context *iface, void *user_data)
{
}

cy_rslt_t connect_to_ethernet (void)
{
   cy_rslt_t result = CY_RSLT_SUCCESS;
   uint8_t mac_address[6] = { 0 };
   cy_network_static_ip_addr_t ip_addr;

   /* Initialize ethernet connection manager. */
   result = cy_ecm_init();
   if (result != CY_RSLT_SUCCESS)
   {
      printf (
         "Ethernet connection manager initialization failed! Error code: "
         "0x%08" PRIx32 "\n",
         (uint32_t)result);
      CY_ASSERT (0);
   }
   else
   {
      printf ("Ethernet connection manager initialized.\n");
   }

   /* Initialize the Ethernet Interface and PHY driver */
   result = cy_ecm_ethif_init (INTERFACE_ID, &phy_callbacks, &ecm_handle);
   if (result != CY_RSLT_SUCCESS)
   {
      printf (
         "Ethernet interface initialization failed! Error code: 0x%08" PRIx32
         "\n",
         (uint32_t)result);

      CY_ASSERT (0);
   }

   if (INTERFACE_ID == CY_ECM_INTERFACE_ETH0)
   {
#if (defined (eth_0_ENABLED) && (eth_0_ENABLED == 1u))
      mac_address[0] = (uint8_t)eth_0_MAC_ADDR0;
      mac_address[1] = (uint8_t)eth_0_MAC_ADDR1;
      mac_address[2] = (uint8_t)eth_0_MAC_ADDR2;
      mac_address[3] = (uint8_t)eth_0_MAC_ADDR3;
      mac_address[4] = (uint8_t)eth_0_MAC_ADDR4;
      mac_address[5] = (uint8_t)eth_0_MAC_ADDR5;
#endif
   }
   else
   {
#if (defined (eth_1_ENABLED) && (eth_1_ENABLED == 1u))
      mac_address[0] = (uint8_t)eth_1_MAC_ADDR0;
      mac_address[1] = (uint8_t)eth_1_MAC_ADDR1;
      mac_address[2] = (uint8_t)eth_1_MAC_ADDR2;
      mac_address[3] = (uint8_t)eth_1_MAC_ADDR3;
      mac_address[4] = (uint8_t)eth_1_MAC_ADDR4;
      mac_address[5] = (uint8_t)eth_1_MAC_ADDR5;
#endif
   }

   ip_addr.addr.version = CY_ECM_IP_VER_V4;
   ip_addr.addr.ip.v4 = db_get_network_ipaddr();
   ip_addr.gateway.version = CY_ECM_IP_VER_V4;
   ip_addr.gateway.ip.v4 = db_get_network_gateway();
   ip_addr.netmask.version = CY_ECM_IP_VER_V4;
   ip_addr.netmask.ip.v4 = db_get_network_netmask();

   cy_network_add_nw_interface (
      CY_NETWORK_ETH_INTERFACE,
      INTERFACE_ID,
      (INTERFACE_ID == CY_ECM_INTERFACE_ETH1) ? ETH1 : ETH0,
      mac_address,
      db_get_network_dhcp() ? NULL : &ip_addr,
      &iface
   );

#if LWIP_NETIF_HOSTNAME
   struct netif * netif = (struct netif *)iface->nw_interface;
   netif->hostname = db_get_network_hostname();
#endif

#if LWIP_DNS
   ip_addr_t nameserver = IPADDR4_INIT (db_get_network_nameserver());
   dns_setserver (0, &nameserver);
#endif

   /* Register to receive netif status changes  */
   cy_network_register_ip_change_cb (iface, iface_status_cb, NULL);

   /* Register to receive link changes  */
   cy_ecm_register_event_callback (ecm_handle, ecm_link_cb);

   return result;
}
