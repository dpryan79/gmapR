static char rcsid[] = "$Id: tally.c 137450 2014-05-28 23:56:20Z twu $";
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "tally.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mem.h"


#define INITIAL_READLENGTH 75


/* Matchpool and mismatchpool are buggy, resulting in a loop somewhere */
/* #define USE_MATCHPOOL 1 */
/* #define USE_MISMATCHPOOL 1 */


#ifndef USE_MATCHPOOL
Match_T
Match_new (int shift, int mapq, char quality, int xs) {
  Match_T new = (Match_T) MALLOC(sizeof(*new));

  new->shift = shift;
  new->mapq = mapq;
  new->quality = quality;
  new->xs = xs;			/* intron strand (+1 for '+', +2 for '-', 0 for others) */
  new->count = 1;

  return new;
}

void
Match_free (Match_T *old) {
  FREE(*old);
  return;
}
#endif



#ifndef USE_MISMATCHPOOL
Mismatch_T
Mismatch_new (char nt, int shift, int mapq, char quality, int xs) {
  Mismatch_T new = (Mismatch_T) MALLOC(sizeof(*new));

  new->nt = nt;
  new->shift = shift;
  new->mapq = mapq;
  new->quality = quality;
  new->xs = xs;
  new->count = 1;

  /* Assigned when mismatch added to unique list */
  /* new->count_plus = 0; */
  /* new->count_minus = 0; */
  
  new->next = NULL;
  return new;
}

void
Mismatch_free (Mismatch_T *old) {
  FREE(*old);
  return;
}
#endif


Insertion_T
Insertion_new (Genomicpos_T chrpos, char *query_insert, int mlength, int shift, int mapq, char quality) {
  Insertion_T new = (Insertion_T) MALLOC(sizeof(*new));

  new->chrpos = chrpos;
  new->segment = (char *) CALLOC(mlength+1,sizeof(char));
  strncpy(new->segment,query_insert,mlength);
  new->mlength = mlength;
  new->shift = shift;
  new->mapq = mapq;
  new->quality = quality;
  new->count = 1;

  /* Assigned when mismatch added to unique list */
  /* new->count_plus = 0; */
  /* new->count_minus = 0; */
  
  new->next = NULL;

  return new;
}

void
Insertion_free (Insertion_T *old) {
  FREE((*old)->segment);
  FREE(*old);
  return;
}

int
Insertion_count_cmp (const void *a, const void *b) {
  Insertion_T x = * (Insertion_T *) a;
  Insertion_T y = * (Insertion_T *) b;

  if (x->count > y->count) {
    return -1;
  } else if (x->count < y->count) {
    return +1;
  } else {
    return 0;
  }
}


Insertion_T
find_insertion_byshift (List_T insertions, char *segment, int mlength, int shift) {
  List_T p;
  Insertion_T ins;

  for (p = insertions; p != NULL; p = List_next(p)) {
    ins = (Insertion_T) List_head(p);
    if (ins->shift == shift && ins->mlength == mlength && !strncmp(ins->segment,segment,mlength)) {
      return ins;
    }
  }
  return (Insertion_T) NULL;
}


Insertion_T
find_insertion_seg (List_T insertions, char *segment, int mlength) {
  List_T p;
  Insertion_T ins;

  for (p = insertions; p != NULL; p = List_next(p)) {
    ins = (Insertion_T) List_head(p);
    if (ins->mlength == mlength && !strncmp(ins->segment,segment,mlength)) {
      return ins;
    }
  }
  return (Insertion_T) NULL;
}

Deletion_T
Deletion_new (Genomicpos_T chrpos, char *deletion, int mlength, int shift, int mapq) {
  Deletion_T new = (Deletion_T) MALLOC(sizeof(*new));

  new->chrpos = chrpos;
  new->segment = (char *) CALLOC(mlength+1,sizeof(char));
  strncpy(new->segment,deletion,mlength);
  new->mlength = mlength;
  new->shift = shift;
  new->mapq = mapq;
  new->count = 1;

  /* Assigned when mismatch added to unique list */
  /* new->count_plus = 0; */
  /* new->count_minus = 0; */
  
  new->next = NULL;

  return new;
}

void
Deletion_free (Deletion_T *old) {
  FREE((*old)->segment);
  FREE(*old);
  return;
}


