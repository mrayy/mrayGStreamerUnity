#include "stdafx.h"
#include "rtp.h"

#ifdef _WIN32
#include <winsock.h>
#endif

#ifdef __ANDROID__
#include <sys/endian.h>
#endif


static unsigned int header_offset = (4 + 1 + 1 + 2 + 7 + 1 + 16 + 32 + 32) / sizeof(char);

typedef struct rtp_header
{
	uint16_t csrccount : 4;
	uint16_t extension : 1;
	uint16_t padding : 1;
	uint16_t version : 2;
	uint16_t type : 7;
	uint16_t markerbit : 1;
	uint16_t seq_number;
	uint32_t timestamp;
	uint32_t ssrc;
	uint32_t csrc[16];
} rtp_header;

uint16_t csrccount(void *src){
	rtp_header *rtp = (rtp_header*)src;
	return rtp->csrccount;
}

uint16_t extension(void *src){
	rtp_header *rtp = (rtp_header*)src;
	return rtp->extension;
}

void set_padding_bit(void *src, uint16_t padding){
	rtp_header *rtp = (rtp_header*)src;
	rtp->padding = padding;
}

uint16_t padding_bit(void *src){
	rtp_header *rtp = (rtp_header*)src;
	return rtp->padding;
}

uint16_t version(void *src){
	rtp_header *rtp = (rtp_header*)src;
	return rtp->version;
}

uint16_t type(void *src){
	rtp_header *rtp = (rtp_header*)src;
	return rtp->type;
}

uint16_t markerbit(void *src){
	rtp_header *rtp = (rtp_header*)src;
	return rtp->markerbit;
}

uint16_t seq_number(void *src){
	rtp_header *rtp = (rtp_header*)src;
	return rtp->seq_number;
}

uint16_t ssrc(void *src){
	rtp_header *rtp = (rtp_header*)src;
	return rtp->ssrc;
}

//bytes
unsigned short header_length(char* src){
	rtp_header *rtp = (rtp_header*)src;
	return (header_offset + sizeof(uint32_t)* rtp->csrccount / sizeof(char)) / 8;
}

void split_rtp(char* src, unsigned short length, char* header, char* payload){
	int header_len = header_length(src);
	memcpy(header, src, sizeof(char)* header_len);
	memcpy(payload, &src[header_len], sizeof(char)* (length - header_len));
}

uint32_t rtp_timestamp(void *src){
	rtp_header *rtp = (rtp_header*)src;
	uint32_t timestamp = rtp->timestamp;
	return ntohl(timestamp);
}

unsigned short rtp_padding_payload(unsigned char *src, int len, unsigned char *data){
	rtp_header *rtp = (rtp_header*)src;
	if (rtp->timestamp == 0) return -1;
	unsigned char padding_len = src[len - 1];
	//printf("padding len %d\n", padding_len);
	memcpy(data, &src[len - 1 - padding_len], padding_len * sizeof(char));
	return padding_len;
}