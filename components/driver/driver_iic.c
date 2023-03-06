#include <stdint.h>

#include "co_printf.h"

#include "plf.h"
#include "driver_iic.h"
#include "driver_system.h"

#define I2C_LOG(...)            //co_printf(__VA_ARGS__)

#define SS_SCL_HCNT             (48*1000*1000/(100*1000)/2)
#define SS_SCL_LCNT             (48*1000*1000/(100*1000)/2)
#define FS_SCL_HCNT             (48*1000*1000/(400*1000)/2)
#define FS_SCL_LCNT             (48*1000*1000/(400*1000)/2)
#define FS_SPIKE_CNT            (48*1000*1000/(400*1000)/8)
#define FSP_SCL_HCNT            (48*1000*1000/(1000*1000)/2)
#define FSP_SCL_LCNT            (48*1000*1000/(1000*1000)/2)
#define FSP_SPIKE_CNT           (48*1000*1000/(1000*1000)/8)
#define HS_SCL_HCNT             (96*1000*1000/(3400*1000)/2)
#define HS_SCL_LCNT             (96*1000*1000/(3400*1000)/2)
#define HS_SPIKE_CNT            (96*1000*1000/(3400*1000)/8)

#define IIC_TX_FIFO_DEPTH       32
#define IIC_RX_FIFO_DEPTH       32

void i2c_init(struct i2c_config_param_t *param)
{
    volatile struct i2c_regs_t *i2c_regs;
        
    /* reset module first */
    if(param->channel == I2C_CHANNEL_0) {
        system_regs->rst.i2c0_rst = 1;
        system_regs->clk_gate.i2c0_clk_en = 1;
        system_regs->misc.rich_i2c_apb_clk_en = 1;
        if((param->speed == I2C_SPEED_HIGH)
            && (param->work_role == I2C_WORK_ROLE_MASTER)) {
            system_regs->clk_cfg.i2c0_clk_sel = 1;
        }
        else {
            system_regs->clk_cfg.i2c0_clk_sel = 0;
        }

        i2c_regs = (struct i2c_regs_t *)I2C0_BASE;
    }
    else if(param->channel == I2C_CHANNEL_1) {
        system_regs->rst.i2c1_rst = 1;
        system_regs->clk_gate.i2c1_clk_en = 1;
        system_regs->clk_cfg.i2c1_clk_sel = 0;
        system_regs->misc.tiny_i2c_apb_clk_en = 1;
        if(param->speed != I2C_SPEED_STANDARD) {
            /* I2C_CHANNEL_1 support I2C_SPEED_STANDARD only */
            return;
        }

        i2c_regs = (struct i2c_regs_t *)I2C1_BASE;
    }
    else {
        return;
    }

    if(param->work_role == I2C_WORK_ROLE_MASTER) {
        i2c_regs->ctrl.slave_disable = 1;
        i2c_regs->ctrl.master_enable = 1;

        /* configure clock when working in master mode */
        if(param->speed == I2C_SPEED_FAST_PLUS) {
            i2c_regs->ctrl.speed = I2C_SPEED_FAST;
        }
        else {
            i2c_regs->ctrl.speed = param->speed;
        }
        
        if(param->speed == I2C_SPEED_STANDARD) {
            /* i2c clock is 48MHz */
            i2c_regs->ss_scl_hcnt = SS_SCL_HCNT;
            i2c_regs->ss_scl_lcnt = SS_SCL_LCNT;
        }
        else if(param->speed == I2C_SPEED_FAST) {
            /* i2c clock is 48MHz */
            i2c_regs->fs_scl_hcnt = FS_SCL_HCNT;
            i2c_regs->fs_scl_lcnt = FS_SCL_LCNT;
            i2c_regs->fs_spklen = FS_SPIKE_CNT;
        }
        else if(param->speed == I2C_SPEED_FAST_PLUS) {
            /* i2c clock is 48MHz */
            i2c_regs->fs_scl_hcnt = FSP_SCL_HCNT;
            i2c_regs->fs_scl_lcnt = FSP_SCL_LCNT;
            i2c_regs->fs_spklen = FSP_SPIKE_CNT;
        }
        else if(param->speed == I2C_SPEED_HIGH) {
            /* i2c clock is 96MHz */
            i2c_regs->hs_scl_hcnt = HS_SCL_HCNT;
            i2c_regs->hs_scl_lcnt = HS_SCL_LCNT;
            i2c_regs->hs_spklen = HS_SPIKE_CNT;
        }
        else {
            return;
        }
    }
    else {
        i2c_regs->ctrl.slave_disable = 0;
        i2c_regs->ctrl.master_enable = 0;

        /* configure address */
        i2c_regs->ctrl.bit10addr_slave = param->addr_type;
        i2c_regs->tar.tar = param->slave_addr;
    }
}

