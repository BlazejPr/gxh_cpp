/* 
 * File:   app_module_outs.h
 * Author: Blazej Kita
 *
 * Created on 30 lipiec 2018, 14:31
 */

#include "app_module.h"

#ifndef APP_MODULE_OUTS_H
#define	APP_MODULE_OUTS_H

#define MAX_OUTS_BUFFER 50

class app_module_outs : public app_module{
public:
              app_module_outs(gxh_scope* appHomeHandle); 
              virtual ~app_module_outs();       
   
    
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
          bool firstSynchronize;   
     
     
          unsigned int count_outs;
          ih_outs outs[MAX_OUTS_BUFFER];
 
          /**
           * 
           * @param inMsg
           * @param outMsg
           * @param option 1-on, 2-off, 3-on/off
           * @return 
           */
          bool setStatus(gxh_message* inMsg, gxh_message* outMsg, int option);
          bool setStatus2(int id, int option);
   
};



extern "C" app_module* get_module(gxh_scope* appHomeHandle);

#endif	/* APP_MODULE_OUTS_H */


