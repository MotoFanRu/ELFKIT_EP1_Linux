#define DEBUG

#ifndef XML_H
#define XML_H


#include <apps.h>
#include <typedefs.h>
#include <utilities.h>

typedef struct
{
	char   name[32];
	char   string[32];
} ptag_t; // тег 2го уровня

typedef struct
{
	char	name[32];
	ptag_t  *ptag;
	UINT32	count_ptags;
}tag_t;

typedef enum
{
	MODE_NAME,
	MODE_STRING
} PARSE_MODES_T;

void InitTag(char *name, tag_t *tag);
char* ReadTag(char *buffer, tag_t *tag, UINT32 num);
char* GetPTagString(tag_t tag, char *name_ptag);
void CloseTag(tag_t *tag);

#endif
