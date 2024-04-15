#include "named_pipe.h"
#include "kmalloc.h"
#include "process.h"
#include "page.h"

struct named_pipe *named_pipe_create(const char* fname) {
    struct named_pipe *np = kmalloc(sizeof(struct named_pipe));
    np->fname = fname;
    np->buffer = page_alloc(1);
    if (!np->buffer) {
        kfree(np);
        return 0;
    }
    np->read_pos = 0;
    np->write_pos = 0;
    np->flushed = 0;
    np->refcount = 1;
    return np;
}

struct named_pipe *named_pipe_addref(struct named_pipe *np) {
    np->refcount++;
    return np;
}

void named_pipe_delete(struct named_pipe *np) {
    if (!np) return;
    np->refcount--;
    if (np->refcount == 0) {
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
        np->write_pos %= PAGE_SIZE;
        if (np->read_pos == np->write_pos) {
            if (!blocking) return written;
            process_wait(&np->queue);
            continue;
        }
        np->buffer[np->write_pos++] = buffer[written++];
    }
    process_wakeup_all(&np->queue);
    return written;
}