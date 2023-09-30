#ifndef MME_H
#define MME_H

#include <typedefs.h>

#define EV_MME_RANGE_START      0xE0000		    //Проверено на v3x!
#define EV_MME_OPEN_SUCCESS     EV_MME_RANGE_START+0x0  //+++ //удачное открытие файла. В аттаче 16 байт MME_OPEN_SUCCESS_T
#define EV_MME_OPEN_ERROR       EV_MME_RANGE_START+0x1  //+++ //не удачное открытие файла
#define EV_MME_SEEK_SUCCESS     EV_MME_RANGE_START+0x2  //+++ //возникает после использования ф-ции MME_GC_playback_seek (LTE)
#define EV_MME_SEEK_ERROR       EV_MME_RANGE_START+0x3  //+++ //не удачный seek
#define EV_MME_PLAY_COMPLETE    EV_MME_RANGE_START+0xF  //+++ //возникает при окончании проигрывания. в аттаче 16 байт (PLAY_COMPLETE_T)

//Проверить эти ивенты
#define EV_MME_PLAY_MARKER      EV_MME_RANGE_START+0xE  //??? //Возникает каждую секудну при проигрывании. в аттаче 16 байт (MME_PLAY_COMPLETE_T)
#define EV_MME_CLOSE_COMPLETE   EV_MME_RANGE_START+0x14 // при закрытия
#define EV_MME_STOP_COMPLETE    EV_MME_RANGE_START+0x15 // при стопе

#define EV_MME_STOPPED_AT_TIME  EV_MME_RANGE_START+0x16 //+++ стоп во время воспроизведения. в аттаче 12 байт (MME_OPEN_SUCCESS_T)

//Handle файла
typedef void * MME_GC_MEDIA_FILE;

typedef struct
{
   IFACE_DATA_T                 iface_data;
   MME_GC_MEDIA_FILE            media_handle;
   UINT8			status;	//RAINBOW POG. V3x - 8 при окончании воспроизведения. 
   UINT8			unk[3];
} MME_PLAY_COMPLETE_T;	 //Размер = 16 байт

typedef struct
{
   IFACE_DATA_T          iface_data;
   MME_GC_MEDIA_FILE   	 media_handle;
} MME_OPEN_SUCCESS_T;    //Размер = 12 байт

//Далее то что не помечено как v3x, возможно надо исправить с учётом архитектуры
/*******************************/
typedef struct 
{
    WCHAR           *str;
    UINT16          str_size;
} STRING_T;

typedef struct 
{
    WCHAR           *str;
    UINT16          str_size;
    UINT8           unk0[4];
    UINT8           unk1[4];
} RATING_T;

typedef struct 
{
    STRING_T   title;
    STRING_T   author;
    STRING_T   copyright;
    STRING_T   description;
    STRING_T   performer;
    STRING_T   genre;
    RATING_T   rating;
    STRING_T   location;
    STRING_T   date;
    STRING_T   album;

} MEDIA_FILE_INFO_T;

//v3x
typedef enum 
{
   PLAY_RATE_1X = 1,
   PLAY_RATE_2X,
   PLAY_RATE_3X,
   PLAY_RATE_4X,
   PLAY_RATE_MAX
} PLAY_RATE_T;


//AUDIO_CODEC_TYPE_T
enum
{
    CODEC_NONE = 0,	//Проверено
    MP3_CODEC = 2,	//Проверено
    AAC_CODEC,
    AAC_PLUS_CODEC = 4,
    MIDI_CODEC = 5,
    WAV_CODEC = 6,
    WMA_CODEC = 10,
    AMR_CODEC = 18,	//Проверено
    RAW_AUDIO = 19
};
typedef UINT8 AUDIO_CODEC_TYPE_T;

typedef struct
{
    UINT32        		bit_rate;	//+++
    AUDIO_CODEC_TYPE_T		audio_codec;	//+++
    UINT32        		sampling_freq;	
    UINT8           		audio_mode;     // 0=Mono, 1=Stereo 
    UINT16       		unk1[7];
    UINT8            		wav_bit_rate; // 0 =4, 1=8, 2=16 Kb/s
    UINT16            		unk2[7];
} AUDIO_FORMAT_T;			//Size = 0x48

