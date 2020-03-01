#include "frame.h"

void FrameData_Buffer(FrameData_Typedef *FData_t, uint8_t *Output)
{
    Output[0] = FData_t->Start;
    Output[1] = FData_t->Lenght;
    Output[2] = FData_t->Cmd;
    for(int i = 0; i < (FData_t->Lenght - 2); i++)
    {
        Output[3 + i] = FData_t->pData[i];
    }
    Output[FData_t->Lenght + 1] = FData_t->Crc;
    Output[FData_t->Lenght + 2] = FData_t->Stop;
}

void FrameData_Create(FrameData_Typedef *FData_t, uint8_t Cmd, uint8_t *pData, uint8_t DataLenght)
{
    FData_t->Start = START_BYTE;
    FData_t->Lenght = DataLenght + 2;
    FData_t->Cmd = Cmd;
    FData_t->pData = pData;    
    FData_t->Stop = STOP_BYTE;   
    FData_t->Crc = 0; 
    FData_t->Crc ^= FData_t->Lenght;
    FData_t->Crc ^= FData_t->Cmd;
    for(int i = 0; i < (FData_t->Lenght - 2); i++)
    {
        FData_t->Crc ^= FData_t->pData[i];
    }    
}

Frame_Result_Typedef FrameData_Parse(FrameData_Typedef *FData_t, uint8_t *pPack, uint8_t PackLenght)
{
    if(PackLenght < FRAME_LENGHT_MIN)
    {
        return FRAME_LENGHT_MIN_ERR;
    }
    FData_t->Start = pPack[0];
    FData_t->Lenght = pPack[1];
    FData_t->Cmd = pPack[2];
    FData_t->pData = &pPack[3];    
    FData_t->Stop = pPack[PackLenght - 1];
    FData_t->Crc = pPack[PackLenght - 2];

    if(FData_t->Start != START_BYTE 
        || FData_t->Stop != STOP_BYTE         
        )
    {
        return FRAME_SOF_EOF_ERR;
    }

    if(FData_t->Lenght != (PackLenght - 3))
    {
        return FRAME_LENGHT_PACK_ERR;   
    }

    uint8_t Crc = 0; 
    Crc ^= FData_t->Lenght;
    Crc ^= FData_t->Cmd;
    for(int i = 0; i < (FData_t->Lenght - 2); i++)
    {
        Crc ^= FData_t->pData[i];
    }
    if(Crc != FData_t->Crc)
    {
        FData_t->Crc = Crc;
        return FRAME_CRC_ERR;
    }
    return FRAME_OK;
}
