#ifndef LDR_ELFLIST_H
#define LDR_ELFLIST_H

#include <loader.h>
#include <utilities.h>

typedef struct
{
	UINT32	count;
	ldrElf	*root;
	ldrElf	*tail;
} ELF_LIST_T;

BOOL	 ldrIsLoaded (const char *name);
ldrElf * ldrFindElf  (const char *name);
ldrElf * ldrGetElfsList (UINT32 *count);

ldrElf * elfListAdd  (ldrElf *elf);
ldrElf * elfListFind (ldrElf *elf);
ldrElf * elfListFindAndDel (ldrElf *elf);

#endif
