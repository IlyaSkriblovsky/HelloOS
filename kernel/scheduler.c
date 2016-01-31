/*
 * -= HelloOS Educational Project =-
 * -===============================-
 *
 *  $Id: scheduler.c 18 2005-12-07 07:28:43Z ilya $
 *
 *  Планировщик процессов
 *
 *  Функции, относящиеся к управлению процессами
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


// Код для PIT-таймера. См. [3]
#define TIMER_VALUE  (1193180L/CFG_SCHED_HZ)


// Массив Task содержит указатели на страницы, содержащие
// структуру, описывающую процесс.
TaskStruct *Task[CFG_SCHED_MAX_TASK];

ulong NTasks;    // Текущее количество живых процессов
ulong Current;   // Номер текущего процесса в массиве Task
ulong CurPID;     // PID, который будет присвоен очередному
                  // созданному процессу



// Инициализация планировщика
// Для создания первой задачи, head.S выделяет страницу для структуры
// TaskStruct и заполняет ее (используется то, что поле tss является
// первым, а tsss - вторым). Адрес страницы заносится в Task[0] еще
// до вызова этой функции.
void init_scheduler()
{
   printf_color(0x0b, "Starting scheduler...\t\t"FLGREEN"%dHz switching frequency\n", CFG_SCHED_HZ);

   // Устанавливаем задержку PIT-таймера
   outb(36, 0x43);
   outb(TIMER_VALUE & 0xff, 0x40);
   outb(TIMER_VALUE >> 8, 0x40);

   CurPID = 0;
   Current = 0;
   NTasks = 0;
}


// Функция убивает процесс, заданный идентификатором
// При этом освобождаются все занятые им страницы, в т.ч. страницы,
// содержащие его каталог страниц и TSS
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
         // Проходим по каталогу страниц задачи и ищем непустые записи
         // Для каждой записи проходим по соответствующей таблице страниц
         // и освобождаем выделенные страницы. По пути считаем их количество.
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

         if (NTasks <= 1) // Система осталась без процессов :(
            return panic("Heh... Last process has died...\n");
         Task[i] = Task[NTasks-1];
         NTasks--;
         if (Current >= NTasks) Current = 0;

         // Оживляем все процессы, которые ожидали завершения текущего
         uint p;
         for (p = 0; p < NTasks; p++)
            if (Task[p]->state == PS_WAITPID &&
                Task[p]->waitfor.pid == pid)
               Task[p]->state = PS_RUNNING;

         // Вызываем смену задачи, на случай, если убили текущий процесс
         CALL_SCHEDULER; // Над этим надо еще подумать
         return;
      }
   }
}

// Убить текущую задачу. Вызывается из обработчиков #GP, #PF и др.
void scheduler_kill_current()
{
   scheduler_kill(Task[Current]->pid);
}




// Печатает информацию о том, сколько страниц занимает процесс
// К пользовательским страницам относятся те, которые заняты
// кодом и данными процесса, а к системным - содержащие каталог
// и таблицы страниц, TSS
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
         sys += 2; // Еще TSS и pg_dir
         printf("System pages:\t%d\n", sys);
         printf("System pages\nin user space:\t%d\n", nf);
         printf("=================\n");
         printf("Total pages:\t%d\n", user+sys+nf);
         printf("Process size:\t%d\n", user+sys);
      }
   }
}


// А вот тут, собственно, и заключен наш планировщик. Эта функция вызывается из
// обработчика irq0, который обрабатывает таймер. Планировщик переводит Current
// на новую задачу и заменяет в tss'е обработчика прерывания номер сегмента
// прерванной задачи на новый. Может быть, это не самый красивый способ
// переключения.
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


// Выводит справку о запущенных процессах. Для каждого процесса печатается его
// идентификатор, номер сегмента TSS и адрес каталога страниц.
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
