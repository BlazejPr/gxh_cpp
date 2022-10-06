/* 
 * File:   app_module_blind.cpp
 * Author: Blazej Kita
 * 
 * Created on 14 grudzień 2016, 14:31
 */

#include "app_module_blind.h"


 
app_module* get_module(gxh_scope* appHomeHandle)
{
    return new app_module_blind(appHomeHandle); 
};
 
//-------------------------------------------------------------------------------
  
app_module_blind::app_module_blind(gxh_scope* appHomeHandle) : app_module(appHomeHandle ) 
{           
    this->getAppHandle()->show_log_module("app_module_blind","app_module_blind","start",2 );         
    
     this->ih_blind_count = 0;
     this->getAppHandle()->destroyResource(RES_VECTOR_BLIND);
    
     sqlite3* dbSqLite;               
     
     
     std::string baza = this->getAppHandle()->getDB(); // this->getAppHandle()->getRunPath();
     //baza.append("../gxh_base.db");
     
     int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
     if( r != SQLITE_OK )
     {
          this->getAppHandle()->show_error("app_module_blind","app_module_blind","Cannot open sqlLite 1",0);   
          return;
     }
     
     
    std::string query="SELECT ih_blind.id, ih_blind.name, ih_blind.ih_zone_id  ,ih_blind.open_percent, ih_blind.status_time, ih_blind.last_operation , ih_sector.ih_home_id, ih_sector.id  FROM ih_blind, ih_zone, ih_sector, ih_home WHERE ih_blind.ih_zone_id=ih_zone.id AND ih_zone.ih_sector_id=ih_sector.id AND ih_blind.is_delete='0' AND ih_zone.is_delete='0'; "; 
               
    this->getAppHandle()->show_log_data_base("app_module_blind","app_module_blind", query.c_str() ,6); 
     
    sqlite3_stmt* stmt; 
    
    const char* queryChar = query.c_str();          
    if(sqlite3_prepare(dbSqLite , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
    {
         this->getAppHandle()->show_error("app_module_blind","app_module_blind", sqlite3_errstr( sqlite3_errcode(dbSqLite  ) ),2);          
         sqlite3_finalize(stmt);        
    }else
    {    
      bool done = false;     
      while (!done)               
      { 
         int res = sqlite3_step (stmt);        
         if(res ==  SQLITE_ROW)
         {              
             this->blind[this->ih_blind_count].id                     = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 0) ) );                                                          
             
             const char* tmp1                                         = (const char*)sqlite3_column_text(stmt, 1 );
             memset(this->blind[this->ih_blind_count].name, 0, 100);
             memcpy(this->blind[this->ih_blind_count].name, tmp1, strlen(tmp1));
             
             this->blind[this->ih_blind_count].ih_zone_id             = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 2)  ) );          
             this->blind[this->ih_blind_count].open_percent           = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 3) ) );          
             this->blind[this->ih_blind_count].status_time            = gxh_StringToDouble( std::string( (const char*)sqlite3_column_text(stmt, 4) ) ) * 1000;  //sekundy zamień na milisekundy...    
             if( (const char*)sqlite3_column_text(stmt, 5) != NULL )
             {
                 const char* tmp2                                     = (const char*)sqlite3_column_text(stmt, 5);
                 memset( this->blind[this->ih_blind_count].last_operation, 0, 10 );
                 memcpy( this->blind[this->ih_blind_count].last_operation, tmp2, strlen(tmp2) );
             }
             this->blind[this->ih_blind_count].ih_home_id             = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 6) ) );  
             this->blind[this->ih_blind_count].ih_sector_id           = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 7) ) );  
             this->blind[this->ih_blind_count].toSave                 = false;                         
             this->ih_blind_count++;
         }               
         if(res == SQLITE_DONE)done = true;                                     
      }
      sqlite3_finalize(stmt);     
    }
    
    this->getAppHandle()->registerResource( this->blind ,this->ih_blind_count, RES_VECTOR_BLIND );
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    
   
     sqlite3_close_v2(dbSqLite);
}

