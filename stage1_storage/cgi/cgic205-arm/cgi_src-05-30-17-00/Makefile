CC=arm-buildroot-linux-gnueabihf-gcc
AFLAGS=-Wall -g
LDFLAGS= -L ./lib -lsqlite3
OBJS= capture1.cgi picture1.cgi picture2.cgi m0_fan.cgi m0_led.cgi \
m0_seg.cgi env1.cgi env2.cgi setEnv.cgi login.cgi set_sms.cgi wifi.cgi delete.cgi

run:$(OBJS)

$(OBJS):%.cgi:%.c
	$(CC) $(AFLAGS) $< cgic.c -o $@ $(LDFLAGS)
	mv *.cgi ./out/

.PHONY:clean
clean:
	rm ./out/*.cgi

