# Makefile
# Copyright (C) 2006-2018  Victor C. Salas P. <vsalas@member.fsf.org>
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.

CC        = gcc
CCOPTIONS = -fPIC
CCFLAGS   = -O2 $(CCOPTIONS) -Wall -I.
DEFINES   = -D_TEST_MODE -D_TEST_ALL
#DEFINES   = -D_TEST_MODE
LIBDB     = -lgdbm
LIBRARIES = -ludbm $(LIBDB)
PREFIX    = .
INSTALL   = install
STRIP     = strip
UNAME     = $(shell uname)

all: lib_fixed_memory

install: uninstall install0 install1 install2

lib_fixed_memory:
	$(CC) $(CCFLAGS) -c udbm.c
	$(CC) $(CCFLAGS) -c hdb.c
	$(CC) $(CCFLAGS) -c rdb.c
ifeq ($(UNAME),Linux)
	$(CC) -shared -o libudbm.so.0.0.1 -Wl,-soname,libudbm.so.0 udbm.o hdb.o rdb.o $(LIBDB)
else
	$(CC) -shared -o libudbm.so.0.0.1 -Wl,-soname,libudbm.so.0,--out-implib,libudbm.a udbm.o hdb.o rdb.o $(LIBDB)
endif
	rm -f *.o

clean:
	rm -f *~
	rm -f *.o *.a
	rm -f lib*.so.*
	rm -f hadb hadbdel redb redbdel

install0:
ifeq ($(UNAME),Linux)
	$(INSTALL) -m 0755 -v libudbm.so.0.0.1 /usr/lib/
else
	$(INSTALL) -m 0755 -v libudbm.so.0.0.1 /usr/lib/
	$(INSTALL) -m 0644 -v libudbm.a /usr/lib/
endif
	$(INSTALL) -m 0644 -v udbm.h /usr/include/
	cd /usr/lib && ln -s libudbm.so.0.0.1 libudbm.so.0
	cd /usr/lib && ln -s libudbm.so.0.0.1 libudbm.so
	ldconfig

install1:
	$(CC) $(CCFLAGS) $(DEFINES) -o hadb hdb.c $(LIBRARIES)
	$(STRIP) hadb
	$(CC) $(CCFLAGS) $(DEFINES) -D_DEL_SUFIX -o hadbdel hdb.c $(LIBRARIES)
	$(STRIP) hadbdel
	$(CC) $(CCFLAGS) $(DEFINES) -o redb rdb.c $(LIBRARIES)
	$(STRIP) redb
	$(CC) $(CCFLAGS) $(DEFINES) -D_DEL_SUFIX -o redbdel rdb.c $(LIBRARIES)
	$(STRIP) redbdel

install2:
	$(INSTALL) -m 0755 -v hadb /usr/bin/
	$(INSTALL) -m 0755 -v hadbdel /usr/bin/
	$(INSTALL) -m 0755 -v redb /usr/bin/
	$(INSTALL) -m 0755 -v redbdel /usr/bin/

uninstall:
	rm -f /usr/bin/hadb
	rm -f /usr/bin/hadbdel
	rm -f /usr/bin/redb
	rm -f /usr/bin/redbdel
ifeq ($(UNAME),Linux)
	rm -f /usr/lib/libudbm.so
else
	rm -f /usr/lib/libudbm.a
	rm -f /usr/lib/libudbm.so
endif
	rm -f /usr/lib/libudbm.so.0
	rm -f /usr/lib/libudbm.so.0.0.1
	rm -f /usr/include/udbm.h
	ldconfig
