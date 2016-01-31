/*
 * -= HelloOS Educational Project =-
 * -===============================-
 *
 *  $Id: fd.c 8 2005-07-22 09:25:42Z ilya $
 *
 *  ������� ������� ������-���������
 *
 *  ���� ��� � �� ��������� � ��������� �������
 *  ����� � ������ ���������, ��������� �
 *  ����������������, ��� ������� ��� �� ���� �
 *  ������ ��������� ����� ����.
 *
 *  �������� ������ � ������������ ��������� �����
 *  ����� � [6] � [7].
 *
 *
 *  ��� ������
 *    ��� ����������� ������ � ������ ������ ��� ��������,
 *    ������� �������� ������ ��� ������. � �� ������ � ���,
 *    ��� ��� ���� ������ �����, �� ���� ��� ������� ��������
 *    ������ �������� FAT.
 *
 *
 *  FIXME: �����, ���������� ������������� ���� �� ������
 *    ���������� ��� ����������. ��� ������ ��������
 *    �����������!
 */


#include <config.h>
#include <helloos/types.h>
#include <helloos/io.h>
#include <helloos/fd.h>
#include <helloos/panic.h>
#include <helloos/scrio.h>
#include <string.h>




#define FD_SECTOR_SIZE     512
#define FD_SECTOR_SIZE_CODE   0x02



// ���� ������� ��� ��������
#if (CFG_FD_READ_CACHE == 1)
struct _fd_cache_item
{
   uint LBA;
   uchar Data[FD_SECTOR_SIZE];
};
typedef struct _fd_cache_item fd_cache_item;


// ��� ���������������� � ���� ��������� �������
// ��� ��������� ������� ������ ����� ������
// ������ CFG_FD_READ_CACHE_SIZE. ������� �����������
// ��������������� ���������. ��� �������� ������ �������
// �� ���� ��������� ������� � ������� fd_cache_pos.
fd_cache_item fd_cache[CFG_FD_READ_CACHE_SIZE];
uint fd_cache_pos;

#endif



// ����� ����� ������� �����/������ � ����� �����������
// ��� ����� ��������� ���� RQM � ������� �������� � ����
// ����, ��� ���������� ����� �������.
void fd_wait_for_RQM()
{
   while ((inb_p(FD_MSR) & 0x80) == 0);
}

// ������� ���� � ������� DOR
void out_DOR(uchar value)
{
   outb_p(value, FD_DOR);
}

// ������� � FIFO
void out_FIFO(uchar value)
{
   fd_wait_for_RQM();
   outb_p(value, FD_FIFO);
}

// ������� ���� �� FIFO
uchar in_FIFO()
{
   fd_wait_for_RQM();
   return inb_p(FD_FIFO);
}



// ����� �������� ����������� ������ ��� ����� ���������
// ��������� ����������. ��� ����� ���������� ����� �����
// ������: ��������������� � ����, ���� ���������� ����������
// �� �������� ����.

// ���� ������� ����������
volatile uchar __FD_INT_FLAG = 0;

// ������� �����
void fd_clear_int_flag()
{
   __FD_INT_FLAG = 0;
}

// �������� ����������
void fd_wait_for_int()
{
   while (__FD_INT_FLAG == 0);// PrintHex(&__FD_INT_FLAG, 1);
   __FD_INT_FLAG = 0;
}


// ������������ ����������� � ��������
void fd_init()
{
   puts_color("Starting FD driver...", 0x0b);
#if (CFG_FD_READ_CACHE == 1)
   // ������������� ��������� ���-�������
   uint i;
   for (i = 0; i < CFG_FD_READ_CACHE_SIZE; i++)
      fd_cache[i].LBA = -1;
   fd_cache_pos = 0;
   printf_color(0x0a, "\t\t%dKb sector cache\n", CFG_FD_READ_CACHE_SIZE/2);
#else
   puts_color(0x0a, "\tno sector cache\n");
#endif

   fd_clear_int_flag();
   out_DOR(0x1c);    // �������� ��������� ������� �������
   fd_wait_for_RQM();

   // �������� ������� ������������
   out_FIFO(0x07);
   out_FIFO(0x00);

   //fd_wait_for_RQM();
   fd_wait_for_int();

   out_FIFO(0x08);      // �������� ������� Sense Interrupt Status
   uchar ST0;
   ST0 = in_FIFO();           // � ��������� �� ���������: ST0 � PCN
   in_FIFO();
}




