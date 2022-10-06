/* 
 * File:   app_module_outs.cpp
 * Author: Blazej Kita
 * 
 * Created on 30 lipiec 2018, 14:31
 */

#include "app_module_outs.h"


 
app_module* get_module(gxh_scope* appHomeHandle )
{
    return new app_module_outs(appHomeHandle); 
};
 
//-------------------------------------------------------------------------------
  
app_module_outs::app_module_outs(gxh_scope* appHomeHandle) : app_module(appHomeHandle) 
{           
    this->getAppHandle()->show_log_module("app_module_outs","app_module_outs","start",2 );         
    
    
    this->firstSynchronize = true;
    this->count_outs = 0;
    
     this->getAppHandle()->destroyResource(RES_VECTOR_OUTS);
    
     sqlite3* dbSqLite;               
     
     
     std::string baza = this->getAppHandle()->getDB(); //this->getAppHandle()->getRunPath();
     //baza.append("../gxh_base.db");
     
     int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
     if( r != SQLITE_OK )
     {
          this->getAppHandle()->show_error("app_module_outs","app_module_outs","Cannot open sqlLite 1",0);            
     }
     else 
     {
    
        std::string query="SELECT ih_outs.id, ih_outs.name, ih_outs.ih_zone_id  ,ih_outs.status,  ih_sector.ih_home_id, ih_sector.id     FROM ih_outs, ih_zone, ih_sector, ih_home WHERE ih_outs.ih_zone_id=ih_zone.id AND ih_zone.ih_sector_id=ih_sector.id AND ih_outs.is_delete='0' AND ih_zone.is_delete='0'; ";                
        this->getAppHandle()->show_log_data_base("app_module_outs","app_module_outs", query.c_str() ,6);  
         
        sqlite3_stmt* stmt; 
    
        const char* queryChar = query.c_str();          
        if(sqlite3_prepare(dbSqLite , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
        {
          this->getAppHandle()->show_error("app_module_outs","app_module_outs", sqlite3_errstr( sqlite3_errcode(dbSqLite  ) ),4);          
          sqlite3_finalize(stmt);                 
        }else
        {    
           bool done = false;       
           while (!done)               
           { 
               int res = sqlite3_step (stmt);        
               if(res ==  SQLITE_ROW)
               {                       
                  this->outs[this->count_outs].id                 = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 0) ) );
                  
                  const char* tmp1                                = (const char*)sqlite3_column_text(stmt, 1 );
                  memset(this->outs[this->count_outs].name, 0, 100 );
                  memcpy(this->outs[this->count_outs].name, tmp1, strlen(tmp1));
                  
                  this->outs[this->count_outs].ih_zone_id             = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 2) ) );                  
                  this->outs[this->count_outs].status                 = gxh_StringToChar( std::string( (const char*)sqlite3_column_text(stmt, 3) ) );  
                  this->outs[this->count_outs].ih_home_id             = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 4) ) );
                  this->outs[this->count_outs].ih_sector_id           = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 5) ) );
             
                  
                  this->outs[this->count_outs].toSave = false;
                  this->count_outs++;
                  
                  if( this->count_outs >= MAX_OUTS_BUFFER) done = true;
                  
               }               
              if(res == SQLITE_DONE)done = true;                                     
            }
          sqlite3_finalize(stmt); 
         }
      }
    
    
     this->getAppHandle()->registerResource( this->outs ,this->count_outs, RES_VECTOR_OUTS );
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
     
    
  
     sqlite3_close_v2(dbSqLite);
   
}

//------------------------------------------------------------------------------

app_module_outs::~app_module_outs() 
{     
    this->getAppHandle()->show_log_module("app_module_outs","app_module_outs","stop",2 );
    
 
}


//-------------------------------------------------------+-----------------------

void app_module_outs::onClick(int ih_driver_port_id, int listener_status)
{
}

//-------------------------------------------------------+-----------------------

void app_module_outs::forceCancel()
{  
      this->getAppHandle()->show_log_module("app_module_outs","app_module_outs","forceCancel",2);
      
    
      this->forceSynchronize();
      this->getAppHandle()->destroyResource(RES_VECTOR_OUTS);
}

