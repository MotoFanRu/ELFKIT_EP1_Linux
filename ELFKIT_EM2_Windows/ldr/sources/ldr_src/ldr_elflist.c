#include "ldr_elflist.h"
#include "ldr_features.h"

///Data
ELF_LIST_T loadedElfsList = {0, NULL, NULL};

///Code
BOOL ldrIsLoaded (const char *name)
{
    return ldrFindElf(name) != NULL;
}

ldrElf * ldrGetElfsList (UINT32 *count)
{
    if (count)
        *count = loadedElfsList.count;
    
    return loadedElfsList.root;
}

ldrElf * ldrFindElf (const char *name)
{
    if (!name) return NULL;

    ldrElf *eapp = loadedElfsList.root;
    while (eapp != NULL)
    {
        if (!strncmp((char *)name, eapp->name, 13)) 
            return eapp;
        eapp = eapp->next;
    }
    
    return NULL;
}

ldrElf * elfListAdd (ldrElf *elf)
{
    if (!elf) return NULL;
    
    if (loadedElfsList.root == NULL) 
        loadedElfsList.root = elf;
    else
        loadedElfsList.tail->next = elf;
    
    loadedElfsList.tail = elf;
    loadedElfsList.count++;
    elf->next = NULL;
    
    AFW_CreateInternalQueuedEvAux(EV_PM_ELF_LIST_CHANGED, FBF_LEAVE, 0, 0);
    return elf;
}

ldrElf * elfListFind (ldrElf *elf)
{
    ldrElf *pelf = NULL;
    if(!elf) return NULL;
    
    pelf = loadedElfsList.root;
    while(pelf)
    {
        if(memcmp(elf, pelf, (sizeof(ldrElf))) == 0)
            return pelf;

        pelf = pelf->next;
    }
    
    return NULL;
}

ldrElf * elfListFindAndDel (ldrElf *eapp)
{
    ldrElf *elf, *pelf = NULL;
    if(!eapp) return NULL;
    
    elf = loadedElfsList.root;
    while(elf)
    {
        if(memcmp(eapp, elf, (sizeof(ldrElf))) == 0)
        {
            if (elf->next == NULL)        // deleting the last one
                loadedElfsList.tail = pelf;
            
            if (pelf == NULL)
                loadedElfsList.root = elf->next; // deleting the first one
            else 
                pelf->next = elf->next; // deleting from the middle
            
            loadedElfsList.count--;
            
            AFW_CreateInternalQueuedEvAux(EV_PM_ELF_LIST_CHANGED, FBF_LEAVE, 0, 0);
            return elf;
        }
        pelf = elf;
        elf = elf->next;
    }
    
    return NULL;
}
