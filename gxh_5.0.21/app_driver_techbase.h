/* 
 * File:   app_driver_techbase.h
 * Author: Blazej Kita
 *
 * Created on 14 wrzesień 2016, 15:05
 */

#include "app_driver.h"

#ifndef APP_DRIVER_TECHBASE_H
#define	APP_DRIVER_TECHBASE_H



class app_driver_techbase : public app_driver{
public:
    app_driver_techbase(gxh_scope* appHomeHandle, app_port* appPortHandle,int ih_driver_id,const char* name, const char* address, int delay_before_read_ms);
    virtual ~app_driver_techbase();
 
    void execute(gxh_message* command);
    bool resume();
    
   
   
  private:
        int licznik;
        bool stan;
        
        bool statusCache;
        char statusBuffer[12];
      
        ih_driver_port ports[25];
        int num_ports;
        
        
        struct time_status
        {
            unsigned long time;
            bool status;
            char option;
        };
        
        
        void scrollHistory(unsigned long time, bool status, char option, int index);
        
        struct history_port
        {
          time_status info[6]; //pamiętaj wstecz 3 kliknięcia 3ON 3 OFF          
        };
        
        history_port* history_ports;
        
        ih_driver driver;
        ih_sl_driver ihSlDriver;
      
      
        unsigned int di_last_state ;      //cztero bajtowa zmienna  Di0 to najmłodszy bit
        unsigned int di_actual_state;    //cztero bajtowa zmienna 
        
        
        
        
        
        void updateAddress(gxh_message* command);
        
        void sendMsgToCore(int func_internal_number, int ih_driver_port_id);
        
        
       
        int HexToTwoByte(char* hex);
        int HexCharToInt(char hex);
        int HexPortToInt(int numPort);
        char* IntPortToChar(int portStatus);
        char IntToHexChar(int num);
        
        
        
        bool checkPortStatus7051D();
        bool checkPortStatus7055D();
        
        void setPortStatus7061D(gxh_message* command);
        void setPortStatus7055D(gxh_message* command);
        bool sendPortStatus(gxh_message* command, int portStatus);
        bool sendPortStatus7055(gxh_message* command, int portStatus);
        int getPortStatus( );
        //void checkPortStatusNoCrc();
        
        void changePortListener(int numPort, bool status);
        
        //---
        
};


extern "C" app_driver* get_module(gxh_scope* appHomeHandle, app_port* appPortHandle,int ih_driver_id, const char*, const char*, int delay_before_read_ms);


#endif	/* APP_DRIVER_TECHBASE_H */


