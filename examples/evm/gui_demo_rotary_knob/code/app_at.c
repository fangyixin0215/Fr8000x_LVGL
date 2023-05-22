#include <stdint.h>
#include "string.h"

#include "gap_api.h"
#include "user_task.h"
#include "os_msg_q.h"
#include "os_mem.h"
#include "os_timer.h"
#include "co_printf.h"

#include "driver_uart_ex.h"
#include "driver_uart.h"
#include "plf.h"
#include "driver_system.h"
#include "driver_pmu.h"

#define AT_RECV_MAX_LEN             32

uint8_t app_at_recv_char;
uint8_t at_recv_buffer[AT_RECV_MAX_LEN];
uint8_t at_recv_index = 0;
uint8_t at_recv_state = 0;

static void app_at_recv_c(uint8_t c);

static UART_HandleTypeDef Uart0_handle;
static UART_HandleTypeDef Uart1_handle;
void uart_isr_int(UART_HandleTypeDef *huart)
{
	huart->UARTx->IER_DLH.IER.ELSI = 1;
	huart->UARTx->IER_DLH.IER.ERBFI = 1;
}

void uart0_init(uint32_t baudrate)
{
 
		__SYSTEM_UART0_CLK_SELECT_96M();
		__SYSTEM_UART0_CLK_ENABLE();
		system_set_port_pull(GPIO_PA0,GPIO_PULL_UP,true);
    system_set_port_mux(GPIO_PORT_A, GPIO_BIT_0, PORTA0_FUNC_UART0_RXD);
    system_set_port_mux(GPIO_PORT_A, GPIO_BIT_1, PORTA1_FUNC_UART0_TXD);
		Uart0_handle.UARTx = Uart0;
    Uart0_handle.Init.BaudRate   = baudrate;
    Uart0_handle.Init.DataLength = UART_DATA_LENGTH_8BIT;
    Uart0_handle.Init.StopBits   = UART_STOPBITS_1 ;
    Uart0_handle.Init.Parity     = UART_PARITY_NONE;
    Uart0_handle.Init.FIFO_Mode  = UART_FIFO_ENABLE;
		uart_init_ex(&Uart0_handle);
		NVIC_SetPriority(UART0_IRQn, 5);
		NVIC_EnableIRQ(UART0_IRQn);
		/*enable recv and line status interrupt*/
		uart_isr_int(&Uart0_handle);
}

void uart1_init(uint32_t baudrate)
{	
		__SYSTEM_UART1_CLK_ENABLE();
		system_set_port_pull(GPIO_PA2,GPIO_PULL_UP,true);
    /* set PA2 and PA3 for AT command interface */
    system_set_port_mux(GPIO_PORT_A, GPIO_BIT_2, PORTA2_FUNC_UART1_RXD);
    system_set_port_mux(GPIO_PORT_A, GPIO_BIT_3, PORTA3_FUNC_UART1_TXD);
    Uart1_handle.UARTx = Uart1;
    Uart1_handle.Init.BaudRate   = baudrate;
    Uart1_handle.Init.DataLength = UART_DATA_LENGTH_8BIT;
    Uart1_handle.Init.StopBits   = UART_STOPBITS_1;
    Uart1_handle.Init.Parity     = UART_PARITY_NONE;
    Uart1_handle.Init.FIFO_Mode  = UART_FIFO_ENABLE;

    uart_init_ex(&Uart1_handle);
	
		NVIC_EnableIRQ(UART1_IRQn);
    NVIC_SetPriority(UART1_IRQn, 5);
		/*enable recv and line status interrupt*/
		uart_isr_int(&Uart1_handle);

}


__attribute__((section("ram_code"))) void uart0_isr(void)
{
	uint32_t isr_id;
	uint8_t res=0;
	volatile struct_UART_t * const uart_reg_ram = (volatile struct_UART_t *)UART0_BASE;
	isr_id = uart_reg_ram->FCR_IID.IID;
	if(((isr_id & 0x04) == 0x04) || ((isr_id & 0x0c) == 0x0c)) //receciver data available or character timeout indication
	{
		while(uart_reg_ram->LSR.LSR_BIT.DR)
		{
				res = (uint8_t)uart_reg_ram->DATA_DLL.DATA;
				uart_putc_noint_no_wait(UART0,res );
			  app_at_recv_c(res);
		}
	}
	else if((isr_id & 0x06) == 0x06)//receiver line status interrupt
	{
		uint32_t tmp = uart_reg_ram->LSR.LSR_DWORD;
		uart_reg_ram->FCR_IID.FCR = isr_id;
		uart_reg_ram->IER_DLH.IER.ELSI = 0;
	}
}



