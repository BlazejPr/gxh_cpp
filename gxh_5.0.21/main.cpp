/* 
 * File:   main.cpp
 * Author: Blazej Kita
 *
 * Created on 15 may 2018, 10:50
 */


#include "gxh_core.h"
#include "signal.h"

void sig_handler(int sig);


 

int main(int argc, char** argv) 
{
    
        sigset_t iset;
    struct sigaction act;     
	
    sigemptyset(&iset);
    act.sa_handler = &sig_handler;
    act.sa_mask = iset;
    act.sa_flags = 0;               //program nigdy nie zostanie 'zabity'
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGPIPE, &act, NULL);
    sigaction(SIGSEGV, &act, NULL);
    
    gxh_scope* scope = new gxh_core(argc, argv);
    delete scope;
    
    return 0;
}


void sig_handler(int sig) {
    
    if(sig == SIGSEGV )
    {
       printf("\n ---------------------------------------------------\n") ;
       printf("\nInvalid memory reference.\n") ;
       printf("\n ---------------------------------------------------\n") ;
       
       exit(0);
    }
    
    if(sig == SIGPIPE )
    {
       printf("\n ---------------------------------------------------\n") ;
       printf("Write on a pipe with no one to read it.") ;
       printf("\n ---------------------------------------------------\n") ;
     //  exit(0);
       return;
    }
    
    if(sig == SIGINT )
    {
       printf("\n ---------------------------------------------------\n") ;
       printf("Terminal interrupt signal.") ;
       printf("\n ---------------------------------------------------\n") ;
       exit(0);
    }
          
    
    printf("\n ---------------------------------------------------\n") ;
    printf("Interrupt signal: %d ",sig);
    printf("\n ---------------------------------------------------\n") ;
    exit(0);
}