#define ATTRIBUTE_RANGE_1 0x56 //V3x

//Проверено на v3x
typedef enum
{
	//get и set
	FILE_INFO = 0, 				// MEDIA_FILE_INFO_T (Size = 0x5C)
	DURATION,    				// UINT32 (Длительность в секундах)
	FILE_SIZE,				// UINT32 (Размер файла в байтах)
	UNK_ADDR,				// UINT32 (Похоже на какой-то адрес в RAM)
	UNKNOWN2,				// UINT32
	UNKNOWN3,				// UINT32
	MEDIA_PATH,				// WCHAR (Мне вернуло AC)
	UNKNOWN4,				// UINT8
	PLAY_RATE,				// PLAY_RATE_T (UINT8)
	UNKNOWN5,				// UINT8
	UNKNOWN6 = 10,				// UINT32 (Чёта меняется, а чё это хз)

	MEDIA_STOP_TIME = 58,			// UINT32	//Проверить
	MEDIA_STOP_TIME_MS = 60,		// UINT32	//Проверить
	PLAYBACK_AUDIO_VOLUME = 63,		// UINT8	//Проверить
	MEDIA_VOLUME,				// UINT8	//Проверить
  
	//get
	AUDIO_FORMAT = ATTRIBUTE_RANGE_1,	// AUDIO_FORMAT_T (Size = 0x30)
	UNKNOWN7,				// Size = 0x50
	UNKNOWN8,				// UINT8
	POSITION = ATTRIBUTE_RANGE_1 + 3,	// UINT32 (Текущая позиция в секундах)
	PAUSE_POSITION = 114			// UINT32 //Проверить
} ATTRIBUTE_NAME_T;

typedef struct RECTANGLE_LIST
{
   UINT16                  x1; // верхний левый угол
   UINT16                  y1;
   UINT16                  x2; // нижний правый угол
   UINT16                  y2; 
   struct RECTANGLE_LIST   *next_list;
} RECTANGLE_LIST_T;

typedef struct RECTANGLE_T
{
	UINT16 left_x;
	UINT16 left_y;
	UINT16 right_x;
	UINT16 right_y;
} RECTANGLE_T;


typedef struct  
{
	UINT16 				frame_num;
	UINT32 				color_key;
	UINT8  				rotation;
	UINT8  				display; // 0 - primary, 1 - secondary
	RECTANGLE_T 		base_rectangle;
	BOOL 				alpha_blending;
	UINT32 				alpha_value;
	INT16 				rectangle_off_x;
	INT16 				rectangle_off_y;
	RECTANGLE_LIST_T   *rectangle_list;
} SAFE_FRAME_ID_T;

typedef enum
{
	MMSS_MP4 = 4,
	MMSS_3GP,
	MMSS_ROM,
	MMSS_RAW,

	MMSS_WMA = 17,
	MMSS_MP3,
	MMSS_MIDI,

	MMSS_IMELODY = 21,
	MMSS_MYMIX_MIX = 22,
	MMSS_MYMIX_BASE_MIDI = 23,
	MMSS_FUNLIGHT = 24,
	MMSS_AMR = 25,
	MMSS_AAC = 26,
	MMSS_AMR_WB = 27,
	MMSS_AU = 28,
	MMSS_WAV = 29,
	MMSS_3GA = 30,
	MMSS_AMRWB_FORMAT = 31,

	MMSS_JPEG = 35,
	MMSS_JFIF = 36,
	MMSS_JFIF_AND_EXIF = 37,
    MMSS_EXIF = 38,
    MMSS_CIFF = 39,
    MMSS_GIF = 40,
    MMSS_PNG = 41,
    MMSS_BMP = 42,
    MMSS_WBMP = 43,
    MMSS_EMS_BMP = 44
	
} MMSS_FILE_FORMAT_T;

typedef enum
{
    H263_BASELINE_CODEC=1,
    H263_PROFILE_3_CODEC,
    WMV_CODEC,
    WMV9_CODEC,
    RV8_CODEC,
    RV9_CODEC,
    MPEG4_LEVEL_0_CODEC, // default
    VIDEO_NANCY_CODEC,
    VIDEO_INPUT_DEVICE  // для видео захвата

} VIDEO_CODEC_T; 

