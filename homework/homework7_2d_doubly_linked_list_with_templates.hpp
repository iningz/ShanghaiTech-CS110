template <typename T> /* dependent type */
typename __detail::list_row_elem_iter<T>::reference
__detail::list_row_elem_iter<T>::operator*() {
  return this->inner->data; /* return by reference */
}

template <typename T> /* dependent type */
typename __detail::list_row_elem_iter<T>::pointer
__detail::list_row_elem_iter<T>::operator->() {
  return &this->inner->data; /* return by pointer */
}

template <typename T> /* dependent type */
typename __detail::list_row_elem_iter<T> &
__detail::list_row_elem_iter<T>::operator++() { /* prefix ++ */
  this->inner = this->inner->right;             /* move right */
  return *this;                                 /* return by value */
}

template <typename T> /* dependent type */
typename __detail::list_row_elem_iter<T>
__detail::list_row_elem_iter<T>::operator++(int) { /* postfix ++ */
  auto ret = list_row_elem_iter(this->inner);      /* make a copy */
  this->inner = this->inner->right;                /* move right */
  return ret;
}

template <typename T> /* dependent type */
typename __detail::list_row_elem_iter<T> &
__detail::list_row_elem_iter<T>::operator--() { /* prefix -- */
  this->inner = this->inner->left;              /* move left */
  return *this;                                 /* return by value */
}

template <typename T> /* dependent type */
typename __detail::list_row_elem_iter<T>
__detail::list_row_elem_iter<T>::operator--(int) { /* postfix -- */
  auto ret = list_row_elem_iter(this->inner);      /* make a copy */
  this->inner = this->inner->left;                 /* move right */
  return ret;
}

template <typename T> /* dependent type */
bool __detail::list_row_elem_iter<T>::operator==(
    const list_row_elem_iter &other) {
  return this->inner == other.inner; /* equality */
}

template <typename T> /* dependent type */
bool __detail::list_row_elem_iter<T>::operator!=(
    const list_row_elem_iter &other) {
  return this->inner != other.inner; /* inequality */
}

/* ==================================================== */

template <typename T> /* dependent type */
typename __detail::list_row_iter<T>::iterator
__detail::list_row_iter<T>::begin() {
  return iterator(this->left); /* the leftmost */
}

template <typename T> /* dependent type */
typename __detail::list_row_iter<T>::iterator
__detail::list_row_iter<T>::end() {
  return this->right ? iterator(this->right->right) /* next to the rightmost */
                     : iterator(this->right);
}

template <typename T> /* dependent type */
typename __detail::list_row_iter<T>::reverse_iterator
__detail::list_row_iter<T>::rbegin() {
  return reverse_iterator(iterator(this->right)); /* the rightmost */
}

template <typename T> /* dependent type */
typename __detail::list_row_iter<T>::reverse_iterator
__detail::list_row_iter<T>::rend() {
  return this->left ? reverse_iterator(
                          iterator(this->left->left)) /* before the leftmost */
                    : reverse_iterator(iterator(this->left));
}

template <typename T> /* dependent type */
typename __detail::list_row_iter<T>::reference
__detail::list_row_iter<T>::operator*() {
  return *this; /* return by reference */
}

template <typename T> /* dependent type */
typename __detail::list_row_iter<T>::pointer
__detail::list_row_iter<T>::operator->() {
  return &*this; /* return by pointer */
}

template <typename T> /* dependent type */
typename __detail::list_row_iter<T> &
__detail::list_row_iter<T>::operator++() { /* prefix ++ */
  this->right = this->right->down;         /* move down */
  this->left = this->left->down;           /* both ends */
  return *this;                            /* return by value */
}

template <typename T> /* dependent type */
typename __detail::list_row_iter<T>
__detail::list_row_iter<T>::operator++(int) {        /* postfix ++ */
  auto ret = list_row_iter(this->left, this->right); /* make a copy */
  this->right = this->right->down;                   /* move down */
  this->left = this->left->down;                     /* both ends */
  return ret;
}

