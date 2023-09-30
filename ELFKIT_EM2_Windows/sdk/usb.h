#ifndef USB_H
#define USB_H

#include <typedefs.h>


//Сим в котором хранится текущий режим подключения
#define SEEM_USB_CDC_CONFIG 0x178


//Перечисление режиммов USB
enum
{
	USB_MODE_MODEM 	= 0x00,		//Модем
	USB_MODE_UNK_01,
	USB_MODE_UNK_02,
	USB_MODE_UNK_03,
	USB_MODE_UNK_04,
	USB_MODE_UNK_05,
	USB_MODE_UNK_06,
	USB_MODE_P2K,			//P2K режим
	USB_MODE_UNK_08,
	USB_MODE_UNK_09,
	USB_MODE_UNK_0A,
	USB_MODE_UNK_0B,
	USB_MODE_UNK_0C,
	USB_MODE_UNK_0D,
	USB_MODE_UNK_0E,
	USB_MODE_MCARD,			//Карта памяти
	USB_MODE_MAX
};
typedef UINT8 USB_MODE_T;


//Получаем адрес usb_default_config из либы
EXTERN_LIB usb_default_config;
#define usb_def_cfg (UINT8 *)usb_default_config

//Выбор режима подключения
void USB_DRIVER_select_config(USB_MODE_T cfg_id);
#define USB_CHANGE_MODE(mode) USB_DRIVER_select_config(mode & 0x0F)

#endif
