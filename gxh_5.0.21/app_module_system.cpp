/* 
 * File:   app_module_system.cpp
 * Author: Blazej Kita
 * 
 * Created on 14 grudzień 2016, 14:31
 */

#include "app_module_system.h"


 
app_module* get_module(gxh_scope* appHomeHandle)
{
    return new app_module_system(appHomeHandle); 
};
 
//-------------------------------------------------------------------------------
  
app_module_system::app_module_system(gxh_scope* appHomeHandle) : app_module(appHomeHandle ) 
{           
    this->getAppHandle()->show_log_module("app_module_system","app_module_system","start",2 );        
    
    
    //załaduj funkcje warunkowe...
    this->count_func = 0;
    this->count_func_var = 0;
  
    
     sqlite3* dbSqLite;               
          
     std::string baza = this->getAppHandle()->getDB();//this->getAppHandle()->getRunPath();
    // baza.append("../gxh_base.db");
     
     int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
     if( r != SQLITE_OK )
     {
          this->getAppHandle()->show_error("app_module_system","app_module_system","Cannot open sqlLite 1",0);            
     }
     else 
     {
    
        std::string query="SELECT id, name, all_ok, all_nok,active FROM ih_adv_function WHERE is_delete='0' AND active='1';";                
        this->getAppHandle()->show_log_data_base("app_module_system","app_module_system", query.c_str() ,6);  
         
        sqlite3_stmt* stmt; 
    
        const char* queryChar = query.c_str();          
        if(sqlite3_prepare(dbSqLite , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
        {
          this->getAppHandle()->show_error("app_module_system","app_module_system", sqlite3_errstr( sqlite3_errcode(dbSqLite  ) ),4);          
          sqlite3_finalize(stmt);                 
        }else
        {    
           bool done = false;       
           while (!done)               
           { 
               int res = sqlite3_step (stmt);        
               if(res ==  SQLITE_ROW)
               {                       
                  this->func[this->count_func].id                 = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 0) ) );
                  
                  const char* tmp1                                = (const char*)sqlite3_column_text(stmt, 1 );
                  memset(this->func[this->count_func].name, 0, 50 );
                  if(strlen(tmp1) < 50 )memcpy(this->func[this->count_func].name, tmp1, strlen(tmp1));
                  
                  this->func[this->count_func].all_ok  = -1;
                  this->func[this->count_func].all_nok = -1;
                  
                  if(sqlite3_column_text(stmt, 2)  != NULL)this->func[this->count_func].all_ok                 = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 2) ) );
                  if(sqlite3_column_text(stmt, 3)  != NULL)this->func[this->count_func].all_nok                 = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 3) ) );                                
                  
                  if(sqlite3_column_text(stmt, 4)  != NULL)this->func[this->count_func].active                 = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 4) ) );
                 
                 
                //  std::cout<<"\n\n\n"<<this->func[this->count_func].name<<" | "<<this->func[this->count_func].all_ok<<" | "<<this->func[this->count_func].all_nok<<"\n\n";
                  
                  this->func[this->count_func].toSave = false;
       
        
                  this->count_func++;
               }               
              if(res == SQLITE_DONE)done = true;                                     
            }
          sqlite3_finalize(stmt); 
         }
        
        
        
        
         query="SELECT id, ih_adv_function_id, type_ion, resource_name, resource_id, wartosc_jest_rowna, wartosc_jest_wieksza, wartosc_jest_mniejsza,wartosc,typ_wartosci,out_const_function_symbol,out_resource_id, out_param1,out_param2, p1_const_function_symbol, p1_resource_id, p1_param1, p1_param2, p2_const_function_symbol,p2_resource_id, p2_param1, p2_param2 FROM ih_adv_var WHERE is_delete='0';";                
        this->getAppHandle()->show_log_data_base("app_module_system","app_module_system", query.c_str() ,6);  
         
        
    
        queryChar = query.c_str();          
        if(sqlite3_prepare(dbSqLite , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
        {
          this->getAppHandle()->show_error("app_module_system","app_module_system", sqlite3_errstr( sqlite3_errcode(dbSqLite  ) ),4);          
          sqlite3_finalize(stmt);                 
        }else
        {    
           bool done = false;       
           while (!done)               
           { 
               int res = sqlite3_step (stmt);        
               if(res ==  SQLITE_ROW)
               {                       
                  this->func_var[this->count_func_var].id                 = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 0) ) );
                  this->func_var[this->count_func_var].ih_adv_function_id = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 1) ) );
                  
                  
                  
                  const char* tmp1                                = (const char*)sqlite3_column_text(stmt, 2 );
                  memset(this->func_var[this->count_func_var].type_ion, 0, 2 );
                  if(tmp1 != NULL && strlen(tmp1) < 2 )memcpy(this->func_var[this->count_func_var].type_ion, tmp1, strlen(tmp1));
                  
                  
                  const char* tmp2                                = (const char*)sqlite3_column_text(stmt, 3 );
                  memset(this->func_var[this->count_func_var].resource_name, 0, 50 );
                  if(tmp2 != NULL &&  strlen(tmp2) < 50 )memcpy(this->func_var[this->count_func_var].resource_name, tmp2, strlen(tmp2));
                  
                  
                  if(sqlite3_column_text(stmt, 4) != NULL )  this->func_var[this->count_func_var].resource_id = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 4) ) );
                  if(sqlite3_column_text(stmt, 5) != NULL )this->func_var[this->count_func_var].wartosc_jest_rowna = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 5) ) );
                  if(sqlite3_column_text(stmt, 6) != NULL )this->func_var[this->count_func_var].wartosc_jest_wieksza = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 6) ) );
                  if(sqlite3_column_text(stmt, 7) != NULL )this->func_var[this->count_func_var].wartosc_jest_mniejsza = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 7) ) );
                  
                                        
                  const char* tmp3                                = (const char*)sqlite3_column_text(stmt, 8 );
                  memset(this->func_var[this->count_func_var].wartosc, 0, 255 );
                  if(tmp3 != NULL &&  strlen(tmp3) < 255 )memcpy(this->func_var[this->count_func_var].wartosc, tmp3, strlen(tmp3));
        
                  if(sqlite3_column_text(stmt, 9) != NULL )this->func_var[this->count_func_var].typ_wartosci = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 9) ) );
                  if(sqlite3_column_text(stmt, 10) != NULL )this->func_var[this->count_func_var].out_const_function_symbol = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 10) ) );
                  if(sqlite3_column_text(stmt, 11) != NULL )this->func_var[this->count_func_var].out_resource_id = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 11) ) );
                  
                  
                  const char* tmp4                                = (const char*)sqlite3_column_text(stmt, 12 );
                  memset(this->func_var[this->count_func_var].out_param1, 0, 255 );
                  if(tmp4 != NULL &&  strlen(tmp4) < 255 )memcpy(this->func_var[this->count_func_var].out_param1, tmp4, strlen(tmp4));
                  
                  const char* tmp5                                = (const char*)sqlite3_column_text(stmt, 13 );
                  memset(this->func_var[this->count_func_var].out_param2, 0, 255 );
                  if(tmp5 != NULL &&  strlen(tmp5) < 255 )memcpy(this->func_var[this->count_func_var].out_param2, tmp5, strlen(tmp5));
                  
                  
                  //notice ex       
                  if(sqlite3_column_text(stmt, 14)  != NULL) this->func_var[this->count_func_var].notice_ex.p1_const_function_response = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 14) ) );
                  if(sqlite3_column_text(stmt, 15)  != NULL)  this->func_var[this->count_func_var].notice_ex.p1_resource_id = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 15) ) );
                  
                  const char* tmp6                                = (const char*)sqlite3_column_text(stmt, 16 );
                  memset( this->func_var[this->count_func_var].notice_ex.p1_param1 , 0, 255 );
                  if(tmp6 != NULL &&  strlen(tmp6) < 255 )memcpy(this->func_var[this->count_func_var].notice_ex.p1_param1, tmp6, strlen(tmp6));
                  
                  
                  const char* tmp7                                = (const char*)sqlite3_column_text(stmt, 17 );
                  memset( this->func_var[this->count_func_var].notice_ex.p1_param2 , 0, 500 );
                  if(tmp7 != NULL &&  strlen(tmp7) < 500 )memcpy(this->func_var[this->count_func_var].notice_ex.p1_param2, tmp7, strlen(tmp7));
                  
                  
                  
                 if(sqlite3_column_text(stmt, 18)  != NULL)   this->func_var[this->count_func_var].notice_ex.p2_const_function_response = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 18) ) );
                 if(sqlite3_column_text(stmt, 19)  != NULL)   this->func_var[this->count_func_var].notice_ex.p2_resource_id = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 19) ) );
                  
                  const char* tmp8                                = (const char*)sqlite3_column_text(stmt, 20 );
                  memset( this->func_var[this->count_func_var].notice_ex.p2_param1 , 0, 255 );
                  if(tmp8 != NULL &&  strlen(tmp8) < 255 )memcpy(this->func_var[this->count_func_var].notice_ex.p2_param1, tmp8, strlen(tmp8));
                  
                  
                  const char* tmp9                                = (const char*)sqlite3_column_text(stmt, 21 );
                  memset( this->func_var[this->count_func_var].notice_ex.p2_param2 , 0, 500 );
                  if(tmp9 != NULL &&  strlen(tmp9) < 500 )memcpy(this->func_var[this->count_func_var].notice_ex.p2_param2, tmp9, strlen(tmp9));
                  
                  
                  
                  
                  this->count_func_var++;
               }               
              if(res == SQLITE_DONE)done = true;                                     
            }
          sqlite3_finalize(stmt); 
         }
        
      }
    
 
     
  
     sqlite3_close_v2(dbSqLite);
}

