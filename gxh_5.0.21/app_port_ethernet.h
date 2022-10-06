
/* 
 * File:   app_port_ethernet.h
 * Author: Blazej Kita
 *
 * Created on 8 wrzesień 2016, 11:46
 */

#include "app_port.h"

#ifndef APP_PORT_ETHERNET_H
#define	APP_PORT_ETHERNET_H

class app_port_ethernet : public app_port{
public:
    app_port_ethernet(gxh_scope* appHomeHandle, int ih_computer_port_id );   
    virtual ~app_port_ethernet();
    
    void* runThread();
    void* runThreadClient(sockaddr_in* client, int socket, bool run_as_new_thread);
    
    void startLoop();
    void stopLoop();
    void checkPortDebug();
    
private:
    int max_client_thread;
    int count_client;
    bool goRunning;
       
    int socket_desc;    
    struct sockaddr_in server;  
    
    pthread_mutex_t gxh_client__muteks;
        
    
  
};

#endif	/* APP_PORT_ETHERNET_H */

