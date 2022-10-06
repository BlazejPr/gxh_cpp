/* 
 * File:   app_module_camera.h
 * Author: Blazej Kita
 *
 * Created on 22 maj 2018, 14:31
 */

#include "app_module.h"

#ifndef APP_MODULE_CAMERA_H
#define	APP_MODULE_CAMERA_H


class app_module_camera : public app_module{
public:
              app_module_camera(gxh_scope* appHomeHandle); 
              virtual ~app_module_camera();       
   
    
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
 
           ih_camera camers[20];
           int count_camers;
    
};



extern "C" app_module* get_module(gxh_scope* appHomeHandle);

#endif	/* APP_MODULE_CAMERA_H */


