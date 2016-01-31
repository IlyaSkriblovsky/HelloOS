/*
 * -= HelloOS Educational Project =-
 * -===============================-
 *
 *  $Id: elf.h 18 2005-12-07 07:28:43Z ilya $
 *
 *  ��������� � ��������� ��� ������ � ELF
 *
 *  ��. [11] - �������� ����!
 *
 *  ������� [*] �������� ���� � ���������, �������
 *  ������ ������������ HelloOS ��� �������� ������
 *
 */




#ifndef __ELF_H
#define __ELF_H

#include <helloos/types.h>


// ����������, �������� �� ���� ELF
bool elf_is(char *Name);
// ���������� ��������� ELF
void elf_info(char *Name);
// ��������� ����
uint elf_load(char *Name, char *arg);
// ���������� #PF ��� ELF-����������
addr_t elf_pf(addr_t address);


// ������ ���� e_ident
#define EI_NIDENT 16

// ��������� ELF
typedef struct {
   uchar    e_ident[EI_NIDENT];  // ������������� �����
   ushort   e_type;              // ��� ���������� �����
   ushort   e_machine;           // �����������
   ulong    e_version;           // ������ [*]
   addr_t   e_entry;             // ����� �����
   offs_t   e_phoff;             // �������� ���������� ���������
   offs_t   e_shoff;             // �������� ������� ������
   ulong    e_flags;             // ����� (��� 386 ��� ������) [*]
   ushort   e_ehsize;            // ������ ��������� [*]
   ushort   e_phentsize;         // ������ ������ ��������� ���������
   ushort   e_phnum;             // ���������� ���������� ���������
   ushort   e_shentsize;         // ������ ��������� ������
   ushort   e_shnum;             // ���������� ������
   ushort   e_shstrndx;          // ������ ������, ���������� �������� ������
}  Elf32_Ehdr;


// ���� ���������� �����
// HelloOS ��������� ������ ET_EXEC

// ��� �������
#define ET_NONE   0
// ������������
#define ET_REL    1
// �����������
#define ET_EXEC   2
// ������������ ����������
#define ET_DYN    3
// Core-����
#define ET_CORE   4
// �������� ����������� ��� ���������� ��������
#define ET_LOPROC 0xff00
#define ET_HIPROC 0xffff


// �����������
// HelloOS ��������� ������ EM_386

// ��� �������
#define EM_NONE   0
// AT&T WE 32100
#define EM_M32    1
// SPARC
#define EM_SPARC  2
// Intel 80386
#define EM_386    3
// Motorola 68000
#define EM_68K    4
// Motorola 88000
#define EM_88K    5
// Intel 80860
#define EM_860    7
// MIPS RS3000
#define EM_MIPS   8


// ������ �������. � ����, �� ������� � ������ ELF
// ����������� ������ 1, �, ��������� � ����, ������
// ���� �� ���������� [*]
#define EV_NONE      0
#define EV_CURRENT   1


// �������� � e_ident

// �������� ������ ����� (�����������)
#define EI_CLASS  4
// �������� ������� ������ (low/high-endian)
#define EI_DATA   5
// �������� ������ ����� [*]
#define EI_VERSION   6
// ������ ������������� ������ [*]
#define EI_PAD    7


// ������ (�����������) ������
// HelloOS ��������� ������ ELFCLASS32

#define ELFCLASSNONE    0
#define ELFCLASS32      1
#define ELFCLASS64      2


// ������� ������
// HelloOS ��������� ������ ELFDATA2LSB

#define ELFDATANONE  0
// low-endian
#define ELFDATA2LSB  1
// high-endian
#define ELFDATA2MSB  2


//=================================//


// ���������������� ������ ������ [*]

// �������������� ������. ��� ��������� ������������ ���
// ������� � �������, ��� ������� �� ����� ��������� �����
// ������.
#define SHN_UNDEF       0
// ������ �������� ����������������� �������. ������ �
// ������ �������� � ����� �� ����� (��� ��! 65 ���. ������!)
#define SHN_LORESERVE   0xff00
// �������� ������������-����������� ������
#define SHN_LOPROC      0xff00
#define SHN_HIPROC      0xff1f
// ���� � ������ ����� SHN_ABS, �� ����� ����������
#define SHN_ABS         0xfff1
// ���� � ������ ����� SHN_COMMON, �� ��� extern-���������� � �
#define SHN_COMMON      0xfff2
#define SHN_HIRESERVE   0xffff


