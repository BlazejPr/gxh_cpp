/* 
 * File:   app_driver_satel.h
 * Author: Blazej Kita
 *
 * Created on 06 czerwiec 2018, 13:05
 */

#include "app_driver.h"

#ifndef APP_DRIVER_SATEL_H
#define	APP_DRIVER_SATEL_H

 
class app_driver_satel : public app_driver{
public:
    app_driver_satel(gxh_scope* appHomeHandle, app_port* appPortHandle,int ih_driver_id,const char* name, const char* address, int delay_before_read_ms);
    virtual ~app_driver_satel();
 
    void execute(gxh_message* command);
    bool resume();
    
   
   
  private:
       
 
        
};


extern "C" app_driver* get_module(gxh_scope* appHomeHandle, app_port* appPortHandle,int ih_driver_id, const char*, const char*, int delay_before_read_ms);


#endif	/* APP_DRIVER_SATEL_H */