template <typename T> /* dependent type */
typename __detail::list_row_iter<T> &
__detail::list_row_iter<T>::operator--() { /* prefix -- */
  this->right = this->right->up;           /* move up*/
  this->left = this->left->up;             /* both ends */
  return *this;                            /* return by value */
}

template <typename T> /* dependent type */
typename __detail::list_row_iter<T>
__detail::list_row_iter<T>::operator--(int) {        /* postfix ++ */
  auto ret = list_row_iter(this->left, this->right); /* make a copy */
  this->right = this->right->up;                     /* move up */
  this->left = this->left->up;                       /* both ends */
  return ret;
}

template <typename T> /* dependent type */
bool __detail::list_row_iter<T>::operator==(const list_row_iter &other) {
  return this->right == other.right && this->left == other.left; /* equality */
}

template <typename T> /* dependent type */
bool __detail::list_row_iter<T>::operator!=(const list_row_iter &other) {
  return this->right != other.right ||
         this->left != other.left; /* inequality */
}

/* ==================================================== */

template <typename T> /* dependent type */
typename __detail::list_col_elem_iter<T>::reference
__detail::list_col_elem_iter<T>::operator*() {
  return this->inner->data; /* return by reference */
}

template <typename T> /* dependent type */
typename __detail::list_col_elem_iter<T>::pointer
__detail::list_col_elem_iter<T>::operator->() {
  return &this->inner->data; /* return by pointer */
}

template <typename T> /* dependent type */
typename __detail::list_col_elem_iter<T> &
__detail::list_col_elem_iter<T>::operator++() { /* prefix ++ */
  this->inner = this->inner->down;              /* move down */
  return *this;                                 /* return by value */
}

template <typename T> /* dependent type */
typename __detail::list_col_elem_iter<T>
__detail::list_col_elem_iter<T>::operator++(int) { /* postfix ++ */
  auto ret = list_col_elem_iter(this->inner);      /* make a copy */
  this->inner = this->inner->down;                 /* move down */
  return ret;
}

template <typename T> /* dependent type */
typename __detail::list_col_elem_iter<T> &
__detail::list_col_elem_iter<T>::operator--() { /* prefix -- */
  this->inner = this->inner->up;                /* move up */
  return *this;                                 /* return by value */
}

template <typename T> /* dependent type */
typename __detail::list_col_elem_iter<T>
__detail::list_col_elem_iter<T>::operator--(int) { /* postfix -- */
  auto ret = list_col_elem_iter(this->inner);      /* make a copy */
  this->inner = this->inner->up;                   /* move up */
  return ret;
}

template <typename T> /* dependent type */
bool __detail::list_col_elem_iter<T>::operator==(
    const list_col_elem_iter &other) {
  return this->inner == other.inner; /* equality */
}

template <typename T> /* dependent type */
bool __detail::list_col_elem_iter<T>::operator!=(
    const list_col_elem_iter &other) {
  return this->inner != other.inner; /* inequality */
}

/* ==================================================== */

template <typename T> /* dependent type */
typename __detail::list_col_iter<T>::iterator
__detail::list_col_iter<T>::begin() {
  return iterator(this->up); /* the top */
}

template <typename T> /* dependent type */
typename __detail::list_col_iter<T>::iterator
__detail::list_col_iter<T>::end() {
  return this->down ? iterator(this->down->down) /* next to the bottom */
                    : iterator(this->down);
}

template <typename T> /* dependent type */
typename __detail::list_col_iter<T>::reverse_iterator
__detail::list_col_iter<T>::rbegin() {
  return reverse_iterator(iterator(this->down)); /* the bottom */
}

template <typename T> /* dependent type */
typename __detail::list_col_iter<T>::reverse_iterator
__detail::list_col_iter<T>::rend() {
  return this->up ? reverse_iterator(iterator(this->up->up)) /* up the top */
                  : reverse_iterator(iterator(this->up));
}

template <typename T> /* dependent type */
typename __detail::list_col_iter<T>::reference
__detail::list_col_iter<T>::operator*() {
  return *this; /* return by reference */
}

