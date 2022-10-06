
/* 
 * File:   app_zqueue_interrupt.cpp
 * Author: Blazej Kita
 * 
 * Created on 5 wrzesień 2016, 11:08
 */

#include "app_zi_queue.h"

//------------------------------------------------------------------------------

app_zi_queue::app_zi_queue(gxh_scope* scope)
{
    this->size = 0;    
    firstToOut = NULL; //98r
  
    
}

//------------------------------------------------------------------------------

int app_zi_queue::add(gxh_message* interrupt)
{  
   
    
    if(this->size == 0)
    {
        firstToOut            = new item();   
        firstToOut->back      = NULL;
        firstToOut->next      = NULL;        
        memcpy(&firstToOut->interrupt, interrupt, sizeof(gxh_message));
        
        this->size++;
        return this->size;
    }
    
    //Find the last element of the list        
    item* tmpItem = firstToOut;       
    
    while(1)
    {                
        if(tmpItem->back == NULL)
        {
             item* newLastElemnent = new item();  
             newLastElemnent->next = tmpItem;
             newLastElemnent->back = NULL; 
             memcpy(&newLastElemnent->interrupt, interrupt, sizeof(gxh_message));
             
            
             
             tmpItem-> back = newLastElemnent;
             this->size++;
             break;
        }
        
        tmpItem = tmpItem->back;
    }
    
    
  
    return this->size;
}

//------------------------------------------------------------------------------

 bool app_zi_queue::get(gxh_message* interrupt)
 {    
    
     
    if(this->size ==0)  
    {        
        return false;
    }
     
     
    item* tmpItem = firstToOut;    
    if(tmpItem == NULL) return false; // first element doesn't create
                          
          
    if(firstToOut->back != NULL)
    {     
        firstToOut = firstToOut->back;
        firstToOut->next = NULL;                              
    }
                     
    memcpy(interrupt, &tmpItem->interrupt,sizeof(gxh_message));
            
   
    delete tmpItem;
    tmpItem = NULL;
    this->size--;       
 
   // if(interrupt->func_number == 500) std::cout<<"SIZE: "<<this->size<<"\n";
   
    return true;
 }

//------------------------------------------------------------------------------
 
 int app_zi_queue::getSize()
 {
     return this->size;
 }
 
//------------------------------------------------------------------------------

 
 
 
//------------------------------------------------------------------------------