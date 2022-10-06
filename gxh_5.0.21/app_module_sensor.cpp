/* 
 * File:   app_module_sensor.cpp
 * Author: Blazej Kita
 * 
 * Created on 14 grudzień 2016, 14:31
 */

#include "app_module_sensor.h"


 
app_module* get_module(gxh_scope* appHomeHandle)
{
    return new app_module_sensor(appHomeHandle); 
};
 
//-------------------------------------------------------------------------------
  
app_module_sensor::app_module_sensor(gxh_scope* appHomeHandle) : app_module(appHomeHandle) 
{           
    this->getAppHandle()->show_log_module("app_module_sensor","app_module_sensor","start",2);         
    
    this->sensor_count = 0;
    
    this->getAppHandle()->destroyResource(RES_VECTOR_SENSOR);
    
    //pobierz sensory do pamieci..
    
      sqlite3* dbSqLite;               
     
     
     std::string baza = this->getAppHandle()->getDB();//this->getAppHandle()->getRunPath();
     //baza.append("../gxh_base.db");
     
     int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
     if( r != SQLITE_OK )
     {
          this->getAppHandle()->show_error("app_module_sensor","app_module_sensor","Cannot open sqlLite 1",0);   
          return;
     }
     
     
    sqlite3_stmt* stmt; 
    
    std::string query="";
    query.append("SELECT id, name, ih_zone_id, status, strftime('%s',status_time) as status_time, status_time, symbol,analogowy, wartosc,ih_driver_values_id FROM ih_sensor WHERE is_delete='0'"); 
    
    
    const char* queryChar = query.c_str();           
    if(sqlite3_prepare(dbSqLite , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
    {     
         this->getAppHandle()->show_error("app_module_sensor","app_module_sensor", sqlite3_errstr( sqlite3_errcode(dbSqLite  ) ),4);          
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
             this->sensor[this->sensor_count].id                    = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 0) ) );                                                          
             
      
    
             
             const char*  tmp1                                      = (const char*)sqlite3_column_text(stmt, 1 );
             memset(this->sensor[this->sensor_count].name, 0, 100);
             memcpy(this->sensor[this->sensor_count].name, tmp1, strlen(tmp1));
             
                    
             
             this->sensor[this->sensor_count].ih_zone_id            = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 2)  ) );                       
             this->sensor[this->sensor_count].status                = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 3) ) );          
             
             this->sensor[this->sensor_count].status_time           = gxh_StringToDouble( std::string( (const char*)sqlite3_column_text(stmt, 4) ) ) * 1000;  //sekundy zamień na milisekundy...               
             
  
             const char*  tmp2                                      = (const char*)sqlite3_column_text(stmt, 5 );
             memset(this->sensor[this->sensor_count].status_time_str, 0, 15);
             memcpy(this->sensor[this->sensor_count].status_time_str, tmp2, strlen(tmp2));
             
              
             const char*  tmp3                                      = (const char*)sqlite3_column_text(stmt, 6 );
             memset(this->sensor[this->sensor_count].symbol, 0, 2);
             memcpy(this->sensor[this->sensor_count].symbol, tmp3, strlen(tmp3));
             
 
             this->sensor[this->sensor_count].analogowy            = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 7)  ) );     
             
             if(  this->sensor[this->sensor_count].analogowy  ==0)
             {
                 this->sensor[this->sensor_count].status = 0;
             }
             
            
             const char*  tmp4                                      = (const char*)sqlite3_column_text(stmt, 8 );
             memset(this->sensor[this->sensor_count].wartosc, 0, 255);
             if(tmp4 != NULL)memcpy(this->sensor[this->sensor_count].wartosc, tmp4, strlen(tmp4));
             
             
            
             
             this->sensor[this->sensor_count].ih_driver_values_id  = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 9) ) );
             
             this->sensor[this->sensor_count].toSave                = false;               
             this->sensor_count++;                          
         }               
         if(res == SQLITE_DONE)done = true;                                     
    }
    
    
    this->getAppHandle()->registerResource( this->sensor ,this->sensor_count, RES_VECTOR_SENSOR );
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
          
    sqlite3_finalize(stmt);    
    sqlite3_close_v2(dbSqLite);
    
}

//------------------------------------------------------------------------------

app_module_sensor::~app_module_sensor() 
{     
    this->getAppHandle()->show_log_module("app_module_sensor","app_module_sensor","stop",2);
}

//------------------------------------------------------------------------------

void app_module_sensor::onClick(int ih_driver_port_id, int listener_status)
{
}

//------------------------------------------------------------------------------

