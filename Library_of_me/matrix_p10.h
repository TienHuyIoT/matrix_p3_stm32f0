#ifndef __MATRIX_H__
#define __MATRIX_H__

#include <defines.h>
//Khai bao ma tran
#define MT_row					1
#define MT_col					1
#define MT_pixel_one_row	64
#define MT_pixel_one_col	64

//Mac dinh
#define MT_line_row			MT_row*MT_pixel_one_row
#define MT_line_col			MT_col*MT_pixel_one_col
#define MT_byte_col			MT_line_col/8



/******************************Phan khai bao cho hieu ung ************************************/
/*****************************************************************************************/
void shift_data_595(unsigned char d1, unsigned char d2, unsigned char d3);
void latch_data(void);
void select_row(unsigned char n);

void MT_perodic_poll(void);
void MT_poll(void);

void MT_puts(char row, char *str);

#endif  //__MATRIX_H__