template <typename T> /* dependent type */
typename __detail::list_col_iter<T>::pointer
__detail::list_col_iter<T>::operator->() {
  return &*this; /* return by pointer */
}

template <typename T> /* dependent type */
typename __detail::list_col_iter<T> &
__detail::list_col_iter<T>::operator++() { /* prefix ++ */
  this->up = this->up->right;              /* move right */
  this->down = this->down->right;          /* both ends */
  return *this;                            /* return by value */
}

template <typename T> /* dependent type */
typename __detail::list_col_iter<T>
__detail::list_col_iter<T>::operator++(int) {     /* postfix ++ */
  auto ret = list_col_iter(this->up, this->down); /* make a copy */
  this->up = this->up->right;                     /* move right */
  this->down = this->down->right;                 /* both ends */
  return ret;
}

template <typename T> /* dependent type */
typename __detail::list_col_iter<T> &
__detail::list_col_iter<T>::operator--() { /* prefix -- */
  this->up = this->up->left;               /* move left */
  this->down = this->down->left;           /* both ends */
  return *this;                            /* return by value */
}

template <typename T> /* dependent type */
typename __detail::list_col_iter<T>
__detail::list_col_iter<T>::operator--(int) {     /* postfix ++ */
  auto ret = list_col_iter(this->up, this->down); /* make a copy */
  this->up = this->up->left;                      /* move left */
  this->down = this->down->left;                  /* both ends */
  return ret;
}

template <typename T> /* dependent type */
bool __detail::list_col_iter<T>::operator==(const list_col_iter &other) {
  return this->up == other.up && this->down == other.down; /* equality */
}

template <typename T> /* dependent type */
bool __detail::list_col_iter<T>::operator!=(const list_col_iter &other) {
  return this->up != other.up || this->down != other.down; /* inequality */
}

/* ==================================================== */

template <typename T> typename doubll2d<T>::row_iter doubll2d<T>::row_begin() {
  return row_iter(this->up_left, this->up_right); /* top row */
}

template <typename T> typename doubll2d<T>::row_iter doubll2d<T>::row_end() {
  return this->down_left && this->down_right /* not empty */
             ? row_iter(this->down_left->down, this->down_right->down)
             : row_iter(this->down_left, this->down_right);
}

template <typename T> /* dependent type */
typename doubll2d<T>::row_riter doubll2d<T>::row_rbegin() {
  return row_riter(
      row_iter(this->down_left, this->down_right)); /* bottom row */
}

template <typename T> typename doubll2d<T>::row_riter doubll2d<T>::row_rend() {
  return this->up_left && this->up_right /* not empty */
             ? row_riter(row_iter(this->up_left->up, this->up_right->up))
             : row_riter(row_iter(this->up_left, this->up_right));
} /* up top row */

template <typename T> typename doubll2d<T>::col_iter doubll2d<T>::col_begin() {
  return col_iter(this->up_left, this->down_left); /* leftmost col */
}

template <typename T> typename doubll2d<T>::col_iter doubll2d<T>::col_end() {
  return this->up_right && this->down_right /* not empty */
             ? col_iter(this->up_right->right, this->down_right->right)
             : col_iter(this->up_right, this->down_right);
}

template <typename T> /* dependent type */
typename doubll2d<T>::col_riter doubll2d<T>::col_rbegin() {
  return col_riter(
      col_iter(this->up_right, this->down_right)); /* rightmost col */
}

template <typename T> typename doubll2d<T>::col_riter doubll2d<T>::col_rend() {
  return this->up_left && this->down_left /* not empty */
             ? col_riter(col_iter(this->up_left->left, this->down_left->left))
             : col_riter(col_iter(this->up_left, this->down_left));
} /* before leftmost row */

/* ==================================================== */

