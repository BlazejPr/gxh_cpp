/* 
 * File:   app_module_door.cpp
 * Author: Blazej Kita
 * 
 * Created on 08 styczeń 2020, 14:31
 */

#include "app_module_door.h"


 
app_module* get_module(gxh_scope* appHomeHandle )
{
    return new app_module_door(appHomeHandle); 
};
 
//-------------------------------------------------------------------------------
  
app_module_door::app_module_door(gxh_scope* appHomeHandle) : app_module(appHomeHandle) 
{               
    this->getAppHandle()->show_log_module("app_module_door","app_module_door","start",2 );         
    
    
    
    
    
    this->door_count = 0;
    this->getAppHandle()->destroyResource(RES_VECTOR_DOOR);
    
     sqlite3* dbSqLite;               
     
     
     std::string baza = this->getAppHandle()->getDB(); //this->getAppHandle()->getRunPath();
     //baza.append("../gxh_base.db");
     
     int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
     if( r != SQLITE_OK )
     {
          this->getAppHandle()->show_error("app_module_door","app_module_door","Cannot open sqlLite 1",0);            
     }
     else 
     {
    
        std::string query="SELECT ih_doors.id, ih_doors.name, ih_doors.ih_zone_id, ih_doors.status, ih_doors.type, ih_sector.ih_home_id, ih_sector.id  FROM ih_doors, ih_zone, ih_sector WHERE ih_doors.ih_zone_id=ih_zone.id AND ih_zone.ih_sector_id=ih_sector.id AND ih_doors.is_delete='0' AND ih_zone.is_delete='0'; ";                
        this->getAppHandle()->show_log_data_base("app_module_door","app_module_door", query.c_str() ,6);  
         
        sqlite3_stmt* stmt; 
    
        const char* queryChar = query.c_str();          
        if(sqlite3_prepare(dbSqLite , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
        {
          this->getAppHandle()->show_error("app_module_door","app_module_door", sqlite3_errstr( sqlite3_errcode(dbSqLite  ) ),4);          
          sqlite3_finalize(stmt);                 
        }else
        {    
           bool done = false;       
           while (!done)               
           { 
               int res = sqlite3_step (stmt);        
               if(res ==  SQLITE_ROW)
               {                       
                  this->doors[this->door_count].id                 = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 0) ) );
                  
                  const char* tmp1                                = (const char*)sqlite3_column_text(stmt, 1 );                  
                  memset(this->doors[this->door_count].name, 0, 100 );
                  memcpy(this->doors[this->door_count].name, tmp1, strlen(tmp1));
                                   
                  
                  this->doors[this->door_count].ih_zone_id    = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 2) ) ); 
                  this->doors[this->door_count].status                = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 3) ) );
                  this->doors[this->door_count].type                = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 4) ) );
                  this->doors[this->door_count].toSave = false;
                  this->door_count++;
               }               
              if(res == SQLITE_DONE)done = true;                                     
            }
          sqlite3_finalize(stmt); 
         }
      }
    
    
     this->getAppHandle()->registerResource( this->doors ,this->door_count, RES_VECTOR_DOOR );
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
     
     
  
     sqlite3_close_v2(dbSqLite);
}

//------------------------------------------------------------------------------

app_module_door::~app_module_door() 
{     
    this->getAppHandle()->show_log_module("app_module_door","app_module_door","stop",2 );
}


//-------------------------------------------------------+-----------------------

void app_module_door::onClick(int ih_driver_port_id, int listener_status)
{
}

//-------------------------------------------------------+-----------------------

void app_module_door::forceCancel()
{  
      this->getAppHandle()->show_log_module("app_module_door","app_module_door","forceCancel",2);
      
    
      this->forceSynchronize();
      this->getAppHandle()->destroyResource(RES_VECTOR_GATE);
}

//-------------------------------------------------------+-----------------------

