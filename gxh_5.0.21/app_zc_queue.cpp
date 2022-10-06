/* 
 * File:   app_zqueue_interrupt.cpp
 * Author: Blazej Kita
 * 
 * Created on 5 wrzesień 2016, 11:08
 */

#include "app_zc_queue.h"


app_zc_queue::app_zc_queue(  gxh_scope* appHandle)
{
    this->appHandle = appHandle;
    this->count_element = 0; 
    this->startElement = NULL;
    this->endElement = NULL;
}

app_zc_queue::app_zc_queue(const app_zc_queue& orig) {
}

app_zc_queue::~app_zc_queue() 
{
    for(int i=0; i<this->count_element+1; i++)
    {
        gxh_message msg;
        this->get(&msg); //pobierz a zaraze usunie wszystkie pozostale elementy..
    }
    
    this->count_element = 0; 
    this->startElement = NULL;
    this->endElement = NULL;
}





int app_zc_queue::add(gxh_message* command)
{           
    
    item* new_element = new item();
    
    memcpy( &new_element->command,command, sizeof(gxh_message) );
    
    new_element->created_at = gxh_getTimestamp();
  
    if(this->count_element == 0)
    {
        this->startElement = new_element;
        this->endElement   = new_element;

        this->startElement->next = this->endElement;
        this->endElement->back = this->startElement;        
    }
    
    
    if(this->count_element >0)
    {
        this->endElement->next = new_element;
        new_element->back = this->endElement;
        this->endElement = new_element;
    }
    
    
    this->count_element++;        
    
    
      
  //  std::string msg = "addCommand, actual elements: ";
   // msg.append(this->appHandle->IntToString(this->count_element));
    
   // this->appHandle->show_log("app_zc_queue","add", msg.c_str());  
    
    
    return this->count_element;
}



bool app_zc_queue::get(gxh_message* message)
{ 
    if(this->count_element == 0) return NULL;
   
    if(this->count_element == 1)
    {        
       memcpy(message, &this->endElement->command, sizeof(gxh_message));
       delete this->endElement;
       
       this->startElement = NULL;
       this->endElement   = NULL;
       
       this->count_element--;
       return true;
    }
    
    
    
    if(this->count_element > 1)
    {       
       memcpy(message, &this->startElement->command, sizeof(gxh_message));
       item* tmpEnd =  this->startElement;
       
       this->startElement = this->startElement->next;
       
       delete tmpEnd; //usun element z listy aby nie wisial w pamieci..
       
       this->count_element--;
       return true;  
    }
    
    return false;
}


 


bool app_zc_queue::get(unsigned int gxhHandle, gxh_message* message)
{ 
 
      
       if(this->count_element == 0) return false;
      
      
       if(this->count_element == 1 && this->endElement->command.gxh_handle == gxhHandle)
       {          
         memcpy(message, &this->endElement->command, sizeof(gxh_message));
         delete this->endElement;
       
         this->startElement = NULL;
         this->endElement   = NULL;
       
         this->count_element--;
         return true;
      }
      
      if(this->count_element == 1 && this->endElement->command.gxh_handle != gxhHandle) return false;
      
        
      
      item* itelator = this->startElement;      
      for(int i=0; i<this->count_element; i++)
      {
          // this->appHandle->show_log((char* )"app_zc_queue",(char* )"getKKKKK",  this->appHandle->IntToString(this->count_element)); 
          if( itelator->command.gxh_handle == gxhHandle )
          {                            
               item* itTmp = itelator;                            
               memcpy(message, &itelator->command, sizeof(gxh_message) );
 
               
               if(  itelator!= this->startElement && itelator != this->endElement ) //iterator jest środkowym elementem...nie ostatnim ani nie pierwszym..
               {
                  // this->appHandle->show_log((char* )"app_zc_queue",(char* )"getAAAAAAA",  this->appHandle->IntToString(this->count_element)); 
                   itelator->back->next = itelator->next;
                   itelator->next->back = itelator->back;
               }
               
               if(  itelator == this->endElement ) //iterator jest pierwszym elementem - swiezo dodanym...
               {       
                  // this->appHandle->show_log((char* )"app_zc_queue",(char* )"getBBBBB",  this->appHandle->IntToString(this->count_element)); 
                   this->endElement = itelator->back;                   
               }
               
               
               
               if( itelator == this->startElement ) //iterator jest ostatnim elementem - dawno dodanym...
               {                   
                  // this->appHandle->show_log((char* )"app_zc_queue",(char* )"getCCCC",  this->appHandle->IntToString(this->count_element));   
                   this->startElement = itelator->next;
               }
               
              // this->appHandle->show_log((char* )"app_zc_queue",(char* )"getXXXXXXX",  this->appHandle->IntToString(this->count_element)); 
               
               delete itTmp;                                            
               this->count_element--;                              
               return true;
               
          }  
          
           
          itelator = itelator->next;
      }            
 
       
      return false;
}



int app_zc_queue::getCount()
{
    return this->count_element;
}


