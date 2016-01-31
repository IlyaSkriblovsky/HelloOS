#
# -= HelloOS Educational Project =-
# -===============================-
#
#  $Id: Makefile 18 2005-12-07 07:28:43Z ilya $
#
#  �������� makefile �������
#
#  �������� �������� �����:
#
#	image:	������� ����� 3.5"-�������, ���� ����������
#		����������� ������, ����, ��������� ���� � ����
#		���������������� ����������
#
#  install:	���������� �� ������� �� �� �����, �������
#  	���������� � �����, ������ �� ������ �� �����������.
#  	(��. ��������� � ������������ � ����� boot.S)
#
#  emu:		������� ����� � ��������� ��� � ��������� Bochs
#  	(!! ��������� ���������� ���� � bios � emulate.txt !!)
#
#  qemu:	������� ����� � ��������� ��� � ��������� QEmu
#
#  clean:	���������� ��������� ��� ��������� � ���������
#  	�����
#
#  depend:	������������ ����������� �������� ������
#
#

VER_MAJOR = 0
VER_MINOR = 0
VER_ALPHA = j


.EXPORT_ALL_VARIABLES:

INCLUDES = -I../include -I../config -I..
CFLAGS = -O2 -nostdlib -ffreestanding -W -Wall -save-temps $(INCLUDES) \
			-DVER_MAJOR=$(VER_MAJOR) -DVER_MINOR=$(VER_MINOR) -DVER_ALPHA=\"$(VER_ALPHA)\"
#-Werror
AOUT_CFLAGS = $(CFLAGS)


all: image

new: clean all


SUBDIRS = boot lib kernel start user

.PHONY: clean install emu qemu subdirs

subdirs: 
	set -e; for i in $(SUBDIRS); do make -C $$i; done



FILES = kernel/hello start/start user/{test,cat,ls,xo,waitvar,setvar} test.txt

image: subdirs
	dd if=/dev/zero of=image bs=512 count=2880
	chmod +x image
	dd if=boot/boot.bin of=image bs=512 count=1 conv=notrunc
	mcopy -i image $(FILES) ::


install: subdirs
	dd if=boot/boot.bin of=/dev/fd0
	mount /mnt/floppy
	cp -f $(FILES) /mnt/floppy
	umount /mnt/floppy


emu: image
	bochs -qf emulate.txt

qemu: image
	qemu -m 16 -fda image


clean:
	for i in $(SUBDIRS); do make -C $$i clean; done
	rm -f image *~
	rm -f include/*~
	rm -f include/helloos/*~

depend:
	for i in $(SUBDIRS); do make -C $$i depend; done
