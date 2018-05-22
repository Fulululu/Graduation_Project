#include "OSAL.h"
#include "ZGlobals.h"
#include "AF.h"
#include "aps_groups.h"
#include "ZDApp.h"

#include "SampleApp.h"
#include "SampleAppHw.h"

#include "OnBoard.h"  //incorporate har_uart.h 

//#include <stdio.h>
#include "IIC.h"
#include "BH1750FVI.h"
#include "SHT10.h"
#include "PMS7003.h"
#include "DS_CO2_20.h"
#include "Soilhumi.h"
#include "ds18b20.h"
#include "ZE03.h"

/* HAL */
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "MT_UART.h"
#include "MT_APP.h"
#include "MT.h"
#include "hal_adc.h"
#include "hal_uart.h"

/*********************************************************************
 * MACROS
 */
#if defined( ZDO_COORDINATOR ) 
#define RELAY P0_4
#endif

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

uint8 LedState = 0;

// This list should be filled with Application specific Cluster IDs.
const cId_t SampleApp_ClusterList[SAMPLEAPP_MAX_CLUSTERS] =
{
  SAMPLEAPP_BROADCAST_CLUSTERID,
  SAMPLEAPP_P2P_CLUSTERID
};

const SimpleDescriptionFormat_t SampleApp_SimpleDesc =
{
  SAMPLEAPP_ENDPOINT,              //  int Endpoint;
  SAMPLEAPP_PROFID,                //  uint16 AppProfId[2];
  SAMPLEAPP_DEVICEID,              //  uint16 AppDeviceId[2];
  SAMPLEAPP_DEVICE_VERSION,        //  int   AppDevVer:4;
  SAMPLEAPP_FLAGS,                 //  int   AppFlags:4;
  SAMPLEAPP_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)SampleApp_ClusterList,  //  uint8 *pAppInClusterList;
  SAMPLEAPP_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)SampleApp_ClusterList   //  uint8 *pAppInClusterList;
};

// This is the Endpoint/Interface description.  It is defined here, but
// filled-in in SampleApp_Init().  Another way to go would be to fill
// in the structure here and make it a "const" (in code space).  The
// way it's defined in this sample app it is define in RAM.
endPointDesc_t SampleApp_epDesc;

/*********************************************************************
 * EXTERNAL VARIABLES
 */
//20171108 add rxLen and MT_RX_Buffer
extern uint8 MT_RX_Buffer[];
extern uint16 rxLen;
extern uint8 frame[];
/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
uint8 SampleApp_TaskID;   // Task ID for internal task/event processing
                          // This variable will be received when
                          // SampleApp_Init() is called.
devStates_t SampleApp_NwkState;

uint8 SampleApp_TransID;  // This is the unique message ID (counter)

afAddrType_t SampleApp_Broadcast_DstAddr; //广播描述符
afAddrType_t SampleApp_Group_DstAddr;    //组播描述符
afAddrType_t SampleApp_P2P_DstAddr;      //点播描述符

aps_Group_t SampleApp_Group;

uint8 myRXbuf[2];
uint8 len = 0;
uint8 lightbuf[5];
            
/*********************************************************************
 * LOCAL FUNCTIONS
 */
void SampleApp_HandleKeys( uint8 shift, uint8 keys );
void SampleApp_coord_HandleMsg( afIncomingMSGPacket_t *pckt );
void SampleApp_router_HandleMsg( afIncomingMSGPacket_t *pckt );
void SampleApp_end_HandleMsg( afIncomingMSGPacket_t *pckt );

void SampleApp_coord_HandlerUART(void);
void SampleApp_Send_P2P_Message(void);
uint8 Soilhumi_Read_Str(uint8 *tmp);


/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      SampleApp_Init
 *
 * @brief   Initialization function for the Generic App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by OSAL.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
