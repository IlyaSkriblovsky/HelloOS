/*
 * -= HelloOS Educational Project =-
 * -===============================-
 *
 *  $Id: scheduler.c 18 2005-12-07 07:28:43Z ilya $
 *
 *  ����������� ���������
 *
 *  �������, ����������� � ���������� ����������
 *
 */

#include <helloos/types.h>
#include <helloos/io.h>
#include <helloos/panic.h>
#include <config.h>
#include <helloos/scheduler.h>
#include <helloos/head.h>
#include <helloos/ipc.h>

#include <helloos/scrio.h>
#include <string.h>

#include <helloos/fat.h>
#include <helloos/pager.h>


// ��� ��� PIT-�������. ��. [3]
#define TIMER_VALUE  (1193180L/CFG_SCHED_HZ)


// ������ Task �������� ��������� �� ��������, ����������
// ���������, ����������� �������.
TaskStruct *Task[CFG_SCHED_MAX_TASK];

ulong NTasks;    // ������� ���������� ����� ���������
ulong Current;   // ����� �������� �������� � ������� Task
ulong CurPID;     // PID, ������� ����� �������� ����������
                  // ���������� ��������



// ������������� ������������
// ��� �������� ������ ������, head.S �������� �������� ��� ���������
// TaskStruct � ��������� �� (������������ ��, ��� ���� tss ��������
// ������, � tsss - ������). ����� �������� ��������� � Task[0] ���
// �� ������ ���� �������.
void init_scheduler()
{
   printf_color(0x0b, "Starting scheduler...\t\t"FLGREEN"%dHz switching frequency\n", CFG_SCHED_HZ);

   // ������������� �������� PIT-�������
   outb(36, 0x43);
   outb(TIMER_VALUE & 0xff, 0x40);
   outb(TIMER_VALUE >> 8, 0x40);

   CurPID = 0;
   Current = 0;
   NTasks = 0;
}


// ������� ������� �������, �������� ���������������
// ��� ���� ������������� ��� ������� �� ��������, � �.�. ��������,
// ���������� ��� ������� ������� � TSS
void scheduler_kill(ulong pid)
{
   ulong i;
   for (i = 0; i < NTasks; i++)
   {
      if (Task[i]->pid == pid)
      {
         uint pagecount = 0;
         TaskStruct* task = Task[i];
         int j, k;
         // �������� �� �������� ������� ������ � ���� �������� ������
         // ��� ������ ������ �������� �� ��������������� ������� �������
         // � ����������� ���������� ��������. �� ���� ������� �� ����������.
         ulong *pg_dir = (ulong*)PAGE_ADDR(task->tss.cr3);
         for (j = 0; j < 512; j++)
            if ((ulong)pg_dir[j] & 0x1)
            {
               for (k = 0; k < 1024; k++)
               {
                  addr_t page = ((ulong*)PAGE_ADDR(pg_dir[j]))[k];
                  if ((page & PA_P) && ((page & PA_NONFREE) == 0))
                  {
                     pagecount++;
                     free_page(page);
                  }
               }
               pagecount++;
               free_page(pg_dir[j]);
            }
         pagecount+=2;
         free_page(task->tss.cr3);
         free_page((ulong)task);

         printf_color(0x4, "%d pages freed\n", pagecount);

         if (NTasks <= 1) // ������� �������� ��� ��������� :(
            return panic("Heh... Last process has died...\n");
         Task[i] = Task[NTasks-1];
         NTasks--;
         if (Current >= NTasks) Current = 0;

         // �������� ��� ��������, ������� ������� ���������� ��������
         uint p;
         for (p = 0; p < NTasks; p++)
            if (Task[p]->state == PS_WAITPID &&
                Task[p]->waitfor.pid == pid)
               Task[p]->state = PS_RUNNING;

         // �������� ����� ������, �� ������, ���� ����� ������� �������
         CALL_SCHEDULER; // ��� ���� ���� ��� ��������
         return;
      }
   }
}

// ����� ������� ������. ���������� �� ������������ #GP, #PF � ��.
void scheduler_kill_current()
{
   scheduler_kill(Task[Current]->pid);
}




// �������� ���������� � ���, ������� ������� �������� �������
// � ���������������� ��������� ��������� ��, ������� ������
// ����� � ������� ��������, � � ��������� - ���������� �������
// � ������� �������, TSS
void scheduler_pages(ulong pid)
{
   ulong i;
   for (i = 0; i < NTasks; i++)
   {
      if (Task[i]->pid == pid)
      {
         TaskStruct* task = Task[i];
         int j, k;
         ulong *pg_dir = (ulong*)PAGE_ADDR(task->tss.cr3);
         uint user = 0, sys= 0, nf = 0;
         for (j = 0; j < 512; j++)
            if ((ulong)pg_dir[j] & 0x1)
            {
               sys++;
               for (k = 0; k < 1024; k++)
               {
                  addr_t page = ((ulong*)PAGE_ADDR(pg_dir[j]))[k];
                  if (page & PA_P)
                  {
                     if (page & PA_NONFREE)
                        nf++;
                     else
                        user++;
                  }
               }
            }
         printf("User pages:\t%d\n", user);
         sys += 2; // ��� TSS � pg_dir
         printf("System pages:\t%d\n", sys);
         printf("System pages\nin user space:\t%d\n", nf);
         printf("=================\n");
         printf("Total pages:\t%d\n", user+sys+nf);
         printf("Process size:\t%d\n", user+sys);
      }
   }
}


// � ��� ���, ����������, � �������� ��� �����������. ��� ������� ���������� ��
// ����������� irq0, ������� ������������ ������. ����������� ��������� Current
// �� ����� ������ � �������� � tss'� ����������� ���������� ����� ��������
// ���������� ������ �� �����. ����� ����, ��� �� ����� �������� ������
// ������������.
extern TSSStruct irq0_tss;
void scheduler()
{
   if (NTasks == 0)
      return panic("No processes!");

   do
   {
      Current = (Current+1)%NTasks;
   } while (Task[Current]->state != PS_RUNNING);

   irq0_tss.tl = Task[Current]->tsss;

}


// ������� ������� � ���������� ���������. ��� ������� �������� ���������� ���
// �������������, ����� �������� TSS � ����� �������� �������.
void scheduler_ps()
{
   printf("PID\tFileName\tState\n");

   uint i;
   uchar name83[12];
   name83[11] = 0;
   for (i = 0; i < NTasks; i++)
   {
      memcpy(&name83, &Task[i]->file.Name, 11);
      int j;
      for (j = 0; j < 11; j++)
         if (name83[j] == 0)
            name83[j] = ' ';
      printf("%d\t%s\t", Task[i]->pid, name83);
      switch (Task[i]->state)
      {
         case PS_RUNNING: puts("running"); break;
         case PS_WAITPID: printf("waiting pid=%d", Task[i]->waitfor.pid); break;
         case PS_WAITCOMVAR: printf("waiting \"%s\"=%d", Task[i]->waitfor.comvar.name,
                                                     Task[i]->waitfor.comvar.value);
                             break;
         default:
                             puts("Unknown?!");
      }
      puts("\n");
   }
}