//-------------------------------------------------------+-----------------------

void app_module_outs::forceSynchronize()
{  
     this->getAppHandle()->show_log_module("app_module_outs","forceSynchronize","start",20);          
     
     
    
          
          for(unsigned int x=0; x<this->count_outs;x++)
          {
           if(this->outs[x].toSave )
           {
               
             std::string msg = "Update status for ";
             msg.append( this->outs[x].name);             
             this->getAppHandle()->show_log_module("app_module_outs","forceSynchronize",msg.c_str(),6); 
             
             //----
             sqlite3* dbSqLite;  
             
             std::string baza = this->getAppHandle()->getDB(); //this->getAppHandle()->getRunPath();
             //baza.append("../gxh_base.db");
     
             int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
             if( r != SQLITE_OK )
             {
               this->getAppHandle()->show_error("app_module_outs","forceSynchronize","Cannot open sqlLite 1",0);                 
             }else
             {                                           
                 std::string statusTMP = "0";
                 if(this->outs[x].status =='1' || this->outs[x].status==1) statusTMP = "1";
       
                 std::string query="";
                 query.append("UPDATE ih_outs SET status='");
                 query.append( statusTMP  );
                 query.append("' WHERE is_delete='0' AND id='");
                 query.append( gxh_IntToString(this->outs[x].id) );
                 query.append("'; ");
                   
                 
                 char *zErrMsg = 0;     
                 int rc = sqlite3_exec( dbSqLite , query.c_str(), 0, 0, &zErrMsg);
                 if( rc != SQLITE_OK )
                     {        
                        std::cout<<query.c_str()<<"\n";          
                        this->getAppHandle()->show_error("app_module_outs","forceSynchronize", (const char*)zErrMsg,0);
                        sqlite3_free(zErrMsg);                                
                        sqlite3_close_v2(dbSqLite);   
                     }else
                     {                                                            
                        this->outs[x].toSave = false;
                        sqlite3_close_v2(dbSqLite);                         
                     }              
                  }
                                     
                  //----
              }
          }
     
     
     if(this->firstSynchronize)
     {
         
         for(unsigned int x=0; x < this->count_outs; x++)
         {
             if(this->outs[x].status == '1' || this->outs[x].status == 1)
             {
                 this->setStatus2(this->outs[x].id, 1);
             }
         }
         
         this->firstSynchronize = false;
     }
     
     
     this->getAppHandle()->show_log_module("app_module_outs","forceSynchronize","stop",20);          
}

//-------------------------------------------------------+-----------------------


bool app_module_outs::execute(gxh_message* inMsg,gxh_message* message_ret)  
{ 
    
          
      gxh_message message;
      memcpy(&message, inMsg, sizeof(gxh_message));
    
     if(message.func_number == IH_MODULE_FUNCTION_OUTS_ON )     //wymuś włączenie
           {
             return this->setStatus(&message, message_ret, 1);  
           }
      
      if(message.func_number == IH_MODULE_FUNCTION_OUTS_OFF )     //wymuś wyłączenie
           {
                 return this->setStatus(&message, message_ret, 0);  
           }
      
      
      if(message.func_number == IH_MODULE_FUNCTION_OUTS_CHANGE )     //zmień stan na przeciwny
           {
                 return this->setStatus(&message, message_ret, 3);  
           }
     
      
          
         
     
          const char* response = "OUTS MODULE"; 
          gxh_message msg;
          msg.gxh_handle = message.gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;
          
}

//------------------------------------------------------------------------------

