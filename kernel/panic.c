/*
 * -= HelloOS Educational Project =-
 * -===============================-
 *
 *  $Id: panic.c 8 2005-07-22 09:25:42Z ilya $
 *
 *  Функция, вызываемая при фатальной ошибке
 *
 *  Идея взята из [5]. Это, конечно, далеко не самый
 *  лучший способ решать проблемы, но на первых порах
 *  и в целях отладки сойдет.
 *
 *  Функция выводит на экран сообщение об ошибке и
 *  отключает прерывания и процессор.
 *
 */

#include <helloos/types.h>
#include <helloos/scrio.h>

void panic(char *msg)
{
   puts_color("\nKernel panic!\nOops: ", 0xc);
   puts_color(msg, 0x0c);
   asm("cli; hlt");
   while(1);
}
