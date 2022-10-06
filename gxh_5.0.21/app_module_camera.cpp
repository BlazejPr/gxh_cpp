/* 
 * File:   app_module_camera.cpp
 * Author: Blazej Kita
 * 
 * Created on 14 grudzień 2016, 14:31
 */

#include "app_module_camera.h"


 
app_module* get_module(gxh_scope* appHomeHandle )
{
    return new app_module_camera(appHomeHandle); 
};
 
//-------------------------------------------------------------------------------
  
app_module_camera::app_module_camera(gxh_scope* appHomeHandle) : app_module(appHomeHandle) 
{           
    this->getAppHandle()->show_log_module("app_module_camera","app_module_camera","start",2 );         
    
    
    this->count_camers = 0;
    this->getAppHandle()->destroyResource(RES_VECTOR_CAMERA);
    
    
     sqlite3* dbSqLite;                    
     
     std::string baza = this->getAppHandle()->getDB(); //this->getAppHandle()->getRunPath();
    // baza.append("../gxh_base.db");
     
     int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
     if( r != SQLITE_OK )
     {
          this->getAppHandle()->show_error("app_module_camera","app_module_camera","Cannot open sqlLite 1",0);            
     }
     else 
     {
    
        std::string query=" SELECT id, name, ip, login, password, protocol, url, local_port, remote_port, protocol FROM ih_camers WHERE is_delete='0';  ";                
        this->getAppHandle()->show_log_data_base("app_module_camera","app_module_camera", query.c_str() ,6);  
         
        sqlite3_stmt* stmt; 
    
        const char* queryChar = query.c_str();          
        if(sqlite3_prepare(dbSqLite , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
        {
          this->getAppHandle()->show_error("app_module_camera","app_module_camera", sqlite3_errstr( sqlite3_errcode(dbSqLite  ) ),4);          
          sqlite3_finalize(stmt);                 
        }else
        {    
           bool done = false;       
           while (!done)               
           { 
               int res = sqlite3_step (stmt);        
               if(res ==  SQLITE_ROW)
               {                       
                  this->camers[this->count_camers].id                 = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 0) ) );
                  
                  
                  const char* tmp1 =   (const char*)sqlite3_column_text(stmt, 1)  ;
                  memset( this->camers[this->count_camers].name, 0, 100 );
                  if(tmp1 != NULL)memcpy( this->camers[this->count_camers].name, tmp1, strlen(tmp1) );
                  
                  
                  const char* tmp2 =   (const char*)sqlite3_column_text(stmt, 2)  ;
                  memset( this->camers[this->count_camers].localIp, 0, 100 );
                  if(tmp2 != NULL)memcpy( this->camers[this->count_camers].localIp, tmp2, strlen(tmp2) );
                  
                  const char* tmp3 =   (const char*)sqlite3_column_text(stmt, 7)  ;
                  if(tmp3 != NULL)
                  {
                     this->camers[this->count_camers].localPort = gxh_StringToInt( std::string(tmp3) ); 
                  }
                  
                  
                  const char* tmp4 =   (const char*)sqlite3_column_text(stmt, 8)  ;
                  if(tmp4 != NULL)
                  {
                     this->camers[this->count_camers].remotePort = gxh_StringToInt( std::string(tmp4) ); 
                  }
                  
                   const char* tmp5 =   (const char*)sqlite3_column_text(stmt, 6)  ;
                  memset( this->camers[this->count_camers].url, 0, 1024 );
                  if(tmp5 != NULL)memcpy( this->camers[this->count_camers].url, tmp5, strlen(tmp5) );
                  
                  
                  const char* tmp6 =   (const char*)sqlite3_column_text(stmt, 3)  ;
                  memset( this->camers[this->count_camers].user, 0, 50 );
                  if(tmp6 != NULL)memcpy( this->camers[this->count_camers].user, tmp6, strlen(tmp6) );
                  
                  
                  const char* tmp7 =   (const char*)sqlite3_column_text(stmt, 4)  ;
                  memset( this->camers[this->count_camers].password, 0, 50 );
                  if(tmp7 != NULL)memcpy( this->camers[this->count_camers].password, tmp7, strlen(tmp7) );
                  
                   const char* tmp8 =   (const char*)sqlite3_column_text(stmt, 9)  ;
                  memset( this->camers[this->count_camers].protocol, 0, 10 );
                  if(tmp8 != NULL)memcpy( this->camers[this->count_camers].protocol, tmp8, strlen(tmp8) );
                  
                  
                  
                  this->count_camers++;
               }               
              if(res == SQLITE_DONE)done = true;                                     
            }
          sqlite3_finalize(stmt); 
         }
      }
    
    
     this->getAppHandle()->registerResource( this->camers ,this->count_camers, RES_VECTOR_CAMERA );
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
     
     
  
     sqlite3_close(dbSqLite);
}

//------------------------------------------------------------------------------

app_module_camera::~app_module_camera() 
{     
    this->getAppHandle()->show_log_module("app_module_camera","app_module_camera","stop",2 );
}


//-------------------------------------------------------+-----------------------

void app_module_camera::onClick(int ih_driver_port_id, int listener_status)
{
}

//-------------------------------------------------------+-----------------------

void app_module_camera::forceCancel()
{  
      this->getAppHandle()->show_log_module("app_module_camera","app_module_camera","forceCancel",2);
      
    
      this->forceSynchronize();
      this->getAppHandle()->destroyResource(RES_VECTOR_CAMERA);
}

//-------------------------------------------------------+-----------------------

void app_module_camera::forceSynchronize()
{  
     this->getAppHandle()->show_log_module("app_module_camera","forceSynchronize","start",20);          
     this->getAppHandle()->show_log_module("app_module_camera","forceSynchronize","stop",20);          
}

//-------------------------------------------------------+-----------------------


bool app_module_camera::execute(gxh_message* inMsg,gxh_message* message_ret)  
{ 
    
          
      gxh_message message;
      memcpy(&message, inMsg, sizeof(gxh_message));
    
      
       //  if(message.func_number ==  0 )     //
        //   {
                 
         //  }
          
        
     
          const char* response = "CAMERA MODULE"; 
          gxh_message msg;
          msg.gxh_handle = message.gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;
          
}

//------------------------------------------------------------------------------

  
//------------------------------------------------------------------------------

 
//------------------------------------------------------------------------------
