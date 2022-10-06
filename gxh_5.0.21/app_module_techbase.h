/* 
 * File:   app_module_techbase.h
 * Author: Blazej Kita
 *
 * Created on 14 wrzesień 2016, 10:40
 */

#include "app_module.h"

#ifndef APP_MODULE_TECHBASE_H
#define	APP_MODULE_TECHBASE_H

class app_module_techbase : public app_module{
public:
             app_module_techbase(gxh_scope* appHomeHandle ); 
             virtual ~app_module_techbase();       
   
               /**
              * Execute interrupt from GXHSystem
              * @param inMsg
              * @param outMsg
              * @return 
              */
             bool execute(gxh_message* inMsg, gxh_message* outMsg);
             
             /**
              * Method is call when GXHSystem is going to delete object from memory. Method call from clientThread
              */
             void forceCancel();
             
             /**
              * Synchronize, method call from serviceThread
              */
             void forceSynchronize();
             void onClick(int ih_driver_port_id, int listener_status);
    
             void changePortListener(int ih_driver_port_id, int listener_status);
 
};


extern "C" app_module* get_module(gxh_scope* appHomeHandle);

#endif	/* APP_MODULE_TECHBASE_H */


