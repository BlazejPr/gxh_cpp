#include "app_module_light.h"


 
app_module* get_module(gxh_scope* appHomeHandle)
{
    return new app_module_light(appHomeHandle); 
};
 
//-------------------------------------------------------------------------------
  
app_module_light::app_module_light(gxh_scope* appHomeHandle) : app_module(appHomeHandle) 
{           
    this->getAppHandle()->show_log_module("app_module_light","app_module_light","start",2);  
    
    
    this->forceBreak = false; // jesli true to moduł zostanie zaraz usuniety z pamieci...
    
    //załaduj oświetlenie do bufora..
    
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    
    this->light_count = 0;
    this->getAppHandle()->destroyResource(RES_VECTOR_LIGHT);
    
     sqlite3* dbSqLite;               
     
     
     std::string baza = this->getAppHandle()->getDB(); //this->getAppHandle()->getRunPath();
     //baza.append("../gxh_base.db");
     
     int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
     if( r != SQLITE_OK )
     {
          this->getAppHandle()->show_error("app_module_light","app_module_light","Cannot open sqlLite 1",0);   
          return;
     }
     
     
    sqlite3_stmt* stmt; 
    
    std::string query="SELECT ih_lighting.id, ih_lighting.name, ih_lighting.ih_zone_id  ,ih_lighting.status, ih_lighting.status_time, ih_lighting.wat, ih_sector.ih_home_id, ih_sector.id     FROM ih_lighting, ih_zone, ih_sector, ih_home WHERE ih_lighting.ih_zone_id=ih_zone.id AND ih_zone.ih_sector_id=ih_sector.id AND ih_lighting.is_delete='0' AND ih_zone.is_delete='0'; ";    
    this->getAppHandle()->show_log_data_base("app_module_light","app_module_light", query.c_str() ,6); 
    
    const char* queryChar = query.c_str();        
    if(sqlite3_prepare(dbSqLite , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
    {
         this->getAppHandle()->show_error("app_module_light","app_module_light", sqlite3_errstr( sqlite3_errcode(dbSqLite  ) ),4);          
         sqlite3_finalize(stmt);       
         sqlite3_close_v2(dbSqLite);
         return;
    }
    
    bool done = false;
    while (!done)               
    { 
       int res = sqlite3_step (stmt);
        
         if(res ==  SQLITE_ROW)
         {                                       
             this->light[this->light_count].id                     = gxh_StringToInt( std::string(  (const char*)sqlite3_column_text(stmt, 0) ) );      
             
             const char* tmp1                                      = (const char*)sqlite3_column_text(stmt, 1); 
             memset(this->light[this->light_count].name, 0, 100);
             memcpy(this->light[this->light_count].name, tmp1, strlen(tmp1) );
             
             this->light[this->light_count].ih_zone_id             = gxh_StringToInt( std::string(  (const char*)sqlite3_column_text(stmt, 2 ) ) );          
             this->light[this->light_count].status                 = gxh_StringToChar( std::string( (const char*)sqlite3_column_text(stmt, 3) ) );          
             this->light[this->light_count].status_time            = gxh_getSecondsFrom1900();
             this->light[this->light_count].status_time_last       = gxh_getSecondsFrom1900();
             this->light[this->light_count].wat                    = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 5) ) );  //sekundy zamień na milisekundy...
             this->light[this->light_count].ih_home_id             = gxh_StringToInt( std::string(  (const char*)sqlite3_column_text(stmt, 6 ) ) );  
             this->light[this->light_count].ih_sector_id           = gxh_StringToInt( std::string(  (const char*)sqlite3_column_text(stmt, 7 ) ) );  
             this->light[this->light_count].toSave = false; // jesli true to zapisz do bazy danych..
            
            this->light_count++;
         } 
              
          if(res == SQLITE_DONE)done = true;                            
         
    }

    this->getAppHandle()->registerResource( this->light ,this->light_count, RES_VECTOR_LIGHT );
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
     
     
     sqlite3_finalize(stmt);    
     sqlite3_close_v2(dbSqLite);
}

//-------------------------------------------------------+-----------------------

app_module_light::~app_module_light() 
{     
    this->getAppHandle()->show_log_module("app_module_light","app_module_light","stop",2);
}

//-------------------------------------------------------+-----------------------

void app_module_light::onClick(int ih_driver_port_id, int listener_status)
{
    
}

//-------------------------------------------------------+-----------------------

void app_module_light::forceCancel()
{  
      this->getAppHandle()->show_log_module("app_module_light","app_module_light","forceCancel",2);
      
      this->forceBreak = true;
      usleep(1000 * 100); //100 ms na zakonczenie watku..
      
      this->forceSynchronize();
      this->getAppHandle()->destroyResource(RES_VECTOR_LIGHT);
}

//-------------------------------------------------------+-----------------------

