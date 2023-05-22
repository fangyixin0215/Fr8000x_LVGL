/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdio.h>
#include <string.h>

#include "gap_api.h"
#include "gatt_api.h"
#include "ble_stack.h"

#include "os_timer.h"
#include "os_mem.h"
#include "button.h"
#include "jump_table.h"
#include "co_printf.h"
#include "co_log.h"
#include "sys_utils.h"

#include "user_task.h"
#include "app_at.h"

#include "plf.h"
#include "driver_system.h"
#include "driver_psram.h"
#include "driver_pmu.h"

#include "ble_simple_peripheral.h"
#include "simple_gatt_service.h"
#include "co_log.h"
#include "dma_refresh_lvgl.h"
#include "driver_ktm57xx.h"

#undef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL        (LOG_LEVEL_INFO)
const char *app_tag = "project";

#define SYSTEM_STACK_SIZE           0x1000

extern void gui_main(void);


/*
 * LOCAL VARIABLES
 */
__attribute__((section("stack_section"))) static uint32_t system_stack[SYSTEM_STACK_SIZE/sizeof(uint32_t)];

const struct jump_table_version_t _jump_table_version __attribute__((section("jump_table_3"))) = 
{
    .stack_top_address = &system_stack[SYSTEM_STACK_SIZE/sizeof(uint32_t)],
    .firmware_version = 0x00000000,
};

const struct jump_table_image_t _jump_table_image __attribute__((section("jump_table_1"))) =
{
    .image_type = IMAGE_TYPE_APP,
    .image_size = 0x80000,      
};

/*********************************************************************
 * @fn      user_custom_parameters
 *
 * @brief   initialize several parameters, this function will be called 
 *          at the beginning of the program. 
 *
 * @param   None. 
 *       
 *
 * @return  None.
 */ 
void user_custom_parameters(void)
{
    __jump_table.addr.addr[0] = 0xBD;
    __jump_table.addr.addr[1] = 0xAD;
    __jump_table.addr.addr[2] = 0x10;
    __jump_table.addr.addr[3] = 0x11;
    __jump_table.addr.addr[4] = 0x20;
    __jump_table.addr.addr[5] = 0x20;
    __jump_table.system_clk = SYSTEM_SYS_CLK_96M;
    
    __jump_table.diag_port = 0x00008300;
    
    __jump_table.system_option &= (~SYSTEM_OPTION_SLEEP_ENABLE);

    //system_set_internal_flash_clock_div(0);
    //system_enable_internal_flash_q_read();
}

void user_init_static_memory(void)
{
    //initial_static_memory(22, 1, 20, 6, 27, 6, 27, 254, 0x800); //for multi connection
    //initial_static_memory(6, 2, 4, 8, 100, 8, 100, 31, 0x1100E000);
}



/*********************************************************************
 * @fn      user_entry_before_sleep_imp
 *
 * @brief   Before system goes to sleep mode, user_entry_before_sleep_imp()
 *          will be called, MCU peripherals can be configured properly before 
 *          system goes to sleep, for example, some MCU peripherals need to be
 *          used during the system is in sleep mode. 
 *
 * @param   None. 
 *       
 *
 * @return  None.
 */
__attribute__((section("ram_code"))) void user_entry_before_sleep_imp(void)
{
}

/*********************************************************************
 * @fn      user_entry_after_sleep_imp
 *
 * @brief   After system wakes up from sleep mode, user_entry_after_sleep_imp()
 *          will be called, MCU peripherals need to be initialized again, 
 *          this can be done in user_entry_after_sleep_imp(). MCU peripherals
 *          status will not be kept during the sleep. 
 *
 * @param   None. 
 *       
 *
 * @return  None.
 */
__attribute__((section("ram_code"))) void user_entry_after_sleep_imp(void)
{

}



__attribute__((section("ram_code"))) void main_loop(void)
{
	
    while(1)
    {
        if(ble_stack_schedule_allow())
        {
            /*user code should be add here*/
            
            /* schedule internal stack event */
            ble_stack_schedule();
        }
        
        GLOBAL_INT_DISABLE();
        switch(ble_stack_sleep_check())
        {
            case 2:
            {
                ble_stack_enter_sleep();
            }
            break;
            default:
                break;
        }
        GLOBAL_INT_RESTORE();
        
        ble_stack_schedule_backward();
    }
}

