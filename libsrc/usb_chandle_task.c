//
// Created by linux on 12/3/19.
//

#include "usb_chandle_task.h"
#include "global.h"
#include <stdlib.h>
#include <linux/netlink.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "usb_node_list.h"
#include "switch_to_json.h"
#include <regex.h>
#include <sys/time.h>
#include <signal.h>

#define USBHOTBUF 1024
#define USB_REV_BUF 1024

extern char port[6];
extern char http[32];

typedef struct {
    char *data;
    int len;
} regexp_ret;

static void _system(const char *cmd, char *ret, int ret_len)
{
    FILE *fp = popen(cmd, "r");
    fread(ret, ret_len, 1, fp);
    pclose(fp);
}

static bool post_usb_list(const char *post_data)
{
    static char send_data[1024] = {0};
    static char ret_data[10] = {0};
    memset(send_data, 0, sizeof(send_data));
    memset(ret_data, 0, sizeof(ret_data));

    clock_t start = clock();

    bool ret_result = false;
    snprintf(send_data, sizeof(send_data), "curl -i -sl -w \"%{http_code}\" -H \"Content-type: application/json\" -X POST -d '%s' -o /dev/null  https://mqtt.huikezk.com/login", post_data);

    _system(send_data, ret_data, sizeof(ret_data));
    if(strstr(ret_data, "200") != NULL) {
        ret_result = true;
    }

    clock_t stop = clock();
    printf("time%f\n", (double)(stop - start) / CLOCKS_PER_SEC );

    return ret_result;
}

static regexp_ret regexp_parse(const char *data, const char *pattern)
{
    regexp_ret ret_data= {.data = NULL, .len = 0};

    regex_t reg;
    int ret = regcomp(&reg, pattern, REG_EXTENDED);
    if(ret != 0) {
        goto clear;
    }

    regmatch_t pmatch = {0};

    ret = regexec(&reg, data, 1, &pmatch, 0);
    if(ret == 0 && pmatch.rm_so != -1) {
        ret_data.data = (char *)(data + pmatch.rm_so);
        ret_data.len = pmatch.rm_eo - pmatch.rm_so;
    }
 clear:
    regfree(&reg);
    return ret_data;
}

static USB_Node *g_head = NULL;

static void get_usb_list(const char *cmd)
{
    static char usb_rev_buf[USB_REV_BUF] = {0};
    memset(usb_rev_buf, 0, sizeof(usb_rev_buf)); //clear 0

    struct sockaddr_in server, client;
    memset(&client, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(port)); //12369
    server.sin_addr.s_addr = inet_addr("192.168.140.7");

    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if(socketfd < 0) {
        goto clear;
    }
    if(connect(socketfd, (struct sockaddr*)&server, sizeof(server)) == -1) {
        goto clear;
    }

    struct timeval timeout={3, 0}; //3s timout
    if(setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO,(char*)&timeout, sizeof(struct timeval)) < 0) {
        goto clear;
    }

    if(send(socketfd, cmd, strlen(cmd), 0) < 0) {
        goto clear;
    }

    ssize_t rev_size = recv(socketfd, usb_rev_buf, sizeof(usb_rev_buf), 0);
    if(rev_size <= 0) {
        goto clear;
    }
    //usb parse
    char *parse_data = usb_rev_buf;
    int ret_index = 0;
    regexp_ret ret_data = {0};

    USB_Node *head = malloc_usb_node();
    while(true) {
        ret_data = regexp_parse(parse_data, "<jqbh>[0-9]+");
        if(ret_data.len == 0) {
            break;
        }
        parse_data = ret_data.data + ret_data.len;
        if(ret_data.len < 32 && ret_data.len > 6) {
            if(strlen(head->id) == 0) {
                memcpy(head->id, ret_data.data + 6, ret_data.len - 6);
            } else {
                USB_Node *node = malloc_usb_node();
                append_node(head, node);
                memcpy(node->id, ret_data.data + 6, ret_data.len - 6);
            }
        }
    }

    static char json_data[1024] = {0};
    memset(json_data, 0, sizeof(json_data));

    if(g_head != NULL) {
        if(usb_list_length(g_head) > usb_list_length(head)) {
            remove_usb_json(get_last_usb_node(head), json_data, sizeof(json_data));
        } else {
            add_usb_json(get_last_usb_node(g_head), json_data, sizeof(json_data));
        }

        free_usb_node(g_head);
    } else {
        register_usb_json(head, json_data, sizeof(json_data));
    }

    post_usb_list(json_data);
    printf("%s\n", json_data);
    g_head = head;

clear:
    close(socketfd);
}

static int init_hotplug_socket(void)
{
    struct sockaddr_nl snl = {
            .nl_family = AF_NETLINK,
            .nl_pid = getpid(),
            .nl_groups = 1,
    };

    int hotplug_socket = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
    if(hotplug_socket <= 0) {
        syslog(LOG_ERR, "hotplug socket create fail\n");
        return -1;
    }

    int buffersize = 10 * 1024 * 1024;
    setsockopt(hotplug_socket, SOL_SOCKET, SO_RCVBUFFORCE, &buffersize, sizeof(buffersize));

    int retval = bind(hotplug_socket, (struct sockaddr *)&snl, sizeof(struct sockaddr_nl));
    if(retval < 0) {
        syslog(LOG_ERR, "hotplug socket bind fail\n");
        close(hotplug_socket);
        return -1;
    }
    return hotplug_socket;
}

void set_timer()
{
    static struct itimerval oldtv;
    struct itimerval itv;
    itv.it_interval.tv_sec = 10;
    itv.it_interval.tv_usec = 0;
    itv.it_value.tv_sec = 5;
    itv.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &itv, &oldtv);
}

void heartbeat_callback(int m)
{
    static char json_data[256] = {0};

    heartbeat_usb_json(json_data, sizeof(json_data));
    post_usb_list(json_data);
}

void *usb_hot_handle(void *arg)
{
    static char hot_rev_buf[USBHOTBUF] = {0};

    int hot_socket = init_hotplug_socket();
    if(hot_socket < 0) { goto clear; }
    //heartbeat
    signal(SIGALRM, heartbeat_callback);
    set_timer();

    while (true) {
        memset(hot_rev_buf, sizeof(hot_rev_buf), 0);
        ssize_t rev_size = recv(hot_socket, hot_rev_buf, sizeof(hot_rev_buf), 0);
        if(rev_size <= 0) {
            close(hot_socket);
            goto clear;
        }
        regexp_ret ret_data = regexp_parse(hot_rev_buf, "^(add|remove).*/block/");

        if(ret_data.len > 0) {
            get_usb_list("acl::string strRequest_10001 = \""
                         "<?xml version=\"1.0\" encoding=\"gbk\"?>"
                         "<business id=\"10001\" comment=\"获取服务器基本信息\">"
                         "  <body yylxdm=\"1\">"
                         "    <fwqdz>127.0.0.1</fwqdz>"
                         "    <fwqdkh>12368</fwqdkh>"
                         "    <jqbh></jqbh>"
                         "    <fpcbh>0</fpcbh>"
                         "  </body>"
                         "</business>"
                         "\";");
        }
    }

clear:
    pthread_exit(0);
}

bool init_usb_thread(pthread_t *usb_thread)
{
    assert(usb_thread != NULL);
    return (pthread_create(usb_thread, NULL, usb_hot_handle, NULL) == 0);
}