//------------------------------------------------------------------------------

app_module_system::~app_module_system() 
{     
    this->getAppHandle()->show_log_module("app_module_system","app_module_system","stop",2);
}

//------------------------------------------------------------------------------

void app_module_system::onClick(int ih_driver_port_id, int listener_status)
{
}

//------------------------------------------------------------------------------
 
void app_module_system::forceCancel()
{  
      this->getAppHandle()->show_log_module("app_module_system","app_module_system","forceCancel",2);          
      this->forceSynchronize(); 
}

//-------------------------------------------------------+-----------------------

void app_module_system::forceSynchronize()
{ 
      //internet status variable...
      bool isVar = false;
      
      unsigned int size = 0;
      ih_driver_values* first = (ih_driver_values*)this->getAppHandle()->getResource(RES_VECTOR_DRIVER_VALUE, &size);  
      
      for(unsigned int x=0; x<size; x++)
      {
          if( (first[x].free == false)   && (first[x].const_function == VAR_INTERNET_STATUS) &&  (first[x].ih_driver_id < 0) && (first[x].px_group_id < 0) )
          {
              isVar = true;
             // std::cout<<"\nSTDSTDSTD: "<<  first[x].status;
              break;
          }
      }
      
     // std::cout<<"\n\nStatus zmiennej: "<<isVar;
      
      if(isVar == false) //dodaj potrzebną zmienną..
      {          
        for(unsigned int x=0; x<size; x++)
        {
           if( (first[x].free == true) )
           {
                std::string msg = "internet - status";
               
                first[x].free   = false;             
                first[x].id   = -1;
                
                memset(first[x].name_param, 0, msg.length()+1 );
                memcpy(first[x].name_param, msg.c_str(), msg.length() );
                
                first[x].ih_driver_id = -1;
                first[x].px_group_id = -1;
                first[x].const_function = VAR_INTERNET_STATUS; //internet status
                first[x].status = 2; //add to database
                
               // std::cout<<"\nInternet add "<< msg.c_str()<<", n: "<<x;
                
                break;
           }
        }                         
      }
      
      for(int x=0; x<this->count_func; x++)
      {
          if(this->func[x].toSave)
          {
                      sqlite3* dbSqLite2;                    
    
                      std::string baza = this->getAppHandle()->getDB();//this->getAppHandle()->getRunPath();
                     // baza.append("../gxh_base.db");
     
                      int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite2, SQLITE_OPEN_READWRITE, NULL   );                                  
                      if( r != SQLITE_OK )
                      {
                         this->getAppHandle()->show_error("app_module_system","forceSynchronize","Cannot open sqlLite 1",0);                      
                      }else
                      {                           
                           
                                std::string query = "UPDATE ih_adv_function SET all_ok='";
                                query.append(gxh_IntToString( this->func[x].all_ok ) );
                                query.append("', all_nok='" );
                                query.append(gxh_IntToString( this->func[x].all_nok ) );
                                query.append("' WHERE id='" );
                                query.append(gxh_IntToString( this->func[x].id ) );
                                query.append("'; " );
                                
                                this->getAppHandle()->show_log_module("app_module_system","forceSynchronize",query.c_str(),6 );
                                
                                char *zErrMsg = 0;     
                                int rc = sqlite3_exec( dbSqLite2 , query.c_str(), 0, 0, &zErrMsg);
                                if( rc != SQLITE_OK )
                                   {  
                                      this->getAppHandle()->show_error("app_module_system","forceSynchronize", (const char*)zErrMsg ,0);   
                                      sqlite3_free(zErrMsg);    
                                   }else this->func[x].toSave = false; 
                            
                            
                          sqlite3_close_v2(dbSqLite2);                          
                       } 
          }
      }
      
}

//------------------------------------------------------------------------------
 
