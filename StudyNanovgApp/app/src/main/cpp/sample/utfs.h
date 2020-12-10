//
// Created by Administrator on 2020/12/10 0010.
//

#ifndef STUDYNANOVGAPP_UTFS_H
#define STUDYNANOVGAPP_UTFS_H

#include "platform.h"

EXTERN_C_ENTER

unsigned int Utf8ToUcs2(unsigned char *utf8, unsigned int utf8_size,
                        unsigned short *ucs2, unsigned int ucs2_size);

EXTERN_C_EXIT

#endif //STUDYNANOVGAPP_UTFS_H
