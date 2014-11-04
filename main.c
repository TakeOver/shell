//
//  main.c
//  dshellh
//
//  Created by Aleksei on 29/10/14.
//  Copyright (c) 2014 Aleksei. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "proc.h"
int main(int argc, char const *argv[])
{
   while(1){
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
       action* tmp = res;
       while(tmp && 0){
            for (int j = 0; j < tmp->size; ++j) {
                printf("[%s conv:%i args:%i io:%i filei:\"%s\" fileo:\"%s\"]\n",tmp->tok[j]->str,tmp->is_conv,tmp->size, tmp->io_ty, tmp->filei?tmp->filei:"",tmp->fileo?tmp->fileo:"");
            }
            tmp = tmp->next;
        }
       if(!res){
            printf("shit!\n");
        }


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
        }    }
    return 0;
}