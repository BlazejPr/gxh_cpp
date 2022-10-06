
/* 
 * File:   app_module_elpiast.cpp
 * Author: Blazej Kita
 * 
 * Created on 25 styczeń 2019,  14:02
 */

#include "app_module_elpiast.h"

 
app_module* get_module(gxh_scope* appHomeHandle)
{
    return new app_module_elpiast(appHomeHandle); 
};
 
//-------------------------------------------------------------------------------
  
app_module_elpiast::app_module_elpiast(gxh_scope* appHomeHandle) : app_module(appHomeHandle) 
{           
    this->getAppHandle()->show_log_module("app_module_elpiast","app_module_elpiast","start.",3 );         
}

//------------------------------------------------------------------------------

app_module_elpiast::~app_module_elpiast() 
{     
    this->getAppHandle()->show_log_module("app_module_elpiast","app_module_elpiast","stop",3 );
}

//-------------------------------------------------------------------------------

void app_module_elpiast::onClick(int ih_driver_port_id, int listener_status)
{
}

//-------------------------------------------------------------------------------

void app_module_elpiast::forceCancel()
{
    
}

//-------------------------------------------------------------------------------

void app_module_elpiast::forceSynchronize()
{
     this->getAppHandle()->show_log_module("app_module_elpiast","forceSynchronize","start",20);  
     
     //posprzataj wpisy odnośnie histori kliknięć
     
                      sqlite3* dbSqLite2;                    
    
                      std::string baza = this->getAppHandle()->getDBTmp(); //this->getAppHandle()->getRunPath();
                      //baza.append("../gxh_base_tmp.db");
     
                      int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite2, SQLITE_OPEN_READWRITE, NULL   );                                  
                      if( r != SQLITE_OK )
                      {
                         this->getAppHandle()->show_error("app_module_elpiast","forceSynchronize","Cannot open sqlLite 1",0);                      
                      }else
                      {
                          sqlite3_stmt* stmt; 
                          int lastId = -1;
                          std::string query="select * from ih_driver_port_history ORDER BY id DESC LIMIT 1;";                            
                         
                          const char* queryChar = query.c_str();       
                          if(sqlite3_prepare(dbSqLite2 , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
                           {
                             this->getAppHandle()->show_error("app_module_elpiast","forceSynchronize", sqlite3_errstr( sqlite3_errcode(dbSqLite2  ) ),4);                                        
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
                            
                            if(lastId > 20)
                            {
                                std::string query = "DELETE FROM ih_driver_port_history;";
                                
                                char *zErrMsg = 0;     
                                int rc = sqlite3_exec( dbSqLite2 , query.c_str(), 0, 0, &zErrMsg);
                                if( rc != SQLITE_OK )
                                   {  
                                      this->getAppHandle()->show_error("app_module_elpiast","forceSynchronize", (const char*)zErrMsg ,0);   
                                      sqlite3_free(zErrMsg);    
                                   } 
                            }
                            
                          sqlite3_close_v2(dbSqLite2);                          
                       }  
     
     this->getAppHandle()->show_log_module("app_module_elpiast","forceSynchronize","stop",20);   
}

//-------------------------------------------------------------------------------
 
bool app_module_elpiast::execute(gxh_message* inMsg, gxh_message* outMsg)
{        
    
           gxh_message messageIn;
           memcpy(&messageIn, inMsg, sizeof(gxh_message));
           
           
          
           //dane wysylane.......
    
          if(messageIn.func_internal_number == IH_UPDATE_ADDRESS)     //zmiana adresu urzadzenia..
          {
            
          }
          
          
         
          if(messageIn.func_internal_number == IH_SET_PORT_DOUBLE_CLICK || messageIn.func_internal_number  == IH_SET_PORT_ON || messageIn.func_internal_number  == IH_SET_PORT_OFF || messageIn.func_internal_number  == IH_SET_PORT_CLICK )      
          {             
               
              this->getAppHandle()->show_log_module("app_module_elpiast","execute","change status of port",8 );
                    
              ih_change_port_info portInfo;             
              memcpy(&portInfo, messageIn.dataStruct, sizeof(ih_change_port_info) );
                          
              int ih_driver_id = -1; //this->getAppHandle()->getDriverIdByPort(this->getDbSqLite(),  portInfo.ih_driver_port_id, true );    
              
              int allP = 0;
              ih_driver_port* firstT = this->getAppHandle()->getPortsDriver(&allP);
              for(int x=0; x<allP; x++)
              {
                  if(firstT[x].id ==  portInfo.ih_driver_port_id)
                  {
                     ih_driver_id = firstT[x].ih_driver_id;
                     break;
                  }
              }
              
              if(ih_driver_id == -1) 
              {
                  return false;
              }              
              
              //td::cout<<"\n\n\n\Module portId "<< portInfo.ih_driver_port_id<<"\n\n";
              
              bool status = false;
              bool response_from_driver = false;
             
              portInfo.status = &status; //domyślnie polecenie nie zostało prawidłowo wykonane...
              portInfo.response_from_driver = &response_from_driver; //brak odpowiedzi o sterownika...
             
              
              
            
              
              gxh_message msgCommand;
              msgCommand.system_code = GXH_MESSAGE_REDIRECT; //przekaż bezpośrednia od gxhHandle....
              msgCommand.gxh_handle = this->getAppHandle()->getGXHHandleByIdAndtype(ih_driver_id, 1);//dirver handle....
              msgCommand.func_number = messageIn.func_number;        
              msgCommand.func_internal_number = messageIn.func_internal_number;
              memcpy(msgCommand.dataStruct, &portInfo, sizeof(ih_change_port_info) );
              msgCommand.sizeDataStruct = sizeof(ih_change_port_info);
                                   
          
              if( msgCommand.gxh_handle  <= 0)              
              {
                 this->getAppHandle()->show_error("app_module_elpiast","execute","module elpiast, device not registered!",5);               
                 return false;
              }
                 
              this->getAppHandle()->sendGXHMessage(&msgCommand);                             
              //czekaj na odpowiedz sterownika.....
                             
             
              for(int y=0; y<10; y++)
              {
                 usleep(1000 * 50); //
                 if(response_from_driver == true &&    status == true )
                 {
                    outMsg->gxh_handle = inMsg->gxh_handle;
                    outMsg->dataStruct[0] = 'O';
                    outMsg->dataStruct[1] = 'K';
                    outMsg->dataStruct[2] =  0;
                    outMsg->sizeDataStruct = 3;
                    return true;
                 }
              }
                
                
                //brak informacji o ustalonyn stanie...
                outMsg->gxh_handle = inMsg->gxh_handle;
                outMsg->dataStruct[0] =  0;
                outMsg->dataStruct[1] =  0;
                outMsg->sizeDataStruct = 0;
                
              
                return true;            
          }
          
        
          
          //komunikaty odebrane.......
          
          
          if(messageIn.func_internal_number == IH_SWITCH_ON_CLICK)     //klikniecie na porcie..
          {
               gxh_on_click gxhOnClick;
               memcpy(&gxhOnClick, messageIn.dataStruct, sizeof(gxh_on_click) );
              
               this->changePortListener(gxhOnClick.ih_driver_port_id,IH_SWITCH_ON_CLICK );
               return false;               
          }
          
          if(messageIn.func_internal_number == IH_SWITCH_OFF_CLICK)     //klikniecie na porcie..
          {
                gxh_on_click gxhOnClick;
                memcpy(&gxhOnClick, messageIn.dataStruct, sizeof(gxh_on_click) );
              
                this->changePortListener(gxhOnClick.ih_driver_port_id,IH_SWITCH_OFF_CLICK );
                return false;               
          }
          
          if(messageIn.func_internal_number== IH_SWITCH_DOUBLE_CLICK)     //podwujne klikniecie na porcie
          {        
               gxh_on_click gxhOnClick;
               memcpy(&gxhOnClick, messageIn.dataStruct, sizeof(gxh_on_click) );
              
               this->changePortListener(gxhOnClick.ih_driver_port_id,IH_SWITCH_DOUBLE_CLICK );
               return false;    
          }
          
          
          if(messageIn.func_internal_number == IH_SWITCH_LONG_CLICK)     //przytrzymanie stanu na porcie..
          {
                gxh_on_click gxhOnClick;
                memcpy(&gxhOnClick, messageIn.dataStruct, sizeof(gxh_on_click) );
              
                this->changePortListener(gxhOnClick.ih_driver_port_id,IH_SWITCH_LONG_CLICK );
                return false;             
          }
                          
    
          std::string msg = "Module function not found for ";
          msg.append(gxh_IntToString(messageIn.func_internal_number ));
          
          this->getAppHandle()->show_log_module("app_module_elpiast","execute", msg.c_str(),0 );                                              
          return false;                    
}
 
 
//-------------------------------------------------------------------------------

void app_module_elpiast::changePortListener(int ih_driver_port_id, int listener_status)
{
 
    app_module::changePortListener( ih_driver_port_id,  listener_status);
    
    //tu dodac do histori ....
                      sqlite3* dbSqLite2;                    
    
                      std::string baza = this->getAppHandle()->getDBTmp(); //this->getAppHandle()->getRunPath();
                      //baza.append("../gxh_base_tmp.db");
     
                      int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite2, SQLITE_OPEN_READWRITE, NULL   );                                  
                      if( r != SQLITE_OK )
                       {
                         this->getAppHandle()->show_error("app_module_elpiast","changePortListener","Cannot open sqlLite 1",0);                      
                       }else
                       {
                        std::string query="";
                        query.append("INSERT INTO ih_driver_port_history (ih_driver_port_id, created_at, updated_at, is_delete, status) VALUES ('");
                        query.append(   gxh_IntToString( ih_driver_port_id ) );                          
                        query.append("', DATETIME('now','localtime') ,DATETIME('now','localtime'),'0','");               
                        query.append(   gxh_IntToString( listener_status ) ); 
                        query.append("');");               
                                    
                         char *zErrMsg = 0;     
                         int rc = sqlite3_exec( dbSqLite2 , query.c_str(), 0, 0, &zErrMsg);
                         if( rc != SQLITE_OK )
                         {  
                               this->getAppHandle()->show_error("app_module_elpiast","changePortListener", (const char*)zErrMsg ,0);   
                               sqlite3_free(zErrMsg);    
                         }                     
                         sqlite3_close_v2(dbSqLite2);  
                        
                       }  
}

//-------------------------------------------------------------------------------
 

 