int
Deletion_count_cmp (const void *a, const void *b) {
  Deletion_T x = * (Deletion_T *) a;
  Deletion_T y = * (Deletion_T *) b;

  if (x->count > y->count) {
    return -1;
  } else if (x->count < y->count) {
    return +1;
  } else {
    return 0;
  }
}

Deletion_T
find_deletion_byshift (List_T deletions, char *segment, int mlength, int shift) {
  List_T p;
  Deletion_T del;

  for (p = deletions; p != NULL; p = List_next(p)) {
    del = (Deletion_T) List_head(p);
    if (del->shift == shift && del->mlength == mlength /* && !strncmp(del->segment,segment,mlength)*/) {
      return del;
    }
  }
  return (Deletion_T) NULL;
}



Deletion_T
find_deletion_seg (List_T deletions, char *segment, int mlength) {
  List_T p;
  Deletion_T del;

  for (p = deletions; p != NULL; p = List_next(p)) {
    del = (Deletion_T) List_head(p);
    /* Not necessary to check segment, since it is defined by starting position and mlength */
    if (del->mlength == mlength /* && !strncmp(del->segment,segment,mlength)*/ ) {
      return del;
    }
  }
  return (Deletion_T) NULL;
}


/************************************************************************
 *   Tally_T
 ************************************************************************/

#define T Tally_T

T
Tally_new () {
  T new = (T) MALLOC(sizeof(*new));

  new->refnt = ' ';
  new->nmatches = 0;
  new->n_fromleft_plus = 0;
  new->n_fromleft_minus = 0;
  
#ifdef USE_MATCHPOOL
  new->matchpool = Matchpool_new();
#endif

  new->use_array_p = false;
  new->list_matches_byshift = (List_T) NULL;
  new->list_matches_byquality = (List_T) NULL;
  new->list_matches_bymapq = (List_T) NULL;
  new->list_matches_byxs = (List_T) NULL;

  new->n_matches_byshift_plus = INITIAL_READLENGTH+1;
  new->n_matches_byshift_minus = INITIAL_READLENGTH+1;
  new->matches_byshift_plus = (int *) CALLOC(new->n_matches_byshift_plus,sizeof(int));
  new->matches_byshift_minus = (int *) CALLOC(new->n_matches_byshift_minus,sizeof(int));

  new->n_matches_byquality = MAX_QUALITY_SCORE+1;
  new->n_matches_bymapq = MAX_MAPQ_SCORE+1;
  new->n_matches_byxs = 3+1;	/* for 0, 1, and 2 */
  new->matches_byquality = (int *) CALLOC(new->n_matches_byquality,sizeof(int));
  new->matches_bymapq = (int *) CALLOC(new->n_matches_bymapq,sizeof(int));
  new->matches_byxs = (int *) CALLOC(new->n_matches_byxs,sizeof(int));

#ifdef USE_MISMATCHPOOL
  new->mismatchpool = Mismatchpool_new();
#endif

  new->mismatches_byshift = (List_T) NULL;
  new->mismatches_byquality = (List_T) NULL;
  new->mismatches_bymapq = (List_T) NULL;
  new->mismatches_byxs = (List_T) NULL;

  new->insertions_byshift = (List_T) NULL;
  new->deletions_byshift = (List_T) NULL;

  return new;
}


