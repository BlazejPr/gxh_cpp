/* 
 * File:   app_port_serial.h
 * Author: Blazej Kita
 *
 * Created on 8 wrzesień 2016, 10:53
 */

#define TIOCSRS485 0x542F
#define TIOCGRS485 0x542E

 

#include "app_port.h"
#include "app_driver.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <termios.h>
#include <fcntl.h>

#include <linux/serial.h>
#include <sys/ioctl.h>

 





#ifndef APP_PORT_SERIAL_H
#define	APP_PORT_SERIAL_H



typedef  app_driver* (*cosine_func1_ptr)(gxh_scope*, app_port*,int,const char*, const char*, int delay_before_read_ms ); 
    
    struct ih_driver_handle
    {
        int id;
        int const_driver_symbol;
        char module[50];
        
        void *handle;
        cosine_func1_ptr getModule;
        char *error;
        
        bool loaded;
                
    };     



class app_port_serial  : public app_port{
public:
    app_port_serial(gxh_scope* appHomeHandle,const char* nameOnComputer,const char* pathToPort,const char* portType, int ih_computer_port_id,const char* parity,const char* stop_bit, int speed,const char* data);  
    virtual ~app_port_serial();
    
    
    void startLoop();  
    void stopLoop(); 
    void checkPortDebug();
    
    
    void* runThread(app_port* appPort); 
      
   
    
    //app_zc_queue* commandQueue;     
    
    
    ih_driver drivers[50];
    int countDrivers;

    ih_driver_handle* driver_handles;
    int countDriverHandles;
    
    struct driver_item
    {
      ih_driver* drivers;
      app_driver* driver_moudles;
      bool loaded;
      int gxhHandle;
    };
    
    driver_item* driver_list;
    
    
    
    
    
    
    int readData(char* buffer, int size);
    int writeData(char* buffer, int size);
    void flush();
    bool changePortParameter(char parity, char stop_bit, int speed, char data);
    bool changePortParameter(const char* parity, const char* stop_bit, int speed, const char* data);
    bool setDefaultPortParameter( );
    
private:
      
    bool checkParameter(const char* parity, const char* stop_bit, int speed, const char* data);
    
    
    bool goRunning;
    
    const char* def_parity;
    const char* def_stop_bit;
    int def_speed;
    const char* def_data;
    
    int fd;
    serial_rs485 rs485conf;
    
    void loadDrivers();
    void deleteDrivers();
    
    
    void getDrivers(int ih_computer_port_id,int* count, int type, ih_driver* rows );
 
    
};

#endif	/* APP_PORT_SERIAL_H */




