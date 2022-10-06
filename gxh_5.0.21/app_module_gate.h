/* 
 * File:   app_module_gate.h
 * Author: Blazej Kita
 *
 * Created on 22 maj 2018, 14:31
 */

#include "app_module.h"

#ifndef APP_MODULE_GATE_H
#define	APP_MODULE_GATE_H

#define MAX_GATE_BUFFER 10

class app_module_gate : public app_module{
public:
              app_module_gate(gxh_scope* appHomeHandle); 
              virtual ~app_module_gate();       
   
    
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
     int gate_count;
     ih_gate gate[MAX_GATE_BUFFER];
     
     bool goOpenGate(gxh_message* inMsg,gxh_message* message);
     bool goCloseGate(gxh_message* inMsg,gxh_message* message);
     bool goClickGate(gxh_message* inMsg,gxh_message* message);
     bool setStatusGate(gxh_message* inMsg,gxh_message* message,int status, std::string last_operation );
  
    
 
   
};



extern "C" app_module* get_module(gxh_scope* appHomeHandle);

#endif	/* APP_MODULE_GATE_H */


