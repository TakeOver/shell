//
//  proc.h
//  dshellh
//
//  Created by Aleksei on 30/10/14.
//  Copyright (c) 2014 Aleksei. All rights reserved.
//

#ifndef dshellh_proc_h
#define dshellh_proc_h
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "parser.h"
#include <fcntl.h>
struct list_proc{
    struct list_proc * next;
    pid_t proc;
};
static struct list_proc* ALL_CHILDS;
int sendsig(pid_t pid, int sig){
    return kill(pid,sig);
}
void push_child(pid_t pid){
    struct list_proc *lp = (struct list_proc*)malloc(sizeof(struct list_proc));
    lp->proc = pid;
    lp->next = ALL_CHILDS;
    ALL_CHILDS = lp;
}

void handle_abrt(int s){
    struct list_proc* ptr = ALL_CHILDS;
    while(ptr){
        kill(ptr->proc,SIGINT);
        ptr = ptr->next;
    }
    ptr = ALL_CHILDS;
    fprintf(stderr,"An error accused\n");
    while(wait(NULL) != -1);
}

void init_lp(){
    ALL_CHILDS = NULL;
    signal(SIGABRT,handle_abrt);
}
void free_lp(){
    while(ALL_CHILDS){
        struct list_proc *tmp = ALL_CHILDS->next;
        free(ALL_CHILDS);
        ALL_CHILDS = tmp;
    }
}
void execute(action* acts, int inp_fd){
    DBG_TRACE("");
    if(!acts){
        return;
    }
    int fd[2];
    
    if(pipe(fd) == -1){
        handle_abrt(SIGABRT);
    }
    int pid = fork();
    if(pid == -1){
        handle_abrt(SIGABRT);
    }
    if(pid == 0){
        if(acts->io_ty == 1 || acts->io_ty > 3){
            int inp = open(acts->filei, O_RDONLY | O_NONBLOCK);
            if(inp == -1){
                perror("Failed to open file for  input");
                exit(1);
            }
            dup2(inp,0);
        }else if(inp_fd != 0){
            dup2(inp_fd,0);
        }
        if(acts->io_ty == 2 || acts->io_ty == 4){
            int outp = open(acts->fileo, O_WRONLY | O_TRUNC | O_CREAT | O_NONBLOCK);
            if(outp == -1){
                perror("Failed to open file for  output/r");
                exit(1);
            }
            dup2(outp,1);
        } else if(acts->io_ty == 3 || acts->io_ty == 5){
            int outp = open(acts->fileo, O_WRONLY | O_APPEND | O_CREAT | O_NONBLOCK);
            if(outp == -1){
                perror("Failed to open file for  output/a");
                exit(1);
            }
            dup2(outp,1);
        } else if(acts->is_conv){
            dup2(fd[1],1);
        }
        char** argv = (char**)malloc(sizeof(void*)*(acts->size+1));
        for (int i = 0; i < acts->size; ++i) {
            argv[i] = (char*) malloc(sizeof(char)*acts->tok[i]->len + sizeof(char));
            strcpy(argv[i], acts->tok[i]->str);
        }
        argv[acts->size] = NULL;
        DBG_TRACE("calling func:%s",(*acts->tok)->str);
        execvp((*acts->tok)->str, argv);
        fprintf(stderr,"Cannot exec %s\n",argv[0]);
        kill(getppid(),SIGABRT);
        exit(1);
    }
    if(inp_fd){
        close(inp_fd);
    }
    close(fd[1]);
    execute(acts->next, fd[0]);
}
#endif