//------------------------------------------------------------------------------

app_module_blind::~app_module_blind() 
{     
    this->getAppHandle()->show_log_module("app_module_blind","app_module_blind","stop",2);
}

//------------------------------------------------------------------------------

void app_module_blind::onClick(int ih_driver_port_id, int listener_status)
{
}

//------------------------------------------------------------------------------
 
void app_module_blind::forceCancel()
{  
      this->getAppHandle()->show_log_module("app_module_blind","app_module_blind","forceCancel",2);
      
    
      this->forceSynchronize();
      this->getAppHandle()->destroyResource(RES_VECTOR_BLIND);
}

//-------------------------------------------------------+-----------------------

void app_module_blind::forceSynchronize()
{  
     this->getAppHandle()->show_log_module("app_module_blind","forceSynchronize","start",20);   
     
     
     for(int x=0; x<this->ih_blind_count; x++)
     {
         if(this->blind[x].toSave==true)
         {
             std::string msg = "Update status for ";
             msg.append( this->blind[x].name);             
             this->getAppHandle()->show_log_module("app_module_blind","forceSynchronize",msg.c_str(),6); 
             
             //----
             sqlite3* dbSqLite;  
             
             std::string baza = this->getAppHandle()->getDB(); //this->getAppHandle()->getRunPath();
             //baza.append("../gxh_base.db");
     
             int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
             if( r != SQLITE_OK )
             {
               this->getAppHandle()->show_error("app_module_blind","forceSynchronize","Cannot open sqlLite 1",0);                 
             }else
             {                                           
                std::string query="";
                query.append("UPDATE ih_blind SET status_time=DATETIME('now','localtime'), open_percent='");
                query.append(  gxh_IntToString( this->blind[x].open_percent  ) );
                query.append("', last_operation='");
                query.append(this->blind[x].last_operation);
                query.append("' WHERE is_delete='0' AND id='");
                query.append( gxh_IntToString(this->blind[x].id) );
                query.append("';");    
                 
                 char *zErrMsg = 0;     
                 int rc = sqlite3_exec( dbSqLite , query.c_str(), 0, 0, &zErrMsg);
                 if( rc != SQLITE_OK )
                     {        
                        std::cout<<query.c_str()<<"\n";          
                        this->getAppHandle()->show_error("app_module_blind","forceSynchronize", (const char*)zErrMsg,0);
                        sqlite3_free(zErrMsg);                                
                     }else
                     {                                                     
                        this->blind[x].toSave = false;
                     }                                                   
                sqlite3_close_v2(dbSqLite);           
             }
                                     
             //----
         }
     }
     
     //  this->blind[index].last_operation = "CLICK"; 
     //  this->blind[index].open_percent = 100; 
     //  this->blind[index].toSave = true;
    this->getAppHandle()->show_log_module("app_module_blind","forceSynchronize","stop",20); 
}

//------------------------------------------------------------------------------
 