bool app_module_system::execute(gxh_message* inMsg, gxh_message* message_ret)  
{  
     gxh_message message;
     memcpy(&message, inMsg, sizeof(message) );
     
     
     if(message.func_number == IH_MODULE_FUNCTION_LIGHT_BACKUP )  //na starcie aplikacji / komputera przywróc właściwy stan oświetlenia     
        {
              return this->lightBackup(&message, message_ret);
        }
     
     if(message.func_number == IH_MODULE_FUNCTION_RESTART )  //restart komputera..
        {
              return this->restartHardware(&message, message_ret);
        }
     
     if(message.func_number == IH_MODULE_FUNCTION_RESTART_APP )  //restart aplikacji..
        {
              return this->restartApp(&message, message_ret);
        }
     
      if(message.func_number == IH_MODULE_FUNCTION_PORT_GO_ON )  //ustaw stan wysoki na porcie
        {
              return this->setPort(&message, message_ret,1);
        }
     
     if(message.func_number == IH_MODULE_FUNCTION_PORT_GO_OFF )  //ustaw stan niski na porcie
        {
              return this->setPort(&message, message_ret,0);
        }
     
      if(message.func_number == IH_MODULE_FUNCTION_PORT_GO_CLICK )  //ustaw klik 
        {
              return this->setPort(&message, message_ret,2);
        }
     
      if(message.func_number == IH_MODULE_FUNCTION_PORT_GO_ON_1sek )  
        {
              return this->setPortClick(&message, message_ret,1);
        }
     
     if(message.func_number == IH_MODULE_FUNCTION_PORT_GO_ON_2sek )  
        {
              return this->setPortClick(&message, message_ret,2);
        }
     
     if(message.func_number == IH_MODULE_FUNCTION_PORT_GO_ON_3sek )  
        {
              return this->setPortClick(&message, message_ret,3);
        }
     
     if(message.func_number == IH_MODULE_FUNCTION_PORT_GO_ON_4sek )  
        {
              return this->setPortClick(&message, message_ret,4);
        }
     
     if(message.func_number == IH_MODULE_FUNCTION_PORT_GO_ON_5sek )  
        {
              return this->setPortClick(&message, message_ret,5);
        }
     
      if(message.func_number == IH_MODULE_FUNCTION_PORT_GO_ON_6sek )  
        {
              return this->setPortClick(&message, message_ret,6);
        }
     
      if(message.func_number == IH_MODULE_FUNCTION_PORT_GO_ON_7sek )  
        {
              return this->setPortClick(&message, message_ret,7);
        }
     
      if(message.func_number == IH_MODULE_FUNCTION_PORT_GO_ON_8sek )  
        {
              return this->setPortClick(&message, message_ret,8);
        }
     
      if(message.func_number == IH_MODULE_FUNCTION_PORT_GO_ON_9sek )  
        {
              return this->setPortClick(&message, message_ret,9);
        }
     
      if(message.func_number == IH_MODULE_FUNCTION_PORT_GO_ON_10sek )  
        {
              return this->setPortClick(&message, message_ret,10);
        }
     
      if(message.func_number == IH_MODULE_FUNCTION_PORT_GO_ON_1min )  
        {
              return this->setPortClick(&message, message_ret,60);
        }
     
      if(message.func_number == IH_MODULE_FUNCTION_PORT_GO_ON_2min )  
        {
              return this->setPortClick(&message, message_ret,60 * 2);
        }
     
      if(message.func_number == IH_MODULE_FUNCTION_PORT_GO_ON_3min )  
        {
              return this->setPortClick(&message, message_ret,60 * 3);
        }
     
      if(message.func_number == IH_MODULE_FUNCTION_PORT_GO_ON_4min )  
        {
              return this->setPortClick(&message, message_ret,60 * 4);
        }
     
      if(message.func_number == IH_MODULE_FUNCTION_PORT_GO_ON_5min )  
        {
              return this->setPortClick(&message, message_ret,60 * 5);
        }
     
      if(message.func_number == IH_MODULE_FUNCTION_PORT_GO_ON_6min )  
        {
              return this->setPortClick(&message, message_ret,60 * 6);
        }
     
      if(message.func_number == IH_MODULE_FUNCTION_PORT_GO_ON_7min )  
        {
              return this->setPortClick(&message, message_ret,60 * 7);
        }
     
      if(message.func_number == IH_MODULE_FUNCTION_PORT_GO_ON_8min )  
        {
              return this->setPortClick(&message, message_ret,60 * 8);
        }
     
      if(message.func_number == IH_MODULE_FUNCTION_PORT_GO_ON_9min )  
        {
              return this->setPortClick(&message, message_ret,60 * 9);
        }
     
      if(message.func_number == IH_MODULE_FUNCTION_PORT_GO_ON_10min )  
        {
              return this->setPortClick(&message, message_ret,60 * 10);
        }
     
     
     
      if(message.func_number == IH_MODULE_FUNCTION_CHECK_INTERNET_STATUS )  //sprawdz status internetu
        {
              return this->check_internet(&message, message_ret);
        }
     
      if(message.func_number == IH_MODULE_FUNCTION_ADV_TEST )  //zaawansowane funkcje zdarzeń...
        {
           return this->check_adv( &message, message_ret );      
        }
     
     if(message.func_number == IH_MODULE_FUNCTION_NOTICE_QUERY )  //PYTANIE TAK NIE
        {
           return this->notice_query(&message, message_ret);  
        }
    
      if(message.func_number == IH_MODULE_FUNCTION_NOTICE_MSG )  //wiadomość - powiadomoenie
        {
           return this->notice(&message, message_ret);  
        }
     
      if(message.func_number == IH_MODULE_FUNCTION_NOTICE_UPDATE )  //powiadom o nowej wersji
        {
           return this->notice_version(&message, message_ret);  
        }
     
     if(message.func_number == IH_MODULE_FUNCTION_CHECK_VER )  //SPRAWDZAJ WERSJE...
        {
           return this->checkVersion(&message, message_ret);  
        }
     
     if(message.func_number == IH_MODULE_FUNCTION_GO_UPDATE )  //go update
        {
           return this->goUpdate(&message, message_ret);  
        }
     
    
     if(inMsg->gxh_handle > 0)
     {
          const char* response = "Module Error"; 
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;
          
     }else return false;
}


//-------------------------------------------------------+----------------------

 bool app_module_system::goUpdate(gxh_message* inMsg, gxh_message* outMsg)
 {                  
    
     this->executeSystemConsole(inMsg, outMsg, 500 );
     
     
     if(inMsg->gxh_handle > 0)
     {
          const char* response = "OK"; 
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(outMsg, &msg, sizeof(gxh_message));            
          return true;
          
     }else return false;
 }

//-------------------------------------------------------+-----------------------