__attribute__((section("ram_code"))) void uart1_isr(void)
{
	uint32_t isr_id;
	volatile struct_UART_t * const uart_reg_ram = (volatile struct_UART_t *)UART1_BASE;
	isr_id = uart_reg_ram->FCR_IID.IID;
	if(((isr_id & 0x04) == 0x04) || ((isr_id & 0x0c) == 0x0c)) //receciver data available or character timeout indication
	{
		while(uart_reg_ram->LSR.LSR_BIT.DR)
		{
			
   		uart_putc_noint_no_wait(UART1, (uint8_t)uart_reg_ram->DATA_DLL.DATA);
		 
 
		}
	}
	else if((isr_id & 0x06) == 0x06)//receiver line status interrupt
	{
		uint32_t tmp = uart_reg_ram->LSR.LSR_DWORD;
		uart_reg_ram->FCR_IID.FCR = isr_id;
		uart_reg_ram->IER_DLH.IER.ELSI = 0;
	}
}

/*-------------------------------------------------------------------------
    Function    :  ascii_char2val             ----add by chsheng, chsheng@accelsemi.com
    Return: -1=error
    Description:
        'a' -> 0xa  'A' -> 0xa
-------------------------------------------------------------------------*/
static char ascii_char2val(const char c)
{
    if(c>='0' && c<='9')
        return c-'0';
    if((c>='a' && c<='f') || (c>='A' && c<='F'))
        return (c&0x7)+9;

    return (char)(-1);
}

/*-------------------------------------------------------------------------
    Function    :  ascii_strn2val             ----add by chsheng, chsheng@accelsemi.com
    Return: -1=error
    Description:
        str = "123" bas = 10 return 123
        str = "123" bas = 16 return 0x123        
-------------------------------------------------------------------------*/
int ascii_strn2val( const char str[], char base, char n)
{
    int val = 0;
    char v;
    while(n != 0){
        v = ascii_char2val(*str);
#if 0
        if (v == -1 || v >= base) 
            return -1;
#else
        if (v == (char)(-1) || v >= base)
        {
            if(val == 0) //to filter abormal beginning and ending
            {
                str ++;
                n --;
                continue;
            }
            else
            {
                break;
            }
        }
#endif
        val = val*base + v;
        
        str++;
        n--;
    }
    return val;
}

