#ifndef Z_H
#define Z_H

#include <apps.h>
#include <dl.h>
#include <sessions.h>
#include <filesystem.h>

typedef struct
{
	UINT16 Count;		//Кол-во трэков в плейлисте
	UINT32 MaxCount;	//Наверное максимальное кол-во трэков в плейлисте (0x00010000).
	UINT8  unk1[2];		//0
} AP_PLAYLIST_HEADER;	//Размер - 8 байт

typedef DL_FS_MID_T AP_PLAYLIST_ELEMENT;

enum
{
	RESULT_CODE_SUCCESS = 0,
	RESULT_CODE_ERROR_INVALID_FILE_URI,
	RESULT_CODE_ERROR_INVALID_DATA_PTR,
	RESULT_CODE_ERROR_INVALID_DATA_SIZE,
	RESULT_CODE_ERROR_FILE_OPEN,
	RESULT_CODE_ERROR_INCOMPLETE_FILE_WRITE
};
typedef UINT32 RESULT_CODE_T;


RESULT_CODE_T DumpData(WCHAR *uri, void *data, UINT32 size)
{
	//check input vars
	if(uri == NULL) return RESULT_CODE_ERROR_INVALID_FILE_URI;
	if(data == NULL) return RESULT_CODE_ERROR_INVALID_DATA_PTR;
	if(size <= 0) return RESULT_CODE_ERROR_INVALID_DATA_SIZE;

	//open file
	FILE_HANDLE_T hFile = DL_FsOpenFile(uri, FILE_APPEND_PLUS_MODE, NULL);
	if(hFile == FILE_HANDLE_INVALID) return RESULT_CODE_ERROR_FILE_OPEN;

	//write file
	UINT32 written;
	DL_FsWriteFile((char*)data, size, 1, hFile, &written);

	//close file
	DL_FsCloseFile(hFile);
	if(written != size) return RESULT_CODE_ERROR_INCOMPLETE_FILE_WRITE;

	return RESULT_CODE_SUCCESS;
}

//If success, function return AWF_ID_T, else - 0xFFFFFFFF. Maybe this function will be added in new version of elfloader
AFW_ID_T ldrGetAwfIdByRegId (REG_ID_T reg_id)
{
    UINT8 i, j, sessions_cnt;
    SESSION_INFO_T *sessions_lst;

    AFW_ID_T afw_id = 0xFFFFFFFF;
    APP_INFO_T *app_info = NULL;


    //Gettint couunt of sessions
	sessions_cnt = AFW_GetNumSessions();

	//Allocating memory for sessions list
	sessions_lst = (SESSION_INFO_T *)malloc(sessions_cnt * sizeof(SESSION_INFO_T));
	if(sessions_lst == NULL) return afw_id;

    //Getting sessions list
    AFW_GetSessionList(sessions_lst, sessions_cnt);

	for(i=0; i<sessions_cnt; i++)
	{
	    //Check if there no applications in session.
		if(sessions_lst[i].apps_num == 0) continue;

        //Allocate memory for applications in session.
        app_info = (APP_INFO_T *)malloc(sessions_lst[i].apps_num * sizeof(APP_INFO_T));
        if(app_info == NULL)
        {
            free(sessions_lst);
            return afw_id;
        }

        //Getting information about all applications in session
		AFW_GetAppsInSession(sessions_lst[i].ss_id, app_info, sessions_lst[i].apps_num);

		//Search awf_id by reg_id
		for(j=0; j<sessions_lst[i].apps_num; j++)
		{
            if(reg_id == app_info[j].app->reg_id)
			{
			    afw_id = app_info[j].app->afw_id;

			    free(app_info);
				free(sessions_lst);

				return afw_id;
			}
		}

		free(app_info);
	}

	free(sessions_lst);
	return 0xFFFFFFFF;
}

static APPLICATION_T * GetAppByRegID (REG_ID_T reg_id)
{
	UINT8 num = 0;
	APPLICATION_T *app = NULL;
	SESSION_INFO_T *sessions_inf = NULL;

	num = AFW_GetNumSessions();

	sessions_inf = (SESSION_INFO_T *)device_Alloc_mem(num, sizeof(SESSION_INFO_T));
	if(sessions_inf == NULL) return app;

        AFW_GetSessionList(sessions_inf, num);

        UINT8 i,j;
	for(i=0; i<num; i++)
	{
		if(sessions_inf[i].apps_num == 0) continue;

		APP_INFO_T *app_info = (APP_INFO_T *)device_Alloc_mem(sessions_inf[i].apps_num, sizeof(APP_INFO_T));
		if(app_info == NULL)
            	{
                	device_Free_mem_fn(sessions_inf);
			return app;
		}

		AFW_GetAppsInSession(sessions_inf[i].ss_id, app_info, sessions_inf[i].apps_num);

		for(j=0; j<sessions_inf[i].apps_num; j++)
		{
            if(reg_id == app_info[j].app->reg_id)
			{
				app = app_info[j].app;

				device_Free_mem_fn(app_info);
				device_Free_mem_fn(sessions_inf);

				return app;
			}
		}

		device_Free_mem_fn(app_info);
	}

	device_Free_mem_fn(sessions_inf);
	return app;
}

UINT32 Z_GetDispBacklight (UINT8 *BLKval)
{
	if(BLKval == NULL)
		return RESULT_FAIL;

	UINT8 data[0x10];
	DL_DbFeatureGetBlock(ID_DISP_BACKLIGHT, data);
	*BLKval = data[7];

	return RESULT_OK;
}

UINT32 Z_SetDispBacklight (UINT8 BLKval)
{
	if(BLKval > 6)
		return RESULT_FAIL;

	UINT8 data[0x10];
	DL_DbFeatureGetBlock(ID_DISP_BACKLIGHT, data);

	data[7] = BLKval;
	DL_DbFeatureStoreBlock(ID_DISP_BACKLIGHT, data, 0x10);

	return RESULT_OK;

}


UINT32 Z_GetDispContrast (UINT8 *ContrastValue)
{
	if(ContrastValue == NULL)
		return RESULT_FAIL;

	UINT8 data[0x10];
	DL_DbFeatureGetBlock(ID_DISP_BACKLIGHT, data);
	*ContrastValue = data[3];

	return RESULT_OK;
}

UINT32 Z_SetDispContrast (UINT8 ContrastValue)
{
	UINT8 data[0x10];
	DL_DbFeatureGetBlock(ID_DISP_BACKLIGHT, data);

	data[3] = ContrastValue;
	DL_DbFeatureStoreBlock(ID_DISP_BACKLIGHT, data, 0x10);

	return RESULT_OK;
}

UINT32 Z_ApplyDisplayContrast (UINT8 ContrastValue)
{
	return DAL_AdjustContrast(ContrastValue);
}

UINT32 RC_GetDLTime(UINT16 TimeID, CALL_TIME_T *time)
{
	if(time == NULL)
		return RESULT_FAIL;

	memclr(time, sizeof(CALL_TIME_T));
	DL_DbFeatureGetBlock(TimeID, time);
	return RESULT_OK;
}

UINT32 RC_SetDLTime(UINT16 TimeID, CALL_TIME_T *time)
{
	if(time == NULL)
		return RESULT_FAIL;

	DL_DbFeatureStoreBlock(TimeID, time, sizeof(CALL_TIME_T));
	return RESULT_OK;
}

UINT32 Z_ChangeEndian(UINT32 value)
{
	UINT8 *ptr = (UINT8 *)&value;
	return (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | ptr[3];
}

#endif
