/*
 * -= HelloOS Educational Project =-
 * -===============================-
 *
 *  $Id: scheduler.h 18 2005-12-07 07:28:43Z ilya $
 *
 *  ��������� ������������ ���������
 *
 */


#ifndef __SCHEDULER_H
#define __SCHEDULER_H


#include <helloos/types.h>
#include <helloos/io.h>
#include <helloos/head.h>
#include <helloos/fat.h>
#include <helloos/elf.h>
#include <helloos/binfmt.h>
#include <helloos/ipc.h>
#include <config.h>

// ��� ��������� ����� ������� TSS ������. ��� ����� ���
// uchar TSS[104], �.�. �� ������ �� �-���� ������� �����
// ��������.
typedef struct _TSSStruct TSSStruct;
struct _TSSStruct
{
   ulong tl;   // ������������ ������ ������� �����
   ulong esp0;
   ulong ss0;  // ������������ ������ ������� �����
   ulong esp1;
   ulong ss1;  // ������������ ������ ������� �����
   ulong esp2;
   ulong ss2;  // ������������ ������ ������� �����
   ulong cr3;
   ulong eip;
   ulong eflags;
   ulong eax;
   ulong ecx;
   ulong edx;
   ulong ebx;
   ulong esp;
   ulong ebp;
   ulong esi;
   ulong edi;
   ulong es;   // ������������ ������ ������� �����
   ulong cs;   // ������������ ������ ������� �����
   ulong ss;   // ������������ ������ ������� �����
   ulong ds;   // ������������ ������ ������� �����
   ulong fs;   // ������������ ������ ������� �����
   ulong gs;   // ������������ ������ ������� �����
   ulong ldt;
   ulong iomap_trace;
};


// ������� ���������� � ����������
extern uint cpu_khz;


// ��������� ��������� ��������

// �����������
#define PS_RUNNING      0
// ������� ���������� ������� ��������
#define PS_WAITPID      1
// ������� �������� ����� ����������
#define PS_WAITCOMVAR   2
//// ����
//#define PS_WAITSLEEP    3


// ��� ��������� ����� ������� �������� ������. �� ���
// �� ����� ���������� ������������ �������� ����� ������
// ���������� �� ��������.
typedef struct
{
   // TSS ������
   TSSStruct tss;
   // �������� TSS-�������� � GDT, *������� CPL � TI*. �.�. ����� ��������
   // ����� ��������, ���� ������� tsss>>3.
   uint tsss;

   // �������������
   ulong pid;

   // ��������� ��������
   ulong state;

   // ������ ��� ��������
   union
   {
      // ���� state==PS_WAITPID, �� ����� ��������
      // pid ������������� ��������
      ulong pid;
      struct
      {
         char name[MAX_VAR_NAME];
         uint value;
      } comvar;
   } waitfor;

   // ���������� �����. ����� ��� demand-loading.
   DirEntry file;

   uchar BinFormat;

   // ��������� ��� elf
   Elf32_Ehdr elf_header;
   Elf32_Phdr pheaders[5];

   // ���� ������ ����� ������� ������ ��� ��������� �������
   uchar syscall_stack[3024];
} TaskStruct;


// �������� ���������� �������. ������������ ��� ���������������
// ������ ������������
#define CALL_SCHEDULER     call_int(IRQ0_INT)


// ������������ ���������������
void init_scheduler();



// ����������, �������� ������ �������
extern TaskStruct *Task[CFG_SCHED_MAX_TASK];
extern ulong NTasks;    // ������� ���������� ����� ���������
extern ulong Current;   // ����� �������� �������� � ������� Task
extern ulong CurPID;     // PID, ������� ����� �������� ����������


// �������� �������
void scheduler_ps();
void scheduler_kill(ulong pid);
void scheduler_kill_current();
void scheduler_pages(ulong pid);



#endif // __SCHEDULER_H
