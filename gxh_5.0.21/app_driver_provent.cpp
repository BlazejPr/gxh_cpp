
/* 
 * File:   app_driver_provent.cpp
 * Author: Blazej Kita
 * 
 * Created on 06 czerwiec 2018, 13:05
 */

#include "app_driver_provent.h"


app_driver* get_module(gxh_scope* appHomeHandle, app_port* appPortHandle,int ih_driver_id, const char* name, const char* address, int delay_before_read_ms)
{         
    return new app_driver_provent(appHomeHandle, appPortHandle, ih_driver_id, name,address,delay_before_read_ms );
}
 
//-------------------------------------------------------------------------------


app_driver_provent::app_driver_provent(gxh_scope* appHomeHandle, app_port* appPortHandle,int ih_driver_id, const char* name, const char* address, int delay_before_read_ms ) : app_driver(appHomeHandle, appPortHandle,ih_driver_id, name,address, delay_before_read_ms )
{
    char name1[50];
    this->getName( (char*)name1 );
    
     std::string msg = std::string(name1);
     msg.append(" has been loaded..");
        
     this->getAppHandle()->show_log_driver( "app_driver_provent","app_driver_provent",msg.c_str(),0 );
}

//-------------------------------------------------------------------------------

app_driver_provent::~app_driver_provent() {  
    
    char name[50];
    this->getName( (char*)name );
    
    std::string msg = std::string(name);
    msg.append(" has been deleted");      
    
    this->getAppHandle()->show_log_driver("app_driver_provent","app_driver_provent",msg.c_str(),0);   
}


//-------------------------------------------------------------------------------

void app_driver_provent::execute(gxh_message* command)
{
    gxh_message message;
    memcpy(&message, command, sizeof(gxh_message) );
    
    
    if(message.func_internal_number == IH_UPDATE_ADDRESS)
    { 
       proventData st;
       memcpy(&st, message.dataStruct, sizeof(proventData) ); 
        
       this->getAppHandle()->show_error("app_driver_provent","execute","Cannot set address",0);            
       if(st.gxhHandle >0) //moduł oczekuje odpowiedzi....
       {                                    
                   proventData stRet; 
                   stRet.retSuccess = false; 
                   
                   gxh_message msgCommand;
                   msgCommand.system_code          = GXH_MESSAGE_REDIRECT; //przekaż bezpośrednia od gxhHandle....
                   msgCommand.gxh_handle           = st.gxhHandle;             
                   msgCommand.sizeDataStruct       = sizeof(proventData);
                   memcpy(msgCommand.dataStruct, &stRet, sizeof(proventData) );                                                    
                   this->getAppHandle()->sendGXHMessage(&msgCommand);  
       }      
       return; 
    } 
     
     
    if( message.func_internal_number == 100) //włacz wietrzenie
    {  
      proventData st;
      memcpy(&st, message.dataStruct, sizeof(proventData) );
        
      this->getAppHandle()->show_log_driver ("app_driver_provent","execute","wietrzenie",3);        
      if(st.stan == 2) this->wietrzenie(false, &st );
      if(st.stan == 1) this->wietrzenie(true, &st );
      this->timer = 1500; //przyspiesz odswiezenie informacji.
      return; 
    }  
     
     
    if( message.func_internal_number  == 101) //ustaw bieg wentylacji
    {  
      proventData st;
      memcpy(&st, message.dataStruct, sizeof(proventData) );  
      
      std::string msg ="zmien bieg wentylacji na ";
      msg.append(gxh_IntToString(st.bieg_wentylacji));
      
      this->getAppHandle()->show_log_driver("app_driver_provent","execute",msg.c_str(),3 );        
      this->bieg_wentylacji(st.bieg_wentylacji, &st ) ;
      this->timer = 1500; //przyspieszy aktualizacje rekordow..
      return; 
    }
     
     
    if(message.func_internal_number== 102)  //tryb pracy centrali...
    {  
      proventData st;
      memcpy(&st, message.dataStruct, sizeof(proventData) );    
        
      this->getAppHandle()->show_log_driver ("app_driver_provent","execute","tryb pracy centrali",3);        
      this->tryb_pracy_centrali(st.tryb_pracy_centrali, &st );
      this->timer = 1500; //przyspieszy aktualizacje rekordow..
      return; 
    } 
     
     
     if(message.func_internal_number == 103) //
    {  
      proventData st;
      memcpy(&st, message.dataStruct, sizeof(proventData) );    
         
      this->getAppHandle()->show_log_driver  ("app_driver_provent","execute","rodzaj_wentylacji",3);        
      this->rodzaj_wentylacji(st.tryb_pracy_wentylacji, &st ) ;
      this->timer = 1500; //przyspieszy aktualizacje rekordow..
      return; 
    }
     
     
     if(message.func_internal_number == 104) //
    {  
      proventData st;
      memcpy(&st, message.dataStruct, sizeof(proventData) );    
         
      this->getAppHandle()->show_log_driver ("app_driver_provent","execute","bypass",3);        
      this->bypass(st.paramInt, &st ) ;
      this->timer = 1500;//przyspieszy aktualizacje rekordow..
      return; 
    }
    
     
    if(message.func_internal_number == 105) //
    {  
      proventData st;
      memcpy(&st, message.dataStruct, sizeof(proventData) );      
        
      this->getAppHandle()->show_log_driver("app_driver_provent","execute","tryb pracy wentylacji",3);        
      this->tryb_wentylacji(st.paramInt, &st ) ;
        this->timer = 1500; //przyspieszy aktualizacje rekordow..
      return; 
    }
     
    
     
    this->getAppHandle()->show_log_driver("app_driver_provent","execute","Execute........",3);
}

