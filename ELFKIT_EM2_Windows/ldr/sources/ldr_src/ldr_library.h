#ifndef LDR_LIBRARY_H
#define LDR_LIBRARY_H

#include <loader.h>
#include <filesystem.h>
#include <utilities.h>

//�-��� ��������(������������) ����
UINT32 ldrLoadDefLibrary (void);

//�-��� ������ ������ � ���� �� �����
LIBRARY_RECORD_T * ldrSearchInDefLibrary (char *record_name);

#endif

//������ ����:
//���������
//������ 1
//������ 2
//...
//������ n
//��� 1
//��� 2
//...
//��� n
//
//����� ������� � �� ����������)
