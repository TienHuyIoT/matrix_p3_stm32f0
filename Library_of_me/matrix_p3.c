
#include "matrix_p3.h"
#include "Font_H32.c"

/*____________XỬ LÝ FONT___________*/
#define FONT_HIGHT 32
#define INDEX_START_WIDTH 4
#define INDEX_TOTAL_BYTE 96
#define INDEX_START_DATA 100
#define BYTE_FIRST_TABLE ' '

static uint8_t row_of_line = PIXEL_ROW_OF_LINE;

static uint8_t row_update_enable = 0;

uint8_t font_get_location_lenght(uint8_t Byte_data, uint16_t *location, uint16_t *width)
{
	uint16_t byte_cnt = 0;
	uint16_t location_font = 0;

	Byte_data -= BYTE_FIRST_TABLE;

	/*kiểm tra lỗi ngoài bảng mã font*/
	if (Byte_data > INDEX_TOTAL_BYTE)
		return 0;
	*location = INDEX_START_DATA;
	for (byte_cnt = 0; byte_cnt < Byte_data; byte_cnt++)
	{
		location_font = (arial[INDEX_START_WIDTH + byte_cnt] / 8 + ((arial[INDEX_START_WIDTH + byte_cnt] % 8) ? 1 : 0)) * FONT_HIGHT;
		*location += location_font;
	}
	*width = arial[INDEX_START_WIDTH + byte_cnt];
	return 1;
}
uint8_t font_get_lenght_pixel(char *data)
{
	uint8_t i;
	uint16_t width_font, location_font;
	uint16_t pixel_num = 0;

	for (i = 0; i < strlen(data); i++)
	{
		font_get_location_lenght(data[i], &location_font, &width_font);
		pixel_num += width_font;
	}
	if (pixel_num > PIXEL_COL * TABLE_COL_NUM)
		return 0;
	else
		return 1;
}
/*____________XỬ LÝ XUẤT DATA___________*/
void matrix_shift_data(uint8_t d1, uint8_t d2)
{
	uint8_t k;

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
		CLK(1);
		CLK(1);
	}
}
void matrix_select_line(uint8_t n)
{
	A(n % 2);
	B((n / 2) % 2);
	C((n / 4) % 2);
	D((n / 8) % 2);
	E((n / 16) % 2);
}
void matrix_latch_data(void)
{
	LAT(1);
	LAT(1);
	LAT(0);
}
/*_________XỬ LÝ DỮ LIỆU__________*/
struct
{
	uint8_t flag_data : 1; //0: không có dữ liệu mới, 1: có dữ liệu mới
	char string[201];
	uint8_t id_string;	//vị trí đang xử lý của chuổi string
	uint8_t id_pixel_col; //vị trí cột pixel đang xử lý
	uint8_t pause_run;	//0: pause, 1: run
	uint8_t one_byte[PIXEL_ROW_OF_LINE][NUM_BYTE_OF_LINE];
	uint16_t width_byte;
	uint8_t encryption[PIXEL_ROW_OF_LINE][ALL_BYTE_OF_LINE + 1];
} Matrix_data[TABLE_ROW_NUM];
/*chương trình dịch bits*/
void matrix_move_bits(uint8_t move, uint8_t *data, uint16_t lenght)
{
	uint16_t index;

	for (index = 0; index < lenght; index++)
	{
		data[index] = data[index] << move;
		if (index + 1 < lenght)
			data[index] |= ((data[index + 1] >> (8 - move)));
	}
}
void matrix_get_one_font(uint8_t line, uint16_t location, uint16_t width)
{
	uint16_t index_row, index_col;
	uint8_t num_byte_of_line;
	uint8_t data_bit, data;

	num_byte_of_line = width / 8 + ((width % 8) ? 1 : 0);
	for (index_row = 0; index_row < PIXEL_ROW_OF_LINE; index_row++)
	{
		for (index_col = 0; index_col < num_byte_of_line; index_col++)
		{
			data_bit = arial[location + index_row * num_byte_of_line + index_col];
			// debug_msg("%02X_", data_bit);
			data = ((data_bit >> 7) & 0x01) | ((data_bit >> 5) & 0x02) | ((data_bit >> 3) & 0x04) | ((data_bit >> 1) & 0x08);
			data |= ((data_bit << 7) & 0x80) | ((data_bit << 5) & 0x40) | ((data_bit << 3) & 0x20) | ((data_bit << 1) & 0x10);
			// debug_msg("%02X__", data);
			Matrix_data[line].one_byte[index_row][index_col] = data;
		}
		// debug_msg("%02X-%02X\r", DATA_ONE_FONT[index_row][0], DATA_ONE_FONT[index_row][1]);
	}
}
void matrix_prepare_data(uint8_t line, uint8_t coppy)
{
	uint16_t index_row;

	for (index_row = 0; index_row < PIXEL_ROW_OF_LINE; index_row++)
	{
		matrix_move_bits(coppy, Matrix_data[line].encryption[index_row], ALL_BYTE_OF_LINE);
		Matrix_data[line].encryption[index_row][ALL_BYTE_OF_LINE - 1] |= (Matrix_data[line].one_byte[index_row][0] >> (8 - coppy));
		matrix_move_bits(coppy, Matrix_data[line].one_byte[index_row], NUM_BYTE_OF_LINE);
	}
}
/*chương trình xóa trống bảng led*/
void matrix_refesh(uint8_t line)
{
	uint8_t row_of_line;
	for (row_of_line = 0; row_of_line < PIXEL_ROW_OF_LINE; row_of_line++)
		memset(Matrix_data[line].encryption[row_of_line], 0x00, ALL_BYTE_OF_LINE);
}
/*chương trình nhận chuỗi*/
void matrix_send(uint8_t line, char *data, uint8_t pause_run)
{
	Matrix_data[line].flag_data = 1;
	strcpy(Matrix_data[line].string, data);
	if (pause_run == 1 && strlen(data) < 194){ //buff 200 byte, thêm 5 byte "space" khi có hiệu ứng
		strcat(Matrix_data[line].string, "     ");
  }
	Matrix_data[line].pause_run = pause_run;
}