bool app_module_blind::execute(gxh_message* inMsg, gxh_message* message_ret)  
{  
     gxh_message message;
     memcpy(&message, inMsg, sizeof(message) );
    
     if(message.func_number == IH_MODULE_FUNCTION_BLIND_SWITCH_UP )     //wlacznik rolety góra
           {
              return this->goUp(&message, message_ret);
           }
     
     
     if(message.func_number == IH_MODULE_FUNCTION_BLIND_SWITCH_DOWN )     //wlacznik rolety góra
           {
              return this->goDown(&message, message_ret);
           }
     
     if(message.func_number == IH_MODULE_FUNCTION_BLIND_OPEN_OK )     //pelne otwarcie rolety
           {
                return this->setStatus(&message, message_ret,100,"MAX_OPEN");
           }
     
     if(message.func_number == IH_MODULE_FUNCTION_BLIND_CLOSE_OK )     //pelne zamkniecie rolety
           {
                return this->setStatus(&message, message_ret, 0,"MAX_CLOSE");
           }
     
     
     
     
         if(message.func_number == IH_MODULE_FUNCTION_BLIND_100_HOME)     
           {                
                return this->setBlindOn(&message, message_ret,1,1);
           }
         
         if(message.func_number == IH_MODULE_FUNCTION_BLIND_0_HOME)     
           {               
                return this->setBlindOn(&message, message_ret,1,1);
           }
         
     
     
          if(message.func_number == IH_MODULE_FUNCTION_BLIND_100_SECTOR)     
           {
                         return this->setBlindOn(&message, message_ret,2,1);
           }
         
         if(message.func_number == IH_MODULE_FUNCTION_BLIND_0_SECTOR)     
           {
                        return this->setBlindOn(&message, message_ret,2,2);
           }
     
     
     
         if(message.func_number == IH_MODULE_FUNCTION_BLIND_100_ZONE)     
           {
                        return this->setBlindOn(&message, message_ret,3,1);
           }
         
         if(message.func_number == IH_MODULE_FUNCTION_BLIND_0_ZONE)     
           {
                         return this->setBlindOn(&message, message_ret,3,2);
           }
     
    
    
    
    
          const char* response = "BLIND MODULE"; 
          gxh_message msg;
          msg.gxh_handle = message.gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;
}



bool app_module_blind::goUp(gxh_message* inMsg,gxh_message* message_ret) 
{  
    
     ih_driver_port_function functionList;
     memcpy(&functionList, inMsg->dataStruct, sizeof(ih_driver_port_function) );
    
     int index = -1;     
     for(int x=0; x<this->ih_blind_count; x++)
     {
         if(this->blind[x].id == functionList.resource_id) { index = x; break; }
     }
     
     if( index == -1) 
     {
          const char* response = "BLIND NOT FOUND"; 
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;
     }
    
     
          int countPortOut = 0;          
          ih_driver_port_function functionListOut[20];   
          
          int sizeS = 0;
          ih_driver_port_function* tmpS = this->getAppHandle()->getDriverPortFunction(&sizeS);
          
          for(int x=0; x<sizeS; x++)
          {
              if(tmpS[x].resource_id == functionList.resource_id && tmpS[x].const_function_symbol==IH_MODULE_FUNCTION_BLIND_OPEN )
              {
                  memcpy( &functionListOut[countPortOut], &tmpS[x], sizeof(ih_driver_port_function) );
                  countPortOut++;
              }
          }
          
          
          for(int p=0; p< countPortOut; p++)
          {
                 //std::string msg1 = "Wyjscie dla oswietlenia ";                               
                 //ih_lighting* lightsOut = this->getDataBaseHandle()->getLight(  functionListOut[p].resource_id );
                 //msg1.append(lightsOut[0].name);                 
                 //this->getAppHandle()->show_log("app_module_light","changeLightStatus", msg1.c_str());
                 
             bool isOk = false;
                 
                 
             if(functionListOut[p].port_typ == IH_PORT_BISTABILE_OUT_ON )                   
             {                                   
                isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id, IH_SET_PORT_ON,-1,-1,-1  );                                            
             }
              
             if(functionListOut[p].port_typ == IH_PORT_MONOSTABILE_ON_OFF_CLICK )                   
             { 
                isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id, IH_SET_PORT_CLICK ,-1,-1,-1 );                        
             }
              
              
             if(functionListOut[p].port_typ == IH_PORT_MONOSTABILE_ON_OFF_DOUBLE_CLICK )                   
             { 
                isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id, IH_SET_PORT_DOUBLE_CLICK,-1,-1,-1  );                           
             } 
              
             if(isOk)      
             {
                 this->getAppHandle()->show_log_module("app_module_blind","goUp","Port zostal ustawiony",11 );
             }
             
          }
     
        
      strcpy( this->blind[index].last_operation,"UP"); //50 jesli nie wiadomo iloe..
      this->blind[index].open_percent = 50; //50 jesli nie wiadomo iloe..
      this->blind[index].toSave = true;
     
  
     
    const char* response = "OK"; 
    gxh_message msg;
    msg.gxh_handle = inMsg->gxh_handle;
    memcpy(msg.dataStruct, response, strlen(response) );
    msg.sizeDataStruct = sizeof(response) ;
    msg.system_code = GXH_MESSAGE_SUCCESS;
            
    memcpy(message_ret, &msg, sizeof(gxh_message));            
    return true;
}


