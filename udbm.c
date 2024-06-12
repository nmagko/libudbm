/* 
 * Command line Hash & Recno database
 * Copyright (C) 2018  Victor C. Salas P. <vsalas@member.fsf.org>
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

#include "udbm.h"
#include <string.h>

int db185_close (DB *gdbmp) {
  gdbm_close(gdbmp->dbf);
  free(gdbmp);
  return(EXIT_SUCCESS);
}

int db185_del (const DB *gdbmp, const DBT *keyp, u_int unused_compat) {
  datum *keyp_t = (datum *)keyp;
  return gdbm_delete(gdbmp->dbf, *keyp_t);
}

int db185_get (const DB *gdbmp, const DBT *keyp, DBT *valuep, u_int unused_compat) {
  datum *keyp_t = (datum *)keyp;
  datum *valuep_t = (datum *)valuep;
  *valuep_t = gdbm_fetch(gdbmp->dbf, *keyp_t);
  if ( valuep == NULL ) {
    return -1;
  }
  return(EXIT_SUCCESS);
}

int db185_put (const DB *gdbmp, DBT *keyp, const DBT *valuep, u_int unused_compat) {
  datum *keyp_t = (datum *)keyp;
  datum *valuep_t = (datum *)valuep;
  return gdbm_store(gdbmp->dbf, *keyp_t, *valuep_t, GDBM_REPLACE);
}

void __db185_freelastkey (DB *dbc) {
  if (dbc->internal->dptr != NULL) {
    free(dbc->internal->dptr);
  }
  free(dbc->internal);
  //dbc->internal = calloc(1, sizeof(datum));
  dbc->internal = NULL;
}

datum *__db185_setlastkey_1 (DB *dbc, datum *keyp) {
  dbc->internal = calloc(1, sizeof(datum));
  dbc->internal->dsize = keyp->dsize;
  dbc->internal->dptr = malloc(keyp->dsize);
  memcpy(dbc->internal->dptr, keyp->dptr, keyp->dsize);
  return(keyp);
}

datum *__db185_setlastkey_2 (DB *dbc, datum *keyp) {
  if (dbc->internal->dptr != NULL) {
    free(dbc->internal->dptr);
  }
  free(dbc->internal);
  dbc->internal = calloc(1, sizeof(datum));
  dbc->internal->dsize = keyp->dsize;
  dbc->internal->dptr = malloc(keyp->dsize);
  memcpy(dbc->internal->dptr, keyp->dptr, keyp->dsize);
  return(keyp);
}

int db185_seq (const DB *gdbmp, DBT *keyp, DBT *valuep, u_int unused_compat) {
  datum *keyp_t = (datum *)keyp;
  datum *valuep_t = (datum *)valuep;
  datum keys_t;
  datum values_t;
  if ( gdbmp->internal == NULL || gdbmp->internal->dptr == NULL ) {
    keys_t = gdbm_firstkey(gdbmp->dbf);
    if ( keys_t.dptr != NULL ) {
      __db185_setlastkey_1(gdbmp->dbc, &keys_t);
    }
  } else {
    keys_t = gdbm_nextkey(gdbmp->dbf, *(gdbmp->internal));
    if ( keys_t.dptr == NULL ) {
      __db185_freelastkey(gdbmp->dbc);
    } else {
      __db185_setlastkey_2(gdbmp->dbc, &keys_t);
    }
  }
  if ( gdbmp->internal == NULL || gdbmp->internal->dptr == NULL ) {
    return(EXIT_FAILURE);
  }
  keyp_t->dsize = keys_t.dsize;
  keyp_t->dptr = malloc(keys_t.dsize);
  memcpy(keyp_t->dptr, keys_t.dptr, keys_t.dsize);
  free(keys_t.dptr);
  values_t = gdbm_fetch(gdbmp->dbf, *keyp_t);
  valuep_t->dsize = values_t.dsize;
  valuep_t->dptr = malloc(values_t.dsize);
  memcpy(valuep_t->dptr, values_t.dptr, values_t.dsize);
  free(values_t.dptr);
  return(EXIT_SUCCESS);
}

int db185_sync (const DB *gdbmp, u_int unused_compat) {
  gdbm_sync(gdbmp->dbf);
  return(EXIT_SUCCESS);
}

int db185_fd (const DB *gdbmp) {
  return gdbm_fdesc(gdbmp->dbf);
}

DB *dbopen (const char *file, int oflags, int mode, DBTYPE type, const void *openinfo) {
  /* type is always DB_HASH */
  DB *gdbmp;
  if ( ( gdbmp = calloc(1, sizeof(*gdbmp)) ) == NULL ) {
    perror("calloc()");
    return NULL;
  }
  if ( ( gdbmp->dbf = gdbm_open(file, 0, (GDBM_OPENMASK|GDBM_CLOEXEC)&oflags, mode, NULL) ) == NULL ) {
    perror("gdbm_open()");
    return NULL;
  }
  gdbmp->close = db185_close;
  gdbmp->del = db185_del;
  gdbmp->get = db185_get;
  gdbmp->put = db185_put;
  gdbmp->seq = db185_seq;
  gdbmp->sync = db185_sync;
  gdbmp->internal = NULL; //calloc(1, sizeof(datum));
  gdbmp->fd = db185_fd;
  gdbmp->dbc = gdbmp; /* itself */
  return gdbmp;
}

void banner(void) {
  printf("\n\
 Quick hash & recno databases for shells (header)\n\
 Copyright (C) 2010,2018  Victor C. Salas P. <vsalas@member.fsf.org>\n\
 \n\
 This program is free software: you can redistribute it and/or modify\n\
 it under the terms of the GNU General Public License as published by\n\
 the Free Software Foundation, either version 3 of the License, or\n\
 (at your option) any later version.\n\
 \n\
 This program is distributed in the hope that it will be useful,\n\
 but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
 GNU General Public License for more details.\n\
 \n\
 You should have received a copy of the GNU General Public License\n\
 along with this program.  If not, see <http://www.gnu.org/licenses/>.\n");
}
