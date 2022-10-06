

#include "app_module.h"

app_module* get_moduleStandard(gxh_scope* appHomeHandle  )
{         
    return new app_module(appHomeHandle);
};

//------------------------------------------------------------------------------

app_module::app_module(gxh_scope* appHomeHandle)
{    
    this->appHandle      = appHomeHandle;               
    this->moduleName     = "app_module";
};

//------------------------------------------------------------------------------

app_module::~app_module()
{    
     
};

//------------------------------------------------------------------------------

gxh_scope* app_module::getAppHandle()
{
    return this->appHandle;
}

//------------------------------------------------------------------------------

void app_module::forceCancel()
{
    
}

//------------------------------------------------------------------------------

void app_module::forceSynchronize()
{
    
}

//------------------------------------------------------------------------------

bool app_module::execute(gxh_message* inMsg, gxh_message* outMsg)
{

   return false;
} 

//------------------------------------------------------------------------------
// num 100 - run apache, 101 - wylacz panel www, 200 - run ftp, 201- stop ftp, 300 - run ssh, 301 - sotp ssh
bool app_module::executeSystemConsole(gxh_message* intMsg, gxh_message* message, int num)
{
  
     
    if(num == 100)
    {
        this->getAppHandle()->exec("/etc/init.d/apache2 start");
    }
    
    if(num == 101)
    {
         this->getAppHandle()->exec("/etc/init.d/apache2 stop");
    }
    
    if(num == 200)
    {
         this->getAppHandle()->exec("/etc/init.d/vsftpd start");
    }
    
    if(num == 201)
    {
          this->getAppHandle()->exec("/etc/init.d/vsftpd stop"); 
    }
    
    if(num == 300)
    {
         this->getAppHandle()->exec("/etc/init.d/ssh start"); 
         this->getAppHandle()->exec("/etc/init.d/dropbear start"); 
    }
    
    if(num == 301)
    {
            this->getAppHandle()->exec("/etc/init.d/ssh stop");  
            this->getAppHandle()->exec("/etc/init.d/dropbear stop");  
    }
    
    
     if(num == 500)
    {
            std::string baza = this->getAppHandle()->getRunPath();
            baza.append("nohup ../update.sh 0<&- &>../update_log &");
         
            this->getAppHandle()->exec(baza.c_str());   
    }
    
    return false;
}
  
//------------------------------------------------------------------------------
 

bool app_module::setPortStatus(int ih_driver_port_id, int statusType, int switch_delay_ms, int time_to_off_ms, int delay_from_start_ms  )
{                   
             
               //uwaga zmiennej drivers nie usuwamy po jest wskaznikiem do cacheu..
               ih_driver_port driver;  
            
               
               int sizeTmp = 0;
               ih_driver_port* driverPortTmp =  this->getAppHandle()->getPortsDriver(&sizeTmp);
               
               for(int x=0; x<sizeTmp; x++)
               {
                   if(driverPortTmp[x].id == ih_driver_port_id)
                   {
                       memcpy(&driver, &driverPortTmp[x], sizeof(ih_driver_port) );
                       break;
                   }
               }
               
               if(sizeTmp == 0) 
               {
                   this->getAppHandle()->show_error("app_module","setPortStatus", "driver port not found",0);          
                   return false;
               }
               
                                                           
               int const_function_symbol = -1;
             
               int sizeTmp2 = 0;               
               ih_driver* tmpD = this->getAppHandle()->getDrivers(&sizeTmp2);
               
               for(int x=0;x<sizeTmp2; x++)
               {
                   if(tmpD[x].id == driver.ih_driver_id )
                   {
                       const_function_symbol = tmpD[x].const_function_driver;
                   }
               }
               
               if(const_function_symbol == -1) 
               {
                   this->getAppHandle()->show_error("app_module","setPortStatus", "No found dirver for the function!",0);          
                   return false;
               }
               
              
               
               ih_change_port_info portInfo; // obiekt musi byc zniczony w module lub sterowniku...
               portInfo.ih_driver_port_id = ih_driver_port_id;
             //  portInfo.status = false;                         
               portInfo.switch_delay_ms      = switch_delay_ms; //domyslnie 
               portInfo.time_to_off_ms       = time_to_off_ms; //domyslnie....
               portInfo.delay_from_start_ms  = delay_from_start_ms;
               
               if(switch_delay_ms <=0 )portInfo.switch_delay_ms      = 110; //domyslnie 
               if(time_to_off_ms <= 0 )portInfo.time_to_off_ms       = 110; //domyslnie....
               if(delay_from_start_ms <=0 )portInfo.delay_from_start_ms  = 0;
    
              
               
               gxh_message msg;
               msg.func_number  =   const_function_symbol; 
               msg.func_internal_number = statusType; //IH_SET_PORT_DOUBLE_CLICK // IH_SET_PORT_ON // itd..
              // msg.gxh_handle   = intMsg->gxh_handle; 
               msg.sizeDataStruct = sizeof(ih_change_port_info);
               msg.check_timeout = 0; //brak timeOut
               memcpy(&msg.dataStruct, &portInfo, sizeof(ih_change_port_info)   );
                                            
               gxh_message msgResponse;
               this->getAppHandle()->execute( &msg, &msgResponse); 
               
               if(msgResponse.dataStruct[0] == 'O' && msgResponse.dataStruct[1] == 'K' && msgResponse.dataStruct[2] == 0)
               {
                  
                   return true;
               }else
               {
                    this->getAppHandle()->show_error("app_module","setPortStatus", "No response from driver-module ... (ok) ",0);          
               }
              
               
              
               
    return  false; ///?????
}