void app_module_sensor::forceCancel()
{
    
}

//------------------------------------------------------------------------------

void app_module_sensor::forceSynchronize()
{
     for(unsigned int x=0; x<this->sensor_count; x++)
     {
        if(sensor[x].toSave) 
        {
             this->getAppHandle()->show_log_module("app_module_sensor","forceSynchronize","forceSynchronize....................",20);          
     
   
             std::string msg = "Update status for ";
             msg.append( this->sensor[x].name);             
             this->getAppHandle()->show_log_module("app_module_sensor","forceSynchronize",msg.c_str(),6); 
             
             //----
             sqlite3* dbSqLite;  
             
             std::string baza = this->getAppHandle()->getDB();//this->getAppHandle()->getRunPath();
            // baza.append("../gxh_base.db");
     
             int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
             if( r != SQLITE_OK )
             {
               this->getAppHandle()->show_error("app_module_sensor","forceSynchronize","Cannot open sqlLite 1",0);                 
             }else
             {                                                            
                 std::string query="";
                 query.append("UPDATE ih_sensor SET status_time=DATETIME('now','localtime'), status='");
                 query.append( gxh_IntToString(this->sensor[x].status)  );
                 query.append("' WHERE is_delete='0' AND id='");
                 query.append( gxh_IntToString(this->sensor[x].id) );
                 query.append("'; ");
                   
                 
                 char *zErrMsg = 0;     
                 int rc = sqlite3_exec( dbSqLite , query.c_str(), 0, 0, &zErrMsg);
                 if( rc != SQLITE_OK )
                     {        
                        std::cout<<query.c_str()<<"\n";          
                        this->getAppHandle()->show_error("app_module_sensor","forceSynchronize", (const char*)zErrMsg,0);
                        sqlite3_free(zErrMsg);                                
                     }else
                     {                                                            
                        this->sensor[x].toSave = false;
                     }
                 sqlite3_close_v2(dbSqLite);    
             }
                                     
             //----
       
        }
     }
}

//------------------------------------------------------------------------------


bool app_module_sensor::execute(gxh_message* inMsg, gxh_message* message_ret)  
{         
          if(inMsg->func_number == IH_MODULE_FUNCTION_STANDARD_SENSOR )   //powiadom o stanie niskim portu
           {
               ih_driver_port_function functionList;
               memcpy(&functionList, inMsg->dataStruct, sizeof(ih_driver_port_function) );
               
               if(functionList.port_typ == IH_SWITCH_BISTABILE_ON_NO) this->setSensorOnOf(inMsg,message_ret, functionList.resource_id, 1); //stan wysoki na porcie
               if(functionList.port_typ == IH_SWITCH_BISTABILE_OFF_NO) this->setSensorOnOf(inMsg, message_ret, functionList.resource_id, 0); //stan niski na porcie
               
               if(functionList.port_typ == IH_SWITCH_BISTABILE_ON_NC) this->setSensorOnOf(inMsg, message_ret, functionList.resource_id, 1); //stan wysoki na porcie - odwrotnie
               if(functionList.port_typ == IH_SWITCH_BISTABILE_OFF_NC) this->setSensorOnOf(inMsg, message_ret, functionList.resource_id, 0); //stan niski na porcie - odwrotnie
               
               
                if(functionList.port_typ == IH_SWITCH_MONOSTABILE_ON_OFF_CLICK_NO) this->setSensorOnOf(inMsg, message_ret, functionList.resource_id, 2); //stan na przeciwny            
                if(functionList.port_typ == IH_SWITCH_MONOSTABILE_ON_OFF_CLICK_NC) this->setSensorOnOf(inMsg, message_ret, functionList.resource_id, 2); //stan na przeciwny            
           }
           
         
          const char* response = "SENSOR MODULE"; 
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

bool app_module_sensor::setSensorOnOf(gxh_message* inMsg, gxh_message* message_ret, int id, int status)
{
     unsigned int index = -1;
     
     for(unsigned int x=0; x<this->sensor_count; x++)
     {
         if(sensor[x].id == id) {index = x; break;}
     }
     
     
     if(status == 1)
     { 
         sensor[index].status = 1;
         sensor[index].toSave = true;
     }
     
     
     if(status == 0)
     {
         sensor[index].status = 0;
         sensor[index].toSave = true;
     }
     
     
     if(status == 2)
     {
        if(sensor[index].status == 1)    
        {
         sensor[index].status = 0;
         sensor[index].toSave = true;
        }
            
        if(sensor[index].status == 0)
        {
         sensor[index].status = 1;
         sensor[index].toSave = true;
        }
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