bool app_module_system::checkVersion(gxh_message* inMsg, gxh_message* outMsg)
{
    
    
    return false;
     this->getAppHandle()->show_log_module("app_module_system","checkVersion","check...",4);
    
     unsigned int countT = 0;
     ih_driver_values* value   = (ih_driver_values*)this->getAppHandle()->getResource(RES_VECTOR_DRIVER_VALUE, &countT); 
     
     int indexNow    = -1; //aktualna wresja
     int indexNew    = -1; //stara wersja..
     int indexISNew  = -1; //aktualizaje gotowe do pobrania..
     
     for(unsigned int i=0; i<countT; i++)
     {
         if(value[i].ih_driver_id == -1 && value[i].px_group_id==-1 && value[i].const_function == VAR_VERSION_NOW )
         {
             indexNow = i;
         }
         
         if(value[i].ih_driver_id == -1 && value[i].px_group_id==-1 && value[i].const_function == VAR_VERSION_NEW )
         {
             indexNew = i;
         }
         
         if(value[i].ih_driver_id == -1 && value[i].px_group_id==-1 && value[i].const_function == VAR_VERSION_ISNEW )
         {
             indexISNew = i;
         }
     }
     
     if(indexNow >= 0)
     {
         std::string ver = this->getAppHandle()->getVersion();
         
         memset(value[indexNow].value,0,20);
         memcpy(value[indexNow].value, ver.c_str(), ver.length() );
         value[indexNow].status =1;
     }
     
     
     if(indexNew >= 0)
     {
         
         //------------------------------------------
         //------------------------------------------
       
          std::string hostStr =  "http://panel.gxh.pl/api,get_new_version,name,";                                     
          hostStr.append( this->getAppHandle()->getComp() ); 
          hostStr.append( ",key," ); 
          hostStr.append( this->getAppHandle()->getKey() ); 
          
          char bufferHost[1024];
          char params[1024];
          this->getHost( (char*)hostStr.c_str() ,bufferHost, params);
          
          
          this->getAppHandle()->show_log_module("app_module_system","checkVersion",bufferHost,4 );
          
          std::string POST_DATA ="ver=";
          POST_DATA.append("true"); 
          
          
          std::string URL = "";
          URL.append(bufferHost);
          
          std::string POST = "";
          POST.append("POST ");
          POST.append(params);
          POST.append(" HTTP/1.0\r\n");
          
          POST.append("Host: ");
          POST.append(bufferHost);
          POST.append("\r\n");
          
          POST.append("User-Agent: ");
          POST.append("GXH web plugin/1.0 Błażej Kita, GXH ");
          POST.append(this->getAppHandle()->getVersion());
          POST.append("\r\n");
          
          POST.append("Content-Type: ");
          POST.append("application/x-www-form-urlencoded");
          POST.append("\r\n");
          
          POST.append("Content-Length: ");
          POST.append( gxh_IntToString( POST_DATA.length() ) );
          POST.append("\r\n");
          
          POST.append("\r\n");     
          POST.append(POST_DATA);
          
          
          this->getAppHandle()->show_log_module("app_module_system","checkVersion",POST.c_str(),4 );
          
          
          /* first what are we going to send and where are we going to send it? */
          int portno =        80;
          const char *host =        URL.c_str();
          const char *message_fmt = POST.c_str();  // "POST /apikey=%s&command=%s HTTP/1.0\r\n\r\n";

          struct hostent *server;
          struct sockaddr_in serv_addr;
          int sockfd, bytes, sent, received, total;
          char message[4096],response_serv[4096];
        

          /* fill in the parameters */      
          memcpy(message, message_fmt, strlen(message_fmt));

          /* create the socket */
         sockfd = socket(AF_INET, SOCK_STREAM, 0);
         if (sockfd < 0)
         {                        
             this->getAppHandle()->show_error("app_module_system","checkVersion","error opening socket",1);
             return false;
         }

          /* lookup the ip address */
         server = gethostbyname(host);
         if (server == NULL)
         {     
              close(sockfd);
              this->getAppHandle()->show_error("app_module_system","checkVersion","not such host",1);
              return false;
         }

         /* fill in the structure */
         memset(&serv_addr,0,sizeof(serv_addr));
         serv_addr.sin_family = AF_INET;
         serv_addr.sin_port = htons(portno);
         memcpy(&serv_addr.sin_addr.s_addr,server->h_addr,server->h_length);

         /* connect the socket */
         if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
         {
               this->getAppHandle()->show_error("app_module_system","checkVersion","error connection",1);
               close(sockfd);
               return false;
         }

         /* send the request */
         total = strlen(message);
         sent = 0;
         do 
         {
          bytes = write(sockfd,message+sent,total-sent);
           if (bytes < 0)
           {      
               this->getAppHandle()->show_error("app_module_system","checkVersion","ERROR writing message to socket",1);
           }
            if (bytes == 0)
             break;
             sent+=bytes;
         } while (sent < total);

         
        /* receive the response */
        memset(response_serv,0,sizeof(response_serv));
        total = sizeof(response_serv)-1;
        received = 0;
        do {
           bytes = read(sockfd,response_serv+received,total-received);
           if (bytes < 0)
           {              
              this->getAppHandle()->show_error("app_module_system","checkVersion","ERROR reading response from socket",1);
           }
           if (bytes == 0)
              break;
           received+=bytes;
           } while (received < total);

        if (received == total)
        {            
             this->getAppHandle()->show_error("app_module_system","checkVersion","ERROR storing complete response from socket",1);
        }

     
       close(sockfd);
     
       
       if(response_serv[0] == 'H' && response_serv[1] == 'T' && response_serv[2] == 'T' && response_serv[3] == 'P')
       {
           char br[4096];
           memset(br,0,4096);
           int indexR = 0;
           unsigned int first = 0;
           for(unsigned int u=0; u< 4096-4;u++)
           {
               if(response_serv[u] == '\r' &&response_serv[u+1] =='\n' && response_serv[u+2] == '\r' &&response_serv[u+3] =='\n')
               {
                   first = u +4;
                   break;
               }
           }
           
           for(unsigned int u=first; u< 4096-1;u++)
           {
               if(response_serv[u] == 0) break;
               br[indexR] = response_serv[u];
               indexR++;
           }
           
           if(br[0] != '?')
           {           
                memset(value[indexNew].value,0,20);
                memcpy(value[indexNew].value, br,  strlen(br) );
                value[indexNew].status =1;
           }else
           {
                std::string ver = this->getAppHandle()->getVersion();         
                memset(value[indexNew].value,0,20);
                memcpy(value[indexNew].value, ver.c_str(), ver.length() );
                value[indexNew].status =1;
           }
       }
       
         //------------------------------------------
         //------------------------------------------
     }
    
     
     if(indexISNew >=0 && indexNew>=0 && indexNow >=0 )
     {
        if(gxh_compare(value[indexNew].value, value[indexNow].value) == 0)   
        {
            if(value[indexISNew].value[0] == '1')
            {
            value[indexISNew].value[0] ='0';
            value[indexISNew].value[1] =0;
            value[indexISNew].status = 1;
            }
        }else
        {
            if(value[indexISNew].value[0] == '0')
            {
            value[indexISNew].value[0] ='1';
            value[indexISNew].value[1] =0;
            value[indexISNew].status = 1;
            }
        }
         
     }
     
     
     return false;    
}

//-------------------------------------------------------+-----------------------

//------------------------------------------------------------------------------

void app_module_system::getHost(char* from, char* to,char* param)
{
    int length = strlen(from);
    int start = 0;    
    int startParam = 0;
    
    for(int i=1; i<length; i++)
    {
        if(from[i-1] == '/' && from[i] == '/' ) //http://
        {
            start = i+1;
        }
    }
    
    int ch = 0;
    
    for(int i=start; i<length; i++)
    {
        if( from[i] == '/' ) // .pl/
        {
            startParam = i;
            break;
        }
        
        to[ch] = from[i];
        ch++;
    }
    
    
    to[ch] = '\0';
 
    
    int p = 0;
    for(int i=startParam; i<length; i++)
    {
        param[p] = from[i];
        p++;
    }
    
    param[p] = '\0';
    
    
}

//-------------------------------------------------------+-----------------------

 bool app_module_system::notice_query(gxh_message* inMsg, gxh_message* outMsg)
 {
     
      gxh_message message;
      memcpy(&message, inMsg, sizeof(gxh_message));
      
      
      ih_driver_port_function functionList;
      memcpy(&functionList,inMsg->dataStruct, sizeof(ih_driver_port_function) );
     
      
     this->getAppHandle()->addNotice( functionList.resource_id , "GXH", functionList.name , functionList.text_content , IH_MODULE_FUNCTION_NOTICE_QUERY_YES_NO , (ih_notice_ex*)functionList.ex_tmp_struct  );
      
     
     if(inMsg->gxh_handle > 0)
     {
          const char* response = "OK"; 
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(outMsg, &msg, sizeof(gxh_message));            
          return true;
     }
     return false;
 }
 
 //-------------------------------------------------------+-----------------------

 bool app_module_system::notice(gxh_message* inMsg, gxh_message* outMsg)
 {
     
      gxh_message message;
      memcpy(&message, inMsg, sizeof(gxh_message));
      
      
      ih_driver_port_function functionList;
      memcpy(&functionList,inMsg->dataStruct, sizeof(ih_driver_port_function) );
     
      
     this->getAppHandle()->addNotice( functionList.resource_id , "GXH", functionList.name , functionList.text_content , IH_MODULE_FUNCTION_NOTICE_NORMAL , NULL  );
      
     
     if(inMsg->gxh_handle > 0)
     {
          const char* response = "OK"; 
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(outMsg, &msg, sizeof(gxh_message));            
          return true;
     }
     return false;
 }

//-------------------------------------------------------+-----------------------

//-------------------------------------------------------+-----------------------

 bool app_module_system::notice_version(gxh_message* inMsg, gxh_message* outMsg)
 {
     
      gxh_message message;
      memcpy(&message, inMsg, sizeof(gxh_message));
      
      
      ih_driver_port_function functionList;
      memcpy(&functionList,inMsg->dataStruct, sizeof(ih_driver_port_function) );
     

      unsigned int countT = 0;
      ih_driver_values* value   = (ih_driver_values*)this->getAppHandle()->getResource(RES_VECTOR_DRIVER_VALUE, &countT);
      
      for(unsigned int y=0; y<countT; y++)
      {
          if(value[y].px_group_id == -1 && value[y].ih_driver_id == -1 && value[y].const_function == VAR_VERSION_NEW)
          {              
             this->getAppHandle()->addNotice( functionList.resource_id , "GXH", "GXH Update" , value[y].value , IH_MODULE_FUNCTION_NOTICE_USER_UPDATE ,  NULL  );
             break;
          }
      }
      
      
     
     if(inMsg->gxh_handle > 0)
     {
          const char* response = "OK"; 
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(outMsg, &msg, sizeof(gxh_message));            
          return true;
     }
     return false;
 } 
 
//-------------------------------------------------------+-----------------------

