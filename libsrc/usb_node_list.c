//
// Created by linux on 12/10/19.
//

#include "usb_node_list.h"
#include "global.h"

void append_node(USB_Node *head, USB_Node *node)
{
    assert(head != NULL);
    assert(node != NULL);

    while(head->next != NULL) {
        head = head->next;
    }
    head->next = node;
}

USB_Node *malloc_usb_node(void)
{
    USB_Node *usb_node = (USB_Node *)malloc(sizeof(USB_Node));
    if(usb_node != NULL) {
        memset(usb_node, 0, sizeof(USB_Node));
    }
    return usb_node;
}

void free_usb_node(USB_Node *head)
{
    assert(head != NULL);

    USB_Node *node = head;
    while(head->next != NULL) {
        node = head;
        head = head->next;

        free(node);
    }
    free(head);
}

int usb_list_length(USB_Node *head)
{
    int length = 1;

    while(head->next != NULL) {
        head = head->next;
        length++;
    }
    return length;
}

USB_Node *get_last_usb_node(USB_Node *head)
{
    while(head->next != NULL) {
        head = head->next;
    }
    return head;
}

void foreach_usb_node(USB_Node *head, void (*callback)(USB_Node *node))
{
    while(head->next != NULL) {
        callback(head);
        head = head->next;
    }
    callback(head);
}












