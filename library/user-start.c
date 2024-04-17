/*
Copyright (C) 2016-2019 The University of Notre Dame
This software is distributed under the GNU General Public License.
See the file LICENSE for details.
*/

/*
This module is the runtime start of every user-level program.
The very first symbol in this module must be _start() because
the kernel simply jumps to the very first location of the executable.
_start() sets up any necessary runtime environment and invokes
the main function.  Note that this function cannot exit, but
must invoke the syscall_process_exit() system call to terminate the process.
*/

#include "library/syscalls.h"
#include "library/string.h"

int main(int argc, const char *argv[]);

void _start(int argc, const char **argv)
{
	printf("HELLO! I'm a process with PID %d and PRI %d!\n", syscall_process_self(), syscall_process_pri());
	const int exit_code = main(argc, argv);
	printf("BYE! I was a process with PID %d and PRI %d! My exit code is %d!\n", syscall_process_self(), syscall_process_pri(), exit_code);
	syscall_process_exit(exit_code);
}
