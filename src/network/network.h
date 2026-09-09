/******************************************************************************
 * File Name:   network.h
 *
 * Description: Network initialization.
 *              Based on the udp server example.
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

#ifndef NETWORK_H_
#define NETWORK_H_

#include "cy_ecm.h"
#include "cy_ecm_error.h"

extern uint32_t db_get_network_ipaddr (void);
extern uint32_t db_get_network_netmask (void);
extern uint32_t db_get_network_gateway (void);
extern uint32_t db_get_network_nameserver (void);
extern bool db_get_network_dhcp (void);
extern const char * db_get_network_hostname (void);
extern void db_load (const char * filename);

/*******************************************************************************
 * Function Prototypes
 ********************************************************************************/

/**
 * Connect to the Ethernet network.
 * If the IP configuration is static, the static IP configuration defined
 * by the macros above is used.
 *
 * @param ip_config IP configuration type
 * @return CY_RSLT_SUCCESS if successful, error code otherwise
 */
cy_rslt_t connect_to_ethernet (void);

#endif /* NETWORK_H_ */

/* [] END OF FILE */