bool app_module_system::lightBackup(gxh_message* inMsg, gxh_message* message_ret) 
{    
    this->getAppHandle()->show_log_module("app_module_system","lightBackup","ok",2);
 
     unsigned int count = 0;
     ih_lighting* first = (ih_lighting*)this->getAppHandle()->getResource(RES_VECTOR_LIGHT, &count);
    
     for(unsigned int x=0; x<count; x++)
     {
         if(first[x].status == '1')
         {
            gxh_message inM;
            gxh_message outM;
            this->executeFunctionParam(&inM , &outM, IH_MODULE_FUNCTION_LIGHT_ON , first[x].id ,   ""  ,   "", NULL );                      
         }
     }
     
    return false; //dla usług nie zwracamy odpowiedzi..
}

//-------------------------------------------------------+-----------------------

double app_module_system::Distance( double lat1,  double lon1,  double lat2,  double lon2) {
   
  //  double R    = 6371000; // Radius of the earth in m
 //   double dLat = (lat1 - lat2) * M_PI / 180;
 //   double dLon = (lon1 - lon2) * M_PI / 180;
 //   double a    = sin(dLat/2) * sin(dLat/2) + cos(lat1 * M_PI / 180) * cos(lat2 * M_PI / 180) * sin(dLon/2) * sin(dLon/2);
 //   double c    = 2 * atan2(sqrt(a), sqrt(1-a));
 //   double d    = R * c;
 //   return d;
             	

       double theta, dist;
       theta = lon1 - lon2;
       dist = sin( M_PI  / 180 * lat1) * sin( M_PI / 180 * lat2 ) + cos( M_PI / 180 * lat1) * cos( M_PI / 180 * lat2) * cos( M_PI / 180 * theta );
       dist = acos(dist);
       dist = dist * 180 / M_PI;
       dist = dist * 60 * 1.1515;
   
        dist = dist * 1.609344; // zamien na kilometry...
        
        dist = dist * 1000; //zamien na metry....
   
       return dist;
}

//-------------------------------------------------------+-----------------------

int app_module_system::locationToMetrs(const char* location)
{
    int indexLat = 0;
    int indexLng = 0;
    
    char lat[10];
    char lng[10];
    
    memset(lat,0, 10);
    memset(lng,0, 10);
    
    for(unsigned int i=0; i<strlen(location); i++)
    {
        if(location[i] == ':')break;
        
        if(indexLat >9) break;
        lat[indexLat] = location[i];
        indexLat++;
    }
    
    for(unsigned int i=indexLat+1; i<strlen(location); i++)
    { 
        
        if(indexLng >9) break;
        lng[indexLng] = location[i];
        indexLng++;
    }
    
    double latUsr = gxh_StringToDouble(lat);
    double lngUsr = gxh_StringToDouble(lng);
    
    unsigned int count = 0;
    ih_home* homes = (ih_home*)this->getAppHandle()->getResource(RES_VECTOR_HOME, &count );  
   
    if(count >0)
    {
        double latHome = gxh_StringToDouble(homes[0].latitude);
        double lngHome = gxh_StringToDouble(homes[0].longitude);
        
        double pozM = this->Distance(latUsr, lngUsr,latHome, lngHome );
        
        return pozM;
    }
    
    
    return -1;
}

//-------------------------------------------------------+-----------------------

