#include "xmlparser.h"

char * StrStr(char *str1, char *str2)
{
    UINT32 len_str1 = strlen(str1);
    UINT32 len_str2 = strlen(str2);
    UINT32 i=0, j=0;


    for (i=0; i < len_str1; i++)
    {
        if (str1[i] == str2[j])
        {
            if (j+1 == len_str2) return str1+i-j;
            else j++;
        }
        else j=0;
    }

    return NULL;
}

void InitTag(char *name, tag_t *tag)
{
    if (name != NULL)
    {
        tag->ptag = NULL;
        strcpy(tag->name, "<");
        strcat(tag->name, name);
        dbgf("tag = %s", name);
    }

    tag->ptag = NULL;
}

void CloseTag(tag_t *tag)
{
    if (tag->ptag != NULL)
    {
        free(tag->ptag);
        tag->ptag = NULL;
    }
}



char* ReadTag(char *buffer, tag_t *tag, UINT32 num)
{
	char	*buf;
	UINT32  i=0, j=0, n=0, k=0;
	UINT8	parse_mode=MODE_NAME;

    if (buffer == NULL || buffer[0] == 0) return NULL;

    buf = buffer;
    for (i=0; i < num; i++)
    {
        buf = StrStr( buf, tag->name );
        if (buf == buffer || buf == NULL)
        {
            dbgf("tag %d not found", i+1);
            return NULL; // не найдено
        }
        buf = buf + strlen(tag->name);
    }
    dbgf("Read tag %d", num);



	tag->count_ptags = 0;

	// посчитаем кол-во значений
    i=0;
	while (buf[i] != '>')
	{
		if (buf[i] == '=')
            tag->count_ptags++;
		i++;
	}

    dbgf("tag->count_ptags = %d\n", tag->count_ptags);

	tag->ptag = (ptag_t*)malloc(sizeof(ptag_t)*tag->count_ptags);
    if (tag->ptag == NULL) return NULL;


	for (k=0; k < i; k++)
	{
		if (buf[k] == ' ' || buf[k] == '/') continue; // пропускать

		if (parse_mode == MODE_NAME)
		{
			if (buf[k] == '=')
			{
                dbgf("tag_name = %s", tag->ptag[n].name);
                j=0;
                parse_mode = MODE_STRING;
                continue;
            }
            tag->ptag[n].name[j] = buf[k];
            tag->ptag[n].name[j+1] = 0;

            j++;
		}
		else if (parse_mode == MODE_STRING)
		{
			if (buf[k] == '"' && j==0) continue;
			if (buf[k] == '"')
			{
                dbgf("tag_string = %s", tag->ptag[n].string);
                j=0;
                parse_mode = MODE_NAME;
                n++;
                continue;
            }
			tag->ptag[n].string[j] = buf[k];
			tag->ptag[n].string[j+1] = 0;

			j++;
		}
	}

	return buf+k;
}

char* GetPTagString(tag_t tag, char *name_ptag)
{
	INT32	i;

    if (tag.ptag == NULL || name_ptag == NULL) return NULL;

	for (i=0; i < tag.count_ptags; i++)
	{
		if (!strcmp(tag.ptag[i].name, name_ptag))
            return tag.ptag[i].string;
	}

	return NULL;
}
