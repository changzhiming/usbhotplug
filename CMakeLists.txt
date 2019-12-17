cmake_minimum_required(VERSION 3.15)
project(usbhotplug C)

set(CMAKE_C_STANDARD 11)

set(INC_DIR ./libsrc/json)
set(LINK_DIR /lib/x86_64-linux-gnu)

include_directories(${INC_DIR})
link_directories(${LINK_DIR})
link_libraries(pthread json)

add_executable(usbhotplug main.c libsrc/daemon.c libsrc/daemon.h libsrc/singleton.c libsrc/singleton.h libsrc/usb_chandle_task.c libsrc/usb_chandle_task.h libsrc/global.h libsrc/local_ip_mac.c libsrc/local_ip_mac.h libsrc/usb_node_list.c libsrc/usb_node_list.h libsrc/switch_to_json.c libsrc/switch_to_json.h)

target_link_libraries(usbhotplug pthread json)