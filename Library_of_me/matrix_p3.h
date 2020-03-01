#ifndef __MATRIX_P3_H__
#define __MATRIX_P3_H__

#include <string.h>

#include <defines.h>
#include "my_printf.h"

/* QUY ĐỊNH CHUNG
row: là số pixel được tính theo hàng ngan
col: là số pixel được tính theo hàng dọc
line: là số dòng chữ được hiển thị trên 1 bản led
*/
//Khai bao ma tran
#define PIXEL_ROW 64	//64 điểm ảnh trên 1 dòng
#define PIXEL_COL 64	//64 điểm ảnh trên 1 cột
#define TABLE_ROW_NUM 2 //số bảng led đươc ghép theo hàng ngan
#define TABLE_COL_NUM 3 //số bảng led được ghép theo hàng dọc

#define PIXEL_ROW_OF_LINE 32						  //1 chân data xuất ra điều khiển 32 row là 1 line
#define NUM_BYTE_OF_LINE PIXEL_ROW_OF_LINE / 8		  // số byte trên hàng ngan của pixel
#define ALL_BYTE_OF_LINE PIXEL_COL *TABLE_COL_NUM / 8 // tổng số byte trên hàng ngan của pixel

uint8_t font_get_lenght_pixel(char *data);

void matrix_refesh(uint8_t line);
void matrix_send(uint8_t line, char *data, uint8_t pause_run);
void matrix_process(void);
void matrix_scan_show(void);

#endif //__MATRIX_P3_H__
/*THE END*/
