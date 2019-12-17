//
// Created by linux on 12/3/19.
//

#ifndef USBHOTPLUG_LOCAL_IP_MAC_H
#define USBHOTPLUG_LOCAL_IP_MAC_H

int get_ip_by_domain(const char *domain, char *ip);  // 根据域名获取ip
int get_local_mac(const char *eth_inf, char *mac);   // 获取本机mac
int get_local_ip(const char *eth_inf, char *ip);     // 获取本机ip

#endif //USBHOTPLUG_LOCAL_IP_MAC_H