bool app_module_outs::setStatus2(int id, int option)
{
    
           int index = -1;
          
          for(unsigned int x=0; x<this->count_outs;x++)
          {
              if(this->outs[x].id == id)
              {
                  index = x;
                  break;
              }
          }
    
          int countPortOut = 0;           
          ih_driver_port_function functionListOut[20];  
          
          int sizeS = 0;
          ih_driver_port_function* tmpS = this->getAppHandle()->getDriverPortFunction(&sizeS);
          
          for(int x=0; x<sizeS; x++)
          {
              if(tmpS[x].resource_id == id && tmpS[x].const_function_symbol==IH_MODULE_FUNCTION_OUTS_OUT )
              {
                  memcpy( &functionListOut[countPortOut], &tmpS[x], sizeof(ih_driver_port_function) );
                  countPortOut++;
              }
          }
          
          if(countPortOut < 1) 
          {
              return false;
          }
          
          
          bool isOk = false;
             
             
             if(functionListOut[0].port_typ == IH_PORT_BISTABILE_OUT_ON )                   
             {                      
                 isOk = this->setPortStatus( functionListOut[0].ih_driver_port_id, IH_SET_PORT_ON,-1,-1,-1  );                            
                 if(isOk) 
                 {     
                     this->outs[index].status = '1';
                     this->outs[index].toSave = true;                            
                 }
             }  
             
             if(functionListOut[0].port_typ == IH_PORT_MONOSTABILE_ON_OFF_CLICK )                   
             {
                     if(this->outs[index].status == '0')
                     {
                       isOk = this->setPortStatus( functionListOut[0].ih_driver_port_id, IH_SET_PORT_CLICK,-1,-1,-1  );   
                       if(isOk)
                       {    
                           this->outs[index].status ='1';
                           this->outs[index].toSave = true;              
                       }
                     }
             }
             
             if(functionListOut[0].port_typ == IH_PORT_MONOSTABILE_ON_OFF_DOUBLE_CLICK )                   
             {
                     if(this->outs[index].status == '0')
                     {
                       isOk = this->setPortStatus( functionListOut[0].ih_driver_port_id, IH_SET_PORT_DOUBLE_CLICK,-1,-1,-1  );   
                       if(isOk)
                       {    
                           this->outs[index].status='1';
                           this->outs[index].toSave = true;                               
                       }
                     }
             } 
          
    
    return true;
}

//------------------------------------------------------------------------------
 
