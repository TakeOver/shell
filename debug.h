//
//  debug.h
//  dshellh
//
//  Created by Aleksei on 30/10/14.
//  Copyright (c) 2014 Aleksei. All rights reserved.
//

#ifndef dshellh_debug_h
#define dshellh_debug_h

#ifdef DEBUG_HUY
    #ifdef USE_PRETTY_FUNCTION
        #define ___FUNC___ __PRETTY_FUNCTION__
    #else
        #define ___FUNC___ __FUNCTION__
    #endif
    #define DBG_TRACE(format,...) fprintf(stdout,"[\"%s\":%d][%s][" format "]\n", __FILE__,__LINE__,___FUNC___, ##__VA_ARGS__)
#else
    #define DBG_TRACE(a,...) ((void)0)
#endif
#endif
