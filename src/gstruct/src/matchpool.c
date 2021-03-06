static char rcsid[] = "$Id: matchpool.c 159527 2015-02-25 21:26:07Z twu $";
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "matchpool.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>		/* For memcpy */
#include "assert.h"
#include "mem.h"
#include "matchdef.h"
#include "listdef.h"


#define CHUNKSIZE 1000


#ifdef DEBUG
#define debug(x) x
#else
#define debug(x)
#endif

/* For mechanics of memory allocation and deallocation */
#ifdef DEBUG1
#define debug1(x) x
#else
#define debug1(x)
#endif

/* For popping */
#ifdef DEBUG2
#define debug2(x) x
#else
#define debug2(x)
#endif


#define T Matchpool_T
struct T {
  int nobjects;
  int objectctr;
  struct Match_T *objectptr;
  List_T objectchunks;

  int nlistcells;
  int listcellctr;
  struct List_T *listcellptr;
  List_T listcellchunks;
};

void
Matchpool_free (T *old) {
  List_T p;
  struct Match_T *objectptr;
  struct List_T *listcellptr;

  if (*old) {
    for (p = (*old)->objectchunks; p != NULL; p = List_next(p)) {
      objectptr = (struct Match_T *) List_head(p);
      FREE(objectptr);
    }
    List_free(&(*old)->objectchunks);
    for (p = (*old)->listcellchunks; p != NULL; p = List_next(p)) {
      listcellptr = (struct List_T *) List_head(p);
      FREE(listcellptr);
    }
    List_free(&(*old)->listcellchunks);
    FREE(*old);
  }
  return;
}

void
Matchpool_free_memory (T this) {
  List_T p;
  struct Match_T *objectptr;
  struct List_T *listcellptr;

  for (p = this->objectchunks; p != NULL; p = List_next(p)) {
    objectptr = (struct Match_T *) List_head(p);
    FREE(objectptr);
  }
  List_free(&this->objectchunks);
  for (p = this->listcellchunks; p != NULL; p = List_next(p)) {
    listcellptr = (struct List_T *) List_head(p);
    FREE(listcellptr);
  }
  List_free(&this->listcellchunks);

  this->nobjects = 0;
  this->objectctr = 0;
  this->objectchunks = NULL;
  /* this->objectptr = add_new_objectchunk(this); */

  this->nlistcells = 0;
  this->listcellctr = 0;
  this->listcellchunks = NULL;
  /* this->listcellptr = add_new_listcellchunk(this); */

  return;
}


void
Matchpool_report_memory (T this) {
  printf("Matchpool has %d pairchunks and %d listcellchunks\n",
	 List_length(this->objectchunks),List_length(this->listcellchunks));
  return;
}


static struct Match_T *
add_new_objectchunk (T this) {
  struct Match_T *chunk;

  chunk = (struct Match_T *) MALLOC(CHUNKSIZE*sizeof(struct Match_T));
  this->objectchunks = List_push_keep(this->objectchunks,(void *) chunk);
  debug1(printf("Adding a new chunk of objects.  Ptr for object %d is %p\n",
		this->nobjects,chunk));

  this->nobjects += CHUNKSIZE;

  return chunk;
}

static struct List_T *
add_new_listcellchunk (T this) {
  struct List_T *chunk;

  chunk = (struct List_T *) MALLOC(CHUNKSIZE*sizeof(struct List_T));
  this->listcellchunks = List_push_keep(this->listcellchunks,(void *) chunk);
  debug1(printf("Adding a new chunk of listcells.  Ptr for listcell %d is %p\n",
	       this->nlistcells,chunk));

  this->nlistcells += CHUNKSIZE;

  return chunk;
}

T
Matchpool_new (void) {
  T new = (T) MALLOC(sizeof(*new));

  new->nobjects = 0;
  new->objectctr = 0;
  new->objectchunks = NULL;
  /* new->objectptr = add_new_objectchunk(new); */

  new->nlistcells = 0;
  new->listcellctr = 0;
  new->listcellchunks = NULL;
  /* new->listcellptr = add_new_listcellchunk(new); */

  return new;
}

void
Matchpool_reset (T this) {
  this->objectctr = 0;
  this->listcellctr = 0;
  return;
}

List_T
Matchpool_push (List_T list, T this, int shift, int nm, int xs, int ncounts) {
  List_T listcell;
  Match_T new;
  List_T p;
  int n;

  if (this->objectctr >= this->nobjects) {
    this->objectptr = add_new_objectchunk(this);
  } else if ((this->objectctr % CHUNKSIZE) == 0) {
    for (n = this->nobjects - CHUNKSIZE, p = this->objectchunks;
	 n > this->objectctr; p = p->rest, n -= CHUNKSIZE) ;
    this->objectptr = (struct Match_T *) p->first;
    debug1(printf("Located object %d at %p\n",this->objectctr,this->objectptr));
  }    
  new = this->objectptr++;
  this->objectctr++;

  new->shift = shift;
  new->nm = nm;
  new->xs = xs;
  new->count = ncounts;

  debug(printf("Creating %p: %d %d\n",new,shift));


  if (this->listcellctr >= this->nlistcells) {
    this->listcellptr = add_new_listcellchunk(this);
  } else if ((this->listcellctr % CHUNKSIZE) == 0) {
    for (n = this->nlistcells - CHUNKSIZE, p = this->listcellchunks;
	 n > this->listcellctr; p = p->rest, n -= CHUNKSIZE) ;
    this->listcellptr = (struct List_T *) p->first;
    debug1(printf("Located listcell %d at %p\n",this->listcellctr,this->listcellptr));
  }
  listcell = this->listcellptr++;
  this->listcellctr++;

  listcell->first = (void *) new;
  listcell->rest = list;

  return listcell;
}



/* Note: this does not free the list cell */
List_T
Matchpool_pop (List_T list, Match_T *x) {
  List_T head;

  if (list != NULL) {
    head = list->rest;
    *x = (Match_T) list->first;
    return head;
  } else {
    return list;
  }
}


