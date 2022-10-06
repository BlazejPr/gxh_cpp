/* 
 * File:   app_module_ethernet.cpp
 * Author: Blazej Kita
 * 
 * Created on 25 lipiec 2018, 14:31
 */

#include "app_module_ethernet.h"


 
app_module* get_module(gxh_scope* appHomeHandle )
{
    return new app_module_ethernet(appHomeHandle); 
};
 
//-------------------------------------------------------------------------------
  
app_module_ethernet::app_module_ethernet(gxh_scope* appHomeHandle) : app_module(appHomeHandle) 
{           
    this->getAppHandle()->show_log_module("app_module_ethernet","app_module_ethernet","start",2 );         
 
}

//------------------------------------------------------------------------------

app_module_ethernet::~app_module_ethernet() 
{     
    this->getAppHandle()->show_log_module("app_module_ethernet","app_module_ethernet","stop",2 );
}


//-------------------------------------------------------+-----------------------

void app_module_ethernet::onClick(int ih_driver_port_id, int listener_status)
{
}

//-------------------------------------------------------+-----------------------

void app_module_ethernet::forceCancel()
{  
      this->getAppHandle()->show_log_module("app_module_ethernet","app_module_ethernet","forceCancel",2);
      
    
      this->forceSynchronize();
 
}

//-------------------------------------------------------+-----------------------

void app_module_ethernet::forceSynchronize()
{  
     this->getAppHandle()->show_log_module("app_module_ethernet","forceSynchronize","start",20);          
     this->getAppHandle()->show_log_module("app_module_ethernet","forceSynchronize","stop",20);          
}

//-------------------------------------------------------+-----------------------