typedef enum
{
  COUNT_COLOR_2=1, 
  COUNT_COLOR_4,
  COUNT_COLOR_8, 
  COUNT_COLOR_16,
  COUNT_COLOR_32,
  COUNT_COLOR_64,
  COUNT_COLOR_128,
  COUNT_COLOR_256,
  COUNT_COLOR_512,
  COUNT_COLOR_1024,
  COUNT_COLOR_2048,
  COUNT_COLOR_4096,
  COUNT_COLOR_8192,
  COUNT_COLOR_16384,
  COUNT_COLOR_32768,
  COUNT_COLOR_65536 // 16bit, default

} COUNT_COLORS_T;

typedef enum
{
    COUNT_FRAME_RATE_1=1,
    COUNT_FRAME_RATE_2,
    COUNT_FRAME_RATE_3,
    COUNT_FRAME_RATE_4,
    COUNT_FRAME_RATE_5,
    COUNT_FRAME_RATE_6,
    COUNT_FRAME_RATE_7,
    COUNT_FRAME_RATE_10,
    COUNT_FRAME_RATE_15 // default

} COUNT_VIDEO_FRAME_RATE_T;

typedef struct MMSS_VIDEO_FORMAT_S
{
    VIDEO_CODEC_T          		video_codec; // видео-кодек
    COUNT_COLORS_T              count_colors;
    UINT32				        bit_rate;
    UINT8					    unk1; // =2
    COUNT_VIDEO_FRAME_RATE_T    frame_rate; // кол-во кадров в секунду
	UINT32    					frame_width; // ширина фрейма
    UINT32   					frame_height; // высота фрейма
	UINT32						video_sampling;
} VIDEO_FORMAT_T;

typedef enum
{
    JPEG_HEIRARCHICAL_CODEC=1,
    GIF87a_CODEC,
    GIF89a_CODEC,
    JPEG_PROGRESSIVE_CODEC,
    JPEG_BASELINE_CODEC // default
} IMAGE_CODEC_T;

typedef struct
{
    COUNT_COLORS_T          colors;
    UINT8   				unk1; // = 3 - VGA
    IMAGE_CODEC_T       	image_codec;
    UINT8   				unk2; // = 0,1,2 ???
} IMAGE_FORMAT_T;

//v3x
//Проверено 
typedef enum
{
    MMSS_AUDIO = 0,
    MMSS_VIDEO,
    MMSS_AUDIO_VIDEO,
    MMSS_IMAGE,
    MMSS_UNKNOWN		//В прошиве в3х есть. by zeDDer
} MMSS_MEDIA_TYPE_T;		//Компилятор сцуко пишет эту хрень как 4 байта (MMSS_MEDIA_TYPE_T, 0x00, 0x00, 0x00); big endian :)

//MMSS_MEDIA_TYPE_T тут есть. //Структура. Вродь её размер = 252 байта.
typedef struct
{
    MMSS_MEDIA_TYPE_T		media_type;
    AUDIO_FORMAT_T		audio_format;	
    VIDEO_FORMAT_T		video_format;
    IMAGE_FORMAT_T		image_format;
} MMSS_MEDIA_FORMAT_T;

typedef struct 
{
    UINT16   width;
    UINT16   height;
} PICTURE_SIZE_T;


//Все ф-ции проверены на v3x и работают.


// Создаёт объект для воспроизведения
UINT32 MME_FW_gc_handle_playback_create
(
	IFACE_DATA_T    *if_data,	//IFACE_DATA_T
	WCHAR 			*uri,		//путь к файлу
	SAFE_FRAME_ID_T *sf_id,		//для видео и изображений
	INT32 			unk1,		//???
	void 			*image_mem,	//указатель на изображение
	UINT32			image_size,	//размер изображения
	UINT32 			unk4,		//0 - for audio&video, 1 - for image
	void 			*unk5, 		//???
	PICTURE_SIZE_T	*picture_size	//размер картинки
);

