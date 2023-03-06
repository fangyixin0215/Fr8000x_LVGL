#ifndef _DRIVER_IIC_H
#define _DRIVER_IIC_H

#include <stdint.h>

enum i2c_channel_t {
    I2C_CHANNEL_0, 
    I2C_CHANNEL_1,
};

enum i2c_work_role_t {
    I2C_WORK_ROLE_MASTER,
    I2c_WORK_ROLE_SLAVE,
};

enum i2c_work_mode_t {
    I2C_WORK_MODE_BLOCK,
    I2C_WORK_MODE_INTERRUPT,
    I2C_WORK_MODE_DMA,
};

enum i2c_speed_t {
    I2C_SPEED_STANDARD = 1, // 100k
    I2C_SPEED_FAST = 2,     // 400k
    I2C_SPEED_HIGH = 3,     // 3400k
    I2C_SPEED_FAST_PLUS = 4,// 1000k
};

enum i2c_addr_type_t {
    I2C_ADDR_MODE_7b,
    I2c_ADDR_MODE_10b,
};

struct i2c_ctrl_t {
    uint32_t master_enable:1;
    uint32_t speed:2;
    uint32_t bit10addr_slave:1;
    uint32_t reserved3:1;
    uint32_t restart_en:1;
    uint32_t slave_disable:1;
    uint32_t stop_det_ifaddressed:1;
    uint32_t tx_emtpy_ctrl:1;
    uint32_t rx_fifo_full_hld_ctrl:1;
    uint32_t stop_det_if_master_active:1;
    uint32_t bus_clear_featrure_ctrl:1;
    uint32_t reserved2:4;
    uint32_t reserved1:16;
};

struct i2c_tar_t {
    uint32_t tar:10;
    uint32_t gc_or_start:1;
    uint32_t special:1;
    uint32_t bit10addr_master:1;
    uint32_t device_id:1;
    uint32_t reserved1:18;
};

union i2c_data_cmd_t {
    uint32_t data;
    struct {
        uint32_t data:8;
        uint32_t cmd:1;             // valid only in master mode, 1: read, 0: write;
        uint32_t stop:1;            // This bit controls whether a STOP is issued after the byte is sent or received.
        uint32_t restart:1;         // This bit controls whether a RESTART is issued before the byte is sent or received.
        uint32_t first_data_byte:1; // indicate the received data is the first byte or not
        uint32_t reserved1:20;
    } p;
};

struct i2c_intr_stat_t {
    uint32_t rx_under:1;
    uint32_t rx_over:1;
    uint32_t rx_full:1;
    uint32_t tx_over:1;
    uint32_t tx_empty:1;
    uint32_t rd_req:1;
    uint32_t tx_abrt:1;
    uint32_t rx_done:1;
    uint32_t activity:1;
    uint32_t stop_det:1;
    uint32_t start_det:1;
    uint32_t gen_call:1;
    uint32_t restart_det:1;
    uint32_t master_on_hold:1;
    uint32_t scl_stuck_at_low:1;
    uint32_t reserved1:17;
};

struct i2c_intr_mask_t {
    uint32_t rx_under:1;
    uint32_t rx_over:1;
    uint32_t rx_full:1;
    uint32_t tx_over:1;
    uint32_t tx_empty:1;
    uint32_t rd_req:1;
    uint32_t tx_abrt:1;
    uint32_t rx_done:1;
    uint32_t activity:1;
    uint32_t stop_det:1;
    uint32_t start_det:1;
    uint32_t gen_call:1;
    uint32_t restart_det:1;
    uint32_t master_on_hold:1;
    uint32_t scl_stuck_at_low:1;
    uint32_t reserved1:17;
};

