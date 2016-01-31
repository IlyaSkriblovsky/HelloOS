/*
 * -= HelloOS Educational Project =-
 * -===============================-
 *
 *  $Id: syscall.c 18 2005-12-07 07:28:43Z ilya $
 *
 *  ��������� ������
 *
 *  ���� ������ �������� ������
 *
 */


#include <helloos/types.h>
#include <helloos/scrio.h>
#include <helloos/scheduler.h>
#include <helloos/binfmt.h>
#include <helloos/io.h>
#include <helloos/syscall.h>
#include <helloos/ipc.h>
#include <helloos/fat.h>


// ��������� �� syscall
typedef uint (*syscall_ptr)();

// �������� ��������, ��� ��� ������ ������� ������ � ����������
// ����� � �� �����, ���������� ������� �� ���������� � ����������

uint syscall_exit(uint exitcode);

uint syscall_getpid();

char syscall_getchar(uint x, uint y);
uint syscall_setchar(uint x, uint y, char c);
uint syscall_setattr(uint x, uint y, byte a);

char syscall_getch();

uint syscall_nputs_color(char *s, uint n, uchar attr);

uint syscall_clear_screen();
uint syscall_readline(char *cmd, uint buf_size);

uint syscall_ps();
uint syscall_kill(uint pid);
uint syscall_pages_info(uint pid);

uint syscall_bin_info(char *filename);
uint syscall_bin_load(char *filename, char *arg);

uint syscall_dbg();

uint syscall_screen_info(uint *w, uint *h);

uint syscall_waitpid(uint pid);

// ������� ��������� �������
syscall_ptr syscall_table[] = {
   (syscall_ptr)syscall_exit,
   (syscall_ptr)syscall_getpid,

   (syscall_ptr)syscall_getchar,
   (syscall_ptr)syscall_setchar,
   (syscall_ptr)syscall_setattr,
   (syscall_ptr)syscall_getch,
   (syscall_ptr)syscall_nputs_color,
   (syscall_ptr)syscall_clear_screen,
   (syscall_ptr)syscall_readline,

   (syscall_ptr)syscall_ps,
   (syscall_ptr)syscall_kill,
   (syscall_ptr)syscall_bin_info,
   (syscall_ptr)syscall_bin_load,
   (syscall_ptr)syscall_pages_info,
   (syscall_ptr)syscall_dbg,

   (syscall_ptr)syscall_screen_info,

   (syscall_ptr)syscall_comvar_init,
   (syscall_ptr)syscall_comvar_get,
   (syscall_ptr)syscall_comvar_set,
   (syscall_ptr)syscall_comvar_add,
   (syscall_ptr)syscall_waitcomvar,
   (syscall_ptr)syscall_ipc_info,

   (syscall_ptr)syscall_waitpid,

   (syscall_ptr)syscall_find_file,
   (syscall_ptr)syscall_file_load,
   (syscall_ptr)syscall_dir_load,
};
// ����������� ���������� �������. ������� � ���� ����������,
// ����� ���� �������� ��������� �� ����������.
uint syscall_nr = (sizeof(syscall_table) / sizeof(syscall_ptr));


// ��������� ����� syscall_exit
// ��� �������� ���������������� ������� ����� �����������
uint syscall_exit(uint exitcode)
{
   printf_color(0x04, "Process %d exits with code %d\n", Task[Current]->pid, exitcode);
   scheduler_kill_current();

   return 0;
}


// ��������� ����� getpid
// ���������� pid �������� ��������
uint syscall_getpid()
{
   return Task[Current]->pid;
}



// ��������� ����� syscall_getnewcharaddr
// �������� �������� ���� ������ � ����������� � ����������
// ��� �����
uint syscall_getnewcharaddr()
{
   static uint addr = 0xf06; // �������� ������� ������
   uint res = addr;
   addr += 2;  // ��������� �� ���������
   return res;
}

// ��������� ����� syscall_incvideochar
// ����������� �� 1 �������� ����� � �����������
// ���������� 0
uint syscall_incvideochar(uint addr)
{
   (*(uchar*)(0xb8000+addr))++;
   return 0;
}

// ��������� ����� syscall_nputs_color
// ������������� ��������� ������� nputs_color
// ��������� �� ��, ��� � ������. ������ �� ������� 255 ��������.
// ���������� ���������� ���������� ��������.
//
// FIXME: ����� ��������� ������� ������� � ����������������
// ������. ������������� GP, PF (� ������ ������ �� ���������� ������.
uint syscall_nputs_color(char *s, uint n, uchar attr)
{
   uchar localbuf[256];
   memcpy_from_user(localbuf, s, MIN(n, 256));
   nputs_color(localbuf, MIN(n, 256), attr);
   return MIN(n, 256);
}


// ��������� ����� syscall_getch
// ������� ������� ������� � ���������� �� ascii-���
char syscall_getch()
{
   uchar cp, c = inb(0x60);
   char ascii;
   do
   {
      cp = c;
      c = inb(0x60);
   } while ((c == '\0') || (c == cp) || ((ascii = scan2ascii(c)) == '\0'));
   return scan2ascii(c);
}



uint syscall_clear_screen()
{
   clear_screen();
   return 0;
}

uint syscall_readline(char *cmd, uint buf_size)
{
   char localbuf[256];
   readline(localbuf, 256);
   localbuf[buf_size] = 0;
   memcpy_to_user(cmd, localbuf, buf_size);
   return 0;
}

uint syscall_ps()
{
   scheduler_ps();
   return 0;
}

uint syscall_kill(uint pid)
{
   scheduler_kill(pid);
   return 0;
}

uint syscall_bin_info(char *filename)
{
   char localbuf[256];
   strncpy_from_user(localbuf, filename, 256);
   if (!bin_dump_info(localbuf))
      printf("Cannot read binary\n");
   return 0;
}

uint syscall_bin_load(char *filename, char *arg)
{
   char localbuf[256];
   char locarg[256];
   strncpy_from_user(localbuf, filename, 256);
   strncpy_from_user(locarg,   arg,      256);
   uint pid = bin_load_bin(localbuf, locarg);
   if (pid == (uint)-1)
      printf_color(0x04, "Cannot load binary\n");
   return pid;
}

uint syscall_pages_info(uint pid)
{
   scheduler_pages(pid);
   return 0;
}

uint syscall_dbg()
{
   outw(0x8A00, 0x8A00);
   outw(0x8AE0, 0x8A00);
   return 0;
}

uint syscall_waitpid(uint pid)
{
   uint p;
   for (p = 0; p < NTasks; p++)
      if (Task[p]->pid == pid)
         goto ok;
   return 0;

ok:
   Task[Current]->waitfor.pid = pid;
   Task[Current]->state = PS_WAITPID;
   CALL_SCHEDULER;
   return 1;
}
