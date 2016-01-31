/*
 * -= HelloOS Educational Project =-
 * -===============================-
 *
 *  $Id$
 *
 *  ���������� ����������� ������� �����-������
 *  ��� ���������������� ��������
 *
 */


#include <helloos/types.h>
#include <helloos/user_syscalls.h>
#include <stdarg.h>
#include <string.h>

// �������� ��� vsnprintf � ��.
#include "stdio.inc"

// ����� ���������� ������������ vprintf � ���������� �����
// ������������ vsnprintf, � ����� puts
// ����� �������� �� ����� 256(-1) ��������
// ���� ������ ���������� � puts_color
int vprintf_color(uchar attr, char *format, va_list curp)
{
   char Buffer[256];
   uint res = vsnprintf(Buffer, 256, format, curp);
   sys_nputs_color(Buffer, 256, attr);
   return res;
}

// ���������� ������������ vprintf ����� vprintf_color
int vprintf(char *format, va_list curp)
{
   return vprintf_color(0x0e, format, curp);
}

// ������� printf ����� vprintf_color
int printf_color(uchar attr, char *format, ...)
{
   va_list curp;
   uint res;
   va_start(curp, format);
   res = vprintf_color(attr, format, curp);
   va_end(curp);
   return res;
}

// ���������� ������������ printf ����� vprintf_color
int printf(char *format, ...)
{
   va_list curp;
   uint res;
   va_start(curp, format);
   res = vprintf_color(0x0e, format, curp);
   va_end(curp);
   return res;
}



// ����� � ������ ��� ����������� �����
void puts_color(const char *s, uchar attr)
{
   sys_nputs_color(s, -1, attr);
}


// ����� ��� ����� ��� �����
void puts(const char *s)
{
   sys_nputs_color(s, -1, 0x0e);
}


// ����� ��� �����, � ������������ �����
void nputs(const char *s, uint n)
{
   sys_nputs_color(s, n, 0x0e);
}

uchar CharTo83(uchar c)
{
   // �������� ����������� ������� �� �������������
   // ������������ ��������:
   //  - ������� 0x20
   //  - " * + , / : ; < = > ? [ \ ] |
   //  ������ � �������� ������������� ������ �������� ������� ascii (���� ;)
   if (c<0x20 || c==0x22 || c==0x2a || c==0x2b || c==0x2c || c==0x2e || c==0x2f || (c>=0x3a && c<=0x3f) || c==0x5b || c==0x5c || c==0x5d || c >= 0x80)
      c = '_';

   // �������� � �������� ��������
   if (c >= 0x61 && c <= 0x7a) c -= 0x20;

   return c;
}

void Make83Name(char *fullname, char *name83)
{
   int len = 0, i, j;

   // �������������� ��������� ���������
   for (i = 0; i < 11; i++) name83[i] = 0x20;

   // �������� ������������ . � ..
   if (strcmp(fullname, ".") == 0)
   {
      name83[0] = '.';
      return;
   }
   if (strcmp(fullname, "..") == 0)
   {
      name83[0] = '.'; name83[1] = '.';
      return;
   }

   // �������� 8 �������� ��� �����
   i = 0;
   while ((len <= 8)  &&  (i < strlen(fullname))  &&  (fullname[i] != '.'))
      name83[len++] = CharTo83(fullname[i++]);

   // ���� �����
   for (i = 0; i < strlen(fullname); i++) if (fullname[i] == '.') break;

   // ���� ����� - �������� ����������
   if (i < strlen(fullname))
   {
      j = 8;
      i++;
      while ((j < 11)  &&  (i < strlen(fullname)))
         name83[j++] = CharTo83(fullname[i++]);
   }

   // ������ ������ �� ����� ���� �������� (��������, ��� ".foo")
   if (name83[0] == 0x20)
      name83[0] = '_';
}
