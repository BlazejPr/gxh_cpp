/* 
 * File:   app_zqueue_interrupt.h
 * Author: Blazej Kita
 *
 * Created on 5 wrzesień 2016, 11:08
 */
#include <memory.h>
#include "gxh_scope.h"

#ifndef APP_ZCQUEUE_INTERRUPT_H
#define	APP_ZCQUEUE_INTERRUPT_H

 

class app_zc_queue {
public:
    app_zc_queue( gxh_scope* appHandle);
    app_zc_queue(const app_zc_queue& orig);
    virtual ~app_zc_queue();
    
    /**
     * Add next elemtn to list
     */
    int add(gxh_message* item);
    /**
     * Get first inserted element of array. You should use delete operatior
     * @return 
     */
    bool get(gxh_message* message);     
    bool get(unsigned int gxhHandle, gxh_message* message);     
   
    
    int getCount();
    
private:
    
   
    
    class item
    {
      public :
        item* back;
        item* next;
        gxh_message  command;                
        long int created_at;
    };
    
           
    item* startElement;
    item* endElement;
    
    int count_element;

    gxh_scope* appHandle;
};

#endif	/* APP_ZQUEUE_INTERRUPT_H */


