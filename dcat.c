//
//  dcat.c
//  dshellh
//
//  Created by Aleksei on 06/11/14.
//  Copyright (c) 2014 Aleksei. All rights reserved.
//

#include <stdio.h>
#include <string.h>
int main(int argc, char** argv){
    if(argc == 1){
        printf("File name not specified.\n");
        return 1;
    }
    int count = 0;
    FILE * f = fopen(argv[1],"r");
    if(!f){
        printf("File not found\n");
        return 2;
    }
    if(argc == 3){
        if(!strcmp(argv[2],"-n")){
            count = 1;
        }else{
            printf("Format: dcat filename [-n]\n");
            fclose(f);
            return 4;
        }
    }else if(argc != 2){
        printf("Incorrect amount of arguments\n");
        fclose(f);
        return 3;
    }
    int len = 0;
    while(!feof(f)){
        int c = fgetc(f);
        if(feof(f))break;
        if(count){
            printf("\t%i ",len++);
        }
        while(((c != '\n')) && (c != EOF) ){
            putchar(c);
            c = getc(f);
        }
        if(!feof(f))
            putchar('\n');
    }
    fclose(f);
    return 0;
}