/*********************************************************************
 * @fn      proj_init
 *
 * @brief   Main entrancy of user application. This function is called after BLE stack
 *          is initialized, and all the application code will be executed from here.
 *          In that case, application layer initializtion can be startd here. 
 *
 * @param   None. 
 *       
 *
 * @return  None.
 */
void proj_init(void)
{
    LOG_INFO(app_tag, "GUI Demo\r\n");

    __SYSTEM_GPIO_CLK_ENABLE();
    system_regs->mdm_qspi_cfg.qspi_ref_128m_en = 1; // configure qspi reference clock to 128MHz
    system_regs->mdm_qspi_cfg.qspi_ref_clk_sel = 1; // qspi is used for internal flash, set its reference clock to 96MHz
//    system_enable_internal_flash_q_read(0x01);
    system_set_internal_flash_clock_div(0);
    //flash_set_read_fast_quad();
	
    // configure PSRAM pin and init PSRAM
    system_set_port_mux(GPIO_PORT_C, GPIO_BIT_0, PORTC0_FUNC_QSPI0_IO3);
    system_set_port_mux(GPIO_PORT_C, GPIO_BIT_1, PORTC1_FUNC_QSPI0_SCLK0);
    system_set_port_mux(GPIO_PORT_C, GPIO_BIT_2, PORTC2_FUNC_QSPI0_CSN0);
    system_set_port_mux(GPIO_PORT_C, GPIO_BIT_3, PORTC3_FUNC_QSPI0_IO1);
    system_set_port_mux(GPIO_PORT_C, GPIO_BIT_4, PORTC4_FUNC_QSPI0_IO2);
    system_set_port_mux(GPIO_PORT_C, GPIO_BIT_5, PORTC5_FUNC_QSPI0_IO0);
    psram_init();
	
	  //lv_gpu_stm32_dma2d_init();
    
    // User task initialization, for buttons.
    user_task_init();
    uart0_init(115200);
    simple_peripheral_init();
		
    printf(" %s size =%d\r\n",__FUNCTION__,os_get_free_heap_size());
		
    gui_main();
		
		
}



/*********************************************************************
 * @fn      user_main
 *
 * @brief   Code to be executed before BLE stack to be initialized.
 *          Power mode configurations, PMU part driver interrupt enable, MCU 
 *          peripherals init, etc. 
 *
 * @param   None. 
 *       
 *
 * @return  None.
 */
void user_main(void)
{
		mac_addr_t mac_addr;

    /* initialize log module */
    log_init();

    /* initialize PMU module at the beginning of this program */
    pmu_sub_init();
    pmu_ioldosw_ctrl(true);

    /* enable gpio clock */
    __SYSTEM_GPIO_CLK_ENABLE();
    /* set system clock */
    system_set_clock(SYSTEM_CLOCK_SEL);
	
    /* configure ble stack capabilities */
    ble_stack_configure(BLE_STACK_ENABLE_MESH,
                        BLE_STACK_ENABLE_CONNECTIONS,
                        BLE_STACK_RX_BUFFER_CNT,
                        BLE_STACK_RX_BUFFER_SIZE, 
                        BLE_STACK_TX_BUFFER_CNT, 
                        BLE_STACK_TX_BUFFER_SIZE, 
                        BLE_STACK_ADV_BUFFER_SIZE,
                        BLE_STACK_RETENTION_RAM_SIZE,
                        BLE_STACK_KEY_STORAGE_OFFSET);
		
    mac_addr.addr[0] = 0x55;
    mac_addr.addr[1] = 0x44;
    mac_addr.addr[2] = 0x33;
    mac_addr.addr[3] = 0x22;
    mac_addr.addr[4] = 0x11;
    mac_addr.addr[5] = 0x20;
    gap_address_set(&mac_addr, BLE_ADDR_TYPE_PRIVATE);										
												
    /* initialize ble stack */
    ble_stack_init();
    system_sleep_disable();


    proj_init();
    /* enter main loop */
    main_loop();
}
