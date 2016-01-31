/*
 * -= HelloOS Educational Project =-
 * -===============================-
 *
 *  $Id: scrio.h 18 2005-12-07 07:28:43Z ilya $
 *
 *  ������� �����/������, � ��� ����� ���������� �����������
 *  �������.
 *
 */


#ifndef __HELLO_STDIO_H
#define __HELLO_STDIO_H


#include <helloos/types.h>
#include <stdarg.h>

#include <helloos/colors.h>


// ������������
void scrio_init(int cur_x, int cur_y);

// ����� ������
void puts(const char *s);

// ����� ������, �� ����� n ��������
void nputs(const char *s, uint n);

// ����� ������ � ����������
void puts_color(const char *s, uchar attr);

// ����� ������ � ����������, �� ����� n ��������
void nputs_color(const char *s, uint n, uchar attr);

// ����������� �������
void gotoxy(int x, int y);

// ������������� ������ �� ���� ������ �����
void scroll();

// ������� ������
void clear_screen();

// ����� ������ � hex-����. Size - ������ ������ � ������.
void PrintHex(void *val, uchar size);

// ������� ������ � ������� ���������� �� �����
void readline(char *s, uint buf_size);

// ���������� ������������ vsnprintf'�. �� ������������� ��. hello_stdio.c 
int vsnprintf(char *str, size_t maxlen, const char *format, va_list curp);

// ���������� ������������ snprintf'�. �� ������������� ��. hello_stdio.c
int snprintf(char *str, size_t maxlen, const char *format, ...);

// ���������� ������������ printf'�. �� ������������� ��. hello_stdio.c
int printf(char *format, ...);

// ������� printf
int printf_color(uchar attr, char *format, ...);

char scan2ascii(byte);

#endif // __HELLO_STDIO_H
