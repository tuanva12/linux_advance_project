


#ifndef _DEFINE_H__
#define _DEFINE_H__

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <asm/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>

#define myprint(__myinfo__) std::cout << __myinfo__ << std::endl

#define APP_VERSION "1.0.0"

#define TYPE_CMD_VERSION "version"
#define TYPE_CMD_REBOOT "reboot"
#define TYPE_CMD_DOWNLOAD "download"
#define TYPE_CMD_VIEWCAM "view_cam"

typedef enum
{
    CMD_VERSION = 0,
    CMD_REBOOT,
    CMD_DOWNLOAD,
    CMD_VIEWCAM
}cmd_status_t;

typedef struct
{
    bool streamFlag;
    bool downloadFlag;
    bool rebootFlag;
}flag_struct_t;

#endif // _DEFINE_H__