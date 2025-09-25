#ifndef ARSYNC_H
#define ARSYNC_H

#define NUM_THREADS 8

#include <assert.h>
#include <ncurses.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
  char msg[256];
} Status;

#endif