bool app_module_system::check_adv(gxh_message* inMsg, gxh_message* outMsg)
{
    
    for(int x=0; x<this->count_func; x++)
    {
        bool allTrue  = true;       
        
        
        //sprawdz funkcje wejsciowe dla tej funkcji..
        for(int y=0; y<this->count_func_var; y++)
        { 
            if(this->func_var[y].ih_adv_function_id == this->func[x].id && this->func_var[y].type_ion[0]=='I')
            {                               
                 if( gxh_compare( this->func_var[y].resource_name,"light") == 0 && this->func_var[y].typ_wartosci == 1 && this->func_var[y].wartosc_jest_rowna==1 )    //swiatło ON OFF
                 {                                            
                     unsigned int countT = 0;
                     ih_lighting* lights   = (ih_lighting*)this->getAppHandle()->getResource(RES_VECTOR_LIGHT, &countT); 
                     
                     for(unsigned r=0; r<countT; r++)
                     {
                         if(lights[r].id ==  this->func_var[y].resource_id)
                         {                       
                             if( this->func_var[y].wartosc[0] == '1'  && lights[r].status== '0'  ) allTrue = false;
                             if( this->func_var[y].wartosc[0] == '0'  && lights[r].status== '1'  ) allTrue = false;
                                                   
                         }
                     }
                 }
          
            
                                          
                 if( gxh_compare( this->func_var[y].resource_name,"blind") == 0 && this->func_var[y].typ_wartosci == 1 && this->func_var[y].wartosc_jest_rowna==1 )    //roleta
                 {                                            
                     unsigned int countT = 0;
                     ih_blind* blinds   = (ih_blind*)this->getAppHandle()->getResource(RES_VECTOR_BLIND, &countT); 
                     
                     for(unsigned r=0; r<countT; r++)
                     {
                         if(blinds[r].id ==  this->func_var[y].resource_id)
                         {                       
                             if( gxh_compare( this->func_var[y].wartosc,"0")==  0  && blinds[r].open_percent== 100  ) allTrue = false;
                             if( gxh_compare( this->func_var[y].wartosc,"100")==0   && blinds[r].open_percent== 0  ) allTrue = false;
                                                   
                         }
                     }
                 }
          
            
                                         
                 if( gxh_compare( this->func_var[y].resource_name,"outs") == 0 && this->func_var[y].typ_wartosci == 1  && this->func_var[y].wartosc_jest_rowna==1 )    //gniazdka wyjścia
                 {        
                     unsigned int countT = 0;
                     ih_outs* outs   = (ih_outs*)this->getAppHandle()->getResource(RES_VECTOR_OUTS, &countT); 
                     
                    //  std::cout<<"\n\n\nwarunek aouts"<<countT ;
                     
                     for(unsigned r=0; r<countT; r++)
                     {
                         if(outs[r].id ==  this->func_var[y].resource_id)
                         {   
                            // std::cout<<"\nWartosc: "<< outs[r].status<<" | "<< this->func_var[y].wartosc <<" |X";
                             
                             if(  this->func_var[y].wartosc[0] == '1'   && outs[r].status== '0'  ) allTrue = false;
                             if(  this->func_var[y].wartosc[0] == '0'   && outs[r].status== '1'  ) allTrue = false;
                                                   
                         }
                     }
                 }
          
                                        
                 if( gxh_compare( this->func_var[y].resource_name,"sensor") == 0 && this->func_var[y].typ_wartosci == 1 && this->func_var[y].wartosc_jest_rowna==1 )    //czujniki on off
                 {        
                     unsigned int countT = 0;
                     ih_sensor* sensor   = (ih_sensor*)this->getAppHandle()->getResource(RES_VECTOR_SENSOR, &countT); 
                     
                    //  std::cout<<"\n\n\nwarunek aouts"<<countT ;
                     
                     for(unsigned r=0; r<countT; r++)
                     {
                         if(sensor[r].id ==  this->func_var[y].resource_id)
                         {   
                          //   std::cout<<"\nNazwa: "<<sensor[r].name<<"\nWartosc: "<< sensor[r].status<<" | "<< this->func_var[y].wartosc <<" |X";
                             
                             if(  this->func_var[y].wartosc[0] == '1'   && sensor[r].status== 0  ) allTrue = false;
                             if(  this->func_var[y].wartosc[0] == '0'   && sensor[r].status== 1  ) allTrue = false;                                                   
                         }
                     }
                 }
        
            
                                       
                 if( gxh_compare( this->func_var[y].resource_name,"gate") == 0 && this->func_var[y].typ_wartosci == 1  && this->func_var[y].wartosc_jest_rowna==1 )    //gate
                 {                                            
                     unsigned int countT = 0;
                     ih_gate* gates   = (ih_gate*)this->getAppHandle()->getResource(RES_VECTOR_GATE, &countT); 
                     
                     for(unsigned r=0; r<countT; r++)
                     {
                         if(gates[r].id ==  this->func_var[y].resource_id)
                         {                       
                             if( gxh_compare( this->func_var[y].wartosc,"0")== 0  && gates[r].open_percent== 100  ) allTrue = false;
                             if( gxh_compare( this->func_var[y].wartosc,"100")==0   && gates[r].open_percent== 0  ) allTrue = false;                                                   
                         }
                     }
                 }
           
            
                                         
                 if( gxh_compare( this->func_var[y].resource_name,"var") == 0 && this->func_var[y].typ_wartosci == 1 )    //var porownanie wedlug liczby
                 {                                            
                     unsigned int countT = 0;
                     ih_driver_values* value   = (ih_driver_values*)this->getAppHandle()->getResource(RES_VECTOR_DRIVER_VALUE, &countT); 
                     
                     for(unsigned r=0; r<countT; r++)
                     {
                         if(value[r].id ==  this->func_var[y].resource_id)
                         {       
                             if( this->func_var[y].wartosc_jest_rowna==1 )
                             {
                                if( gxh_StringToDouble(this->func_var[y].wartosc) !=  gxh_StringToDouble(value[r].value) ) allTrue = false;                                              
                             }
                             
                             if(this->func_var[y].wartosc_jest_mniejsza==1)
                             {
                                if(  !(gxh_StringToDouble(this->func_var[y].wartosc) >  gxh_StringToDouble(value[r].value)) ) allTrue = false;                                              
                             }
                             
                             if(this->func_var[y].wartosc_jest_wieksza==1 )
                             {
                                if( !(gxh_StringToDouble(this->func_var[y].wartosc) <  gxh_StringToDouble(value[r].value)) ) allTrue = false;                                              
                             }
                         }
                     }
                 }
                 
                 if( gxh_compare( this->func_var[y].resource_name,"var") == 0 && this->func_var[y].typ_wartosci == 5 )    //var pozycja gps metry od obiektu..
                 {                                            
                     unsigned int countT = 0;
                     ih_driver_values* value   = (ih_driver_values*)this->getAppHandle()->getResource(RES_VECTOR_DRIVER_VALUE, &countT); 
                     
                     for(unsigned r=0; r<countT; r++)
                     {
                         if(value[r].id ==  this->func_var[y].resource_id)
                         {    
                             int metry = locationToMetrs( (const char*)value[r].value );
                             
                             
                           
                             if(metry > 0 && this->func_var[y].wartosc_jest_rowna==1    && gxh_StringToInt(this->func_var[y].wartosc) !=  metry ) allTrue = false;                                              
                             if(metry > 0 && this->func_var[y].wartosc_jest_mniejsza==1 && !(gxh_StringToInt(this->func_var[y].wartosc) >  metry ) ) allTrue = false;                                              
                             if(metry > 0 && this->func_var[y].wartosc_jest_wieksza==1  && !(gxh_StringToInt(this->func_var[y].wartosc) < metry ) ) allTrue = false;                                              
                             
                             if(allTrue)                             
                             {
                                 std::string msgT = "Warunki dotyczace odleglosci zostaly spelnione: ";
                                 msgT.append( gxh_IntToString(metry) );
                                 msgT.append( " metrów, użytkownik: " );
                                 msgT.append( value[r].name_param );
                                 this->getAppHandle()->show_log_module("app_module_system","check_adv",msgT.c_str(),11);
                             }
                         }
                     }
                 }
                 
                 
                 if( gxh_compare( this->func_var[y].resource_name,"var") == 0 && this->func_var[y].typ_wartosci == 2 )    //takładny tekst
                 {                                            
                     unsigned int countT = 0;
                     ih_driver_values* value   = (ih_driver_values*)this->getAppHandle()->getResource(RES_VECTOR_DRIVER_VALUE, &countT); 
                     
                     for(unsigned r=0; r<countT; r++)
                     {
                         if(value[r].id ==  this->func_var[y].resource_id)
                         {    
                             //   (const char*)value[r].value
                            // this->func_var[y].wartosc//
                           
                             if(strlen( value[r].value ) == strlen( this->func_var[y].wartosc ) )
                             {
                                 for(unsigned w=0; w<strlen(value[r].value ); w++ )
                                 {
                                     if(value[r].value[w] != this->func_var[y].wartosc[w])  allTrue = false;
                                 }
                                 
                             }else                              
                             allTrue = false;   
                                     
                             if(allTrue)                             
                             {
                                 std::string msgT = "Warunki dotyczace podanego tekstu zostaly spelnione: ";                                                  
                                 msgT.append( value[r].name_param );
                                 this->getAppHandle()->show_log_module("app_module_system","check_adv",msgT.c_str(),11);
                             }
                         }
                     }
                 }
                 
                 
                 
                 if( gxh_compare( this->func_var[y].resource_name,"var") == 0 && this->func_var[y].typ_wartosci == 3 )    //fraza zawarta w odczytaen wartosci...
                 {                                            
                     unsigned int countT = 0;
                     ih_driver_values* value   = (ih_driver_values*)this->getAppHandle()->getResource(RES_VECTOR_DRIVER_VALUE, &countT); 
                     
                     for(unsigned r=0; r<countT; r++)
                     {
                         if(value[r].id ==  this->func_var[y].resource_id)
                         {    
                             //   (const char*)value[r].value
                            // this->func_var[y].wartosc//
                              
                             std::string str(value[r].value);
                             std::string str2( this->func_var[y].wartosc);
                             
                           //  std::cout<<"\nTEkst<< "<<value[r].value<<" | "<<this->func_var[y].wartosc<<" | "<<str2.find(str) ;
                             
                             if (str2.find(str) != std::string::npos) {
                                 //.. found.
                              } else allTrue = false;
                                                                                                                  
                                     
                             if(allTrue)                             
                             {
                                 std::string msgT = "Warunki dotyczace podanego tekstu zostaly spelnione: ";                         
                                 msgT.append( value[r].name_param );
                                 this->getAppHandle()->show_log_module("app_module_system","check_adv",msgT.c_str(),11);
                             }
                         }
                     }
                 }
                 
                 
                 if( gxh_compare( this->func_var[y].resource_name,"other") == 0 && this->func_var[y].typ_wartosci == 61 )    //dzień miesiaca
                 {                                                                                                                                                          
                             time_t theTime = time(NULL);
                             struct tm *aTime = localtime(&theTime);

                             int day = aTime->tm_mday;
                             //int month = aTime->tm_mon + 1; // Month is 0 - 11, add 1 to get a jan-dec 1-12 concept
                             //int year = aTime->tm_year + 1900; // Year is # years since 1900                                                                                     
                             
                         
                             if(this->func_var[y].wartosc_jest_rowna==1) 
                             {
                               if((day > 0  && gxh_StringToInt(this->func_var[y].wartosc) ==  day )==false) allTrue = false;                                              
                             }
                             
                             if(this->func_var[y].wartosc_jest_mniejsza==1)
                             {
                                if((day > 0 && (gxh_StringToInt(this->func_var[y].wartosc) >  day ) )==false) allTrue = false;                                              
                             }
                             
                             if(this->func_var[y].wartosc_jest_wieksza==1)
                             {
                                if((day > 0 && (gxh_StringToInt(this->func_var[y].wartosc) < day ))==false ) allTrue = false;                                              
                             }
                             
                             if(allTrue)                             
                             {
                                 std::string msgT = "Warunki dotyczace dnia miesiaca zostaly spelnione: ";
                                 msgT.append( this->func_var[y].wartosc );                            
                                 this->getAppHandle()->show_log_module("app_module_system","check_adv",msgT.c_str(),11);
                             }                                              
                 }
                 
                 
                 if( gxh_compare( this->func_var[y].resource_name,"other") == 0 && this->func_var[y].typ_wartosci == 62 )    //numer miesiaca
                 {                                                                                                                                                          
                             time_t theTime = time(NULL);
                             struct tm *aTime = localtime(&theTime);

                             //int day = aTime->tm_mday;
                             int month = aTime->tm_mon + 1; // Month is 0 - 11, add 1 to get a jan-dec 1-12 concept
                             //int year = aTime->tm_year + 1900; // Year is # years since 1900                                                                                     
                             
                             
                           
                             if(this->func_var[y].wartosc_jest_rowna==1 ) 
                             {
                                if((month > 0 &&  gxh_StringToInt(this->func_var[y].wartosc) ==   month)==false ) allTrue = false;                                              
                             }
                             
                             if(this->func_var[y].wartosc_jest_mniejsza==1 )
                             {
                               if((month > 0 && (gxh_StringToInt(this->func_var[y].wartosc) >  month ) )==false) allTrue = false;                                              
                             }
                             
                             if( this->func_var[y].wartosc_jest_wieksza==1)
                             {
                               if((month > 0 && (gxh_StringToInt(this->func_var[y].wartosc) <  month ) ) ==false )allTrue = false;                                              
                             }
                             
                             if(allTrue)                             
                             {
                                 std::string msgT = "Warunki dotyczace numeru miesiaca zostaly spelnione: ";
                                 msgT.append( this->func_var[y].wartosc );                            
                                 this->getAppHandle()->show_log_module("app_module_system","check_adv",msgT.c_str(),11);
                             }                                              
                 }
                 
                 
                 if( gxh_compare( this->func_var[y].resource_name,"other") == 0 && this->func_var[y].typ_wartosci == 60 )    //godzina GG:MM
                 {                                                                                                                                                          
                             time_t theTime = time(NULL);
                             struct tm *aTime = localtime(&theTime);

                             //int day = aTime->tm_mday;
                             //int month = aTime->tm_mon + 1; // Month is 0 - 11, add 1 to get a jan-dec 1-12 concept
                             //int year = aTime->tm_year + 1900; // Year is # years since 1900                                                                                     
                             
                             int hour   = aTime->tm_hour;
                             int minute = aTime->tm_min;
                             //int second = aTime->tm_sec;
                                        
                             int godzina = -1;
                             int minuta  = -1;
                             
                             int sekWartosc = -1;
                             
                             
                             int sekNow = hour * 3600;
                                 sekNow += minute * 60;
                                 
                              //   std::cout<<"\n\nGodzina:.... "<<this->func_var[y].wartosc<<", "<<strlen(this->func_var[y].wartosc);
                             
                             if(strlen(this->func_var[y].wartosc) == 5)
                             {
                                 char g[3];  memset(g,0,3);
                                 char m[3];  memset(m,0,3);
                                 
                                 g[0] = this->func_var[y].wartosc[0];
                                 g[1] = this->func_var[y].wartosc[1];
                                 
                                 m[0] = this->func_var[y].wartosc[3];
                                 m[1] = this->func_var[y].wartosc[4];
                                 
                                 godzina = gxh_StringToInt(g);
                                 minuta = gxh_StringToInt(m);
                                 
                                 sekWartosc = godzina * 3600;
                                 sekWartosc += minuta * 60;
                                 
                               //  std::cout<<"\n\nT: "<<g<<", "<<m<<", "<<godzina<<" | "<<minuta<<" | \n";                                                                  
                             }
                             
                              //std::cout<<"\n\nNow: "<<sekNow<<", "<<sekWartosc<<"\n";   
                                
                            if(this->func_var[y].wartosc_jest_rowna==1 )
                            {
                                if( (sekWartosc > 0 && sekWartosc  == sekNow ) == false) allTrue = false;
                            }
                                
                             if(this->func_var[y].wartosc_jest_mniejsza==1)
                             {
                                if( (sekWartosc > 0 && (sekWartosc >  sekNow ) ) == false) allTrue = false; 
                             }
                               
                             if(this->func_var[y].wartosc_jest_wieksza==1)    
                             {
                               if( (sekWartosc > 0 &&  (sekWartosc <  sekNow ) ) == false) allTrue = false; 
                             }
                              
                                // std::cout<<"\nBBB: "<<sekWartosc<<" | "<<sekNow<<" | "<<allTrue;
                                 
                             if(allTrue)                             
                             {
                                 std::string msgT = "Warunki dotyczace godziny i minuty zostaly spelnione: ";
                                 msgT.append( gxh_IntToString(godzina));                            
                                 msgT.append(":");                            
                                 msgT.append( gxh_IntToString(minuta));                            
                                 msgT.append(  " -- ");                            
                                 msgT.append( gxh_IntToString(hour));
                                 msgT.append( ":");
                                 msgT.append( gxh_IntToString(minute));
                                 this->getAppHandle()->show_log_module("app_module_system","check_adv",msgT.c_str(),7);
                             }                                              
                 }
                 
                 
                 
                 if( gxh_compare( this->func_var[y].resource_name,"other") == 0 && this->func_var[y].typ_wartosci == 63 )    //godzina RRRR-MM-DD
                 {                                                                                                                                                          
                             time_t theTime = time(NULL);
                             struct tm *aTime = localtime(&theTime);

                             int day = aTime->tm_mday;
                             int month = aTime->tm_mon + 1; // Month is 0 - 11, add 1 to get a jan-dec 1-12 concept
                             int year = aTime->tm_year + 1900 - 2000; // Year is # years since 1900                                                                                     
                             
                             //int hour   = aTime->tm_hour;
                             //int minute = aTime->tm_min;
                             //int second = aTime->tm_sec;
                                        
                             int rok = -1;
                             int miesiac  = -1;
                             int dzien  = -1;
                             
                             int dayWartosc = -1;
                             
                             
                             int dayNow = day;
                                 dayNow += month * 12;
                                 dayNow += year * 366; //od 200 roku..
                                 
                              //   std::cout<<"\n\nGodzina:.... "<<this->func_var[y].wartosc<<", "<<strlen(this->func_var[y].wartosc);
                             
                             if(strlen(this->func_var[y].wartosc) == 10)
                             {
                                 char yr[5];  memset(yr,0,5);
                                 char m[3];  memset(m,0,3);
                                 char d[3];  memset(d,0,3);
                                 
                                 yr[0] = this->func_var[y].wartosc[0];
                                 yr[1] = this->func_var[y].wartosc[1];
                                 yr[2] = this->func_var[y].wartosc[2];
                                 yr[3] = this->func_var[y].wartosc[3];
                                 
                                 m[0] = this->func_var[y].wartosc[5];
                                 m[1] = this->func_var[y].wartosc[6];
                                 
                                 d[0] = this->func_var[y].wartosc[8];
                                 d[1] = this->func_var[y].wartosc[9];
                                 
                                 rok      = gxh_StringToInt(yr) - 2000; //liczmy wszedzie od 2000
                                 miesiac  = gxh_StringToInt(m);
                                 dzien    = gxh_StringToInt(d);
                                 
                                 dayWartosc = dzien;
                                 dayWartosc += miesiac * 12;
                                 dayWartosc += rok * 366;
                                 
                               //  std::cout<<"\n\nT: "<<g<<", "<<m<<", "<<godzina<<" | "<<minuta<<" | \n";                                                                  
                             }
                             
                           //  std::cout<<"\n\nNow: "<<rok<<", "<<miesiac<<", "<<dzien<< " || "<<dayWartosc<<" === "<<dayNow <<"\n";   
                            if(this->func_var[y].wartosc_jest_rowna==1)
                            {
                                if((dayWartosc > 0  &&  dayWartosc  == dayNow )==false) allTrue = false;                                              
                            }
                                
                            if( this->func_var[y].wartosc_jest_mniejsza==1 )
                            {
                                if((dayWartosc > 0 && (dayWartosc >  dayNow )) == false ) allTrue = false;                                              
                            }
                             
                            if(this->func_var[y].wartosc_jest_wieksza==1)
                            {
                                if((dayWartosc > 0 && (dayWartosc <  dayNow ))==false ) allTrue = false;                                              
                            }
                             
                             if(allTrue)                             
                             {
                                 std::string msgT = "Warunki dotyczace YYYY-MM-DD zostaly spelnione: ";
                                 msgT.append( gxh_IntToString(rok));                            
                                 msgT.append("-");                            
                                 msgT.append( gxh_IntToString(miesiac));                            
                                 msgT.append(  "-");                            
                                 msgT.append( gxh_IntToString(dzien));
                                 msgT.append( "");         
                                 this->getAppHandle()->show_log_module("app_module_system","check_adv",msgT.c_str(),11);
                             }                                              
                 }
                 
        
        
            }
        
        }               
        
      // std::cout<<"\nXXXX: "<<this->func[x].name<<" | "<<allTrue<<", "<< this->func[x].all_ok ;
        
        
        
      
        if(allTrue && this->func[x].all_ok == 0 )
        {                                        
             //wykonaj akcje dla spełnionych warunków
              for(int y=0; y<this->count_func_var; y++)
               { 
                  if(this->func_var[y].ih_adv_function_id == this->func[x].id && this->func_var[y].type_ion[0]=='O')
                  {    
                      gxh_message msgRet;        
                      gxh_message msgIn;        
                      
                      memcpy(msgIn.dataStruct, &this->func_var[y].notice_ex, sizeof(ih_notice_ex) );
                      msgIn.sizeDataStruct = sizeof(ih_notice_ex);                      
                      //   std::cout<<"\n\nParam1: "<<this->func_var[y].out_param1;
                      //   std::cout<<"\n\nParam2: "<<this->func_var[y].out_param2;                     
                      this->executeFunctionParam(&msgIn, &msgRet, this->func_var[y].out_const_function_symbol ,this->func_var[y].out_resource_id, std::string(this->func_var[y].out_param1), std::string(this->func_var[y].out_param2), -1, &this->func_var[y].notice_ex );                       
                    //  goSave = true;                      
                  }
              }
             //--             
              std::string msg = "Wszystkie warunki zostały spełnione: ";
              msg.append( this->func[x].name);             
              this->getAppHandle()->show_log_module("app_module_system","check_adv",msg.c_str(), 4);    
                                       
              this->func[x].all_ok = 1;
              //  this->func[x].all_nok = 0;
              this->func[x].toSave = true; 
             
        }     
        
        
        if(!allTrue && this->func[x].all_ok == 1)
        {                                                 
             //wykonaj akcje dla spełnionych warunków
              for(int y=0; y<this->count_func_var; y++)
               { 
                  if(this->func_var[y].ih_adv_function_id == this->func[x].id && this->func_var[y].type_ion[0]=='N')
                  {    
                      gxh_message msgRet;        
                      gxh_message msgIn;        
                      this->executeFunctionParam(&msgIn, &msgRet, this->func_var[y].out_const_function_symbol ,this->func_var[y].out_resource_id, std::string(this->func_var[y].out_param1), std::string(this->func_var[y].out_param2), -1, &this->func_var[y].notice_ex );                                                                   
                  }
              }
             
             
               std::string msg = "Warunki nie zostały spełnione: ";
               msg.append( this->func[x].name);             
               this->getAppHandle()->show_log_module("app_module_system","check_adv",msg.c_str(), 4);  
                 
      
               this->func[x].all_ok = 0;
               //this->func[x].all_nok = 1;
               this->func[x].toSave = true; 
            
        } 
        
    }
        
    return false;
}

