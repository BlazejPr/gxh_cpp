/* 
 * File:   app_module_timer.h
 * Author: Blazej Kita
 *
 * Created on 02 luty 2021, 14:31
 */

#include "app_module.h"

#ifndef APP_MODULE_TIMER_H
#define	APP_MODULE_TIMER_H

 

#define MAX_TIMER_BUFFER  10 //WIĘCEJ NIE DA RADY...
  

class app_module_timer : public app_module{
public:
              app_module_timer(gxh_scope* appHomeHandle); 
              virtual ~app_module_timer();       
   
    
                /**
              * Execute interrupt from GXHSystem
              * @param inMsg
              * @param outMsg
              * @return 
              */
             bool execute(gxh_message* inMsg, gxh_message* outMsg);
             
             /**
              * Method is call when GXHSystem is going to delete object from memory
              */
             void forceCancel();      
             void forceSynchronize();  
             void onClick(int ih_driver_port_id, int listener_status);
 
    
 private: 
        
          int timer_count;
          ih_timer timers[MAX_TIMER_BUFFER];
       
          bool goZeroTime(gxh_message* inMsg,gxh_message* message);
          bool goRefresh(gxh_message* inMsg,gxh_message* message);
          bool changeStatus(gxh_message* inMsg,gxh_message* message, char status);
   
};



extern "C" app_module* get_module(gxh_scope* appHomeHandle);

#endif	/* APP_MODULE_TIMER_H */


