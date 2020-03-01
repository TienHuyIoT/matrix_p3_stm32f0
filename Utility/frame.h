#ifndef _FRAME_H
#define _FRAME_H
#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"

#define FRAME_LENGHT_MIN      5
#define DATA_LENGHT_MIN       2

enum
{
    START_BYTE = 0x7E,
    STOP_BYTE = 0x7F
};

typedef enum
{
    FRAME_OK = 0,
    FRAME_ERR,
    FRAME_LENGHT_MIN_ERR,
    FRAME_SOF_EOF_ERR,
    FRAME_LENGHT_PACK_ERR,
    FRAME_CRC_ERR
} Frame_Result_Typedef;

typedef struct
{
    uint8_t Start;
    uint8_t Lenght;
    uint8_t Cmd;
    uint8_t *pData;
    uint8_t Crc;
    uint8_t Stop;
} FrameData_Typedef;

void FrameData_Buffer(FrameData_Typedef *FData_t, uint8_t *Output);
void FrameData_Create(FrameData_Typedef *FData_t, uint8_t Cmd, uint8_t *pData, uint8_t DataLenght);
Frame_Result_Typedef FrameData_Parse(FrameData_Typedef *FData_t, uint8_t *pPack, uint8_t PackLenght);

#ifdef __cplusplus
}
#endif
#endif

