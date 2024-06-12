/* 
 * Recno table for shells
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
 * Compilation options (massivememory):
 * gcc -Wall -I. -D_TEST_MODE -D_TEST_ALL -o redb rdb.c -ldb
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
DB *rdbopen(const char *filename) {
  return hdbopen(filename);
}

/* Free DB descriptor */
int rdbclose(DB *lplz) {
  return hdbclose(lplz);
}

/* Flush to disk */
int rdbsync(DB *lplz) {
  return hdbsync(lplz);
}

/* Get rows sequentially */
int rdbseq(DB *lplz, char **s1, char **s2) {
  return hdbseq(lplz, s1, s2);
}

/* Get rid a row */
int rdbdel(DB *lplz, const char *s1) {
  int retlplz;
  unsigned long long rn, recno;
  char *s2, *v1, *v2;
  char *sRecno = "recno";
  rn = strtoll(s1, NULL, 10);
  if ( errno == ERANGE || errno == EINVAL )
    return -1;
  recno = 0;
  if ( ( s2 = rdbget(lplz, sRecno) ) != NULL) {
    recno = strtoll(s2, NULL, 10);
    free(s2);
  }
  if ( rn > recno || recno == 0 || rn == 0 )
    return -1;
  while ( rn++ < recno ) {
    asprintf(&v1, "%llu", rn);
    v2 = hdbget(lplz, v1);
    free (v1);
    asprintf(&v1, "%llu", rn - 1);
    if ( ( retlplz = hdbput(lplz, v1, v2) ) == -1 ) {
      free (v1);
      return retlplz;
    }
    free (v1);
    free (v2);
  }
  rn--;
  asprintf(&v1, "%llu", rn - 1);
  if ( ( retlplz = hdbput(lplz, sRecno, v1) ) == -1 ) {
    free (v1);
    return retlplz;
  }
  free (v1);
  asprintf(&v1, "%llu", rn);
  retlplz = hdbdel(lplz, v1);
  free (v1);
  return retlplz;
}

/* You have to free returned pointer if it is not used anymore */
char *rdbget(DB *lplz, const char *s1) {
  return hdbget(lplz, s1);
}

/* Insert row */
int rdbput(DB *lplz, const char *s2) {
  int retlplz;
  char *s1, *sKey;
  char *sRecno = "recno";
  unsigned long long rn = 0;
  if ( ( s1 = rdbget(lplz, sRecno) ) != NULL) {
    rn = strtoll(s1, NULL, 10) + 1;
    free(s1);
  }
  asprintf(&sKey, "%llu", rn);
  if ( ( retlplz = hdbput(lplz, sKey, s2) ) == -1 ) {
    free (sKey);
    return retlplz;
  }
  if ( ( retlplz = hdbput(lplz, sRecno, sKey) ) == -1 ) {
    free (sKey);
    return retlplz;
  }
  free (sKey);
  return retlplz;
}

/* Set header line */
int rdbheader(DB *lplz, const char *s2) {
  return hdbput(lplz, "fields", s2);
}

#else /* _TEST_MODE */

#include <udbm.h>

#ifndef program_invocation_short_name
#  ifdef __CYGWIN__ /* cygwin compat */
#    ifdef _DEL_SUFIX
#      define program_invocation_short_name "redbdel"
#    else
#      define program_invocation_short_name "redb"
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
    printf("usage: %s <rdbfile> <recno>\n", program_invocation_short_name);
#else
# if defined(_TEST_MODE) && defined(_TEST_ALL) /* COMPLETE TEST SECTION */
  if ( argc > 4 || argc < 2 ) {
    printf("usage: %s <rdbfile> [[i|f] <recno|value>]\n", program_invocation_short_name);
# else
  if ( argc > 4 || argc < 3 ) {
    printf("usage: %s <rdbfile> [i|f] <recno|value>\n", program_invocation_short_name);
# endif
#endif
    banner();
    exit (EXIT_FAILURE);
  }

  if ( ( lplz = rdbopen(argv[1]) ) == NULL ) {
    exit (EXIT_FAILURE);
  }

#ifdef _DEL_SUFIX
  if ( rdbdel(lplz, argv[2]) == -1 ) {
    rdbclose(lplz);
    exit (EXIT_FAILURE);
  }
#else
  if ( argc == 4 && argv[2][0] == 'i') {
    if ( rdbput(lplz, argv[3]) == -1 ) {
      rdbclose(lplz);
      exit (EXIT_FAILURE);
    }
  } else if ( argc == 4 && argv[2][0] == 'f') {
    if ( rdbheader(lplz, argv[3]) == -1 ) {
      rdbclose(lplz);
      exit (EXIT_FAILURE);
    }
  } else if ( argc == 3 ) {
    if ( ( v = rdbget(lplz, argv[2]) ) != NULL ) {
      printf("%s\n", v);
      free(v);
    }
  }
#endif

#if defined(_TEST_MODE) && defined(_TEST_ALL) /* COMPLETE TEST SECTION */
  if ( argc == 2 ) {
    printf("-*-*-*-*-*-*-*-\nL={");
    while ( rdbseq(lplz, &kk, &vv) == 0 ) {
      printf("(%s,%s),", kk, vv);
      free(kk);
      free(vv);
    }
    printf("%s}\n-*-*-*-*-*-*-*-\n", vacio);
  }
#endif /* end COMPLETE TEST SECTION */

  rdbclose(lplz);

  exit (EXIT_SUCCESS);
}

#endif /* end _TEST_MODE || LIBRARY MODE */
