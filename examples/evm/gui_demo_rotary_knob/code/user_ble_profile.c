#include "user_ble_profile.h"
/*
 * INCLUDES (包含头文件)
 */
#include <stdio.h>
#include <string.h>
#include "co_printf.h"
#include "gap_api.h"
#include "gatt_api.h"
#include "gatt_sig_uuid.h"
#include "co_log.h"
#include "driver_system.h"
#include "user_knob_config.h"
/*
 * MACROS (宏定义)
 */

/*
 * CONSTANTS (常量定义)
 */
static uint8_t user_ntf_enable_flag=0;

// Simple GATT Profile Service UUID: 0xFFF0
const static uint8_t user_svc_uuid[] = USER_SVC_UUID;
/******************************* Characteristic 1 defination *******************************/
// Characteristic 1 UUID: 0xFFF1
// Characteristic 1 data 
#define USER_CHAR1_VALUE_LEN  128
static uint8_t user_char1_value[USER_CHAR1_VALUE_LEN] = {0};
// Characteristic 1 User Description
#define USER_CHAR1_DESC_LEN   6
static const uint8_t user_char1_desc[6] = "test";

#define USER_CHAR_CCC_LEN 2

/*
 * TYPEDEFS (类型定义)
 */

/*
 * GLOBAL VARIABLES (全局变量)
 */
uint8_t user_svc_id = 0;
/*
 * LOCAL VARIABLES (本地变量)
 */
static gatt_service_t user_profile_svc;

/*********************************************************************
 * Profile Attributes - Table
 * 每一项都是一个attribute的定义。
 * 第一个attribute为Service 的的定义。
 * 每一个特征值(characteristic)的定义，都至少包含三个attribute的定义；
 * 1. 特征值声明(Characteristic Declaration)
 * 2. 特征值的值(Characteristic value)
 * 3. 特征值描述符(Characteristic description)
 * 如果有notification 或者indication 的功能，则会包含四个attribute的定义，除了前面定义的三个，还会有一个特征值客户端配置(client characteristic configuration)。
 *
 */

const gatt_attribute_t user_profile_att_table[USER_IDX_NB] =
{
    // userr gatt Service Declaration
				[USER_IDX_SERVICE]={
                             { UUID_SIZE_2, UUID16_ARR(GATT_PRIMARY_SERVICE_UUID) },     /* UUID */
                              GATT_PROP_READ,                                             /* Permissions */
                              UUID_SIZE_16,                                                /* Max size of the value */     /* Service UUID size in service declaration */
                              (uint8_t*)user_svc_uuid,                                      /* Value of the attribute */    /* Service UUID value in service declaration */
                            },

        // Characteristic 1 Declaration           
        [USER_IDX_CHAR1_DECLARATION]          = {
                                                    { UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID) },           /* UUID */
																											GATT_PROP_READ,                                             /* Permissions */
																											0,                                                          /* Max size of the value */
																											NULL,                                                       /* Value of the attribute */
                                                },
        // Characteristic 1 Value                  
        [USER_IDX_CHAR1_VALUE]                =   {
                                                    { UUID_SIZE_16,USER_CHAR1_UUID },                 /* UUID */
                                                    GATT_PROP_NOTI,                           /* Permissions */                        /* Permissions */
                                                    USER_CHAR1_VALUE_LEN,                                         /* Max size of the value */
                                                    NULL,                                                       /* Value of the attribute */    /* Can assign a buffer here, or can be assigned in the application by user */
                                                },      

			   [USER_IDX_CHAR_CFG]                  =   {
                                                    { UUID_SIZE_2, UUID16_ARR(GATT_CLIENT_CHAR_CFG_UUID) },     /* UUID */
                                                    GATT_PROP_READ | GATT_PROP_WRITE,                           /* Permissions */
                                                    USER_CHAR_CCC_LEN,                                           /* Max size of the value */
                                                    NULL,                                                       /* Value of the attribute */    /* Can assign a buffer here, or can be assigned in the application by user */
                                                }, 																								
				        // Characteristic 1 User Description
