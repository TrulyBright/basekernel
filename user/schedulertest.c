#include "kernel/types.h"
#include "library/syscalls.h"
#define NULL 0

typedef struct {
    const char *path;
    const uint32_t pri;
} ExecInfo;

int main(void) {
    const ExecInfo a[] = {
        {"/bin/process1.exe", 9},
        {"/bin/process2.exe", 7},
        {"/bin/process3.exe", 2},
        {"/bin/process4.exe", 1},
        {"/bin/process5.exe", 5},
    };
    const int size = (int) (sizeof(a) / sizeof(a[0]));
    int pid[size];
    for (int i = 0; i < size; i++) {
        const int pfd = syscall_open_file(KNO_STDDIR, a[i].path, 0, 0);
        pid[i] = syscall_process_run(pfd, 0, NULL, a[i].pri);
        syscall_object_close(pfd);
    }
    struct process_info info;
    syscall_process_wait(&info, 0);
    return 0;
}