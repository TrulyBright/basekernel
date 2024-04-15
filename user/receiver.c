#include "library/string.h"
#include "library/syscalls.h"

int main(void) {
    const char *fname = "/.pipe";
    int res = syscall_make_named_pipe(fname);
    int fd = syscall_open_named_pipe(fname);

    char buffer[20];
    syscall_object_read(fd, buffer, 20, 0);
    printf("%s\n", buffer);
}