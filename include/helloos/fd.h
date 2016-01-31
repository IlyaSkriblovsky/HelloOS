/*
 * -= HelloOS Educational Project =-
 * -===============================-
 *
 *  $Id: fd.h 8 2005-07-22 09:25:42Z ilya $
 *
 *  ��������� �������� ������-���������
 *
 */


#ifndef __FD_H
#define __FD_H


#include <helloos/types.h>


// ���������, ������������ ������ ������ ������-�����������
// ��. [6] � [7].

// Main Status Register
#define FD_MSR   0x3f4

// Digital Output Register
#define FD_DOR   0x3f2

// FIFO, ������� ������ � ������
#define FD_FIFO  0x3f5

extern uint fd_cache_pos;
// ������������ �������� � �����������
void fd_init();

// ������ �������
void fd_write_sector(uint lba, uchar *buf);

// ������ �������
void fd_read_sector(uint lba, uchar *buf);


#endif // __FD_H
