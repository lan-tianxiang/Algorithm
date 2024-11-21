/*
    FILE NAME: hd_circle_fifo.c
    PURPOSE: Implementation of circular FIFO using Array
    DATE: 2024/08/01
    AUTHOR: lan-tianxiang
*/

#include "hd_circle_fifo.h"

void circle_fifo_init(circle_fifo_t *fifo, int *buffer, size_t size) {
    fifo->buffer = buffer;
    fifo->max = size;
    fifo->head = 0;
    fifo->tail = 0;
    fifo->full = false;
}

bool circle_fifo_put(circle_fifo_t *fifo, int data) {
    if (fifo->full) {
        return false;
    }
    fifo->buffer[fifo->head] = data;
    fifo->head = (fifo->head + 1) % fifo->max;
    fifo->full = (fifo->head == fifo->tail);
    return true;
}

bool circle_fifo_get(circle_fifo_t *fifo, int *data) {
    if (circle_fifo_empty(fifo)) {
        return false;
    }
    *data = fifo->buffer[fifo->tail];
    fifo->tail = (fifo->tail + 1) % fifo->max;
    fifo->full = false;
    return true;
}

bool circle_fifo_empty(circle_fifo_t *fifo) {
    return (!fifo->full && (fifo->head == fifo->tail));
}

bool circle_fifo_full(circle_fifo_t *fifo) {
    return fifo->full;
}

size_t circle_fifo_size(circle_fifo_t *fifo) {
    size_t size = fifo->max;
    if (!fifo->full) {
        if (fifo->head >= fifo->tail) {
            size = fifo->head - fifo->tail;
        } else {
            size = fifo->max + fifo->head - fifo->tail;
        }
    }
    return size;
}