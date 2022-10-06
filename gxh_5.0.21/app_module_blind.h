/* 
 * File:   app_module_blind.h
 * Author: Blazej Kita
 *
 * Created on 22 maj 2018, 14:31
 */

#include "app_module.h"

#ifndef APP_MODULE_BLIND_H
#define	APP_MODULE_BLIND_H

#define MAX_BLIND_BUFFER 100

class app_module_blind : public app_module{
public:
         app_module_blind(gxh_scope* appHomeHandle); 
         virtual ~app_module_blind();      
   
         bool execute(gxh_message* inMsg, gxh_message* outMsg);   
 
         void forceCancel();
         void forceSynchronize();     
         void onClick(int ih_driver_port_id, int listener_status);
    
 private:    
     
         ih_blind blind[MAX_BLIND_BUFFER];
         int ih_blind_count;
         
         
 
         bool goUp(gxh_message* inMsg,gxh_message* message);      
         bool goDown(gxh_message* inMsg,gxh_message* message);      
         bool setStatus(gxh_message* inMsg,gxh_message* message,int procent,  std::string last_operation);    
   
    
    /**
     * 
     * @param message_ret
     * @param type 1/ dom 2 sector, 3 strefa
     * @param command 1, up, 2 down
     * @return 
     */
    bool setBlindOn(gxh_message* inMsg,gxh_message* message_ret, int type, int command);
  
};



extern "C" app_module* get_module(gxh_scope* appHomeHandle);

#endif	/* APP_MODULE_BLIND_H */