void i2c_transmit(struct i2c_transmit_param_t *param)
{
    volatile struct i2c_regs_t *i2c_regs;
    union i2c_data_cmd_t data_cmd;
    uint8_t reg_length = param->reg_length;
    uint8_t *regs = (void *)&param->reg;
    uint8_t *data;

    if(param->channel == I2C_CHANNEL_0) {
        i2c_regs = (struct i2c_regs_t *)I2C0_BASE;
    }
    else if(param->channel == I2C_CHANNEL_1) {
        i2c_regs = (struct i2c_regs_t *)I2C1_BASE;
    }
    else {
        return;
    }

    if(i2c_regs->status.activity) {
        I2C_LOG("i2c_transmit: i'm busy.\r\n");
        return;
    }

    i2c_regs->ctrl.restart_en = 1;

    i2c_regs->tar.special = 0;
    i2c_regs->tar.gc_or_start = 0;
    i2c_regs->tar.bit10addr_master = param->addr_type;
    i2c_regs->tar.tar = param->slave_addr;

    i2c_regs->ctrl.stop_det_ifaddressed = 1;

    i2c_regs->tx_tl = IIC_TX_FIFO_DEPTH / 2;

    i2c_regs->enable.tx_cmd_blcok = 0;
    i2c_regs->enable.enable = 1;
    if(reg_length) {
        I2C_LOG("i2c_transmit: trasmit header.\r\n");
        data_cmd.p.data = *regs++;
        data_cmd.p.cmd = 0;
        data_cmd.p.stop = 0;
        data_cmd.p.restart = 0;
        i2c_regs->data_cmd.data = data_cmd.data;
        reg_length--;
    }

    while(reg_length) {
        while(i2c_regs->raw_intr_mask.tx_empty == 0);
        i2c_regs->data_cmd.data = *regs++;
    }

    data = param->buffer;
    while(param->length > 1) {
        while(i2c_regs->raw_intr_mask.tx_empty == 0);
        i2c_regs->data_cmd.data = *data++;
        param->length--;
    }
    data_cmd.data = *data;
    data_cmd.p.stop = 1;
    while(i2c_regs->raw_intr_mask.tx_empty == 0);
    i2c_regs->data_cmd.data = data_cmd.data;

    while(i2c_regs->raw_intr_mask.stop_det == 0);

    i2c_regs->enable.enable = 0;
}