void SampleApp_Init( uint8 task_id )
{ 
  SampleApp_TaskID = task_id;	        //保存OSAL分配的任务ID
  SampleApp_NwkState = DEV_INIT;	//设备网络状态设置为初始化网络状态初始化应用设备的网络类型
  SampleApp_TransID = 0;	        //设置消息发送ID
  
  //判断该设备是协调器 还是 路由器
#if defined ( BUILD_ALL_DEVICES )
  if ( readCoordinatorJumper() )
    zgDeviceLogicalType = ZG_DEVICETYPE_COORDINATOR;
  else
    zgDeviceLogicalType = ZG_DEVICETYPE_ROUTER;
#endif
  
  //1、初始化端点描述符（不同端点应不同）并向AF层注册(端点不是终端相当于端口号)
	  // Fill out the endpoint description.
  SampleApp_epDesc.endPoint = SAMPLEAPP_ENDPOINT;
  SampleApp_epDesc.task_id = &SampleApp_TaskID;
  SampleApp_epDesc.simpleDesc
            = (SimpleDescriptionFormat_t *)&SampleApp_SimpleDesc;
  SampleApp_epDesc.latencyReq = noLatencyReqs;		
	  // Register the endpoint description with the AF
  afRegister( &SampleApp_epDesc );   

  //2、设置af发送的地址描述符（包含数据发送寻址模式及目的地址信息）
  SampleApp_Group_DstAddr.addrMode = (afAddrMode_t)afAddrGroup;	  //组寻址
  SampleApp_Group_DstAddr.endPoint = SAMPLEAPP_ENDPOINT;	  //指定端点号
  SampleApp_Group_DstAddr.addr.shortAddr = SAMPLEAPP_GROUP_ADDR;    //组地址
  
  SampleApp_Broadcast_DstAddr.addrMode = (afAddrMode_t)AddrBroadcast;//广播寻址
  SampleApp_Broadcast_DstAddr.endPoint = SAMPLEAPP_ENDPOINT;	//指定端点号
  SampleApp_Broadcast_DstAddr.addr.shortAddr = 0xFFFF;	        //广播地址 
  
  SampleApp_P2P_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;     //点播寻址 
  SampleApp_P2P_DstAddr.endPoint = SAMPLEAPP_ENDPOINT;          //指定端点号
  SampleApp_P2P_DstAddr.addr.shortAddr = 0;                     //不指定
  
  //3、初始化组描述符
  SampleApp_Group.ID = SAMPLEAPP_GROUP_ADDR;	        //组号
  osal_memcpy(SampleApp_Group.name, "Group 111", 9);	//组名
  //4、将端点加入到组中
  aps_AddGroup(SAMPLEAPP_ENDPOINT,&SampleApp_Group);
  
  //------------------------配置按键---------------------------------
    // Register for all key events - This app will handle all key events
  RegisterForKeys( SampleApp_TaskID );
  //------------------------配置串口---------------------------------
  MT_UartInit();                    //串口初始化
  MT_UartRegisterTaskID(task_id);   //注册串口任务
  HalUARTWrite(HAL_UART_PORT_0,"UartInit OK\n", sizeof("UartInit OK\n"));
  //------------------------配置GPIO---------------------------------
  #if defined( ZDO_COORDINATOR ) 
    P0SEL &= ~0xF0;                 //设置P0.4 P0.5 P0.6 P0_7为普通IO口
    P0DIR |= 0xF0;                 //P0.4 P0.5 P0.6 P0.7定义为输出口
    RELAY = 1;  
  #else
  //20171025 add iic and bh1750  
    I2C_Init();
    BH1750_Init();
  //20171107 add PMS7003
    //PMS7003_Init();
  //20171202 add DS_CO2_20  
    //DS_CO2_20_Init();
  //20171203 add ZE03
    //ZE03_Init();
  //20171202 add soilhumi
    Soilhumi_Init();
  #endif
}

/*********************************************************************
 * @fn      SampleApp_ProcessEvent
 *
 * @brief   Generic Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  none
 */