void app_module_light::forceSynchronize()
{  
     this->getAppHandle()->show_log_module("app_module_light","forceSynchronize","start",20);          
   
     
     unsigned int countT = 0;
     ih_driver_values* value   = (ih_driver_values*)this->getAppHandle()->getResource(RES_VECTOR_DRIVER_VALUE, &countT); 
     
     int inxAny = -1;
     for(unsigned int c=0; c<countT; c++)
     {
         if( value[c].ih_driver_id == -1 &&  value[c].px_group_id == -1 && value[c].const_function == VAR_LIGHT_ON_IN_HOME )
         {
             inxAny = c;
             break;
         }
     }
     
     int isAnyOn = 0;
     
     for(int x=0; x<this->light_count; x++)
     {
         if(this->light[x].toSave==true)
         {
             std::string msg = "Update status for ";
             msg.append( this->light[x].name);             
             this->getAppHandle()->show_log_module("app_module_light","forceSynchronize",msg.c_str(),6); 
             
             //----
             sqlite3* dbSqLite;  
             
             std::string baza = this->getAppHandle()->getDB();//this->getAppHandle()->getRunPath();
             //baza.append("../gxh_base.db");
     
             int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );      //tu coś nie gra..                            
             if( r != SQLITE_OK )
             {
               this->getAppHandle()->show_error("app_module_light","forceSynchronize","Cannot open sqlLite 1",0);                 
             }else
             {                                           
                 std::string statusTMP = "0";
                 if(this->light[x].status =='1' || this->light[x].status==1) statusTMP = "1";
       
                 std::string query="";
                 query.append("UPDATE ih_lighting SET status_time=DATETIME('now','localtime'), status='");
                 query.append( statusTMP  );
                 query.append("' WHERE is_delete='0' AND id='");
                 query.append( gxh_IntToString(this->light[x].id) );
                 query.append("'; ");
                   
                 
           
                 char *zErrMsg = 0;     
                 int rc = sqlite3_exec( dbSqLite , query.c_str(), 0, 0, &zErrMsg);
                 if( rc != SQLITE_OK )
                     {        
                        std::cout<<query.c_str()<<"\n";          
                        this->getAppHandle()->show_error("app_module_light","forceSynchronize", (const char*)zErrMsg,0);
                        sqlite3_free(zErrMsg);                                
                        sqlite3_close_v2(dbSqLite);   
                     }else
                     {                                                            
                        this->light[x].toSave = false;
                        sqlite3_close_v2(dbSqLite);   
                        
                        //dodaj do histori..............
                         sqlite3* dbSqLite2;  
             
                         std::string baza2 = this->getAppHandle()->getDBTmp(); //this->getAppHandle()->getRunPath();
                         //baza2.append("../gxh_base_tmp.db");
     
                         int r2  =  sqlite3_open_v2(baza2.c_str(), &dbSqLite2, SQLITE_OPEN_READWRITE, NULL   );                                  
                         if( r2 != SQLITE_OK )
                          {
                             this->getAppHandle()->show_error("app_module_light","forceSynchronize","Cannot open sqlLite 1",0);                 
                          }else
                          {
                                 std::string query="";
                                 query.append("INSERT INTO ih_lighting_h (ih_lighting_id,status,czas_sek,wat, created_at, updated_at, is_delete ) VALUES ('");
                                 query.append(  gxh_IntToString(this->light[x].id)  );                          
                                 query.append( "','" );                          
                                 if(this->light[x].status == '1' || this->light[x].status == 1 )query.append(  "0"  ); //ustawia stan odwrotny dlatego ze podajemy długość czasu ostatenig ostanu
                                 if(this->light[x].status == '0' || this->light[x].status == 0)query.append(  "1"  );                                 
                                 query.append( "','" );  
                                 query.append(  gxh_DoubleToString(this->light[x].status_time - this->light[x].status_time_last )  );  
                                 query.append( "','" ); 
                                 query.append(  gxh_IntToString(this->light[x].wat)  ); 
                                 query.append("', DATETIME('now','localtime') ,DATETIME('now','localtime'),'0'); ");                      
                                
                                char *zErrMsg = 0;     
                                int rc = sqlite3_exec( dbSqLite2 , query.c_str(), 0, 0, &zErrMsg);
                                if( rc != SQLITE_OK )
                                {  
                                  this->getAppHandle()->show_error("app_module_light","forceSynchronize", (const char*)zErrMsg ,0);   
                                  sqlite3_free(zErrMsg);    
                                } 
                                
                               sqlite3_close_v2(dbSqLite2);   
                          }
                        
                        //dodaj do histori..............
                     }              
             }
                                     
             //----
         }
     
       
       
         
         if( (this->light[x].status == 1 || this->light[x].status == '1') )
         {
              isAnyOn++; //coś jest zapalone.. 
         }
     }
     
    // std::cout<<"\n\nilosc "<<isAnyOn;
     
     if(inxAny >= 0)
     {
        std::string g = gxh_IntToString(isAnyOn);
        
        int last = gxh_StringToInt(value[inxAny].value);
        
        if(last != isAnyOn)
        {
           memset(value[inxAny].value,0,10);
           memcpy(value[inxAny].value, g.c_str(), g.length() );
           value[inxAny].status = 1; 
        }
         
         
     }
     
     ///
      //posprzataj wpisy odnośnie histori oświetlenia, jeśli jest za dużo rekordów to je usuwamy..
     
                      sqlite3* dbSqLite2;                    
    
                      std::string baza = this->getAppHandle()->getDBTmp(); // this->getAppHandle()->getRunPath();
                      //baza.append("../gxh_base_tmp.db");
     
                      int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite2, SQLITE_OPEN_READWRITE, NULL   );                                  
                      if( r != SQLITE_OK )
                      {
                         this->getAppHandle()->show_error("app_module_light","forceSynchronize","Cannot open sqlLite 1",0);                      
                      }else
                      {
                          sqlite3_stmt* stmt; 
                          int lastId = -1;
                          std::string query="select * from ih_lighting_h ORDER BY id DESC LIMIT 1;";                            
                         
                          const char* queryChar = query.c_str();       
                          if(sqlite3_prepare(dbSqLite2 , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
                           {
                             this->getAppHandle()->show_error("app_module_light","forceSynchronize", sqlite3_errstr( sqlite3_errcode(dbSqLite2  ) ),4);                                        
                            }else
                            {
                                //--                            
                               bool done = false;     
                               while (!done)               
                                { 
                                  int res = sqlite3_step (stmt);       
                                  if(res ==  SQLITE_ROW)
                                    {        
                                         const char* text  = (const char*)sqlite3_column_text(stmt, 0);  
                                         if(text != NULL) lastId = gxh_StringToInt( std::string(text) );
                                    }               
                                    if(res == SQLITE_DONE)done = true;                                         
                                 }
                                //--
                            }                                           
                            sqlite3_finalize(stmt);                     
                            
                            
                            if(lastId > 10000)
                            {
                                std::string query = "DELETE FROM ih_lighting_h;";
                                
                                char *zErrMsg = 0;     
                                int rc = sqlite3_exec( dbSqLite2 , query.c_str(), 0, 0, &zErrMsg);
                                if( rc != SQLITE_OK )
                                   {  
                                      this->getAppHandle()->show_error("app_module_light","forceSynchronize", (const char*)zErrMsg ,0);   
                                      sqlite3_free(zErrMsg);    
                                   } 
                            }
                            
                          sqlite3_close_v2(dbSqLite2);                          
                       }  
     
     this->getAppHandle()->show_log_module("app_module_light","forceSynchronize","stop",20);
}

//-------------------------------------------------------+-----------------------

bool app_module_light::execute(gxh_message* inMsg, gxh_message* outMsg)
{  
      this->getAppHandle()->show_log_module("app_module_light","execute","execute",2);
      
      
      gxh_message message;
      memcpy(&message, inMsg, sizeof(gxh_message));
      
      
      
      if(this->forceBreak == false && message.func_number == IH_MODULE_FUNCTION_LIGHT_SWITCH &&   message.sizeDataStruct > sizeof(ih_driver_port_function) )     //wlacznik swiatla przypisany do kilku swiatel..
           {
              return this->changeLightMultiStatus(&message, outMsg);
           }
    
         if(this->forceBreak == false &&  message.func_number == IH_MODULE_FUNCTION_LIGHT_SWITCH)     //wlacznik standardowy światła
           {
             return this->changeLightStatus(&message,outMsg);
           }
         
         
         if(this->forceBreak == false &&  message.func_number == IH_MODULE_FUNCTION_LIGHT_OFF)     //wymus wylaczenie
           {
             return this->changeLightOff(&message,outMsg);
           }
         
         
         if(this->forceBreak == false &&  message.func_number == IH_MODULE_FUNCTION_LIGHT_ON)     //wymus wlaczenie
           {
            return this->changeLightOn(&message,outMsg);
           }
         
         
         
          if(this->forceBreak == false &&  message.func_number == IH_MODULE_FUNCTION_LIGHT_ON_HOME)     //zapal wszystkie światła w domu
           {               
                return this->setLightOn(&message,outMsg,1,1);
           }
         
         if(this->forceBreak == false &&  message.func_number == IH_MODULE_FUNCTION_LIGHT_OFF_HOME)     //zgaś wszystkie światła w domu
           {              
                return this->setLightOn(&message,outMsg,1,0);
           }
         
         
          if(this->forceBreak == false &&  message.func_number == IH_MODULE_FUNCTION_LIGHT_ON_SECTOR)     //zapal wszystkie światła w domu
           { 
                return this->setLightOn(&message,outMsg,2,1);
           }
         
         if(this->forceBreak == false &&  message.func_number == IH_MODULE_FUNCTION_LIGHT_OFF_SECTOR)     //zgaś wszystkie światła w domu
           {           
               return this->setLightOn(&message,outMsg,2,0);
           }
        
         
         
           if(this->forceBreak == false &&  message.func_number == IH_MODULE_FUNCTION_LIGHT_ON_ZONE)     //zapal wszystkie światła w domu
           { 
                 return this->setLightOn(&message,outMsg,3,1);
           }
         
         if(this->forceBreak == false &&  message.func_number == IH_MODULE_FUNCTION_LIGHT_OFF_ZONE)     //zgaś wszystkie światła w domu
           {             
              return this->setLightOn(&message,outMsg,3,0);
           }
         
      
          if(this->forceBreak == true) return false;
      
    
          const char* response = "LIGHT MODULE"; 
          gxh_message msg;
          msg.gxh_handle = message.gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(outMsg, &msg, sizeof(gxh_message));            
          return true;
  
}

//-------------------------------------------------------------------------------

//------------------------------------------------------------------------------    

bool app_module_light::changeLightOn(gxh_message* message, gxh_message* message_ret)
{          
     ih_driver_port_function functionList;
     memcpy(&functionList, message->dataStruct, sizeof(ih_driver_port_function) );
     
     
   
     int index = -1;
          
     for(int w=0; w<this->light_count; w++)
     {
         if(this->light[w].id == functionList.resource_id)
         {
             index = w;
             break;
         }
     }
    
     
     if( index == -1) 
     {
          const char* response = "LIGHT NOT FOUND"; 
          gxh_message msg;
          msg.gxh_handle = message->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;
     }
     
    
           int countPortOut = 0;
          //ih_driver_port_function* functionListOut = this->getDriverPortFunction( functionList.resource_id, IH_MODULE_FUNCTION_LIGHT_OUT, &countPortOut);
          ih_driver_port_function functionListOut[20];  //this->getAppHandle()->getDriverPortFunction( functionList.resource_id, IH_MODULE_FUNCTION_LIGHT_OUT, &countPortOut);
          
          int sizeS = 0;
          ih_driver_port_function* tmpS = this->getAppHandle()->getDriverPortFunction(&sizeS);
          
          for(int x=0; x<sizeS; x++)
          {
              if(tmpS[x].resource_id == functionList.resource_id && tmpS[x].const_function_symbol==IH_MODULE_FUNCTION_LIGHT_OUT )
              {
                  memcpy( &functionListOut[countPortOut], &tmpS[x], sizeof(ih_driver_port_function) );
                  countPortOut++;
              }
          }
      
     
          for(int p=0; p< countPortOut; p++)
          {
              if(this->forceBreak == true) return false;
              
               
                 
             bool isOk = false;
             
             
             if(functionListOut[p].port_typ == IH_PORT_BISTABILE_OUT_ON )                   
             {                      
                 isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id, IH_SET_PORT_ON,-1,-1,-1  );                            
                 if(isOk) 
                 {     
                     this->light[index].status = '1';
                     this->light[index].toSave = true;
                     this->light[index].status_time_last =  this->light[index].status_time;
                     this->light[index].status_time = gxh_getSecondsFrom1900();
                    
                     this->sendInterrrupt(IH_MODULE_FUNCTION_LIGHT_INTERRUPT_ON, functionList.resource_id);
                    
                 }
             }  
             
             if(functionListOut[p].port_typ == IH_PORT_MONOSTABILE_ON_OFF_CLICK )                   
             {
                     if(this->light[index].status == '0')
                     {
                       isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id, IH_SET_PORT_CLICK,-1,-1,-1  );   
                       if(isOk)
                       {    
                           this->light[index].status ='1';
                           this->light[index].toSave = true;
                           this->light[index].status_time_last =  this->light[index].status_time;
                           this->light[index].status_time = gxh_getSecondsFrom1900();
              
                           this->sendInterrrupt(IH_MODULE_FUNCTION_LIGHT_INTERRUPT_ON, functionList.resource_id);
                       }
                     }
             }
             
              if(functionListOut[p].port_typ == IH_PORT_MONOSTABILE_ON_OFF_DOUBLE_CLICK )                   
             {
                     if(this->light[index].status == '0')
                     {
                       isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id, IH_SET_PORT_DOUBLE_CLICK,-1,-1,-1  );   
                       if(isOk)
                       {    
                           this->light[index].status='1';
                           this->light[index].toSave = true;
                           this->light[index].status_time_last =  this->light[index].status_time;
                           this->light[index].status_time = gxh_getSecondsFrom1900();
                         
                           this->sendInterrrupt(IH_MODULE_FUNCTION_LIGHT_INTERRUPT_ON, functionList.resource_id);
                       }
                     }
             } 
                   
          }
 
     const char* response = "OK"; 
     gxh_message msg;
     msg.gxh_handle = message->gxh_handle;
     memcpy(msg.dataStruct, response, strlen(response) );
     msg.sizeDataStruct = sizeof(response) ;
     msg.system_code = GXH_MESSAGE_SUCCESS;
            
     memcpy(message_ret, &msg, sizeof(gxh_message));            
     return true;
}

