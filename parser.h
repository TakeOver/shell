//
//  parser.h
//  dshellh
//
//  Created by Aleksei on 30/10/14.
//  Copyright (c) 2014 Aleksei. All rights reserved.
//

#ifndef dshellh_parser_h
#define dshellh_parser_h
#include "core.h"
#include "proc.h"
#include <assert.h>
#include "debug.h"
enum io_type{
    NONE = 0,
    INP,
    OUTP,
    ASOUTP,
    INOUTP,
    ASINOUTP
    
};
struct action_shell{
    token** tok; // obviusly, tokens
    int size; // number of tokens
    int is_conv; // if 1 then necceraly next !=NULL and next->io_ty != 1
    char* filei, * fileo;
    enum io_type io_ty; // 0 - none, 1 - input, 2 - output, 3 - assign output, 4 - both 1 2, but necceraly is_conv == 0 and next == NULL, 5 both 1 3
    struct action_shell * next;
    int nohang;
};
struct shell_command{
    struct action_shell * actions;
    int logic; // 0 - none, 1 - or, 2 - and
    struct shell_command * next;
};
typedef struct action_shell action;
#define SYNTAXERR(msg) do{printf("Incorrect syntax: %s.\n",msg);PARSER_ERROR = 1; return NULL;}while(0)

action * create_action(token**tok,int size, int is_conv, char* filei,char* fileo,int io_ty){
    DBG_TRACE("");
    action* res = (action*)malloc(sizeof(action));
    res->tok = tok;
    res->size = size;
    res->is_conv = is_conv;
    res->filei = filei;
    res->fileo = fileo;
    res->io_ty = io_ty;
    res->next = NULL;
    return res;
}
static int PARSER_ERROR = 0;
action* parse_prog_call(token*** _tkns){
    DBG_TRACE("");
    token ** tkns = *_tkns;
    action* act = (action*)malloc(sizeof(action));
    act->next = NULL;
    int size= 0;
    char* filei = NULL;
    int is_inp = 0;
    while(tkns[size]){
        if(tkns[size]->ty == IDENT){
            size++;
        }else{
            break;
        }
    }
    act->tok = tkns;
    int is_conv = 0;
    int io_ty = 0;
    int nohang = 0;
    char* fileo = NULL;
    while(tkns[size]){
        if(tkns[size]->ty == REIN){
            if(filei){
                SYNTAXERR("2 files scpecified for input");
            }
            DBG_TRACE("0");
            is_inp = 1;
            if(!tkns[size+1]){
                free(act);
                SYNTAXERR("no file specified for input");
            }
            if((tkns[size+1]->ty != IDENT)){
                free(act);
                SYNTAXERR("input file name incorrect");
            }
            filei = tkns[size+1]->str;
            tkns = tkns+2;
        }else if(tkns[size]->ty == REOUT || tkns[size]->ty == SPECOUT){
            if(fileo){
                SYNTAXERR("2 files specified for output");
            }
            io_ty = 2 + (tkns[size]->ty == SPECOUT);
            DBG_TRACE("1");
            if(!tkns[size+1]){
                free(act);
                SYNTAXERR("no file scpecified for output");
            }
            if(tkns[size+1]->ty != IDENT){
                free(act);
                SYNTAXERR("output file name incorrect");
            }
            if(tkns[size+2] && 0){
                free(act);
                SYNTAXERR("end of commands expected");
            }
            fileo = tkns[size+1]->str;
            tkns+=2;
        }else{
            if(tkns[size]->ty == BREAK){
              //  ++tkns;
                is_conv = 0;
                break;
            }else if((tkns)[size]->ty == NOHANG && !nohang){
                nohang = 1;
                ++tkns;
                continue;
            }
            is_conv = tkns[size]->ty == CONV;
            break;
        }
    }
    act->fileo = fileo;
    act->io_ty = io_ty;
    if(is_inp){
        if(io_ty == NONE){
            act->io_ty = INP;
        }else{
            act->io_ty+=OUTP;
        }
    }
    act->is_conv = is_conv;
    act->filei = filei;
    act->size = size;
    act->next = NULL;
    act->nohang = nohang;
    *_tkns = tkns + size + is_conv;
    return act;
}
action* parse_(token*** tkns){
    DBG_TRACE("");
    if(*tkns == 0 || **tkns == 0 || (**tkns)->ty == OR || (**tkns)->ty == AND || (**tkns)->ty == BREAK){
        return NULL;
    }
    if((**tkns)->ty != IDENT){
        SYNTAXERR("identifer expected");
    }
    if(!(*tkns)[1]){
        action * tmp = create_action((*tkns), 1, 0, 0, 0, NONE);
        *tkns = *tkns+1;
        return tmp;
    }
    action* tmp = parse_prog_call(tkns);
    if(PARSER_ERROR){
        return NULL;
    }
    tmp->next = parse_(tkns);
    
    return tmp;
}
struct shell_command*parse(token***tkns){
    action* tmp = parse_(tkns);
    if(!tmp){
        return NULL;
    }
    struct shell_command* res= (struct shell_command*)malloc(sizeof(struct shell_command));
    res->actions = tmp;
    if((**tkns)!= NULL && (**tkns)->ty == OR){
        res->logic = 1;
      //  ++*tkns;
    }else
        if((**tkns)!= NULL && (**tkns)->ty == AND){
            res->logic = 2;
     //       ++*tkns;
        }else
            res->logic = 0;
    if(!res->logic && ((**tkns) && ((**tkns)->ty != BREAK))){
        if(**tkns){
            free(res);
            SYNTAXERR("End of commands expected");
        }
    }else{
        ++*tkns;
    }
    res->next = parse(tkns);
    if(PARSER_ERROR){
        free(res);
        return NULL;
    }
    return res;
}


#endif