//-------------------------------------------------------------------------------

void app_module::onClick(int ih_driver_port_id, int listener_status)
{
    this->getAppHandle()->show_log_module("app_module","onClick","ok",5);
}

//-------------------------------------------------------------------------------

void app_module::changePortListener(int ih_driver_port_id, int listener_status)
{    
         
     //this->getAppHandle()->show_log_module("app_module","changePortListener", " ---------------------------------------- \n",0);
     
    
     ih_driver_port driverPort;
     
     ih_driver_port* firstT;
     int tmpFirstT = 0;
     bool findPort = false;
     
     firstT = this->getAppHandle()->getPortsDriver(&tmpFirstT);
     for(int x=0; x<tmpFirstT;x++)
     {       
         if(firstT[x].id == ih_driver_port_id)
         {
             memcpy(&driverPort, &firstT[x], sizeof(ih_driver_port) );
             findPort=true;
             break;
         }
     }
     
     
     if(!findPort )
     {
           this->getAppHandle()->show_error("app_module","changePortListener", "driver port not found",0);
           return;
     }
        
   
     int functionListCout  = 0;   
     ih_driver_port_function functionList[40]; // =   this->getAppHandle()->getFunctionPorts(ih_driver_port_id,&functionListCout);    //stara wersja.. this->getDriverPortFunction(ih_driver_port_id,&functionListCout);
 
     int tmpFunc = 0;
     ih_driver_port_function* firstFunc = this->getAppHandle()->getDriverPortFunction(&tmpFunc);
     
     for(int x=0; x<tmpFunc; x++)
     {         
         if(firstFunc[x].ih_driver_port_id==ih_driver_port_id)
         {
             memcpy( &functionList[functionListCout] , &firstFunc[x], sizeof(ih_driver_port_function) );
             functionListCout++;
         }
     }
     
      
     
     if(functionListCout == 0)
     {        
         this->getAppHandle()->show_error("app_module","changePortListener", "Function not found for selected port",0);  
      //   delete[] driverPort;         
         
       //  this->getAppHandle()->setPortHistory(this->getDbSqLiteTmp(), ih_driver_port_id, listener_status, this->debugLevel, true);                         
         return;
     }
     
     
        
     char port_type[25];     
     unsigned short finded = 0;
     
     for(int op=0; op<25; op++) port_type[op] = 0;
     
    
         
         if(driverPort.reverts =='1') // Normal close
         {
               if(listener_status == IH_SWITCH_ON_CLICK)   {port_type[finded] = IH_SWITCH_BISTABILE_ON_NC; finded++;  }           
               if(listener_status == IH_SWITCH_OFF_CLICK)  {port_type[finded] = IH_SWITCH_BISTABILE_OFF_NC; finded++;     
 
               
               if(listener_status == IH_SWITCH_ON_CLICK)   {port_type[finded] = IH_SWITCH_MONOSTABILE_ON_OFF_CLICK_NC; finded++;  }}
               
               if(listener_status == IH_SWITCH_DOUBLE_CLICK){port_type[finded] = IH_SWITCH_BISTABILE_ON_NC; finded++;  } //tutaj dodane..
               if(listener_status == IH_SWITCH_DOUBLE_CLICK){port_type[finded] = IH_SWITCH_MONOSTABILE_ON_OFF_DOUBLE_CLICK_NC; finded++;  }
               
               if(listener_status == IH_SWITCH_LONG_CLICK)  {
                                                             port_type[finded] = IH_SWITCH_MONOSTABILE_ON_OFF_LONG_CLICK_NC; finded++;  
                                                             port_type[finded] = IH_SWITCH_BISTABILE_OFF_NC; finded++;    //<< long click == off button //tu zmianione z NO ->> NC
                                                            }
         }
         
         
         if(driverPort.reverts =='0') // Normal open
         {
               if(listener_status == IH_SWITCH_ON_CLICK)   {port_type[finded] = IH_SWITCH_BISTABILE_ON_NO; finded++;  }          
               if(listener_status == IH_SWITCH_OFF_CLICK)  {port_type[finded] = IH_SWITCH_BISTABILE_OFF_NO; finded++;  }
               
               
               if(listener_status == IH_SWITCH_ON_CLICK)   {port_type[finded] = IH_SWITCH_MONOSTABILE_ON_OFF_CLICK_NO; finded++;  }
               
               if(listener_status == IH_SWITCH_DOUBLE_CLICK)  {port_type[finded] = IH_SWITCH_BISTABILE_ON_NO; finded++;  } //tutau dodane..
               if(listener_status == IH_SWITCH_DOUBLE_CLICK)  {port_type[finded] = IH_SWITCH_MONOSTABILE_ON_OFF_DOUBLE_CLICK_NO; finded++;  }
               
               if(listener_status == IH_SWITCH_LONG_CLICK)  {
                                                              port_type[finded] = IH_SWITCH_MONOSTABILE_ON_OFF_LONG_CLICK_NO; finded++; 
                                                              port_type[finded] = IH_SWITCH_BISTABILE_OFF_NO; finded++;  //tu zmienione na z NC na NO
                                                            }
         }
      
      
     
     //-------------------------------------------------------------------------
     
    //ih_driver_port_function* functionListTmp = new ih_driver_port_function[50];
    ih_driver_port_function functionListTmp[50]; // = new ih_driver_port_function[50];
    int functionListIndex = 0;
    bool allSame = true;
    
     
    
     for(int w=0; w<functionListCout; w++)
     {                                         
           bool portTypeCheck = false;                          
           
           for(unsigned short t=0; t<finded; t++)
           {
              // std::cout<<"\n\n Port type: "<<functionList[w].port_typ<<" i finded: "<<port_type[t];
               
               if(functionList[w].port_typ == port_type[t]) 
               {
                   portTypeCheck = true;                                     
                   
                   std::string msg = "changePortListener, fName: ";
                   msg.append(functionList[w].fName);                      
                   msg.append(", rName: ");
                   msg.append( functionList[w].rName );
                   this->getAppHandle()->show_log_module("app_module","app_module", msg.c_str(),1);
               }
           }           
           
           if(!portTypeCheck)continue;  
           
           if(functionList[w].const_function_symbol != 100) {allSame = false; break; }
           
           functionListTmp[functionListIndex] = functionList[w];
           functionListIndex++;
     }
    
      

    
    //allSame = false;
    //-------------------------------------------------------------------------
    if(allSame && functionListIndex>1  )  //  zatrzymane dla testow..... jeden przycisk jest wlacznikem wielu świateł.... klikniecie go powinno uruchomic lub zgasic wiele swiatel...
    {
           
             gxh_message msg;
             msg.func_number  =  functionList[0].const_function_symbol;  
             msg.func_internal_number = -1;
             msg.gxh_handle   = 0; 
             msg.sizeDataStruct = sizeof(ih_driver_port_function) * functionListIndex;
             memcpy(msg.dataStruct, functionListTmp, sizeof(ih_driver_port_function) * functionListIndex  );
                                             
             
             this->getAppHandle()->execute( &msg,   NULL );                
             
             
             this->getAppHandle()->show_log_module("app_module","changePortListener","many functions for the switch!",3 );             
    }
    else
    {
     
     for(int w=0; w<functionListCout; w++)
     {                                         
           bool portTypeCheck = false;         
           
           for(unsigned short t=0; t<finded; t++)
           {
               if(functionList[w].port_typ == port_type[t]) portTypeCheck = true;       
           }           
           
           if(!portTypeCheck)continue;                             
              
         
            gxh_message msg;
            msg.func_number  =   functionList[w].const_function_symbol;  
            msg.func_internal_number = -1;
            msg.gxh_handle   = 0;
            msg.sizeDataStruct = sizeof(ih_driver_port_function);
            memcpy(msg.dataStruct, &functionList[w], sizeof(ih_driver_port_function)   );                                              
            
            this->getAppHandle()->execute( &msg,  NULL );         
            
       
          
     }     
        
    }
    
     //delete[] functionListTmp;
     //-------------------------------------------------------------------------
     
   
     for(int w=0; w<functionListCout; w++)
     {                    
                   
         for(int t=0; t<finded; t++)
           {
             if(functionList[w].port_typ == port_type[t]) 
                 
                 this->getAppHandle()->addHistory( functionList[w].const_function_symbol ,  functionList[w].resource_id,  functionList[w].name , functionList[w].text_content, -1);
           }                      
                    
     }               
         
    
    
    
     
    //--------------------------------     
    struct tmListener
    {
        int ih_driver_port_id;
        int listener_status;
    };
    
    tmListener tm; 
    tm.ih_driver_port_id  = ih_driver_port_id;
    tm.listener_status    = listener_status;
    
    gxh_message msgT;
    msgT.func_number  =  0;
    msgT.func_internal_number = 10; //onClick
    msgT.gxh_handle   = 0; 
    msgT.sizeDataStruct = sizeof(tmListener);
    memcpy(msgT.dataStruct, &tm, sizeof(tmListener)  );
    
    this->getAppHandle()->sendGXHMessage(&msgT);
    
    
 
       
}

