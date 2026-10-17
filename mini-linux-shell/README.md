# Mini Linux Shell with Background Job Management

## Project Overview
This project implements a basic Linux shell in C.
It accepts user commands and launches Linux programs.

## Features
1. Execute external Linux commands using fork() and execvp().
2. Wait for foreground processes using waitpid().
3. Run background commands using &.
4. Display active background jobs using jobs.
5. Send termination signals using kill %JOB_ID.
6. Detect completed background jobs.
7. Built-in commands: cd, pwd, exit.
8. Environment variable expansion, such as $HOME.

## Technologies
- C programming language
- Ubuntu Linux / WSL
- GCC compiler
- Linux process management system calls

## Compile
gcc -Wall -Wextra -o mini-shell shell.c

## Run
./mini-shell

## Demonstration Commands
pwd
ls
echo hello
echo $HOME
sleep 30 &
jobs
kill %1
jobs
exit

## Limitations
This is an educational shell implementation.
Full job control, command quoting, pipes, and I/O
redirection are not implemented in this version.
