/*
 *********************************************************************************************************
 *                                            br16
 *                                            btstack
 *                                             CODE
 *
 *                          (c) Copyright 2016-2016, ZHUHAI JIELI
 *                                           All Rights Reserved
 *
 * File : *
 * By   : jamin.li
 * DATE : 2016-04-12 10:17AM    build this file
 *********************************************************************************************************
 */
#include "typedef.h"
//#include "error.h"
#include "sdk_cfg.h"
#include "common/msg.h"
#include "common/app_cfg.h"
#include "bluetooth/le_profile_def.h"
#include "bluetooth/le_server_module.h"
#include "rtos/os_api.h"

#include "rtos/task_manage.h"
#include "rcsp/rcsp_interface.h"
#include "rcsp/rcsp_head.h"


#if(BLE_BREDR_MODE&BT_BLE_EN)


static uint16_t server_send_handle;//ATT 发送handle

uint16_t ble_conn_handle;//设备连接handle
static u8 ble_mutex_flag;// ble与spp 互斥标记,0:表示ble可连接，1：表示ble不可连接

static u32 app_data_send(u8 *data,u16 len);
static void app_data_recieve(u8 *data, u16 len);

void app_server_le_msg_callback(uint16_t msg, uint8_t *buffer, uint16_t buffer_size)
{
    printf("\n-%s, msg= 0x%x\n",__FUNCTION__,msg);
    switch(msg)
    {
    case BTSTACK_LE_MSG_CONNECT_COMPLETE:
        ble_conn_handle = buffer[0]+ (buffer[1]<<8);
        printf("conn_handle= 0x%04x\n",ble_conn_handle);

#if SUPPORT_APP_RCSP_EN
        rcsp_event_com_start(RCSP_APP_TYPE_BLE);
        rcsp_register_comsend(app_data_send);
#endif // SUPPORT_APP_RCSP_EN

		break;

    case BTSTACK_LE_MSG_DISCONNECT_COMPLETE:
        printf("disconn_handle= 0x%04x\n",buffer[0]+ (buffer[1]<<8));
        server_send_handle = 0;

        if(ble_conn_handle != 0)
        {
            ble_conn_handle = 0;

#if SUPPORT_APP_RCSP_EN
            rcsp_event_com_stop();
#endif // SUPPORT_APP_RCSP_EN

			if(ble_mutex_flag == 0)
            {
                server_advertise_enable();
            }
        }

        break;

	case BTSTACK_LE_MSG_INIT_COMPLETE:
        puts("init server_adv\n");
		server_advertise_enable();
		break;

    default:
        break;
    }

    puts("exit_le_msg_callback\n");
}


// ATT Client Write Callback for Dynamic Data
// @param attribute_handle to be read
// @param buffer
// @param buffer_size
// @return 0
uint16_t app_server_write_callback(uint16_t attribute_handle, uint8_t * buffer, uint16_t buffer_size)
{

    u16 handle = attribute_handle;
    u16 copy_len;

#if 0
    if(buffer_size > 0)
    {
        printf("\n write_cbk: handle= 0x%04x",handle);
        put_buf(buffer,buffer_size);
    }
#endif

    switch (handle)
    {
    case ATT_CHARACTERISTIC_ae02_01_CLIENT_CONFIGURATION_HANDLE:
        if(buffer[0] != 0)
        {
            server_send_handle = handle -1;
        }
        else
        {
            server_send_handle = 0;
        }
        break;


    case ATT_CHARACTERISTIC_ae01_01_VALUE_HANDLE:

        /* printf("\n--write, %d\n",buffer_size); */
        /* put_buf(buffer,buffer_size); */
		app_data_recieve(buffer,buffer_size);

        break;

    default:
        break;
    }

	return 0;
}

//ble send data
static u32 app_data_send(u8 *data,u16 len)
{
    int ret_val;

	if(server_send_handle == NULL)
	{
	    return 4;// is disconn
	}

    ret_val = server_notify_indicate_send(server_send_handle,data,len);

    if(ret_val !=  0)
    {
        puts("\n app_ntfy_fail!!!\n");
        return 4;//disconn
    }
    return 0;
}

//ble recieve data
static void app_data_recieve(u8 *data, u16 len)
{
#if SUPPORT_APP_RCSP_EN
   rcsp_comdata_recieve(data,len);
#endif // SUPPORT_APP_RCSP_EN

} 


void app_server_init(void)
{
    printf("\n%s\n",__FUNCTION__);
    server_register_profile_data(profile_data);
    server_register_app_callbak(app_server_le_msg_callback,NULL,app_server_write_callback);
	ble_mutex_flag = 0;
}

/*
spp 和 ble 互斥连接
1、当spp 连接后，ble变为不可连接
2、当ble连上后，若有spp连接上，则断开ble；ble变为不可连接
 */
void ble_enter_mutex(void)
{
	P_FUNCTION


    if(ble_mutex_flag == 1)
	{
		return;
	}

	puts("in enter mutex\n");
    ble_mutex_flag = 1;
    if(ble_conn_handle != 0)
    {
        ble_hci_disconnect(ble_conn_handle);
        rcsp_event_com_stop();
    }
    else
    {
        server_advertise_disable();
    }
    puts("exit_enter_mu\n");
}

void ble_exit_mutex(void)
{
    P_FUNCTION

	if(ble_mutex_flag == 0)
	{
		return;
	}
   
	puts("in_exit mutex\n");
	ble_mutex_flag = 0;
   	server_advertise_enable();
   	puts("exit_exit_mu\n");
}

void ble_server_disconnect(void)
{
    P_FUNCTION
    if(ble_conn_handle != 0)
    {
        printf("server discon handle= 0x%x\n ",ble_conn_handle); 
		ble_hci_disconnect(ble_conn_handle);
    }
    puts("exit_discnt\n");
}



#endif //#if (BLE_BREDR_MODE&BT_BLE_EN)
