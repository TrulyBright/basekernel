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

// create a named pipe with the given name. return the named pipe.
struct named_pipe *named_pipe_create(const char* fname);
// increment the refcount of the named pipe and return it.
struct named_pipe *named_pipe_addref(struct named_pipe *np);
// decrement the refcount of the named pipe. delete it if the refcount is 0.
void named_pipe_delete(struct named_pipe *np);
// mark the named pipe as flushed. it means the pipe has no data.
void named_pipe_flush(struct named_pipe *np);
// write to the named pipe. return the number of bytes written.
int named_pipe_write(struct named_pipe *np, char *buffer, int size);
// write to the named pipe. return the number of bytes written. non-blocking version.
int named_pipe_write_nonblock(struct named_pipe *np, char *buffer, int size);
// read from the named pipe. return the number of bytes read.
int named_pipe_read(struct named_pipe *np, char *buffer, int size);
// read from the named pipe. return the number of bytes read. non-blocking version.
int named_pipe_read_nonblock(struct named_pipe *np, char *buffer, int size);
// return the number of bytes in the named pipe.
int named_pipe_size(struct named_pipe *np);
// look up the named pipe by its name. return the named pipe.
struct named_pipe *named_pipe_lookup(const char* fname);