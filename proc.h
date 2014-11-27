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
    volatile struct list_proc * next;
    pid_t proc;
    int nohang;
    int deleted;
};
static volatile int ABRT_TRIGGER;
static volatile struct list_proc* ALL_CHILDS;
volatile int failed;
int sendsig(pid_t pid, int sig){
    return kill(pid,sig);
}
void push_child(pid_t pid,int h){
    volatile struct list_proc *lp = (volatile struct list_proc*)malloc(sizeof(volatile struct list_proc));
    lp->proc = pid;
    lp->next = ALL_CHILDS;
    ALL_CHILDS = lp;
    lp->nohang = h;
    lp->deleted = 0;
}

void free_lp(volatile struct list_proc* ptr){
    if(!ptr)return;
    if(ptr->deleted){
        free_lp(ptr->next);
        return;
    }
    if(ptr->nohang){
        free_lp(ptr->next);
        return;
    }
    int fail;
    waitpid(ptr->proc,&fail,0);
    failed = failed || !!WEXITSTATUS(fail);
    DBG_TRACE("exit code:%d",WEXITSTATUS(fail));
    ptr->deleted = 1;
    
    
}
void handle_abrt(int s){
    volatile struct list_proc* ptr = ALL_CHILDS;
    while(ptr){
        if(ptr->nohang == 0){
            kill(ptr->proc,SIGINT);
        }
        ptr = ptr->next;
    }
    ptr = ALL_CHILDS;
    if(!ABRT_TRIGGER){
        fprintf(stderr,"An error accused\n");
    }
    ABRT_TRIGGER = 1;
    failed = 1;
    free_lp(ALL_CHILDS);
}

void init_lp(){
    ALL_CHILDS = NULL;
    failed = 0;
    ABRT_TRIGGER = 0;
    signal(SIGABRT,handle_abrt);
}
void reinit_lp(){
    failed = 0;
    ABRT_TRIGGER = 0;
}
void execute(action* acts, int inp_fd){
    DBG_TRACE("");
    if(!acts ||  ABRT_TRIGGER){
        if(inp_fd){
            close(inp_fd);
        }
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
        if(acts->nohang){
            signal(SIGINT,SIG_IGN);
        }
        if(acts->io_ty == INP || acts->io_ty > ASOUTP){
            int inp = open(acts->filei, O_RDONLY | O_NONBLOCK);
            if(inp == -1){
                perror("Failed to open file for  input");
                exit(1);
            }
            dup2(inp,0);
            close(inp);
        }else if(inp_fd != 0){
            dup2(inp_fd,0);
            close(inp_fd);
        }
        if(acts->io_ty == OUTP || acts->io_ty == INOUTP){
            int outp = open(acts->fileo, O_WRONLY | O_TRUNC | O_CREAT, 0660);
            if(outp == -1){
                perror("Failed to open file for  output/r");
                exit(1);
            }
            dup2(outp,1);
            close(outp);
        } else if(acts->io_ty == ASOUTP || acts->io_ty == ASINOUTP){
            int outp = open(acts->fileo, O_WRONLY | O_APPEND | O_CREAT, 0660);
            if(outp == -1){
                perror("Failed to open file for  output/a");
                exit(1);
            }
            dup2(outp,1);
            close(outp);
        } else if(acts->is_conv){
            dup2(fd[1],1);
            close(fd[1]);
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
    push_child(pid,acts->nohang);
    if(inp_fd){
        close(inp_fd);
    }
    close(fd[1]);
    if(ABRT_TRIGGER){
        close(fd[0]);
        handle_abrt(SIGABRT);
    }else{
        execute(acts->next, fd[0]);
    }
}
#endif