//------------------------------------------------------------------------------      
 
bool app_module_light::changeLightOff(gxh_message* message, gxh_message* message_ret)
{          
     ih_driver_port_function functionList;
     memcpy(&functionList, message->dataStruct, sizeof(ih_driver_port_function) );
     
     
     int index = -1;
          
     for(int w=0; w<this->light_count; w++)
     {
         if(this->light[w].id == functionList.resource_id)
         {
             index = w;
             break;
         }
     }
    
     
     if( index == -1) 
     {
          const char* response = "LIGHT NOT FOUND"; 
          gxh_message msg;
          msg.gxh_handle = message->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;
     }
     
    
           int countPortOut = 0;
          //ih_driver_port_function* functionListOut = this->getDriverPortFunction( functionList.resource_id, IH_MODULE_FUNCTION_LIGHT_OUT, &countPortOut);
          ih_driver_port_function functionListOut[20];  //this->getAppHandle()->getDriverPortFunction( functionList.resource_id, IH_MODULE_FUNCTION_LIGHT_OUT, &countPortOut);
          
          int sizeS = 0;
          ih_driver_port_function* tmpS = this->getAppHandle()->getDriverPortFunction(&sizeS);
          
          for(int x=0; x<sizeS; x++)
          {
              if(tmpS[x].resource_id == functionList.resource_id && tmpS[x].const_function_symbol==IH_MODULE_FUNCTION_LIGHT_OUT )
              {
                  memcpy( &functionListOut[countPortOut], &tmpS[x], sizeof(ih_driver_port_function) );
                  countPortOut++;
              }
          }
          
          for(int p=0; p< countPortOut; p++)
          {
                 if(this->forceBreak == true) return false;
                
                 bool isOk = false;
                 
                 if(functionListOut[p].port_typ == IH_PORT_BISTABILE_OUT_ON )                   
                 { 
                       isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id, IH_SET_PORT_OFF,-1,-1,-1  );   
                       if(isOk) 
                       {
                           this->light[index].status ='0';
                           this->light[index].toSave = true;
                           this->light[index].status_time_last =  this->light[index].status_time;
                           this->light[index].status_time = gxh_getSecondsFrom1900();
                         
                            this->sendInterrrupt(IH_MODULE_FUNCTION_LIGHT_INTERRUPT_OFF, functionList.resource_id);
                       }
                 }
                 
                 
                if(functionListOut[p].port_typ == IH_PORT_MONOSTABILE_ON_OFF_CLICK )                   
                {
                    if(this->light[index].status == '1')
                    {
                       isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id, IH_SET_PORT_CLICK ,-1,-1,-1 );                         
                       if(isOk)
                       {
                           this->light[index].status = '0';
                           this->light[index].toSave = true;
                           this->light[index].status_time_last =  this->light[index].status_time;
                           this->light[index].status_time = gxh_getSecondsFrom1900();
                        
                           this->sendInterrrupt(IH_MODULE_FUNCTION_LIGHT_INTERRUPT_OFF, functionList.resource_id);
                       }
                    }
                }
                 
                 
                if(functionListOut[p].port_typ == IH_PORT_MONOSTABILE_ON_OFF_DOUBLE_CLICK )                   
                {
                    if(this->light[index].status == '1')
                    {
                       isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id, IH_SET_PORT_DOUBLE_CLICK ,-1,-1,-1 );                          
                       if(isOk)
                       {
                           this->light[index].status = '0';
                           this->light[index].toSave = true;
                           this->light[index].status_time_last =  this->light[index].status_time;
                           this->light[index].status_time = gxh_getSecondsFrom1900();
                        
                           this->sendInterrrupt(IH_MODULE_FUNCTION_LIGHT_INTERRUPT_OFF, functionList.resource_id);
                       }
                    }
                }  
                   
          }
 
 
          
    
     const char* response = "OK"; 
     gxh_message msg;
     msg.gxh_handle = message->gxh_handle;
     memcpy(msg.dataStruct, response, strlen(response) );
     msg.sizeDataStruct = sizeof(response) ;
     msg.system_code = GXH_MESSAGE_SUCCESS;
            
     memcpy(message_ret, &msg, sizeof(gxh_message));            
     return true;
}
 
