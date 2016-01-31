/*
 * -= HelloOS Educational Project =-
 * -===============================-
 *
 *  $Id: binfmt.h 18 2005-12-07 07:28:43Z ilya $
 *
 *  ��������� ��� ������ � ��������� �������
 *
 */


#ifndef __BINFMT_H
#define __BINFMT_H

#include <helloos/types.h>


// ��������� ��������� ������� �������� ������
typedef struct
{
   char *FormatName;    // �������� �������
   bool (*is)(char*);   // �������, ������������ ��������� �� ������ ����
                        // � ����� �������
   void (*dump_info)(char*);  // ������� ������ ���������� ����� �������
   uint (*load_bin)(char*, char*);   // ������� ������� ������������ �����
   addr_t (*load_page)(uint); // �������, ����������� �������������
                              // �������� (���������� �� #PF)
} BinFmt;


// ���������, ������������ �������� �������
// ��� ������ ��������� ��������������� ������� ������� BinFormats
// ��������� ��������������� ������
#define BIN_ELF      0

// ���������� �������� / ������� � ������� BinFormats
#define BIN_N        1

extern BinFmt BinFormats[BIN_N];


// ���������� ������ ����� (-1 ��� �������)
int bin_type(char *name);
// ���������� ��������� ����� (0 ��� �������)
bool bin_dump_info(char *name);
// ��������� ����
// ���������� ��� PID ��� (uint)-1 � ������ �������
uint bin_load_bin(char *name, char* args);


typedef struct
{
   ushort index_in_catalog;
   ulong  table_address;
} SysPageTableRef;

extern SysPageTableRef sys_pagerefs[];
extern ushort sys_pagerefs_n;

void add_sys_pageref(ushort index, ulong address);



#endif // __BINFMT_H
