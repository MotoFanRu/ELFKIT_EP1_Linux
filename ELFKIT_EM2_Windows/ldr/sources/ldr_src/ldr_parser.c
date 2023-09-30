#include "ldr_parser.h"
#include "ldr_features.h"

//(+)
UINT32 ldrParseImage (BYTE *image, ldrElf *app)
{
    PElf32_Ehdr header;
    PElf32_Shdr section;
    PElf32_Rela relocation;
    UINT32      section_index;
    UINT32      relocation_index;

    //проверка версии эльфа
    header = (PElf32_Ehdr)image;
    if(header->Magic != 0x7F454C46)
    {
        PFprintf("[ELF_LDR] This is not ELF file\n");
        return 0;
    }

    if(header->Bitness != 1)
    {
        PFprintf("[ELF_LDR] 64-bit ELF not supported, only 32-bit\n");
        return 0;
    }

    if(header->Endian != 2)
    {
        PFprintf("[ELF_LDR] Little endian ELF not supported, only Big\n");
        return 0;
    }
    
    if(header->ElfVer1 != 1)
    {
        PFprintf("[ELF_LDR] Bad ELF version: %u\n", header->ElfVer1);
        return 0;
    }
    
    if(header->FileType != 0 && header->FileType != 1 && header->FileType != 2)
    {
        PFprintf("[ELF_LDR] File is not relocatable ELF\n");
        return 0;
    }
    
    if(header->Machine != 0x27)
    {
        PFprintf("[ELF_LDR] This is not M*CORE ELF (this is 4 Machine => %x)\n", header->Machine);
        return 0;
    }
    
    if(header->ElfVer2 != 1)
    {
        PFprintf("[ELF_LDR] Bad ELF version: %lu\n", header->ElfVer2);
        return 0;
    }

    //задание адресов секций и поиск точки входа
    for(section_index = 0; section_index < header->SectionsNumb; section_index++)
    {
        section = (PElf32_Shdr)(image + header->ShTableOffset + header->ShTabEntSize * section_index);
        section->VirtualAddress = (DWORD)image + section->Offset;

        //точка входа
        if(section->Type == 1 && section->Flags == 6)
            header->EntryPoint += section->VirtualAddress;
    }

    //проверяем точку входа
    if(header->EntryPoint <= (DWORD)image) 
    {
        PFprintf("[ELF_LDR] Can't find entry point\n");
        return 0;
    }

    //релокация секций
    for(section_index = 0; section_index < header->SectionsNumb; section_index++)
    {
        section = (PElf32_Shdr)(image + header->ShTableOffset + header->ShTabEntSize * section_index);
        if(section->Type == 4 || section->Type == 9)
        {
            for(relocation_index = 0; relocation_index < section->Size / section->EntrySize; relocation_index += 1)
            {
                relocation = (PElf32_Rela)(image + section->Offset + section->EntrySize * relocation_index);
                if(ldrRelocateSection(image, section, relocation) != 0) return 0;
            }
        }
    }

    //инициализация app
    memset(app, 0, sizeof(ldrElf));
    app->image = image;

    //возвращаем точку входа
    PFprintf("[ELF_LDR] Elf mapped at 0x%X with entry point at 0x%X\n", image, header->EntryPoint);
    return header->EntryPoint;
}

//(?)
UINT32 ldrRelocateSection (BYTE *image, PElf32_Shdr section, PElf32_Rela relocation)
{
    PElf32_Ehdr header;
    PElf32_Sym  link_symbol;
    PElf32_Shdr link_section;
    PElf32_Shdr temp_section;
    PElf32_Shdr rela_section;
    PDWORD      rela_address;
    char        *symbol_name;
    int         symbol_value;

    //заголовок elf файла
    header  = (PElf32_Ehdr)image;

    //секция к которой будет применятся релокация
    rela_section = (PElf32_Shdr)(image + header->ShTableOffset + header->ShTabEntSize * section->Info);
    rela_address = (PDWORD)(rela_section->VirtualAddress + relocation->Address);

    //link секция (symtab). к этой секции относится symbol
    link_section = (PElf32_Shdr)(image + header->ShTableOffset + header->ShTabEntSize * section->Link);            
    link_symbol = (PElf32_Sym)(image + link_section->Offset) + ELF32_R_SYM(relocation->Info);

    //внешний symbol! (константа, ивент, функция из либы)
    if(link_symbol->Section == 0)
    {
        temp_section = (PElf32_Shdr)(image + header->ShTableOffset + header->ShTabEntSize * link_section->Link);        
        symbol_name = (char *)image + temp_section->Offset + link_symbol->Name;

        LIBRARY_RECORD_T *rec = ldrSearchInDefLibrary(symbol_name);
        if(rec == NULL) 
        {
            PFprintf("[ELF_LDR] Undefined external symbol '%s'\n", symbol_name);
            return 1;
        }
        symbol_value = rec->record_value;
        *rela_address = symbol_value;
        return 0;
    }


    //все остальные symbols
    temp_section = (PElf32_Shdr)(image + header->ShTableOffset + header->ShTabEntSize * link_symbol->Section);
    symbol_value = link_symbol->Value + temp_section->VirtualAddress;
    

    //релокация
    switch(ELF32_R_TYPE(relocation->Info))
    {
        case 1:
            *rela_address = symbol_value + relocation->Addend;
            break;

        case 2:
            *rela_address = symbol_value + relocation->Addend - relocation->Address;
            break;

        case 6:
        case 7:
            *rela_address = symbol_value;
            break;

        default:
            return 2;
    }

    return 0;
}

