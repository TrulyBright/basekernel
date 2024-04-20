#include "named_pipe.h"
#include "kmalloc.h"
#include "process.h"
#include "page.h"
#include "hash_set.h"

// Q. Why not use hash_set_create()?
// A. We know the number of buckets that is constant all the time.
//    Thus we avoid dynamic memory allocation here.
static struct hash_set_node *nodes[MAX_NAMED_PIPES] = {};
static struct hash_set named_pipes = {
    .num_entries = 0,
    .total_buckets = MAX_NAMED_PIPES,
    .head = nodes
};

struct named_pipe *named_pipe_create(const char* fname) {
    if (named_pipes.num_entries >= MAX_NAMED_PIPES) return 0;
    struct named_pipe *existing = named_pipe_lookup(fname);
    if (existing) {
        named_pipe_delete(existing); // decrease refcount
        return 0;
    }
    struct named_pipe *np = kmalloc(sizeof(struct named_pipe));
    np->fname = fname;
    np->buffer = page_alloc(1);
    if (!np->buffer) {
        kfree(np);
        return 0;
    }
    hash_set_add(&named_pipes, hash_string(fname, 0, MAX_NAMED_PIPES), np);
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
        hash_set_remove(&named_pipes, hash_string(np->fname, 0, MAX_NAMED_PIPES));
        if (np->buffer) page_free(np->buffer);
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
    struct named_pipe *np = (struct named_pipe *)hash_set_lookup(&named_pipes, hash_string(fname, 0, MAX_NAMED_PIPES));
    return np ? named_pipe_addref(np) : 0;
}