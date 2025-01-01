#include "Utils.h"

DL_FS_MID_T file_send_mid;

// запись в файл
UINT32 file_write(WCHAR* uri, UINT8 mode, void* buffer, UINT32 size)
{
    UINT32  w, r=0, fsize;
    BYTE    *buf;
    FILE_HANDLE_T f;

    f = DL_FsOpenFile(uri, mode, 0);
    DL_FsWriteFile(buffer, size, 1, f, &w);
    DL_FsCloseFile(f); 

    return w;
}

// получение строки из ленга
UINT32 GetResourceString(WCHAR *str, RESOURCE_ID resID)
{
	UINT32 resource_size;
	DRM_GetResourceLength(resID, &resource_size);
	if(resource_size > 0) DRM_GetResource(resID, str, resource_size);
	return RESULT_OK;
}

// взять часть строки от spliter до конца
WCHAR* SplitPath( WCHAR* path, WCHAR* spliter ) 
{
	int j = u_strlen(path)-2;
	while( j > 0 ) 
	{
		if ( !u_strncmp(path+j, spliter, 1) ) 
		{
			return path+j+1;
		}
		j--;
	}
	return path;
}

 // состояние: TRUE - на рабочем столе, FALSE  - не нарабочем столе
BOOL WorkingTable(void)
{
	UINT8 res;
	UIS_GetActiveDialogType(&res);
	return (res == DialogType_Idle ? TRUE : FALSE);
}

 // состояние: TRUE - в яве, FALSE  - не в яве
BOOL JavaApp(void)
{
	UINT8 res;	
	UIS_GetActiveDialogType(&res);
	return (res == DialogType_Dynamic_Idle ? TRUE : FALSE);
}

// состояние: TRUE - клавиатура заблок., FALSE  - разблок.
BOOL KeypadLock(void)
{
	UINT8 state;
	DL_DbFeatureGetCurrentState(DB_FEATURE_KEYPAD_STATE, &state);
	return state;
}	

UINT32 util_asc2ul( const char * s, UINT32 count )
{
    char    b;
    UINT32    res = 0;
    
    while ( count > 0 ) {
        b = *s++;
        if ( b >= '0' && b <= '9' ) {
            b -= '0';
        } else if ( b >= 'A' && b <= 'F' ) {
            b = b - 'A' + 10;
        } else if ( b >= 'a' && b <= 'f' ) {
            b = b - 'a' + 10;
        } else
            b = 0;
        res = res << 4;
        res |= b;
        count--;
    }
    
    return res;
}

//поправить эту ф-цию для работы с WCHAR *
UINT32 param2uri(WCHAR *param, WCHAR *uri)
{
    _u64                    tmp;
    DL_FS_MID_T             fid;
/*
    if (u_strlen(param) > 16) 
    {
        u_strcpy(uri, param);
    }
    else 
    { // id
        tmp.R0 = util_asc2ul(param+0, 8);
        tmp.R1 = util_asc2ul(param+8, 8);
    
        if (tmp.R0==0 && tmp.R1==0) 
        {
            u_atou(param, uri);
        }
        else 
        {
            memcpy(&fid, &tmp, sizeof(UINT64));
            DL_FsGetURIFromID( &fid, uri );
        }
    }
*/
    return RESULT_OK;
}

// функции для удобства работы с действиями
UINT32 OneAction( ACTIONS_T * list, EVENT_CODE_T event, RESOURCE_ID res, UINT8 op )
{
	if ( list == NULL || list->count >= 16 )
		return RESULT_FAIL;
	list->action[list->count].operation = op;
	list->action[list->count].event = event;
	list->action[list->count].action_res = res;
	list->count++;
	return RESULT_OK;
}

UINT32 ActionAdd( ACTIONS_T * list, EVENT_CODE_T event, RESOURCE_ID res )
{
	return OneAction(list, event, res, ACTION_OP_ADD);
}

UINT32 ActionDel( ACTIONS_T * list, EVENT_CODE_T event, RESOURCE_ID res )
{
	return OneAction(list, event, res, ACTION_OP_DELETE);
}

UINT32 ActionUpd( ACTIONS_T * list, EVENT_CODE_T event, RESOURCE_ID res )
{
	return OneAction(list, event, res, ACTION_OP_UPDATE);
}


// проверка расширения
BOOL isAudio(WCHAR *ext) 
{
    WCHAR   *tmp_ext;
    BOOL    status;

    tmp_ext = malloc(u_strlen(ext)*sizeof(WCHAR));
    if (tmp_ext == NULL) return false;

    u_strcpy(tmp_ext, ext);
    u_strmakelower(tmp_ext);

    if (!u_strcmp(tmp_ext, L"mp3") || !u_strcmp(tmp_ext, L"m4a") || !u_strcmp(tmp_ext, L"wav") || !u_strcmp(tmp_ext, L"amr") || !u_strcmp(tmp_ext, L"mid") || !u_strcmp(tmp_ext, L"mp4") || !u_strcmp(tmp_ext, L"3gp") || !u_strcmp(tmp_ext, L"aac")) status = true;
    else status = false;

    free(tmp_ext);
    return status;
}

// сравнить две строки без учёта регистра
BOOL CmpStr(WCHAR *str1, WCHAR *str2) 
{
    WCHAR   *tmp_str1;
    WCHAR   *tmp_str2;
    BOOL    status;

    tmp_str1 = malloc(u_strlen(str1)*sizeof(WCHAR));
    tmp_str2 = malloc(u_strlen(str2)*sizeof(WCHAR));
    if (!tmp_str1 || !tmp_str2) return false;

    u_strcpy(tmp_str1, str1);
    u_strcpy(tmp_str2, str2);
    u_strmakelower(tmp_str1);
    u_strmakelower(tmp_str2);

    if (!u_strcmp(tmp_str1, tmp_str2)) return true;
    else return false;

    free(tmp_str1);
    free(tmp_str2);
    return status;
}

UINT32 SendViaBT(WCHAR* uri)
{
    RAINBOW_POG_SEND_BT_ATT att;
    memset(&att, 0, sizeof(RAINBOW_POG_SEND_BT_ATT));

    att.b_vobjectid = OBEX_OBJECT_FILE;
    att.obex_action = OBEX_ACTION_COPY;
    att.num_files = 1;
    att.selected_transport_type = OBEX_TRANSPORT_BT;
    att.transport_list_size = 0;

    DL_FsGetIDFromURI(uri, &file_send_mid);
    att.p_ids = &file_send_mid;

    return AFW_CreateInternalQueuedEvAux(EV_BT_SEND, FBF_LEAVE, sizeof(RAINBOW_POG_SEND_BT_ATT), &att);
}

