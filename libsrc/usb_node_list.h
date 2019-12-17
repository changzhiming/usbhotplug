//
// Created by linux on 12/10/19.
//

#ifndef USBHOTPLUG_USB_NODE_LIST_H
#define USBHOTPLUG_USB_NODE_LIST_H


typedef struct usb_node{
    char id[32];
    struct usb_node *next;
} USB_Node;


void append_node(USB_Node *head, USB_Node *node);
USB_Node *malloc_usb_node(void);
void free_usb_node(USB_Node *head);
int usb_list_length(USB_Node *head);
void foreach_usb_node(USB_Node *head, void (*callback)(USB_Node *node));
USB_Node *get_last_usb_node(USB_Node *head);

#endif //USBHOTPLUG_USB_NODE_LIST_H