//------------------------------------------------------------------------------   
 
bool app_module_light::changeLightMultiStatus(gxh_message* message, gxh_message* message_ret)
{          
 
  int numLight = (int)(message->sizeDataStruct / sizeof(ih_driver_port_function));
  
  char aktualStatusFirstLight = 'x';
  
  struct st
  {
      int resource_id;
      int status;
      int last;
      int index;
  };
  
  st stLight[20];
  int toSave = 0;
  
  for(int m=0; m<20; m++)
  {
      stLight[m].resource_id = -1;
      stLight[m].status = -1;
      stLight[m].last = -1;
  }
  
  
  for(int u=0; u<numLight; u++)
  {
      if(this->forceBreak == true) return false;
      
      ih_driver_port_function functionList;
      memcpy( &functionList, (message->dataStruct + (u*sizeof(ih_driver_port_function)) ) , sizeof(ih_driver_port_function)   );
      
      
      this->getAppHandle()->show_log_module("app_module_light","changeLightMultiStatus","ok",4);
      
         
       int index = -1;
          
       for(int w=0; w<this->light_count; w++)
       {
         if(this->light[w].id == functionList.resource_id)
         {
             index = w;
             break;
         }
      }    
     
       if( index == -1) 
       {
          const char* response = "LIGHT NOT FOUND"; 
          gxh_message msg;
          msg.gxh_handle = message->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;
       }
      
    
      
     if(aktualStatusFirstLight == 'x')
     {
         aktualStatusFirstLight = this->light[index].status;       
     }
          
          
          
          int countPortOut = 0;
          //ih_driver_port_function* functionListOut = this->getDriverPortFunction( functionList.resource_id, IH_MODULE_FUNCTION_LIGHT_OUT, &countPortOut);
          ih_driver_port_function functionListOut[20];  //this->getAppHandle()->getDriverPortFunction( functionList.resource_id, IH_MODULE_FUNCTION_LIGHT_OUT, &countPortOut);
          
          int sizeS = 0;
          ih_driver_port_function* tmpS = this->getAppHandle()->getDriverPortFunction(&sizeS);
          
          for(int x=0; x<sizeS; x++)
          {
              if(tmpS[x].resource_id == functionList.resource_id && tmpS[x].const_function_symbol==IH_MODULE_FUNCTION_LIGHT_OUT )
              {
                  memcpy( &functionListOut[countPortOut], &tmpS[x], sizeof(ih_driver_port_function) );
                  countPortOut++;
              }
          }
          
          
          for(int p=0; p< countPortOut; p++)
          {                
              bool isOk = false;
                 
                 
             if(functionListOut[p].port_typ == IH_PORT_BISTABILE_OUT_ON )                   
             {
                 if( aktualStatusFirstLight == '0')
                 {                    
                       isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id, IH_SET_PORT_ON ,-1,-1,-1 );  
                       {
                           if(isOk)// this->getDataBaseHandle()->setLightStatus( this->getDbSqLite(), functionListOut[p].resource_id,1);
                           {
                               stLight[toSave].resource_id = functionListOut[p].resource_id;
                               stLight[toSave].status = '1';
                               stLight[toSave].last = '0';
                               stLight[toSave].index = index;
                               toSave++;
                           }
                       }
                 }else
                 {  
                       isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id, IH_SET_PORT_OFF,-1,-1,-1  );   
                       if(isOk)// this->getDataBaseHandle()->setLightStatus( this->getDbSqLite(), functionListOut[p].resource_id,0);
                       {
                               stLight[toSave].resource_id = functionListOut[p].resource_id;
                               stLight[toSave].status = '0';
                               stLight[toSave].last = '1';
                               stLight[toSave].index = index;
                               toSave++;
                       }
                 }
             }
              
             if(functionListOut[p].port_typ == IH_PORT_MONOSTABILE_ON_OFF_CLICK )                   
             {
                       isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id, IH_SET_PORT_CLICK,-1,-1,-1  );   
                       if(isOk && this->light[index].status == '0' && aktualStatusFirstLight=='0') 
                       {
                           //this->getDataBaseHandle()->setLightStatus( this->getDbSqLite(), functionListOut[p].resource_id,1);
                               stLight[toSave].resource_id = functionListOut[p].resource_id;
                               stLight[toSave].status = '1';
                               stLight[toSave].last = '0';
                               stLight[toSave].index = index;
                               toSave++;
                       }
                       if(isOk && this->light[index].status == '1' && aktualStatusFirstLight=='1' ) 
                       {
                           //this->getDataBaseHandle()->setLightStatus( this->getDbSqLite(), functionListOut[p].resource_id,0);
                           stLight[toSave].resource_id = functionListOut[p].resource_id;
                           stLight[toSave].status = '0';
                           stLight[toSave].last = '1';
                           stLight[toSave].index = index;
                           toSave++;
                       }
             }
              
              
             if(functionListOut[p].port_typ == IH_PORT_MONOSTABILE_ON_OFF_DOUBLE_CLICK )                   
             {
                       isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id, IH_SET_PORT_DOUBLE_CLICK,-1,-1,-1  );   
                       if(isOk && this->light[index].status == '0' && aktualStatusFirstLight=='0' ) 
                       {
                           //this->getDataBaseHandle()->setLightStatus( this->getDbSqLite(), functionListOut[p].resource_id,1);
                           stLight[toSave].resource_id = functionListOut[p].resource_id;
                           stLight[toSave].status = '1';
                           stLight[toSave].last = '0';
                           stLight[toSave].index = index;
                           toSave++;
                       }
                       if(isOk && this->light[index].status == '1' && aktualStatusFirstLight=='1' )
                       {
                           //this->getDataBaseHandle()->setLightStatus( this->getDbSqLite(), functionListOut[p].resource_id,0);
                           stLight[toSave].resource_id = functionListOut[p].resource_id;
                           stLight[toSave].status = '1';
                           stLight[toSave].last = '1';
                           stLight[toSave].index = index;
                           toSave++;
                       }
             } 
              
                   
          }
 
 
       
       
  }
  
  for(int m=0; m<toSave; m++)
  {  
        // this->getAppHandle()->setLightStatus( this->getDbSqLite(),  stLight[m].resource_id,    stLight[m].status, this->debugLevel,true  );         
        this->light[  stLight[m].index ].status =   stLight[m].status;
        this->light[stLight[m].index].toSave = true;
        this->light[stLight[m].index].status_time_last =  this->light[stLight[m].index].status_time;
        this->light[stLight[m].index].status_time = gxh_getSecondsFrom1900();
       
       
         if(stLight[m].last == '0' && stLight[m].status == '1')
         { 
              //this->addHistory(100,  stLight[m].resource_id, -1, "", "", "0", "1", "" );
            //  this->getAppHandle()->addHistory(this->getDbSqLiteTmp(),100, stLight[m].resource_id, -1,"", "","0","1", "", this->debugLevel, true ); 
         }
        
          if(stLight[m].last == '1' && stLight[m].status == '0')
         { 
             // this->addHistory(100,  stLight[m].resource_id, -1, "", "", "1", "0", "" );
            //  this->getAppHandle()->addHistory(this->getDbSqLiteTmp(),100, stLight[m].resource_id, -1,"", "","1","0", "", this->debugLevel,true );
         }
  }
  
  
  
    
          const char* response = "OK"; 
          gxh_message msg;
          msg.gxh_handle = message->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;
}
 

