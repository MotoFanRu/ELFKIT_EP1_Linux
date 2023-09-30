#ifndef ELFFORMAT_H
#define ELFFORMAT_H

    #include <typedefs.h>

    #define ELF32_R_SYM(i)		((i) >> 8)
    #define ELF32_R_TYPE(i)		((i) & 0xff)

    #define SHN_COMMON      0xfff2

    typedef UINT32 DWORD;
    typedef UINT32 INT;
    typedef DWORD * PDWORD;
    typedef UINT16 WORD;
    typedef UINT8  BYTE;

	typedef struct
	{
		BYTE  Magic[ 4 ];
		BYTE  Bitness;
		BYTE  Endian;
		BYTE  ElfVer1;
		BYTE  Res[ 9 ];
		WORD  FileType;
		WORD  Machine;
		DWORD ElfVer2;
		DWORD EntryPoint;
		DWORD PhTableOffset;
		DWORD ShTableOffset;
		DWORD Flags;
		WORD  FileHeadSize;
		WORD  PhTabEntSize;
		WORD  PhTabNumEnts;
		WORD  ShTabEntSize;
		WORD  SectionsNumb;
		WORD  ShstrTabIndex;
	} SElfFile, *PElfFile;
	
	typedef struct
	{
		DWORD SectionName;
		DWORD Type;
		DWORD Flags;
		DWORD VirtualAddress;
		DWORD Offset;
		DWORD Size;
		DWORD Link;
		DWORD Info;
		DWORD Align;
		DWORD EntrySize;
	} SElfSection, *PElfSection;
	
	typedef struct
	{
		DWORD Address;
		DWORD Info;
		DWORD Addend;
	} SElfRelocation, *PElfRelocation;
	
	typedef struct
	{
		DWORD Name;
		DWORD Value;
		DWORD Size;
		unsigned Type : 4;
		unsigned Binding : 4;
		BYTE Zero;
		WORD Section;
	} SElfSymbol, *PElfSymbol;
	
	typedef struct
	{
		char Name[64];
		unsigned SAddress;
	} ElfFunctions;
	
#endif