//        [USER_IDX_CHAR1_USER_DESCRIPTION]     =   {
//                                                    { UUID_SIZE_2, UUID16_ARR(GATT_CHAR_USER_DESC_UUID) },      /* UUID */
//                                                    GATT_PROP_READ,                                             /* Permissions */
//                                                    USER_CHAR1_DESC_LEN,                                          /* Max size of the value */
//                                                    (uint8_t *)user_char1_desc,                                   /* Value of the attribute */
//                                                },
			
																									
			 // Characteristic 2 Declaration           
        [USER_IDX_CHAR2_DECLARATION]          = {
                                                    { UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID) },           /* UUID */
																											GATT_PROP_READ,                                             /* Permissions */
																											0,                                                          /* Max size of the value */
																											NULL,                                                       /* Value of the attribute */
                                                },
        // Characteristic 2 Value                  
        [USER_IDX_CHAR2_VALUE]                = {
                                                    { UUID_SIZE_16,USER_CHAR2_UUID },                 /* UUID */
                                                    GATT_PROP_WRITE,                           /* Permissions */                        /* Permissions */
                                                    USER_CHAR1_VALUE_LEN,                                         /* Max size of the value */
                                                    NULL,                                                       /* Value of the attribute */    /* Can assign a buffer here, or can be assigned in the application by user */
                                                },      																				
};

uint16_t crc16_check(uint8_t *p_dat, uint16_t len)
{
    uint16_t i,j;
    uint8_t crc_temp;
    uint16_t crc_cnt = 0xffff;
    static uint16_t crcpoly[2] = {0, 0xa001}; 
    for(j = len; j > 0; j--)
    {
        crc_temp = *p_dat++;
        for(i = 0; i < 8; i++)
        {
            crc_cnt = (crc_cnt >> 1) ^ crcpoly[(crc_cnt ^ crc_temp ) & 1];
            crc_temp = crc_temp >> 1;
        }
    }
    return crc_cnt;
}

void receive_app_decode(uint8_t type, uint8_t *data, uint8_t len);


void user_ble_data_receive(uint8_t *p_buff, uint16_t data_len)
{
	uint16_t crc_val, temp;
	if(data_len>=4)
	{
		temp = p_buff[data_len-1];
		temp = (temp<<8) | p_buff[data_len-2];
		crc_val = crc16_check(p_buff, data_len - 2);
		#if 0
		co_printf("\r\nrx: ");
		for(int i = 0; i < data_len; i++)
		{
			co_printf("%02X ",p_buff[i]);
		}
		co_printf("\r\n crc %x",crc_val);
		#endif
		if((temp == crc_val) && (p_buff[0]==0xF0) && ((data_len-2)==p_buff[1]))
		{
				receive_app_decode(p_buff[2], &p_buff[3], p_buff[1]);
		}
	}
}


void send_app_encode(uint8_t type, uint8_t *p_data, uint8_t data_len) 
{
	#define SEND_DATALEN 64
	static uint8_t tempbuff[SEND_DATALEN]={0};
	uint8_t i=0;
	uint16_t crc_val=0;
	if((data_len+5)>64)return;
	tempbuff[0] = 0xF1;
	tempbuff[1] = data_len + 3;
	tempbuff[2] = type;
	for(i = 0; i<data_len; i++)
	{
		tempbuff[3+i] = p_data[i];
	}
	crc_val = crc16_check(tempbuff, data_len +3);
	tempbuff[3+data_len] = crc_val & 0xff;
	tempbuff[4+data_len] = crc_val >> 8;
	#ifdef DEBUG_DRV_APP
	U_UART_PRINTF("tx:");
	for(i = 0; i < len + 5; i++)
	{
		U_UART_PRINTF("%02X ", send[i]);
	}
	U_UART_PRINTF("\r\n");
	#endif
	user_notify(0,(uint8_t*)tempbuff,data_len + 5);
}

