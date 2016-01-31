/*
 * -= HelloOS Educational Project =-
 * -===============================-
 *
 *  $Id: types.h 18 2005-12-07 07:28:43Z ilya $
 *
 *  Сокращенные определения часто используемых
 *  типов
 *
 */


#ifndef __TYPES_H
#define __TYPES_H



// Полезные shortcut'ы
typedef unsigned char   uchar;
typedef unsigned char   byte;
typedef unsigned short  ushort;
typedef unsigned int    uint;
typedef unsigned int    ulong;

// Типы используются для того, чтобы подчеркнуть,
// что переменная содержит адрес, смещение, индекс
typedef unsigned int    addr_t;
typedef unsigned int    offs_t;
typedef unsigned int    indx_t;

// Для стандартных функций
typedef unsigned int    size_t;


typedef uchar bool;




// Структура для одного дескриптора в системных
// таблицах типа GDT.
typedef struct _Descriptor Descriptor;
struct _Descriptor
{
   ulong a, b;
};

// Эта структура используется для вызова инструкций sgdt/lgdt
typedef struct _GDTDescriptor GDTDescriptor;
struct _GDTDescriptor
{
   ushort Size; // Размер GDT - 1
   Descriptor *Addr; // Адрес GDT
} __attribute__((packed));



#define MAX(a,b) ((a)>(b) ? (a) : (b))
#define MIN(a,b) ((a)<(b) ? (a) : (b))


#endif // __TYPES_H
