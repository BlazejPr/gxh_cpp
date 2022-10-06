/* 
 * File:   app_driver_netronix.h
 * Author: Blazej Kita
 *
 * Created on 25 styczen 2020, 14:02
 */

#include "app_driver.h"

#ifndef APP_DRIVER_NETRONIX_H
#define	APP_DRIVER_NETRONIX_H


#define APP_DRIVER_INTERRUPT 610
#define APP_DRIVER_READ_ACTIVE 611
#define APP_DRIVER_READ_NOTACTIVE 612
#define APP_DRIVER_READ_CARD 100
 


 

class app_driver_netronix : public app_driver{
public:
    app_driver_netronix(gxh_scope* appHomeHandle, app_port* appPortHandle,int ih_driver_id,const char* name, const char* address, int delay_before_read_ms);
    virtual ~app_driver_netronix();
 
    void execute(gxh_message* command);
    bool resume();
    
   
   
  private:
    
     ih_driver driver;
     ih_sl_driver ihSlDriver;
     
     int prepare(int size, char* in, char* out);
     void convertToHex(char* in, char* out, int sizeIn, int* sizeOut);
   
      
     void updateAddress(gxh_message* command);
     void updateSpeed(gxh_message* command);
     void updateDataBit(gxh_message* command);
     void updateStopBit(gxh_message* command);
     void updateParity(gxh_message* command);
  
        
};


extern "C" app_driver* get_module(gxh_scope* appHomeHandle, app_port* appPortHandle,int ih_driver_id, const char*, const char*, int delay_before_read_ms);


#endif	/* APP_DRIVER_NETRONIX_H */



