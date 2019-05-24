UNAME := $(shell uname)
CFLAGS += -I../sml/include/ -g -std=c99 -Wall -Wextra -pedantic
OBJS = sml_server.o
OBJSSPLUNK = splunk_logger.o
LIBSML = ../sml/lib/libsml.a
INSTALL = /usr/bin/install
prefix = /usr/local
exec_prefix = ${prefix}
bindir = ${exec_prefix}/bin
libdir = ${exec_prefix}/lib
includedir = ${prefix}/include

LIBS = -lm -ljson-c -lcurl
ifneq ($(UNAME), Darwin)
LIBS += -luuid
endif

all: sml_server

splunk_logger: $(OBJSSPLUNK) $(LIBSML)
	$(CC) $(OBJSSPLUNK) $(LIBSML) $(LIBS) -o splunk_logger

sml_server: $(OBJS) $(LIBSML)
	$(CC) $(OBJS) $(LIBSML) $(LIBS) -o sml_server

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

.PHONY: clean install uninstall
clean:
	@rm -f *.o
	@rm -f sml_server

install: sml_server
	$(INSTALL) -d $(DESTDIR)$(bindir)
	$(INSTALL) sml_server $(DESTDIR)$(bindir)

uninstall:
	@rm -f $(DESTDIR)$(bindir)/sml_server
