//
//  dtail.c
//  dshellh
//
//  Created by Aleksei on 06/11/14.
//  Copyright (c) 2014 Aleksei. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct list_{
    char* str;
    struct list_* next;
};
typedef struct list_ list;
void push(list** l,char* str){
    if(!*l){
        *l = (list*)malloc(sizeof(list));
        (*l)->str = str;
        (*l)->next = NULL;
    }else{
        push(&(*l)->next,str);
    }
}
void freelist(list*l){
    if(!l)return;
    list* tmp = l->next;
    free(l);
    freelist(tmp);
}
int main(int argc, char** argv){
    int reverse = 0;
    int n = 10;
    if(argc == 1){
        printf("File name not specified\n");
        return 1;
    }else if(argc == 3){
        if(strlen(argv[2])<2){
            printf("Incorrect syntax\n");
            return 2;
        }
        if(argv[2][0] == '-'){
            reverse = 1;
        }else if(argv[2][0] != '+'){
            printf("Incorrect syntax\n");
            return 3;
        }
        n = atoi(argv[2]+1);
    } else if(argc != 2){
        printf("Incorrect amount of arguments\n");
        return 4;
    }
    FILE* f = fopen(argv[1],"r");
    if(!f){
        printf("Cannot open file\n");
        return 5;
    }
    list *head = NULL;
    list * ptr = NULL;
    char* str;
    int size = 0;
    while(!feof(f)){
        str = (char*)malloc(256);
        if(!fgets(str,255, f)){
            free(str);
            break;
        }
        size++;
        if(!head){
            push(&head,str);
            ptr = head;
        }else{
            push(&ptr,str);
            ptr = ptr->next;
        }
    }
    if(!reverse){
        n = size - n;
        if(n < 0){
            n = 0;
        }
    }
    ptr = head;
    while(ptr && (n --)) ptr = ptr->next;
    while(ptr){
            printf("%s",ptr->str);
            ptr = ptr->next;
    }
    
    freelist(head);
    fclose(f);
    return 0;
}