void matrix_run_hidden(void)
{
	static uint16_t pixel_col_num_hidden[TABLE_ROW_NUM] = {0,0};
	static uint16_t location_font_hidden[TABLE_ROW_NUM] = {0,0}; //vị trí của một font chữ trong bảng mã
	for (uint8_t line_num = 0; line_num < TABLE_ROW_NUM; line_num++)
	{
		if (Matrix_data[line_num].flag_data == 1)
		{				
			if (pixel_col_num_hidden[line_num] == 0)
			{
				pixel_col_num_hidden[line_num] = PIXEL_COL * TABLE_COL_NUM;
				// Matrix_data[line_num].flag_data = 0;
				Matrix_data[line_num].id_pixel_col = 0;
				Matrix_data[line_num].id_string = 0;
				matrix_refesh(line_num);
				font_get_location_lenght((uint8_t)Matrix_data[line_num].string[Matrix_data[line_num].id_string], &location_font_hidden[line_num], &Matrix_data[line_num].width_byte);
				matrix_get_one_font(line_num, location_font_hidden[line_num], Matrix_data[line_num].width_byte);
			}
			else
			{

				matrix_prepare_data(line_num, 1); //chỉ sao chép 1 cột vào mãng
				if (++Matrix_data[line_num].id_pixel_col == Matrix_data[line_num].width_byte)
				{
					Matrix_data[line_num].id_pixel_col = 0;
					/*nếu đã hết ký tự thì chèn khoảng trống vào*/
					if (++Matrix_data[line_num].id_string >= strlen(Matrix_data[line_num].string))
					{
						/*Nếu không có hiệu ứng thì thoát*/
						if (Matrix_data[line_num].pause_run == 0)
						{
							font_get_location_lenght(' ', &location_font_hidden[line_num], &Matrix_data[line_num].width_byte);
							matrix_get_one_font(line_num, location_font_hidden[line_num], Matrix_data[line_num].width_byte);
						}
						else
						{
							Matrix_data[line_num].id_string = 0;
							font_get_location_lenght((uint8_t)Matrix_data[line_num].string[Matrix_data[line_num].id_string], &location_font_hidden[line_num], &Matrix_data[line_num].width_byte);
							matrix_get_one_font(line_num, location_font_hidden[line_num], Matrix_data[line_num].width_byte);
						}
					}
					else
					{
						font_get_location_lenght((uint8_t)Matrix_data[line_num].string[Matrix_data[line_num].id_string], &location_font_hidden[line_num], &Matrix_data[line_num].width_byte);
						matrix_get_one_font(line_num, location_font_hidden[line_num], Matrix_data[line_num].width_byte);
					}										
				}
				pixel_col_num_hidden[line_num]--;
				if(pixel_col_num_hidden[line_num] == 0)
				{
					Matrix_data[line_num].flag_data = 0;
				}
			}
		}
	}
}

