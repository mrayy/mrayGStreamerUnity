#ifndef __RTP__
#define __RTP__
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

extern "C" uint32_t rtp_timestamp(void *src);
extern "C" unsigned short rtp_padding_payload(unsigned char *src, int len, unsigned char *data);

#endif
