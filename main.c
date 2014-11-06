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

#include "proc.h"

int main(int argc, char const *argv[])
{
    while(1){
        init_lp();
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
        action* res = parse(&tks);
        execute(res, 0);
        while(wait(NULL) != -1);
        for (int j = 0; j < i; ++j) {
            free(tkns[j]->str);
            free(tkns[j]);
        }
        free(tkns);
        while(res){
            action* tmp = res->next;
            free(res);
            res = tmp;
        }
        free_lp();
    }
    return 0;
}