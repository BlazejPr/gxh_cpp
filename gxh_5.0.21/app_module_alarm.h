/* 
 * File:   app_module_alarm.h
 * Author: Blazej Kita
 *
 * Created on 25 maj 2020, 14:31
 */

#include "app_module.h"

#ifndef APP_MODULE_DOOR_H
#define	APP_MODULE_DOOR_H

#define MAX_DOOR_BUFFER  20

 

class app_module_alarm : public app_module{
public:
              app_module_alarm(gxh_scope* appHomeHandle); 
              virtual ~app_module_alarm();       
   
    
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
        ih_alarm_zone alerm_zones[30];
        int alarm_zones_count;
        
        ih_alarm_device alerm_devices[50];
        int alarm_devices_count;
        
     void wyzerujCzasy(int zone_id);   
     
     bool goActiveAlarmZone(gxh_message* inMsg,gxh_message* message);
     bool goActiveAlarmZone2(gxh_message* inMsg,gxh_message* message);
     bool goDezactiveAlarmZone(gxh_message* inMsg,gxh_message* message);
     bool goActiveAlarmZoneAll(gxh_message* inMsg,gxh_message* message);
     bool goDezactiveAlarmZoneAll(gxh_message* inMsg,gxh_message* message);
     bool onSpeaker(gxh_message* inMsg,gxh_message* message);
     bool offSpeaker(gxh_message* inMsg,gxh_message* message);
     bool noticeSpeaker(int num);
     bool moveSensor(gxh_message* inMsg,gxh_message* message);
     bool smokeSensor(gxh_message* inMsg,gxh_message* message);
     bool gasSensor(gxh_message* inMsg,gxh_message* message);
   
    
 
   
};



extern "C" app_module* get_module(gxh_scope* appHomeHandle);

#endif	/* APP_MODULE_DOOR_H */


