
/* 
 * File:   app_zqueue_interrupt.h
 * Author: Blazej Kita
 *
 * Created on 5 wrzesień 2016, 11:08
 */
#include <memory.h>
#include "gxh_scope.h"

#ifndef APP_ZQUEUE_INTERRUPT_H
#define	APP_ZQUEUE_INTERRUPT_H


 
class app_zi_queue {
    
private:    
    int size;   
   
     
    
    class item
    {
      public :
        item* back;
        item* next;
        gxh_message  interrupt;                     
        item() {  };
    };    
    
    item* firstToOut;
    
  
    
public:                 
    app_zi_queue(gxh_scope* scope);    
    int add(gxh_message* interrupt);
    bool get(gxh_message* interrupt);            
    int getSize();
};

#endif	/* APP_ZQUEUE_INTERRUPT_H */

