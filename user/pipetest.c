#include "library/syscalls.h"
#include "library/string.h"

int main(void) {
    const char *pipe_name = "test_pipe";
    const char *sender_exe = "/bin/sender.exe",
               *receiver_exe = "/bin/receiver.exe";
    printf("Testing pipe. pipe name: %s\n", pipe_name);
    const int fd_sender = syscall_open_file(KNO_STDDIR, sender_exe, 0, 0),
              fd_receiver = syscall_open_file(KNO_STDDIR, receiver_exe, 0, 0);
    if (fd_sender < 0 || fd_receiver < 0) {
        printf("Failed to open: %s, code: %d\n", fd_sender < 0 ? sender_exe : receiver_exe, fd_sender < 0 ? fd_sender : fd_receiver);
        return 1;
    }
    const char *content = "I love CityU";
    if (syscall_process_fork() == 0) {
        const char *argv[] = {receiver_exe, pipe_name, content};
        syscall_process_exec(fd_receiver, 3, argv);
    } else if (syscall_process_fork() == 0) {
        const char *argv[] = {sender_exe, pipe_name, content};
        syscall_process_exec(fd_sender, 3, argv);
    } else {
        struct process_info info;
        syscall_process_wait(&info, 0);
    }
    return 0;
}