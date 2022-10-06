/* 
 * File:   app_module_gate.cpp
 * Author: Blazej Kita
 * 
 * Created on 14 grudzień 2016, 14:31
 */

#include "app_module_gate.h"


 
app_module* get_module(gxh_scope* appHomeHandle )
{
    return new app_module_gate(appHomeHandle); 
};
 
//-------------------------------------------------------------------------------
  
app_module_gate::app_module_gate(gxh_scope* appHomeHandle) : app_module(appHomeHandle) 
{           
    this->getAppHandle()->show_log_module("app_module_gate","app_module_gate","start",2 );         
    
    this->gate_count = 0;
    this->getAppHandle()->destroyResource(RES_VECTOR_GATE);
    
     sqlite3* dbSqLite;               
     
     
     std::string baza = this->getAppHandle()->getDB(); //this->getAppHandle()->getRunPath();
     //baza.append("../gxh_base.db");
     
     int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
     if( r != SQLITE_OK )
     {
          this->getAppHandle()->show_error("app_module_gate","app_module_gate","Cannot open sqlLite 1",0);            
     }
     else 
     {
    
        std::string query="SELECT ih_gate.id, ih_gate.name, ih_gate.from_ih_zone_id, ih_gate.open_percent, ih_gate.status_time, ih_sector.ih_home_id, ih_sector.id, ih_gate.type  FROM ih_gate, ih_zone, ih_sector WHERE ih_gate.from_ih_zone_id=ih_zone.id AND ih_zone.ih_sector_id=ih_sector.id AND ih_gate.is_delete='0' AND ih_zone.is_delete='0'; ";                
        this->getAppHandle()->show_log_data_base("app_module_gate","app_module_gate", query.c_str() ,6);  
         
        sqlite3_stmt* stmt; 
    
        const char* queryChar = query.c_str();          
        if(sqlite3_prepare(dbSqLite , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
        {
          this->getAppHandle()->show_error("app_module_gate","app_module_gate", sqlite3_errstr( sqlite3_errcode(dbSqLite  ) ),4);          
          sqlite3_finalize(stmt);                 
        }else
        {    
           bool done = false;       
           while (!done)               
           { 
               int res = sqlite3_step (stmt);        
               if(res ==  SQLITE_ROW)
               {                       
                  this->gate[this->gate_count].id                 = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 0) ) );
                  
                  const char* tmp1                                = (const char*)sqlite3_column_text(stmt, 1 );
                  memset(this->gate[this->gate_count].name, 0, 100 );
                  memcpy(this->gate[this->gate_count].name, tmp1, strlen(tmp1));
                  
                  
                  
                  this->gate[this->gate_count].from_ih_zone_id    = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 2) ) );
                  this->gate[this->gate_count].open_percent       = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 3) ) );
                  this->gate[this->gate_count].status_time        = gxh_StringToDouble( std::string( (const char*)sqlite3_column_text(stmt, 4) ) );                         
                  this->gate[this->gate_count].ih_home_id         = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 5) ) );
                  this->gate[this->gate_count].ih_sector_id       = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 6) ) );
                  this->gate[this->gate_count].typ                = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 7) ) );
                  this->gate[this->gate_count].toSave = false;
                  this->gate_count++;
               }               
              if(res == SQLITE_DONE)done = true;                                     
            }
          sqlite3_finalize(stmt); 
         }
      }
    
    
     this->getAppHandle()->registerResource( this->gate ,this->gate_count, RES_VECTOR_GATE );
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
     
     
  
     sqlite3_close_v2(dbSqLite);
}

//------------------------------------------------------------------------------

app_module_gate::~app_module_gate() 
{     
    this->getAppHandle()->show_log_module("app_module_gate","app_module_gate","stop",2 );
}


//-------------------------------------------------------+-----------------------

void app_module_gate::onClick(int ih_driver_port_id, int listener_status)
{
}

//-------------------------------------------------------+-----------------------

void app_module_gate::forceCancel()
{  
      this->getAppHandle()->show_log_module("app_module_gate","app_module_gate","forceCancel",2);
      
    
      this->forceSynchronize();
      this->getAppHandle()->destroyResource(RES_VECTOR_GATE);
}

//-------------------------------------------------------+-----------------------

void app_module_gate::forceSynchronize()
{  
     this->getAppHandle()->show_log_module("app_module_gate","forceSynchronize","start",20);          
     this->getAppHandle()->show_log_module("app_module_gate","forceSynchronize","stop",20);          
}

//-------------------------------------------------------+-----------------------