// Создаёт объект для захвата
UINT32 MME_FW_gc_handle_capture_create (IFACE_DATA_T *if_data, SAFE_FRAME_ID_T *sf_id, MMSS_FILE_FORMAT_T file_format, MMSS_MEDIA_FORMAT_T *media_format); //file_format = 0 для аудио

//удаление объекта воспроизведения/захвата
UINT32 MME_FW_gc_handle_delete (MME_GC_MEDIA_FILE media_handle);

//Начинает воспроизведение/захват
UINT32 MME_FW_gc_handle_start (MME_GC_MEDIA_FILE media_handle, WCHAR *path);

//Останавливает воспроизведение/захват
UINT32 MME_FW_gc_handle_stop (MME_GC_MEDIA_FILE	media_handle);

//Закрывает объект воспроизведение/захват
UINT32 MME_FW_gc_handle_close (MME_GC_MEDIA_FILE media_handle);

//Приостанавливает воспроизведение/захват
UINT32 MME_FW_gc_handle_pause (MME_GC_MEDIA_FILE media_handle);

//перемотка на ms мс. от начала файла. Не проверял, но в прошиве так
UINT32 MME_FW_gc_handle_seek (MME_GC_MEDIA_FILE media_handle, UINT32 ms);

//Получить аттрибут
UINT32 MME_FW_gc_handle_get_attribute (MME_GC_MEDIA_FILE media_handle, ATTRIBUTE_NAME_T attribute_name,void *attribute_value);

//Получить размер аттрибута
UINT32 mme_fw_gc_handle_get_attrib_len(ATTRIBUTE_NAME_T attribute_name, UINT32 unk); //unk = 0;

//Установить аттрибут
UINT32 MME_FW_gc_handle_set_attribute (MME_GC_MEDIA_FILE media_handle, ATTRIBUTE_NAME_T attribute_name,void *attribute_value);



//Для совместимости с сорцами LTE.
//Воспроизведение
#define MME_GC_playback_create(ifd, uri, sf, unk1, im_ptr, im_size, unk4, unk5, pic_size) MME_FW_gc_handle_playback_create(ifd, uri, sf, unk1, im_ptr, im_size, unk4, unk5, pic_size);
#define MME_GC_playback_delete(mh) MME_FW_gc_handle_delete(mh);
#define MME_GC_playback_start(mh, uri) MME_FW_gc_handle_start(mh, uri); //uri = 0, так как путь к файлу задаётся в  MME_FW_gc_handle_playback_create. взято из прошивы. 
#define MME_GC_playback_stop(mh) MME_FW_gc_handle_stop(mh);
#define MME_GC_playback_close(mh) MME_FW_gc_handle_close(mh);
#define MME_GC_playback_pause(mh) MME_FW_gc_handle_pause(mh);
#define MME_GC_playback_seek(mh, ms) MME_FW_gc_handle_seek(mh, ms);
#define MME_GC_playback_get_attribute(mh, an, av) MME_FW_gc_handle_get_attribute(mh, an, av);
#define MME_GC_playback_set_attribute(mh, an, av) MME_FW_gc_handle_set_attribute(mh, an, av);

//захват
#define MME_GC_capture_create(ifd, sf, ff, mf) MME_FW_gc_handle_capture_create(ifd, sf, ff, mf);
#define MME_GC_capture_delete(mh) MME_FW_gc_handle_delete(mh);
#define MME_GC_capture_start(mh, uri) MME_FW_gc_handle_start(mh, uri);
#define MME_GC_capture_stop(mh) MME_FW_gc_handle_stop(mh);
#define MME_GC_capture_close(mh) MME_FW_gc_handle_close(mh);
#define MME_GC_capture_pause(mh) MME_FW_gc_handle_pause(mh);
#define MME_GC_capture_get_attribute(mh, an, av) MME_FW_gc_handle_get_attribute(mh, an, av);
#define MME_GC_capture_get_attrib_len(an) mme_fw_gc_handle_get_attrib_len(an, 0);
#define MME_GC_capture_set_attribute(mh, an, av) MME_FW_gc_handle_set_attribute(mh, an, av);

#endif
