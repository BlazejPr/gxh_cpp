/* 
 * File:   app_driver_elpiast.h
 * Author: Blazej Kita
 *
 * Created on 25 styczen 2016, 14:02
 */

#include "app_driver.h"

#ifndef APP_DRIVER_ELPIAST_H
#define	APP_DRIVER_ELPIAST_H


#define MAX_DELAY_MSG 15

/*
struct elpiastData
{
    bool retSuccess;
};
 */


class app_driver_elpiast : public app_driver{
public:
    app_driver_elpiast(gxh_scope* appHomeHandle, app_port* appPortHandle,int ih_driver_id,const char* name, const char* address, int delay_before_read_ms);
    virtual ~app_driver_elpiast();
 
    void execute(gxh_message* command);
    bool resume();
    
   
   
  private:
        int licznik;
        bool stan;
        
        bool statusCache;
        char statusBuffer[12];
      
        ih_driver_port ports[25];
        int num_ports;
        
        
        
        unsigned short LDI1;
        unsigned short LDI2;
        unsigned short LDI3;
        unsigned short LDI4;
        unsigned short LDI5;
        unsigned short LDI6;
        unsigned short LDI7;
        unsigned short LDI8; 

        unsigned short LDI9;         
        unsigned short LDI10;         
        unsigned short LDI11;         
        unsigned short LDI12;         
        unsigned short LDI13;         
        unsigned short LDI14;         
        unsigned short LDI15;         
        unsigned short LDI16; 
        
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
      
       
        
        struct message_dalay
        {
            gxh_message message;
            unsigned long time_execute; // o której wykonać to polecenie... 0 - puste / wykonane..            
        };
        
        message_dalay msgDelay[MAX_DELAY_MSG];
        
        bool addMessageDelay(gxh_message* command, int msDelay, int func_internal_number);        
        bool getMessageDelay(gxh_message* command);
        bool clearMessageDelay(gxh_message* command);
        
        
        void updateAddress(gxh_message* command);
        
        void sendMsgToCore(int func_internal_number, int ih_driver_port_id);
        
        
       
        int HexToTwoByte(char* hex);
        int HexCharToInt(char hex);
        int HexPortToInt(int numPort);
      
        char IntToHexChar(int num);
        
        
        
        bool checkPortStatusDI8DO8();
        bool checkPortStatusDI16();
      
        
        void setPortStatusEl8I8O(gxh_message* command);
       
        bool sendPortStatus(gxh_message* command, int portStatus);
        
        int getPortStatus( );
        //void checkPortStatusNoCrc();
        
        void changePortListener(int numPort, bool status);
        
        //---
        
};


extern "C" app_driver* get_module(gxh_scope* appHomeHandle, app_port* appPortHandle,int ih_driver_id, const char*, const char*, int delay_before_read_ms);


#endif	/* APP_DRIVER_ELPIAST_H */


