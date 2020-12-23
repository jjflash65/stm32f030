PREFIX = /usr/local
CFLAGS += -Wall -g

INSTALL = install

OBJS =	dev_table.o	\
	i2c.o		\
	init.o		\
	main.o		\
	port.o		\
	serial_common.o	\
	serial_platform.o	\
	stm32.o		\
	utils.o

LIBOBJS = parsers/parsers.a

all: stm32chflash

serial_platform.o: serial_posix.c serial_w32.c

parsers/parsers.a: force
	cd parsers && $(MAKE) parsers.a

stm32chflash: $(OBJS) $(LIBOBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBOBJS)

clean:
	rm -f $(OBJS) stm32flash
	cd parsers && $(MAKE) $@


force:

.PHONY: all clean install force