template <typename T>          /* dependent type */
template <typename input_iter> /* multiple templates */
typename doubll2d<T>::row_iter
doubll2d<T>::insert_row(row_iter cursor, input_iter begin, input_iter end) {
  if (!this->dim_row && !this->dim_col) { /* empty */
    if (begin == end)
      return row_end(); /* do nothing */

    int dim = 0;
    __detail::list_node<T> *ptr = nullptr, *prev = nullptr;
    /* iterate through input */
    for (auto const &it : __detail::make_iterable(begin, end)) {
      ptr = new __detail::list_node<T>(it);

      if (prev == nullptr) { /* first element */
        this->up_left = ptr;
      } else {
        prev->right = ptr; /* doubly link */
        ptr->left = prev;
      }

      prev = ptr;
      ++dim;
    }

    this->up_right = ptr;            /* last element */
    this->down_left = this->up_left; /* one row */
    this->down_right = this->up_right;
    this->dim_col = dim; /* update dim_col */
    this->dim_row = 1;   /* one row */
    return row_iter(this->up_left, this->up_right);
  }

  bool isFst = cursor == row_end() ? true : false;
  bool isLst = !isFst && cursor.begin().inner->down ? false : true;
  __detail::list_node<T> *ptr = nullptr, *prev = nullptr;
  auto data = begin;                       /* iterate through input */
  auto row = isFst ? row_begin() : cursor; /* choose row */

  for (auto it = row.begin(); it != row.end(); ++it) {
    ptr = data != end
              ? new __detail::list_node<T>(*(data++))
              : new __detail::list_node<T>(T()); /* if data at the end */

    if (isFst) {
      ptr->down = it.inner;
      it.inner->up = ptr; /* insert at top */
    } else {
      ptr->up = it.inner;
      ptr->down = it.inner->down;
      if (!isLst) /* if not last row */
        it.inner->down->up = ptr;
      it.inner->down = ptr; /* insert below cursor */
    }

    if (prev) {
      prev->right = ptr; /* doubly link */
      ptr->left = prev;
    }

    prev = ptr; /* advance */
  }

  if (isFst) {
    this->up_left = this->up_left->up;
    this->up_right = this->up_right->up; /* inserted at top */
  } else if (isLst) {
    this->down_left = this->down_left->down; /* inserted at bottom */
    this->down_right = this->down_right->down;
  }

  ++dim_row; /* update dim_row */
  return isFst ? row_iter(row.begin().inner->up, ptr)
               : row_iter(row.begin().inner->down, ptr);
}

template <typename T>          /* dependent type */
template <typename input_iter> /* multiple templates */
typename doubll2d<T>::col_iter
doubll2d<T>::insert_col(col_iter cursor, input_iter begin, input_iter end) {
  if (!this->dim_row && !this->dim_col) { /* empty */
    if (begin == end)
      return col_end(); /* do nothing */

    int dim = 0;
    __detail::list_node<T> *ptr = nullptr, *prev = nullptr;
    /* iterate through input */
    for (auto const &it : __detail::make_iterable(begin, end)) {
      ptr = new __detail::list_node<T>(it);

      if (prev == nullptr) { /* first element */
        this->up_left = ptr;
      } else {
        prev->down = ptr; /* doubly link */
        ptr->up = prev;
      }

      prev = ptr;
      ++dim;
    }

    this->down_left = ptr;          /* last element */
    this->up_right = this->up_left; /* one col */
    this->down_right = this->down_left;
    this->dim_row = dim; /* update dim_row */
    this->dim_col = 1;   /* one col */
    return col_iter(this->up_left, this->down_left);
  }

  bool isFst = cursor == col_end() ? true : false;
  bool isLst = !isFst && cursor.begin().inner->right ? false : true;
  __detail::list_node<T> *ptr = nullptr, *prev = nullptr;
  auto data = begin;                       /* iterate through input */
  auto col = isFst ? col_begin() : cursor; /* choose col */

  for (auto it = col.begin(); it != col.end(); ++it) {
    ptr = data != end
              ? new __detail::list_node<T>(*(data++))
              : new __detail::list_node<T>(T()); /* if data at the end */

    if (isFst) {
      ptr->right = it.inner;
      it.inner->left = ptr; /* insert at leftmost */
    } else {
      ptr->left = it.inner;
      ptr->right = it.inner->right;
      if (!isLst) /* if not last col */
        it.inner->right->left = ptr;
      it.inner->right = ptr; /* insert after cursor */
    }

    if (prev) {
      prev->down = ptr; /* doubly link */
      ptr->up = prev;
    }

    prev = ptr; /* advance */
  }

  if (isFst) {
    this->up_left = up_left->left;
    this->down_left = down_left->left; /* inserted at leftmost */
  } else if (isLst) {
    this->up_right = this->up_right->right; /* inserted at rightmost */
    this->down_right = this->down_right->right;
  }

  ++dim_col; /* update dim_col */
  return isFst ? col_iter(col.begin().inner->left, ptr)
               : col_iter(col.begin().inner->right, ptr);
}

