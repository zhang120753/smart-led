CC=arm-buildroot-linux-gnueabihf-gcc
AFLAGS=-Wall -g
LDFLAGS= -L ./lib
OBJS= capture.cgi m0_led.cgi \
	env1.cgi setEnv.cgi

run:$(OBJS)

$(OBJS):%.cgi:%.c
	$(CC) $(AFLAGS) $< cgic.c -o $@ $(LDFLAGS)
	
install:
	$(CC) $(AFLAGS) login.c -o login.cgi
	mv *.cgi ./out/
	sudo cp  ./out/*.cgi  /home/flh/linux/IMX6ULL/alientek/nfs/cgi-bin

.PHONY:clean
clean:
	rm ./out/*.cgi