void app_module_door::forceSynchronize()
{  
     this->getAppHandle()->show_log_module("app_module_door","forceSynchronize","start",20);         
     
     
     for(int x=0; x<this->door_count; x++)
     {
         if(this->doors[x].toSave == true) 
          {   
              std::string msg = "Update status for ";
              msg.append( doors[x].name);             
              this->getAppHandle()->show_log_module("app_module_door","forceSynchronize",msg.c_str(),3); 
              
                         //----
                         sqlite3* dbSqLite;  

                         std::string baza = this->getAppHandle()->getDB();//this->getAppHandle()->getRunPath();
                         //baza.append("../gxh_base.db");

                         int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );      //tu coś nie gra..                            
                         if( r != SQLITE_OK )
                         {
                           this->getAppHandle()->show_error("app_module_door","forceSynchronize","Cannot open sqlLite 1",0);                 
                         }else
                         { 
 


                             std::string query="";
                             query.append("UPDATE ih_doors SET  status='");
                             query.append(  gxh_IntToString(doors[x].status)  );                            
                             query.append("' WHERE  id='");
                             query.append( gxh_IntToString(doors[x].id) );
                             query.append("'; ");



                             char *zErrMsg = 0;     
                             int rc = sqlite3_exec( dbSqLite , query.c_str(), 0, 0, &zErrMsg);
                             if( rc != SQLITE_OK )
                                 {        
                                    std::cout<<query.c_str()<<"\n";          
                                    this->getAppHandle()->show_error("app_module_door","forceSynchronize", (const char*)zErrMsg,0);
                                    sqlite3_free(zErrMsg);                                
                                    sqlite3_close_v2(dbSqLite);   
                                 }else
                                 {  
                                    doors[x].toSave = false;
                                    sqlite3_close_v2(dbSqLite);  
                                 }                                                                    
                         }
          }
     }
     
     
     this->getAppHandle()->show_log_module("app_module_door","forceSynchronize","stop",20);          
}

//-------------------------------------------------------+-----------------------


bool app_module_door::execute(gxh_message* inMsg,gxh_message* message_ret)  
{ 
    
          
      gxh_message message;
      memcpy(&message, inMsg, sizeof(gxh_message));
    
      
     
         
      if(message.func_number == IH_MODULE_FUNCTION_DOOR_OPEN )     //wymuś otwarci drzwi..
           {
          
                return this->goOpenDoor(&message, message_ret);
           }
      
   
     
        if(message.func_number == IH_MODULE_FUNCTION_DOOR_OPENED )     //potwierdzenie otwarcia drzwi..
           {
                //
                  this->getAppHandle()->show_log_module("app_module_door","execute","Potwierdzenie otwarcia drzwi",2); 
                  return this->doorOpened(&message, message_ret);
           }
      
        if(message.func_number == IH_MODULE_FUNCTION_DOOR_CLOSED )     //potwierdzenie zamkniecia drzwi..
           {
                //
                  this->getAppHandle()->show_log_module("app_module_door","execute","Potwierdzenie zamkniecia drzwi",2); 
                  return this->doorClosed(&message, message_ret);
               
           }
      
      
     
          const char* response = "GATE MODULE"; 
          gxh_message msg;
          msg.gxh_handle = message.gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;
          
}

//-----------------------------------------------------------------------------

