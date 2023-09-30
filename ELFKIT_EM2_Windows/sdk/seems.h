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

//*data_ctrl_ptr - ����� ���������, ���������� ���������� ��� �������.
//*data_buf - ����� ���� ����� ������� ����������
//read_zero_byte_allowed - �������� �� ������ ���� �������� ������ 255 ���� (TRUE - ��, FALSE - ���)


//������ �� ����
UINT16 SEEM_ELEMENT_DATA_read  (SEEM_ELEMENT_DATA_CONTROL_T *data_ctrl_ptr, UINT8 *data_buf, BOOL read_zero_byte_allowed);

//������ � ���
UINT16 SEEM_ELEMENT_DATA_write (SEEM_ELEMENT_DATA_CONTROL_T *data_ctrl_ptr, UINT8 *seem_buf);

//���������� ���-�� records (�������) ����
UINT16 SEEM_MAX_RECORD_get_max_record (UINT16 seem_element_id);

//���������� ������ ����
UINT32 SEEM_ELEMENT_get_length (UINT16 seem_element_id);

//������ � ����� seem_data count ���� ���� seem, ������ record. ����� ������� ����������� �������� �� ����� count ���� ������!
UINT32 SEEM_FDI_OUTSIDE_SEEM_ACCESS_read (UINT32 seem, UINT32 record, void* seem_data, UINT32 count);

//���������� �� ������ seem_data count ���� � ��� seem, ������ record
UINT32 SEEM_FDI_OUTSIDE_SEEM_ACCESS_write (UINT32 method, UINT32 seem, UINT32 record, void* seem_data, UINT32 count);


#endif