void i2c_receive(struct i2c_receive_param_t *param)
{
    volatile struct i2c_regs_t *i2c_regs;
    union i2c_data_cmd_t data_cmd;
    uint8_t reg_length = param->reg_length;
    uint8_t *regs = (void *)&param->reg;
    uint8_t *data;

    if(param->channel == I2C_CHANNEL_0) {
        i2c_regs = (struct i2c_regs_t *)I2C0_BASE;
    }
    else if(param->channel == I2C_CHANNEL_1) {
        i2c_regs = (struct i2c_regs_t *)I2C1_BASE;
    }
    else {
        return;
    }

    if(i2c_regs->status.activity) {
        I2C_LOG("i2c_transmit: i'm busy.\r\n");
        return;
    }

    i2c_regs->ctrl.restart_en = 1;

    i2c_regs->tar.special = 0;
    i2c_regs->tar.gc_or_start = 0;
    i2c_regs->tar.bit10addr_master = param->addr_type;
    i2c_regs->tar.tar = param->slave_addr;

    i2c_regs->ctrl.stop_det_ifaddressed = 1;

    i2c_regs->rx_tl = IIC_RX_FIFO_DEPTH / 2;

    i2c_regs->enable.tx_cmd_blcok = 0;
    i2c_regs->enable.enable = 1;
    if(reg_length) {
        I2C_LOG("i2c_transmit: trasmit header.\r\n");
        data_cmd.p.data = *regs++;
        data_cmd.p.cmd = 0;
        data_cmd.p.stop = 0;
        data_cmd.p.restart = 0;
        i2c_regs->data_cmd.data = data_cmd.data;
        reg_length--;
    }

    while(reg_length) {
        while(i2c_regs->raw_intr_mask.tx_empty == 0);
        i2c_regs->data_cmd.data = *regs++;
    }

    data = param->buffer;
    data_cmd.data = 0;
    data_cmd.p.cmd = 1;
    while(param->length > 1) {
        i2c_regs->data_cmd.data = data_cmd.data;
        while(i2c_regs->status.rfne == 0);
        *data++ = i2c_regs->data_cmd.data;
        param->length--;
    }
    data_cmd.p.stop = 1;
    i2c_regs->data_cmd.data = data_cmd.data;
    while(i2c_regs->status.rfne == 0);
    *data++ = i2c_regs->data_cmd.data;

    while(i2c_regs->raw_intr_mask.stop_det == 0);

    i2c_regs->enable.enable = 0;
}

#if 0
void test_i2c(void)
{
#define TEST_I2C_LENGTH         10
    struct i2c_config_param_t config_param;
    struct i2c_transmit_param_t transmit_param;
    struct i2c_receive_param_t receive_param;

    system_set_port_pull(GPIO_PC4|GPIO_PC5|GPIO_PC2|GPIO_PC3, GPIO_PULL_UP, true);
    system_set_port_mux(GPIO_PORT_C, GPIO_BIT_4, PORTC4_FUNC_I2C0_CLK);
    system_set_port_mux(GPIO_PORT_C, GPIO_BIT_5, PORTC5_FUNC_I2C0_DAT);
    system_set_port_mux(GPIO_PORT_C, GPIO_BIT_2, PORTC2_FUNC_I2C1_CLK);
    system_set_port_mux(GPIO_PORT_C, GPIO_BIT_3, PORTC3_FUNC_I2C1_DAT);

    config_param.channel = I2C_CHANNEL_0;
    config_param.speed = I2C_SPEED_FAST;
    config_param.work_role = I2C_WORK_ROLE_MASTER;
    i2c_init(&config_param);

    co_printf("after i2c_init.\r\n");

    transmit_param.channel = I2C_CHANNEL_0;
    transmit_param.addr_type = I2C_ADDR_MODE_7b;
    transmit_param.slave_addr = 0x55;
    transmit_param.reg_length = 1;
    transmit_param.reg = 0x02;
    transmit_param.length = TEST_I2C_LENGTH;
    transmit_param.buffer = (void *)0;
    i2c_transmit(&transmit_param);

    co_printf("after i2c_transmit.\r\n");

    uint8_t rx_buffer[TEST_I2C_LENGTH];
    receive_param.channel = I2C_CHANNEL_0;
    receive_param.addr_type = I2C_ADDR_MODE_7b;
    receive_param.slave_addr = 0x55;
    receive_param.reg_length = 1;
    receive_param.reg = 0x02;
    receive_param.length = TEST_I2C_LENGTH;
    receive_param.buffer = rx_buffer;
    i2c_receive(&receive_param);
}
#endif

