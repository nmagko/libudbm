### MICRO DATABASE MANAGEMENT LIBRARY

Copyright (C) 2010-2018 Victor C. Salas <nmagko@gmail.com>

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Public License as published by the Free
Software Foundation; either version 3 of the license or (in your
opinion) any later version.


### WHAT IS LIBUDBM?

The Micro Database Manager Library is a set of facilities and tools to
handle Hash and Recno Databases. It includes hash and recno programs to
handle databases in shell and a library for C developers.


### DEPENDS

- make

- gcc

- gdbm [1]


### PROVIDES

- hadb: Hash Database program to insert, update, and list hashes.

- hadbdel: Hash Database program to remove hashes.

- redb: Recno Database program to insert, update, and list records.

- redbdel: Recno Database program to remove database records.

- udbm.h: Header files that have interfaces for C programming.

- libudbm.so: Library sharing objects.


### HOW TO INSTALL

Go to the libudbm source folder and run as root:

```
make
make install
```

### HOW TO UNINSTALL

Go to the libudbm source folder and run as root:

```
make uninstall
make clean
```

### PROGRAMMING INTERFACES FOR REPLACING BERKELEY DB

The following functions have been implemented from scratch inside
libudbm as a replacement for Berkeley DB to simplify programming ports
to gdbm. The functions prototype is defined under udbm.h header file.

```
  int db185_close (DB *);

  int db185_del (const DB *, const DBT *, u_int);

  int db185_get (const DB *, const DBT *, DBT *, u_int);

  int db185_put (const DB *, DBT *, const DBT *, u_int);

  int db185_seq (const DB *, DBT *, DBT *, u_int);

  int db185_sync (const DB *, u_int);

  int db185_fd (const DB *);

  DB *dbopen (const char *, int, int, DBTYPE, const void *);

  #define __db185_open dbopen
```

If your program uses Berkeley DB with any of the above functions you can
migrate to gdbm by exchanging db_185.h header file to udbm.h file.

For compatibility the udbm.h file includes the `DBT' structure (mapped
to `datum' structure) and `DB' structure.


### LIBUDBM HASH NATIVE FUNCTIONS DEFINITION

```
DB *hdbopen(const char *filename);

  It opens _filename_, creates a DB descriptor, and returns it.
```

```
int hdbclose(DB *dsc);

  This function is used to free a _dsc_ DB descriptor.
```

```
int hdbsync(DB *dsc);

  Sync memory. Flush to disk.
```

```
int hdbseq(DB *dsc, char **key, char **value);

  Get hashes sequentially. You have to free the pointer of _key_ name
  and its _value_ when they are not used anymore and if the returned
  value of the function is 0.
```

```
int hdbdel(DB *dsc, const char *key);

  Get rid of the _key_ from _dsc_ DB descriptor.
```

```
char *hdbget(DB *dsc, const char *key);

  It returns the value of the _key_ from _dsc_ DB descriptor. You have
  to free the returned pointer when it is not used anymore.
```

```
int hdbput(DB *dsc, const char *key, const char *value);

  Overwrites or sets a new _key_ entry with provided _value_.
```

### LIBUDBM RECNO NATIVE FUNCTIONS DEFINITION

```
DB *rdbopen(const char *filename);

  It opens _filename_, creates a DB descriptor, and returns it.
```

```
int rdbclose(DB *dsc);

  This function is used to free a _dsc_ DB descriptor.
```

```
int rdbsync(DB *dsc);

  Sync memory. Flush to disk.
```

```
int rdbseq(DB *dsc, char **rNum, char **lnValue);

  Get rows sequentially. You have to free row number _rNum_ and line
  content _lnValue_ pointers when they are not used anymore and if the
  returned value of the function is 0.
```

```
int rdbdel(DB *dsc, const char *rNum);

  Get rid of the row number _rNum_ from _dsc_ DB descriptor.
```

```
char *rdbget(DB *dsc, const char *rNum);

  It returns the line content of the row number _rNum_ from _dsc_ DB
  descriptor. You have to free the returned pointer when it is not used
  anymore.
```

```
int rdbput(DB *dsc, const char *lnValue);

  Insert a new row with line content _lnValue_ to _dsc_ DB descriptor.
```

```
int rdbheader(DB *dsc, const char *fieldnames);

  Set header line or field names to _dsc_ DB descriptor with
  _filednames_. You can use commas or another character that you prefer
  to separate field names but remember the post-processing of the header
  line is up to you i.e. splitting _fieldnames_.
```

### HOW TO USE HADB AND REDB PROGRAMS

> hadb <hdbfile> [key] [value]

It's a facility to insert a key-value pair in hash DB.

```
  hadb database.hdb
```

Show the content of all hashes. Be aware that hadb is going to show
everything even if hash DB is huge.

```
  hadb database.hdb five
```

Show `five' key content.

```
  hadb database.hdb four "it is an integer number"
```

Insert/Update the `four' key with "it is an integer number" as a value.

> hadbdel <hdbfile> <key>

It's a facility to get rid of key-value pairs from hash DB.

```
  hadbdel database.hdb four
```

Get rid of four keys and their value.

> redb <rdbfile> [[i|f] <recno|value>]

It's a facility to insert lines in recno DB and show its content.

```
  redb database.rdb
```

Show all recno database content. Be aware that redb is going to show
everything even if recno DB is huge.

```
  redb database.rdb 5
```

Show the content of row number 5.

```
  redb database.rdb recno
```

Show rows inserted count number (recno is a reserved word).

```
  redb database.rdb i "whatever content do you want"
```

Insert a new database row with "whatever content you want".

```
  redb database.rdb f "whatever"
```

Insert/Update header line with "whatever" field names.

```
  redb database.rdb fields
```

Show header line (fields is a reserved word).

> redbdel <rdbfile> <recno>

It's a facility to get rid of a row from recno DB. After the row is
removed the program reconstructs row numbers and can be slow if recno DB
is huge.

```
  redbdel database.rdb 2
```

Get rid of four keys and their value.


### REFERENCES

[1] gdbm at http://www.nongnu.org/


P.S.

Write me an email if you have any questions, please use American English
or Spanish, I will reply to you when I get time.

Victor C Salas (aka nmagko)


### FREE DOCUMENTATION LICENSE

Copyright (C) 2010-2018 Victor C Salas

  Permission is granted to copy, distribute, and/or modify this document
  under the terms of the GNU Free Documentation License, Version 1.3 or
  any later version published by the Free Software Foundation; with no
  Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.  A
  copy of the license is included in the section entitled "GNU Free
  Documentation License."