/*chương trình nhận và xử lý ký tự thành byte array để hiển thị*/
void matrix_process(void)
{
	uint8_t line_num;
	uint16_t location_font; //vị trí của một font chữ trong bảng mã
	uint16_t pixel_col_num = 1;
	
	matrix_run_hidden();
	
	if (row_of_line != 0)
	{
		return;
	}

	for (line_num = 0; line_num < TABLE_ROW_NUM; line_num++)
	{
		/*trong trường hợp không hiệu ứng thì chỉ xử lý 1 lần*/
		if (Matrix_data[line_num].flag_data == 0 && Matrix_data[line_num].pause_run == 0)
		{
			continue;
		}
			
		if (Matrix_data[line_num].flag_data == 1)
		{
				continue;
//			row_update_enable = 1;
//			pixel_col_num = PIXEL_COL * TABLE_COL_NUM;
//			// Matrix_data[line_num].flag_data = 0;
//			Matrix_data[line_num].id_pixel_col = 0;
//			Matrix_data[line_num].id_string = 0;
//			matrix_refesh(line_num);
//			font_get_location_lenght((uint8_t)Matrix_data[line_num].string[Matrix_data[line_num].id_string], &location_font, &Matrix_data[line_num].width_byte);
//			matrix_get_one_font(line_num, location_font, Matrix_data[line_num].width_byte);
		}
		else
		{
		  pixel_col_num = 1;
		}
		while (pixel_col_num)
		{
			matrix_prepare_data(line_num, 1); //chỉ sao chép 1 cột vào mãng
			if (++Matrix_data[line_num].id_pixel_col == Matrix_data[line_num].width_byte)
			{
				Matrix_data[line_num].id_pixel_col = 0;
				/*nếu đã hết ký tự thì chèn khoảng trống vào*/
				if (++Matrix_data[line_num].id_string >= strlen(Matrix_data[line_num].string))
				{
					/*Nếu không có hiệu ứng thì thoát*/
					if (Matrix_data[line_num].pause_run == 0)
					{
						font_get_location_lenght(' ', &location_font, &Matrix_data[line_num].width_byte);
						matrix_get_one_font(line_num, location_font, Matrix_data[line_num].width_byte);
					}
					else
					{
						Matrix_data[line_num].id_string = 0;
						font_get_location_lenght((uint8_t)Matrix_data[line_num].string[Matrix_data[line_num].id_string], &location_font, &Matrix_data[line_num].width_byte);
						matrix_get_one_font(line_num, location_font, Matrix_data[line_num].width_byte);
					}
				}
				else
				{
					font_get_location_lenght((uint8_t)Matrix_data[line_num].string[Matrix_data[line_num].id_string], &location_font, &Matrix_data[line_num].width_byte);
					matrix_get_one_font(line_num, location_font, Matrix_data[line_num].width_byte);
				}
			}

			pixel_col_num -= 1;
		}
		//Matrix_data[line_num].flag_data = 0;
	}
	
	if(row_update_enable == 1)
	{
	  row_update_enable = 0;
	}
	else
	{
		row_of_line = PIXEL_ROW_OF_LINE;
	}
}
/*chương trình scan hiển thị*/
void matrix_scan_show(void)
{
	uint8_t lenght;
	uint8_t data1, data2;
	
	if(row_of_line == 0)
	{
		if(row_update_enable == 0)
		{
			return;
		}
		else
		{
			row_of_line = PIXEL_ROW_OF_LINE;
		}
	}

	row_of_line --;
	matrix_select_line(row_of_line);

	for (lenght = 0; lenght < ALL_BYTE_OF_LINE; lenght++)
	{
		data1 = Matrix_data[0].encryption[row_of_line][lenght];
		data2 = Matrix_data[1].encryption[row_of_line][lenght];
		
		if(Matrix_data[0].flag_data == 1)
		{
			data1 = 0;
		}
		if(Matrix_data[1].flag_data == 1)
		{
			data2 = 0;
		}
		matrix_shift_data(data1, data2);
	}		

	matrix_latch_data();
	OE(1);
	OE(0);		
}

/*THE END*/
