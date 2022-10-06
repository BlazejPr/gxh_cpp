/* 
 * File:   app_port_ethernet.cpp
 * Author: Blazej Kita
 * 
 * Created on 8 wrzesień 2016, 11:46
 */

#include "app_port_ethernet.h"

 

void* runThreadPortEthernet(void* portHandle)
{
    app_port_ethernet* core = (app_port_ethernet*)portHandle;
    core->runThread();     
    return NULL;
}

struct eth_client
{
    void* portHandle;
    sockaddr_in* clStruct;
    int socket;
};


struct eth_client_th
{
    pthread_t clientThread;       
    void* portHandle;
    sockaddr_in* clStruct;
    int socket;
};

void* runThreadPortEthernetForClient(void* portHandle)
{
    eth_client* eth = (eth_client* )(portHandle);
    
    app_port_ethernet* core = (app_port_ethernet*)eth->portHandle;
    core->runThreadClient(eth->clStruct, eth->socket,true); 
    return NULL;
}


//funkcja domyka otwarte polaczenia z klientem...
void* checkClose(void* portHandle)
{
    eth_client_th* eth = (eth_client_th* )(portHandle);    
  
    sleep(2); //5 sek na zamkniecie..  
    pthread_cancel(eth->clientThread);    
    close(eth->socket); //zamknij port     
    
    return NULL;
}
 

app_port_ethernet::app_port_ethernet(gxh_scope* appHomeHandle,  int ih_computer_port_id ) : app_port(appHomeHandle, ih_computer_port_id) {
        
    this->getAppHandle()->show_log_port("app_port_ethernet","app_port_ethernet", "start ethernet",0);
    
    this->count_client = 0;
    this->max_client_thread = MAX_CLIENT_TCP;
    
   
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);		/* inicjalizacja atrybutów mutexa */    
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
     
    pthread_mutex_init(&this->gxh_client__muteks, &attr);
    
}
 

app_port_ethernet::~app_port_ethernet() {
    this->getAppHandle()->show_log_port("app_port_ethernet","app_port_ethernet", "stop ethernet",0);   
    
   pthread_mutex_destroy(&this->gxh_client__muteks);     
}

void* app_port_ethernet::runThread()
{
    //sync attack
    //https://www.symantec.com/connect/articles/hardening-tcpip-stack-syn-attacks
    // netstat -anlp | grep :7777
    
    this->getAppHandle()->show_log_port("app_port_ethernet","runThread","start thread listnener",0);
    
    sockaddr_in client;
    int new_socket; 
    int c;      
     
    
    while(this->goRunning)
    {
        this->getAppHandle()->show_log_port("app_port_thread","runThread","Waiting for new client",7);
        
        c = sizeof(struct sockaddr_in);
        new_socket = accept(  this->socket_desc , (struct sockaddr *)&client, (socklen_t*)&c);
        if(new_socket < 0) 
        {
            sleep(1);
            continue;                                 
        }
        
        //--------------
       
        
        char readyGXH[100]; 
        memset(readyGXH,0,100);        
        
        recv(new_socket, readyGXH, 15, 0);  //GXH: MK-GXH-001\r\n  10  +2 pierszych znaków to hash + \r\n.. reszty nie czytamy..
                
        
        if ( !(readyGXH[0] == 'P' && readyGXH[1] == 'O' && readyGXH[2] == 'S' && readyGXH[3] == 'T' && readyGXH[4] == ' ' && readyGXH[5] == '/' && readyGXH[6] == 'w' && readyGXH[7] == 's' && readyGXH[8] == 'd' &&readyGXH[9] == 'l' ) )
        {             
            char* ip =  inet_ntoa( client.sin_addr);    
            std::string newClient = "New client tcp/ip witch bad request ";             
            newClient.append(ip);
            newClient.append(":");
            newClient.append( gxh_IntToString( ntohs(client.sin_port) ) );
            newClient.append( "\nData: "  );
            newClient.append( readyGXH  );
            newClient.append( "\n"  );
            
            this->getAppHandle()->show_error("app_port_thread","runThread", newClient.c_str() ,4);             
            sleep(1);
            close(new_socket);
            continue;
        }
        //--------------
        
         pthread_mutex_lock(&this->gxh_client__muteks);          
        
         if( this->count_client > this->max_client_thread)
         {
            this->getAppHandle()->show_log_port("app_port_ethernet","runThread", "Too many clients.......",2 );
            // shutdown( new_socket, SHUT_RDWR);           
            sleep(1);
            close(new_socket);
            pthread_mutex_unlock(&this->gxh_client__muteks);  
            continue;
         }
        
         pthread_mutex_unlock(&this->gxh_client__muteks);   
         
         
         //new thread to read
         pthread_t clientThread;       
         //pthread_t clientThreadTmp;       
         eth_client ethClient;
         ethClient.portHandle = (void*)this;
         ethClient.clStruct   = &client;
         ethClient.socket = new_socket;
         
         
         //eth_client_th ethClientTh;
         //ethClientTh.portHandle = (void*)this;
         //ethClientTh.clStruct   = &client;
         //ethClientTh.socket = new_socket;
         //ethClientTh.clientThread = clientThread;
         
         
                 
         pthread_attr_t attr;
         pthread_attr_init(&attr);
         pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); //po zakonczeniu zwolni sie od razu pamiec...        
            
         int rc = pthread_create(&clientThread, &attr,  &runThreadPortEthernetForClient, (void*)&ethClient );
         if (rc)
         {
            this->getAppHandle()->show_error( "app_port_ethernet","app_port_ethernet","Unable to create core thread!" ,0);                                             
         } else
         {
                pthread_detach( runThreadPortEthernetForClient );//2022-03
         }
           
         //utworz wątek domykający..
         //pthread_create(&clientThreadTmp, &attr,  &checkClose, (void*)&ethClientTh ); //dodatkowy watek który zamknie port klient w przypadku braku odpowiedzi o watku głownego klienta..
         
         pthread_attr_destroy(&attr);
          
       
    }
    
    
    pthread_exit((void*)1);     
}

