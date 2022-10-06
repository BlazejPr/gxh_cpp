
/* 
 * File:   app_driver_satel.cpp
 * Author: Blazej Kita
 * 
 * Created on 06 czerwiec 2018, 13:05
 */

#include "app_driver_satel.h"


app_driver* get_module(gxh_scope* appHomeHandle, app_port* appPortHandle,int ih_driver_id, const char* name, const char* address, int delay_before_read_ms)
{         
    return new app_driver_satel(appHomeHandle, appPortHandle, ih_driver_id, name,address,delay_before_read_ms);
}
 
//-------------------------------------------------------------------------------


app_driver_satel::app_driver_satel(gxh_scope* appHomeHandle, app_port* appPortHandle,int ih_driver_id, const char* name, const char* address, int delay_before_read_ms) : app_driver(appHomeHandle, appPortHandle,ih_driver_id, name,address, delay_before_read_ms)
{
     char n[50];
     this->getName((char*)n);
     
     std::string msg = std::string(n);
     msg.append(" has been loaded..");
        
     this->getAppHandle()->show_log_driver( "app_driver_satel","app_driver_satel",msg.c_str(),0 );
}

//-------------------------------------------------------------------------------

app_driver_satel::~app_driver_satel() {  
    
     char n[50];
     this->getName((char*)n);
     
     std::string msg = std::string(n);
    msg.append(" has been deleted");      
    
    this->getAppHandle()->show_log_driver("app_driver_satel","app_driver_satel",msg.c_str(),0);   
}


//-------------------------------------------------------------------------------

void app_driver_satel::execute(gxh_message* command)
{
    gxh_message message;
    memcpy(&message, command, sizeof(gxh_message) );
    
    
   
     
    this->getAppHandle()->show_log_driver("app_driver_satel","execute","Execute........",3);
}

//-------------------------------------------------------------------------------

unsigned short obliczCRC(const unsigned char* pCmd, unsigned int length)
//oblicza CRC dla transmisji bez 2 bsjtow na pocz i 2 na koncu.
 {
       unsigned short crc = 0x147A, i;

        for (i = 2; i < length+2; ++i) //liczby od cmd
        {
                crc = (crc << 1) | (crc >> 15);
                crc = crc ^ 0xFFFF;
                crc = crc + (crc >> 8) + pCmd[i];
        }

        return crc;
}


bool app_driver_satel::resume()
{
 //   std::cout<<"\nSat.";
    
      sleep(5);
      
      
    
      char buffer[7];
      buffer[0] = 0xFE;
      buffer[1] = 0xFE;
      
      buffer[2] = 0x03; //cmd
      
      buffer[3] = 0; //crc
      buffer[4] = 0; //crc
      
      unsigned short crc=obliczCRC(buffer,1);
      buffer[3] = (crc >> 8) & 0xff; //crc1
      buffer[4] = crc & 0xff;   //crc2
      
      buffer[5] = 0xFE;
      buffer[6] = 0x0D;
    
    //  std::cout<<"\nSatel write: ";
    
      this->getPortHandle()->writeData(buffer,7); 
    
      usleep (1000* 500);  //500 mili sekund...    
    
       char rBuffer[15]; 
       memset(rBuffer, 0, 15);
    
    //   std::cout<<"\nSatel read: ";
       int r = this->getPortHandle() ->readData(rBuffer, 15);   
    
     
       if(r>2)
       {
           std::cout<<"\n ------------------------ Statel coś odczytał.. "<<rBuffer;
       }
       
    
       
    
    return false;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
 
 
//------------------------------------------------------------------------------


//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
