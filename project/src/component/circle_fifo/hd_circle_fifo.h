#ifndef HD_CIRCLE_FIFO_H
#define HD_CIRCLE_FIFO_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {
    int *buffer;
    size_t head;
    size_t tail;
    size_t max;
    bool full;
} circle_fifo_t;

void circle_fifo_init(circle_fifo_t *fifo, int *buffer, size_t size);
bool circle_fifo_put(circle_fifo_t *fifo, int data);
bool circle_fifo_get(circle_fifo_t *fifo, int *data);
bool circle_fifo_empty(circle_fifo_t *fifo);
bool circle_fifo_full(circle_fifo_t *fifo);
size_t circle_fifo_size(circle_fifo_t *fifo);

#endif // HD_CIRCLE_FIFO_H