struct i2c_raw_intr_mask_t {
    uint32_t rx_under:1;            // Set if the processor attempts to read the receive buffer when it is empty by reading from the IC_DATA_CMD register.
    uint32_t rx_over:1;             // Set if the receive buffer is completely filled to IC_RX_BUFFER_DEPTH and an additional byte is received from an external I2C device.
    uint32_t rx_full:1;             // Set when the receive buffer reaches or goes above the RX_TL threshold in the IC_RX_TL register.
    uint32_t tx_over:1;             // Set during transmit if the transmit buffer is filled to IC_TX_BUFFER_DEPTH and the processor attempts to issue another I2C command by writing to the IC_DATA_CMD register.
    uint32_t tx_empty:1;            // This bit is set to 1 when the transmit buffer is at or below the threshold value set in the IC_TX_TL register
    uint32_t rd_req:1;              // This bit is set to 1 when DW_apb_i2c is acting as a slave and another I2C master is attempting to read data from DW_apb_i2c.
    uint32_t tx_abrt:1;             // This bit indicates if DW_apb_i2c, as an I2C transmitter, is unable to complete the intended actions on the contents of the transmit FIFO
    uint32_t rx_done:1;             // When the DW_apb_i2c is acting as a slave-transmitter, this bit is set to 1 if the master does not acknowledge a transmitted byte.
    uint32_t activity:1;            // This bit captures DW_apb_i2c activity and stays set until it is cleared.
    uint32_t stop_det:1;            // Indicates whether a STOP condition has occurred on the I2C interface regardless of whether DW_apb_i2c is operating in slave or master mode.
    uint32_t start_det:1;           // Indicates whether a START or RESTART condition has occurred on the I2C interface regardless of whether DW_apb_i2c is operating in slave or master mode.
    uint32_t gen_call:1;            // Set only when a General Call address is received and it is acknowledged.
    uint32_t restart_det:1;         // Indicates whether a RESTART condition has occurred on the I2C interface when DW_apb_i2c is operating in Slave mode and the slave is being addressed.
    uint32_t master_on_hold:1;      // Indicates whether master is holding the bus and TX FIFO is empty
    uint32_t scl_stuck_at_low:1;    // Indicates whether the SCL Line is stuck at low for the IC_SCL_STUCK_LOW_TIMEOUT number of ic_clk periods
    uint32_t reserved1:17;
};

struct i2c_enable_t {
    uint32_t enable:1;
    uint32_t abort:1;
    uint32_t tx_cmd_blcok:1;
    uint32_t sda_stuck_recover_enable:1;
    uint32_t reserved1:28;
};

struct i2c_status_t {
    uint32_t activity:1;                // I2C Activity Status.
    uint32_t tfnf:1;                    // Transmit FIFO Not Full
    uint32_t tfe:1;                     // Transmit FIFO Completely Empty.
    uint32_t rfne:1;                    // Receive FIFO Not Empty
    uint32_t rff:1;                     // Receive FIFO Completely Full
    uint32_t mst_activity:1;            // Master FSM Activity Status. When the Master Finite State Machine (FSM) is not in the IDLE state, this bit is set
    uint32_t slv_activity:1;            // Slave FSM Activity Status. When the Slave Finite State Machine (FSM) is not in the IDLE state, this bit is set.
    uint32_t mst_hold_tx_fifo_empty:1;  // This bit indicates the BUS hold when the master holds the bus because of the Tx FIFO being empty
    uint32_t mst_hold_rx_fifo_full:1;   // This bit indicates the BUS Hold in Master mode due to Rx FIFO is Full and additional byte has been received
    uint32_t slv_hold_tx_fifo_empty:1;  // This bit indicates the BUS Hold in Slave mode for the Read request when the Tx FIFO is empty
    uint32_t slv_hold_rx_fifo_full:1;   // This bit indicates the BUS Hold in Slave mode due to Rx FIFO is Full and an additional byte has been received
    uint32_t sda_stuck_not_recovered:1; // This bit indicates that SDA stuck at low is not recovered after the recovery mechanism.
    uint32_t reserved1:20;
};

struct i2c_sda_hold_t {
    uint32_t sda_tx_hold:16;
    uint32_t sda_rx_hold:8;
    uint32_t reserved1:8;
};

