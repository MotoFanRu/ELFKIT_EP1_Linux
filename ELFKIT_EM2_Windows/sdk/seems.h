#ifndef SEEMS_H
#define SEEMS_H

#include <typedefs.h>

#define SEEM_WRITE_METHOD_ADD	  0
#define SEEM_WRITE_METHOD_UPDATE  1

typedef struct 
{
	UINT32 seem_offset;
	UINT32 seem_size;
	UINT16 seem_element_id;
	UINT16 seem_record_number;
} SEEM_ELEMENT_DATA_CONTROL_T; 

//*data_ctrl_ptr - адрес структуры, содержащей информацию дл€ запроса.
//*data_buf - адрес куда будет считана информаци€
//read_zero_byte_allowed - возможно ли читать симы размером меньше 255 байт (TRUE - да, FALSE - нет)


//чтение из сима
UINT16 SEEM_ELEMENT_DATA_read  (SEEM_ELEMENT_DATA_CONTROL_T *data_ctrl_ptr, UINT8 *data_buf, BOOL read_zero_byte_allowed);

//запись в сим
UINT16 SEEM_ELEMENT_DATA_write (SEEM_ELEMENT_DATA_CONTROL_T *data_ctrl_ptr, UINT8 *seem_buf);

//возвращает кол-во records (записей) сима
UINT16 SEEM_MAX_RECORD_get_max_record (UINT16 seem_element_id);

//возвращает размер сима
UINT32 SEEM_ELEMENT_get_length (UINT16 seem_element_id);

//читает в буфер seem_data count байт сима seem, записи record. ѕеред чтением ќЅя«ј“≈Ћ№Ќќ выделить не менее count байт пам€ти!
UINT32 SEEM_FDI_OUTSIDE_SEEM_ACCESS_read (UINT32 seem, UINT32 record, void* seem_data, UINT32 count);

//записывает из буфера seem_data count байт в сим seem, запись record
UINT32 SEEM_FDI_OUTSIDE_SEEM_ACCESS_write (UINT32 method, UINT32 seem, UINT32 record, void* seem_data, UINT32 count);


#endif
