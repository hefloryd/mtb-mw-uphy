/*********************************************************************
 *        _       _         _
 *  _ __ | |_  _ | |  __ _ | |__   ___
 * | '__|| __|(_)| | / _` || '_ \ / __|
 * | |   | |_  _ | || (_| || |_) |\__ \
 * |_|    \__|(_)|_| \__,_||_.__/ |___/
 *
 * http://www.rt-labs.com
 * Copyright 2023 rt-labs AB, Sweden.
 * See LICENSE file in the project root for full license information.
 ********************************************************************/

#include <shell.h>

#include <lwip/dns.h>
#include <lwip/netif.h>

#include <stdio.h>

static int _cmd_ip_show (int argc, char * argv[])
{
   struct netif * netif;
#if LWIP_DNS
   const ip_addr_t * dns = dns_getserver (0);
#endif

   if (argc > 1)
   {
      shell_usage (argv[0], "wrong number of arguments");
      return -1;
   }

   NETIF_FOREACH (netif)
   {
      /* Do not show lo0 interface */
      if (netif->name[0] == 'l' && netif->name[1] == 'o' && netif->num == 0)
      {
         continue;
      }

      printf ("Interface:     %c%c%u\n", netif->name[0], netif->name[1], netif->num);
      printf (
         " MAC address:  %02X:%02X:%02X:%02X:%02X:%02X\n",
         netif->hwaddr[0],
         netif->hwaddr[1],
         netif->hwaddr[2],
         netif->hwaddr[3],
         netif->hwaddr[4],
         netif->hwaddr[5]);
      printf (" Up:           %s\n", netif_is_up (netif) ? "True" : "False");
      printf (" Link:         %s\n", netif_is_link_up (netif) ? "True" : "False");
      printf (
         " IP address:   %u.%u.%u.%u\n",
         ip4_addr1 (&netif->ip_addr),
         ip4_addr2 (&netif->ip_addr),
         ip4_addr3 (&netif->ip_addr),
         ip4_addr4 (&netif->ip_addr));
      printf (
         " Netmask:      %u.%u.%u.%u\n",
         ip4_addr1 (&netif->netmask),
         ip4_addr2 (&netif->netmask),
         ip4_addr3 (&netif->netmask),
         ip4_addr4 (&netif->netmask));
      printf (
         " Gateway:      %u.%u.%u.%u\n",
         ip4_addr1 (&netif->gw),
         ip4_addr2 (&netif->gw),
         ip4_addr3 (&netif->gw),
         ip4_addr4 (&netif->gw));
#if LWIP_NETIF_HOSTNAME
      printf (" Hostname:     %s\n", netif->hostname ? netif->hostname : "");
#endif
   }

#if LWIP_DNS
   printf (
      "DNS server:    %u.%u.%u.%u\n",
      ip4_addr1 (dns),
      ip4_addr2 (dns),
      ip4_addr3 (dns),
      ip4_addr4 (dns));
#endif

   return 0;
}

const shell_cmd_t cmd_ip_show = {
   .cmd = _cmd_ip_show,
   .name = "ip_show",
   .help_short = "show IP address and netmask",
   .help_long = "ip_show\n"
                "\n"
                "Show the current IP address and netmask.\n"};

static int _cmd_lwip_stats (int argc, char *argv[])
{
   (void)argc;
   (void)argv;

   stats_display();
   return 0;
}

const shell_cmd_t cmd_lwip_stats =
{
   .cmd = _cmd_lwip_stats,
   .name = "lwip_stats",
   .help_short = "display lwip stats",
   .help_long =
   "lwip_stat\n"
   "\n"
   "List statistics from lwip\n"
};

SHELL_CMD (cmd_ip_show);
SHELL_CMD (cmd_lwip_stats);