void fd_write_sector_real(uint lba, uchar *buf);


// ���������� ���� ������ �� ������ � ��������� ������ �� �����
// ������ ����������� � ������� �������� ��������� (LBA?)
// 
// ����� ������ ������� �� ���� ��� ������� ��������� ���� ������
// � ����, ���� �� ���, �������, ����.
void fd_write_sector(uint lba, uchar *buf)
{
   fd_write_sector_real(lba, buf);

#if (CFG_FD_READ_CACHE == 1)
   uint i;
   for (i = 0; i < CFG_FD_READ_CACHE_SIZE; i++)
      if (fd_cache[i].LBA == lba)
      {
         memcpy(&fd_cache[i].Data, buf, FD_SECTOR_SIZE);
         return;
      }
#endif
}


// � ��� ������� �� ����� ���� ���������� ������ �� ����
// FIXME: ��������� ������� � ���
void fd_write_sector_real(uint lba, uchar *buf)
{
   const int SecPerCyl = 18;

   uchar PCN, ST0, ST1, ST2, C, H, R, N;
   int sector, head, cylinder;

   // ��������� LBA � Cylinder:Head:Sector
   // [80][2][18]
   cylinder = lba / SecPerCyl / 2;
   head = lba % (SecPerCyl * 2) / SecPerCyl;
   sector = lba % SecPerCyl + 1;

   fd_clear_int_flag();
   out_FIFO(0x0f);   // ���������� ������� �� ������ �������
   out_FIFO(0x00);
   out_FIFO((uchar)(cylinder));
   fd_wait_for_int();

   out_FIFO(0x08);      // �������� ������� Sense Interrupt Status
   in_FIFO();           // � ��������� �� ���������: ST0 � PCN
   PCN = in_FIFO();

   // ���� ����������� ������� �� ����������
   // �� �������� ������. ������ ������
   // � ����� ������ ����� ����������� ��� ���.
   if (PCN != cylinder)
      panic("FD: Seek error");



   // ������������� ���������� DMA
   // FIXME!!!!
   // ����� ������ ���������� � �������� ������ 16 ��������!
   // ����� ����� �������� ����������� ��������� ����� � ����������
   // �� ���� � ���������������� �����.
   outb_p(0x4a, 0x0b);  // 4a - ������
   outb_p(0x4a, 0x0c);  //
   outb_p((uint)(buf) & 0xff, 0x04);         //
   outb_p(((uint)(buf) >> 8 ) & 0xff, 0x04); // 24-������ �����
   outb_p(((uint)(buf) >> 16) & 0xff, 0x81); //
   outb_p(0xff, 0x05);  //
   outb_p(0x01, 0x05);  // 0x01ff - ������
   outb_p(0x02, 0x0a);


   // �������� ������� ������
   out_FIFO(0xc5);
   out_FIFO(head << 2);
   out_FIFO(cylinder);
   out_FIFO(head);
   out_FIFO(sector);
   out_FIFO(FD_SECTOR_SIZE_CODE);   // ��� ������� �������. 0x02 = 512 ����
   out_FIFO(0x12);   // EOT. ���-�� �������� �� �������
   out_FIFO(0x1b);   // GPL. ��� 1.44mb ������ 0x1b
   out_FIFO(0xff);   // DTL. ��� ������ ������ ��������� - 0xff

   ST0 = in_FIFO();
   ST1 = in_FIFO();
   ST2 = in_FIFO();
   C   = in_FIFO();
   H   = in_FIFO();
   R   = in_FIFO();
   N   = in_FIFO();

   if (ST0 & 0xd8) panic("FD: Read error (ST0)");
   if (ST1 & 0x02) panic("FD: Read-only");
   if (ST1)        panic("FD: Read error (ST1)");
   if (ST2 & 0xf7) panic("FD: Read error (ST2)");
}



void fd_read_sector_real(uint lba, uchar *buf);