void
Tally_clear (T this) {
  List_T ptr;
  Match_T match;
  Mismatch_T mismatch;
  Insertion_T ins;
  Deletion_T del;

  this->refnt = ' ';
  this->nmatches = 0;
  this->n_fromleft_plus = 0;
  this->n_fromleft_minus = 0;

  if (this->use_array_p == true) {
#if 1
    /* Note: these memset instructions are necessary to get correct results */
    memset((void *) this->matches_byshift_plus,0,this->n_matches_byshift_plus * sizeof(int));
    memset((void *) this->matches_byshift_minus,0,this->n_matches_byshift_minus * sizeof(int));
    memset((void *) this->matches_byquality,0,this->n_matches_byquality * sizeof(int));
    memset((void *) this->matches_bymapq,0,this->n_matches_bymapq * sizeof(int));
    memset((void *) this->matches_byxs,0,this->n_matches_byxs * sizeof(int));
#endif
    this->use_array_p = false;
  } else {

#ifdef USE_MATCHPOOL
    Matchpool_reset(this->matchpool);
#else
    for (ptr = this->list_matches_byshift; ptr != NULL; ptr = List_next(ptr)) {
      match = (Match_T) List_head(ptr);
      Match_free(&match);
    }
    List_free(&(this->list_matches_byshift));
    this->list_matches_byshift = (List_T) NULL;

    for (ptr = this->list_matches_byquality; ptr != NULL; ptr = List_next(ptr)) {
      match = (Match_T) List_head(ptr);
      Match_free(&match);
    }
    List_free(&(this->list_matches_byquality));
    this->list_matches_byquality = (List_T) NULL;

    for (ptr = this->list_matches_bymapq; ptr != NULL; ptr = List_next(ptr)) {
      match = (Match_T) List_head(ptr);
      Match_free(&match);
    }
    List_free(&(this->list_matches_bymapq));
    this->list_matches_bymapq = (List_T) NULL;

    for (ptr = this->list_matches_byxs; ptr != NULL; ptr = List_next(ptr)) {
      match = (Match_T) List_head(ptr);
      Match_free(&match);
    }
    List_free(&(this->list_matches_byxs));
    this->list_matches_byxs = (List_T) NULL;
#endif
  }

#ifdef USE_MISMATCHPOOL
  Mismatchpool_reset(this->mismatchpool);
#else
  for (ptr = this->mismatches_byshift; ptr != NULL; ptr = List_next(ptr)) {
    mismatch = (Mismatch_T) List_head(ptr);
    Mismatch_free(&mismatch);
  }
  List_free(&(this->mismatches_byshift));
  this->mismatches_byshift = (List_T) NULL;

  for (ptr = this->mismatches_byquality; ptr != NULL; ptr = List_next(ptr)) {
    mismatch = (Mismatch_T) List_head(ptr);
    Mismatch_free(&mismatch);
  }
  List_free(&(this->mismatches_byquality));
  this->mismatches_byquality = (List_T) NULL;

  for (ptr = this->mismatches_bymapq; ptr != NULL; ptr = List_next(ptr)) {
    mismatch = (Mismatch_T) List_head(ptr);
    Mismatch_free(&mismatch);
  }
  List_free(&(this->mismatches_bymapq));
  this->mismatches_bymapq = (List_T) NULL;

  for (ptr = this->mismatches_byxs; ptr != NULL; ptr = List_next(ptr)) {
    mismatch = (Mismatch_T) List_head(ptr);
    Mismatch_free(&mismatch);
  }
  List_free(&(this->mismatches_byxs));
  this->mismatches_byxs = (List_T) NULL;
#endif


  for (ptr = this->insertions_byshift; ptr != NULL; ptr = List_next(ptr)) {
    ins = (Insertion_T) List_head(ptr);
    Insertion_free(&ins);
  }
  List_free(&(this->insertions_byshift));
  this->insertions_byshift = (List_T) NULL;

  for (ptr = this->deletions_byshift; ptr != NULL; ptr = List_next(ptr)) {
    del = (Deletion_T) List_head(ptr);
    Deletion_free(&del);
  }
  List_free(&(this->deletions_byshift));
  this->deletions_byshift = (List_T) NULL;

  return;
}


void
Tally_transfer (T *dest, T *src) {
  T temp;

  temp = *dest;
  *dest = *src;

  temp->refnt = ' ';
  temp->nmatches = 0;
  temp->n_fromleft_plus = 0;
  temp->n_fromleft_minus = 0;

  if (temp->use_array_p == true) {
    memset((void *) temp->matches_byshift_plus,0,temp->n_matches_byshift_plus * sizeof(int));
    memset((void *) temp->matches_byshift_minus,0,temp->n_matches_byshift_minus * sizeof(int));
    memset((void *) temp->matches_byquality,0,temp->n_matches_byquality * sizeof(int));
    memset((void *) temp->matches_bymapq,0,temp->n_matches_bymapq * sizeof(int));
    memset((void *) temp->matches_byxs,0,temp->n_matches_byxs * sizeof(int));
    temp->use_array_p = false;
  }
  temp->list_matches_byshift = (List_T) NULL;
  temp->list_matches_byquality = (List_T) NULL;
  temp->list_matches_bymapq = (List_T) NULL;
  temp->list_matches_byxs = (List_T) NULL;

  temp->mismatches_byshift = (List_T) NULL;
  temp->mismatches_byquality = (List_T) NULL;
  temp->mismatches_bymapq = (List_T) NULL;
  temp->mismatches_byxs = (List_T) NULL;

  temp->insertions_byshift = (List_T) NULL;
  temp->deletions_byshift = (List_T) NULL;

  *src = temp;

  return;
}



