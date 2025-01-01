#include "Config.h"

CONFIG_T cfg_skin;

static const UINT8 structs[][MAX_FIELDS] =
{
    { 2,4,6,7,8,12,16   }, //STR_CFG
    { 2,4,8             }, //PIC_CFG
    { 2,4,6,8,12        }, //OBJ_CFG
    { 1                 }, //SET_CFG
};

static const CFG_FIELD_T fields[]=
{
    { "TrackNumber",		    structs[0],   sizeof(STR_CFG)*0 },
    { "Playlist",		        structs[0],	  sizeof(STR_CFG)*1 },
    { "Artist",		            structs[0],	  sizeof(STR_CFG)*2 },
    { "Title",		            structs[0],	  sizeof(STR_CFG)*3 },
    { "Album",		            structs[0],	  sizeof(STR_CFG)*4 },
    { "Time",                   structs[0],	  sizeof(STR_CFG)*5 },
    { "TimeCurrent",            structs[0],	  sizeof(STR_CFG)*6 },
    { "TimeElapsed",            structs[0],	  sizeof(STR_CFG)*7 },
    { "SoftLabel",              structs[0],	  sizeof(STR_CFG)*8 },

    { "TitlePicture",           structs[1],	  sizeof(STR_CFG)*9 + sizeof(PIC_CFG)*0  },
    { "SoftPicture",            structs[1],	  sizeof(STR_CFG)*9 + sizeof(PIC_CFG)*1  },
    { "Backward",               structs[1],	  sizeof(STR_CFG)*9 + sizeof(PIC_CFG)*2  },
    { "Play",                   structs[1],	  sizeof(STR_CFG)*9 + sizeof(PIC_CFG)*3  },
    { "Pause",                  structs[1],	  sizeof(STR_CFG)*9 + sizeof(PIC_CFG)*4  },
    { "Stop",                   structs[1],	  sizeof(STR_CFG)*9 + sizeof(PIC_CFG)*5  },
    { "Forward",                structs[1],	  sizeof(STR_CFG)*9 + sizeof(PIC_CFG)*6  },
    { "Random",                 structs[1],	  sizeof(STR_CFG)*9 + sizeof(PIC_CFG)*7  },
    { "RepetitionOne",          structs[1],	  sizeof(STR_CFG)*9 + sizeof(PIC_CFG)*8  },
    { "RepetitionAll",          structs[1],	  sizeof(STR_CFG)*9 + sizeof(PIC_CFG)*9  },
    { "Mute",                   structs[1],	  sizeof(STR_CFG)*9 + sizeof(PIC_CFG)*10 },
    { "Animation",              structs[1],	  sizeof(STR_CFG)*9 + sizeof(PIC_CFG)*11 },
 
    { "Background",             structs[2],	  sizeof(STR_CFG)*9 + sizeof(PIC_CFG)*12 + sizeof(OBJ_CFG)*0 },
    { "ProgressBar",            structs[2],	  sizeof(STR_CFG)*9 + sizeof(PIC_CFG)*12 + sizeof(OBJ_CFG)*1 },
    { "VolumeBar",              structs[2],	  sizeof(STR_CFG)*9 + sizeof(PIC_CFG)*12 + sizeof(OBJ_CFG)*2 },

    { "StatusLine",             structs[3],	  sizeof(STR_CFG)*9 + sizeof(PIC_CFG)*12 + sizeof(OBJ_CFG)*3 }
};


UINT32 ParseConfig(CONFIG_T *cfg, WCHAR  *folder)
{
    UINT32                  count;
    FILE_HANDLE_T           f;
    char                    *buf;
    UINT32                  i=0, result = PARS_SKIP;
    UINT32				    filesize;
    WCHAR                   file[256];                   

    u_strcpy(file, folder);
    u_strcat(file, L"Tunes.tsk");
	
	if (!DL_FsFFileExist(file)) return 0;
	dbgf("Tunes.tsk exist!");
	
    f = DL_FsOpenFile(file, FILE_READ_MODE, 0);
    if(f == FILE_HANDLE_INVALID)
    {
        memset(cfg, 0, sizeof(CONFIG_T));
        return 0;
    }

    filesize = DL_FsGetFileSize(f);
    if(filesize == 0) return 0;

    buf = (char*)malloc(filesize+3);
    if(buf == NULL) return 0;

    DL_FsReadFile(buf, filesize, sizeof(UINT8), f, &count);
    DL_FsCloseFile(f);

    if(buf[filesize-1]!='\n')
    {
        buf[filesize++]='\r';
        buf[filesize++]='\n';
    }
    buf[filesize]=(char)0xFF;

    do
    {
        result = ParseString(&buf[i], &i, cfg);

    }while(result != 3);

    free(buf);
    return 1;
}

void SkipLine(char *buf, UINT32 *pindex)
{
    UINT32 i=0;
    while( buf[i]!='\n' ) i++;
    *pindex += i+1;
}


UINT32  ParseString(char* buf, UINT32 *pindex, CONFIG_T *config)
{
    UINT32		i = 0, j = 0, k = 0, f=1, l=0;
    //UINT32		index = *pindex;
    UINT32		status = PARS_DONE;

    if(buf[0]==(char)0xFF) return PARS_EOF;

    if( (buf[0]==';') || (buf[0]=='\r') )
    {
        SkipLine(buf, pindex);
        return PARS_SKIP;
    }

    while((buf[j]!=' ') && (buf[j]!='=')) j++;

    buf[j++] = 0; //&buf[0] - name

    for(k=0; k< sizeof(fields)/sizeof(CFG_FIELD_T); k++)
    {
        status = name_cmp(buf, fields[k].name);
        if(status == 1) break;
    }

    if(status==0)
    {
        SkipLine(buf, pindex);
        return PARS_INVALID;
    }

    while(buf[j]=='=' || buf[j]==' ') j++;

    i=j;
    do
    {
        while ((buf[i]!=' ') && (buf[i]!='\r')) i++;
        if(buf[i]=='\r') f = 0;
        buf[i] = 0;

        status = ParseValue(&buf[j], l++, k, config);
        if(status != PARS_DONE) 
        {
            SkipLine(buf, pindex);
            return PARS_INVALID;
        }

        while (buf[++i]==' ');
        j=i;
    }while(f);


    *pindex+=i+1;
    return status;
}


UINT32	ParseValue(char* buf, UINT32 strn, UINT32 fldn,  CONFIG_T *config)
{
    UINT32		i=0, j=0;
    void		*ptr = (char*)config+fields[fldn].off;

    if(strn>0) j = fields[fldn].stt[strn-1];
    else j=0;

    ptr = (char*)ptr + j;

    if(buf[1]=='x')
    {
        i = strtoul(buf, 0, 16);
    }
    else
    {
        i = strtoul(buf, 0, 10);
    }

    switch(fields[fldn].stt[strn]-j)
    {
        case 1: *(UINT8*)ptr = (UINT8)i; break;
        case 2: *(UINT16*)ptr = (UINT16)i; break;
        case 4: *(UINT32*)ptr = (UINT32)i; break;
    }

    return PARS_DONE;
}


UINT32 name_cmp (char* str1, const char* str2)
{
    UINT32 i=0;
    while( str1[i] == str2[i] )
    {
        if(str1[i++] == 0) return 1;
    }
    return 0;
}