//------------------------------------------------------------------------------

void* app_port_ethernet::runThreadClient(sockaddr_in* clientPtr, int socket,  bool run_as_new_thread)
{
    
    sockaddr_in client;    
    memcpy(&client, clientPtr, sizeof(sockaddr_in));
    
    char* ip =  inet_ntoa( client.sin_addr);    
    std::string newClient = "New client tcp/ip ";   
    newClient.append(ip);
    newClient.append(":");
    newClient.append( gxh_IntToString( ntohs(client.sin_port) ) );
    newClient.append(", all clients: ");
    newClient.append( gxh_IntToString( this->count_client+1 )  );
             
    this->getAppHandle()->show_log_port("app_port_ethernet","runThread", newClient.c_str(),1 );
    
 
    pthread_mutex_lock(&this->gxh_client__muteks);  
    
    if( this->count_client > this->max_client_thread)
    {
          this->getAppHandle()->show_log_port("app_port_ethernet","runThread", "Too many clients",2 );
          //shutdown( socket, SHUT_RDWR);           
          this->getAppHandle()->show_log_port("app_port_ethernet","runThread", "Client disconnected (0x0001)",1 );
          sleep(3);
          close(socket);
          if(run_as_new_thread)  pthread_exit(NULL);
          return NULL;
    }
    
    pthread_mutex_unlock(&this->gxh_client__muteks); 
    
    
    
      int gxhHandle         = this->getAppHandle()->createGXHHandle("clientTCP", socket, 2);
      
     
      
      if(gxhHandle < 0)
      {                           
          //shutdown( socket, SHUT_RDWR);                     
          this->getAppHandle()->show_log_port("app_port_ethernet","runThread", "Client disconnected (0x0002)",1 );
          sleep(1);
          close(socket);
          if(run_as_new_thread)  pthread_exit(NULL);
          return NULL;
      }
      
      
      pthread_mutex_lock(&this->gxh_client__muteks); 
      this->count_client++;  
      pthread_mutex_unlock(&this->gxh_client__muteks); 
      
      
      
      int nowRead           = 0;
      std::string bufferAll = "";
      int dataSize          = 0;
      char buffer[1024];          
            
      usleep(50);
      
      struct timeval tv;
      tv.tv_sec = 0;  //0
      tv.tv_usec = 10; /////tututu 10
      setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv,sizeof(struct timeval));
    
    
      for(int mP=0; mP<100; mP++)
      {
        while(  (nowRead = recv(socket, buffer, sizeof(buffer), 0) )>0 )
        {             
                bufferAll.append(buffer, nowRead);      
                dataSize += nowRead;
        }
    
        if(dataSize >0) break;
        usleep(500); 
     }
    
      
    
       
        if(dataSize > 0 )
        {                     
          this->getAppHandle()->show_log_port("app_port_ethernet","runThreadClient", "received data from client .........", 2);
          this->getAppHandle()->show_log_port("app_port_ethernet","runThreadClient", bufferAll.c_str(), 4);
          
          gxh_message msg;
          msg.func_number          = 2; //wsdl...
          msg.func_internal_number = 0;
          msg.gxh_handle           = gxhHandle;
          msg.check_timeout        = 10;
          msg.execute_timeout      = 30 * 1000;
          msg.localhost            = false;
          
          if(ip[0] == '1' && ip[1]=='2' && ip[2] == '7')  
          {
              msg.localhost = true;
              this->getAppHandle()->show_log_port("app_port_ethernet","runThreadClient", "Localhost...", 4);  
          }
          
          unsigned int dsize = dataSize;
          if(dsize > sizeof(msg.dataStruct) ) dsize = sizeof(msg.dataStruct);
          msg.sizeDataStruct = dsize;
          memcpy(msg.dataStruct, bufferAll.c_str(), dsize );
          
          msg.sizeDataStruct = dsize;
          
            
          
          this->getAppHandle()->sendGXHMessage(&msg);
          
          gxh_message resultMsg;
          
          
          
          if(this->getAppHandle()->receiveGXHMessage(gxhHandle, &resultMsg, 1000 * 30)) //30 sek na odpowiedz..           
          {                                                          
                this->getAppHandle()->show_log_port("app_port_ethernet","runThreadClient", "Got data from core.......", 2);
                
              
                std::string response =  std::string(  resultMsg.dataStruct );    
                
                if(resultMsg.system_code == GXH_MESSAGE_ERROR)
                {
                 
                     this->getAppHandle()->show_error("app_port_ethernet","runThread", "Core error .....",2 );
                     this->getAppHandle()->show_error("app_port_ethernet","runThread",  (char*)resultMsg.dataStruct ,2 );
                
              
                     std::string response = "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Client</faultcode><faultstring>";       
                                 response.append( resultMsg.dataStruct  );       
                                 response.append( "</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>\n" );
              
                     const char* responseWSDL = "HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-Type:text/xml; charset=UTF-8\r\nCache-Control: no-cache\r\n";
                     write(socket , responseWSDL , strlen( responseWSDL));  
                    
                     responseWSDL = "Content-Length:";
                     write(socket , responseWSDL , strlen( responseWSDL));  
                      
              
                     std::string msg = gxh_IntToString(response.length() );
                     responseWSDL = msg.c_str();
                     write(socket , responseWSDL , strlen( responseWSDL));  
                
                
                     responseWSDL = "\r\n\r\n";
                     write(socket , responseWSDL , strlen( responseWSDL));  
                
                
                     write(socket , response.c_str() , strlen(response.c_str()));   
                
                this->getAppHandle()->show_log_port("app_port_ethernet","runThread", response.c_str() ,4 );
                    
                }else
                {
                 
                  const char* responseWSDL = "HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-Type:text/xml; charset=UTF-8\r\nCache-Control: no-cache\r\n";
                  write(socket , responseWSDL , strlen( responseWSDL));  
                  
                
                  responseWSDL = "Content-Length:";                             
                  write(socket , responseWSDL , strlen( responseWSDL));                       
               
                  std::string msg = gxh_IntToString(  strlen(response.c_str())  ); // plus null
                  responseWSDL = msg.c_str();     
                  write(socket , responseWSDL , strlen( responseWSDL));  
                 
                
                  responseWSDL = "\r\n\r\n";
                  write(socket , responseWSDL , strlen( responseWSDL));  
                
                
                  write(socket , response.c_str() , strlen(response.c_str()));   
                  usleep(1000 * 500);//wait before disconnect..
                  
                    
                
                  
                  this->getAppHandle()->show_log_port("app_port_ethernet","runThread", response.c_str() ,4 );
                  
                  
                  
                  
                }
                                              
          }else
          {    
                this->getAppHandle()->show_error("app_port_ethernet","runThread", "TimeOut of core .....",2 );
                
              
                std::string response = "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Client</faultcode><faultstring>GXH TimeOut</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>\n";       
              
                const char* responseWSDL = "HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-Type:text/xml; charset=UTF-8\r\nCache-Control: no-cache\r\n";
                write(socket , responseWSDL , strlen( responseWSDL));  
                    
                responseWSDL = "Content-Length:";
                write(socket , responseWSDL , strlen( responseWSDL));  
                      
              
                std::string msg = gxh_IntToString(response.length() );
                responseWSDL = msg.c_str();
                write(socket , responseWSDL , strlen( responseWSDL));  
                
                
                responseWSDL = "\r\n\r\n";
                write(socket , responseWSDL , strlen( responseWSDL));  
                
                
                write(socket , response.c_str() , strlen(response.c_str()));   
                
                this->getAppHandle()->show_log_port("app_port_ethernet","runThread", response.c_str() ,4 );
          }
          
        }  else
        {
                this->getAppHandle()->show_log_port("app_port_ethernet","runThread", "Nothing to read......",0 );
        }
    
    this->getAppHandle()->show_log_port("app_port_ethernet","runThread", "Disconnect client .....",2 );      
    
    bufferAll = "";
    
    
    this->getAppHandle()->destroyGXHHandle(gxhHandle);   
    this->getAppHandle()->show_log_port("app_port_ethernet","runThread", "Client disconnected (0x0007)",1 );
    
    pthread_mutex_lock(&this->gxh_client__muteks); 
    this->count_client--;  
    pthread_mutex_unlock(&this->gxh_client__muteks); 
    //shutdown( socket, SHUT_RDWR);      
    close(socket);
                                     
    
    this->getAppHandle()->show_log_port("app_port_ethernet","runThread", "Disconnect client ok.....",2 );   
    
    if(run_as_new_thread)  pthread_exit(NULL);  
    return NULL;
}

