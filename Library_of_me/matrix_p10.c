
#include <matrix_p10.h>
#include "stdio.h"
#include "stdint.h"
#include <string.h>
#include <FONT_16.c>
#include <Font_H32.c>



//#include "Printf_GetString.h"

char MT_data_out[MT_line_row][MT_byte_col + 1];

//==========Cac ham giao tiep 595/SPI====================
void shift_data_595(unsigned char d1, unsigned char d2, unsigned char d3)
{
	unsigned char k;

	for (k = 7; k < 8; k--)
	{
		CLK(0);
		//COM1
		if (d1 & (0x01 << k))
			R0_on;
		else
			R0_off;
		//COM2
		if (d2 & (0x01 << k))
			R1_on;
		else
			R1_off;
		//COM3
		if (d3 & (0x01 << k))
			R2_on;
		else
			R2_off;
		CLK(1);
		CLK(1);
	}
}
void select_row(unsigned char n)
{
	A(n % 2);
	B((n / 2) % 2);
	C((n / 4) % 2);
	D((n / 8) % 2);
	E((n / 16) % 2);
}
void latch_data(void)
{
	LAT(1);
	LAT(1);
	LAT(0);
}
//==============Cac ham phu tro==========================
//Ham copy data - font 16x16
void MT_get_dot_from_font_16(char row, char *row_str)
{
	uint8_t i, j, k, x, y, z, len, width_pixel;
	char c, buff_temp[16];

	len = 0;
	y = 0;
	z = 0;
	i = 0;
	for (k = 0; k < 16; k++)
		buff_temp[k] = 0;
	while (i < strlen(row_str))
	{

		//kiem tra do dai chuoi
		c = row_str[i];
		width_pixel = font16x16[c].Width;
		len += width_pixel;
		if (len > MT_line_col)
		{ //neu vuoc qua chieu dai leb thi copy du lieu ra => ket thuc
			for (k = 0; k < 16; k++)
			{
				MT_data_out[k][z] = buff_temp[k];
				buff_temp[k] = 0x00;
			}
			break;
		}

		//sao chep vao du lieu tam
		for (j = 0; j < font16x16[c].Width_bytes; j++)
		{
			for (x = 0; x < 8; x++)
			{
				// printf_1("\r%2d:%2d-%2d:", x, y, width_pixel);
				for (k = 0; k < 16; k++)
				{
					if (font16x16[c].code[k * font16x16[c].Width_bytes + j] & (0x80 >> x))
					{
						buff_temp[k] = buff_temp[k] | (0x80 >> y);
						// printf_1("X");
					}
					else
					{
						// printf_1(".");
					}
				}
				y++;
				width_pixel--;
				if (y == 8)
				{
					y = 0;
					for (k = 0; k < 16; k++)
					{
						MT_data_out[row * MT_pixel_one_row + k][z] = buff_temp[k];
						buff_temp[k] = 0x00;
					}
					z++;
				}
				if (width_pixel == 0)
				{
					i++;
					break;
				}
			}
			if (width_pixel == 0)
			{
				break;
			}
		}
	}
}
//=========Cac ham bat buoc bo sung =====================
//Dua vao timer, toc do cao
void MT_perodic_poll(void)
{
	unsigned char d1, d2, d3;
	uint16_t k, m;
	static uint16_t MT_scan_row = 0;

	OE(1);
	select_row(MT_scan_row);

	//xuat du lieu ra mang hinh
	for (m = 0; m < MT_byte_col; m++)
	{
		// for (k = 15; k < 4; k--)
		// {
			d1 = MT_data_out[MT_scan_row][m];
			// d2 = MT_data_out[k * 16 + 16 + MT_scan_row][m];
			// d3 = MT_data_out[k * 16 + 32 + MT_scan_row][m];
			shift_data_595(d1, 0x00, 0x00);
		// }
	}

	OE(0);
	latch_data();

	//muon chay cham thi chi can dich data hien co nhieu lan hon
	if (++MT_scan_row == 32)
	{ //Neu hang ve 0 nghia la da dich du lieu het so_hang
		MT_scan_row = 0;
	}
}

//==============Cac ham chuc nang =======================

void MT_puts(char row, char *str)
{
	uint8_t i, j;
	char buff_temp[strlen(str)+1];

	j = 0;
	for (i = 0; i < strlen(str); i++)
	{
		if (str[i] == 0xFF)
			str[i] = ' ';
		buff_temp[j++] = str[i];
		if (str[i] != '.' && str[i + 1] != '.')
		{
			buff_temp[j++] = '_'; //them dau canh vao dung yeu cau hien thi
		}
		if (j >= 12)
			break; //lon hon 12 = (6 so + 6 dau '.') thi bo ngoai
	}
	buff_temp[j] = 0;

	//tinh toan do dai chuoi
	j = 0;
	for (i = 0; i < strlen(buff_temp); i++)
	{
		j += font16x16[buff_temp[i]].Width;
	}

	/*nếu độ dài của dữ liệu độ dài hiển thị*/
	if (MT_line_col - j > 0)
	{
		j = (MT_line_col - j) / 2;
		for (i = strlen(buff_temp); i <= strlen(buff_temp); i--)
		{
			buff_temp[i + j] = buff_temp[i];
		}
		for (i = j - 1; i < j; i--)
		{
			buff_temp[i] = '_';
		}
	}
	//	printf_1("row%d:%s\r",row,buff_temp);
	MT_get_dot_from_font_16(row, buff_temp);
	//	MT_perodic_poll();
}

void matrix_send(uint8_t line, char *data){
	
}
