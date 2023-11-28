/*
 * -= HelloOS Educational Project =-
 * -===============================-
 *
 *  $Id$
 *
 *  Функции манипуляций с пользовательской
 *  памятью для системных вызовов
 *
 */


// Во время системного вызова все сегментные регистры кроме GS
// переключаются на сегменты ядра. Эта функция копирует строку
// из пользовательской памяти в системную через GS.
inline void strncpy_from_user(char *dest, char *src, uint n)
{
   asm volatile(
       "push %%ds\n"
       "push %%gs\n"
       "pop %%ds\n"
       "cld\n"
//         "rep movsb\n"
       "0: cmpb $0, (%%esi)\n"
       "je 2f\n"
       "movsb\n"
       "loop 0b\n"
       "jmp 1f\n"
       "2: movb $0, %%es:(%%edi)\n"
       "1: pop %%ds\n"
       :"+c"(n), "+D"(dest), "+S"(src)
       :
       :"flags", "memory"
   );
   dest[n-1] = 0;
}


inline void memcpy_from_user(void *dest, void *src, uint n)
{
    asm volatile(
        "push %%ds\n"
        "push %%gs\n"
        "pop %%ds\n"
        "cld\n"
        "rep movsb\n"
        "pop %%ds\n"
        :"+c"(n), "+D"(dest), "+S"(src)
        :
        :"flags", "memory"
    );
}



inline void memcpy_to_user(void *dest, void *src, uint n)
{
    asm volatile(
        "push %%ecx\n"
        "push %%es\n"
        "push %%gs\n"
        "pop %%es\n"
        "cld\n"
        "rep movsb\n"
        "pop %%es\n"
        "pop %%ecx\n"
        :"+c"(n), "+D"(dest), "+S"(src)
        :
        :"flags", "memory"
    );
}