//------------------------------------------------------------------------------


void app_port_ethernet::startLoop()
{
    this->goRunning = true;
    this->workCorrect = false;
    
    
    
    this->socket_desc = socket(AF_INET , SOCK_STREAM , 0);
     
     if (this->socket_desc == -1)
     {
            this->getAppHandle()->show_error( "app_port_ethernet","app_port_ethernet","Could not create socket" ,0);
            return;
     }
     
     //dont block port... close imediatelly      
     int yes=1;     
     if (setsockopt(  this->socket_desc, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) 
     {
          this->getAppHandle()->show_error( "app_port_ethernet","app_port_ethernet","Could not save sockopt",0 );            
          return; 
     }
       
 
    this->server.sin_family = AF_INET;
    this->server.sin_addr.s_addr = INADDR_ANY;
    this->server.sin_port = htons( CONSOLE_PORT ); 
    
    
    if( bind(this->socket_desc,(struct sockaddr *)&this->server , sizeof(this->server)) < 0)
         {
            this->getAppHandle()->show_error( "app_port_ethernet","app_port_ethernet","Bind failed",0 );                        
            close(this->socket_desc);
            return;
         }
    
    
    listen(this->socket_desc ,  MAX_CLIENT_TCP );   
    
    
    
     //new thread to read
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); //po zakonczeniu zwolni sie od razu pamiec...
       
     
    int rc = pthread_create(&this->mainThread, &attr,  &runThreadPortEthernet, (void*)this );
    if (rc)
    {
         this->getAppHandle()->show_error( "app_port_ethernet","app_port_ethernet","Unable to create core thread!" ,0);       
         pthread_attr_destroy(&attr);
         close(this->socket_desc);
         return;
    }else
    {
         pthread_setname_np(this->mainThread,"ETH");
    }
         
    pthread_attr_destroy(&attr);
    
    this->workCorrect = true;
}



