/* 
 * File:   app_module_scene.h
 * Author: Blazej Kita
 *
 * Created on 13 czerwiec 2018, 14:31
 */

#include "app_module.h"

#ifndef APP_MODULE_SCENE_H
#define	APP_MODULE_SCENE_H

#define MAX_BUFFER_SCENE 20  //maksymalnie 20 scen..
#define MAX_BUFFER_SCENE_ACTION 200  //maksymalnie 20 scen..

#define MAX_BUFFER_TASK 50  //maksymalnie 20 scen.. 
 

class app_module_scene : public app_module{
public:
              app_module_scene(gxh_scope* appHomeHandle); 
              virtual ~app_module_scene();       
   
    
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
 
           void loadToCache();
     
            ih_scene scenes[MAX_BUFFER_SCENE];
            ih_scene_action scene_actions[MAX_BUFFER_SCENE_ACTION];
            
            
            void runScene(int idScene);
 
            
            ih_task tasks[MAX_BUFFER_TASK];
            
            void runTask();
   
   
};



extern "C" app_module* get_module(gxh_scope* appHomeHandle);

#endif	/* APP_MODULE_SCENE_H */