// ��������� ����������� ������ [*]
typedef struct
{
   ulong    sh_name;
   ulong    sh_type;
   ulong    sh_flags;
   addr_t   sh_addr;
   offs_t   sh_offset;
   ulong    sh_size;
   ulong    sh_link;
   ulong    sh_info;
   ulong    sh_addralign;
   ulong    sh_entsize;
} Elf32_Shdr;

// ���� ������ [*]
// �� ������ ������ � ��������� ��� ���������� � �������
// � ��� �������� ��������� ������ ��������� ���������
// (program headers). ��������������, ��� ������������ �
// ����������� ���������� ������������.

// ��������� ������
#define SHT_NULL     0
// ���������� ������� �� ���������. ������ ��� �����������
// ������ � ����� ��� �������.
#define SHT_PROGBITS 1
// ������� ��������
#define SHT_SYMTAB   2
// ������� �����
#define SHT_STRTAB   3
// Relocation entries with explicit addends (?)
#define SHT_RELA     4
// ���-������� �������� (?)
#define SHT_HASH     5
// ������� ��� ������������ ��������
#define SHT_DYNAMIC  6
// �������������� ����������
#define SHT_NOTE     7
// ����� ��, ��� PROGBITS, �� �� �������� ������. ������ ���
// ������ .bss.
#define SHT_NOBITS   8
// Relocation entries without explicit addends (?)
#define SHT_REL      9
// ���� ��� ��������������, �� �� ���������
#define SHT_SHLIB    10
// ����������� ������� ��������, ����������� ��� ��������
#define SHT_DYNSYM   11
// �������������� ���������
#define SHT_LOPROC   0x70000000
#define SHT_HIPROC   0x7fffffff
#define SHT_LOUSER   0x80000000
#define SHT_HIUSER   0xffffffff

// ����� ������ [*]

// ������ ���� �������� ��� ������ �� ����� ������ ���������
#define SHF_WRITE       0x1
// ������ ���� ��������� � ������ ���������
#define SHF_ALLOC       0x2
// ������ ���� �������� ��� ���������� ������������ ����������
#define SHF_EXECINSTR   0x4
// ����� ��� ������������-����������� ������
#define SHF_MASKPROC    0xf0000000


//=================================//

// ��������� ��������� ���������. ������ ��� �������� ����������
// ������������ ������ ��� ���������, ������������ �������� ����
// � ������. ��� ����������, GCC ����� ���� ���� ��������� ����������
// ��� � ������� ����� �� ��������� ����������: ������=0 � ��������=0.
// � ���� �� ���� ������� ����� �� ������������ � ������������ ���.
// ������� ���� ������� ������������, � ���� ������������� � ������ ��
// �������� � ��� ���� ���������� USER_STACK_PAGES �������.
typedef struct
{
   ulong    p_type;     // ��� �������� [*]
   offs_t   p_offset;   // �������� � �����
   addr_t   p_vaddr;    // ����������� ����� � �� ��������
   addr_t   p_paddr;    // ���������� ����� � �� �������� [*]
   ulong    p_filesz;   // ������ � �����
   ulong    p_memsz;    // ������ � ������ (����� ���� ������ ��� � �����,
                        // � ����� ������ ����������� ������)
   ulong    p_flags;    // �����
   ulong    p_align;    // ������������ �� ���� ��������
} Elf32_Phdr;

// ���� ���������
// HelloOS ��������� ��� ��������, � ������� ��������� ����� ����������.
// ��� ������������ �� p_vaddr � p_memsz. ��� ���� ��� ������������.

// ��������������
#define PT_NULL      0
// �����������
#define PT_LOAD      1
// ���������� ��� ������������ ��������
#define PT_DYNAMIC   2
// ��������� �� ��� ��������������
#define PT_INTERP    3
// �������������� ����������
#define PT_NOTE      4
// ���������������, �� �� ����������
#define PT_SHLIB     5
// ���������� ���� ������� ���������
#define PT_PHDR      6
// �������� ������������-����������� �����
#define PT_LOPROC    0x70000000
#define PT_HIPROC    0x7fffffff

#endif // __ELF_H
