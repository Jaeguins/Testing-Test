#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#define BLANK_STRING  " "
#define PROMPT_STRING ">>"
#define QUIT_STRING "q"
#include <unistd.h>

#define MAX_BUFFER 256

#include <string.h>
#include <stdlib.h>

int makeargv(char* s, char* delimiters, char*** argvp) {
    char* t;
    char* snew;
    int numtokens;
    int i; /* snew is real start of string after skipping leading delimiters */
    snew = s + strspn(s, delimiters); /* create space for a copy of snew in t */
    if ((t = calloc(strlen(snew) + 1, sizeof(char))) == NULL) {
        *argvp = NULL;
        numtokens = -1;
    }
    else {
        /* count the number of tokens in snew */
        strcpy(t, snew);
        if (strtok(t, delimiters) == NULL) numtokens = 0;
        else
            for (numtokens = 1; strtok(NULL, delimiters) != NULL; numtokens++);
        if ((*argvp = calloc(numtokens + 1, sizeof(char*))) == NULL) {
            free(t);
            numtokens = -1;
        }
        else {
            /* insert pointers to tokens into the array */
            if (numtokens > 0) {
                strcpy(t, snew);
                **argvp = strtok(t, delimiters);
                for (i = 1; i < numtokens + 1; i++)* ((*argvp) + i) = strtok(NULL, delimiters);
            }
            else {
                **argvp = NULL;
                free(t);
            }
        }
    }
    return numtokens;
}


int main(int argc, char* argv[]) {
    char inbuf[MAX_BUFFER];
    char** chargv;
    for (;;) {
        printf(PROMPT_STRING);
        gets(inbuf);
        if (strcmp(inbuf, QUIT_STRING) == 0) break;
        else {
            if (fork() == 0) {
                if (makeargv(inbuf, BLANK_STRING, &chargv) > 0) {
                    if(strcmp(chargv[0],"cd")==0){
                        int chdir(chargv[1]);
                    }
                    execvp(chargv[0], chargv);
                    printf("%s: command not found.", chargv[0]);
                }
                exit(1);
            }
            wait(NULL);
        }
    }
    return 0;
}