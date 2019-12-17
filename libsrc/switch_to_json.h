//
// Created by linux on 12/10/19.
//

#ifndef USBHOTPLUG_SWITCH_TO_JSON_H
#define USBHOTPLUG_SWITCH_TO_JSON_H

#include "usb_node_list.h"

typedef enum {
    REGISTER = 0,
    ADDUSB,
    REMOVEUSB,
    HEARTBEAT,
} Package_Type;

void add_usb_json(USB_Node *head, char *data, int data_len);
void remove_usb_json(USB_Node *head, char *data, int data_len);
void register_usb_json(USB_Node *head, char *data, int data_len);
void heartbeat_usb_json(char *data, int data_len);

#endif //USBHOTPLUG_SWITCH_TO_JSON_H