struct i2c_tx_abrt_source_t {
    uint32_t abrt_7b_addr_noack:1;      // This field indicates that the Master is in 7-bit addressing mode and the address sent was not acknowledged by any slave.
    uint32_t abrt_10addr1_noack:1;      // This field indicates that the Master is in 10-bit address mode and the first 10-bit address byte was not acknowledged by any slave.
    uint32_t abrt_10addr2_noack:1;      // This field indicates that the Master is in 10-bit address mode and that the second address byte of the 10-bit address was not acknowledged by any slave.
    uint32_t abrt_txdata_noack:1;       // When the master receives an acknowledgement for the address, but when it sends data byte(s) following the address, it did not receive an acknowledge from the remote slave(s).
    uint32_t abrt_gcall_noack:1;        // This field indicates that DW_apb_i2c in master mode has sent a General Call and no slave on the bus acknowledged the General Call.
    uint32_t abrt_gcall_read:1;         // This field indicates that DW_apb_i2c in the master mode has sent a General Call but the user programmed the byte following the General Call to be a read from the bus (IC_DATA_CMD[9] is set to 1).
    uint32_t abrt_hs_ackdet:1;          // This field indicates that the Master is in High Speed mode and the High Speed Master code was acknowledged (wrong behavior).
    uint32_t abrt_sbyte_ackdet:1;       // This field indicates that the Master has sent a START Byte and the START Byte was acknowledged (wrong behavior
    uint32_t abrt_hs_norstrt:1;         // This field indicates that the restart is disabled (IC_RESTART_EN bit (IC_CON[5]) =0) and the user is trying to use the master to transfer data in High Speed mode.
    uint32_t abrt_sbyte_norstrt:1;
    uint32_t abrt_10b_rd_norstrt:1;     // This field indicates that the restart is disabled (IC_RESTART_EN bit (IC_CON[5]) =0) and the master sends a read command in 10-bit addressing mode
    uint32_t abrt_master_dis:1;         // This field indicates that the User tries to initiate a Master operation with the Master mode disabled
    uint32_t arb_lost:1;                // This field specifies that the Master has lost arbitration, or if IC_TX_ABRT_SOURCE[14] is also set, then the slave transmitter has lost arbitration.
    uint32_t abrt_slvflush_txfifo:1;    // This field specifies that the Slave has received a read command and some data exists in the TX FIFO, so the slave issues a TX_ABRT interrupt to flush old data in TX FIFO
    uint32_t abrt_slv_arblost:1;        // This field indicates that a Slave has lost the bus while transmitting data to a remote master.
    uint32_t abrt_slvrd_intx:1;         // When the processor side responds to a slave mode request for data to be transmitted to a remote master and user writes a 1 in CMD (bit 8) of IC_DATA_CMD register.
    uint32_t abrt_user_abrt:1;          // Master has detected the transfer abort (IC_ENABLE[1])
    uint32_t abrt_sda_stuck_at_low:1;   // Master detects the SDA Stuck at low for the IC_SDA_STUCK_AT_LOW_TIMEOUT value of ic_clks.
    uint32_t abrt_device_noack:1;       //This is a master-mode-only bit. Master is initiating the DEVICE_ID transfer and the device id sent was not acknowledged by any slave.
    uint32_t abrt_device_slvaddr_noack:1; // Master is initiating the DEVICE_ID transfer and the slave address sent was not acknowledged by any slave.
    uint32_t abrt_device_write:1;       // Master is initiating the DEVICE_ID transfer and the Tx-FIFO consists of write commands.
    uint32_t reserved1:2;
    uint32_t tx_flush_cnt:9;            // This field indicates the number of Tx FIFO Data Commands which are flushed due to TX_ABRT interrupt.
};

struct i2c_dma_cr_t {
    uint32_t rdmae:1;       // Receive DMA Enable
    uint32_t tdmae:1;       // Transmit DMA Enable
    uint32_t reserved1:30;
};

struct i2c_dma_tdlr_t {
    uint32_t dmatdl:5;      // Transmit Data Level. This bit field controls the level at which a DMA request is made by the transmit logic.
    uint32_t reserved:27;
};

struct i2c_dma_rdlr_t {
    uint32_t dmardl:5;      // Receive Data Level. This bit field controls the level at which a DMA request is made by the receive logic
    uint32_t reserved:27;
};

struct i2c_ack_general_call_t {
    uint32_t ack_gen_call:1;    // ACK General Call. When set to 1, DW_apb_i2c responds with a ACK (by asserting ic_data_oe) when it receives a General Call. Otherwise, DW_apb_i2c responds with a NACK (by negating ic_data_oe).
    uint32_t reserved1:31;
};

struct i2c_enable_status_t {
    uint32_t en:1;
    uint32_t slv_disabled_while_busy:1; // Slave Disabled While Busy (Transmit, Receive).
    uint32_t slv_rx_data_lost:1;        // This bit indicates if a Slave-Receiver operation has been aborted with at least one data byte received from an I2C transfer due to the setting bit 0 of IC_ENABLE from 1 to 0.
    uint32_t reserved1:29;
};