bool app_module_door::doorOpened(gxh_message* inMsg,gxh_message* message_ret) 
{ 
       
     ih_driver_port_function functionList;
     memcpy(&functionList,inMsg->dataStruct, sizeof(ih_driver_port_function) );
     
     
     int index = -1;
     
     for(int x=0; x<this->door_count; x++)
     {
         if(this->doors[x].id == functionList.resource_id) { index = x; break; }
     }
     
     if( index == -1) 
     {
          const char* response = "DOOR NOT FOUND"; 
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;
     }
    
     std::string msg1 = "Otwarcie drzwi ";
     msg1.append(this->doors[index].name);
     
     this->getAppHandle()->show_log_module("app_module_door","doorOpened", msg1.c_str() ,2); 
     
     
       this->sendInterrrupt(IH_MODULE_FUNCTION_DOOR_ON_P, functionList.resource_id);
       
     
     this->doors[index].status = 2;
     this->doors[index].toSave = true;
               
     
     const char* response = "OK"; 
     gxh_message msg;
     msg.gxh_handle = inMsg->gxh_handle;
     memcpy(msg.dataStruct, response, strlen(response) );
     msg.sizeDataStruct = sizeof(response) ;
     msg.system_code = GXH_MESSAGE_SUCCESS;
            
     memcpy(message_ret, &msg, sizeof(gxh_message));            
     return true; 
    
}

bool app_module_door::doorClosed(gxh_message* inMsg,gxh_message* message_ret) 
{ 
       
     ih_driver_port_function functionList;
     memcpy(&functionList,inMsg->dataStruct, sizeof(ih_driver_port_function) );
     
     
     int index = -1;
     
     for(int x=0; x<this->door_count; x++)
     {
         if(this->doors[x].id == functionList.resource_id) { index = x; break; }
     }
     
     if( index == -1) 
     {
          const char* response = "DOOR NOT FOUND"; 
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;
     }
    
     std::string msg1 = "Zamkniecie drzwi ";
     msg1.append(this->doors[index].name);
     
     this->getAppHandle()->show_log_module("app_module_door","doorClosed", msg1.c_str() ,2); 
     
     
     this->doors[index].status = 1;
     this->doors[index].toSave = true;
     
     
     this->sendInterrrupt(IH_MODULE_FUNCTION_DOOR_OFF_P, functionList.resource_id);
       
     
     const char* response = "OK"; 
     gxh_message msg;
     msg.gxh_handle = inMsg->gxh_handle;
     memcpy(msg.dataStruct, response, strlen(response) );
     msg.sizeDataStruct = sizeof(response) ;
     msg.system_code = GXH_MESSAGE_SUCCESS;
            
     memcpy(message_ret, &msg, sizeof(gxh_message));            
     return true; 
     
}

//-----------------------------------------------------------------------------

bool app_module_door::goOpenDoor(gxh_message* inMsg,gxh_message* message_ret) 
{  
    
            
    
     ih_driver_port_function functionList;
     memcpy(&functionList,inMsg->dataStruct, sizeof(ih_driver_port_function) );
     
     
     int index = -1;
     
     for(int x=0; x<this->door_count; x++)
     {
         if(this->doors[x].id == functionList.resource_id) { index = x; break; }
     }
     
     if( index == -1) 
     {
          const char* response = "DOOR NOT FOUND"; 
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
              if(tmpS[x].resource_id == functionList.resource_id &&  //znajdz porty wyjscia...
                      (
                       (tmpS[x].const_function_symbol==IH_MODULE_FUNCTION_DOOR_OUT_CLICK ) ||
                       (tmpS[x].const_function_symbol==IH_MODULE_FUNCTION_DOOR_OUT_1sek ) ||
                       (tmpS[x].const_function_symbol==IH_MODULE_FUNCTION_DOOR_OUT_5sek ) ||
                       (tmpS[x].const_function_symbol==IH_MODULE_FUNCTION_DOOR_OUT_10sek )                
                      )
                      
                      )
              {
                  memcpy( &functionListOut[countPortOut], &tmpS[x], sizeof(ih_driver_port_function) );
                  countPortOut++;
              }
          }
          
          for(int p=0; p< countPortOut; p++)
          {
          
             bool isOk = false;
                 
                 
            // if(functionListOut[p].port_typ == IH_PORT_BISTABILE_OUT_ON )                   
            // {                                   
             //   isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id, IH_SET_PORT_ON ,-1,-1,-1 );                                            
             //}
              
          
             
             if(functionListOut[p].const_function_symbol == IH_MODULE_FUNCTION_DOOR_OUT_CLICK )     //klik               
             { 
                isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id , IH_SET_PORT_CLICK,-1, -1 ,-1  );                 
             }
             
              if(functionListOut[p].const_function_symbol == IH_MODULE_FUNCTION_DOOR_OUT_1sek )     //klik               
             { 
                isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id , IH_SET_PORT_CLICK,-1,1000 * 1,-1  );                 
             }
             
               if(functionListOut[p].const_function_symbol == IH_MODULE_FUNCTION_DOOR_OUT_5sek )     //klik               
             { 
                isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id , IH_SET_PORT_CLICK,-1,1000 * 5,-1  );                 
             }
              
                  if(functionListOut[p].const_function_symbol == IH_MODULE_FUNCTION_DOOR_OUT_10sek )     //klik               
             { 
                isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id , IH_SET_PORT_CLICK,-1,1000 * 10,-1  );                 
             }
              
          
             
             if(isOk)
             {
                 //
             }
              
                   
          }
     
     
   
          //this->doors[index].toSave = true;        
     
          //this->getAppHandle()->setGateStatus(this->getDbSqLite() ,functionList.resource_id, -1, "CLICK", this->debugLevel);
     
     
     
          const char* response = "OK"; 
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;   
    
}


