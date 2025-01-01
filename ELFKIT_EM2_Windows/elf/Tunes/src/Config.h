#define DEBUG

#ifndef CONFIG_H
#define CONFIG_H

#include <apps.h>
#include <filesystem.h>
#include <utilities.h>

#define PARS_DONE			  0
#define PARS_SKIP			  1
#define PARS_INVALID		  2
#define PARS_EOF			  3

#define MAX_FIELDS			  7

typedef unsigned int SU_SIZE;
typedef INT32 SU_RET_STATUS;

typedef enum
{
    Cfg_TrackNumber=0,		
    Cfg_Playlist,	
    Cfg_Artist,	
    Cfg_Title,
    Cfg_Album,
    Cfg_Time, 
    Cfg_TimeCurrent, 
    Cfg_TimeElapsed,
    Cfg_SoftLabel,

    Cfg_Strings_Max

} CFG_FIELD_STR;

typedef enum
{
    Cfg_TitlePicture=0,  
    Cfg_SoftPicture, 
    Cfg_Backward,
    Cfg_Play,    
    Cfg_Pause,
    Cfg_Stop,     
    Cfg_Forward,
    Cfg_Random,
    Cfg_RepetitionOne,
    Cfg_RepetitionAll,
    Cfg_Mute,
    Cfg_Animation,

    Cfg_Pictures_Max

} CFG_FIELD_PIC;     

typedef enum
{
    Cfg_Background=0,             
    Cfg_ProgressBar,  
    Cfg_VolumeBar,

    Cfg_Object_Max

} CFG_FIELD_OBJ;

typedef struct
{
    UINT16		x,y;
    UINT16      anchor;
    UINT8       font_id, font_style;
    UINT32		fcolor, bcolor;
} STR_CFG;

typedef struct
{
    UINT16		x, y;
    UINT32      anchor;
} PIC_CFG;

typedef struct
{
    UINT16 x,y;
    UINT16 w,h;
    UINT32 color;
} OBJ_CFG;

typedef struct
{
    UINT8 s;
} SET_CFG;

typedef struct
{
    STR_CFG		strings[Cfg_Strings_Max];     // Title, Artist
    PIC_CFG	    pictures[Cfg_Pictures_Max];    // Play, Stop, Pause, Rew, Ffw
    OBJ_CFG	    objects[Cfg_Object_Max];       // ProgressBar, VolumeBar
    SET_CFG     setting;
}CONFIG_T;

typedef struct
{
    const char      *name;
    const UINT8	    *stt;
    const UINT16    off;
} CFG_FIELD_T;

extern CONFIG_T cfg_skin;

UINT32  ParseConfig(CONFIG_T *cfg, WCHAR  *folder);
UINT32  ParseString(char* buf, UINT32 *pindex, CONFIG_T *config);
UINT32	ParseValue(char* buf, UINT32 strn, UINT32 fldn,  CONFIG_T *config);
UINT32	name_cmp(char* str1, const char* str2);

#endif
