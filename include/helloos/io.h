/*
 * -= HelloOS Educational Project =-
 * -===============================-
 *
 *  $Id: io.h 11 2005-07-22 14:23:42Z ilya $
 *
 *  Макросы для работы с портами ввода-вывода и
 *  вызова прерывания
 *  Частитчно взято из [5]
 *
 */


#ifndef __IO_H
#define __IO_H


#define call_int(int_n)    __asm__("int %0\n"::"i"(int_n))



#define outb(value,port)         \
__asm__("                        \
      outb %%al,%%dx"            \
      ::"a" (value),"d" (port)   \
      )


#define outw(value,port)         \
__asm__("                        \
      outw %%ax,%%dx"            \
      ::"a" (value),"d" (port)   \
      )


#define inb(port)                \
({                               \
unsigned char _v;                \
__asm__ volatile ("inb %%dx,%%al":"=a" (_v):"d" (port)); \
_v;                              \
})


#define inw(port)                \
({                               \
unsigned short _v;                \
__asm__ volatile ("inw %%dx,%%ax":"=a" (_v):"d" (port)); \
_v;                              \
})


#define outb_p(value,port)       \
__asm__ ("outb %%al,%%dx\n"      \
		"\tjmp 1f\n"               \
		"1:\tjmp 1f\n"             \
		"1:"::"a" (value),"d" (port))

#define inb_p(port) ({              \
unsigned char _v;                   \
__asm__ volatile ("inb %%dx,%%al\n" \
	"\tjmp 1f\n"                     \
	"1:\tjmp 1f\n"                   \
	"1:":"=a" (_v):"d" (port));      \
_v;                                 \
})


#define outw_p(value,port)       \
__asm__ ("outw %%ax,%%dx\n"      \
		"\tjmp 1f\n"               \
		"1:\tjmp 1f\n"             \
		"1:"::"a" (value),"d" (port))

#define inw_p(port) ({              \
unsigned short _v;                   \
__asm__ volatile ("inw %%dx,%%ax\n" \
	"\tjmp 1f\n"                     \
	"1:\tjmp 1f\n"                   \
	"1:":"=a" (_v):"d" (port));      \
_v;                                 \
})



#endif // __IO_H
