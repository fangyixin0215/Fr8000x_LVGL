#ifndef _APP_AT_H
#define _APP_AT_H

#include <stdint.h>

void app_at_cmd_recv_handler(uint8_t *data, uint16_t length);

void uart0_init(uint32_t baudrate);
void uart1_init(uint32_t baudrate);

#endif //_APP_AT_H

