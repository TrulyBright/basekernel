#include "library/string.h"
#include "library/syscalls.h"

int main(const int argc, const char** argv) {
    int fd = syscall_make_named_pipe(argv[1]);
    if (fd < 0) {
        printf("Failed to create named pipe: %d\n", fd);
        return 1;
    }
    int failing_fd = syscall_make_named_pipe(argv[1]);
    if (failing_fd >= 0) {
        printf("Named pipe creation should have failed\n");
        return 1;
    }
    const int content_size = strlen(argv[2]) + 1;
    char buffer[content_size];
    syscall_object_read(fd, buffer, content_size, 0);
    printf("read: %s\n", buffer);
    return 0;
}