/* 
 * File:   app_module_sensor.h
 * Author: Blazej Kita
 *
 * Created on 24 maj 2018, 14:31
 */

#include "app_module.h"

#ifndef APP_MODULE_SENSOR_H
#define	APP_MODULE_SENSOR_H

#define MAX_SENSOR_BUFFER 100

class app_module_sensor : public app_module{
public:
              app_module_sensor(gxh_scope* appHomeHandle); 
              virtual ~app_module_sensor();       
   
    
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
             ih_sensor sensor[MAX_SENSOR_BUFFER];
             unsigned int sensor_count;
     
             bool setSensorOnOf(gxh_message* inMsg, gxh_message* message, int id, int status);       
    
};


extern "C" app_module* get_module(gxh_scope* appHomeHandle);

#endif	/* APP_MODULE_SENSOR_H */


