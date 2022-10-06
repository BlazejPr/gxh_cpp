
/* 
 * File:   app_module_provent.cpp
 * Author: Blazej Kita
 * 
 * Created on 14 wrzesień 2016, 09:04
 */

#include "app_module_provent.h"

 
app_module* get_module(gxh_scope* appHomeHandle)
{
    return new app_module_provent(appHomeHandle); 
};
 
//-------------------------------------------------------------------------------
  
app_module_provent::app_module_provent(gxh_scope* appHomeHandle) : app_module(appHomeHandle) 
{           
    this->getAppHandle()->show_log_module("app_module_provent","app_module_provent","start.",3 );         
}

//------------------------------------------------------------------------------

app_module_provent::~app_module_provent() 
{     
    this->getAppHandle()->show_log_module("app_module_provent","app_module_provent","stop",3 );
}

//-------------------------------------------------------------------------------

void app_module_provent::onClick(int ih_driver_port_id, int listener_status)
{
    
}

//-------------------------------------------------------------------------------

void app_module_provent::forceCancel()
{
    
}

//-------------------------------------------------------------------------------

void app_module_provent::forceSynchronize()
{
     this->getAppHandle()->show_log_module("app_module_provent","app_module_provent","start",20);   
     this->getAppHandle()->show_log_module("app_module_provent","app_module_provent","stop",20);   
}

//-------------------------------------------------------------------------------
 
