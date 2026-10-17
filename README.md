# Mini Linux Shell with Background Job Management

## 1. Project Overview

The Mini Linux Shell is a simple command-line interpreter developed using the C programming language on Ubuntu Linux. It accepts user commands, executes external programs, and manages background processes.

The project demonstrates basic Linux process management using system calls such as `fork()`, `execvp()`, `waitpid()`, and `kill()`.

## 2. Objectives

- To understand the working of a basic Linux shell.
- To execute Linux commands from a custom shell.
- To create and manage background processes.
- To list active background jobs.
- To terminate background processes.
- To understand process creation and synchronization in Linux.

## 3. Technologies Used

- **Programming language:** C
- **Operating system:** Ubuntu Linux
- **Environment:** Windows Subsystem for Linux (WSL)
- **Compiler:** GCC
- **System calls:** `fork()`, `execvp()`, `waitpid()`, `kill()`

## 4. Implemented Features

1. **Command execution:** Executes external Linux commands.
2. **Background execution:** Runs commands in the background using `&`.
3. **Job listing:** Uses the `jobs` command to display active background jobs.
4. **Process termination:** Uses `kill %JOB_ID` to send a termination signal to a background job.
5. **Completion detection:** Detects when background processes finish.
6. **Built-in commands:** Supports `cd`, `pwd`, and `exit`.
7. **Environment-variable expansion:** Supports simple expansion such as `$HOME`.

## 5. Compilation

Open the Ubuntu terminal and navigate to the project directory:

```bash
cd ~/mini-linux-shell
```

Compile the source code:

```bash
gcc -Wall -Wextra -o mini-shell shell.c
```

## 6. Execution

Run the program using:

```bash
./mini-shell
```

The shell displays the following prompt:

```text
mini-shell>
```

## 7. Testing

The following commands can be used to test the implemented features:

| Command | Purpose |
|---|---|
| `pwd` | Displays the current directory |
| `echo hello` | Tests command execution |
| `echo $HOME` | Tests environment-variable expansion |
| `sleep 10 &` | Starts a background job |
| `jobs` | Lists active background jobs |
| `kill %1` | Sends a termination signal to job 1 |
| `exit` | Exits the mini shell |

The job number should be taken from the output displayed by the shell.

## 8. Important System Calls

- **`fork()`** creates a child process.
- **`execvp()`** executes an external program in the child process.
- **`waitpid()`** waits for foreground processes and checks whether background processes have completed.
- **`kill()`** sends a signal to a specified process.

## 9. Limitations

This version is an educational implementation of a basic shell. It does not currently implement full command quoting and escaping, pipes, input/output redirection, or advanced job control such as `fg`, `bg`, and Ctrl+Z.