bool app_module_outs::setStatus(gxh_message* inMsg, gxh_message* outMsg, int option)
{
    
          ih_driver_port_function functionList;
          memcpy(&functionList, inMsg->dataStruct, sizeof(ih_driver_port_function) );
          
          int index = -1;
          
          for(unsigned int x=0; x<this->count_outs;x++)
          {
              if(this->outs[x].id == functionList.resource_id)
              {
                  index = x;
                  break;
              }
          }
    
          
          if(index == -1)
          {
              const char* response = "ERROR"; 
              gxh_message msg;
              msg.gxh_handle = inMsg->gxh_handle;
              memcpy(msg.dataStruct, response, strlen(response) );
              msg.sizeDataStruct = sizeof(response) ;
              msg.system_code = GXH_MESSAGE_SUCCESS;
            
              memcpy(outMsg, &msg, sizeof(gxh_message));            
              return true;
          }
          
          std::string msg1 = "setStatus ";
          msg1.append(this->outs[index].name);
          msg1.append(" ");
          msg1.append(gxh_IntToString(option));
          
          
          this->getAppHandle()->show_log_module("app_module_outs","setStatus",msg1.c_str(),2 );
          
          
          
          int countPortOut = 0;
          //ih_driver_port_function* functionListOut = this->getDriverPortFunction( functionList.resource_id, IH_MODULE_FUNCTION_LIGHT_OUT, &countPortOut);
          ih_driver_port_function functionListOut[20];  //this->getAppHandle()->getDriverPortFunction( functionList.resource_id, IH_MODULE_FUNCTION_LIGHT_OUT, &countPortOut);
          
          int sizeS = 0;
          ih_driver_port_function* tmpS = this->getAppHandle()->getDriverPortFunction(&sizeS);
          
          for(int x=0; x<sizeS; x++)
          {
              if(tmpS[x].resource_id == functionList.resource_id && tmpS[x].const_function_symbol==IH_MODULE_FUNCTION_OUTS_OUT )
              {
                  memcpy( &functionListOut[countPortOut], &tmpS[x], sizeof(ih_driver_port_function) );
                  countPortOut++;
              }
          }
          
          if(countPortOut < 1) 
          {
              const char* response = "ERROR OUT"; 
              gxh_message msg;
              msg.gxh_handle = inMsg->gxh_handle;
              memcpy(msg.dataStruct, response, strlen(response) );
              msg.sizeDataStruct = sizeof(response) ;
              msg.system_code = GXH_MESSAGE_SUCCESS;
            
              memcpy(outMsg, &msg, sizeof(gxh_message));            
              return true;
          }
          
          
          
          if(option == 1) // ON
          {
             bool isOk = false;
             
             
             if(functionListOut[0].port_typ == IH_PORT_BISTABILE_OUT_ON )                   
             {                      
                 isOk = this->setPortStatus( functionListOut[0].ih_driver_port_id, IH_SET_PORT_ON,-1,-1,-1  );                            
                 if(isOk) 
                 {     
                     this->outs[index].status = '1';
                     this->outs[index].toSave = true;                            
                 }
             }  
             
             if(functionListOut[0].port_typ == IH_PORT_MONOSTABILE_ON_OFF_CLICK )                   
             {
                     if(this->outs[index].status == '0')
                     {
                       isOk = this->setPortStatus( functionListOut[0].ih_driver_port_id, IH_SET_PORT_CLICK,-1,-1,-1  );   
                       if(isOk)
                       {    
                           this->outs[index].status ='1';
                           this->outs[index].toSave = true;              
                       }
                     }
             }
             
             if(functionListOut[0].port_typ == IH_PORT_MONOSTABILE_ON_OFF_DOUBLE_CLICK )                   
             {
                     if(this->outs[index].status == '0')
                     {
                       isOk = this->setPortStatus( functionListOut[0].ih_driver_port_id, IH_SET_PORT_DOUBLE_CLICK,-1,-1,-1  );   
                       if(isOk)
                       {    
                           this->outs[index].status='1';
                           this->outs[index].toSave = true;                               
                       }
                     }
             } 
          }
          
          if(option ==0 ) //  OFF
          {
             bool isOk = false;
             
           
             
             if(functionListOut[0].port_typ == IH_PORT_BISTABILE_OUT_ON )                   
             {                      
                 isOk = this->setPortStatus( functionListOut[0].ih_driver_port_id, IH_SET_PORT_OFF,-1,-1,-1  );                            
                 if(isOk) 
                 {     
                     this->outs[index].status = '0';
                     this->outs[index].toSave = true;                            
                 }
             }  
             
             if(functionListOut[0].port_typ == IH_PORT_MONOSTABILE_ON_OFF_CLICK )                   
             {
                     if(this->outs[index].status == '1')
                     {
                       isOk = this->setPortStatus( functionListOut[0].ih_driver_port_id, IH_SET_PORT_CLICK,-1,-1,-1  );   
                       if(isOk)
                       {    
                           this->outs[index].status ='0';
                           this->outs[index].toSave = true;              
                       }
                     }
             }
             
             if(functionListOut[0].port_typ == IH_PORT_MONOSTABILE_ON_OFF_DOUBLE_CLICK )                   
             {
                     if(this->outs[index].status == '1')
                     {
                       isOk = this->setPortStatus( functionListOut[0].ih_driver_port_id, IH_SET_PORT_DOUBLE_CLICK,-1,-1,-1  );   
                       if(isOk)
                       {    
                           this->outs[index].status='0';
                           this->outs[index].toSave = true;                               
                       }
                     }
             }
          }
          
          if(option == 3) // ON / OFF
          {
              if( this->outs[index].status== '0' )
              {
                  return this->setStatus(inMsg, outMsg, 1); //set on
              }
              
              if( this->outs[index].status== '1' )
              {
                  return this->setStatus(inMsg, outMsg, 0); //set off
              }
          }
          
    
          const char* response = "OK"; 
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(outMsg, &msg, sizeof(gxh_message));            
          return true;
}
 
//------------------------------------------------------------------------------
