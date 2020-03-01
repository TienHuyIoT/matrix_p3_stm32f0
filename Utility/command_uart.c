#include "command_uart.h"
#include "TimeOutEvent.h"
#include "frame.h"

#define TAG														"CMD"
#define COMMAND_UART_DEBUG(f_, ...)   BOARD_NEO_LOG(TAG, f_, ##__VA_ARGS__)
#define CHAR_DEBUG(f_, ...)       		USART1_PRINTF(f_, ##__VA_ARGS__)

typedef enum
{
	COMMAND_START,
	COMMAND_DATA,
	COMMAND_STOP
} get_command_state_typedef;

get_command_state_typedef command_rx_state = COMMAND_START;

#define DATA_LENGHT_NUM					200
#define FRAME_LENGHT_TOTAL			DATA_LENGHT_NUM + FRAME_LENGHT_MIN
uint8_t uart_rx_buff[FRAME_LENGHT_TOTAL];
uint8_t uart_rx_id = 0;
uint8_t uart_rx_lenght = 0;
static TIMEOUT_TypeDef uart_rx_to = TIMEOUT_DEFAULT;
void get_command_uart(void)
{
    uint8_t data;

    if(ToEExpired(&uart_rx_to) == TIMEOUT_STATE_END)
    {
        command_rx_state = COMMAND_START;
    }

    if(Usart1_is_available() == 0)
    {
        return;
    }        

    (void)getchar_usart1((char*)(&data));
    CHAR_DEBUG("%c", data);

    switch (command_rx_state)
    {
    case COMMAND_START:
        if (data == START_BYTE)
        {
            command_rx_state = COMMAND_DATA;
            uart_rx_buff[0] = data;
            uart_rx_id = 1;

            COMMAND_UART_DEBUG("COMMAND_START");

            ToEUpdate(&uart_rx_to, 1000);
        }
        break;
    case COMMAND_DATA:
        if(data < DATA_LENGHT_MIN || data > FRAME_LENGHT_TOTAL)
        {
            command_rx_state = COMMAND_START;
            break;
        }
        COMMAND_UART_DEBUG("COMMAND_DATA");
        command_rx_state = COMMAND_STOP;
        uart_rx_buff[1] = data;
        uart_rx_id = 2;
        uart_rx_lenght = data + 1;
        break;
    case COMMAND_STOP:
        uart_rx_buff[uart_rx_id++] = data;
        if(--uart_rx_lenght == 0)
        {
            COMMAND_UART_DEBUG("COMMAND_STOP");
            command_rx_state = COMMAND_START;
            FrameData_Typedef Rx_Frame_t;
            int Rt = FrameData_Parse(&Rx_Frame_t, uart_rx_buff, uart_rx_buff[1] + 3);
            if (Rt == FRAME_OK)
            {
                COMMAND_UART_DEBUG("Uart Rx Cmd parse OK");
            } 
            else
            {
                COMMAND_UART_DEBUG("Rt %u Crc %02X", Rt, Rx_Frame_t.Crc);
            }              
        }
        break;
    }
}
