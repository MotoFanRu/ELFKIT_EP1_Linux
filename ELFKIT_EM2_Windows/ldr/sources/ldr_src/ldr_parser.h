#ifndef LDR_PARSER_H
#define LDR_PARSER_H

#include <loader.h>
#include <utilities.h>

#define ELF32_R_SYM(i)  ((i) >> 8)
#define ELF32_R_TYPE(i) ((i) & 0xff)

#define ELF32_S_BIND(i) ((i) >> 4)
#define ELF32_S_TYPE(i) ((i) & 0x0f)

typedef UINT32 INT;

typedef struct
{
    DWORD Magic;            //0x00    (EI_MAG0 + EI_MAG1 + EI_MAG_2 + EI_MAG3 )
    BYTE  Bitness;          //0x04    (EI_CLASS)
    BYTE  Endian;           //0x05    (EI_DATA)
    BYTE  ElfVer1;          //0x06    (EI_VERSION)
    BYTE  Res[ 9 ];         //0x07    (EI_OSABI + EI_ABIVERSION + EI_PAD + EI_NIDENT)

    WORD  FileType;         //0x10    (E_TYPE)
    WORD  Machine;          //0x12    (E_MACHINE)

    DWORD ElfVer2;          //0x14    (E_VERSION)
    DWORD EntryPoint;       //0x18    �������� �� ����� ����� ������������ ������ �����

    DWORD PhTableOffset;    //0x1C    �������� ������� program header ������������ ������ �����
    DWORD ShTableOffset;    //0x20    �������� ������� section header ������������ ������ �����

    DWORD Flags;            //0x24    (E_FLAGS)    
    WORD  FileHeadSize;     //0x28    ������ ELF-��������� � ������ 

    WORD  PhTabEntSize;     //0x2A    ������ ����� ������ � program header � ������
    WORD  PhTabNumEnts;     //0x2C    ���������� ������� � program header

    WORD  ShTabEntSize;     //0x2E    ������ ����� ������ � section header � ������
    WORD  SectionsNumb;     //0x30    ���������� ������� � section header

    WORD  ShstrTabIndex;    //0x32    ��������� ������������ ��������� ������ .shstrtab � ������� ���������� ������.
} Elf32_Ehdr, *PElf32_Ehdr; //������ = 0x34 ����

typedef struct
{
    DWORD SectionName;      //0x00
    DWORD Type;             //0x04
    DWORD Flags;            //0x08
    DWORD VirtualAddress;   //0x0C
    DWORD Offset;           //0x10
    DWORD Size;             //0x14
    DWORD Link;             //0x18
    DWORD Info;             //0x1C
    DWORD Align;            //0x20
    DWORD EntrySize;        //0x24
} Elf32_Shdr, *PElf32_Shdr; //������ = 0x28 ����
    
typedef struct
{
    DWORD Address;          //0x00
    DWORD Info;             //0x04
    DWORD Addend;           //0x08
} Elf32_Rela, *PElf32_Rela; //������ = 0x0C ����
    
typedef struct
{
    DWORD Name;
    DWORD Value;
    DWORD Size;
    BYTE Info;
    BYTE Zero;
    WORD Section;
} Elf32_Sym, *PElf32_Sym;

UINT32 ldrParseImage (BYTE *image, ldrElf *app);
UINT32 ldrRelocateSection (BYTE *image, PElf32_Shdr section, PElf32_Rela relocation);

#endif