template <typename T> /* dependent type */
typename doubll2d<T>::row_iter doubll2d<T>::delete_row(row_iter cursor) {
  if (cursor == row_end())
    return row_end(); /* do nothing */

  bool isFst = cursor.begin().inner->up ? false : true;
  bool isLst = cursor.begin().inner->down ? false : true;
  auto head = isFst ? cursor.begin().inner->down : cursor.begin().inner->up,
       tail = head;

  for (auto ptr = cursor.begin().inner, toDel = ptr; ptr; ptr = toDel) {
    toDel = ptr->right;                 /* save next */
    tail = isFst ? ptr->down : ptr->up; /* update tial */

    if (isFst && !isLst)
      ptr->down->up = NULL; /* first row to delete */
    if (isLst && !isFst)
      ptr->up->down = NULL; /* last row to delete */
    if (!isFst && !isLst) {
      ptr->up->down = ptr->down;
      ptr->down->up = ptr->up; /* relink */
    }

    delete ptr;
  }

  if (isFst) { /* first row to delete */
    this->up_left = head;
    this->up_right = tail; /* update up */
  }

  if (isLst) { /* last row to delete */
    this->down_left = head;
    this->down_right = tail; /* update down */
  }

  --dim_row;
  if (isFst && isLst)
    dim_col = 0;

  return row_iter(head, tail); /* return new iter */
}

template <typename T> /* dependent type */
typename doubll2d<T>::col_iter doubll2d<T>::delete_col(col_iter cursor) {
  if (cursor == col_end())
    return col_end(); /* do nothing */

  bool isFst = cursor.begin().inner->left ? false : true;
  bool isLst = cursor.begin().inner->right ? false : true;
  auto head = isFst ? cursor.begin().inner->right : cursor.begin().inner->left,
       tail = head;

  for (auto ptr = cursor.begin().inner, toDel = ptr; ptr; ptr = toDel) {
    toDel = ptr->down;                     /* save next */
    tail = isFst ? ptr->right : ptr->left; /* update tial */

    if (isFst && !isLst)
      ptr->right->left = NULL; /* first col to delete */
    if (isLst && !isFst)
      ptr->left->right = NULL; /* last col to delete */
    if (!isFst && !isLst) {
      ptr->left->right = ptr->right;
      ptr->right->left = ptr->left; /* relink */
    }

    delete ptr;
  }

  if (isFst) { /* first col to delete */
    this->up_left = head;
    this->down_left = tail; /* update left */
  }

  if (isLst) { /* last col to delete */
    this->up_right = head;
    this->down_right = tail; /* update right */
  }

  --dim_col;
  if (isFst && isLst)
    dim_row = 0;

  return col_iter(head, tail); /* return new iter */
}

template <typename T> void doubll2d<T>::clear() {
  for (auto cursor = row_iter(this->down_left, this->down_right);
       cursor != row_end(); cursor = delete_row(cursor)) /* delete row by row */
    ;
}

template <typename T> /* dependent type */
template <class R>    /* multiple template */
R doubll2d<T>::reduce(std::function<R(R, const T &)> fn, R init) {
  for (auto &&row : row_iterable()) { /* iterate over rows */
    for (auto &&elem : row)           /* iterate through rows */
      init = fn(init, elem);          /* accumulate */
  }

  return init;
}