void app_port_ethernet::stopLoop()
{
    this->goRunning = false;
    pthread_cancel( this->mainThread );   
    
    close(this->socket_desc);
}
 
void app_port_ethernet::checkPortDebug()
{
  //pping3  
  //while {1} { hping send "ip(saddr=1.2.3.4,daddr=192.168.0.150)+tcp(sport=4231,dport=7777,flags=s)" }
    
    std::string result = this->getAppHandle()->exec( "netstat -anlp | grep :7777 | grep SYN_RECV | wc -l");
    
    if(result.length() > 10)
    {
         int all = gxh_StringToInt(result);
         if(all > 0)  
         {
             this->getAppHandle()->show_error("app_port_ethernet","checkPortDebug","syn_recv attack ........ ",2);
             
             this->stopLoop();
             sleep(5);
             this->startLoop();
         }
    }    
    
    
    result = this->getAppHandle()->exec( "netstat -anlp | grep :7777  | wc -l");
    
    if(result.length() > 20)
    {
         int all = gxh_StringToInt(result);
         if(all > 10)  
         {
             this->getAppHandle()->show_error("app_port_ethernet","checkPortDebug","syn_recv attack? too client connection... ........ ",2);
             
             this->stopLoop();
             sleep(5);
             this->startLoop();
         }
    }
    
   
}