extern uint8_t g_key_code;
/*
帧头	数据长度  数据类型	数据域	CRC16
F0 		04				
*/
void receive_app_decode(uint8_t type, uint8_t *data, uint8_t len)
{
	os_event_t gui_page_event; 
	uint8_t reply_state=0;
	co_printf("%x %d\r\n",type,len);
	switch(type)
	{
		case 0x00:  
		{

		}
		break;
		case 0x01:
		{

		}
		break;
		case 0x10:
		{
					g_key_code=LEFT_CODE;
			    reply_state = 1;
					send_app_encode(type, &reply_state, 1);
		}	
		break;
		case 0x11:
		{
				  g_key_code=RIGHT_CODE;
					reply_state = 1;
					send_app_encode(type, &reply_state, 1);
		}	
		break;
		case 0x12:
		{
					g_key_code=ENTER_CODE;
					reply_state = 1;
					send_app_encode(type, &reply_state, 1);	
		}	
		break;
		case 0x13:  
		{
					g_key_code=DBLCLICK_CODE;
					reply_state = 1;
					send_app_encode(type, &reply_state, 1);	
		}
		break;
		
		case 0x21: //读取运行时间
		{
			char timebuff[16]={0};
			co_sprintf(timebuff,"run:%dmin %dh",(system_get_curr_time()/60000),(system_get_curr_time()/3600000));
			user_notify(0,(uint8_t*)timebuff,strlen(timebuff));
		}
		case 0xE0://读取运行时间秒
		{
		  uint32_t sys_time=system_get_curr_time()/1000;  //得到运行时间秒
			uint8_t sys_temp[4] = {0};
			memcpy(sys_temp,&sys_time,4);
			//user_notify(0xE1, sys_temp, 4);
			send_app_encode(type, sys_temp, 4);	
		}
		break;
		
	}
	
}

void user_notify(uint8_t connect_index,uint8_t *p_data,uint16_t len)
{
        if(user_ntf_enable_flag)
        {
            gatt_ntf_t ntf_att;
            ntf_att.att_idx = USER_IDX_CHAR1_VALUE;
            ntf_att.conidx = connect_index;
            ntf_att.svc_id = user_svc_id;
            ntf_att.data_len = len ;
            ntf_att.p_data = p_data;
            gatt_notification(ntf_att);
					  
        }else{
						co_printf("Notification is not disable\r\n");
		}

	
}
/*********************************************************************
 * @fn      user_gatt_read_cb
 *
 * @brief   Simple Profile user application handles read request in this callback.
 *			应用层在这个回调函数里面处理读的请求。
 *
 * @param   p_read  - the pointer to read buffer. NOTE: It's just a pointer from lower layer, please create the buffer in application layer.
 *					  指向读缓冲区的指针。 请注意这只是一个指针，请在应用程序中分配缓冲区. 为输出函数, 因此为指针的指针.
 *          len     - the pointer to the length of read buffer. Application to assign it.
 *                    读缓冲区的长度，用户应用程序去给它赋值.
 *          att_idx - index of the attribute value in it's attribute table.
 *					  Attribute的偏移量.
 *
 * @return  读请求的长度.
 */
static void user_gatt_read_cb(uint8_t *p_read, uint16_t *len, uint16_t att_idx)
{
    switch (att_idx)
    {
        case USER_IDX_CHAR1_VALUE:
            for (int i = 0; i < USER_CHAR1_VALUE_LEN; i++)
                user_char1_value[i] = user_char1_value[0] + i + 1;
            memcpy(p_read, user_char1_value, USER_CHAR1_VALUE_LEN);
            *len = USER_CHAR1_VALUE_LEN;
        break;

        case USER_IDX_CHAR2_VALUE:
           
       break;
				
				case USER_IDX_CHAR_CFG:
						 //char name_cfg[]={"rotary knob"};
					 *len = 2;
            memcpy(p_read, "\x35\x33", 2);
				break;
        default:
        break;
    }
		//co_printf("\r\n Read request idx:%d \r\n", att_idx);
	  //co_printf("\r\nUser Read request: len: %d  value: 0x%x 0x%x \r\n", *len, (p_read)[0], (p_read)[*len-1]);
    
}

