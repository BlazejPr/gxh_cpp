/* 
 * File:   app_module_light.h
 * Author: Blazej Kita
 *
 * Created on 14 grudzień 2018, 14:31
 */

#include "app_module.h"

#ifndef APP_MODULE_LIGHT_H
#define	APP_MODULE_LIGHT_H

#define MAX_LIGHT_BUFFER 500

class app_module_light : public app_module{
       public:
              app_module_light(gxh_scope* appHomeHandle); 
              virtual ~app_module_light();       
   
    
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
                
              bool forceBreak;
            
              ih_lighting light[MAX_LIGHT_BUFFER];
              int light_count;
              
              bool changeLightStatus(gxh_message* msgIn, gxh_message* message_ret );
              bool changeLightMultiStatus(gxh_message* msgIn, gxh_message* message_ret );
              bool changeLightOff(gxh_message* msgIn, gxh_message* message_ret);
              bool changeLightOn(gxh_message* msgIn, gxh_message* message_ret);
              
               /**
     * 
     * @param message_ret
     * @param type 1 home, 2 sector, 3 zone
     * @param command 1 light, 0 off
     * @return 
     */
              
              bool setLightOn(gxh_message* msgIn, gxh_message* message_ret, int type, int command);
    
    /**
     * 
     * @param message_ret
     * @param type 1 home, 2 sector, 3 zone
     * @param command 1 light, 0 off
     * @return 
     */
    bool setLightOn(gxh_message* message_ret, int type, int command);
    
};



extern "C" app_module* get_module(gxh_scope* appHomeHandle);

#endif	/* APP_MODULE_LIGHT_H */


