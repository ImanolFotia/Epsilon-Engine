/* NOT MY CODE
 * http://media.unpythonic.net/emergent-files/01108826729/popen2.c
 * */

#pragma once

#include <cstdio>

#ifdef __linux__

#include <sys/types.h>
#include <unistd.h>

#endif

struct popen2_t {
    pid_t child_pid;
    int from_child, to_child;
};

static int popen2(const char *cmdline, struct popen2_t *childinfo) {
#ifdef __linux__
    pid_t p;
    int pipe_stdin[2], pipe_stdout[2];

    if (pipe(pipe_stdin)) return -1;
    if (pipe(pipe_stdout)) return -1;

    printf("pipe_stdin[0] = %d, pipe_stdin[1] = %d\n", pipe_stdin[0], pipe_stdin[1]);
    printf("pipe_stdout[0] = %d, pipe_stdout[1] = %d\n", pipe_stdout[0], pipe_stdout[1]);

    p = fork();
    if (p < 0) return p; /* Fork failed */
    if (p == 0) { /* child */
        close(pipe_stdin[1]);
        dup2(pipe_stdin[0], 0);
        close(pipe_stdout[0]);
        dup2(pipe_stdout[1], 1);
        execl(cmdline, NULL);
        perror("execl");
        exit(99);
    }
    childinfo->child_pid = p;
    childinfo->to_child = pipe_stdin[1];
    childinfo->from_child = pipe_stdout[0];
#endif
    return 0;
}

static std::string read_from_pipe(int pipe) {
    const int chunk = 4096;
    char buf[chunk] = {0};
    memset(buf, 0, chunk);
    long nbytes = 0;

    std::string str;

    for (;;) {
#ifdef __linux__
        nbytes = read(pipe, buf, chunk);
#endif
        for (int i = 0; i < nbytes; i++) {
            if (buf[i] == '\0') continue;
            str += buf[i];
        }
        if (nbytes < chunk) break;
    }
    return str;
}

static void write_to_pipe(int pipe, const std::string &data) {
#ifdef __linux__
    write(pipe, data.data(), data.size());
#endif
}