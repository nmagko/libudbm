/* 
 * Command line Hash & Recno database
 * Copyright (C) 2010,2018  Victor C. Salas P. <vsalas@member.fsf.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define _GNU_SOURCE

#ifndef _DBM_H
#define _DBM_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/* REPLACING DB_185.H INTERFACE */

#ifndef _DB_185_H_
#define _DB_185_H_

/* Interface for replacing Berkeley DB by GDBM */

#include <gdbm.h>

#ifndef GDBM_OPENMASK
#define GDBM_OPENMASK	7	/* Mask for the above. */
#endif

#ifndef GDBM_CLOEXEC
#define GDBM_CLOEXEC	0x100	/* Close the underlying fd on exec(3) */
#endif

/* The data and key structure. */
typedef struct {
  char *data;
  int   size;
} DBT;
/* typedef datum DBT; */

/* Routine flags. */
#define	R_CURSOR	1		/* del, put, seq */
#define	__R_UNUSED	2		/* UNUSED */
#define	R_FIRST		3		/* seq */
#define	R_IAFTER	4		/* put (RECNO) */
#define	R_IBEFORE	5		/* put (RECNO) */
#define	R_LAST		6		/* seq (BTREE, RECNO) */
#define	R_NEXT		7		/* seq */
#define	R_NOOVERWRITE	8		/* put */
#define	R_PREV		9		/* seq (BTREE, RECNO) */
#define	R_SETCURSOR	10		/* put (RECNO) */
#define	R_RECNOSYNC	11		/* sync (RECNO) */

typedef enum { DB_BTREE, DB_HASH, DB_RECNO } DBTYPE;

typedef struct __db {
  GDBM_FILE dbf;
  int (*close)	(struct __db *);
  int (*del)	(const struct __db *, const DBT *, u_int);
  int (*get)	(const struct __db *, const DBT *, DBT *, u_int);
  int (*put)	(const struct __db *, DBT *, const DBT *, u_int);
  int (*seq)	(const struct __db *, DBT *, DBT *, u_int);
  int (*sync)	(const struct __db *, u_int);
  datum *internal;
  int (*fd)	(const struct __db *);
  struct __db *dbc;
} DB;

int db185_close (DB *);

int db185_del (const DB *, const DBT *, u_int);

int db185_get (const DB *, const DBT *, DBT *, u_int);

int db185_put (const DB *, DBT *, const DBT *, u_int);

int db185_seq (const DB *, DBT *, DBT *, u_int);

int db185_sync (const DB *, u_int);

int db185_fd (const DB *);

DB *dbopen (const char *, int, int, DBTYPE, const void *);

/* The following definition is just by compatibility */
#define __db185_open dbopen

#endif /* !_DB_185_H_ */

/* HASH DEFINITIONS */

/* Create and open DB descriptor */
DB *hdbopen(const char *);

/* Free DB descriptor */
int hdbclose(DB *);

/* Flush to disk */
int hdbsync(DB *);

/* Get rows sequentially */
int hdbseq(DB *, char **, char **);

/* Get rid key value pair */
int hdbdel(DB *, const char *);

/* You have to free returned pointer if it is not used anymore */
char *hdbget(DB *, const char *);

/* Insert/Update key value pair */
int hdbput(DB *, const char *, const char *);

/* RECNO DEFINITIONS */

/* Create and open DB descriptor */
DB *rdbopen(const char *);

/* Free DB descriptor */
int rdbclose(DB *);

/* Flush to disk */
int rdbsync(DB *);

/* Get rows sequentially */
int rdbseq(DB *, char **, char **);

/* Get rid a row */
int rdbdel(DB *, const char *);

/* You have to free returned pointer if it is not used anymore */
char *rdbget(DB *, const char *);

/* Insert a new row */
int rdbput(DB *, const char *);

/* Set header line */
int rdbheader(DB *, const char *);

/* BANNER */
void banner(void);

#endif /* !_DBM_H */