bool app_module_blind::goDown(gxh_message* inMsg,gxh_message* message_ret) 
{  
    ih_driver_port_function functionList;
     memcpy(&functionList, inMsg->dataStruct, sizeof(ih_driver_port_function) );
     
      
     int index = -1;     
     for(int x=0; x<this->ih_blind_count; x++)
     {
         if(this->blind[x].id == functionList.resource_id) { index = x; break; }
     }
     
     if( index == -1) 
     {
          const char* response = "BLIND NOT FOUND"; 
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;
     }
      
     
          int countPortOut = 0;          
          ih_driver_port_function functionListOut[20];   
          
          int sizeS = 0;
          ih_driver_port_function* tmpS = this->getAppHandle()->getDriverPortFunction(&sizeS);
          
          for(int x=0; x<sizeS; x++)
          {
              if(tmpS[x].resource_id == functionList.resource_id && tmpS[x].const_function_symbol==IH_MODULE_FUNCTION_BLIND_CLOSE )
              {
                  memcpy( &functionListOut[countPortOut], &tmpS[x], sizeof(ih_driver_port_function) );
                  countPortOut++;
              }
          }
          
          for(int p=0; p< countPortOut; p++)
          {
                 //std::string msg1 = "Wyjscie dla oswietlenia ";                               
                 //ih_lighting* lightsOut = this->getDataBaseHandle()->getLight(  functionListOut[p].resource_id );
                 //msg1.append(lightsOut[0].name);                 
                 //this->getAppHandle()->show_log("app_module_light","changeLightStatus", msg1.c_str());
                 
             bool isOk = false;
                 
                 
             if(functionListOut[p].port_typ == IH_PORT_BISTABILE_OUT_ON )                   
             {                                   
                isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id, IH_SET_PORT_ON ,-1,-1,-1 );                                            
             }
              
             if(functionListOut[p].port_typ == IH_PORT_MONOSTABILE_ON_OFF_CLICK )                   
             {
                isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id, IH_SET_PORT_CLICK ,-1,-1,-1 );                        
             }
              
              
             if(functionListOut[p].port_typ == IH_PORT_MONOSTABILE_ON_OFF_DOUBLE_CLICK )                   
             {
                isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id, IH_SET_PORT_DOUBLE_CLICK,-1,-1,-1  );                           
             } 
             
             
             if(isOk)      
             {
                 this->getAppHandle()->show_log_module("app_module_blind","goDown","Port zostal ustawiony",11 );
             }
             
              
                   
          }
     
      strcpy( this->blind[index].last_operation,"DOWN"); //50 jesli nie wiadomo iloe..
      this->blind[index].open_percent = 50; //50 jesli nie wiadomo iloe..
      this->blind[index].toSave = true;
 
    const char* response = "OK"; 
    gxh_message msg;
    msg.gxh_handle = inMsg->gxh_handle;
    memcpy(msg.dataStruct, response, strlen(response) );
    msg.sizeDataStruct = sizeof(response) ;
    msg.system_code = GXH_MESSAGE_SUCCESS;
            
    memcpy(message_ret, &msg, sizeof(gxh_message));            
    return true;
}

 