void
Tally_free (T *old) {
  List_T ptr;
  Match_T match;
  Mismatch_T mismatch;
  Insertion_T ins;
  Deletion_T del;

#if 0
  (*old)->refnt = ' ';
  (*old)->nmatches = 0;
  (*old)->n_fromleft_plus = 0;
  (*old)->n_fromleft_minus = 0;
#endif

  FREE((*old)->matches_byshift_plus);
  FREE((*old)->matches_byshift_minus);
  FREE((*old)->matches_byquality);
  FREE((*old)->matches_bymapq);
  FREE((*old)->matches_byxs);

#ifdef USE_MATCHPOOL
  Matchpool_free(&((*old)->matchpool));
#else
  for (ptr = (*old)->list_matches_byshift; ptr != NULL; ptr = List_next(ptr)) {
    match = (Match_T) List_head(ptr);
    Match_free(&match);
  }
  List_free(&((*old)->list_matches_byshift));
  (*old)->list_matches_byshift = (List_T) NULL;

  for (ptr = (*old)->list_matches_byquality; ptr != NULL; ptr = List_next(ptr)) {
    match = (Match_T) List_head(ptr);
    Match_free(&match);
  }
  List_free(&((*old)->list_matches_byquality));
  (*old)->list_matches_byquality = (List_T) NULL;

  for (ptr = (*old)->list_matches_bymapq; ptr != NULL; ptr = List_next(ptr)) {
    match = (Match_T) List_head(ptr);
    Match_free(&match);
  }
  List_free(&((*old)->list_matches_bymapq));
  (*old)->list_matches_bymapq = (List_T) NULL;

  for (ptr = (*old)->list_matches_byxs; ptr != NULL; ptr = List_next(ptr)) {
    match = (Match_T) List_head(ptr);
    Match_free(&match);
  }
  List_free(&((*old)->list_matches_byxs));
  (*old)->list_matches_byxs = (List_T) NULL;
#endif

#ifdef USE_MISMATCHPOOL
  Mismatchpool_free(&(*old)->mismatchpool);
#else
  for (ptr = (*old)->mismatches_byshift; ptr != NULL; ptr = List_next(ptr)) {
    mismatch = (Mismatch_T) List_head(ptr);
    Mismatch_free(&mismatch);
  }
  List_free(&((*old)->mismatches_byshift));
  (*old)->mismatches_byshift = (List_T) NULL;

  for (ptr = (*old)->mismatches_byquality; ptr != NULL; ptr = List_next(ptr)) {
    mismatch = (Mismatch_T) List_head(ptr);
    Mismatch_free(&mismatch);
  }
  List_free(&((*old)->mismatches_byquality));
  (*old)->mismatches_byquality = (List_T) NULL;

  for (ptr = (*old)->mismatches_bymapq; ptr != NULL; ptr = List_next(ptr)) {
    mismatch = (Mismatch_T) List_head(ptr);
    Mismatch_free(&mismatch);
  }
  List_free(&((*old)->mismatches_bymapq));
  (*old)->mismatches_bymapq = (List_T) NULL;

  for (ptr = (*old)->mismatches_byxs; ptr != NULL; ptr = List_next(ptr)) {
    mismatch = (Mismatch_T) List_head(ptr);
    Mismatch_free(&mismatch);
  }
  List_free(&((*old)->mismatches_byxs));
  (*old)->mismatches_byxs = (List_T) NULL;
#endif


  for (ptr = (*old)->insertions_byshift; ptr != NULL; ptr = List_next(ptr)) {
    ins = (Insertion_T) List_head(ptr);
    Insertion_free(&ins);
  }
  List_free(&((*old)->insertions_byshift));
  (*old)->insertions_byshift = (List_T) NULL;

  for (ptr = (*old)->deletions_byshift; ptr != NULL; ptr = List_next(ptr)) {
    del = (Deletion_T) List_head(ptr);
    Deletion_free(&del);
  }
  List_free(&((*old)->deletions_byshift));
  (*old)->deletions_byshift = (List_T) NULL;

  FREE(*old);
  *old = (T) NULL;

  return;
}


