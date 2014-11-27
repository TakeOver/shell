//
//  main.c
//  dshellh
//
//  Created by Aleksei on 29/10/14.
//  Copyright (c) 2014 Aleksei. All rights reserved.
//
#define USE_PRETTY_FUNCTION
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include "proc.h"

int main(int argc, char const *argv[])
{
    signal(SIGINT,SIG_IGN);
    init_lp();
    while(1){
        reinit_lp();
        printf("shell:>");
        char* str = (char*)malloc(255);
        if(!fgets(str,254,stdin)){
            free(str);
            return 1;
        }
        if(!strcmp(str,"exit\n")){
            free(str);
            return 0;
        }
        if(str == NULL || strlen(str) == 0){
            continue;
        }
        int pos = 0;
        token ** tkns = (token**)malloc(256*sizeof(void*));
        int i = 0;
        while(str[pos]){
            token* t = gettok(str,&pos);
            if(!t){
                free(str);
                break;
            }
            tkns[i++] = t;
        }
        tkns[i] = NULL;
        token ** tks = tkns;
        struct shell_command* res = parse(&tks);
        while(res){
            reinit_lp();
            DBG_TRACE("logic:%d",res->logic);
            execute(res->actions,0);
            free_lp(ALL_CHILDS);
            if(failed){
                if(res->logic == 2){
                    while(res && res->logic){
                        res = res->next;
                    }
                }
            }else if(res->logic == 1){
                while(res && res->logic){
                    res = res->next;
                }
            }
            if(res)
                res = res->next;
        }
        for (int j = 0; j < i; ++j) {
            free(tkns[j]->str);
            free(tkns[j]);
        }
        free(tkns);
    }
    return 0;
}