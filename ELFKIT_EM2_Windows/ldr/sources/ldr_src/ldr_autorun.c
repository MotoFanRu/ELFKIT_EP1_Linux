#include "ldr_autorun.h"
#include "ldr_features.h"

UINT32 ldrAutorunAction (AUTORUN_RECORD_T *autorun_record, AUTORUN_ACTION_T autorun_action)
{
    UINT32 i, res, fSize;
    FILE_HANDLE_T hFile;
    UINT8 *autorun_buffer;
    AUTORUN_HEADER_T Header, *pHeader;
    AUTORUN_RECORD_T Record, *pRecord;

    //��� ������ � ������� ������
    DL_FS_MID_T file_id;
    WCHAR uri[FS_MAX_URI_NAME_LENGTH + 1];

    //������� �����������
    if(autorun_action == AUTORUN_ACTION_CLEAR)
    {
        DL_FsDeleteFile(AUTORUN_PATH, 0);
        return RESULT_OK;
    }

    //��������� ������ �����������. ���� ��� ����� NULL � �������� �� AUTORUN_ACTION_LOAD, �� ���������� RESULT_FAIL
    if(autorun_record == NULL && autorun_action != AUTORUN_ACTION_LOAD)
        return RESULT_FAIL;

    //��������� ���� ��� ������ � ������ � �������� ��� ������
    hFile = DL_FsOpenFile(AUTORUN_PATH, FILE_READ_PLUS_MODE, 0);
    fSize = DL_FsGetFileSize(hFile);

    //���� ���� �� �������� ��� ��� ������ ������ ������� ���������, ��
    if(hFile == FILE_HANDLE_INVALID || fSize <= sizeof(AUTORUN_HEADER_T))
    {
        //��������� ����
        DL_FsCloseFile(hFile);

        //���� �������� "�������� � ����������", �� �������� ������� ���� ����������� � ���������� ������
        if(autorun_action == AUTORUN_ACTION_ADD)
        {
            fSize = sizeof(AUTORUN_HEADER_T);
            hFile = DL_FsOpenFile(AUTORUN_PATH, FILE_WRITE_PLUS_MODE, 0);

            //���� ������� ���� �� �������, �� ���������� RESULT_FAIL
            if(hFile == FILE_HANDLE_INVALID)
                return RESULT_FAIL;
        }
        else
        {
            //��� ���� ������ �������� ���������� RESULT_FAIL
            return RESULT_FAIL;
        }
    }

    switch(autorun_action)
    {
        //���������� ������ � ���������� +++
        case AUTORUN_ACTION_ADD:
            autorun_buffer = (UINT8 *)malloc(fSize);
            if(autorun_buffer == NULL) break;

            if(fSize == sizeof(AUTORUN_HEADER_T)) memset(autorun_buffer, 0, fSize);
            else DL_FsReadFile(autorun_buffer, fSize, 1, hFile, &res);

            pHeader = (AUTORUN_HEADER_T *)autorun_buffer;
            pHeader->records_num += 1;

            DL_FsFSeekFile(hFile, 0, SEEK_WHENCE_SET);
            DL_FsWriteFile(autorun_buffer, fSize, 1, hFile, &res);
            DL_FsWriteFile(autorun_record, sizeof(AUTORUN_RECORD_T), 1, hFile, &res);
            DL_FsCloseFile(hFile);

            return RESULT_OK;
            break;

        case AUTORUN_ACTION_DELETE:
            autorun_buffer = (UINT8 *)malloc(fSize);
            if(autorun_buffer == NULL) break;

            DL_FsReadFile(autorun_buffer, fSize, 1, hFile, &res);
            DL_FsCloseFile(hFile);

            //���� ���� � ����������� ---
            for(i = 0; i < pHeader->records_num; i += 1)
            {
                pRecord = (AUTORUN_RECORD_T *)(pHeader + sizeof(AUTORUN_HEADER_T) + i*sizeof(AUTORUN_RECORD_T));
                if(memcmp(&autorun_record->file_id, &pRecord->file_id, sizeof(DL_FS_MID_T)) == 0)
                {
                        pHeader->records_num -= 1;

                        if(pHeader->records_num == 0)
                        {
                            fSize = sizeof(AUTORUN_HEADER_T);
                        }
                        else 
                        {
                            fSize -= sizeof(AUTORUN_RECORD_T);
                            memcpy((VOID *)pRecord, (VOID *)(pRecord + sizeof(AUTORUN_RECORD_T)), (pHeader->records_num - i - 1)*sizeof(AUTORUN_RECORD_T));
                        }

                        hFile = DL_FsOpenFile(AUTORUN_PATH, FILE_WRITE_PLUS_MODE, NULL);
                        DL_FsWriteFile(autorun_buffer, fSize, 1, hFile, &res);
                        DL_FsCloseFile(hFile);
                        return RESULT_OK;
                }
            }
            break;

        //����� � ����������� + ���������� �����������
        case AUTORUN_ACTION_FIND:
        case AUTORUN_ACTION_LOAD:
            DL_FsReadFile(&Header, sizeof(AUTORUN_HEADER_T), 1, hFile, &res);
            if(autorun_action == AUTORUN_ACTION_LOAD)
            {
                for(i = 0; i < Header.records_num; i += 1)
                {
                    DL_FsReadFile(&Record, sizeof(AUTORUN_RECORD_T), 1, hFile, &res);
                    u_strcpy(uri, L"file:/\0");
                    DL_FsGetURIFromID(&Record.file_id, uri + 6);
        
                    if(DL_FsFFileExist(uri)) 
                        ldrLoadElf(uri, NULL, Record.evcode_base);
                }
                DL_FsCloseFile(hFile);
                return RESULT_OK;
            }
            else
            {
                for(i = 0; i < Header.records_num; i += 1)
                {
                    DL_FsReadFile(&Record, sizeof(AUTORUN_RECORD_T), 1, hFile, &res);
                    if(memcmp(&autorun_record->file_id, &Record.file_id, sizeof(DL_FS_MID_T)) == 0)
                    {
                        DL_FsCloseFile(hFile);
                        return RESULT_OK;
                    }
                }
                DL_FsCloseFile(hFile);
            }
            break;
    }

    return RESULT_FAIL;
}
