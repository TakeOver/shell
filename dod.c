//
//  dod.c
//  dshellh
//
//  Created by Aleksei on 06/11/14.
//  Copyright (c) 2014 Aleksei. All rights reserved.
//

#include <stdio.h>
#include <string.h>
int main(int argc,char** argv){
    int oct = 0;
    if(argc == 1 || argc > 3){
        printf("Incorrect data\n");
        return 1;
    }
    if(argc == 3){
        if(strcmp(argv[2],"-b") == 0){
            oct = 1;
        }else{
            printf("Incorrect syntax\n");
            return 2;
        }
    }
    FILE * f = fopen(argv[1],"r");
    if(!f){
        printf("Failed to open file\n");
        return 3;
    }
    int len = 0;
    int n = 0;
    int c;
    while(!feof(f)){
        if(!((len)%10))
            printf("%0*i\t",4,len+1);
        ++len;
        c = getc(f);
        if(c == '\n'){
            printf("\\n ");
        }else if(c == '\t'){
            printf("\\t ");
        }else{
            putchar(c);
            putchar(' ');
        }
        if(oct){
            printf("%o ",c);
        }
        ++n;
        if( n == 10){
            n = 0;
            putchar('\n');
        }
    }
    if(((len) % 10)){
        putchar('\n');
    }
    fclose(f);
    return 0;
}