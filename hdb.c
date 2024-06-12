/* 
 * Hash database for shells
 * Copyright (C) 2010-2018  Victor C. Salas P. <vsalas@member.fsf.org>
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

/*
 * Compilation options (brainmemory):
 * gcc -Wall -I. -D_TEST_MODE -D_TEST_ALL -o hadb hdb.c -ldb
 */

#define _GNU_SOURCE

#ifndef _TEST_MODE /* LIBRARY MODE */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <limits.h>
#include "udbm.h"

#define R_DEFAULT 0

/* Create and open DB descriptor */
DB *hdbopen(const char *filename) {
  DB *lplz;
  if ( ( lplz = dbopen(filename, O_CREAT|O_RDWR, 0660, DB_HASH, NULL) ) == NULL ) {
    perror("dbopen()");
  }
  return lplz;
}

/* Free DB descriptor */
int hdbclose(DB *lplz) {
  return lplz->close(lplz);
}

/* Flush to disk */
int hdbsync(DB *lplz) {
  return lplz->sync(lplz, R_DEFAULT);
}

/* Get rows sequentially */
int hdbseq(DB *lplz, char **s1, char **s2) {
  int irseq;
  DBT *lplzKey, *lplzValue;
  lplzKey = malloc( sizeof(DBT) + 1 );
  lplzValue = malloc( sizeof(DBT) + 1 );
  irseq = lplz->seq(lplz, lplzKey, lplzValue, R_NEXT);
  if ( irseq == 0 ) {
    *s1 = (char *) malloc (lplzKey->size + 1);
    memset(*s1, 0, lplzKey->size + 1);
    strncpy(*s1, lplzKey->data, lplzKey->size);
    free (lplzKey);
    *s2 = (char *) malloc (lplzValue->size + 1);
    memset(*s2, 0, lplzValue->size + 1);
    strncpy(*s2, lplzValue->data, lplzValue->size);
    free (lplzValue);
  }
  return irseq;
}

/* Get rid a key value pair */
int hdbdel(DB *lplz, const char *s1) {
  int retlplz;
  DBT *lplzKey;
  char *sKey;
  sKey = (char *) malloc( strlen(s1) + 1 );
  strcpy(sKey, s1);
  lplzKey = malloc( sizeof(DBT) + 1 );
  lplzKey->data = sKey;
  lplzKey->size = strlen(sKey);
  if ( ( retlplz = lplz->del(lplz, lplzKey, R_DEFAULT) ) == -1 )
    perror("lplz->del()");
  free (lplzKey);
  free (sKey);
  return retlplz;
}

/* You have to free returned pointer if it is not used anymore */
char *hdbget(DB *lplz, const char *s1) {
  int retlplz;
  DBT *lplzKey, *lplzValue;
  char *sKey, *sValue;
  sKey = (char *) malloc( strlen(s1) + 1 );
  strcpy(sKey, s1);
  lplzKey = malloc( sizeof(DBT) + 1 );
  lplzKey->data = sKey;
  lplzKey->size = strlen(sKey);
  lplzValue = malloc( sizeof(DBT) + 1 );
  if ( ( retlplz = lplz->get(lplz, lplzKey, lplzValue, R_DEFAULT) ) == -1 ) {
    perror("lplz->get()");
    free (lplzKey);
    free (lplzValue);
    free (sKey);
    return NULL;
  }
  sValue = (char *) malloc( lplzValue->size + 1);
  strncpy(sValue, lplzValue->data, lplzValue->size);
  sValue[lplzValue->size] = '\x0';
  /*
   * lplzValue->data is a pointer to physical hdb you shouldn't free it
   * (e.g. free(lplzValue->data)), it will be free with hdbclose
   */
  free (lplzKey);
  free (lplzValue);
  free (sKey);
  return sValue;
}

/* Insert/Update key value pair */
int hdbput(DB *lplz, const char *s1, const char *s2) {
  int retlplz;
  DBT *lplzKey, *lplzValue;
  char *sKey, *sValue;
  sKey = (char *) malloc( strlen(s1) + 1 );
  sValue = (char *) malloc( strlen(s2) + 1 );
  strcpy(sKey, s1);
  strcpy(sValue, s2);
  lplzKey = malloc( sizeof(DBT) + 1 );
  lplzKey->data = sKey;
  lplzKey->size = strlen(sKey);
  lplzValue = malloc( sizeof(DBT) + 1 );
  lplzValue->data = sValue;
  lplzValue->size = strlen(sValue);
  if ( ( retlplz = lplz->put(lplz, lplzKey, lplzValue, R_DEFAULT) ) == -1 )
    perror("lplz->put()");
  free (lplzKey);
  free (lplzValue);
  free (sKey);
  free (sValue);
  return retlplz;
}

#else /* _TEST_MODE */

#include <udbm.h>

#ifndef program_invocation_short_name
#  ifdef __CYGWIN__ /* cygwin compat */
#    ifdef _DEL_SUFIX
#      define program_invocation_short_name "hadbdel"
#    else
#      define program_invocation_short_name "hadb"
#    endif
#  endif
#endif

int main (int argc, char *argv[]) {
#ifndef _DEL_SUFIX
  char *v;
#endif
  DB *lplz;

#if defined(_TEST_MODE) && defined(_TEST_ALL) /* COMPLETE TEST SECTION */
  char *kk, *vv;
  char *vacio = NULL;
#endif /* end COMPLETE TEST SECTION */

#ifdef _DEL_SUFIX
  if ( argc != 3 ) {
    printf("usage: %s <hdbfile> <key>\n", program_invocation_short_name);
#else
# if defined(_TEST_MODE) && defined(_TEST_ALL) /* COMPLETE TEST SECTION */
  if ( argc > 4 || argc < 2 ) {
    printf("usage: %s <hdbfile> [key] [value]\n", program_invocation_short_name);
# else
  if ( argc > 4 || argc < 3 ) {
    printf("usage: %s <hdbfile> <key> [value]\n", program_invocation_short_name);
# endif
#endif
    banner();
    exit (EXIT_FAILURE);
  }

  if ( ( lplz = hdbopen(argv[1]) ) == NULL ) {
    exit (EXIT_FAILURE);
  }

#ifdef _DEL_SUFIX
  if ( hdbdel(lplz, argv[2]) == -1 ) {
    hdbclose(lplz);
    exit (EXIT_FAILURE);
  }
#else
  if ( argc == 4 ) {
    if ( hdbput(lplz, argv[2], argv[3]) == -1 ) {
      hdbclose(lplz);
      exit (EXIT_FAILURE);
    }
  } else if ( argc == 3 ) {
    if ( ( v = hdbget(lplz, argv[2]) ) != NULL ) {
      printf("%s\n", v);
      free(v);
    }
  }
#endif

#if defined(_TEST_MODE) && defined(_TEST_ALL) /* COMPLETE TEST SECTION */
  if ( argc == 2 ) {
    printf("-*-*-*-*-*-*-*-\nS={");
    while ( hdbseq(lplz, &kk, &vv) == 0 ) {
      printf("(%s,%s),", kk, vv);
      free(kk);
      free(vv);
    }
    printf("%s}\n-*-*-*-*-*-*-*-\n", vacio);
  }
#endif /* end COMPLETE TEST SECTION */

  hdbclose(lplz);

  exit (EXIT_SUCCESS);
}

#endif /* end _TEST_MODE || LIBRARY MODE */
