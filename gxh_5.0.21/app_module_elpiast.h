/* 
 * File:   app_module_elpiast.h
 * Author: Blazej Kita
 *
 * Created on 25 styczeń 2019, 14:02
 */

#include "app_module.h"

#ifndef APP_MODULE_TECHBASE_H
#define	APP_MODULE_TECHBASE_H


/*
struct elpiastData
{
    bool retSuccess;
};
 */

class app_module_elpiast : public app_module{
public:
             app_module_elpiast(gxh_scope* appHomeHandle ); 
             virtual ~app_module_elpiast();       
   
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