/*********************************************************************
 * @fn      user_gatt_write_cb
 *
 * @brief   Simple Profile user application handles write request in this callback.
 *			应用层在这个回调函数里面处理写的请求。
 *
 * @param   write_buf   - the buffer for write
 *			              写操作的数据.
 *					  
 *          len         - the length of write buffer.
 *                        写缓冲区的长度.
 *          att_idx     - index of the attribute value in it's attribute table.
 *					      Attribute的偏移量.
 *
 * @return  写请求的长度.GATT_PROP_NOTI
 */
static void user_gatt_write_cb(uint8_t *write_buf, uint16_t len, uint16_t att_idx)
{
	co_printf("\r\n Write request idx:%d \r\n", att_idx);
	//co_printf("User Write request: len: %d, 0x%x \r\n", len, write_buf[0]);
		switch(att_idx)
		{
			case USER_IDX_CHAR1_VALUE:
					 //memcpy(user_char1_value, write_buf, len);
					
			 break;
			 case USER_IDX_CHAR2_VALUE:
            memcpy(user_char1_value, write_buf, len);
			      user_ble_data_receive(user_char1_value,len);
       break;
			case USER_IDX_CHAR_CFG:
				   user_ntf_enable_flag = 1;
			     LOG_INFO(NULL,"Notification is enable\r\n");
			break;
		}

	
//	uint16_t uuid = BUILD_UINT16( user_profile_att_table[att_idx].uuid.p_uuid[0], user_profile_att_table[att_idx].uuid.p_uuid[1] );
//	if (uuid == GATT_CLIENT_CHAR_CFG_UUID)
//    {
//		LOG_INFO("Notification status changed\r\n");
////        if (att_idx == USER_IDX_CHAR4_CFG)
////        {
////            user_char4_ccc[0] = write_buf[0];
////            user_char4_ccc[1] = write_buf[1];
////            LOG_INFO("Char4 ccc: 0x%x 0x%x \r\n", user_char4_ccc[0], user_char4_ccc[1]);
////        }
//    }

}

/*********************************************************************
 * @fn      user_gatt_msg_handler
 *
 * @brief   Simple Profile callback funtion for GATT messages. GATT read/write
 *			operations are handeled here.
 *
 * @param   p_msg       - GATT messages from GATT layer.
 *
 * @return  uint16_t    - Length of handled message.
 */
static uint16_t user_gatt_msg_handler(gatt_msg_t *p_msg)
{
    switch(p_msg->msg_evt)
    {
        case GATTC_MSG_READ_REQ:
            user_gatt_read_cb((uint8_t *)(p_msg->param.msg.p_msg_data), &(p_msg->param.msg.msg_len), p_msg->att_idx);
            break;
        
        case GATTC_MSG_WRITE_REQ:
            user_gatt_write_cb((uint8_t*)(p_msg->param.msg.p_msg_data), (p_msg->param.msg.msg_len), p_msg->att_idx);
            break;
				case GATTC_MSG_NTF_REQ:
					 user_ntf_enable_flag=0;
					break;
        default:
            break;
    }
		//co_printf("\r\nuser_gatt_msg_handler evt:%d\r\n",p_msg->msg_evt);
    return p_msg->param.msg.msg_len;
}


void user_gatt_add_service(void)
{
	user_profile_svc.p_att_tb = user_profile_att_table;
	user_profile_svc.att_nb = USER_IDX_NB;
	user_profile_svc.gatt_msg_handler = user_gatt_msg_handler;
	
	user_svc_id = gatt_add_service(&user_profile_svc);
	
	LOG_INFO(NULL,"user_svc_id:%d",user_svc_id);
}
