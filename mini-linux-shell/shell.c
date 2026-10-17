#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <ctype.h>

#define MAX_LINE 1024
#define MAX_ARGS 100
#define MAX_JOBS 100

typedef struct {
    int id;
    pid_t pid;
    char command[MAX_LINE];
    int active;
} Job;

Job jobs[MAX_JOBS];
int next_job_id = 1;

void check_jobs(void) {
    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs[i].active) {
            int status;
            pid_t result = waitpid(jobs[i].pid, &status, WNOHANG);
            if (result == jobs[i].pid) {
                printf("\n[%d] Finished: %s\n",
                       jobs[i].id, jobs[i].command);
                jobs[i].active = 0;
            }
        }
    }
}

int add_job(pid_t pid, const char *command) {
    for (int i = 0; i < MAX_JOBS; i++) {
        if (!jobs[i].active) {
            jobs[i].id = next_job_id++;
            jobs[i].pid = pid;
            jobs[i].active = 1;
            snprintf(jobs[i].command, MAX_LINE, "%s", command);
            return jobs[i].id;
        }
    }
    return -1;
}

void show_jobs(void) {
    check_jobs();
    int found = 0;
    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs[i].active) {
            printf("[%d] Running (PID %d): %s\n",
                   jobs[i].id, (int)jobs[i].pid, jobs[i].command);
            found = 1;
        }
    }
    if (!found) {
        printf("No background jobs.\n");
    }
}

void stop_job(const char *target) {
    pid_t pid = -1;

    if (target[0] == '%') {
        int id = atoi(target + 1);
        for (int i = 0; i < MAX_JOBS; i++) {
            if (jobs[i].active && jobs[i].id == id) {
                pid = jobs[i].pid;
                break;
            }
        }
    } else {
        char *end;
        long value = strtol(target, &end, 10);
        if (*target != '\0' && *end == '\0' && value > 0) {
            pid = (pid_t)value;
        }
    }

    if (pid <= 0) {
        fprintf(stderr, "Usage: kill %%JOB_ID or kill PID\n");
        return;
    }

    if (kill(pid, SIGTERM) == 0) {
        printf("Termination signal sent to PID %d.\n", (int)pid);
    } else {
        perror("kill");
    }
}

void expand_variables(char *line) {
    char result[MAX_LINE];
    size_t out = 0;

    for (size_t i = 0; line[i] != '\0' && out < MAX_LINE - 1;) {
        if (line[i] == '$') {
            size_t start = i + 1;
            size_t end = start;

            while (isalnum((unsigned char)line[end]) || line[end] == '_') {
                end++;
            }

            if (end == start) {
                result[out++] = line[i++];
                continue;
            }

            char name[128];
            size_t length = end - start;
            if (length >= sizeof(name)) {
                length = sizeof(name) - 1;
            }

            memcpy(name, line + start, length);
            name[length] = '\0';

            const char *value = getenv(name);
            if (value != NULL) {
                size_t value_length = strlen(value);
                if (value_length > MAX_LINE - 1 - out) {
                    value_length = MAX_LINE - 1 - out;
                }
                memcpy(result + out, value, value_length);
                out += value_length;
            }

            i = end;
        } else {
            result[out++] = line[i++];
        }
    }

    result[out] = '\0';
    snprintf(line, MAX_LINE, "%s", result);
}

int main(void) {
    char line[MAX_LINE];

    while (1) {
        check_jobs();
        printf("mini-shell> ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            printf("\n");
            break;
        }

        line[strcspn(line, "\n")] = '\0';
        expand_variables(line);

        char original[MAX_LINE];
        snprintf(original, sizeof(original), "%s", line);

        char *args[MAX_ARGS];
        int argc = 0;
        char *token = strtok(line, " \t");

        while (token != NULL && argc < MAX_ARGS - 1) {
            args[argc++] = token;
            token = strtok(NULL, " \t");
        }
        args[argc] = NULL;

        if (argc == 0) {
            continue;
        }

        int background = 0;
        if (strcmp(args[argc - 1], "&") == 0) {
            background = 1;
            args[--argc] = NULL;
        }

        if (argc == 0) {
            continue;
        }

        if (strcmp(args[0], "exit") == 0) {
            break;
        }

        if (strcmp(args[0], "cd") == 0) {
            const char *path = argc > 1 ? args[1] : getenv("HOME");
            if (path == NULL || chdir(path) != 0) {
                perror("cd");
            }
            continue;
        }

        if (strcmp(args[0], "pwd") == 0) {
            char cwd[4096];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                printf("%s\n", cwd);
            } else {
                perror("pwd");
            }
            continue;
        }

        if (strcmp(args[0], "jobs") == 0) {
            show_jobs();
            continue;
        }

        if (strcmp(args[0], "kill") == 0) {
            if (argc != 2) {
                fprintf(stderr, "Usage: kill %%JOB_ID or kill PID\n");
            } else {
                stop_job(args[1]);
            }
            continue;
        }

        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            continue;
        }

        if (pid == 0) {
            execvp(args[0], args);
            perror(args[0]);
            _exit(127);
        }

        if (background) {
            int id = add_job(pid, original);
            if (id >= 0) {
                printf("[%d] Started (PID %d)\n", id, (int)pid);
            } else {
                fprintf(stderr, "Job list full; stopping new process.\n");
                kill(pid, SIGTERM);
                waitpid(pid, NULL, 0);
            }
        } else {
            int status;
            while (waitpid(pid, &status, 0) < 0) {
                if (errno != EINTR) {
                    perror("waitpid");
                    break;
                }
            }
        }
    }

    return 0;
}
