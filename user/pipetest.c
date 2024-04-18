#include "library/syscalls.h"
#include "library/string.h"

int main(void) {
    const char *pipe_name = "test_pipe";
    const char *sender_exe = "/bin/sender.exe",
               *receiver_exe = "/bin/receiver.exe";
    printf("Testing pipe. pipe name: %s\n", pipe_name);
    const int fd_sender1 = syscall_open_file(KNO_STDDIR, sender_exe, 0, 0),
              fd_sender2 = syscall_open_file(KNO_STDDIR, sender_exe, 0, 0),
              fd_receiver = syscall_open_file(KNO_STDDIR, receiver_exe, 0, 0);
    if (fd_sender1 < 0 || fd_sender2 < 0 || fd_receiver < 0) {
        printf("Failed to open file: %d %d %d\n", fd_sender1, fd_sender2, fd_receiver);
        return 1;
    }
    const char *content = "I love CityU";
    const int content_length = strlen(content) + 1;
    char buffer[3];
    uint_to_string(content_length * 2, buffer);
    const char *argv_receiver[] = {receiver_exe, pipe_name, buffer};
    const char *argv_sender[] = {sender_exe, pipe_name, content};
    syscall_process_run(fd_receiver, 3, argv_receiver, 0);
    syscall_process_run(fd_sender1, 3, argv_sender, 0);
    syscall_process_run(fd_sender2, 3, argv_sender, 0);
    syscall_object_close(fd_receiver);
    syscall_object_close(fd_sender1);
    syscall_object_close(fd_sender2);
    return 0;
}