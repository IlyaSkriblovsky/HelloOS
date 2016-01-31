/*
 * -= HelloOS Educational Project =-
 * -===============================-
 *
 *  $Id: types.h 18 2005-12-07 07:28:43Z ilya $
 *
 *  ����������� ����������� ����� ������������
 *  �����
 *
 */


#ifndef __TYPES_H
#define __TYPES_H



// �������� shortcut'�
typedef unsigned char   uchar;
typedef unsigned char   byte;
typedef unsigned short  ushort;
typedef unsigned int    uint;
typedef unsigned int    ulong;

// ���� ������������ ��� ����, ����� �����������,
// ��� ���������� �������� �����, ��������, ������
typedef unsigned int    addr_t;
typedef unsigned int    offs_t;
typedef unsigned int    indx_t;

// ��� ����������� �������
typedef unsigned int    size_t;


typedef uchar bool;




// ��������� ��� ������ ����������� � ���������
// �������� ���� GDT.
typedef struct _Descriptor Descriptor;
struct _Descriptor
{
   ulong a, b;
};

// ��� ��������� ������������ ��� ������ ���������� sgdt/lgdt
typedef struct _GDTDescriptor GDTDescriptor;
struct _GDTDescriptor
{
   ushort Size; // ������ GDT - 1
   Descriptor *Addr; // ����� GDT
} __attribute__((packed));



#define MAX(a,b) ((a)>(b) ? (a) : (b))
#define MIN(a,b) ((a)<(b) ? (a) : (b))


#endif // __TYPES_H
