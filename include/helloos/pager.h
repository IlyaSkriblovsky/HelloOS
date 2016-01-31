/*
 * -= HelloOS Educational Project =-
 *
 *  $Id: pager.h 16 2005-07-29 10:55:57Z ilya $
 *
 *  ��������� ��������� ������
 *
 */


#ifndef __PAGER_H
#define __PAGER_H


#include <helloos/types.h>
#include <helloos/scheduler.h>

void pager_init();

ulong alloc_first_page();
void free_page(ulong ptr);

void map_page(addr_t phaddr, TaskStruct *task, addr_t vaddr, uint flags);


// ����������� CR3 ��������� ��� �������
#define reload_cr3()  { __asm__("mov %cr3, %eax ; mov %eax, %cr3"); }

// ���������� ��������� �� ������ ��������, ������� �����������
// ��������-���������
#define PAGE_ADDR(addr) ((addr) & 0xfffff000)

// ������ ��������
#define PAGE_SIZE    0x1000


// �������� ������� � ������ �������
// �����������
#define PA_P      1
// ����������� �� ������
#define PA_W      2
// ����������� ��� CPL=3
#define PA_USER   4
// � �������� ���� ��������� (accessed)
#define PA_A      32
// � �������� ���� ������
#define PA_D      64

// �����, ������������ HelloOS
// �������� �� ������� �����������
#define PA_NONFREE   512


// ����� ������ ��������
#define PA_MASK      0xfff


// �������� (�����) ����������� �������. FIXME: ��� ������ ����
// �������, � ����� ��������� �� ������ ����.
#define PAGE_ATTR    0x7

// ����� ��� ��������� ������ �������
#define SYS_PAGE_ATTR    0x5


#endif // __PAGER_H