//------------------------------------------------------------------------------

 
//------------------------------------------------------------------------------

bool app_module::changeAddressDriver(gxh_message* intMsg,int ih_driver_id, char* newAddresss, char dataBit, char parity, char stopBit, int speed)
{              
               
               int const_function_symbol = - 1;
               
               unsigned int size = 0;
               ih_driver* first = (ih_driver*)this->getAppHandle()->getResource(RES_VECTOR_DRIVER, &size);
               
               for(unsigned int x=0;x<size;x++)
               {
                   if(first[x].id == ih_driver_id)
                   {
                      const_function_symbol = first[x].const_function_driver;
                      break;
                   }
               }
               
        
               if( const_function_symbol == -1)
               {
                   this->getAppHandle()->show_error("app_module","changeAddressDriver","function number for driver not found", 0);
                   return false;
               }
                            
            
               system_data_driver_param driverParam;
               driverParam.ih_driver_id = ih_driver_id;
               driverParam.address[0] = newAddresss[0];
               driverParam.address[1] = newAddresss[1];
               driverParam.dataBit = dataBit;
               driverParam.parity  = parity;
               driverParam.stopBit = stopBit;
               driverParam.speed   = speed;
               
        
               //message for module...
               gxh_message msg;
               msg.func_number  = const_function_symbol; //go restart
               msg.func_internal_number = IH_UPDATE_ADDRESS;
               msg.gxh_handle   = intMsg->gxh_handle; 
               memcpy(&msg.dataStruct, &driverParam, sizeof(system_data_driver_param) );
                
                
               bool success = false;
               gxh_message msgResponse;
               
 
               
               if(this->getAppHandle()->execute( &msg,  &msgResponse  ))
                {
                    if(msgResponse.system_code == IH_NEW_ADDRESS_OK) success = true;
                }
                                                            
               return success;        
}