// ������� ������ � �����
// ������ ����������� � ������� �������� ��������� (LBA?)
//
// ��� ������� �� ����� ���� ���� � ���� ��� ��������� ������,
// � ���� ��� ���, �� �������� fd_read_sector_real.
void fd_read_sector(uint lba, uchar *buf)
{
#if (CFG_FD_READ_CACHE == 1)

   uint i;

   // ���� ������ ������ � ���-�������
   for (i = 0; i < CFG_FD_READ_CACHE_SIZE; i++)
      if (fd_cache[i].LBA == lba)
      {
         memcpy(buf, &fd_cache[i].Data, FD_SECTOR_SIZE);
         return;
      }

   // �� �����. ��������� ��������
   fd_read_sector_real(lba, buf);

   fd_cache[fd_cache_pos].LBA = lba;
   memcpy(&fd_cache[fd_cache_pos].Data, buf, FD_SECTOR_SIZE);

   fd_cache_pos = (fd_cache_pos + 1) % CFG_FD_READ_CACHE_SIZE;

#else
   fd_read_sector_real(lba, buf);
#endif
}



// �� ����� ���� ��������� ���� ������ � �����
//
// FIXME: ��������� ������� � ���
void fd_read_sector_real(uint lba, uchar *buf)
{
   const int SecPerCyl = 18;

   uchar PCN, ST0, ST1, ST2, C, H, R, N;
   int sector, head, cylinder;

   // ��������� LBA � Cylinder:Head:Sector
   // [80][2][18]
   cylinder = lba / SecPerCyl / 2;
   head = lba % (SecPerCyl * 2) / SecPerCyl;
   sector = lba % SecPerCyl + 1;

   fd_clear_int_flag();
   out_FIFO(0x0f);   // ���������� ������� �� ������ �������
   out_FIFO(0x00);
   out_FIFO((uchar)(cylinder));
   fd_wait_for_int();

   out_FIFO(0x08);      // �������� ������� Sense Interrupt Status
   in_FIFO();           // � ��������� �� ���������: ST0 � PCN
   PCN = in_FIFO();

   // ���� ����������� ������� �� ����������
   // �� �������� ������. ������ ������
   // � ����� ������ ����� ����������� ��� ���.
   if (PCN != cylinder)
      panic("FD: Seek error");



   // ������������� ���������� DMA
   // FIXME!!!!
   // ����� ������ ���������� � �������� ������ 16 ��������!
   // ����� ����� �������� ����������� ��������� ����� � ����������
   // �� ���� � ���������������� �����.
   outb_p(0x46, 0x0b);  // 4a - ������
   outb_p(0x46, 0x0c);  //
   outb_p((uint)(buf) & 0xff, 0x04);         //
   outb_p(((uint)(buf) >> 8 ) & 0xff, 0x04); // 24-������ �����
   outb_p(((uint)(buf) >> 16) & 0xff, 0x81); //
   outb_p(0xff, 0x05);  //
   outb_p(0x01, 0x05);  // 0x01ff - ������
   outb_p(0x02, 0x0a);


   // �������� ������� ������
   out_FIFO(0xc6);
   out_FIFO(head << 2);
   out_FIFO(cylinder);
   out_FIFO(head);
   out_FIFO(sector);
   out_FIFO(FD_SECTOR_SIZE_CODE);   // ��� ������� �������. 0x02 = 512 ����
   out_FIFO(0x12);   // EOT. ���-�� �������� �� �������
   out_FIFO(0x1b);   // GPL. ��� 1.44mb ������ 0x1b
   out_FIFO(0xff);   // DTL. ��� ������ ������ ��������� - 0xff

   ST0 = in_FIFO();
   ST1 = in_FIFO();
   ST2 = in_FIFO();
   C   = in_FIFO();
   H   = in_FIFO();
   R   = in_FIFO();
   N   = in_FIFO();

   if (ST0 & 0xd8) panic("FD: Read error (ST0)");
   if (ST1 & 0x02) panic("FD: Read-only");
   if (ST1)        panic("FD: Read error (ST1)");
   if (ST2 & 0xf7) panic("FD: Read error (ST2)");
}
