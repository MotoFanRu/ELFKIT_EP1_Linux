#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <filesystem.h>

#define OBEX_TRANSPORT_BT 0x01
#define OBEX_OBJECT_FILE  0x05

#define OBEX_ACTION_COPY 0x00
#define OBEX_ACTION_MOVE 0x01

typedef struct
{
    UINT8        b_vobjectid;             // OBEX_OBJECT_FILE
    UINT8        pad1[584];               // 0
    UINT8        obex_action;             // OBEX_ACTION_*
    UINT16       pad2;                    // 0
    DL_FS_MID_T  *p_ids;                  // Указатель на массив физических идентификаторов файлов
    UINT32       num_files;               // Колличество передаваемых файлов
    UINT8        pad3[1848];              // 0
    UINT16       selected_transport_type; // OBEX_TRANSPORT_BT
    UINT8        pad4[2];                 // 0
    UINT32       transport_list_size;     // 0
} RAINBOW_POG_SEND_BT_ATT;                // Размер 2452 байт

//RAINBOW_POG_SEND_BT_ATT att;
//AFW_CreateInternalQueuedEvAux(EV_BT_SEND, FBF_LEAVE, sizeof(RAINBOW_POG_SEND_BT_ATT), (void*)&att);

//структура описана не до конца
typedef struct
{
    WCHAR        real_name[FS_MAX_FILE_NAME_LENGTH + 1];    //реальное имя файла
    UINT8        unk1[5];
    WCHAR        temp_name[FS_MAX_FILE_NAME_LENGTH + 1]; //путь к временному файлу
} RAINBOW_POG_PREVIEW_RECORD_T;

#endif
