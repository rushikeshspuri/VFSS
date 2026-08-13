# VFSS — Virtual File System Simulator

A custom, user-space implementation of a Linux-style file system, written entirely in C. VFSS recreates the core bookkeeping structures the Linux kernel uses to track open files — inodes, a file table, and a per-process user area — entirely in RAM, wrapped in an interactive shell.

No kernel modules, no real disk partitions — just C, structs, and pointers standing in for what the OS does under the hood.

## Features

- **Custom interactive shell** — a `VFSS :>` prompt that parses and dispatches commands
- **System-call-style operations** — `creat`, `write`, `read`, `stat`, `unlink`, `ls`
- **Built-in manual pages** — `man <command>` prints a formatted DESCRIPTION / USAGE / EXAMPLE page
- **Layered internal data structures** — Boot Block, Super Block, an inode linked list (DILB), a File Table, and a per-process UAREA with a User File Descriptor Table (UFDT)
- **Permission model** — files are created with READ, WRITE, or READ+WRITE permission, enforced on every operation
- **Automatic backup on write** — every write is mirrored to a real file under `Backup/` on disk
- **Defensive error handling** — dedicated error codes for every failure mode, with readable messages in the shell

## Architecture

VFSS models the classic Unix file-access chain — process → file descriptor → file table entry → inode → data — using five core structures:

| Structure | Role |
|---|---|
| `BootBlock` | Short boot message printed once at startup |
| `SuperBlock` | Tracks total and free inode counts (`MAXINODE = 5`) |
| `Inode` (DILB) | One node per file slot: name, inode number, size, type, permission, reference count, data buffer. All inodes are linked into a list built once at startup by `CreateDILB()` |
| `FileTable` | Created per open file; holds independent read/write offsets, access mode, and a pointer back to the inode |
| `UAREA` / `UFDT` | The simulated process area; its User File Descriptor Table is a fixed array of `MAXOPENFILES` (5) file-table pointers — the array index is the file descriptor |

On startup, `StartAuxilaryDataInitialisation()` calls `InitialiseUAREA()`, `InitialiseSuperBlock()`, and `CreateDILB()` in sequence to bring all structures into a ready state before the shell loop begins.

## Commands

| Command | Syntax | Description |
|---|---|---|
| `help` | `help` | Lists all available commands |
| `man` | `man <command>` | Prints a full manual page for a specific command |
| `creat` | `creat <FileName> <Permission>` | Creates a new file. Permission: `1`=Read, `2`=Write, `3`=Read+Write |
| `write` | `write <fd>` | Prompts for a line of data, writes it to the file, backs it up to disk |
| `read` | `read <fd> <size>` | Reads `size` bytes from the file and prints them |
| `stat` | `stat <FileName>` | Displays inode number, size, reference count, permission, and type |
| `ls` | `ls` | Lists the names of all existing files |
| `ls -a` | `ls -a` | Lists name, inode number, and actual size for every file |
| `unlink` | `unlink <FileName>` | Deletes a file and returns its inode to the free pool |
| `clear` | `clear` | Clears the terminal screen |
| `exit` | `exit` | Terminates the shell |

## Error Codes

| Code | Meaning |
|---|---|
| `ERR_INVALID_PARAMTER` (-1) | Malformed or out-of-range argument |
| `ERR_NO_INODES` (-2) | No free inode available |
| `ERR_FILE_ALREADY_EXIST` (-3) | File name already exists |
| `ERR_FILE_NOT_EXIST` (-4) | File not found |
| `ERR_PERMISSION_DENIED` (-5) | Operation not allowed under the file's permission |
| `ERR_INSUFFICIENT_SPACE` (-6) | Write would exceed `MAXFILESIZE` (50 bytes) |
| `ERR_INSUFFICIENT_DATA` (-7) | Read requested more bytes than remain unread |
| `ERR_MAX_FILES_OPEN` (-8) | UFDT is full — no descriptor slot available |

## Build & Run

```bash
gcc VFFS.c -o vfss
./vfss
```

## Example Session

```
$ ./vfss
Booting process of VFSS is completed
 VFSS : UAREA gets initialised successfully.
 VFSS : Super Block gets initialised successfully.
 VFSS : DILB gets created successfully.

===== Virtual File System Simulator started successfully =====

 VFSS :> creat Demo.txt 3
File successfully created with FD : 3

 VFSS :> write 3
Enter the data that you want to write into the file.
Jay Ganesh
10 bytes gets successfully written into the file.

 VFSS :> read 3 10
Read operation is successfull :
Data from file is : Jay Ganesh

 VFSS :> stat Demo.txt
File name : Demo.txt
Inode number : 1
File permission : READ + WRITE
File type : Regular File

 VFSS :> ls
Demo.txt

 VFSS :> unlink Demo.txt
 VFSS :> exit
Thankyou for using  VFSS.
```

## Tech Stack

- **Language:** C (GCC, standard POSIX headers)
- **Headers:** `stdio.h`, `stdlib.h`, `unistd.h`, `fcntl.h`, `string.h`, `stdbool.h`, `sys/stat.h`
- **Persistence:** Host file-system I/O (`open`/`write`/`close`) for the `Backup/` directory
- **Platform:** Linux-first, with a Windows fallback for `clear`

## Known Limitations

- Fixed capacity: only `MAXINODE` (5) files and `MAXOPENFILES` (5) open descriptors at a time
- Max file size capped at `MAXFILESIZE` (50 bytes)
- All data is in-memory only during a session — persistence is limited to the `Backup/` mirror created on write
- `write_file()` checks the inode's permission before checking whether the UFDT entry itself is `NULL`, so an invalid fd can dereference a null pointer before the existence check runs

## Learning Outcomes

- Practical understanding of how Linux organises open-file state through inodes, file tables, and per-process descriptor tables
- Hands-on translation of POSIX system calls (`open`, `read`, `write`, `lseek`, `unlink`) into custom C functions with equivalent semantics
- Manual memory management in C — allocating and freeing inode buffers and file-table entries safely on create/unlink
- Designing a permission and error-code model that fails predictably instead of crashing
- Building an interactive command interpreter, including a self-documenting `man`/`help` system

## Author

**Rushikesh Sanjay Puri**
B.Sc. Computer Science, Vishwakarma University, Pune
GitHub: [rushikeshspuri](https://github.com/rushikeshspuri)