bool app_module_gate::execute(gxh_message* inMsg,gxh_message* message_ret)  
{ 
    
          
      gxh_message message;
      memcpy(&message, inMsg, sizeof(gxh_message));
    
      
      if(message.func_number == IH_MODULE_FUNCTION_GATE_FORCE_OPEN )     //wymuś otwarci ebramy
           {
                return this->goOpenGate(&message, message_ret);
           }
      
      if(message.func_number == IH_MODULE_FUNCTION_GATE_FORCE_CLOSE )     //wymuś zamknięcie ebramy
           {
                return this->goCloseGate(&message, message_ret);
           }
      
      
     if(message.func_number == IH_MODULE_FUNCTION_GATE_CLICK )     //wlacznik bramy
           {
                return this->goClickGate(&message, message_ret);
           }
          
          
           if(message.func_number  == IH_MODULE_FUNCTION_GATE_OPEN_OK )     //potwierdznie otwarcia
           {
                return this->setStatusGate(&message,message_ret, 00,"MAX_OPEN");
           }
          
          
          if(message.func_number  == IH_MODULE_FUNCTION_GATE_CLOSE_OK )     //potwierdznie otwarcia
           {
                return this->setStatusGate(&message, message_ret, 0,"MAX_CLOSE");
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

bool app_module_gate::goOpenGate(gxh_message* inMsg,gxh_message* message_ret) 
{  
     ih_driver_port_function functionList;
     memcpy(&functionList,inMsg->dataStruct, sizeof(ih_driver_port_function) );
     
     
     int index = -1;
     
     for(int x=0; x<this->gate_count; x++)
     {
         if(this->gate[x].id == functionList.resource_id) { index = x; break; }
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
              if(tmpS[x].resource_id == functionList.resource_id && tmpS[x].const_function_symbol==IH_MODULE_FUNCTION_GATE_OPEN_TWO_PORT )
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
     
     
   
          this->gate[index].toSave = true;
          this->gate[index].open_percent = 50; //?? 
     
          //this->getAppHandle()->setGateStatus(this->getDbSqLite() ,functionList.resource_id, -1, "CLICK", this->debugLevel);
     
     
          this->sendInterrrupt(IH_MODULE_FUNCTION_GATE_OPEN_INTTERUPT,  functionList.resource_id);
     
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

bool app_module_gate::goCloseGate(gxh_message* inMsg,gxh_message* message_ret) 
{  
     ih_driver_port_function functionList;
     memcpy(&functionList,inMsg->dataStruct, sizeof(ih_driver_port_function) );
     
     
     int index = -1;
     
     for(int x=0; x<this->gate_count; x++)
     {
         if(this->gate[x].id == functionList.resource_id) { index = x; break; }
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

            //tutaj
            if(functionListOut[p].port_typ == IH_PORT_BISTABILE_OUT_OFF )                   
             {                                   
                isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id, IH_SET_PORT_OFF ,-1,-1,-1 );                                            
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
     
     
   
          this->gate[index].toSave = true;
          this->gate[index].open_percent = 50; //?? 
     
          //this->getAppHandle()->setGateStatus(this->getDbSqLite() ,functionList.resource_id, -1, "CLICK", this->debugLevel);
          this->sendInterrrupt(IH_MODULE_FUNCTION_GATE_CLOSE_INTTERUPT,  functionList.resource_id);
     
     
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



bool app_module_gate::goClickGate(gxh_message* inMsg,gxh_message* message_ret) 
{  
     ih_driver_port_function functionList;
     memcpy(&functionList,inMsg->dataStruct, sizeof(ih_driver_port_function) );
     
     
     int index = -1;
     
     for(int x=0; x<this->gate_count; x++)
     {
         if(this->gate[x].id == functionList.resource_id) { index = x; break; }
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
     
     
   
          this->gate[index].toSave = true;
          this->gate[index].open_percent = 50; //?? 
     
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


bool app_module_gate::setStatusGate(gxh_message* inMsg,gxh_message* message_ret, int status, std::string last_operation) 
{  
     ih_driver_port_function functionList;
     memcpy(&functionList, inMsg->dataStruct, sizeof(ih_driver_port_function) );
     
     
     int index = -1;
     
     for(int x=0; x<this->gate_count; x++)
     {
         if(this->gate[x].id == functionList.resource_id) { index = x; break; }
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
         
          this->gate[index].toSave = true;
          this->gate[index].open_percent = 100;  
     }
     
      if(status == 0)  
     {
         
          this->gate[index].toSave = true;
          this->gate[index].open_percent = 0;  
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