//-------------------------------------------------------------------------------

bool app_driver_provent::resume()
{
    return false;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

 void app_driver_provent::tryb_wentylacji(int tryb,proventData* wskData )
 {
    char buffer[20];
    char rBuffer[20];
    
    app_modbus_rtu* rtu = new app_modbus_rtu();
    rtu->setSlaveAddress(   (char)gxh_StringToInt( this->getAddressPtr() ) );
    rtu->setDataAddress(3); 
    rtu->setRegisterValue(  (unsigned short)tryb);
    rtu->setFunctionForceSingleRegister();
    int size = rtu->get(buffer);         
    
                             
    this->getPortHandle()->flush();      
    this->getPortHandle()->writeData(buffer,size); 
    
    usleep (1000* this->getDelayBeforeRead()  );   
    
    
    int r = this->getPortHandle()->readData(rBuffer, 10);   

    proventData st; 
    st.retSuccess = false;

    if(r>0)    
    {               
        char buffTmp[40];
        
        rtu->toASCII(rBuffer, r, buffTmp);  
        this->getAppHandle()->show_log_driver ("app_driver_provent","tryb_wentylacji", (const char*)buffTmp ,0);              
        
        bool CRC16 = rtu->setResponse(rBuffer, r);                       
        if(CRC16)
        {
            st.retSuccess = true; 
        } 
    } else this->getAppHandle()->show_error("app_driver_provent","bypass","tryb_wentylacji odpowiedzi od urzadzenia",1);
    
    
    if(wskData->gxhHandle >0) //moduł oczekuje odpowiedzi....
    {                                    
                gxh_message msgCommand;
                msgCommand.system_code          = GXH_MESSAGE_REDIRECT; //przekaż bezpośrednia od gxhHandle....
                msgCommand.gxh_handle           = wskData->gxhHandle;             
                msgCommand.sizeDataStruct       = sizeof(proventData);
                memcpy(msgCommand.dataStruct, &st, sizeof(proventData) );                                                    
                this->getAppHandle()->sendGXHMessage(&msgCommand);    
    } 
    
    delete rtu; 
  
     
 }  

//------------------------------------------------------------------------------

  void app_driver_provent::bypass(int status,proventData* wskData )
 {
    char buffer[20];
    char rBuffer[20];
    
    app_modbus_rtu* rtu = new app_modbus_rtu();
    rtu->setSlaveAddress(   (char)gxh_StringToInt( this->getAddressPtr() ) );
    rtu->setDataAddress(33); 
    rtu->setRegisterValue(  (unsigned short)status);
    rtu->setFunctionForceSingleRegister();
    int size = rtu->get(buffer);         
    
                             
    this->getPortHandle()->flush();      
    this->getPortHandle()->writeData(buffer,size); 
    
    usleep (1000* this->getDelayBeforeRead()  );   
    
   
    int r = this->getPortHandle()->readData(rBuffer, 10);   

    proventData st; 
    st.retSuccess = false;

    if(r>0)    
    {               
        char buffTmp[40];
        
        rtu->toASCII(rBuffer, r, buffTmp);  
        this->getAppHandle()->show_log_driver ("app_driver_provent","bypass", (const char*)buffTmp ,0 );              
        
        bool CRC16 = rtu->setResponse(rBuffer, r);                       
        if(CRC16)
        {
            st.retSuccess = true; 
        } 
    } else this->getAppHandle()->show_error("app_driver_provent","bypass","brak odpowiedzi od urzadzenia",1);
    
    
    if(wskData->gxhHandle >0) //moduł oczekuje odpowiedzi....
    {                                    
                gxh_message msgCommand;
                msgCommand.system_code          = GXH_MESSAGE_REDIRECT; //przekaż bezpośrednia od gxhHandle....
                msgCommand.gxh_handle           = wskData->gxhHandle;             
                msgCommand.sizeDataStruct       = sizeof(proventData);
                memcpy(msgCommand.dataStruct, &st, sizeof(proventData) );                                                    
                this->getAppHandle()->sendGXHMessage(&msgCommand);    
    } 
    
    delete rtu;
    
     
 }

//------------------------------------------------------------------------------


  void app_driver_provent::rodzaj_wentylacji(int rodzaj,proventData* wskData )
 {
    char buffer[20];
    char rBuffer[20];
    
    app_modbus_rtu* rtu = new app_modbus_rtu();
    rtu->setSlaveAddress(   (char)gxh_StringToInt( this->getAddressPtr() ) );
    rtu->setDataAddress(2); 
    rtu->setRegisterValue(  (unsigned short)rodzaj);
    rtu->setFunctionForceSingleRegister();
    int size = rtu->get(buffer);         
    
                             
    this->getPortHandle()->flush();      
     this->getPortHandle()->writeData(buffer,size); 
    
    usleep (1000* this->getDelayBeforeRead()  );   
    
   
    int r =  this->getPortHandle()->readData(rBuffer, 10);   

    proventData st; 
    st.retSuccess = false;

    if(r>0)    
    {               
        char buffTmp[40];
        
        rtu->toASCII(rBuffer, r, buffTmp);  
        this->getAppHandle()->show_log_driver ("app_driver_provent","rodzaj_wentylacji", (const char*)buffTmp ,0 );              
        
        bool CRC16 = rtu->setResponse(rBuffer, r);                       
        if(CRC16)
        {
            st.retSuccess = true; 
        } 
    } else this->getAppHandle()->show_error("app_driver_provent","rodzaj_wentylacji","brak odpowiedzi od urzadzenia",1);
    
    
    if(wskData->gxhHandle >0) //moduł oczekuje odpowiedzi....
    {                                    
                gxh_message msgCommand;
                msgCommand.system_code          = GXH_MESSAGE_REDIRECT; //przekaż bezpośrednia od gxhHandle....
                msgCommand.gxh_handle           = wskData->gxhHandle;             
                msgCommand.sizeDataStruct       = sizeof(proventData);
                memcpy(msgCommand.dataStruct, &st, sizeof(proventData) );                                                    
                this->getAppHandle()->sendGXHMessage(&msgCommand);    
    }
    delete rtu;
   
 }

//------------------------------------------------------------------------------
  void app_driver_provent::tryb_pracy_centrali(int tryb,proventData* wskData )
 {
    char buffer[20];
    char rBuffer[20];
    
    app_modbus_rtu* rtu = new app_modbus_rtu();
    rtu->setSlaveAddress(   (char)gxh_StringToInt( this->getAddressPtr() ) );
    rtu->setDataAddress(1); 
    rtu->setRegisterValue(  (unsigned short)tryb);
    rtu->setFunctionForceSingleRegister();
    int size = rtu->get(buffer);         
    
                             
    this->getPortHandle()->flush();      
    this->getPortHandle()->writeData(buffer,size); 
    
    usleep (1000* this->getDelayBeforeRead()  );   
       
    int r =this->getPortHandle()->readData(rBuffer, 10);   

    proventData st; 
    st.retSuccess = false;

    if(r>0)    
    {               
        char buffTmp[40];
        
        rtu->toASCII(rBuffer, r, buffTmp);  
        this->getAppHandle()->show_log_driver ("app_driver_provent","tryb_pracy_centrali", (const char*)buffTmp ,0 );              
        
        bool CRC16 = rtu->setResponse(rBuffer, r);                       
        if(CRC16)
        {
            st.retSuccess = true; 
        } 
    } else this->getAppHandle()->show_error("app_driver_provent","tryb_pracy_centrali","brak odpowiedzi od urzadzenia",1);
    
    
    if(wskData->gxhHandle >0) //moduł oczekuje odpowiedzi....
    {                                    
                gxh_message msgCommand;
                msgCommand.system_code          = GXH_MESSAGE_REDIRECT; //przekaż bezpośrednia od gxhHandle....
                msgCommand.gxh_handle           = wskData->gxhHandle;             
                msgCommand.sizeDataStruct       = sizeof(proventData);
                memcpy(msgCommand.dataStruct, &st, sizeof(proventData) );                                                    
                this->getAppHandle()->sendGXHMessage(&msgCommand);    
    }
     
    delete rtu;
   
 }

//------------------------------------------------------------------------------

void app_driver_provent::wietrzenie(bool status,proventData* wskData )
{           
    char buffer[20];
    char rBuffer[20];
    
    
    if(status)         this->getAppHandle()->show_log_driver ("app_driver_provent","wietrzenie", "on" ,0 ); 
    if(!status)         this->getAppHandle()->show_log_driver ("app_driver_provent","wietrzenie", "off" ,0 ); 
    
    
    
    app_modbus_rtu* rtu = new app_modbus_rtu();
    rtu->setSlaveAddress(   (char)gxh_StringToInt( this->getAddressPtr() ) );
    rtu->setDataAddress(0); //adres bitu wietrzenia
    rtu->setCoilStatus(status);
    rtu->setFunctionForceSingleCoil();
    int size = rtu->get(buffer);         
    
             
    this->getPortHandle()->flush();      
    this->getPortHandle()->writeData(buffer,size); 
    
    usleep (1000* this->getDelayBeforeRead()  );   
    
 
    int r = this->getPortHandle()->readData(rBuffer, 10);   
    
    proventData st; 
    st.retSuccess = false;

    if(r>0)    
    {               
        char buffTmp[40];
        
        rtu->toASCII(rBuffer, r, buffTmp);  
        this->getAppHandle()->show_log_driver ("app_driver_provent","wietrzenie", (const char*)buffTmp ,0 );              
        
        bool CRC16 = rtu->setResponse(rBuffer, r);                       
        if(CRC16)
        {
            st.retSuccess = true; 
        } 
    } else this->getAppHandle()->show_error("app_driver_provent","wietrzenie","brak odpowiedzi od urzadzenia",1);
    
    
    if(wskData->gxhHandle >0) //moduł oczekuje odpowiedzi....
    {                                    
                gxh_message msgCommand;
                msgCommand.system_code          = GXH_MESSAGE_REDIRECT; //przekaż bezpośrednia od gxhHandle....
                msgCommand.gxh_handle           = wskData->gxhHandle;             
                msgCommand.sizeDataStruct       = sizeof(proventData);
                memcpy(msgCommand.dataStruct, &st, sizeof(proventData) );                                                    
                this->getAppHandle()->sendGXHMessage(&msgCommand);    
    }
    
    
    delete rtu;
   
}

//------------------------------------------------------------------------------

 void app_driver_provent::bieg_wentylacji(int nr_bieg,proventData* wskData )
 {
     char buffer[1024];   memset(buffer,0,1024);
     char rBuffer[1024];  memset(rBuffer,0,1024);
    
    app_modbus_rtu* rtu = new app_modbus_rtu();
    rtu->setSlaveAddress(  (char)gxh_StringToInt( this->getAddressPtr() ) );
    rtu->setDataAddress(0); 
    rtu->setRegisterValue(  (unsigned short)nr_bieg);
    rtu->setFunctionForceSingleRegister();
    int size = rtu->get(buffer);         
    
                             
    this->getPortHandle()->flush();      
    this->getPortHandle()->writeData(buffer,size); 
    
    usleep (1000* this->getDelayBeforeRead()  );   
    
    
    int r = this->getPortHandle()->readData(rBuffer, 10);   

    proventData st; 
    st.retSuccess = false;

  
    
    if(r>0)    
    {               
        char buffTmp[40];
        memset(buffTmp,0,40);
        
        rtu->toASCII(rBuffer, r, buffTmp);  
        this->getAppHandle()->show_log_driver ("app_driver_provent","bieg_wentylacji", (const char*)buffTmp ,0 );              
        
        bool CRC16 = rtu->setResponse(rBuffer, r);                       
        if(CRC16)
        {
            st.retSuccess = true; 
        }else  this->getAppHandle()->show_error("app_driver_provent","bieg_wentylacji","suma kontrolna się niezgadza",1);
    } else this->getAppHandle()->show_error("app_driver_provent","bieg_wentylacji","brak odpowiedzi od urzadzenia",1);
    
    
    if(wskData->gxhHandle >0) //moduł oczekuje odpowiedzi....
    {                                    
                gxh_message msgCommand;
                msgCommand.system_code          = GXH_MESSAGE_REDIRECT; //przekaż bezpośrednia od gxhHandle....
                msgCommand.gxh_handle           = wskData->gxhHandle;             
                msgCommand.sizeDataStruct       = sizeof(proventData);
                memcpy(msgCommand.dataStruct, &st, sizeof(proventData) );                                                    
                this->getAppHandle()->sendGXHMessage(&msgCommand);    
    }
    delete rtu;
    
     
 }

 
//------------------------------------------------------------------------------


//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
