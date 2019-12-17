//
// Created by linux on 12/3/19.
//

#ifndef USBHOTPLUG_USB_CHANDLE_TASK_H
#define USBHOTPLUG_USB_CHANDLE_TASK_H

#include <stdbool.h>
#include <pthread.h>

bool init_usb_thread(pthread_t *usb_thread);

#endif //USBHOTPLUG_USB_CHANDLE_TASK_H