//-------------------------------------------------------+-----------------------

bool app_module_system::restartHardware(gxh_message* inMsg, gxh_message* outMsg)
{
         gxh_message msgT;
         msgT.func_number  =  0;
         msgT.func_internal_number = 2; //restart komputer
         msgT.gxh_handle   = 0;      
    
         this->getAppHandle()->sendGXHMessage(&msgT);
    
         this->getAppHandle()->show_log_module("app_module_system","restartHardware","ok",2);
    
          const char* response = "OK"; 
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(outMsg, &msg, sizeof(gxh_message));            
          return true;
}

//-------------------------------------------------------+-----------------------

bool app_module_system::restartApp(gxh_message* inMsg, gxh_message* outMsg)
{
         gxh_message msgT;
         msgT.func_number  =  0;
         msgT.func_internal_number = 1; //restart aplication
         msgT.gxh_handle   = 0;      
    
         this->getAppHandle()->sendGXHMessage(&msgT);
    
         this->getAppHandle()->show_log_module("app_module_system","restartApp","ok",2);
    
          const char* response = "OK"; 
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(outMsg, &msg, sizeof(gxh_message));            
          return true;
}

//-------------------------------------------------------+-----------------------

bool app_module_system::setPort(gxh_message* inMsg, gxh_message* outMsg, int status)
{
          bool isOk = false;
          
          ih_driver_port_function functionList;
          memcpy(&functionList, inMsg->dataStruct, sizeof(ih_driver_port_function) );
          
          if(status == 0)
          {
             isOk = this->setPortStatus( functionList.resource_id , IH_SET_PORT_OFF,-1,-1,-1  );   
          }
          
          if(status == 1)
          {
             isOk = this->setPortStatus( functionList.resource_id  , IH_SET_PORT_ON,-1,-1,-1  );   
          }
          
          if(status == 2)
          {
             isOk = this->setPortStatus( functionList.resource_id   , IH_SET_PORT_CLICK, 2000,-1,-1  );   //2 sek..
          }
    
          const char* response = "OK"; 
          
          if(!isOk) response = "ERROR";
          
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(outMsg, &msg, sizeof(gxh_message));            
          return true;
}

