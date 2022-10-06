/* 
 * File:   app_module_door.h
 * Author: Blazej Kita
 *
 * Created on 09 styczeń 2020, 14:31
 */

#include "app_module.h"

#ifndef APP_MODULE_DOOR_H
#define	APP_MODULE_DOOR_H

#define MAX_DOOR_BUFFER  20

 

class app_module_door : public app_module{
public:
              app_module_door(gxh_scope* appHomeHandle); 
              virtual ~app_module_door();       
   
    
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
     int door_count;
     ih_door doors[MAX_DOOR_BUFFER];
     
     bool goOpenDoor(gxh_message* inMsg,gxh_message* message);
     
     bool doorOpened(gxh_message* inMsg,gxh_message* message);
     bool doorClosed(gxh_message* inMsg,gxh_message* message);
   
    
 
   
};



extern "C" app_module* get_module(gxh_scope* appHomeHandle);

#endif	/* APP_MODULE_DOOR_H */