bool app_module_ethernet::execute(gxh_message* inMsg,gxh_message* outMsg)  
{       
      gxh_message message;
      memcpy(&message, inMsg, sizeof(gxh_message));
    
    
      
     if(message.func_number == 301 )     //wyślij email
           {
               return this->sendEmailSMS(&message,outMsg,0);
           }
      
      if(message.func_number == 302 )     //wywołaj skrypt php
           {
               return this->php(&message,outMsg);
           }
      
      if(message.func_number == 303 )     //wyślij sms
           {
               return this->sendEmailSMS(&message,outMsg,1);
           }
          
          
     
          const char* response = "ETHERNET MODULE"; 
          gxh_message msg;
          msg.gxh_handle = message.gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(outMsg, &msg, sizeof(gxh_message));            
          return true;
          
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void app_module_ethernet::getHost(char* from, char* to,char* param)
{
    int length = strlen(from);
    int start = 0;    
    int startParam = 0;
    
    for(int i=1; i<length; i++)
    {
        if(from[i-1] == '/' && from[i] == '/' ) //http://
        {
            start = i+1;
        }
    }
    
    int ch = 0;
    
    for(int i=start; i<length; i++)
    {
        if( from[i] == '/' ) // .pl/
        {
            startParam = i;
            break;
        }
        
        to[ch] = from[i];
        ch++;
    }
    
    
    to[ch] = '\0';
 
    
    int p = 0;
    for(int i=startParam; i<length; i++)
    {
        param[p] = from[i];
        p++;
    }
    
    param[p] = '\0';
    
    
}

//------------------------------------------------------------------------------

bool app_module_ethernet::php(gxh_message* inMsg, gxh_message* outMsg)
{
          ih_driver_port_function functionList;
          memcpy(&functionList, inMsg->dataStruct, sizeof(ih_driver_port_function) );
          
                
          
          if(strlen(functionList.name) < 5)
          {
              const char* response = "ERROR:EMPTY URL"; 
              gxh_message msg;
              msg.gxh_handle = inMsg->gxh_handle;
              memcpy(msg.dataStruct, response, strlen(response) );
              msg.sizeDataStruct = sizeof(response) ;
              msg.system_code = GXH_MESSAGE_SUCCESS;
            
              memcpy(outMsg, &msg, sizeof(gxh_message));            
              return true;
          }
          
                 
                
          //------------------------------------------
          char bufferHost[1024];
          char params[1024];
          this->getHost( functionList.name ,bufferHost, params);
          
          
          this->getAppHandle()->show_log_module("app_module_ethernet","sendPHP",bufferHost,2 );
          
          std::string POST_DATA ="param_1=";
          POST_DATA.append("app_module_ethernet");
                                       
          
          
          if(strlen(functionList.text_content) > 0)
          {
             POST_DATA.append("&param_2=");
             POST_DATA.append( functionList.text_content);
          }                   
          
          POST_DATA.append("&REG_KEY=");
          POST_DATA.append( this->getAppHandle()->getKey() ); 
          
          
          std::string URL = "";
          URL.append(bufferHost);
          
          std::string POST = "";
          POST.append("POST ");
          POST.append(params);
          POST.append(" HTTP/1.0\r\n");
          
          POST.append("Host: ");
          POST.append(bufferHost);
          POST.append("\r\n");
          
          POST.append("User-Agent: ");
          POST.append("GXH web plugin/1.0 Błażej Kita, GXH ");
          POST.append(this->getAppHandle()->getVersion());
          POST.append("\r\n");
          
          POST.append("Content-Type: ");
          POST.append("application/x-www-form-urlencoded");
          POST.append("\r\n");
          
          POST.append("Content-Length: ");
          POST.append( gxh_IntToString( POST_DATA.length() ) );
          POST.append("\r\n");
          
          POST.append("\r\n");     
          POST.append(POST_DATA);
          
          
          this->getAppHandle()->show_log_module("app_module_ethernet","sendPHP",POST.c_str(),2 );
          
          
          /* first what are we going to send and where are we going to send it? */
          int portno =        80;
          const char *host =        URL.c_str();
          const char *message_fmt = POST.c_str();  // "POST /apikey=%s&command=%s HTTP/1.0\r\n\r\n";

          struct hostent *server;
          struct sockaddr_in serv_addr;
          int sockfd, bytes, sent, received, total;
          char message[4096],response_serv[4096];
        

          /* fill in the parameters */      
          memcpy(message, message_fmt, strlen(message_fmt));

          /* create the socket */
         sockfd = socket(AF_INET, SOCK_STREAM, 0);
         if (sockfd < 0)
         {                        
             this->getAppHandle()->show_error("app_module_ethernet","sendPHP","error opening socket",1);
             return false;
         }

          /* lookup the ip address */
         server = gethostbyname(host);
         if (server == NULL)
         {     
              close(sockfd);
              this->getAppHandle()->show_error("app_module_ethernet","sendPHP","not such host",1);
              
              const char* response_serv = "TimeOut for host";
              
              gxh_message msg;
              msg.gxh_handle = inMsg->gxh_handle;
              memcpy(msg.dataStruct, response_serv, strlen(response_serv) );
              msg.sizeDataStruct = strlen(response_serv) ;
              msg.system_code = GXH_MESSAGE_SUCCESS;
            
             memcpy(outMsg, &msg, sizeof(gxh_message));            
             return true; 
              
         }

         /* fill in the structure */
         memset(&serv_addr,0,sizeof(serv_addr));
         serv_addr.sin_family = AF_INET;
         serv_addr.sin_port = htons(portno);
         memcpy(&serv_addr.sin_addr.s_addr,server->h_addr,server->h_length);

         /* connect the socket */
         if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
         {
              this->getAppHandle()->show_error("app_module_ethernet","sendPHP","error connection",1);
              close(sockfd);
               
              const char* response_serv = "Error connection";
              
              gxh_message msg;
              msg.gxh_handle = inMsg->gxh_handle;
              memcpy(msg.dataStruct, response_serv, strlen(response_serv) );
              msg.sizeDataStruct = strlen(response_serv) ;
              msg.system_code = GXH_MESSAGE_SUCCESS;
            
             memcpy(outMsg, &msg, sizeof(gxh_message));            
             return true; 
         }

         /* send the request */
         total = strlen(message);
         sent = 0;
         do 
         {
          bytes = write(sockfd,message+sent,total-sent);
           if (bytes < 0)
           {      
               this->getAppHandle()->show_error("app_module_ethernet","sendPHP","ERROR writing message to socket",1);
           }
            if (bytes == 0)
             break;
             sent+=bytes;
         } while (sent < total);

         
        /* receive the response */
        memset(response_serv,0,sizeof(response_serv));
        total = sizeof(response_serv)-1;
        received = 0;
        do {
           bytes = read(sockfd,response_serv+received,total-received);
           if (bytes < 0)
           {              
              this->getAppHandle()->show_error("app_module_ethernet","sendPHP","ERROR reading response from socket",1);
           }
           if (bytes == 0)
              break;
           received+=bytes;
           } while (received < total);

        if (received == total)
        {            
             this->getAppHandle()->show_error("app_module_ethernet","sendPHP","ERROR storing complete response from socket",1);
        }

     
       close(sockfd);

    
          
       //------------------------------------------
          
          
        
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response_serv, strlen(response_serv) );
          msg.sizeDataStruct = strlen(response_serv) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(outMsg, &msg, sizeof(gxh_message));            
          return true; 
}