bool app_module_provent::execute(gxh_message* inMsg, gxh_message* message_ret)
{        
    
          gxh_message messageIn;
          memcpy(&messageIn, inMsg, sizeof(gxh_message));
      
          if(messageIn.func_number == 3100 && messageIn.func_internal_number == IH_UPDATE_ADDRESS )               
          {
              //skopiuj ramke przesłanego polecenia do sterownika..
                system_driver_command driverData; 
                memcpy(&driverData, messageIn.dataStruct, sizeof(system_driver_command) );
                
                
                proventData st; 
                st.paramInt = gxh_StringToInt( driverData.param );
                st.gxhHandle = this->getAppHandle()->createGXHHandle("provent", driverData.ih_driver_id, 3); //uchwyt lokalny zwrotny...
                if(st.gxhHandle == -1) 
                {
                    this->getAppHandle()->show_error("app_module_provent","execute","gxhHandle error",4);
                    const char* response = "gxhHandle error";                     
                    gxh_message msg;
                    msg.gxh_handle = messageIn.gxh_handle;
                    memcpy(msg.dataStruct, response, strlen(response) );
                    msg.sizeDataStruct = sizeof(response) ;
                    msg.system_code = GXH_MESSAGE_SUCCESS;            
                    memcpy(message_ret, &msg, sizeof(gxh_message));            
                    return true; 
                }
                
                //
                //send message to driver....
                gxh_message msgCommand;
                msgCommand.system_code          = GXH_MESSAGE_REDIRECT; //przekaż bezpośrednia od gxhHandle....
                msgCommand.gxh_handle           = this->getAppHandle()->getGXHHandleByIdAndtype(driverData.ih_driver_id, 1); //dirver handle....
                msgCommand.func_number          = messageIn.func_number;        
                msgCommand.func_internal_number = messageIn.func_internal_number;
                msgCommand.sizeDataStruct       = sizeof(proventData);
                memcpy(msgCommand.dataStruct, &st, sizeof(proventData) );              
                                      
                if( msgCommand.gxh_handle < 0) 
                {               
                    this->getAppHandle()->destroyGXHHandle(st.gxhHandle); //usuń przydzielony uchwyt..
                    
                    const char* response = "Driver doesn't register!";                     
                    gxh_message msg;
                    msg.gxh_handle = messageIn.gxh_handle;
                    memcpy(msg.dataStruct, response, strlen(response) );
                    msg.sizeDataStruct = sizeof(response) ;
                    msg.system_code = GXH_MESSAGE_SUCCESS;            
                    memcpy(message_ret, &msg, sizeof(gxh_message));            
                    return true; 
                }

                //wyślij polecenie do core -> przekierowania do urzadzenia..                
                this->getAppHandle()->sendGXHMessage(&msgCommand);      //wyślij wiadomość do urzadzenia....
                
                //odbierz zwrotke od urzadzenia..
                gxh_message msgResDriver;
                if(this->getAppHandle()->receiveGXHMessage(st.gxhHandle,&msgResDriver, 2000)) //czekaj na odpowiedz sterownika....
                {
                    this->getAppHandle()->destroyGXHHandle(st.gxhHandle); //usuń przydzielony uchwyt..
                    
                    proventData res_pd;       
                    memcpy(&res_pd, msgResDriver.dataStruct, sizeof(proventData) );
                    
                    const char* response = "Success"; 
                    if(!res_pd.retSuccess) response = "Error";
                    gxh_message msg;
                    msg.gxh_handle = messageIn.gxh_handle;
                    memcpy(msg.dataStruct, response, strlen(response) );
                    msg.sizeDataStruct = sizeof(response) ;
                    msg.system_code = GXH_MESSAGE_SUCCESS;
            
                    memcpy(message_ret, &msg, sizeof(gxh_message));            
                   return true;   
                }else
                {
                    this->getAppHandle()->destroyGXHHandle(st.gxhHandle); //usuń przydzielony uchwyt..
                    
                    const char* response = "Error TIMEOUT (100)"; 
                    gxh_message msg;
                    msg.gxh_handle = messageIn.gxh_handle;
                    memcpy(msg.dataStruct, response, strlen(response) );
                    msg.sizeDataStruct = sizeof(response) ;
                    msg.system_code = GXH_MESSAGE_SUCCESS;
            
                    memcpy(message_ret, &msg, sizeof(gxh_message));            
                    return true;  
                }
          }
          
          
          if(messageIn.func_number == 3100 && messageIn.func_internal_number == 10 )      //update data driver....
          {
                 proventData pd;                
                 memcpy(&pd, messageIn.dataStruct, sizeof(proventData) );
                 
                 unsigned int size = 0;
                 ih_driver_values* first = (ih_driver_values*)this->getAppHandle()->getResource(RES_VECTOR_DRIVER_VALUE, &size);
  
                 this->getAppHandle()->show_log_module("app_module_provent","execute","Dane od sterownika",3 );
                 this->getAppHandle()->show_log_module("app_module_provent","execute",pd.response,3 );
                                 
                 for(unsigned int x=0; x<size; x++)
                 {
                      // zmienna sys. 3100,  stan wietrzenia
                     if(first[x].ih_driver_id == pd.ih_driver_id && first[x].const_function == 3100)
                     {
                          if(pd.wietrzenie) strcpy( first[x].value, "1");
                          if(!pd.wietrzenie) strcpy( first[x].value, "0");
                          first[x].status = 1;
                     }
                     
                     // zmienna sys. 3101,  stan filtrów
                     if(first[x].ih_driver_id == pd.ih_driver_id && first[x].const_function == 3101)
                     {
                          if(pd.stan_filtrow) strcpy( first[x].value, "1");
                          if(!pd.stan_filtrow) strcpy( first[x].value, "0");
                          first[x].status = 1;
                     }
                     
                     // zmienna sys. 3102,  stan wentylacji
                     if(first[x].ih_driver_id == pd.ih_driver_id && first[x].const_function == 3102)
                     {
                          if(pd.stan_wentylacji) strcpy( first[x].value, "1");
                          if(!pd.stan_wentylacji) strcpy( first[x].value, "0");
                          first[x].status = 1;
                     }
                     
                     // zmienna sys. 3103,  bieg wentylacji
                     if(first[x].ih_driver_id == pd.ih_driver_id && first[x].const_function == 3103)
                     {
                          std::string tmpString = gxh_IntToString( (int)pd.bieg_wentylacji );
                          strcpy( first[x].value, tmpString.c_str() );
                          first[x].status = 1;
                     }
                     
                      // zmienna sys. 3104,  tryb pracy wentylacji ????????????
                     if(first[x].ih_driver_id == pd.ih_driver_id && first[x].const_function == 3104)
                     {
                          //??
                     }
                     
                     // zmienna sys. 3105,  rodzaj wentylacji
                     if(first[x].ih_driver_id == pd.ih_driver_id && first[x].const_function == 3105)
                     {
                          std::string tmpString = gxh_IntToString( (int)pd.ustawiony_rodzaj_wentylacji );
                          strcpy( first[x].value, tmpString.c_str() );
                          first[x].status = 1;
                     }
                     
                     // zmienna sys. 3106,  stan nawiewu
                     if(first[x].ih_driver_id == pd.ih_driver_id && first[x].const_function == 3106)
                     {
                          if(pd.stan_nawiewu) strcpy( first[x].value, "1");
                          if(!pd.stan_nawiewu) strcpy( first[x].value, "0");
                          first[x].status = 1;
                     }
                     
                     // zmienna sys. 3107,  stan wywiewu
                     if(first[x].ih_driver_id == pd.ih_driver_id && first[x].const_function == 3107)
                     {
                          if(pd.stan_wywiewu) strcpy( first[x].value, "1");
                          if(!pd.stan_wywiewu) strcpy( first[x].value, "0");
                          first[x].status = 1;
                     }
                     
                     // zmienna sys. 3108,  bypass
                     if(first[x].ih_driver_id == pd.ih_driver_id && first[x].const_function == 3108)
                     {
                          if(pd.stan_bypassu) strcpy( first[x].value, "1");
                          if(!pd.stan_bypassu) strcpy( first[x].value, "0");
                          first[x].status = 1;
                     }
                     
                     // zmienna sys. 3109,  tryb pracy centrali
                     if(first[x].ih_driver_id == pd.ih_driver_id && first[x].const_function == 3109)
                     {
                          std::string tmpString = gxh_IntToString( (int)pd.tryb_pracy_centrali );
                          strcpy( first[x].value, tmpString.c_str() );
                          first[x].status = 1;
                     }
                     
                     // zmienna sys. 3110,  czas wietrzenia
                     if(first[x].ih_driver_id == pd.ih_driver_id && first[x].const_function == 3110)
                     {
                          std::string tmpString = gxh_IntToString( (int)pd.czas_wietrzenia );
                          strcpy( first[x].value, tmpString.c_str() );
                          first[x].status = 1;
                     }
                     
                     // zmienna sys. 3111,  mierzona temp bypass
                     if(first[x].ih_driver_id == pd.ih_driver_id && first[x].const_function == 3111)
                     {
                          std::string tmpString = gxh_IntToString( (int)pd.temperatura_bypass );
                          strcpy( first[x].value, tmpString.c_str() );
                          first[x].status = 1;
                          first[x].toHistory = true;
                     }
                 }
                 
          }
       
           
          if(messageIn.func_number == 3100 && messageIn.func_internal_number == 100 )  //ustaw stan wietrzenia
          { 
              
                //skopiuj ramke przesłanego polecenia do sterownika..
                system_driver_command driverData; 
                memcpy(&driverData, messageIn.dataStruct, sizeof(system_driver_command) );
                
                
                proventData st; 
                if(std::string(driverData.param) == "1") st.stan = 1; //on
                if(std::string(driverData.param) == "0") st.stan = 2; //off
                st.gxhHandle = this->getAppHandle()->createGXHHandle("provent", driverData.ih_driver_id, 3); //uchwyt lokalny zwrotny...
                if(st.gxhHandle == -1) 
                {
                    this->getAppHandle()->show_error("app_module_provent","execute","gxhHandle error",4);
                    const char* response = "gxhHandle error";                     
                    gxh_message msg;
                    msg.gxh_handle = messageIn.gxh_handle;
                    memcpy(msg.dataStruct, response, strlen(response) );
                    msg.sizeDataStruct = sizeof(response) ;
                    msg.system_code = GXH_MESSAGE_SUCCESS;            
                    memcpy(message_ret, &msg, sizeof(gxh_message));            
                    return true; 
                }
                
                //
                //send message to driver....
                gxh_message msgCommand;
                msgCommand.system_code          = GXH_MESSAGE_REDIRECT; //przekaż bezpośrednia od gxhHandle....
                msgCommand.gxh_handle           = this->getAppHandle()->getGXHHandleByIdAndtype(driverData.ih_driver_id, 1); //dirver handle....
                msgCommand.func_number          = messageIn.func_number;        
                msgCommand.func_internal_number = messageIn.func_internal_number;
                msgCommand.sizeDataStruct       = sizeof(proventData);
                memcpy(msgCommand.dataStruct, &st, sizeof(proventData) );              
                                      
                if( msgCommand.gxh_handle < 0) 
                {               
                    this->getAppHandle()->destroyGXHHandle(st.gxhHandle); //usuń przydzielony uchwyt..
                    
                    const char* response = "Driver doesn't register!";                     
                    gxh_message msg;
                    msg.gxh_handle = messageIn.gxh_handle;
                    memcpy(msg.dataStruct, response, strlen(response) );
                    msg.sizeDataStruct = sizeof(response) ;
                    msg.system_code = GXH_MESSAGE_SUCCESS;            
                    memcpy(message_ret, &msg, sizeof(gxh_message));            
                    return true; 
                }

                //wyślij polecenie do core -> przekierowania do urzadzenia..                
                this->getAppHandle()->sendGXHMessage(&msgCommand);      //wyślij wiadomość do urzadzenia....
                
                //odbierz zwrotke od urzadzenia..
                gxh_message msgResDriver;
                if(this->getAppHandle()->receiveGXHMessage(st.gxhHandle,&msgResDriver, 2000)) //czekaj na odpowiedz sterownika....
                {
                    this->getAppHandle()->destroyGXHHandle(st.gxhHandle); //usuń przydzielony uchwyt..
                    
                    proventData res_pd;       
                    memcpy(&res_pd, msgResDriver.dataStruct, sizeof(proventData) );
                    
                    const char* response = "Success"; 
                    if(!res_pd.retSuccess) response = "Error";
                    gxh_message msg;
                    msg.gxh_handle = messageIn.gxh_handle;
                    memcpy(msg.dataStruct, response, strlen(response) );
                    msg.sizeDataStruct = sizeof(response) ;
                    msg.system_code = GXH_MESSAGE_SUCCESS;
            
                    memcpy(message_ret, &msg, sizeof(gxh_message));            
                   return true;   
                }else
                {
                    this->getAppHandle()->destroyGXHHandle(st.gxhHandle); //usuń przydzielony uchwyt..
                    
                    const char* response = "Error TIMEOUT (100)"; 
                    gxh_message msg;
                    msg.gxh_handle = messageIn.gxh_handle;
                    memcpy(msg.dataStruct, response, strlen(response) );
                    msg.sizeDataStruct = sizeof(response) ;
                    msg.system_code = GXH_MESSAGE_SUCCESS;
            
                    memcpy(message_ret, &msg, sizeof(gxh_message));            
                    return true;  
                }
          }
          
          
          if(messageIn.func_number == 3100 && messageIn.func_internal_number == 101 )  //bieg wentylacji
          { 
                //skopiuj ramke przesłanego polecenia do sterownika..
                system_driver_command driverData; 
                memcpy(&driverData, messageIn.dataStruct, sizeof(system_driver_command) );
                
                
                proventData st; 
                st.bieg_wentylacji = (unsigned short)gxh_StringToInt(  std::string( driverData.param) );
                st.gxhHandle = this->getAppHandle()->createGXHHandle("provent", driverData.ih_driver_id, 3); //uchwyt lokalny zwrotny...
                if(st.gxhHandle == -1) 
                {
                    this->getAppHandle()->show_error("app_module_provent","execute","gxhHandle error",4);
                    const char* response = "gxhHandle error";                     
                    gxh_message msg;
                    msg.gxh_handle = messageIn.gxh_handle;
                    memcpy(msg.dataStruct, response, strlen(response) );
                    msg.sizeDataStruct = sizeof(response) ;
                    msg.system_code = GXH_MESSAGE_SUCCESS;            
                    memcpy(message_ret, &msg, sizeof(gxh_message));            
                    return true; 
                }
                
                //
                //send message to driver....
                gxh_message msgCommand;
                msgCommand.system_code          = GXH_MESSAGE_REDIRECT; //przekaż bezpośrednia od gxhHandle....
                msgCommand.gxh_handle           = this->getAppHandle()->getGXHHandleByIdAndtype(driverData.ih_driver_id, 1); //dirver handle....
                msgCommand.func_number          = messageIn.func_number;        
                msgCommand.func_internal_number = messageIn.func_internal_number;
                msgCommand.sizeDataStruct       = sizeof(proventData);
                memcpy(msgCommand.dataStruct, &st, sizeof(proventData) );              
                                      
                if( msgCommand.gxh_handle < 0) 
                {               
                    this->getAppHandle()->destroyGXHHandle(st.gxhHandle); //usuń przydzielony uchwyt..
                    
                    const char* response = "Driver doesn't register!";                     
                    gxh_message msg;
                    msg.gxh_handle = messageIn.gxh_handle;
                    memcpy(msg.dataStruct, response, strlen(response) );
                    msg.sizeDataStruct = sizeof(response) ;
                    msg.system_code = GXH_MESSAGE_SUCCESS;            
                    memcpy(message_ret, &msg, sizeof(gxh_message));            
                    return true; 
                }

                //wyślij polecenie do core -> przekierowania do urzadzenia..                
                this->getAppHandle()->sendGXHMessage(&msgCommand);      //wyślij wiadomość do urzadzenia....
                
                //odbierz zwrotke od urzadzenia..
                gxh_message msgResDriver;
                if(this->getAppHandle()->receiveGXHMessage(st.gxhHandle,&msgResDriver, 2000)) //czekaj na odpowiedz sterownika....
                {
                    this->getAppHandle()->destroyGXHHandle(st.gxhHandle); //usuń przydzielony uchwyt..
                    
                    proventData res_pd;       
                    memcpy(&res_pd, msgResDriver.dataStruct, sizeof(proventData) );
                    
                    const char* response = "Success"; 
                    if(!res_pd.retSuccess) response = "Error";
                    gxh_message msg;
                    msg.gxh_handle = messageIn.gxh_handle;
                    memcpy(msg.dataStruct, response, strlen(response) );
                    msg.sizeDataStruct = sizeof(response) ;
                    msg.system_code = GXH_MESSAGE_SUCCESS;
            
                    memcpy(message_ret, &msg, sizeof(gxh_message));            
                   return true;   
                }else
                {
                    this->getAppHandle()->destroyGXHHandle(st.gxhHandle); //usuń przydzielony uchwyt..
                    
                    const char* response = "Error TIMEOUT (100)"; 
                    gxh_message msg;
                    msg.gxh_handle = messageIn.gxh_handle;
                    memcpy(msg.dataStruct, response, strlen(response) );
                    msg.sizeDataStruct = sizeof(response) ;
                    msg.system_code = GXH_MESSAGE_SUCCESS;
            
                    memcpy(message_ret, &msg, sizeof(gxh_message));            
                    return true;  
                }
          }
           
           if(messageIn.func_number == 3100 && messageIn.func_internal_number == 102 ) //Tryb pracy centrali
          { 
                //skopiuj ramke przesłanego polecenia do sterownika..
                system_driver_command driverData; 
                memcpy(&driverData, messageIn.dataStruct, sizeof(system_driver_command) );
                
                
                proventData st; 
                st.tryb_pracy_centrali = (unsigned short)gxh_StringToInt(  std::string( driverData.param) );
                st.gxhHandle = this->getAppHandle()->createGXHHandle("provent", driverData.ih_driver_id, 3); //uchwyt lokalny zwrotny...
                if(st.gxhHandle == -1) 
                {
                    this->getAppHandle()->show_error("app_module_provent","execute","gxhHandle error",4);
                    const char* response = "gxhHandle error";                     
                    gxh_message msg;
                    msg.gxh_handle = messageIn.gxh_handle;
                    memcpy(msg.dataStruct, response, strlen(response) );
                    msg.sizeDataStruct = sizeof(response) ;
                    msg.system_code = GXH_MESSAGE_SUCCESS;            
                    memcpy(message_ret, &msg, sizeof(gxh_message));            
                    return true; 
                }
                
                //
                //send message to driver....
                gxh_message msgCommand;
                msgCommand.system_code          = GXH_MESSAGE_REDIRECT; //przekaż bezpośrednia od gxhHandle....
                msgCommand.gxh_handle           = this->getAppHandle()->getGXHHandleByIdAndtype(driverData.ih_driver_id, 1); //dirver handle....
                msgCommand.func_number          = messageIn.func_number;        
                msgCommand.func_internal_number = messageIn.func_internal_number;
                msgCommand.sizeDataStruct       = sizeof(proventData);
                memcpy(msgCommand.dataStruct, &st, sizeof(proventData) );              
                                      
                if( msgCommand.gxh_handle < 0) 
                {               
                    this->getAppHandle()->destroyGXHHandle(st.gxhHandle); //usuń przydzielony uchwyt..
                    
                    const char* response = "Driver doesn't register!";                     
                    gxh_message msg;
                    msg.gxh_handle = messageIn.gxh_handle;
                    memcpy(msg.dataStruct, response, strlen(response) );
                    msg.sizeDataStruct = sizeof(response) ;
                    msg.system_code = GXH_MESSAGE_SUCCESS;            
                    memcpy(message_ret, &msg, sizeof(gxh_message));            
                    return true; 
                }

                //wyślij polecenie do core -> przekierowania do urzadzenia..                
                this->getAppHandle()->sendGXHMessage(&msgCommand);      //wyślij wiadomość do urzadzenia....
                
                //odbierz zwrotke od urzadzenia..
                gxh_message msgResDriver;
                if(this->getAppHandle()->receiveGXHMessage(st.gxhHandle,&msgResDriver, 2000)) //czekaj na odpowiedz sterownika....
                {
                    this->getAppHandle()->destroyGXHHandle(st.gxhHandle); //usuń przydzielony uchwyt..
                    
                    proventData res_pd;       
                    memcpy(&res_pd, msgResDriver.dataStruct, sizeof(proventData) );
                    
                    const char* response = "Success"; 
                    if(!res_pd.retSuccess) response = "Error";
                    gxh_message msg;
                    msg.gxh_handle = messageIn.gxh_handle;
                    memcpy(msg.dataStruct, response, strlen(response) );
                    msg.sizeDataStruct = sizeof(response) ;
                    msg.system_code = GXH_MESSAGE_SUCCESS;
            
                    memcpy(message_ret, &msg, sizeof(gxh_message));            
                   return true;   
                }else
                {
                    this->getAppHandle()->destroyGXHHandle(st.gxhHandle); //usuń przydzielony uchwyt..
                    
                    const char* response = "Error TIMEOUT (100)"; 
                    gxh_message msg;
                    msg.gxh_handle = messageIn.gxh_handle;
                    memcpy(msg.dataStruct, response, strlen(response) );
                    msg.sizeDataStruct = sizeof(response) ;
                    msg.system_code = GXH_MESSAGE_SUCCESS;
            
                    memcpy(message_ret, &msg, sizeof(gxh_message));            
                    return true;  
                }
          }
        
          
           if(messageIn.func_number == 3100 && messageIn.func_internal_number == 103 )  //Rodzaj wentylacji
          { 
                //skopiuj ramke przesłanego polecenia do sterownika..
                system_driver_command driverData; 
                memcpy(&driverData, messageIn.dataStruct, sizeof(system_driver_command) );
                
                
                proventData st; 
                st.tryb_pracy_wentylacji = (unsigned short)gxh_StringToInt(  std::string( driverData.param) );
                st.gxhHandle = this->getAppHandle()->createGXHHandle("provent", driverData.ih_driver_id, 3); //uchwyt lokalny zwrotny...
                if(st.gxhHandle == -1) 
                {
                    this->getAppHandle()->show_error("app_module_provent","execute","gxhHandle error",4);
                    const char* response = "gxhHandle error";                     
                    gxh_message msg;
                    msg.gxh_handle = messageIn.gxh_handle;
                    memcpy(msg.dataStruct, response, strlen(response) );
                    msg.sizeDataStruct = sizeof(response) ;
                    msg.system_code = GXH_MESSAGE_SUCCESS;            
                    memcpy(message_ret, &msg, sizeof(gxh_message));            
                    return true; 
                }
                
                //
                //send message to driver....
                gxh_message msgCommand;
                msgCommand.system_code          = GXH_MESSAGE_REDIRECT; //przekaż bezpośrednia od gxhHandle....
                msgCommand.gxh_handle           = this->getAppHandle()->getGXHHandleByIdAndtype(driverData.ih_driver_id, 1); //dirver handle....
                msgCommand.func_number          = messageIn.func_number;        
                msgCommand.func_internal_number = messageIn.func_internal_number;
                msgCommand.sizeDataStruct       = sizeof(proventData);
                memcpy(msgCommand.dataStruct, &st, sizeof(proventData) );              
                                      
                if( msgCommand.gxh_handle < 0) 
                {               
                    this->getAppHandle()->destroyGXHHandle(st.gxhHandle); //usuń przydzielony uchwyt..
                    
                    const char* response = "Driver doesn't register!";                     
                    gxh_message msg;
                    msg.gxh_handle = messageIn.gxh_handle;
                    memcpy(msg.dataStruct, response, strlen(response) );
                    msg.sizeDataStruct = sizeof(response) ;
                    msg.system_code = GXH_MESSAGE_SUCCESS;            
                    memcpy(message_ret, &msg, sizeof(gxh_message));            
                    return true; 
                }

                //wyślij polecenie do core -> przekierowania do urzadzenia..                
                this->getAppHandle()->sendGXHMessage(&msgCommand);      //wyślij wiadomość do urzadzenia....
                
                //odbierz zwrotke od urzadzenia..
                gxh_message msgResDriver;
                if(this->getAppHandle()->receiveGXHMessage(st.gxhHandle,&msgResDriver, 2000)) //czekaj na odpowiedz sterownika....
                {
                    this->getAppHandle()->destroyGXHHandle(st.gxhHandle); //usuń przydzielony uchwyt..
                    
                    proventData res_pd;       
                    memcpy(&res_pd, msgResDriver.dataStruct, sizeof(proventData) );
                    
                    const char* response = "Success"; 
                    if(!res_pd.retSuccess) response = "Error";
                    gxh_message msg;
                    msg.gxh_handle = messageIn.gxh_handle;
                    memcpy(msg.dataStruct, response, strlen(response) );
                    msg.sizeDataStruct = sizeof(response) ;
                    msg.system_code = GXH_MESSAGE_SUCCESS;
            
                    memcpy(message_ret, &msg, sizeof(gxh_message));            
                   return true;   
                }else
                {
                    this->getAppHandle()->destroyGXHHandle(st.gxhHandle); //usuń przydzielony uchwyt..
                    
                    const char* response = "Error TIMEOUT (100)"; 
                    gxh_message msg;
                    msg.gxh_handle = messageIn.gxh_handle;
                    memcpy(msg.dataStruct, response, strlen(response) );
                    msg.sizeDataStruct = sizeof(response) ;
                    msg.system_code = GXH_MESSAGE_SUCCESS;
            
                    memcpy(message_ret, &msg, sizeof(gxh_message));            
                    return true;  
                }
          }
          
          if(messageIn.func_number == 3100 && messageIn.func_internal_number == 104 )  //bypass
          { 
                //skopiuj ramke przesłanego polecenia do sterownika..
                system_driver_command driverData; 
                memcpy(&driverData, messageIn.dataStruct, sizeof(system_driver_command) );
                
                
                proventData st; 
                st.paramInt = (unsigned short)gxh_StringToInt(  std::string( driverData.param) );
                st.gxhHandle = this->getAppHandle()->createGXHHandle("provent", driverData.ih_driver_id, 3); //uchwyt lokalny zwrotny...
                if(st.gxhHandle == -1) 
                {
                    this->getAppHandle()->show_error("app_module_provent","execute","gxhHandle error",4);
                    const char* response = "gxhHandle error";                     
                    gxh_message msg;
                    msg.gxh_handle = messageIn.gxh_handle;
                    memcpy(msg.dataStruct, response, strlen(response) );
                    msg.sizeDataStruct = sizeof(response) ;
                    msg.system_code = GXH_MESSAGE_SUCCESS;            
                    memcpy(message_ret, &msg, sizeof(gxh_message));            
                    return true; 
                }
                
                //
                //send message to driver....
                gxh_message msgCommand;
                msgCommand.system_code          = GXH_MESSAGE_REDIRECT; //przekaż bezpośrednia od gxhHandle....
                msgCommand.gxh_handle           = this->getAppHandle()->getGXHHandleByIdAndtype(driverData.ih_driver_id, 1); //dirver handle....
                msgCommand.func_number          = messageIn.func_number;        
                msgCommand.func_internal_number = messageIn.func_internal_number;
                msgCommand.sizeDataStruct       = sizeof(proventData);
                memcpy(msgCommand.dataStruct, &st, sizeof(proventData) );              
                                      
                if( msgCommand.gxh_handle < 0) 
                {               
                    this->getAppHandle()->destroyGXHHandle(st.gxhHandle); //usuń przydzielony uchwyt..
                    
                    const char* response = "Driver doesn't register!";                     
                    gxh_message msg;
                    msg.gxh_handle = messageIn.gxh_handle;
                    memcpy(msg.dataStruct, response, strlen(response) );
                    msg.sizeDataStruct = sizeof(response) ;
                    msg.system_code = GXH_MESSAGE_SUCCESS;            
                    memcpy(message_ret, &msg, sizeof(gxh_message));            
                    return true; 
                }

                //wyślij polecenie do core -> przekierowania do urzadzenia..                
                this->getAppHandle()->sendGXHMessage(&msgCommand);      //wyślij wiadomość do urzadzenia....
                
                //odbierz zwrotke od urzadzenia..
                gxh_message msgResDriver;
                if(this->getAppHandle()->receiveGXHMessage(st.gxhHandle,&msgResDriver, 2000)) //czekaj na odpowiedz sterownika....
                {
                    this->getAppHandle()->destroyGXHHandle(st.gxhHandle); //usuń przydzielony uchwyt..
                    
                    proventData res_pd;       
                    memcpy(&res_pd, msgResDriver.dataStruct, sizeof(proventData) );
                    
                    const char* response = "Success"; 
                    if(!res_pd.retSuccess) response = "Error";
                    gxh_message msg;
                    msg.gxh_handle = messageIn.gxh_handle;
                    memcpy(msg.dataStruct, response, strlen(response) );
                    msg.sizeDataStruct = sizeof(response) ;
                    msg.system_code = GXH_MESSAGE_SUCCESS;
            
                    memcpy(message_ret, &msg, sizeof(gxh_message));            
                   return true;   
                }else
                {
                    this->getAppHandle()->destroyGXHHandle(st.gxhHandle); //usuń przydzielony uchwyt..
                    
                    const char* response = "Error TIMEOUT (100)"; 
                    gxh_message msg;
                    msg.gxh_handle = messageIn.gxh_handle;
                    memcpy(msg.dataStruct, response, strlen(response) );
                    msg.sizeDataStruct = sizeof(response) ;
                    msg.system_code = GXH_MESSAGE_SUCCESS;
            
                    memcpy(message_ret, &msg, sizeof(gxh_message));            
                    return true;  
                }
          } 
          
          
          if(messageIn.func_number == 3100 && messageIn.func_internal_number == 105 )  //Tryb pracy wentylacji
          { 
                //skopiuj ramke przesłanego polecenia do sterownika..
                system_driver_command driverData; 
                memcpy(&driverData, messageIn.dataStruct, sizeof(system_driver_command) );
                
                
                proventData st; 
                st.paramInt = (unsigned short)gxh_StringToInt(  std::string( driverData.param) );
                st.gxhHandle = this->getAppHandle()->createGXHHandle("provent", driverData.ih_driver_id, 3); //uchwyt lokalny zwrotny...
                if(st.gxhHandle == -1) 
                {
                    this->getAppHandle()->show_error("app_module_provent","execute","gxhHandle error",4);
                    const char* response = "gxhHandle error";                     
                    gxh_message msg;
                    msg.gxh_handle = messageIn.gxh_handle;
                    memcpy(msg.dataStruct, response, strlen(response) );
                    msg.sizeDataStruct = sizeof(response) ;
                    msg.system_code = GXH_MESSAGE_SUCCESS;            
                    memcpy(message_ret, &msg, sizeof(gxh_message));            
                    return true; 
                }
                
                //
                //send message to driver....
                gxh_message msgCommand;
                msgCommand.system_code          = GXH_MESSAGE_REDIRECT; //przekaż bezpośrednia od gxhHandle....
                msgCommand.gxh_handle           = this->getAppHandle()->getGXHHandleByIdAndtype(driverData.ih_driver_id, 1); //dirver handle....
                msgCommand.func_number          = messageIn.func_number;        
                msgCommand.func_internal_number = messageIn.func_internal_number;
                msgCommand.sizeDataStruct       = sizeof(proventData);
                memcpy(msgCommand.dataStruct, &st, sizeof(proventData) );              
                                      
                if( msgCommand.gxh_handle < 0) 
                {               
                    this->getAppHandle()->destroyGXHHandle(st.gxhHandle); //usuń przydzielony uchwyt..
                    
                    const char* response = "Driver doesn't register!";                     
                    gxh_message msg;
                    msg.gxh_handle = messageIn.gxh_handle;
                    memcpy(msg.dataStruct, response, strlen(response) );
                    msg.sizeDataStruct = sizeof(response) ;
                    msg.system_code = GXH_MESSAGE_SUCCESS;            
                    memcpy(message_ret, &msg, sizeof(gxh_message));            
                    return true; 
                }

                //wyślij polecenie do core -> przekierowania do urzadzenia..                
                this->getAppHandle()->sendGXHMessage(&msgCommand);      //wyślij wiadomość do urzadzenia....
                
                //odbierz zwrotke od urzadzenia..
                gxh_message msgResDriver;
                if(this->getAppHandle()->receiveGXHMessage(st.gxhHandle,&msgResDriver, 2000)) //czekaj na odpowiedz sterownika....
                {
                    this->getAppHandle()->destroyGXHHandle(st.gxhHandle); //usuń przydzielony uchwyt..
                    
                    proventData res_pd;       
                    memcpy(&res_pd, msgResDriver.dataStruct, sizeof(proventData) );
                    
                    const char* response = "Success"; 
                    if(!res_pd.retSuccess) response = "Error";
                    gxh_message msg;
                    msg.gxh_handle = messageIn.gxh_handle;
                    memcpy(msg.dataStruct, response, strlen(response) );
                    msg.sizeDataStruct = sizeof(response) ;
                    msg.system_code = GXH_MESSAGE_SUCCESS;
            
                    memcpy(message_ret, &msg, sizeof(gxh_message));            
                   return true;   
                }else
                {
                    this->getAppHandle()->destroyGXHHandle(st.gxhHandle); //usuń przydzielony uchwyt..
                    
                    const char* response = "Error TIMEOUT (100)"; 
                    gxh_message msg;
                    msg.gxh_handle = messageIn.gxh_handle;
                    memcpy(msg.dataStruct, response, strlen(response) );
                    msg.sizeDataStruct = sizeof(response) ;
                    msg.system_code = GXH_MESSAGE_SUCCESS;
            
                    memcpy(message_ret, &msg, sizeof(gxh_message));            
                    return true;  
                }
          } 
        
          
          
          
          if(messageIn.func_number == 3101) //włącznik wietrzenia...
          {
             this->getAppHandle()->show_log_module("app_module_provent","execute","wlacznik wietrzenia.",2 ); 
              
             ih_driver_port_function functionList;
             memcpy(&functionList, messageIn.dataStruct, sizeof(ih_driver_port_function) );
                                       
             unsigned int size = 0;
             ih_driver_values* first = (ih_driver_values*)this->getAppHandle()->getResource(RES_VECTOR_DRIVER_VALUE, &size);
             
             
               for(unsigned int x=0; x<size; x++)
                 {
                      // zmienna sys. 3100,  stan wietrzenia
                     if(first[x].ih_driver_id == functionList.resource_id && first[x].const_function == 3100) //aktualny stan wietrzenia na tym urzadzeniu..
                     {
                          proventData st; 
                          st.gxhHandle = -1; //nie potrzebujemy komunikatu zwrotnego...           
                         
                          if(gxh_compare(first[x].value,"0") == 0)   st.stan = 1; //on
                          if(gxh_compare(first[x].value,"1") == 0)   st.stan = 0; //off 
                        
                          //send message to driver....
                          gxh_message msgCommand;
                          msgCommand.system_code          = GXH_MESSAGE_REDIRECT; //przekaż bezpośrednia od gxhHandle....
                          msgCommand.gxh_handle           = this->getAppHandle()->getGXHHandleByIdAndtype( functionList.resource_id,1  ); //dirver handle....
                          msgCommand.func_number          = 3100;        
                          msgCommand.func_internal_number = 100;
                          msgCommand.sizeDataStruct       = sizeof(proventData);
                          memcpy(msgCommand.dataStruct, &st, sizeof(proventData) );              
                                      
                          this->getAppHandle()->sendGXHMessage(&msgCommand);      //wyślij wiadomość do urzadzenia....
                          return false;
                     }
                 }         
                 return false; 
          }
          
          
          
          if(messageIn.func_number == 3102) //zwiększ bieg wentylacji  
          {
             this->getAppHandle()->show_log_module("app_module_provent","execute","zwieksz bieg wentylacji",2 ); 
              
             ih_driver_port_function functionList;
             memcpy(&functionList, messageIn.dataStruct, sizeof(ih_driver_port_function) );
                                       
             unsigned int size = 0;
             ih_driver_values* first = (ih_driver_values*)this->getAppHandle()->getResource(RES_VECTOR_DRIVER_VALUE, &size);
             
             
               for(unsigned int x=0; x<size; x++)
                 {
                      // zmienna sys. 3100,  stan wietrzenia
                     if(first[x].ih_driver_id == functionList.resource_id && first[x].const_function == 3103) //aktualny bieg wentylacji
                     {
                          proventData st; 
                          st.gxhHandle = -1; //nie potrzebujemy komunikatu zwrotnego...           
                         
                          int bieg = gxh_StringToInt( first[x].value); 
  
                          if(bieg < 8) bieg++;
                          st.bieg_wentylacji = (unsigned short)bieg;
                          st.gxhHandle = -1; //nie potrzebujemy komunikatu zwrotnego... 
                        
                          //send message to driver....
                          gxh_message msgCommand;
                          msgCommand.system_code          = GXH_MESSAGE_REDIRECT; //przekaż bezpośrednia od gxhHandle....
                          msgCommand.gxh_handle           = this->getAppHandle()->getGXHHandleByIdAndtype( functionList.resource_id,1  ); //dirver handle....
                          msgCommand.func_number          = 3100;        
                          msgCommand.func_internal_number = 100;
                          msgCommand.sizeDataStruct       = sizeof(proventData);
                          memcpy(msgCommand.dataStruct, &st, sizeof(proventData) );              
                                      
                          this->getAppHandle()->sendGXHMessage(&msgCommand);      //wyślij wiadomość do urzadzenia....
                          return false;
                     }
                 }         
                 return false; 
          }
          
          
          
          
          if(messageIn.func_number == 3103) //zmniejsz bieg wentylacji
          {
             this->getAppHandle()->show_log_module("app_module_provent","execute","zmniejsz bieg wentylacji.",2 ); 
              
             ih_driver_port_function functionList;
             memcpy(&functionList, messageIn.dataStruct, sizeof(ih_driver_port_function) );
                                       
             unsigned int size = 0;
             ih_driver_values* first = (ih_driver_values*)this->getAppHandle()->getResource(RES_VECTOR_DRIVER_VALUE, &size);
             
             
               for(unsigned int x=0; x<size; x++)
                 {
                      // zmienna sys. 3100,  stan wietrzenia
                     if(first[x].ih_driver_id == functionList.resource_id && first[x].const_function == 3103) //aktualny bieg wentylacji
                     {
                           proventData st; 
                          st.gxhHandle = -1; //nie potrzebujemy komunikatu zwrotnego...           
                         
                          int bieg = gxh_StringToInt( first[x].value); 
  
                          if(bieg>1) bieg--;
                          st.bieg_wentylacji = (unsigned short)bieg;
                          st.gxhHandle = -1; //nie potrzebujemy komunikatu zwrotnego... 
                        
                          //send message to driver....
                          gxh_message msgCommand;
                          msgCommand.system_code          = GXH_MESSAGE_REDIRECT; //przekaż bezpośrednia od gxhHandle....
                          msgCommand.gxh_handle           = this->getAppHandle()->getGXHHandleByIdAndtype( functionList.resource_id,1  ); //dirver handle....
                          msgCommand.func_number          = 3100;        
                          msgCommand.func_internal_number = 100;
                          msgCommand.sizeDataStruct       = sizeof(proventData);
                          memcpy(msgCommand.dataStruct, &st, sizeof(proventData) );              
                                      
                          this->getAppHandle()->sendGXHMessage(&msgCommand);      //wyślij wiadomość do urzadzenia....
                          return false;
                     }
                 }         
                 return false; 
          }
          
          
          std::string msg = "Module function not found for ";
          msg.append(gxh_IntToString(messageIn.func_internal_number ));
          
          this->getAppHandle()->show_log_module("app_module_provent","execute", msg.c_str(),0 );                                              
          return false;                    
}
 
 
//-------------------------------------------------------------------------------
 

 