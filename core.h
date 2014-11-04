//
//  core.h
//  dshellh
//
//  Created by Aleksei on 29/10/14.
//  Copyright (c) 2014 Aleksei. All rights reserved.
//

#ifndef dshellh_core_h
#define dshellh_core_h

enum tokty_{
    UNDEF = 0,
    CONV,
    REIN,
    REOUT,
    IDENT,
    SPECOUT
};
struct token_{
    char* str;
    int len;
    enum tokty_ ty;
};
typedef struct token_ token;
typedef enum tokty_ tokty;
char splitters[] = {'>','<','|',' ','\n','\t',0};
int is_split(char c){
    int i = 0;
    while(splitters[i] && splitters[i] != c)++i;
    return splitters[i] == c;
}
token* gettok(char* str, int *pos){
    token * tk= (token*)malloc(sizeof(token));
    tk->str = (char*)malloc(strlen(str)+1);
    if(!str[*pos]) return NULL;
    else{
        if(is_split(str[*pos])){
            if(str[*pos] == str[*pos+1] && str[*pos] == '>'){
                tk->str[0] = tk->str[1] = '>';
                tk->str[2] = 0;
                tk->len = 2;
                tk->ty = SPECOUT;
                (*pos)+=2;
            }else{
                switch(str[*pos]){
                    case '>':{
                        tk->str[0] = '>';
                        tk->str[1] = 0;
                        tk->len = 1;
                        tk->ty = REOUT;
                    }break;
                        
                    case '<':{
                        tk->str[0] = '<';
                        tk->str[1] = 0;
                        tk->len = 1;
                        tk->ty = REIN;
                    }break;
                        
                    case '|':{
                        tk->str[0] = '|';
                        tk->str[1] = 0;
                        tk->len = 1;
                        tk->ty = CONV;		
                    }break;
                    default:{
                        if(str[*pos]){
                            ++*pos;
                            return gettok(str,pos);
                        }else return NULL;
                    }break;
                }
                
                if(str[*pos]){
                    ++*pos;
                }
            }
            return tk;
        }
        tk->len = 0;
        tk->ty = IDENT;
        while(!is_split(str[*pos])){
            tk->str[tk->len++] = str[(*pos)++];
        }
        tk->str[tk->len] = 0;
    }
    return tk;
}
#endif
