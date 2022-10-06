/* 
 * File:   app_module_netronix.cpp
 * Author: Blazej Kita
 * 
 * Created on 30 lipiec 2018, 14:31
 */

#include "app_module_netronix.h"


 
app_module* get_module(gxh_scope* appHomeHandle )
{
    return new app_module_netronix(appHomeHandle); 
};
 
//-------------------------------------------------------------------------------
  
app_module_netronix::app_module_netronix(gxh_scope* appHomeHandle) : app_module(appHomeHandle) 
{           
    this->getAppHandle()->show_log_module("app_module_netronix","app_module_netronix","start",2 );         
    
 
    for(int m=0; m<MAX_HISTORY_SYNCH; m++)
    {
        this->last_readed_card[m].toSave = false;
    }
    
    
        //LOAD CARD TO MEMORY..
    this->cards_count = 0;
    this->card_driver_count = 0;
    
    this->getAppHandle()->destroyResource(RES_VECTOR_CARDS);
    this->getAppHandle()->destroyResource(RES_VECTOR_CARD_DRIVER);
    
    
     sqlite3* dbSqLite;        
     
     std::string baza = this->getAppHandle()->getDB(); 
     
     int r  =  sqlite3_open_v2( baza.c_str() , &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
     if( r != SQLITE_OK )
     {
          this->getAppHandle()->show_error("app_module_netronix","app_module_netronix","Cannot open sqlLite 1",0);   
          
          return;
     }
     
     
    sqlite3_stmt* stmt; 
    
    std::string query = "SELECT id, code, name, type, px_group_id, is_active FROM ih_nfc_card WHERE is_delete='0' ";
    
    this->getAppHandle()->show_log_data_base("app_module_netronix","app_module_netronix", query.c_str() ,6); 
           
 
    
    const char* queryChar = query.c_str();        
    if(sqlite3_prepare(dbSqLite , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
    {
         this->getAppHandle()->show_error("app_module_netronix","app_module_netronix", sqlite3_errstr( sqlite3_errcode(dbSqLite  ) ),4);          
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
             if(this->cards_count > MAX_CARDS_BUFFER)break;
             
             this->cards[this->cards_count].id                     = gxh_StringToInt( std::string(  (const char*)sqlite3_column_text(stmt, 0) ) );      
             
             
             const char* tmp1                                      = (const char*)sqlite3_column_text(stmt, 1); 
             memset(this->cards[this->cards_count].code, 0, 100);             
             memcpy(this->cards[this->cards_count].code, tmp1, strlen(tmp1) );
             
          //   std::cout<<"\n\nCode: "<<this->cards[this->cards_count].code<<"\n\n-----------------------------------------";
        
             const char* tmp2                                      = (const char*)sqlite3_column_text(stmt, 2); 
             memset(this->cards[this->cards_count].name, 0, 70);             
             memcpy(this->cards[this->cards_count].name, tmp2, strlen(tmp2) );
             
             this->cards[this->cards_count].type             = gxh_StringToInt( std::string(  (const char*)sqlite3_column_text(stmt, 3 ) ) );          
             this->cards[this->cards_count].px_group_id             = gxh_StringToInt( std::string(  (const char*)sqlite3_column_text(stmt, 4 ) ) );          
             this->cards[this->cards_count].is_active             = gxh_StringToInt( std::string(  (const char*)sqlite3_column_text(stmt, 5 ) ) );          
             
            
            
            this->cards_count++;
         } 
              
          if(res == SQLITE_DONE)done = true;                            
         
    }

    this->getAppHandle()->registerResource( this->cards ,this->cards_count, RES_VECTOR_CARDS );
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
     
     
     sqlite3_finalize(stmt);    
     
     //-------------------------------------------------------------------------
     
     query = "SELECT  ih_nfc_card_driver.id, ih_nfc_card_id, ih_driver_id FROM ih_nfc_card_driver, ih_nfc_card WHERE ih_nfc_card.id=ih_nfc_card_id AND ih_nfc_card.is_delete='0' AND ih_nfc_card_driver.is_delete='0';  ";
    
    this->getAppHandle()->show_log_data_base("app_module_netronix","app_module_netronix", query.c_str() ,6); 
           
 
    
    queryChar = query.c_str();        
    if(sqlite3_prepare(dbSqLite , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
    {
         this->getAppHandle()->show_error("app_module_netronix","app_module_netronix", sqlite3_errstr( sqlite3_errcode(dbSqLite  ) ),4);          
         sqlite3_finalize(stmt);    
         sqlite3_close_v2(dbSqLite);
         return;
    }
    
    done = false;
    while (!done)               
    { 
       int res = sqlite3_step (stmt);
        
         if(res ==  SQLITE_ROW)
         {              
             if(this->card_driver_count > MAX_CARD_DRIVER_BUFF)break;
             
             this->card_drivers[this->card_driver_count].id                     = gxh_StringToInt( std::string(  (const char*)sqlite3_column_text(stmt, 0) ) );                                        
             this->card_drivers[this->card_driver_count].ih_nfc_card_id         = gxh_StringToInt( std::string(  (const char*)sqlite3_column_text(stmt, 1) ) );                                        
             this->card_drivers[this->card_driver_count].ih_driver_id           = gxh_StringToInt( std::string(  (const char*)sqlite3_column_text(stmt, 2) ) );                                        
            
            
             this->card_driver_count++;
         } 
              
          if(res == SQLITE_DONE)done = true;                            
         
    }

    this->getAppHandle()->registerResource( this->card_drivers ,this->card_driver_count, RES_VECTOR_CARD_DRIVER );
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
     
     
     sqlite3_finalize(stmt);      
     
     sqlite3_close_v2(dbSqLite);
     
    
    
   
}

//------------------------------------------------------------------------------

app_module_netronix::~app_module_netronix() 
{     
    this->getAppHandle()->show_log_module("app_module_netronix","app_module_netronix","stop",2 );
    
 
}


//-------------------------------------------------------+-----------------------

void app_module_netronix::onClick(int ih_driver_port_id, int listener_status)
{
}

//-------------------------------------------------------+-----------------------

void app_module_netronix::forceCancel()
{  
      this->getAppHandle()->show_log_module("app_module_netronix","app_module_netronix","forceCancel",2);
      
    
      this->forceSynchronize();
      
}

//-------------------------------------------------------+-----------------------

void app_module_netronix::forceSynchronize()
{  
     this->getAppHandle()->show_log_module("app_module_netronix","forceSynchronize","start",20);          
     
     
     for(int m=0; m<MAX_HISTORY_SYNCH; m++)
     {
           if( this->last_readed_card[m].toSave )
           {                
                       //----
                       sqlite3* dbSqLite;  

                       std::string baza = this->getAppHandle()->getDBTmp();//this->getAppHandle()->getRunPath();
                       //baza.append("../gxh_base.db");

                       int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
                       if( r != SQLITE_OK )
                       {
                         this->getAppHandle()->show_error("app_module_netronix","forceSynchronize","Cannot open sqlLite 1",0);                 
                       }else
                       {                                                                      

                           char *zErrMsg = 0;     
                           int rc = sqlite3_exec( dbSqLite , this->last_readed_card[m].query.c_str(), 0, 0, &zErrMsg);
                           if( rc != SQLITE_OK )
                               {        
                                  std::cout<< this->last_readed_card[m].query.c_str()<<"\n";          
                                  this->getAppHandle()->show_error("app_module_netronix","forceSynchronize", (const char*)zErrMsg,0);
                                  sqlite3_free(zErrMsg);                                
                                  sqlite3_close_v2(dbSqLite);   
                               }else
                               {                                                                                             
                                  this->last_readed_card[m].toSave = false;
                                  sqlite3_close_v2(dbSqLite);                                                              
                               }              
                       }               
                                                            
           }
     }
     
     
     
     //usuń nadmiar wpisów z historii kart..
                     sqlite3* dbSqLite2;                    
    
                      std::string baza = this->getAppHandle()->getDBTmp(); // this->getAppHandle()->getRunPath();
                      //baza.append("../gxh_base_tmp.db");
     
                      int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite2, SQLITE_OPEN_READWRITE, NULL   );                                  
                      if( r != SQLITE_OK )
                      {
                         this->getAppHandle()->show_error("app_module_netronix","forceSynchronize","Cannot open sqlLite 1",0);                      
                      }else
                      {
                          sqlite3_stmt* stmt; 
                          int lastId = -1;
                          std::string query="select id from ih_nfc_card_event ORDER BY id DESC LIMIT 1;";                            
                         
                          const char* queryChar = query.c_str();       
                          if(sqlite3_prepare(dbSqLite2 , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
                           {
                             this->getAppHandle()->show_error("app_module_netronix","forceSynchronize", sqlite3_errstr( sqlite3_errcode(dbSqLite2  ) ),4);                                        
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
                            
                            
                            if(lastId > 200)
                            {
                                std::string query = "DELETE FROM ih_nfc_card_event;";
                                
                                char *zErrMsg = 0;     
                                int rc = sqlite3_exec( dbSqLite2 , query.c_str(), 0, 0, &zErrMsg);
                                if( rc != SQLITE_OK )
                                   {  
                                      this->getAppHandle()->show_error("app_module_netronix","forceSynchronize", (const char*)zErrMsg ,0);   
                                      sqlite3_free(zErrMsg);    
                                   } 
                            }
                            
                          sqlite3_close_v2(dbSqLite2);                          
                       }  
     
     
     this->getAppHandle()->show_log_module("app_module_netronix","forceSynchronize","stop",20);          
}

//-------------------------------------------------------+-----------------------


bool app_module_netronix::execute(gxh_message* inMsg,gxh_message* message_ret)  
{ 
    
          
      gxh_message message;
      memcpy(&message, inMsg, sizeof(gxh_message));
    
     
      if(message.func_number == APP_DRIVER_INTERRUPT  && message.func_internal_number == IH_UPDATE_ADDRESS )   
           {                                                                       
               return this->updateAddress(&message, message_ret);
           }   
      
       if(message.func_number == APP_DRIVER_INTERRUPT )   
           {                
               return this->readCard(&message, message_ret);
           }            
      
        
                            
         
     
          const char* response = "NETRONIX MODULE"; 
          gxh_message msg;
          msg.gxh_handle = message.gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;
          
}

//------------------------------------------------------------------------------

bool app_module_netronix::updateAddress(gxh_message* inMsg,gxh_message* message_ret) 
{
                   system_driver_command driverParam;
                   memcpy(&driverParam, inMsg->dataStruct, sizeof(system_driver_command) );
                                                         
                   
                   std::string msg1 = "Netronix update address to ";
                   msg1.append(driverParam.param);
                   this->getAppHandle()->show_log_module("app_module_netronix","execute",msg1.c_str() ,3); 
                   
                   
                   //wyślij polecenie do sterownika..
                   bool result = false;
                   bool response = false;
                   driverParam.result = &result;
                   driverParam.response = &response;
           
              
           
              
                   gxh_message msgCommand;
                   msgCommand.system_code = GXH_MESSAGE_REDIRECT; //przekaż bezpośrednia od gxhHandle....
                   msgCommand.gxh_handle = this->getAppHandle()->getGXHHandleByIdAndtype(driverParam.ih_driver_id, 1);//dirver handle....
                   msgCommand.func_number = inMsg->func_number;        
                   msgCommand.func_internal_number = IH_UPDATE_ADDRESS;
                   memcpy(msgCommand.dataStruct, &driverParam, sizeof(system_driver_command) );
                   msgCommand.sizeDataStruct = sizeof(driverParam);
                                                         
                  this->getAppHandle()->sendGXHMessage(&msgCommand);  
                  
                  //czaka aż sterownik wykona polecenie..
                  for(int tt=0; tt<5; tt++)
                  {
                      if(response) break;
                      sleep(1);
                  }
                  
                  //std::cout<<"\nResponse "<<response<<" result "<<result;
                  
                  if(response && result)
                  {
                      const char* responseMsg = "OK"; 
                      gxh_message msg;
                      msg.gxh_handle = inMsg->gxh_handle;
                      memcpy(msg.dataStruct, responseMsg, strlen(responseMsg) );
                      msg.sizeDataStruct = sizeof(responseMsg) ;
                      msg.system_code = GXH_MESSAGE_SUCCESS;
            
                     memcpy(message_ret, &msg, sizeof(gxh_message));            
                     return true;
                  }                   
                   
                   
                   
                   const char* responseMsg = "ERROR"; 
                   gxh_message msg;
                   msg.gxh_handle = inMsg->gxh_handle;
                   memcpy(msg.dataStruct, responseMsg, strlen(responseMsg) );
                   msg.sizeDataStruct = sizeof(responseMsg) ;
                   msg.system_code = GXH_MESSAGE_SUCCESS;
            
                   memcpy(message_ret, &msg, sizeof(gxh_message));            
                   return true;
                 
}

//------------------------------------------------------------------------------

bool app_module_netronix::readCard(gxh_message* inMsg,gxh_message* message_ret) 
{                           
          ih_card_data_reader dataR;
          memcpy(&dataR, inMsg->dataStruct, sizeof(ih_card_data_reader) );
           
             
        
 
          std::string msg1 = "Card code: ";
          msg1.append(dataR.data);
          this->getAppHandle()->show_log_module("app_module_netronix","readCard",msg1.c_str() ,3);   
          
          //znajdz kartę..
          int cardInx        = -1;
          int ih_driver_id   = -1;
          int ih_nfc_card_driver_id = -1;
        //  int cardDriverInx  = -1;
          
          for(int m=0; m<this->cards_count; m++)
          {
                     
              if(strcmp(this->cards[m].code, dataR.data) == 0)
              {
                  cardInx = m;
                  ih_driver_id    = dataR.ih_driver_id;
                  break;
              }
          }
    
          if(cardInx == -1)
          {
             std::string msg2 = "Not found card: ";
             msg2.append(dataR.data);   
             this->getAppHandle()->show_log_module("app_module_netronix","readCard", msg2.c_str() ,3);               
             
             //---dodaj do historii..
                for(int m=0; m<MAX_HISTORY_SYNCH; m++)
                    {                       
                      if( !this->last_readed_card[m].toSave )
                      {                                               

                          this->last_readed_card[m].query = "INSERT INTO ih_nfc_card_event (ih_nfc_card_id, ih_driver_id, px_group_id, active, code,type, updated_at, created_at, is_delete) VALUES ('";   
                          this->last_readed_card[m].query.append(  "-1" );
                          this->last_readed_card[m].query.append(  "','"  );
                          this->last_readed_card[m].query.append(  gxh_IntToString(  dataR.ih_driver_id )  );
                          this->last_readed_card[m].query.append(  "','"  );
                          this->last_readed_card[m].query.append(   "-1"  );
                          this->last_readed_card[m].query.append(  "','"  );
                          this->last_readed_card[m].query.append(   "0"  );
                          this->last_readed_card[m].query.append(  "','"  );
                          this->last_readed_card[m].query.append(   dataR.data  );
                          this->last_readed_card[m].query.append(  "','"  );
                          this->last_readed_card[m].query.append(  "0"  );
                          this->last_readed_card[m].query.append(  "',DateTime('now','localtime'),DateTime('now','localtime'),'0');"  );
                          this->last_readed_card[m].toSave = true;
                          break;

                      }
                    } 
             
             return false;  
          }
          
          
          
          for(int m=0; m<this->card_driver_count; m++)
          {
             if(this->card_drivers[m].ih_driver_id ==  ih_driver_id && this->card_drivers[m].ih_nfc_card_id == this->cards[cardInx].id ) 
             {
                 ih_nfc_card_driver_id = this->card_drivers[m].id;
                // cardDriverInx = m;
                 break;
             }
          }
          
          
          
          if(this->cards[cardInx].is_active == 1 && ih_nfc_card_driver_id > 0) //karta jest aktywna..
          {
                  this->sendInterrrupt(APP_DRIVER_READ_ACTIVE,ih_nfc_card_driver_id);
          }
          
          
          if(this->cards[cardInx].is_active == 0 && ih_nfc_card_driver_id > 0) //karta jest nie aktywna..
          {
                  this->sendInterrrupt(APP_DRIVER_READ_NOTACTIVE, ih_nfc_card_driver_id);
          }
          
          
         for(int m=0; m<MAX_HISTORY_SYNCH; m++)
         {
             
           if( !this->last_readed_card[m].toSave )
           {                                                   
               this->last_readed_card[m].query = "INSERT INTO ih_nfc_card_event (ih_nfc_card_id, ih_driver_id, px_group_id, active, code,type, updated_at, created_at, is_delete) VALUES ('";   
               this->last_readed_card[m].query.append(  gxh_IntToString( this->cards[cardInx].id)  );
               this->last_readed_card[m].query.append(  "','"  );
               this->last_readed_card[m].query.append(  gxh_IntToString(  ih_driver_id )  );
               this->last_readed_card[m].query.append(  "','"  );
               this->last_readed_card[m].query.append(  gxh_IntToString( this->cards[cardInx].px_group_id )   );
               this->last_readed_card[m].query.append(  "','"  );
               this->last_readed_card[m].query.append(  gxh_IntToString( this->cards[cardInx].is_active )   );
               this->last_readed_card[m].query.append(  "','"  );
               this->last_readed_card[m].query.append(   this->cards[cardInx].code    );
               this->last_readed_card[m].query.append(  "','"  );
               this->last_readed_card[m].query.append(  gxh_IntToString( this->cards[cardInx].type )   );
               this->last_readed_card[m].query.append(  "',DateTime('now','localtime'),DateTime('now','localtime'),'0');"  );
               this->last_readed_card[m].toSave = true;
               
             //  std::cout<<"\n\nkarta : "<<this->last_readed_card[m].query.c_str()<<"\n\n";
               
               break;
          
           }
         }
          
        
          /*
          const char* response = "OK"; 
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          reurn true;   
           */
          return false; // nie potrzebujemy wysłać odpowiedzi na to polecenie..
    
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
