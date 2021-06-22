#ifndef _CONNECTION_H_
#define _CONNECTION_H_

void connection_init();

uint8_t connection_ready();

uint32_t connection_sntp_get_timestamp();

#endif
