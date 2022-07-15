#include "doubll2d.h"
#include <stdlib.h>
#include <string.h>

/* helper function for CHECK_LIST */
bool check_list(doubll2d *list, doubll2d_elem *cursor) {
  doubll2d_elem *rowptr;
  for (rowptr = list->head; rowptr; rowptr = rowptr->down) {
    doubll2d_elem *colptr; /* iterate over rows; */
    for (colptr = rowptr; colptr; colptr = colptr->right) {
      if (colptr == cursor) /* iterate over cols */
        return true;
    }
  }
  return false; /* not found */
}

void doubll2d_init(doubll2d *list) {
  list->head = NULL;
  list->tail = NULL; /* initialize ptrs to NULL */
  list->dim_col = 0;
  list->dim_row = 0; /* initialize dims to 0 */
}

doubll2d_elem *doubll2d_get_head(doubll2d *list) {
  if (!list)     /* check if list exists */
    return NULL; /* return NULL on error */
  return list->head;
}

doubll2d_elem *doubll2d_get_tail(doubll2d *list) {
  if (!list)     /* check if list exists */
    return NULL; /* return NULL on error */
  return list->tail;
}

size_t doubll2d_dim_row(doubll2d *list) {
  if (!list)   /* check if list exists */
    return -1; /* return NULL on error */
  return list->dim_row;
}

size_t doubll2d_dim_col(doubll2d *list) {
  if (!list)   /* check if list exists */
    return -1; /* return NULL on error */
  return list->dim_col;
}

doubll2d_elem *doubll2d_insert_row(doubll2d *list, doubll2d_elem *cursor,
                                   void **data, size_t *size, size_t length) {
  doubll2d_elem *ptr; /* for old row iteration */
  size_t i;           /* for new row iteration */

  if (!list || length < list->dim_col)
    return NULL; /* return NULL on error */

  if (!list->dim_row && !list->dim_col) {
    doubll2d_elem *ptr = (doubll2d_elem *)malloc(sizeof(doubll2d_elem));
    memset(ptr, 0, sizeof(doubll2d_elem)); /* initialize */
    ptr->size = *size;
    ptr->data = malloc(*size);
    memcpy(ptr->data, *data, *size); /* copy data */
    list->head = ptr;
    list->tail = ptr;
    list->dim_row = 1;
    list->dim_col = 1;
    return ptr; /* update list info */
  }

#ifdef CHECK_LIST
  if (!check_list(list, cursor))
    return NULL; /* cursor not member */
#endif

  for (ptr = cursor; ptr->left; ptr = ptr->left)
    ; /* locate the start of old row */

  for (i = 0; ptr; ptr = ptr->right, ++i) {
    doubll2d_elem *elem = (doubll2d_elem *)malloc(sizeof(doubll2d_elem));
    elem->size = size[i]; /* copy data */
    elem->data = malloc(size[i]);
    memcpy(elem->data, data[i], size[i]);
    elem->up = ptr; /* set up and down ptrs */
    elem->down = ptr->down;
    if (ptr->down) /* check whether the last row */
      ptr->down->up = elem;
    ptr->down = elem;
    if (i > 0) { /* check whether at the leftmost */
      elem->left = ptr->left->down;
      elem->left->right = elem;
    } else
      elem->left = NULL;
    elem->right = NULL; /* set default value */
  }

  if (list->tail->down) /* check whether the last element */
    list->tail = list->tail->down;
  ++list->dim_row;

  return cursor->down; /* return the new element below cursor */
}

doubll2d_elem *doubll2d_insert_col(doubll2d *list, doubll2d_elem *cursor,
                                   void **data, size_t *size, size_t length) {
  doubll2d_elem *ptr; /* for old col iteration */
  size_t i;           /* for new col iteration */

  if (!list || length < list->dim_row)
    return NULL; /* return NULL on error */

  if (!list->dim_row && !list->dim_col) {
    doubll2d_elem *ptr = (doubll2d_elem *)malloc(sizeof(doubll2d_elem));
    memset(ptr, 0, sizeof(doubll2d_elem)); /* initialize */
    ptr->size = *size;
    ptr->data = malloc(*size);
    memcpy(ptr->data, *data, *size); /* copy data */
    list->head = ptr;
    list->tail = ptr;
    list->dim_row = 1;
    list->dim_col = 1;
    return ptr; /* update list info */
  }

#ifdef CHECK_LIST
  if (!check_list(list, cursor))
    return NULL; /* cursor not member */
#endif

  for (ptr = cursor; ptr->up; ptr = ptr->up)
    ; /* locate the start of old col */

  for (i = 0; ptr; ptr = ptr->down, ++i) {
    doubll2d_elem *elem = (doubll2d_elem *)malloc(sizeof(doubll2d_elem));
    elem->size = size[i]; /* copy data */
    elem->data = malloc(size[i]);
    memcpy(elem->data, data[i], size[i]);
    elem->left = ptr; /* set left and right ptrs */
    elem->right = ptr->right;
    if (ptr->right) /* check whether the last row */
      ptr->right->left = elem;
    ptr->right = elem;
    if (i > 0) { /* check wheter at the top */
      elem->up = ptr->up->right;
      elem->up->down = elem;
    } else
      elem->up = NULL;
    elem->down = NULL; /* set default value */
  }

  if (list->tail->right) /* check whether the last element */
    list->tail = list->tail->right;
  ++list->dim_col;

  return cursor->right; /* return the new element after cursor */
}

