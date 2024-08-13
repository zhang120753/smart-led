
CROSS_COMPILE=arm-buildroot-linux-gnueabihf-
CC=$(CROSS_COMPILE)gcc
CFLAGS= -c -g
#LDFLAGS= -lpthread -L ./lib -lsqlite3
LDFLAGS= -lpthread

OBJS=main.o data_global.o pthread_transfer.o \
	 pthread_client_request.o pthread_led.o\
	 pthread_sqlite.o \
	 pthread_refresh.o pthread_sms.o

smart_storage :$(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	mv *o ./obj
$(OBJS):%.o:%.c
	$(CC) $(CFLAGS) $< -o $@

install:
	sudo cp smar_storage /home/book/nfs_rootfs

.PHONY:clean
clean:
	rm *.o smart_storage -rf 