static void app_at_recv_cmd_A(uint8_t sub_cmd, uint8_t *data)
{
    switch(sub_cmd)
    {
        case 'A':
            {
                mac_addr_t addr;
                addr.addr[5] = ascii_strn2val((const char *)&data[0], 16, 2);
                addr.addr[4] = ascii_strn2val((const char *)&data[2], 16, 2);
                addr.addr[3] = ascii_strn2val((const char *)&data[4], 16, 2);
                addr.addr[2] = ascii_strn2val((const char *)&data[6], 16, 2);
                addr.addr[1] = ascii_strn2val((const char *)&data[8], 16, 2);
                addr.addr[0] = ascii_strn2val((const char *)&data[10], 16, 2);
                gap_start_conn(&addr, ascii_strn2val((const char *)&data[12], 16, 2), 64, 64, 0, 500);
            }
            break;
        case 'B':
            {
                gap_stop_conn();
            }
            break;
        case 'E':
            co_printf("OOL VAL: 0x%02x.\r\n", ool_pd_read(ascii_strn2val((const char *)&data[0], 16, 2)));
            break;
        case 'F':
            ool_pd_write(ascii_strn2val((const char *)&data[0], 16, 2), ascii_strn2val((const char *)&data[3], 16, 2));
            co_printf("OK\r\n");
            break;
        case 'G':
            co_printf("hello world!\r\n");
            break;
        case 'H':
            co_printf("VAL: 0x%08x.\r\n", *(volatile uint32_t *)(ascii_strn2val((const char *)&data[0], 16, 8)));
            break;
        case 'I':
            *(volatile uint32_t *)ascii_strn2val((const char *)&data[0], 16, 8) = ascii_strn2val((const char *)&data[9], 16, 8);
            co_printf("OK\r\n");
            break;
        case 'J':
            co_printf("OOL VAL: 0x%02x.\r\n", ool_read(ascii_strn2val((const char *)&data[0], 16, 2)));
            break;
        case 'K':
            ool_write(ascii_strn2val((const char *)&data[0], 16, 2), ascii_strn2val((const char *)&data[3], 16, 2));
            co_printf("OK\r\n");
            break;
        case 'L':
            co_printf("VAL: 0x%02x.\r\n", *(uint8_t *)(ascii_strn2val((const char *)&data[0], 16, 8)));
            break;
        case 'M':
            *(uint8_t *)(ascii_strn2val((const char *)&data[0], 16, 8)) = ascii_strn2val((const char *)&data[9], 16, 2);
            co_printf("OK\r\n");
            break;
        case 'O':
            //new_efuse_value = ascii_strn2val((const char *)&cmd->data[0], 16, 8);
            co_printf("OK\r\n");
            break;
        case 'P':
            co_printf("VAL: 0x%02x.\r\n", *(uint8_t *)(MODEM_BASE + ascii_strn2val((const char *)&data[0], 16, 2)));
            break;
        case 'Q':
            *(uint8_t *)(MODEM_BASE + ascii_strn2val((const char *)&data[0], 16, 2)) = ascii_strn2val((const char *)&data[3], 16, 2);
            co_printf("OK\r\n");
            break;
        case 'U':
            {
                uint32_t *ptr = (uint32_t *)(ascii_strn2val((const char *)&data[0], 16, 8) & (~3));
                uint8_t count = ascii_strn2val((const char *)&data[9], 16, 2);
                uint32_t *start = (uint32_t *)((uint32_t)ptr & (~0x0f));
                for(uint8_t i=0; i<count;) {
                    if(((uint32_t)start & 0x0c) == 0) {
                        co_printf("0x%08x: ", start);
                    }
                    if(start < ptr) {
                        co_printf("        ");
                    }
                    else {
                        i++;
                        co_printf("%08x", *start);
                    }
                    if(((uint32_t)start & 0x0c) == 0x0c) {
                        co_printf("\r\n");
                    }
                    else {
                        co_printf(" ");
                    }
                    start++;
                }
            }
            break;
        case 'V':
            co_printf("%d\r\n", ke_get_mem_usage(3));
            break;
    }
}

void app_at_cmd_recv_handler(uint8_t *data, uint16_t length)
{
    switch(data[0])
    {
        case 'A':
            app_at_recv_cmd_A(data[1], &data[2]);
            break;
        
        default:
            break;
    }
}

  
__attribute__((section("ram_code"))) static void app_at_recv_c(uint8_t c)
{
    switch(at_recv_state)
    {
        case 0:
            if(c == 'A')
            {
                at_recv_state++;
            }
            break;
        case 1:
            if(c == 'T')
                at_recv_state++;
            else
                at_recv_state = 0;
            break;
        case 2:
            if(c == '#')
                at_recv_state++;
            else
                at_recv_state = 0;
            break;
        case 3:
            at_recv_buffer[at_recv_index++] = c;
            if((c == '\n')
               ||(at_recv_index >= AT_RECV_MAX_LEN))
            {
                os_event_t at_cmd_event;
                at_cmd_event.event_id = USER_EVT_AT_COMMAND;
                at_cmd_event.param = at_recv_buffer;
                at_cmd_event.param_len = at_recv_index;
                os_msg_post(user_task_id, &at_cmd_event);

                at_recv_state = 0;
                at_recv_index = 0;
            }
            break;
    }
}

#if 0
__attribute__((section("ram_code"))) void app_at_uart_recv(void*dummy, uint8_t status)
{
    app_at_recv_c(app_at_recv_char);
    uart0_read_for_hci(&app_at_recv_char, 1, app_at_uart_recv, NULL);
}

__attribute__((section("ram_code"))) void app_at_init(void)
{
    /* set PA0 and PA1 for AT command interface */
    system_set_port_mux(GPIO_PORT_A, GPIO_BIT_0, PORTA0_FUNC_UART0_RXD);
    system_set_port_mux(GPIO_PORT_A, GPIO_BIT_1, PORTA1_FUNC_UART0_TXD);
    
    uart_init(UART0, 1152);
    
    fr_uart_enableIrq(UART0, Uart_irq_erbfi);
    
    uart0_read_for_hci(&app_at_recv_char, 1, app_at_uart_recv, NULL);
}
#endif
