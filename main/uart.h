
#ifndef MAIN_NTP_H
#define MAIN_NTP_H

#define UART_PORT UART_NUM_0

void init_uart(void);

void read_uart_task(void *pvParameters);


#endif