//-------------------------------------------------------------------------------

bool app_module_light::changeLightStatus(gxh_message* msgIn, gxh_message* message_ret)
{             
     ih_driver_port_function functionList;
     memcpy(&functionList, msgIn->dataStruct, sizeof(ih_driver_port_function) );
           
     
     int index = -1;
          
     for(int w=0; w<this->light_count; w++)
     {
         if(this->light[w].id == functionList.resource_id)
         {
             index = w;
             break;
         }
     }
    
     
     if( index == -1) 
     {
          const char* response = "LIGHT NOT FOUND"; 
          gxh_message msg;
          msg.gxh_handle = msgIn->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;
     }
     
          
          int countPortOut = 0;
          //ih_driver_port_function* functionListOut = this->getDriverPortFunction( functionList.resource_id, IH_MODULE_FUNCTION_LIGHT_OUT, &countPortOut);
          ih_driver_port_function functionListOut[20];  //this->getAppHandle()->getDriverPortFunction( functionList.resource_id, IH_MODULE_FUNCTION_LIGHT_OUT, &countPortOut);
          
          int sizeS = 0;
          ih_driver_port_function* tmpS = this->getAppHandle()->getDriverPortFunction(&sizeS);
          
          for(int x=0; x<sizeS; x++)
          {
              if(tmpS[x].resource_id == functionList.resource_id && tmpS[x].const_function_symbol==IH_MODULE_FUNCTION_LIGHT_OUT )
              {
                  memcpy( &functionListOut[countPortOut], &tmpS[x], sizeof(ih_driver_port_function) );
                  countPortOut++;
              }
          }
         
          
          
          for(int p=0; p< countPortOut; p++)
          {    
             if(this->forceBreak == true) return false; 
              
             bool isOk = false;
                 
                 
             if(functionListOut[p].port_typ == IH_PORT_BISTABILE_OUT_ON )                   
             {  
                 if(this->light[index].status == '0')
                 {                    
                       isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id, IH_SET_PORT_ON,-1,-1,-1  );                            
                       if(isOk) 
                       {
                           this->light[index].status = '1';
                           this->light[index].toSave = true;
                           this->light[index].status_time_last =  this->light[index].status_time;
                           this->light[index].status_time = gxh_getSecondsFrom1900();
                           
                           this->sendInterrrupt(IH_MODULE_FUNCTION_LIGHT_INTERRUPT_ON,  this->light[index].id );
                       }
                 }else
                 {  
                       isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id, IH_SET_PORT_OFF,-1,-1,-1  );   
                       if(isOk) 
                       {
                           this->light[index].status = '0';
                           this->light[index].toSave = true;
                           this->light[index].status_time_last =  this->light[index].status_time;
                           this->light[index].status_time = gxh_getSecondsFrom1900();
                        
                           this->sendInterrrupt(IH_MODULE_FUNCTION_LIGHT_INTERRUPT_OFF,  this->light[index].id );
                           
                       }
                 }
             }
              
             if(functionListOut[p].port_typ == IH_PORT_MONOSTABILE_ON_OFF_CLICK )                   
             { 
                       isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id, IH_SET_PORT_CLICK ,-1,-1,-1 );   
                       
                    //   std::cout<<"\n\n\nON OF CLICK "<<isOk<<" status "<<(int)this->light[index].status;
                       
                       if(isOk && this->light[index].status == '0')
                       {
                           this->light[index].status = '1';   
                           this->light[index].toSave = true;
                           this->light[index].status_time_last =  this->light[index].status_time;
                           this->light[index].status_time = gxh_getSecondsFrom1900();
                         
                           this->sendInterrrupt(IH_MODULE_FUNCTION_LIGHT_INTERRUPT_ON,  this->light[index].id );
                           
                       }else
                       if(isOk && this->light[index].status == '1') 
                       {
                           this->light[index].status = '0';  
                           this->light[index].toSave = true;
                           this->light[index].status_time_last =  this->light[index].status_time;
                           this->light[index].status_time = gxh_getSecondsFrom1900();
                       
                           this->sendInterrrupt(IH_MODULE_FUNCTION_LIGHT_INTERRUPT_OFF,  this->light[index].id );
                       }
             }
              
              
             if(functionListOut[p].port_typ == IH_PORT_MONOSTABILE_ON_OFF_DOUBLE_CLICK )                   
             {
                       isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id, IH_SET_PORT_DOUBLE_CLICK ,-1,-1,-1 );   
                       
                      
                       
                       if(isOk && this->light[index].status == '0')
                       {
                           this->light[index].status = '1';
                           this->light[index].toSave = true;
                           this->light[index].status_time_last =  this->light[index].status_time;
                           this->light[index].status_time = gxh_getSecondsFrom1900();
                        
                           this->sendInterrrupt(IH_MODULE_FUNCTION_LIGHT_INTERRUPT_ON,  this->light[index].id );
                       }else
                       if(isOk && this->light[index].status == '1') 
                       {
                           this->light[index].status = '0';
                           this->light[index].toSave = true;
                           this->light[index].status_time_last =  this->light[index].status_time;
                           this->light[index].status_time = gxh_getSecondsFrom1900();
                           
                           this->sendInterrrupt(IH_MODULE_FUNCTION_LIGHT_INTERRUPT_OFF,  this->light[index].id );
                       }
             }                                  
          }          
     
       
              
     const char* response = "OK"; 
     gxh_message msg;
     msg.gxh_handle = msgIn->gxh_handle;
     memcpy(msg.dataStruct, response, strlen(response) );
     msg.sizeDataStruct = sizeof(response) ;
     msg.system_code = GXH_MESSAGE_SUCCESS;
            
     memcpy(message_ret, &msg, sizeof(gxh_message));            
     return true;
     
}