doubll2d_elem *doubll2d_delete_row(doubll2d *list, doubll2d_elem *cursor) {
  doubll2d_elem *start; /* save the start position */
  doubll2d_elem *res;   /* element to return */
  doubll2d_elem *nextDel;

  if (!list || !cursor)
    return NULL; /* return NULL on error */

#ifdef CHECK_LIST
  if (!check_list(list, cursor))
    return NULL; /* cursor not member */
#endif

  if (!cursor->up) {               /* first row to delete */
    list->head = list->head->down; /* move head */
    res = cursor->down;
  } else
    res = cursor->up; /* return the element above */

  if (!cursor->down) /* last row to delete */
    list->tail = list->tail->up;

  for (start = cursor; start->left; start = start->left)
    ; /* locate the start of the row */

  for (cursor = start; cursor; cursor = cursor->right) {
    if (cursor->up) /* check whether is the top */
      cursor->up->down = cursor->down;
    if (cursor->down) /* check whether is the bottom */
      cursor->down->up = cursor->up;
  }

  for (cursor = start; cursor; cursor = nextDel) {
    nextDel = cursor->right; /* save for next deletion */
    if (cursor->data)
      free(cursor->data); /* delete data */
    free(cursor);         /* delete element */
  }

  --list->dim_row;
  if (!res) /* list becomes empty */
    list->dim_col = 0;

  return res;
}

doubll2d_elem *doubll2d_delete_col(doubll2d *list, doubll2d_elem *cursor) {
  doubll2d_elem *start; /* save the start position */
  doubll2d_elem *res;   /* element to return */
  doubll2d_elem *nextDel;

  if (!list || !cursor)
    return NULL; /* return NULL on error */

#ifdef CHECK_LIST
  if (!check_list(list, cursor))
    return NULL; /* cursor not member */
#endif

  if (!cursor->left) {              /* first col to delete */
    list->head = list->head->right; /* move head */
    res = cursor->right;
  } else
    res = cursor->left; /* return the element before */

  if (!cursor->right) /* last col to delete */
    list->tail = list->tail->left;

  for (start = cursor; start->up; start = start->up)
    ; /* locate the start of the col */

  for (cursor = start; cursor; cursor = cursor->down) {
    if (cursor->left) /* check whether is the leftmost */
      cursor->left->right = cursor->right;
    if (cursor->right) /* check whether is the rightmost*/
      cursor->right->left = cursor->left;
  }

  for (cursor = start; cursor; cursor = nextDel) {
    nextDel = cursor->down; /* save for next deletion */
    if (cursor->data)
      free(cursor->data); /* delete data */
    free(cursor);         /* delete element */
  }

  --list->dim_col;
  if (!res) /* list becomes empty */
    list->dim_row = 0;

  return res; /* return element after */
}

void doubll2d_purge(doubll2d *list) {
  doubll2d_elem *cursor;
  if (!list)
    return; /* return on error */
  for (cursor = list->head; cursor; cursor = doubll2d_delete_row(list, cursor))
    ; /* delete row by row */
}

doubll2d_elem *doubll2d_find_max(doubll2d *list, list_less_func *less) {
  doubll2d_elem *max = list->head; /* save the current max */
  doubll2d_elem *rowptr;

  for (rowptr = list->head; rowptr; rowptr = rowptr->down) {
    doubll2d_elem *colptr; /* iterate over rows; */
    for (colptr = rowptr; colptr; colptr = colptr->right) {
      if (less(max, colptr)) /* iterate over cols */
        max = colptr;        /* update max */
    }
  }

  return max; /* max is NULL if list is empty */
}

doubll2d_elem *doubll2d_find_min(doubll2d *list, list_less_func *less) {
  doubll2d_elem *min = list->head; /* save the current min */
  doubll2d_elem *rowptr;

  for (rowptr = list->head; rowptr; rowptr = rowptr->down) {
    doubll2d_elem *colptr; /* iterate over rows; */
    for (colptr = rowptr; colptr; colptr = colptr->right) {
      if (less(colptr, min)) /* iterate over cols */
        min = colptr;        /* update min */
    }
  }

  return min; /* min is NULL if list is empty */
}