//-----------------------------------------------------------------------------
/*
bool app_module_door::goCloseGate(gxh_message* inMsg,gxh_message* message_ret) 
{  
     ih_driver_port_function functionList;
     memcpy(&functionList,inMsg->dataStruct, sizeof(ih_driver_port_function) );
     
     
     int index = -1;
     
     for(int x=0; x<this->door_count; x++)
     {
         if(this->doors[x].id == functionList.resource_id) { index = x; break; }
     }
     
     if( index == -1) 
     {
          const char* response = "GATE NOT FOUND"; 
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
              if(tmpS[x].resource_id == functionList.resource_id && tmpS[x].const_function_symbol==IH_MODULE_FUNCTION_GATE_CLOSE_TWO_PORT )
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
                 int switch_delay_ms = 300;
                 int time_to_off_ms  = 300;
                 int delay_from_start = 0;
                 
                 if(functionListOut[p].switch_delay_ms > 0) switch_delay_ms = functionListOut[p].switch_delay_ms;
                 if(functionListOut[p].time_to_off_ms  > 0) time_to_off_ms = functionListOut[p].time_to_off_ms;
                 if(functionListOut[p].delay_from_start_ms  > 0) delay_from_start = functionListOut[p].delay_from_start_ms;
                 
               //  std::cout<<"\n\n---------"<<time_to_off_ms<<", "<< functionListOut[p].time_to_off_ms<<"\n\n";
                 
                isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id, IH_SET_PORT_CLICK, switch_delay_ms, time_to_off_ms,delay_from_start );                        
             }
              
              
             if(functionListOut[p].port_typ == IH_PORT_MONOSTABILE_ON_OFF_DOUBLE_CLICK )                   
             { 
                 int switch_delay_ms = 300;
                 int time_to_off_ms  = 300;
                 int delay_from_start = 0;
                 
                 if(functionListOut[p].switch_delay_ms > 0) switch_delay_ms = functionListOut[p].switch_delay_ms;
                 if(functionListOut[p].time_to_off_ms  > 0) time_to_off_ms = functionListOut[p].time_to_off_ms;
                 if(functionListOut[p].delay_from_start_ms  > 0) delay_from_start = functionListOut[p].delay_from_start_ms;
                 
                 isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id, IH_SET_PORT_DOUBLE_CLICK, switch_delay_ms, time_to_off_ms,delay_from_start ); 
                                          
             } 
             
             if(isOk)
             {
                 //
             }
              
                   
          }
     
     
   
          this->doors[index].toSave = true;
          this->doors[index].open_percent = 50; //?? 
     
          //this->getAppHandle()->setGateStatus(this->getDbSqLite() ,functionList.resource_id, -1, "CLICK", this->debugLevel);
     
     
     
          const char* response = "OK"; 
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;   
    
}
*/

