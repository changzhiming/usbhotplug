//
// Created by linux on 12/10/19.
//

#include "switch_to_json.h"
#include "global.h"

#include "local_ip_mac.h"
#include <time.h>
#include "json/json.h"

extern char name[32];

static void add_remove_usb_json(Package_Type pack_type, USB_Node *head, char *data, int data_len)
{
    struct json_object *array_object = json_object_new_array();
    struct json_object *usb_object = json_object_new_object();
    assert(array_object != NULL);
    assert(usb_object != NULL);

    if(head != NULL) {
        while (head->next != NULL) {
            json_object_array_add(array_object, json_object_new_string(head->id));
            head = head->next;
        }
        json_object_array_add(array_object, json_object_new_string(head->id));
    }

    char mac[64] = {0};
    get_local_mac("eth0", mac);

    char *action = NULL;
    switch (pack_type) {
        case ADDUSB:
            action = "INSERTION";
            break;
        case REMOVEUSB:
            action = "EXTRACT";
            break;
        case REGISTER:
            action = "REG";
            break;
        case HEARTBEAT:
            action = "HOST_HB";
            break;
        default:
            action = "INSERTION";
            break;
    }

    json_object_object_add(usb_object, "deviceType", json_object_new_string(name));
    json_object_object_add(usb_object, "mac", json_object_new_string(mac));
    json_object_object_add(usb_object, "action", json_object_new_string(action));
    json_object_object_add(usb_object, "data", array_object);
    json_object_object_add(usb_object, "timestamp", json_object_new_int64(time(NULL)));

    snprintf(data, data_len, "%s", json_object_to_json_string(usb_object));

    json_object_put(usb_object);
}

void add_usb_json(USB_Node *head, char *data, int data_len)
{
    add_remove_usb_json(ADDUSB, head, data, data_len);
}

void remove_usb_json(USB_Node *head, char *data, int data_len)
{
    add_remove_usb_json(REMOVEUSB, head, data, data_len);
}

void register_usb_json(USB_Node *head, char *data, int data_len)
{
    add_remove_usb_json(REGISTER, head, data, data_len);
}

void heartbeat_usb_json(char *data, int data_len)
{
    add_remove_usb_json(HEARTBEAT, NULL, data, data_len);
}