//------------------------------------------------------------------------------

bool app_module_ethernet::sendEmailSMS(gxh_message* inMsg, gxh_message* outMsg, int type)
{
          ih_driver_port_function functionList;
          memcpy(&functionList, inMsg->dataStruct, sizeof(ih_driver_port_function) );
          
          if(strlen(functionList.name) < 2)
          {
              const char* response = "ERROR:EMPTY TITLE"; 
              gxh_message msg;
              msg.gxh_handle = inMsg->gxh_handle;
              memcpy(msg.dataStruct, response, strlen(response) );
              msg.sizeDataStruct = sizeof(response) ;
              msg.system_code = GXH_MESSAGE_SUCCESS;
            
              memcpy(outMsg, &msg, sizeof(gxh_message));            
              return true;
          }
          
          
          
          if(strlen(functionList.text_content) < 2)
          {
              const char* response = "ERROR:EMPTY TEXT"; 
              gxh_message msg;
              msg.gxh_handle = inMsg->gxh_handle;
              memcpy(msg.dataStruct, response, strlen(response) );
              msg.sizeDataStruct = sizeof(response) ;
              msg.system_code = GXH_MESSAGE_SUCCESS;
            
              memcpy(outMsg, &msg, sizeof(gxh_message));            
              return true;
          }
          
          
          
          if(functionList.resource_id < 0)
          {
              const char* response = "ERROR:EMPTY USER"; 
              gxh_message msg;
              msg.gxh_handle = inMsg->gxh_handle;
              memcpy(msg.dataStruct, response, strlen(response) );
              msg.sizeDataStruct = sizeof(response) ;
              msg.system_code = GXH_MESSAGE_SUCCESS;
            
              memcpy(outMsg, &msg, sizeof(gxh_message));            
              return true;
          }
          
          
        
          
          unsigned int count = 0;
          user_gxh* users = (user_gxh*)this->getAppHandle()->getResource(RES_VECTOR_USER, &count ); 
          
          int index = -1;
          for(unsigned int x=0; x<count; x++)
          {
              if(users[x].px_group_id == functionList.resource_id)
              {
                  index = x;                  
                  break;
              }
          }
          
          if(index == -1)
          {
              const char* response = "ERROR:USER NOT FOUND"; 
              gxh_message msg;
              msg.gxh_handle = inMsg->gxh_handle;
              memcpy(msg.dataStruct, response, strlen(response) );
              msg.sizeDataStruct = sizeof(response) ;
              msg.system_code = GXH_MESSAGE_SUCCESS;
            
              memcpy(outMsg, &msg, sizeof(gxh_message));            
              return true;  
          }
          
          
          //------------------------------------------
          char* hostName = 0;
          if(type == 0)hostName = (char*)"http://gxhpanel.progressplus.pl/index.php5?controller=api&caction=send_mail";
          if(type == 1)hostName = (char*)"http://gxhpanel.progressplus.pl/index.php5?controller=api&caction=send_sms";
          
          char bufferHost[1024];
          char params[1024];
          this->getHost( hostName ,bufferHost, params);
          
          
          this->getAppHandle()->show_log_module("app_module_ethernet","sendPHP",bufferHost,4 );
          
          std::string POST_DATA ="subject=";
          POST_DATA.append(functionList.name);                                                           
        
          POST_DATA.append("&text=");
          POST_DATA.append( functionList.text_content);
          
          POST_DATA.append("&mail=");
          POST_DATA.append( users[index].username ) ;
          
          POST_DATA.append("&phone=");
          POST_DATA.append(  users[index].phone);
          
          POST_DATA.append("&user_name=");
          POST_DATA.append( users[index].name);
          
          POST_DATA.append("&user_last_name=");
          POST_DATA.append(  users[index].last_name);
                             
          
          POST_DATA.append("&REG_KEY=");
          POST_DATA.append( this->getAppHandle()->getKey() ); 
          
          
          std::string URL = "";
          URL.append(bufferHost);
          
          std::string POST = "";
          POST.append("POST ");
          POST.append(params);
          POST.append(" HTTP/1.0\r\n");
          
          POST.append("Host: ");
          POST.append(bufferHost);
          POST.append("\r\n");
          
          POST.append("User-Agent: ");
          POST.append("GXH web plugin/1.0 Błażej Kita, GXH ");
          POST.append(this->getAppHandle()->getVersion());
          POST.append("\r\n");
          
          POST.append("Content-Type: ");
          POST.append("application/x-www-form-urlencoded");
          POST.append("\r\n");
          
          POST.append("Content-Length: ");
          POST.append( gxh_IntToString( POST_DATA.length() ) );
          POST.append("\r\n");
          
          POST.append("\r\n");     
          POST.append(POST_DATA);
          
          
          this->getAppHandle()->show_log_module("app_module_ethernet","sendPHP",POST.c_str(),4 );
          
          
          /* first what are we going to send and where are we going to send it? */
          int portno =        80;
          const char *host =        URL.c_str();
          const char *message_fmt = POST.c_str();  // "POST /apikey=%s&command=%s HTTP/1.0\r\n\r\n";

          struct hostent *server;
          struct sockaddr_in serv_addr;
          int sockfd, bytes, sent, received, total;
          char message[4096],response_serv[4096];
        

          /* fill in the parameters */      
          memcpy(message, message_fmt, strlen(message_fmt));

          /* create the socket */
         sockfd = socket(AF_INET, SOCK_STREAM, 0);
         if (sockfd < 0)
         {                        
             this->getAppHandle()->show_error("app_module_ethernet","sendPHP","error opening socket",1);
             return false;
         }

          /* lookup the ip address */
         server = gethostbyname(host);
         if (server == NULL)
         {     
              close(sockfd);
              this->getAppHandle()->show_error("app_module_ethernet","sendPHP","not such host",1);
              return false;
         }

         /* fill in the structure */
         memset(&serv_addr,0,sizeof(serv_addr));
         serv_addr.sin_family = AF_INET;
         serv_addr.sin_port = htons(portno);
         memcpy(&serv_addr.sin_addr.s_addr,server->h_addr,server->h_length);

         /* connect the socket */
         if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
         {
               this->getAppHandle()->show_error("app_module_ethernet","sendPHP","error connection",1);
               close(sockfd);
               return false;
         }

         /* send the request */
         total = strlen(message);
         sent = 0;
         do 
         {
          bytes = write(sockfd,message+sent,total-sent);
           if (bytes < 0)
           {      
               this->getAppHandle()->show_error("app_module_ethernet","sendPHP","ERROR writing message to socket",1);
           }
            if (bytes == 0)
             break;
             sent+=bytes;
         } while (sent < total);

         
        /* receive the response */
        memset(response_serv,0,sizeof(response_serv));
        total = sizeof(response_serv)-1;
        received = 0;
        do {
           bytes = read(sockfd,response_serv+received,total-received);
           if (bytes < 0)
           {              
              this->getAppHandle()->show_error("app_module_ethernet","sendPHP","ERROR reading response from socket",1);
           }
           if (bytes == 0)
              break;
           received+=bytes;
           } while (received < total);

        if (received == total)
        {            
             this->getAppHandle()->show_error("app_module_ethernet","sendPHP","ERROR storing complete response from socket",1);
        }

     
       close(sockfd);

    
          
       //------------------------------------------
     
        this->getAppHandle()->show_error("app_module_ethernet","sendSMS", (const char*)response_serv ,1);
       
       //------------------------------------------
          
          
        
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response_serv, strlen(response_serv) );
          msg.sizeDataStruct = strlen(response_serv) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(outMsg, &msg, sizeof(gxh_message));            
          return true;
}

//------------------------------------------------------------------------------
 