bool app_module_blind::setStatus(gxh_message* inMsg,gxh_message* message_ret, int status, std::string last_operation) 
{  
     ih_driver_port_function functionList;
     memcpy(&functionList, inMsg->dataStruct, sizeof(ih_driver_port_function) );
     
      
     
     int index = -1;     
     for(int x=0; x<this->ih_blind_count; x++)
     {
         if(this->blind[x].id == functionList.resource_id) { index = x; break; }
     }
     
     if( index == -1) 
     {
          const char* response = "BLIND NOT FOUND"; 
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;
     }
     
     
     //this->setBlindStatus(functionList.resource_id , status, last_operation);
  //   this->getAppHandle()->setBlindStatus(this->getDbSqLite(),  functionList.resource_id , status, last_operation, this->debugLevel, true);
     
     
    if(status == 100)
    {
      strcpy(this->blind[index].last_operation,"CLICK" );
      this->blind[index].open_percent = 100; 
      this->blind[index].toSave = true;
    }
    if(status == 0)
    {
      strcpy(this->blind[index].last_operation, "CLICK");
      this->blind[index].open_percent = 0; 
      this->blind[index].toSave = true;
    }
    
     
    const char* response = "OK"; 
    gxh_message msg;
    msg.gxh_handle = inMsg->gxh_handle;
    memcpy(msg.dataStruct, response, strlen(response) );
    msg.sizeDataStruct = sizeof(response) ;
    msg.system_code = GXH_MESSAGE_SUCCESS;
            
    memcpy(message_ret, &msg, sizeof(gxh_message));            
    return true;
}




//??
bool app_module_blind::setBlindOn(gxh_message* inMsg,gxh_message* message_ret, int type, int command)
  {
          
     ih_driver_port_function functionList;
     memcpy(&functionList, inMsg->dataStruct, sizeof(ih_driver_port_function) );
     
   
     
     for(int i=0; i<ih_blind_count; i++)
     {
         if(!(
                 (type == 1) || //cały dom...
                 (type == 2 && this->blind[i].ih_sector_id==functionList.resource_id ) || //wybrany sektor
                 (type == 3 && this->blind[i].ih_zone_id==functionList.resource_id )  //wybrany strefa
           ))continue;
         
       
          int countPortOut = 0;          
          ih_driver_port_function functionListOut[20];   
          
          int sizeS = 0;
          ih_driver_port_function* tmpS = this->getAppHandle()->getDriverPortFunction(&sizeS);
          
          for(int x=0; x<sizeS; x++)
          {
              if( (tmpS[x].resource_id == functionList.resource_id && tmpS[x].const_function_symbol==IH_MODULE_FUNCTION_BLIND_OPEN && command == 1) ||
                 (tmpS[x].resource_id == functionList.resource_id && tmpS[x].const_function_symbol==IH_MODULE_FUNCTION_BLIND_CLOSE && command == 2) )
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
                      isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id, IH_SET_PORT_ON ,-1,-1,-1 );                            
                   
               }  
             
               if(functionListOut[p].port_typ == IH_PORT_MONOSTABILE_ON_OFF_CLICK )                   
               {
                       isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id, IH_SET_PORT_CLICK,-1,-1,-1  );   
                                     
               }
             
               if(functionListOut[p].port_typ == IH_PORT_MONOSTABILE_ON_OFF_DOUBLE_CLICK )                   
               {
                       isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id, IH_SET_PORT_DOUBLE_CLICK,-1,-1,-1  );   
                       
               } 
               
               if(isOk)
               {}
             
          }  
                   
          
          //this->setBlindStatus(  blinds[i].id , -1, "UP");  
          //this->getAppHandle()->setBlindStatus(this->getDbSqLite(),  blinds[i].id, -1, "UP", this->debugLevel, true);
     }
     
     
 
      
      
     const char* response = "OK"; 
     gxh_message msg;
     msg.gxh_handle = inMsg->gxh_handle;
     memcpy(msg.dataStruct, response, strlen(response) );
     msg.sizeDataStruct = sizeof(response) ;
     msg.system_code = GXH_MESSAGE_SUCCESS;
            
     memcpy(message_ret, &msg, sizeof(gxh_message));            
     return true;
  }


 
 

 
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------