struct i2c_regs_t {
    /* @0x00 */
    struct i2c_ctrl_t ctrl;
    struct i2c_tar_t tar;
    uint32_t sar;           // local address in slave mode
    uint32_t hs_mar;        // HS-mode master code
    /* @0x10 */
    union i2c_data_cmd_t data_cmd;
    uint32_t ss_scl_hcnt;   // Standard Speed I2C Clock SCL High Count Register
    uint32_t ss_scl_lcnt;   // Standard Speed I2C Clock SCL Low Count Register
    uint32_t fs_scl_hcnt;   // Fast Mode or Fast Mode Plus I2C Clock SCL High Count Register
    /* @0x20 */
    uint32_t fs_scl_lcnt;   // Fast Mode or Fast Mode Plus I2C Clock SCL Low Count Register
    uint32_t hs_scl_hcnt;   // High Speed I2C Clock SCL High Count Register
    uint32_t hs_scl_lcnt;   // High Speed I2C Clock SCL Low Count Register
    struct i2c_intr_stat_t intr_stat;
    /* @0x30 */
    struct i2c_intr_mask_t intr_mask;
    struct i2c_raw_intr_mask_t raw_intr_mask;
    uint32_t rx_tl;         // I2C Receive FIFO Threshold Register
    uint32_t tx_tl;         // I2C Transmit FIFO Threshold Register
    /* @0x40 */
    uint32_t clr_intr;      // Clear Combined and Individual Interrupt Register
    uint32_t clr_rx_under;  // Clear RX_UNDER Interrupt Register
    uint32_t clr_rx_over;   // Clear RX_OVER Interrupt Register
    uint32_t clr_tx_over;   // Clear TX_OVER Interrupt Register
    /* @0x50 */
    uint32_t clr_rd_req;    // Clear RD_REQ Interrupt Register
    uint32_t clr_tx_abrt;   // Clear TX_ABRT Interrupt Register
    uint32_t clr_rx_done;   // Clear RX_DONE Interrupt Register
    uint32_t clr_activity;  // Clear ACTIVITY Interrupt Register
    /* @0x60 */
    uint32_t clr_stop_det;  // Clear STOP_DET Interrupt Register
    uint32_t clr_start_det; // Clear START_DET Interrupt Register
    uint32_t clr_gen_call;  // Clear GEN_CALL Interrupt Register
    struct i2c_enable_t enable;
    /* @0x70 */
    struct i2c_status_t status;
    uint32_t txflr;         // This register contains the number of valid data entries in the transmit FIFO buffer
    uint32_t rxflr;         // This register contains the number of valid data entries in the receive FIFO buffer
    struct i2c_sda_hold_t sda_hold;
    /* @0x80 */
    struct i2c_tx_abrt_source_t abort_source;
    uint32_t slv_data_nack_only;
    struct i2c_dma_cr_t dma_cr;
    struct i2c_dma_tdlr_t dma_tdlr;
    /* @0x90 */
    struct i2c_dma_rdlr_t dma_rdlr;
    uint32_t sda_setup;
    uint32_t ack_general_call;
    struct i2c_enable_status_t enable_status;
    /* @0xa0 */
    uint32_t fs_spklen;
    uint32_t hs_spklen;
    uint32_t clr_restart_det;
    uint32_t scl_stuck_at_low_timeout;
    /* @0xb0 */
    uint32_t sda_stuck_at_low_timeout;
    uint32_t clr_scl_stuck_det;
    uint32_t device_id;
    uint32_t reserved1;
    /* @0xc0 */
    uint32_t reserved2[4];
    /* @0xd0 */
    uint32_t reserved3[4];
    /* @0xe0 */
    uint32_t reserved4[4];
    /* @0xf0 */
    uint32_t reserved5;
    uint32_t comp_param_1;
    uint32_t comp_version;
    uint32_t comp_type;
};

struct i2c_config_param_t {
    enum i2c_channel_t channel;
    enum i2c_speed_t speed;
    enum i2c_work_role_t work_role;

    enum i2c_addr_type_t addr_type; // local address type when working in slave mode
    uint16_t slave_addr;            // local address when working in slave mode
};

/* transmit parameter when working in master mode */
struct i2c_transmit_param_t {
    enum i2c_channel_t channel;
    enum i2c_work_mode_t work_mode;
    enum i2c_addr_type_t addr_type; // device address type
    uint16_t slave_addr;            // device address

    uint8_t reg_length;
    uint32_t reg;

    uint16_t length;
    uint8_t *buffer;
};

/* receive parameter when working in master mode */
struct i2c_receive_param_t {
    enum i2c_channel_t channel;
    enum i2c_work_mode_t work_mode;
    enum i2c_addr_type_t addr_type; // device address type
    uint16_t slave_addr;            // device address

    uint8_t reg_length;
    uint32_t reg;

    uint16_t length;
    uint8_t *buffer;
};

void i2c_init(struct i2c_config_param_t *param);
void i2c_transmit(struct i2c_transmit_param_t *param);
void i2c_receive(struct i2c_receive_param_t *param);

#endif  // _DRIVER_IIC_H

