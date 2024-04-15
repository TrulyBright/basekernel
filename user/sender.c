#include "library/syscalls.h"

int main(void) {
    const char *fname = "/.pipe";
    int fd = syscall_open_named_pipe(fname);
    char buffer[] = "Hello World\n";
    syscall_object_write(fd, buffer, strlen(buffer), 0);
}