uint16 SampleApp_ProcessEvent( uint8 task_id, uint16 events )
{
  afIncomingMSGPacket_t *MSGpkt;
  (void)task_id;  // Intentionally unreferenced parameter
  //uint8 addrTemp[2];
  
  if ( events & SYS_EVENT_MSG )
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( SampleApp_TaskID );
    while ( MSGpkt )
    {
      switch ( MSGpkt->hdr.event )
      {
        // Received when a key is pressed
        case KEY_CHANGE:
          SampleApp_HandleKeys( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
          break;

        // Received when a messages is received (OTA) for this endpoint
        case AF_INCOMING_MSG_CMD:
          #if defined( ZDO_COORDINATOR ) 
            SampleApp_coord_HandleMsg( MSGpkt );
          #elif defined( RTR_NWK )
            SampleApp_router_HandleMsg( MSGpkt );
          #else
            SampleApp_end_HandleMsg( MSGpkt );
          #endif
          break;

        // Received whenever the device changes state in the network
        case ZDO_STATE_CHANGE:
          SampleApp_NwkState = (devStates_t)(MSGpkt->hdr.status);
          if ( (SampleApp_NwkState == DEV_ZB_COORD) ||  
                 (SampleApp_NwkState == DEV_ROUTER)
              || (SampleApp_NwkState == DEV_END_DEVICE) )
          { 
            // Setup to send message again in normal period (+ a little jitter)
          #if defined( ZDO_COORDINATOR )
            osal_start_timerEx( SampleApp_TaskID, 
                              SAMPLEAPP_SEND_PERIODIC_MSG_EVT,
                              (SAMPLEAPP_SEND_PERIODIC_MSG_TIMEOUT_COORD + (osal_rand() & 0x00FF)) );
          #else
            SampleApp_Send_P2P_Message();
            osal_start_timerEx( SampleApp_TaskID,
                              SAMPLEAPP_SEND_PERIODIC_MSG_EVT,
                              SAMPLEAPP_SEND_PERIODIC_MSG_TIMEOUT );
          #endif
                  
          }
          else
          {
            // Device is no longer in the network
          }
          break;

        default:
          break;
      }

      // Release the memory
      osal_msg_deallocate( (uint8 *)MSGpkt );

      // Next - if one is available
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( SampleApp_TaskID );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  // Send a message out - This event is generated by a timer
  //  (setup in SampleApp_Init()).
  if ( events & SAMPLEAPP_SEND_PERIODIC_MSG_EVT )
  { 
    #if defined( ZDO_COORDINATOR )
      //something to do
    #elif defined( RTR_NWK )
      //something to do
    #else
      SampleApp_Send_P2P_Message();    // 终端或路由器周期处理函数   

    // Setup to send message again in normal period (+ a little jitter)
    osal_start_timerEx( SampleApp_TaskID, SAMPLEAPP_SEND_PERIODIC_MSG_EVT,
        (SAMPLEAPP_SEND_PERIODIC_MSG_TIMEOUT + (osal_rand() & 0x00FF)) );
    #endif
    // return unprocessed events
    return (events ^ SAMPLEAPP_SEND_PERIODIC_MSG_EVT);
  }
  
  //20171108 add uart event
  if ( events & UART_RX_CB_EVT )
  {
    #if defined( ZDO_COORDINATOR ) 
      SampleApp_coord_HandlerUART();
    #elif defined( RTR_NWK )
      //something to do
    #else
      //something to do
    #endif
    // return unprocessed events
    return (events ^ UART_RX_CB_EVT);

  }

  // Discard unknown eventss
  return 0;
}

/*********************************************************************
 * Event Generation Functions
 */
/*********************************************************************
 * @fn      SampleApp_HandleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events. Valid entries:
 *                 HAL_KEY_SW_6(S1)
 *                 HAL_KEY_SW_7(S2)
 *
 * @return  none
 */
void SampleApp_HandleKeys( uint8 shift, uint8 keys )
{
  (void)shift;  // Intentionally unreferenced parameter
  
  if ( keys & HAL_KEY_SW_6 )
  {   
  }
  if ( keys & HAL_KEY_SW_7 )
  {
  }

}

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * @fn      SampleApp_coord_HandleMsg
 *
 * @brief   Data message processor callback.  This function processes
 *          any incoming data - probably from other devices.  So, based
 *          on cluster ID, perform the intended action.
 *
 * @param   none
 *
 * @return  none
 */
void SampleApp_coord_HandleMsg( afIncomingMSGPacket_t *pkt )
{ 
  switch ( pkt->clusterId )
  {
    case SAMPLEAPP_P2P_CLUSTERID: 
      HalUARTWrite(0, pkt->cmd.Data, pkt->cmd.DataLength);    //输出接收到的数据
      break;    

    case SAMPLEAPP_BROADCAST_CLUSTERID:    
      break;
    
    default:
      break;
  }
}



/*********************************************************************
 * @fn      SampleApp_router_HandleMsg
 *
 * @brief   Data message processor callback.  This function processes
 *          any incoming data - probably from other devices.  So, based
 *          on cluster ID, perform the intended action.
 *
 * @param   none
 *
 * @return  none
 */
void SampleApp_router_HandleMsg( afIncomingMSGPacket_t *pkt )
{
  switch ( pkt->clusterId )
  {
    case SAMPLEAPP_P2P_CLUSTERID:
      if( pkt->cmd.Data[0] == 'L'  )
      {
      }
      else if( pkt->cmd.Data[0] == 'D' )
      { 
      }
      break;
      
    case SAMPLEAPP_BROADCAST_CLUSTERID:
      break;
    
    default:
      break;
  }
}


/*********************************************************************
 * @fn      SampleApp_end_HandleMsg
 *
 * @brief   Data message processor callback.  This function processes
 *          any incoming data - probably from other devices.  So, based
 *          on cluster ID, perform the intended action.
 *
 * @param   none
 *
 * @return  none
 */
void SampleApp_end_HandleMsg( afIncomingMSGPacket_t *pkt )
{
  switch ( pkt->clusterId )
  {
    case SAMPLEAPP_P2P_CLUSTERID:
      if( pkt->cmd.Data[0] == 'L'  )
      {
      }
      else if( pkt->cmd.Data[0] == 'D' )
      { 
      }
      break;

    case SAMPLEAPP_BROADCAST_CLUSTERID:
      break;
    
    default:
      break;
  }
}


/*********************************************************************
 * @fn      SampleApp_coord_HandlerUART
 *
 * @brief   Handle the message from UART.
 *
 * @param   none
 *
 * @return  none
 */
void SampleApp_coord_HandlerUART(void)
{
  HalUARTWrite ( HAL_UART_PORT_0, MT_RX_Buffer , rxLen ); // display back for test
}


/*********************************************************************
 * @fn      SampleApp_Send_P2P_Message
 *
 * @brief   point to point.(cost about 3.5s)
 *
 * @param   none
 *
 * @return  none
 */
void SampleApp_Send_P2P_Message( void )
{ 
  static char deviceID = 1;                          //终端节点设备号
  unsigned char frame[14];
  unsigned char light[2],air_temp_humi[5],soil_temp_humi[5];
  
  osal_memset(light,'\0',sizeof(light));
  osal_memset(air_temp_humi,'\0',sizeof(air_temp_humi));
  osal_memset(soil_temp_humi,'\0',sizeof(soil_temp_humi));
  osal_memset(frame,'\0',sizeof(frame));

  BH1750_Read(light);                       //2byte
  SHT10_Read(air_temp_humi);                //4+1byte
  Ds18B20_Readfloat(soil_temp_humi);        //4byte
  Soilhumi_Read(soil_temp_humi+4);          //1byte
    
//将数据整合后方便发给协调器显示
  frame[0] = deviceID;
  osal_memcpy(&frame[1], light, 2); 
  osal_memcpy(&frame[3], air_temp_humi, 5);
  osal_memcpy(&frame[8], soil_temp_humi, 5);
  frame[13] = '\n';
    
//发给协调器
  SampleApp_P2P_DstAddr.addr.shortAddr = 0x0000;             
  if ( AF_DataRequest( &SampleApp_P2P_DstAddr, &SampleApp_epDesc,
                       SAMPLEAPP_P2P_CLUSTERID,
                       13,
                       (uint8 *)frame,
                       &SampleApp_TransID,
                       AF_DISCV_ROUTE,
                       AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
  {
    //do nothing
    deviceID++;
    if(deviceID == 4)
      deviceID = 1;
  }
  else
  {
    // do nothing error occurred .
  }
}

//20171108 add uartcallback
void MT_UartRXCB ( uint8 port, uint8 event )
{
  rxLen  = Hal_UART_RxBufLen(HAL_UART_PORT_0);
  
  if ((event & (HAL_UART_RX_FULL | HAL_UART_RX_ABOUT_FULL | HAL_UART_RX_TIMEOUT)))
  {   
    HalUARTRead( HAL_UART_PORT_0, MT_RX_Buffer, rxLen );
    osal_set_event(SampleApp_TaskID, UART_RX_CB_EVT);
  }
}


/*********************************************************************
*********************************************************************/