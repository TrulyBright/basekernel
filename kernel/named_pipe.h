#pragma once
#include "list.h"
#define PIPE_SIZE PAGE_SIZE
#define MAX_NAMED_PIPES 256

struct named_pipe {
    const char* fname;
    char *buffer;
    int read_pos;
    int write_pos;
    int flushed;
    int refcount;
    struct list queue;
};

struct named_pipe *named_pipe_create(const char* fname);
struct named_pipe *named_pipe_addref(struct named_pipe *np);
void named_pipe_delete(struct named_pipe *np);
void named_pipe_flush(struct named_pipe *np);

int named_pipe_write(struct named_pipe *np, char *buffer, int size);
int named_pipe_write_nonblock(struct named_pipe *np, char *buffer, int size);
int named_pipe_read(struct named_pipe *np, char *buffer, int size);
int named_pipe_read_nonblock(struct named_pipe *np, char *buffer, int size);
int named_pipe_size(struct named_pipe *np);
struct named_pipe *named_pipe_lookup(const char* fname);