/*
 * -= HelloOS Educational Project =-
 * -===============================-
 *
 *  $Id: string.h 18 2005-12-07 07:28:43Z ilya $
 *
 *  ������� ������ �� ��������, ������� ������� �������
 *  �� ����������� ����������
 *
 */


#ifndef __HELLO_STRING_H
#define __HELLO_STRING_H



#include <helloos/types.h>


void *memcpy(void *dest, const void *src, size_t n);
void *memmove(uchar *dest, const uchar *src, size_t n);
void *memset(void *s, int c, size_t n);
int strcmp(char *a, char *b);
int strncmp(char *a, char *b, uint n);
int strlen(char *s);
char *strchr(char *s, int c);



#endif // __HELLO_STRING_H
