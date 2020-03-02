#include "command_uart.h"
#include "TimeOutEvent.h"
#include "frame.h"

#define TAG														"CMD"
#define COMMAND_UART_DEBUG(f_, ...)   //BOARD_NEO_LOG(TAG, f_, ##__VA_ARGS__)
#define CHAR_DEBUG(f_, ...)       		//USART1_PRINTF(f_, ##__VA_ARGS__)

typedef enum
{
	COMMAND_START,
	COMMAND_DATA,
	COMMAND_STOP
} get_command_state_typedef;

get_command_state_typedef command_rx_state = COMMAND_START;

#define LINE_CHAR_NUM											1
#define RUN_CHAR_NUM											1
#define ID_CHAR_NUM											  3
#define DATA_LENGHT_NUM										200
#define DATA_LENGHT_OF_FRAME_NUM					(DATA_LENGHT_NUM + ID_CHAR_NUM + RUN_CHAR_NUM + LINE_CHAR_NUM)
#define FRAME_LENGHT_TOTAL			          (DATA_LENGHT_OF_FRAME_NUM + FRAME_LENGHT_MIN)

#define START_LENGHT											1
#define STOP_LENGHT											  1
#define LENGHT_FIELD_NUM									1
uint8_t uart_rx_buff[FRAME_LENGHT_TOTAL + START_LENGHT + STOP_LENGHT + LENGHT_FIELD_NUM];
uint8_t uart_rx_id = 0;
uint8_t uart_rx_lenght = 0;

#define TIMEOUT_COMMAND_NUM					2000
static TIMEOUT_TypeDef uart_rx_to = TIMEOUT_DEFAULT;

update_str_panel_ptr	p_update_str_panel_cb = 0;

void command_register_callback(update_str_panel_ptr p_callback)
{
	p_update_str_panel_cb = p_callback;
}

void get_command_uart(void)
{
    uint8_t data;
		static uint8_t get_cnt = 0;

    if(ToEExpired(&uart_rx_to) == TIMEOUT_STATE_END)
    {
        command_rx_state = COMMAND_START;
				// COMMAND_UART_DEBUG("Timeout command\r\n");
    }

		get_cnt = 50;
    while((Usart1_is_available() != 0) && (get_cnt--))
    {
        (void)getchar_usart1((char*)(&data));    

				switch (command_rx_state)
				{
				case COMMAND_START:
						if (data == START_BYTE)
						{
								command_rx_state = COMMAND_DATA;
								uart_rx_buff[0] = data;
								uart_rx_id = 1;

								COMMAND_UART_DEBUG("START");

								ToEUpdate(&uart_rx_to, TIMEOUT_COMMAND_NUM);
						}
						break;
				case COMMAND_DATA:
						if(data < DATA_LENGHT_MIN || data > FRAME_LENGHT_TOTAL)
						{
								command_rx_state = COMMAND_START;
								break;
						}        
						command_rx_state = COMMAND_STOP;
						uart_rx_buff[1] = data;
						uart_rx_id = 2;
						uart_rx_lenght = (uint8_t)data + STOP_LENGHT;
						COMMAND_UART_DEBUG("DATA lenght %u\r\n", uart_rx_lenght);
						break;
				case COMMAND_STOP:
						uart_rx_buff[uart_rx_id++] = data;
						CHAR_DEBUG("%c", data);
						if(--uart_rx_lenght == 0)
						{
								COMMAND_UART_DEBUG("STOP");
								command_rx_state = COMMAND_START;
								FrameData_Typedef Rx_Frame_t;
								int Rt = FrameData_Parse(&Rx_Frame_t, uart_rx_buff, uart_rx_buff[1] + 3);
								command_error_typdef error_type;
								if (Rt == FRAME_OK)
								{
										COMMAND_UART_DEBUG("Uart Rx Cmd parse OK");
										error_type = COMMAND_SUCCESS;
								} 
								else if(Rt == FRAME_CRC_ERR)
								{
										COMMAND_UART_DEBUG("Rt %u Crc %02x", (uint8_t)Rt, Rx_Frame_t.Crc);
										error_type = COMMAND_CRC_OF_FRAME_ERROR;
								}  
								else if(Rt == FRAME_LENGHT_PACK_ERR)
								{
										COMMAND_UART_DEBUG("Rt %u Lenght %u", Rt, Rx_Frame_t.Lenght);
										error_type = COMMAND_LENGHT_ERROR;
								}
								else
								{
										error_type = COMMAND_OF_FRAME_ERROR;
										COMMAND_UART_DEBUG("Rt %u", Rt);
								}
								
								if(p_update_str_panel_cb !=0)
								{
									Rx_Frame_t.pData[Rx_Frame_t.Lenght - 2] = 0;	// add null pointer
									p_update_str_panel_cb((char*)Rx_Frame_t.pData, error_type);
								}
						}
						break;
				}
    }            
}