//------------------------------------------------------------------------------


/*
bool app_module_door::goClickGate(gxh_message* inMsg,gxh_message* message_ret) 
{  
     ih_driver_port_function functionList;
     memcpy(&functionList,inMsg->dataStruct, sizeof(ih_driver_port_function) );
     
     
     int index = -1;
     
     for(int x=0; x<this->door_count; x++)
     {
         if(this->doors[x].id == functionList.resource_id) { index = x; break; }
     }
     
     if( index == -1) 
     {
          const char* response = "GATE NOT FOUND"; 
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
              if(tmpS[x].resource_id == functionList.resource_id && tmpS[x].const_function_symbol==IH_MODULE_FUNCTION_GATE_OPEN_PORT )
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
                 int switch_delay_ms = 300;
                 int time_to_off_ms  = 300;
                 int delay_from_start = 0;
                 
                 if(functionListOut[p].switch_delay_ms > 0) switch_delay_ms = functionListOut[p].switch_delay_ms;
                 if(functionListOut[p].time_to_off_ms  > 0) time_to_off_ms = functionListOut[p].time_to_off_ms;
                 if(functionListOut[p].delay_from_start_ms  > 0) delay_from_start = functionListOut[p].delay_from_start_ms;
                 
               //  std::cout<<"\n\n---------"<<time_to_off_ms<<", "<< functionListOut[p].time_to_off_ms<<"\n\n";
                 
                isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id, IH_SET_PORT_CLICK, switch_delay_ms, time_to_off_ms,delay_from_start );                        
             }
              
              
             if(functionListOut[p].port_typ == IH_PORT_MONOSTABILE_ON_OFF_DOUBLE_CLICK )                   
             { 
                 int switch_delay_ms = 300;
                 int time_to_off_ms  = 300;
                 int delay_from_start = 0;
                 
                 if(functionListOut[p].switch_delay_ms > 0) switch_delay_ms = functionListOut[p].switch_delay_ms;
                 if(functionListOut[p].time_to_off_ms  > 0) time_to_off_ms = functionListOut[p].time_to_off_ms;
                 if(functionListOut[p].delay_from_start_ms  > 0) delay_from_start = functionListOut[p].delay_from_start_ms;
                 
                 isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id, IH_SET_PORT_DOUBLE_CLICK, switch_delay_ms, time_to_off_ms,delay_from_start ); 
                                          
             } 
             
             if(isOk)
             {
                 //
             }
              
                   
          }
     
     
   
          this->doors[index].toSave = true;
          this->doors[index].open_percent = 50; //?? 
     
          //this->getAppHandle()->setGateStatus(this->getDbSqLite() ,functionList.resource_id, -1, "CLICK", this->debugLevel);
     
     
     
          const char* response = "OK"; 
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;   
    
}
*/


/*
bool app_module_door::setStatusGate(gxh_message* inMsg,gxh_message* message_ret, int status, std::string last_operation) 
{  
     ih_driver_port_function functionList;
     memcpy(&functionList, inMsg->dataStruct, sizeof(ih_driver_port_function) );
     
     
     int index = -1;
     
     for(int x=0; x<this->door_count; x++)
     {
         if(this->doors[x].id == functionList.resource_id) { index = x; break; }
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
     
     
   //  this->getAppHandle()->setGateStatus(this->getDbSqLite(), functionList.resource_id , status, last_operation, this->debugLevel);
     
     if(status == 100)  
     {
         
          this->doors[index].toSave = true;
          this->doors[index].open_percent = 100;  
     }
     
      if(status == 0)  
     {
         
          this->doors[index].toSave = true;
          this->doors[index].open_percent = 0;  
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
*/
//------------------------------------------------------------------------------

 
//------------------------------------------------------------------------------
