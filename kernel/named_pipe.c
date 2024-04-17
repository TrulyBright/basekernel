#include "named_pipe.h"
#include "kmalloc.h"
#include "process.h"
#include "page.h"

static struct named_pipe *named_pipes[MAX_NAMED_PIPES];
static int named_pipe_count = 0;

struct named_pipe *named_pipe_create(const char* fname) {
    if (named_pipe_count >= MAX_NAMED_PIPES) return 0;
    struct named_pipe *np = kmalloc(sizeof(struct named_pipe));
    np->fname = fname;
    np->buffer = page_alloc(1);
    if (!np->buffer) {
        kfree(np);
        return 0;
    }
    named_pipes[named_pipe_count++] = np;
    named_pipe_count %= MAX_NAMED_PIPES;
    np->read_pos = 0;
    np->write_pos = 0;
    np->flushed = 0;
    np->queue.head = 0;
    np->queue.tail = 0;
    np->refcount = 1;
    return np;
}

struct named_pipe *named_pipe_addref(struct named_pipe *np) {
    np->refcount++;
    return np;
}

void named_pipe_flush(struct named_pipe *np) {
    if (np) {
        np->flushed = 1;
    }
}

void named_pipe_delete(struct named_pipe *np) {
    if (!np) return;
    np->refcount--;
    if (np->refcount == 0) {
        for (int i = 0; i < MAX_NAMED_PIPES; i++)
        // SUGGESTION: np has index of itself in named_pipes[]
        // so that we don't have to loop here
            if (named_pipes[i] == np) {
                named_pipes[i] = 0;
                break;
            }
        if (np->buffer) {
            page_free(np->buffer);
        }
        kfree(np);
    }
}

static int named_pipe_write_internal(struct named_pipe *np, char *buffer, int size, int blocking) {
    if (!np || !buffer) return -1;
    if (np->flushed) return 0;
    int written = 0;
    while (written < size) {
        if (np->read_pos == (np->write_pos + 1) % PIPE_SIZE) {
            if (!blocking) return written;
            if (np->flushed) {
                np->flushed = 0;
                return written;
            }
            process_wait(&np->queue);
            continue;
        }
        np->buffer[np->write_pos++] = buffer[written++];
        np->write_pos %= PIPE_SIZE;
    }
    if (blocking) process_wakeup_all(&np->queue);
    np->flushed = 0;
    return written;
}

int named_pipe_write(struct named_pipe *np, char *buffer, int size) {
    return named_pipe_write_internal(np, buffer, size, 1);
}

int named_pipe_write_nonblock(struct named_pipe *np, char *buffer, int size) {
    return named_pipe_write_internal(np, buffer, size, 0);
}

static int named_pipe_read_internal(struct named_pipe *np, char *buffer, int size, int blocking) {
    if (!np || !buffer) return -1;
    if (np->flushed) return 0;
    int read = 0;
    while (read < size) {
        np->read_pos %= PIPE_SIZE;
        if (np->read_pos == np->write_pos) {
            if (!blocking) return read;
            if (np->flushed) {
                np->flushed = 0;
                return read;
            }
            process_wait(&np->queue);
            continue;
        }
        buffer[read++] = np->buffer[np->read_pos++];
    }
    if (blocking) process_wakeup_all(&np->queue);
    np->flushed = 0;
    return read;
}

int named_pipe_read(struct named_pipe *np, char *buffer, int size) {
    return named_pipe_read_internal(np, buffer, size, 1);
}

int named_pipe_read_nonblock(struct named_pipe *np, char *buffer, int size) {
    return named_pipe_read_internal(np, buffer, size, 0);
}

int named_pipe_size(struct named_pipe *np) {
    if (!np) return -1;
    return (np->write_pos - np->read_pos + PIPE_SIZE) % PIPE_SIZE;
}

// automatically increments refcount.
struct named_pipe *named_pipe_lookup(const char* fname) {
    // TODO: replace loop with a hash table for faster lookup
    for (int i = 0; i < MAX_NAMED_PIPES; i++)
        if (named_pipes[i] && named_pipes[i]->fname == fname)
            return named_pipe_addref(named_pipes[i]);
    return 0;
}