//-------------------------------------------------------------------------------

//------------------------------------------------------------------------------    
 
bool app_module_light::setLightOn(gxh_message* msgIn, gxh_message* message_ret, int type, int command) //3 0 
  {
    
     ih_driver_port_function functionList;
     memcpy(&functionList, msgIn->dataStruct, sizeof(ih_driver_port_function) );
    
          
 //    int countLight = 0;
  //   ih_lighting lights[MAX_LIGHT_BUFFER];
     
      // this->getAppHandle()->getLightIn(this->getDbSqLite(),functionList.resource_id,type,lights, &countLight, this->debugLevel,true );
     
     /*
     for(int x=0; x<this->light_count; x++)
     {
         if(
                 (type == 1) || //cały dom...
                 (type == 2 && this->light[x].ih_sector_id==functionList.resource_id ) || //wybrany sektor
                 (type == 3 && this->light[x].ih_zone_id==functionList.resource_id )  //wybrany strefa
           )
           {
             memcpy(&lights[countLight], &this->light[x], sizeof(ih_lighting));              
             lights[countLight].index = x;
             countLight++;
           }
     }
     */
     //std::cout<<"\nsetLightOn, "<<countLight<<" | "<<type<<" | "<<command<<" |";
     
     
     for(int i=0; i<light_count; i++)
     {
          if(this->forceBreak == true) return false;
          
          if( !(
                 (type == 1) || //cały dom...
                 (type == 2 && this->light[i].ih_sector_id==functionList.resource_id ) || //wybrany sektor
                 (type == 3 && this->light[i].ih_zone_id==functionList.resource_id )  //wybrany strefa
              )
           )continue;
         
         
           int countPortOut = 0;
          //ih_driver_port_function* functionListOut = this->getDriverPortFunction( functionList.resource_id, IH_MODULE_FUNCTION_LIGHT_OUT, &countPortOut);
          ih_driver_port_function functionListOut[20];  //this->getAppHandle()->getDriverPortFunction( functionList.resource_id, IH_MODULE_FUNCTION_LIGHT_OUT, &countPortOut);
          
          int sizeS = 0;
          ih_driver_port_function* tmpS = this->getAppHandle()->getDriverPortFunction(&sizeS);
          
          for(int x=0; x<sizeS; x++)
          {
              if(tmpS[x].resource_id == this->light[i].id && tmpS[x].const_function_symbol==IH_MODULE_FUNCTION_LIGHT_OUT )
              {
                  memcpy( &functionListOut[countPortOut], &tmpS[x], sizeof(ih_driver_port_function) );
                  countPortOut++;
              }
          }
          
         //  std::cout<<"\nsetLightOn PPPPP , "<<countPortOut<<" | "<<type<<" | "<<command<<" |";
          
         
          for(int p=0; p< countPortOut; p++)
          {
            if(command ==1 )
            {
               bool isOk = false;             
             
               if(functionListOut[p].port_typ == IH_PORT_BISTABILE_OUT_ON )                   
               {      
                   if(this->light[i].status == '0') 
                   {
                      isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id, IH_SET_PORT_ON,-1,-1,-1  );                            
                      if(isOk) 
                      {
                         this->light[i].status = '1';
                         this->light[i].toSave = true;
                         this->light[i].status_time_last =  this->light[i].status_time;
                         this->light[i].status_time = gxh_getSecondsFrom1900();
                        //  this->getAppHandle()->setLightStatus( this->getDbSqLite(), lights[i].id,'1', this->debugLevel, true );                  
                          //this->addHistory(functionListOut[p].const_function_symbol, functionListOut[p].resource_id, -1, "", "", "0", "1", "" );
                          // this->getAppHandle()->addHistory(this->getDbSqLiteTmp(),functionListOut[p].const_function_symbol,functionListOut[p].resource_id, -1,"", "","0","1", "", this->debugLevel,true );
                      }
                   }
               }  
             
               if(functionListOut[p].port_typ == IH_PORT_MONOSTABILE_ON_OFF_CLICK )                   
               {
                   if(this->light[i].status == '0') 
                   {
                       isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id, IH_SET_PORT_CLICK ,-1,-1,-1 );   
                       if(isOk)
                       {
                           this->light[i].status  = '1';
                           this->light[i].toSave = true;
                           this->light[i].status_time_last =  this->light[i].status_time;
                           this->light[i].status_time = gxh_getSecondsFrom1900();
                          // this->getAppHandle()->setLightStatus( this->getDbSqLite(), lights[i].id,'1', this->debugLevel, true );                           
                           //this->addHistory(functionListOut[p].const_function_symbol, functionListOut[p].resource_id, -1, "", "", "0", "1", "" );
                          //  this->getAppHandle()->addHistory(this->getDbSqLiteTmp(),functionListOut[p].const_function_symbol,functionListOut[p].resource_id, -1,"", "","0","1", "", this->debugLevel,true );
                       }
                   }
               }
             
               if(functionListOut[p].port_typ == IH_PORT_MONOSTABILE_ON_OFF_DOUBLE_CLICK )                   
               {
                   if(this->light[i].status == '0') 
                   {
                       isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id, IH_SET_PORT_DOUBLE_CLICK ,-1,-1,-1 );   
                       if(isOk) 
                       {
                            this->light[i].status  = '1';
                            this->light[i].toSave = true;
                            this->light[i].status_time_last =  this->light[i].status_time;
                            this->light[i].status_time = gxh_getSecondsFrom1900();
                         //  this->getAppHandle()->setLightStatus( this->getDbSqLite(),  lights[i].id,'1', this->debugLevel, true );     
                          //this->addHistory(functionListOut[p].const_function_symbol, functionListOut[p].resource_id, -1, "", "", "0", "1", "" );
                         //  this->getAppHandle()->addHistory(this->getDbSqLiteTmp(),functionListOut[p].const_function_symbol,functionListOut[p].resource_id, -1,"", "","0","1", "", this->debugLevel,true );
                       }
                   }
               } 
             
          }//on 
            
            
            if(command ==0  )//off
            {
              //  std::cout<<"\nlight status:"<<lights[i].status <<"| ";
                
                 bool isOk = false;             
             
               if(functionListOut[p].port_typ == IH_PORT_BISTABILE_OUT_ON )                   
               {                
                   if(this->light[i].status == '1') 
                   {
                      isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id, IH_SET_PORT_OFF,-1,-1,-1  );                            
                      if(isOk) 
                      {
                         this->light[i].status  = '0';
                         this->light[i].toSave = true;
                         this->light[i].status_time_last =  this->light[i].status_time;
                         this->light[i].status_time = gxh_getSecondsFrom1900();
                         // this->getAppHandle()->setLightStatus( this->getDbSqLite(), lights[i].id,'0', this->debugLevel, true );                      
                         // this->addHistory(functionListOut[p].const_function_symbol, functionListOut[p].resource_id, -1, "", "", "1", "0", "" );
                       //   this->getAppHandle()->addHistory(this->getDbSqLiteTmp(),functionListOut[p].const_function_symbol,functionListOut[p].resource_id, -1,"", "","1","0", "", this->debugLevel,true );
                      }
                   }
               }  
             
               if(functionListOut[p].port_typ == IH_PORT_MONOSTABILE_ON_OFF_CLICK )                   
               {
                   if(this->light[i].status == '1') 
                   {
                       isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id, IH_SET_PORT_CLICK,-1,-1,-1  );   
                       if(isOk) 
                       {
                            this->light[i].status  = '0';
                            this->light[i].toSave = true;
                            this->light[i].status_time_last =  this->light[i].status_time;
                            this->light[i].status_time = gxh_getSecondsFrom1900();
                          // this->getAppHandle()->setLightStatus( this->getDbSqLite(), lights[i].id,'0', this->debugLevel, true );                     
                           //this->addHistory(functionListOut[p].const_function_symbol, functionListOut[p].resource_id, -1, "", "", "1", "0", "" );
                          // this->getAppHandle()->addHistory(this->getDbSqLiteTmp(),functionListOut[p].const_function_symbol,functionListOut[p].resource_id, -1,"", "","1","0", "", this->debugLevel,true );
                       }
                   }
               }
             
               if(functionListOut[p].port_typ == IH_PORT_MONOSTABILE_ON_OFF_DOUBLE_CLICK )                   
               {
                   if(this->light[i].status == '1') 
                   {
                       isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id, IH_SET_PORT_DOUBLE_CLICK,-1,-1,-1  );   
                       if(isOk)
                       {
                           this->light[i].status  = '0';
                           this->light[i].toSave = true;
                           this->light[i].status_time_last =  this->light[i].status_time;
                           this->light[i].status_time = gxh_getSecondsFrom1900();
                          // this->getAppHandle()->setLightStatus( this->getDbSqLite(),  lights[i].id,'0', this->debugLevel, true );                        
                          // this->addHistory(functionListOut[p].const_function_symbol, functionListOut[p].resource_id, -1, "", "", "1", "0", "" );
                          // this->getAppHandle()->addHistory(this->getDbSqLiteTmp(),functionListOut[p].const_function_symbol,functionListOut[p].resource_id, -1,"", "","1","0", "", this->debugLevel,true );
                       }
                   }
               } 
            }
            
               
          }  
             
    
     }
          
     
      
      
     const char* response = "OK"; 
     gxh_message msg;
     msg.gxh_handle = msgIn->gxh_handle;
     memcpy(msg.dataStruct, response, strlen(response) );
     msg.sizeDataStruct = sizeof(response) ;
     msg.system_code = GXH_MESSAGE_SUCCESS;
            
     memcpy(message_ret, &msg, sizeof(gxh_message));            
     return true;
  }

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------