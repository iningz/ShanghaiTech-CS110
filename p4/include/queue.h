#ifndef QUEUE_H
#define QUEUE_H

typedef struct {
    unsigned char x, y;
} Position;

typedef struct {
    Position* data;
    int capacity;
    int front;
    int back;
} Queue;

Queue* Queue_create(int size);

Position Queue_pop(Queue* q);

Position Queue_front(Queue* q);

Position Queue_back(Queue* q);

void Queue_push(Queue* q, Position p);

void Queue_clear(Queue* q);

void Queue_dispose(Queue* q);

#endif