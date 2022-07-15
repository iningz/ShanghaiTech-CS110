#include "queue.h"
#include "stdlib.h"

Queue* Queue_create(int size) {
    Queue* ret = (Queue*)malloc(sizeof(Queue));
    ret->data = (Position*)malloc(sizeof(Position) * size);
    ret->capacity = size;
    ret->front = ret->back = 0;
    return ret;
}

Position Queue_pop(Queue* q) {
    Position ret;
    if (q->front == q->back) {
        ret.x = ret.y = -1;
        return ret;
    }
    ret = q->data[q->front];
    q->front = (q->front + 1) % q->capacity;
    return ret;
}

Position Queue_front(Queue* q) {
    Position ret;
    if (q->front == q->back) {
        ret.x = ret.y = -1;
        return ret;
    }
    return q->data[q->front];
}

Position Queue_back(Queue* q) {
    Position ret;
    if (q->front == q->back) {
        ret.x = ret.y = -1;
        return ret;
    }
    return q->data[(q->back - 1 + q->capacity) % q->capacity];
}

void Queue_push(Queue* q, Position p) {
    if ((q->back + 1) % q->capacity == q->front) return;
    q->data[q->back].x = p.x;
    q->data[q->back].y = p.y;
    q->back = (q->back + 1) % q->capacity;
}

void Queue_clear(Queue* q) {
    q->front = q->back = 0;
}

void Queue_dispose(Queue* q) {
    free(q->data);
    free(q);
}