//-------------------------------------------------------+-----------------------

bool app_module_system::setPortClick(gxh_message* inMsg, gxh_message* outMsg, int sek)
{
    
    
    
          bool isOk = false;
          
          ih_driver_port_function functionList;
          memcpy(&functionList, inMsg->dataStruct, sizeof(ih_driver_port_function) );
          
         isOk = this->setPortStatus( functionList.resource_id , IH_SET_PORT_CLICK,-1,1000 * sek,-1  );       
        
    
          const char* response = "OK"; 
          
          if(!isOk) response = "ERROR";
          
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(outMsg, &msg, sizeof(gxh_message));            
          return true;
}

//-------------------------------------------------------+-----------------------

bool app_module_system::check_internet(gxh_message* inMsg, gxh_message* outMsg)
{
      unsigned int size = 0;
      ih_driver_values* first = (ih_driver_values*)this->getAppHandle()->getResource(RES_VECTOR_DRIVER_VALUE, &size);
      
      struct hostent *hostinfo;
 
      const char* hostname = "google.com";
      hostinfo = gethostbyname (hostname);

      std::string status = "?";
    
      if (hostinfo == NULL) 
      {     
        this->getAppHandle()->show_error("app_module_system","check_internet","No internet",6);          
        status = "0";
      }
      else 
      {
        status = "1";
      } 
      
      //   std::cout<<"\n\ninternet status...."<<status.c_str();
      
      for(unsigned int x=0; x<size; x++)
      {
          if( (first[x].free == false) && (first[x].const_function == VAR_INTERNET_STATUS) &&  (first[x].ih_driver_id < 0) && (first[x].px_group_id < 0) )
          {
              if(gxh_compare(first[x].value, status.c_str()) != 0) 
              {
                memcpy(first[x].value, status.c_str(), status.length() );
                first[x].status =  1; //goupdate..
              }
          }
      }
      
      return false;
}

//-------------------------------------------------------+-----------------------