//------------------------------------------------------------------------------+

bool app_module::closeApp(gxh_message* intMsg, gxh_message* message)
{
               gxh_message msg;               
               msg.func_number  =   0;           //for core
               msg.func_internal_number = 1; //close
               msg.gxh_handle   = intMsg->gxh_handle;
               msg.check_timeout = 0;
                                                                
               this->getAppHandle()->sendGXHMessage(&msg);
               
               
               if(message != NULL)
               {
                  message->gxh_handle =intMsg->gxh_handle;
                  const char* res = "OK";
                  memcpy(message->dataStruct, res, strlen(res) );
                  message->sizeDataStruct = strlen(res);
                  message->system_code = GXH_MESSAGE_SUCCESS;
               }else return false;
               
               return true;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------+

bool app_module::reloadModules( gxh_message* intMsg,gxh_message* message)
{
               gxh_message msg;               
               msg.func_number  =   0;           //for core
               msg.func_internal_number = 20; //reload module
               msg.gxh_handle   = intMsg->gxh_handle;
               msg.check_timeout = 10;
               msg.execute_timeout = 10000;//10sek
                                                                
               this->getAppHandle()->sendGXHMessage(&msg);
               
               
              if(message != NULL)
              { 
                 message->gxh_handle = intMsg->gxh_handle;               
                 const char* res = "OK";
                 memcpy(message->dataStruct, res, strlen(res) );
                 message->sizeDataStruct = strlen(res);
                 message->system_code = GXH_MESSAGE_SUCCESS;
              }else return false;
                              
              return true;
}

//------------------------------------------------------------------------------+

//------------------------------------------------------------------------------+

bool app_module::reloadPorts( gxh_message* intMsg,gxh_message* message)
{
              gxh_message msg;               
              msg.func_number  =   0;           //for core
              msg.func_internal_number = 3; //reload ports
              msg.gxh_handle   = intMsg->gxh_handle;
              msg.check_timeout = 10;
              msg.execute_timeout = 10000;//10sek
                                                                
              this->getAppHandle()->sendGXHMessage(&msg);
               
               
              if(message != NULL)
              { 
                  message->gxh_handle = intMsg->gxh_handle;               
                  const char* res = "OK";
                  memcpy(message->dataStruct, res, strlen(res) );
                  message->sizeDataStruct = strlen(res);
                  message->system_code = GXH_MESSAGE_SUCCESS;
              }else return false;
               
              return true;
}

//------------------------------------------------------------------------------+

//------------------------------------------------------------------------------+

 

//------------------------------------------------------------------------------+

bool app_module::executeFunction(gxh_message* intMsg,gxh_message* message,int ih_sl_const_function_symbol, int resource_id)
{
        return this->executeFunction(intMsg, message,ih_sl_const_function_symbol,resource_id,-1 );      
}

//------------------------------------------------------------------------------+

 bool app_module::sendInterrrupt(int ih_sl_const_function_symbol, int resource_id)
 {
           
            time_t     now = time(0);
            struct tm *aTime = localtime(&now);
            int dayOfWeek = aTime->tm_wday; // 0 sunday
     
            int tmHour   = aTime->tm_hour ;
            int tmMinute = aTime->tm_min ;
            int tmNowMin = tmHour * 60;
            tmNowMin += tmMinute;
     
     
             unsigned int all = 0;
             ih_resource_function* function = (ih_resource_function*)this->getAppHandle()->getResource(RES_VECTOR_INTERRUPT, &all);
             
             for(unsigned int x=0; x<all; x++)
             {
                 if(function[x].in_const_function_symbol == ih_sl_const_function_symbol && function[x].in_resource_id == resource_id )
                 {
                                 this->getAppHandle()->show_log_module("app_module","sendInterrrupt","New interrupt.",4);
                         
                                      // std::cout<<"\n\n"<<function[x].time_active_from<<" -- >> "<<function[x].time_active_to<<"\n\n";
                                
                                bool canExecuteDay = false;
                                if(function[x].mon == 1 && dayOfWeek == 1  ) { canExecuteDay = true; }
                                if(function[x].tue == 1 && dayOfWeek == 2  ) { canExecuteDay = true; }
                                if(function[x].wed == 1 && dayOfWeek == 3  ) { canExecuteDay = true; }
                                if(function[x].thu == 1 && dayOfWeek == 4  ) { canExecuteDay = true; }
                                if(function[x].fri == 1 && dayOfWeek == 5  ) { canExecuteDay = true; }
                                if(function[x].sat == 1 && dayOfWeek == 6  ) { canExecuteDay = true; }
                                if(function[x].sun == 1 && dayOfWeek == 0  ) { canExecuteDay = true; }
                                
                                if(canExecuteDay == false)
                                {
                                       this->getAppHandle()->show_log_module("app_module","sendInterrrupt","New interrupt - cannot execute becouse of bad day settings!",7);
                                     continue;
                                }
                                                  
                                if(strlen(function[x].time_active_from) == 5 && strlen(function[x].time_active_to) == 5)
                                {
                                        char hour[3];
                                        char minute[3];

                                        memset(hour,0,3);
                                        memset(minute,0,3);                                                               
                                        hour[0] = function[x].time_active_from[0];
                                        hour[1] = function[x].time_active_from[1];
                                        minute[0] = function[x].time_active_from[3];
                                        minute[1] = function[x].time_active_from[4];
                                        int hFrom = gxh_StringToInt( std::string(hour) );
                                        int mFrom = gxh_StringToInt( std::string(minute) );

                                        memset(hour,0,3);
                                        memset(minute,0,3);                                                               
                                        hour[0] = function[x].time_active_to[0];
                                        hour[1] = function[x].time_active_to[1];
                                        minute[0] = function[x].time_active_to[3];
                                        minute[1] = function[x].time_active_to[4];
                                        int hTo = gxh_StringToInt( std::string(hour) );
                                        int mTo = gxh_StringToInt( std::string(minute) );
                                        
                                   
                                        
                                        bool canExecute = false;
                                        
                                        int fromTs = hFrom * 60; 
                                        fromTs += mFrom;
                                        
                                        int toTs = hTo * 60; 
                                        toTs += mTo;
                                        
                                        if( tmNowMin >= fromTs &&  tmNowMin <= toTs  )
                                        {
                                            canExecute = true;
                                        }
                                        
                                        
                                     //    std::cout<<"\n\n\nOD "<<hFrom<<":"<<mFrom<<" --- >> "<<hTo<<":"<<mTo<<"|| "<<tmNowMin<<".. "<<fromTs<<".. "<<toTs<<"\n\n";
                                        
                                        if(canExecute == false)
                                        {
                                            this->getAppHandle()->show_log_module("app_module","sendInterrrupt","New interrupt - cannot execute becouse of bad hour settings!",7);
                                           continue;
                                        }else{
                                               this->getAppHandle()->show_log_module("app_module","sendInterrrupt","New interrupt - go execute...",4);
                                        }
                                }
                                
                            
                                ih_driver_port_function pf;
                                pf.id = -1;
                                pf.ih_driver_port_id = -1;
                                pf.const_function_symbol = function[x].const_function_symbol;
                                pf.resource_id = function[x].resource_id;
                                memcpy(pf.name,  function[x].name, 200 );
                                memcpy(pf.text_content,  function[x].text_content, 500 );
                                pf.port_typ = function[x].port_typ;
                                pf.switch_delay_ms = function[x].switch_delay_ms;
                                pf.time_to_off_ms = function[x].time_to_off_ms;
                                pf.delay_from_start_ms = function[x].delay_from_start_ms;
                                pf.repeat = function[x].repeat;

                            //    std::cout<<"\n\nModule name "<<"X "<< function[x].id<<" "<< function[x].rName[0]<<" "<< function[x].rName[1] <<" |||  ";
                              //  std::cout<<"\n@@@@"<< function[x].text_content;
                                
                                gxh_message gxhMessage;
                                gxhMessage.gxh_handle = NULL_DRIVER; //don't response to me..... 
                                gxhMessage.func_number = function[x].const_function_symbol;                                 
                                gxhMessage.func_internal_number = 0;
                                memcpy(gxhMessage.dataStruct, &pf, sizeof(ih_driver_port_function) );
                                gxhMessage.sizeDataStruct =  sizeof(ih_driver_port_function) ;
                                gxhMessage.system_code = GXH_MESSAGE_INTERRUPT;
                                gxhMessage.check_timeout = 0;


                               this->getAppHandle()->sendGXHMessage(&gxhMessage); //max priority              
                        
                 }                 
             }
     
      
     
     return true;
 }

//------------------------------------------------------------------------------+

bool  app_module::executeFunction(gxh_message* intMsg,gxh_message* message, int ih_sl_const_function_symbol, int resource_id, int px_group_id)
{
               std::string msg1 ="Func: ";
               msg1.append(gxh_IntToString(ih_sl_const_function_symbol));
               msg1.append(", res: ");
               msg1.append(gxh_IntToString(resource_id));                           
               this->getAppHandle()->show_log_driver( "app_module","executeFunction",msg1.c_str(),3 );
       
               ih_driver_port_function functionList;
               functionList.resource_id = resource_id;
               
                memset(functionList.name,0,250);
               memset(functionList.text_content,0,500);
               
               functionList.name[0] = '\0';;
               functionList.text_content[0] = '\0';
              
               
               //functionList.msg_type = -1;
               //functionList.result_const_function_symbol = -1;
               //functionList.result_resource_id = -1;
              // functionList.result_name[0] = '\0';;
               //functionList.result_text_content[0] = '\0';;
            
               
               gxh_message msg;               
               msg.func_number  =   ih_sl_const_function_symbol; //go restart                 
               msg.gxh_handle   = intMsg->gxh_handle; 
               msg.sizeDataStruct = sizeof(ih_driver_port_function);
               memcpy(&msg.dataStruct, &functionList, sizeof(ih_driver_port_function)   );
                                                     
               bool ret = this->getAppHandle()->execute( &msg,  message ); 
               
          
               
              this->getAppHandle()->addHistory(ih_sl_const_function_symbol,  resource_id, (char*)" ", (char*)" ", px_group_id);
               
               return ret;
}

//------------------------------------------------------------------------------+

bool  app_module::executeFunctionParam(gxh_message* intMsg,gxh_message* message, int ih_sl_const_function_symbol, int resource_id, std::string name, std::string text_content, void* ex_tmp_struct)
{
    return this->executeFunctionParam(intMsg,message, ih_sl_const_function_symbol, resource_id,  name,  text_content,-1 , ex_tmp_struct);                
}

//------------------------------------------------------------------------------+
/*
bool  app_module::executeFunctionParamWithResult(gxh_message* intMsg,gxh_message* message, int ih_sl_const_function_symbol, int resource_id, std::string name, std::string text_content, int px_group_id, int result_const_function_symbol,int result_resource_id, std::string result_name, std::string result_text_content,  int msg_type )
{
               std::string msg1 ="Func: ";
               msg1.append(gxh_IntToString(ih_sl_const_function_symbol));
               msg1.append(", res: ");
               msg1.append(gxh_IntToString(resource_id));                           
               msg1.append(", param1: ");                           
               msg1.append( name );                                          
               msg1.append(", param2: ");                           
               msg1.append( text_content );  
               this->getAppHandle()->show_log_driver( "app_module","executeFunctionParamWithResult",msg1.c_str(),3 );  
    
    
               ih_driver_port_function functionList;
               functionList.resource_id = resource_id;
               memcpy(functionList.name, name.c_str(), strlen(name.c_str()) );                  
               memcpy(functionList.text_content, text_content.c_str(), strlen(text_content.c_str()) );                  
               functionList.msg_type = msg_type;
               functionList.result_const_function_symbol = result_const_function_symbol;
               functionList.result_resource_id = result_resource_id;
               memcpy(functionList.result_name,result_name.c_str(), strlen(result_name.c_str() ) );
               memcpy(functionList.result_text_content, result_text_content.c_str(), strlen( result_text_content.c_str() ) );
               
             
               gxh_message msg;               
               msg.func_number  =   ih_sl_const_function_symbol; //go restart                 
               msg.gxh_handle   = intMsg->gxh_handle; 
               msg.sizeDataStruct = sizeof(ih_driver_port_function);
               memcpy(&msg.dataStruct, &functionList, sizeof(ih_driver_port_function)   );
                                                     
               bool ret = this->getAppHandle()->execute( &msg,  message ); 
              
               
              this->getAppHandle()->addHistory(ih_sl_const_function_symbol,  resource_id, (char*)name.c_str(), (char*)text_content.c_str(), px_group_id);
               
               return ret;
}
*/
//------------------------------------------------------------------------------+

bool app_module::executeFunctionParam(gxh_message* intMsg,gxh_message* message, int ih_sl_const_function_symbol, int resource_id, std::string name, std::string text_content, int px_group_id, void* ex_tmp_struct)
{    
    
               std::string msg1 ="Func: ";
               msg1.append(gxh_IntToString(ih_sl_const_function_symbol));
               msg1.append(", res: ");
               msg1.append(gxh_IntToString(resource_id));                           
               msg1.append(", param1: ");                           
               msg1.append( name );                                          
               msg1.append(", param2: ");                           
               msg1.append( text_content );  
               this->getAppHandle()->show_log_driver( "app_module","executeFunctionParam",msg1.c_str(),3 );  
    
           
               ih_driver_port_function functionList;
               functionList.resource_id = resource_id;
               functionList.ex_tmp_struct = ex_tmp_struct;
               
               memset(functionList.name,0,250);
               memset(functionList.text_content,0,500);
               
               memcpy(functionList.name, name.c_str(), strlen(name.c_str()));
               memcpy(functionList.text_content, text_content.c_str(), strlen(text_content.c_str()));
               
               //functionList.msg_type = -1;
               //functionList.result_const_function_symbol = -1;
               //functionList.result_resource_id = -1;
               //functionList.result_name[0] = '\0';;
               //functionList.result_text_content[0] = '\0';;
              
               
               gxh_message msg;               
               msg.func_number  =   ih_sl_const_function_symbol;                                   
               msg.gxh_handle   = intMsg->gxh_handle; 
               msg.sizeDataStruct = sizeof(ih_driver_port_function);
               memcpy(&msg.dataStruct, &functionList, sizeof(ih_driver_port_function)   );
                                  
               
             
                   
               gxh_message msgRet;
               bool ret =   this->getAppHandle()->execute( &msg,  &msgRet ); 
            
               
               this->getAppHandle()->addHistory(ih_sl_const_function_symbol,  resource_id, (char*)name.c_str(), (char*)text_content.c_str(), px_group_id);
              
               if(message != NULL)
               {
                  memcpy(message, &msgRet, sizeof(gxh_message) );              
               }else return false;
                 
               
               
               return ret;
}

//------------------------------------------------------------------------------+

bool  app_module::executeFunctionParam(gxh_message* intMsg,gxh_message* message,int ih_sl_const_function_symbol,int ih_module_function, int resource_id, std::string name, std::string text_content, void* ex_tmp_struct )
{
               std::string msg1 ="Func: ";
               msg1.append(gxh_IntToString(ih_sl_const_function_symbol));
               msg1.append(", res: ");
               msg1.append(gxh_IntToString(resource_id));                           
               msg1.append(", param1: ");                           
               msg1.append( name );                                          
               msg1.append(", param2: ");                           
               msg1.append( text_content );  
               this->getAppHandle()->show_log_driver( "app_module","executeFunctionParam",msg1.c_str(),3 ); 
    
      
               ih_driver_port_function functionList;
               functionList.resource_id = resource_id;
               
               memset(functionList.name,0,250);
               memset(functionList.text_content,0,500);
               
               memcpy(functionList.name,name.c_str(), strlen(name.c_str()) );
               memcpy(functionList.text_content, text_content.c_str(), strlen(text_content.c_str() ) );
               
               functionList.ex_tmp_struct = ex_tmp_struct;
               
               //functionList.msg_type = -1;
               //functionList.result_const_function_symbol = -1;
               //functionList.result_resource_id = -1;
               //functionList.result_name[0] = '\0';;
               //functionList.result_text_content[0] = '\0';;
            
               gxh_message msg;               
               msg.func_number  =   ih_sl_const_function_symbol; //go restart                 
               msg.func_internal_number  =   ih_module_function; //go restart                 
               msg.gxh_handle   = intMsg->gxh_handle; 
               msg.sizeDataStruct = sizeof(ih_driver_port_function);
               memcpy(&msg.dataStruct, &functionList, sizeof(ih_driver_port_function)   );
                                                     
               bool ret = this->getAppHandle()->execute( &msg,  message ); 
               
                
              this->getAppHandle()->addHistory(ih_sl_const_function_symbol,  resource_id, (char*)name.c_str(), (char*)text_content.c_str(), -1);
               
               return ret;
}


//------------------------------------------------------------------------------+

bool  app_module::executeFunctionDriver(gxh_message* intMsg,gxh_message* message,int ih_driver_id, int ih_module_function, std::string param)
{         
               std::string msg1 ="ModFunc: ";
               msg1.append(gxh_IntToString(ih_module_function));                                  
               msg1.append(", param1: ");                           
               msg1.append( param );                                           
               this->getAppHandle()->show_log_driver( "app_module","executeFunctionDriver",msg1.c_str(),3 );
    
    
               int const_function_symbol = -1;
               
               
               
               int count = 0;
               ih_driver* first = this->getAppHandle()->getDrivers(&count);
               
               for(int x=0; x<count; x++)
               {
                   if(first[x].id == ih_driver_id ) { const_function_symbol=first[x].const_function_driver; break; }
               }
               
               if(const_function_symbol==-1)
               {
                    this->getAppHandle()->show_error("app_module","executeFunctionDriver","function number for driver not found", 0);
                    return false;
               }
               
            
           
               system_driver_command functionList; 
               functionList.ih_driver_id =  ih_driver_id;
               functionList.ih_module_function = ih_module_function;
               int size = param.length() ;
               if(size> 1024) size = 1024;
               memcpy(functionList.param, param.c_str(), size);
              
                 
               gxh_message msg;               
               msg.func_number  =   const_function_symbol;     
               msg.func_internal_number  =   ih_module_function;  
               msg.gxh_handle   = intMsg->gxh_handle; 
               msg.sizeDataStruct = sizeof(system_driver_command);
               memcpy(msg.dataStruct, &functionList, sizeof(system_driver_command)   );
                                                     
               bool ret = this->getAppHandle()->execute( &msg,  message ); 
               
               this->getAppHandle()->addHistory(const_function_symbol,  ih_driver_id,(char*)" ", (char*)" ", -1);
             
               
               return ret;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
