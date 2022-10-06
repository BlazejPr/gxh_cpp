/* 
 * File:   app_module_satel.h
 * Author: Blazej Kita
 *
 * Created on 30 lipiec 2018, 14:31
 */

#include "app_module.h"

#ifndef APP_MODULE_SATEL_H
#define	APP_MODULE_SATEL_H

 

class app_module_satel : public app_module{
public:
              app_module_satel(gxh_scope* appHomeHandle); 
              virtual ~app_module_satel();       
   
    
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
       
   
};



extern "C" app_module* get_module(gxh_scope* appHomeHandle);

#endif	/* APP_MODULE_SATEL_H */


