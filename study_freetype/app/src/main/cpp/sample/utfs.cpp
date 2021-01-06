//
// Created by Administrator on 2020/12/10 0010.
//

#include "utfs.h"

// Convert Unicode big endian to Unicode little endian
unsigned Ucs2BeToUcs2Le(unsigned short *ucs2bige, unsigned int size)
{
    //LOGD("%s %d\n", __FUNCTION__, __LINE__);

    if (!ucs2bige) {
        return 0;
    }

    unsigned int length = size;
    unsigned short *tmp = ucs2bige;

    while (*tmp && length) {

        length--;
        unsigned char val_high = *tmp >> 8;
        unsigned char val_low = (unsigned char)*tmp;

        *tmp = val_low << 8 | val_high;

        tmp++;
    }

    return size - length;
}

// Convert Ucs-2 to Utf-8
unsigned int Ucs2ToUtf8(unsigned short *ucs2, unsigned int ucs2_size,
                        unsigned char *utf8, unsigned int utf8_size)
{
    unsigned int length = 0;

    if (!ucs2) {
        return 0;
    }

    unsigned short *inbuf = ucs2;
    unsigned char *outbuf = utf8;

    if (*inbuf == 0xFFFE) {
        Ucs2BeToUcs2Le(inbuf, ucs2_size);
    }

    if (!utf8) {
        unsigned int insize = ucs2_size;

        while (*inbuf && insize) {
            insize--;

/*            if (*inbuf == 0xFEFF) {
                inbuf++;
                continue;
            }*/

            if (0x0080 > *inbuf) {
                length++;
            } else if (0x0800 > *inbuf) {
                length += 2;
            } else {
                length += 3;
            }

            inbuf++;
        }
        return length;

    } else {
        unsigned int insize = ucs2_size;

        while (*inbuf && insize && length < utf8_size) {
            insize--;

            if (*inbuf == 0xFFFE) {
                inbuf++;
                continue;
            }

            if (0x0080 > *inbuf) {
                /* 1 byte UTF-8 Character.*/
                *outbuf++ = (unsigned char)(*inbuf);
                length++;
            } else if (0x0800 > *inbuf) {
                /*2 bytes UTF-8 Character.*/
                *outbuf++ = 0xc0 | ((unsigned char)(*inbuf >> 6));
                *outbuf++ = 0x80 | ((unsigned char)(*inbuf & 0x3F));
                length += 2;

            } else {
                /* 3 bytes UTF-8 Character .*/
                *outbuf++ = 0xE0 | ((unsigned char)(*inbuf >> 12));
                *outbuf++ = 0x80 | ((unsigned char)((*inbuf >> 6) & 0x3F));
                *outbuf++ = 0x80 | ((unsigned char)(*inbuf & 0x3F));
                length += 3;
            }

            inbuf++;
        }

        return length;
    }
}

// Convert Utf-8 to Ucs-2
unsigned int Utf8ToUcs2(unsigned char *utf8, unsigned int utf8_size,
                        unsigned short *ucs2, unsigned int ucs2_size)
{
    int length = 0;
    unsigned int insize = utf8_size;
    unsigned char *inbuf = utf8;

    if(!utf8)
        return 0;

    if(!ucs2) {
        while(*inbuf && insize) {
            unsigned char c = *inbuf;
            if((c & 0x80) == 0) {
                length += 1;
                insize -= 1;
                inbuf++;
            }
            else if((c & 0xE0) == 0xC0) {
                length += 1;
                insize -= 2;
                inbuf += 2;
            } else if((c & 0xF0) == 0xE0) {
                length += 1;
                insize -= 3;
                inbuf += 3;
            }
        }
        return length;

    } else {
        unsigned short *outbuf = ucs2;
        unsigned int outsize = ucs2_size;

        while(*inbuf && insize && length < outsize) {
            unsigned char c = *inbuf;
            if((c & 0x80) == 0) {
                *outbuf++ = c;
                inbuf++;
                length++;
                insize--;
            } else if((c & 0xE0) == 0xC0) {
                unsigned short val;

                val = (c & 0x3F) << 6;
                inbuf++;
                c = *inbuf;
                val |= (c & 0x3F);
                inbuf++;

                length++;
                insize -= 2;

                *outbuf++ = val;
            } else if((c & 0xF0) == 0xE0) {
                unsigned short val;

                val = (c & 0x1F) << 12;
                inbuf++;
                c = *inbuf;
                val |= (c & 0x3F) << 6;
                inbuf++;
                c = *inbuf;
                val |= (c & 0x3F);
                inbuf++;

                insize -= 3;
                length++;

                *outbuf++ = val;
            }
        }
        return length;
    }
    return 0;
}
