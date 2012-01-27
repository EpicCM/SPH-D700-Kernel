# Uncomment this for extra output and to compile with debugging symbols
#IMPORTANT:
#enable the debugging. Else The code skips the locking of pid fle. 
#SYMPTOMS:The udhcpd server wil not stop on setting the property value to 0
DEBUG=1

CROSS_COMPILE=arm-none-linux-gnueabi-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar

VER := 0.9.8

OBJS_SHARED = options.o socket.o packet.o pidfile.o
DHCPD_OBJS = dhcpd.o arpping.o files.o leases.o serverpacket.o

EXEC1 = udhcpd
OBJS1 = $(DHCPD_OBJS) $(OBJS_SHARED)

EXEC2 = dumpleases
OBJS2 = dumpleases.o

DAEMONS = udhcpd
COMMANDS = dumpleases

CFLAGS += -W -Wall -Wstrict-prototypes -DVERSION='"$(VER)"'

ifdef DEBUG
CFLAGS += -g -DDEBUG
STRIP=true
else
CFLAGS += -Os -fomit-frame-pointer
STRIP=$(CROSS_COMPILE)strip
endif

all: $(EXEC1) $(EXEC2)
	$(STRIP) --remove-section=.note --remove-section=.comment $(EXEC1) $(EXEC2)

$(OBJS1) $(OBJS2): *.h Makefile
$(EXEC1) $(EXEC2): Makefile

.c.o:
	$(CC) -c $(CFLAGS) $<
	
$(EXEC1): $(OBJS1)
	$(LD) $(LDFLAGS) -static  $(OBJS1) -o $(EXEC1)

$(EXEC2): $(OBJS2)
	$(LD) $(LDFLAGS) -static $(OBJS2) -o $(EXEC2)

clean:
	-rm -f udhcpd dumpleases *.o core