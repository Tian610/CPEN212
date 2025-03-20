#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

// Stuff so waitpid doesnt scream everytime i attempt to compile
#include <sys/types.h>
#include <sys/wait.h>

#define MAXLINE 1024
#define MAX_JOBS 32

int jobIDCounter = 1;

typedef struct {
    int jobID;
    pid_t pid;
    char *command;
} Job;

Job jobs[MAX_JOBS];

// Execute main commands here, use signal handler to handle other stuff, like ending bg processes
void eval(const char **toks, bool bg) { // bg is true if command ended with &
    assert(toks);
    if (*toks == NULL) return;

    // Calling Quit
    if (strcmp(toks[0], "quit") == 0) {
        if (toks[1] != NULL) {
            const char *msg = "ERROR: quit takes no arguments\n";
            write(STDERR_FILENO, msg, strlen(msg));
        } else {
            exit(0);
        }
    }

    else if (strcmp(toks[0], "jobs") == 0) {
        for (int i = 0; i < MAX_JOBS; i++) {
            if (jobs[i].jobID != 0) {
                char msg[100];
                snprintf(msg, sizeof(msg), "[%d] (%d)  running  %s\n", jobs[i].jobID, jobs[i].pid, jobs[i].command);
                write(STDOUT_FILENO, msg, strlen(msg));
            }
        }
    }

    // Execute all other programs. I'll add the process counting and stuff later
    else if (jobIDCounter <= 32) {

        pid_t p1 = fork();
        
        // if PID is 0, we're the child process, so execute the program and ill figure out signals later
        if (p1 == 0) {                   
            int err = execvp(toks[0], (char *const *)toks);

            // Display an Error messaage if the process execution fails for any reason
            if (err == -1) {
                char msg[100];
                snprintf(msg, sizeof(msg), "ERROR: cannot run %s\n", toks[0]);
                write(STDOUT_FILENO, msg, strlen(msg));
                exit(1);
            }
        }

        // So for background, we need to:
        // Print the fact that its running and keep track of its process id
        
        // If we're not the child and BG is true, we can keep running!
        else if (bg) {
            // Find an empty slot in our job counter
            for (int i = 0; i < MAX_JOBS; i++) {
                if (jobs[i].jobID == 0) {
                    
                    jobs[i].jobID = jobIDCounter;
                    jobs[i].pid = p1;
                    jobs[i].command = strdup(toks[0]);

                    char msg[100];
                    snprintf(msg, sizeof(msg), "[%d] (%d)  running  %s\n", jobIDCounter, p1, jobs[i].command);
                    jobIDCounter++;
                    write(STDOUT_FILENO, msg, strlen(msg));
                    break;
                }
            }
        }

    } 
    
    // We cannot handle more than 32 concurrent processes
    else {
        const char *msg = "ERROR: too many jobs";
            write(STDERR_FILENO, msg, strlen(msg));
    }
}

void parse_and_eval(char *s) {
    assert(s);
    const char *toks[MAXLINE+1];
    
    while (*s != '\0') {
        bool end = false;
        bool bg = false;
        int t = 0;

        while (*s != '\0' && !end) {
            while (*s == '\n' || *s == '\t' || *s == ' ') ++s;
            if (*s != ';' && *s != '&' && *s != '\0') toks[t++] = s;
            while (strchr("&;\n\t ", *s) == NULL) ++s;
            switch (*s) {
            case '&':
                bg = true;
                end = true;
                break;
            case ';':
                end = true;
                break;
            }
            if (*s) *s++ = '\0';
        }
        toks[t] = NULL;
        eval(toks, bg);
    }
}

void prompt() {
    const char *prompt = "crash> ";
    ssize_t nbytes = write(STDOUT_FILENO, prompt, strlen(prompt));
}

int repl() {
    char *buf = NULL;
    size_t len = 0;
    while (prompt(), getline(&buf, &len, stdin) != -1) {
        parse_and_eval(buf);
    }

    if (feof(stdin)) {
        printf("\nExiting Shell...\n");
        free(buf);
        return 0;
    }

    if (buf != NULL) free(buf);
    if (ferror(stdin)) {
        perror("ERROR");
        return 1;
    }
    return 0;
}

int main(int argc, char **argv) {
    return repl();
}
