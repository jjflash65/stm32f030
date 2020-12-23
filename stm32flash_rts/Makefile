PREFIX = /usr/local
CFLAGS += -Wall -g

INSTALL = install

OBJS       = dev_table.o
OBJS      += i2c.o
OBJS      += init.o
OBJS      += main.o
OBJS      += port.o
OBJS      += serial_common.o
OBJS      += serial_platform.o
OBJS      += stm32.o
OBJS      += utils.o

LIBOBJS = parsers/parsers.a

all: stm32flash_rts

serial_platform.o: serial_posix.c serial_w32.c

parsers/parsers.a: force
	cd parsers && $(MAKE) parsers.a

stm32flash_rts: $(OBJS) $(LIBOBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBOBJS)

clean:
	rm -f $(OBJS) stm32flash_rts
	cd parsers && $(MAKE) $@


force:

.PHONY: all clean install force
