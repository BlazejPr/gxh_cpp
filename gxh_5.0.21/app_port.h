/* 
 * File:   app_port.h
 * Author: Blazej Kita
 *
 * Created on 31 sierpień 2016, 10:48
 */


#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include"gxh_scope.h" 

 

#ifndef APP_PORT_H
#define	APP_PORT_H

class app_port {
public:
    app_port(gxh_scope* appHomeHandle, int ih_computer_port_id);    
    app_port(gxh_scope* appHomeHandle, const char* nameOnComputer, const char* pathToPort,const char* portType, int ih_computer_port_id, const char* parity, const char* stop_bit, int speed, const char* data);    
    virtual ~app_port();
   
    virtual void startLoop() = 0;  
    virtual void stopLoop()  = 0;  
    virtual void checkPortDebug() = 0; //sprawdza co jakiś czas poprawność działania portu - petli...
 
    virtual int readData(char* buffer, int size);
    virtual int writeData(char* buffer, int size);
    virtual void flush();
    
    /**
     * Change paramter of poart. If parameter is NULL then param is default
     * @param parity
     * @param stop_bit
     * @param speed
     * @param data
     * @return 
     */
    virtual bool changePortParameter( char parity, char stop_bit, int speed, char data);
    virtual bool setDefaultPortParameter( );      
    
    
    bool isCorrect();
private:
             
    gxh_scope* appHandle;     
    
    
 protected:
 
    bool workCorrect; 
     
    pthread_t mainThread;       
  //  pthread_t mainThread2;       
  //  pthread_mutex_t mainThreadPort;        
    
    
    //data for serial port
    char name_on_computer[50];
    char path[255];
    char typ[20];
    int ih_computer_port_id;
    char parity[2];
    char stop_bit[2];
    int speed;
    char data[2];
     
     
    //data for ethernet...
    int socket_desc;    
    struct sockaddr_in server;   
    
    
   
    
    
    /**
     * The method return handle to main core
     * @return 
     */
    gxh_scope* getAppHandle();   
    int   getIhComputerPortId();
    
             
    
    
   
    
        
};

#endif	/* APP_PORT_H */


