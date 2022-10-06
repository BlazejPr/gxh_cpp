/* 
 * File:   app_module_wsdl.cpp
 * Author: Blazej Kita
 * 
 * Created on 22 maj 2018, 14:31
 */

#include "app_module_wsdl.h"

#include <cstdlib>
#include <ctime>


 
app_module* get_module(gxh_scope* appHomeHandle )
{
    return new app_module_wsdl(appHomeHandle); 
};
 
//-------------------------------------------------------------------------------
  
app_module_wsdl::app_module_wsdl(gxh_scope* appHomeHandle) : app_module(appHomeHandle) 
{           
    this->getAppHandle()->show_log_module("app_module_wsdl","app_module_wsdl","start",2 );         
    
    
    
    this->firstSynchronize = true;
    
    
    
    for(int x=0; x<MAX_USER_BUFFER;x++)
    {
        this->usersSSID[x].px_user_id = -1;
        this->usersSSID[x].px_group_id = -1;
        
        
        
        this->usersSSID[x].toSave = false;
        this->usersSSID[x].free = true;
        
        for(int y=0; y<MAX_SHORTS;y++)
        {
           this->usersSSID[x].shorts[y].status = 0;
           this->usersSSID[x].shorts[y].free = true;
        }
    }
    
    this->loadSessionToCache();

}

//------------------------------------------------------------------------------

app_module_wsdl::~app_module_wsdl() 
{     
    this->getAppHandle()->show_log_module("app_module_wsdl","app_module_wsdl","stop",2 );
}


//-------------------------------------------------------+-----------------------

void app_module_wsdl::onClick(int ih_driver_port_id, int listener_status)
{
  
}

//-------------------------------------------------------+-----------------------

void app_module_wsdl::forceCancel()
{  
      this->getAppHandle()->show_log_module("app_module_wsdl","app_module_wsdl","forceCancel",2);
     
}

//-------------------------------------------------------+-----------------------

void app_module_wsdl::forceSynchronize()
{  
     this->getAppHandle()->show_log_module("app_module_wsdl","forceSynchronize","forceSynchronize start",20);          
     
     /// dodaj potrzebne zmienne systemowe.. np. gsp uzytkownikow..
     
     if(  this->firstSynchronize == true)
      {
     
      unsigned int size = 0;
      ih_driver_values* varArr =  ( ih_driver_values*)this->getAppHandle()->getResource(RES_VECTOR_DRIVER_VALUE, &size);
      
      
      for(unsigned int u=0; u<MAX_USER_BUFFER; u++)
      {
          if(this->usersSSID[u].free == false)
          {
               ///sprawdz czy jest ustawina zmienna gps
              bool isVar = false;
              
              for(unsigned int v=0; v<size; v++)
              {
                    if(varArr[v].free==false && varArr[v].const_function == VAR_SYS_USER_GPS && varArr[v].px_group_id == this->usersSSID[u].px_group_id)
                    {  
                        isVar = true;
                        break;
                    }
              }
              
              if(!isVar) //dodaj zmienna gps..
              {
                 for(unsigned int v=0; v<size; v++)
                 {
                      if(varArr[v].free == true)
                      {
                         varArr[v].free = false;
                         varArr[v].id = -1;
                         
                         std::string value =  "0000:0000";                                
                         const char* val = value.c_str();
           
                         memset(varArr[v].value, 0, strlen(val)+1);              
                         memcpy(varArr[v].value, val, strlen(val));              
                         varArr[v].toHistory = false; 
                         
                         varArr[v].const_function = VAR_SYS_USER_GPS;
                         varArr[v].ih_driver_id = -1;
                         varArr[v].px_group_id = this->usersSSID[u].px_group_id;                                                
                         
                         std::string name = "GPS:";
                         name.append(this->usersSSID[u].username);
                         memcpy(varArr[v].name_param, name.c_str(), name.length() );
                         
                         varArr[v].status    = 2; //add to base..
                         
                        // std::cout<<"\nGPS add "<< name.c_str()<<", n: "<<v;
                         
                         break;
                      }
                              
                 }  
              } //koniec brak zmiennej gps..
              
              
                ///sprawdz czy jest ustawina zmienna wifi
              isVar = false;
              
              for(unsigned int v=0; v<size; v++)
              {
                    if(varArr[v].free==false && varArr[v].const_function == VAR_SYS_USER_WIFI && varArr[v].px_group_id == this->usersSSID[u].px_group_id)
                    {  
                        isVar = true;
                        break;
                    }
              }
              
              if(!isVar) //dodaj zmienna wifi..
              {
                 for(unsigned int v=0; v<size; v++)
                 {
                      if(varArr[v].free == true)
                      {
                         varArr[v].free = false;
                         varArr[v].id = -1;
                         
                         std::string value =  "none";                                
                         const char* val = value.c_str();
           
                         memset(varArr[v].value, 0, strlen(val)+1);              
                         memcpy(varArr[v].value, val, strlen(val));              
                         varArr[v].toHistory = false; 
                         
                         varArr[v].const_function = VAR_SYS_USER_WIFI;
                         varArr[v].ih_driver_id = -1;
                         varArr[v].px_group_id = this->usersSSID[u].px_group_id;                                                
                         
                         std::string name = "WIFI:";
                         name.append(this->usersSSID[u].username);
                         memcpy(varArr[v].name_param, name.c_str(), name.length() );
                         
                         varArr[v].status    = 2; //add to base..
                         
                        // std::cout<<"\nGPS add "<< name.c_str()<<", n: "<<v;
                         
                         break;
                      }
                              
                 }  
              } //koniec brak zmiennej wifi
              
              
              
                  ///sprawdz czy jest ustawina zmienna bluetooth
              isVar = false;
              
              for(unsigned int v=0; v<size; v++)
              {
                    if(varArr[v].free==false && varArr[v].const_function == VAR_SYS_USER_BLUETOOTH && varArr[v].px_group_id == this->usersSSID[u].px_group_id)
                    {  
                        isVar = true;
                        break;
                    }
              }
              
              if(!isVar) //dodaj zmienna wifi..
              {
                 for(unsigned int v=0; v<size; v++)
                 {
                      if(varArr[v].free == true)
                      {
                         varArr[v].free = false;
                         varArr[v].id = -1;
                         
                         std::string value =  "none";                                
                         const char* val = value.c_str();
           
                         memset(varArr[v].value, 0, strlen(val)+1);              
                         memcpy(varArr[v].value, val, strlen(val));              
                         varArr[v].toHistory = false; 
                         
                         varArr[v].const_function = VAR_SYS_USER_BLUETOOTH;
                         varArr[v].ih_driver_id = -1;
                         varArr[v].px_group_id = this->usersSSID[u].px_group_id;                                                
                         
                         std::string name = "BLUETOOTH:";
                         name.append(this->usersSSID[u].username);
                         memcpy(varArr[v].name_param, name.c_str(), name.length() );
                         
                         varArr[v].status    = 2; //add to base..
                         
                        // std::cout<<"\nGPS add "<< name.c_str()<<", n: "<<v;
                         
                         break;
                      }
                              
                 }  
              } //koniec brak zmiennej bluetooth
              
              
          }
      }
      
     
      //pobierz skróty uzytkownikow...
      
            for(unsigned int u=0; u<MAX_USER_BUFFER; u++)
            {
                if(this->usersSSID[u].free == true) continue; //brak uzytkownika..               
                   
                  sqlite3* dbSqLite;    
                  sqlite3_stmt* stmt; 
          
                  std::string baza = this->getAppHandle()->getDB(); //this->getAppHandle()->getRunPath();
                  //baza.append("../gxh_base.db");
     
                  int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
                  if( r != SQLITE_OK )
                    {
                     this->getAppHandle()->show_error("app_module_wsdl","forceSynchronize","Cannot open sqlLite 1",0);           
                    }
                    else
                    {
                       std::string query="SELECT id, const_function_symbol, resource_id, name, text_content FROM ih_short WHERE is_delete='0' AND px_group_id='";    
                       query.append(gxh_IntToString(this->usersSSID[u].px_group_id) );
                       query.append("'; ");
                       this->getAppHandle()->show_log_data_base("app_module_wsdl","forceSynchronize", query.c_str() ,6); 
     
                       const char* queryChar = query.c_str();        
                       if(sqlite3_prepare(dbSqLite , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
                        {
                         this->getAppHandle()->show_error("app_module_wsdl","forceSynchronize", sqlite3_errstr( sqlite3_errcode(dbSqLite  ) ),4);          
                         sqlite3_finalize(stmt);                     
                        }else
                        {    
                          bool done = false;      
                          int index = 0;
                          while (!done)               
                            { 
                               int res = sqlite3_step (stmt);        
                               if(res ==  SQLITE_ROW)
                                {                                       
                                    //(const char*)sqlite3_column_text(stmt, 0)                     
                                   
                                   this->usersSSID[u].shorts[index].id = gxh_StringToInt( (const char*)sqlite3_column_text(stmt, 0)  );
                                   this->usersSSID[u].shorts[index].const_function_symbol = gxh_StringToInt( (const char*)sqlite3_column_text(stmt, 1)  );
                                   this->usersSSID[u].shorts[index].resource_id = gxh_StringToInt( (const char*)sqlite3_column_text(stmt, 2)  );
                                   
                                   const char* tmp3 = (const char*)sqlite3_column_text(stmt, 3);
                                   memset(this->usersSSID[u].shorts[index].name, 0, 255);                                  
                                   if(tmp3 != NULL)memcpy(this->usersSSID[u].shorts[index].name, tmp3, strlen(tmp3));                                  
                                   
                                   const char* tmp4 = (const char*)sqlite3_column_text(stmt, 4);
                                   memset(this->usersSSID[u].shorts[index].text_content, 0, 500);                                  
                                   if(tmp4 != NULL) memcpy(this->usersSSID[u].shorts[index].text_content, tmp4, strlen(tmp4));
                                   
                                   this->usersSSID[u].shorts[index].free = false;
                                   this->usersSSID[u].shorts[index].status = 0;
                             
                                   index++;
                                   if(index >= MAX_SHORTS) done = true;
                                }              
                              if(res == SQLITE_DONE)done = true;                                     
                            }           
                           sqlite3_finalize(stmt); 
                         }
                       
                       sqlite3_close_v2(dbSqLite);
                    }
            }
            
            this->firstSynchronize = false;
      }
     
     
     //sprawdz czy zaktualizować skróty uzytkownika w bazie danych....
     
       for(unsigned int u=0; u<MAX_USER_BUFFER; u++)
       {
                if(this->usersSSID[u].free == true) continue; //brak uzytkownika..
                
                for(int s=0; s< MAX_SHORTS; s++)
                {
                    if( this->usersSSID[u].shorts[s].free == true) continue; //brak skrotu
                        
                    if( this->usersSSID[u].shorts[s].status == 1)   //aktualizuj skrot uzytkowika..
                    {
                    }
                    
                    if( this->usersSSID[u].shorts[s].status == 2)   //dodaj do bazy danych...
                    {
                          this->usersSSID[u].shorts[s].id = -1;
                        
                         sqlite3* dbSqLite;               
                      
                         
                         std::string baza = this->getAppHandle()->getDB();//this->getAppHandle()->getRunPath();
                        // baza.append("../gxh_base.db");
     
                          int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
                          if( r != SQLITE_OK )
                          {
                            this->getAppHandle()->show_error("app_module_wsdl","forceSynchronize","Cannot open sqlLite 1",0);                 
                          }else
                          {
                             std::string query="";
                             query.append("INSERT INTO ih_short (id, px_group_id, const_function_symbol, resource_id,name,text_content, is_delete) VALUES(null,'");                                                       
                             query.append( gxh_IntToString( this->usersSSID[u].px_group_id  ) );
                             query.append("','");
                             query.append( gxh_IntToString( this->usersSSID[u].shorts[s].const_function_symbol) );
                             query.append("','");                         
                             query.append(  this->usersSSID[u].shorts[s].name );
                             query.append("','");                         
                             query.append(  this->usersSSID[u].shorts[s].text_content );  
                             query.append("','0');");
                         
                             char *zErrMsg = 0;     
                             int rc = sqlite3_exec( dbSqLite , query.c_str(), 0, 0, &zErrMsg);                                                       
                             if( rc != SQLITE_OK )  sqlite3_free(zErrMsg); 
                             if( rc == SQLITE_OK ) 
                             {                                                               
                                 this->usersSSID[u].shorts[s].status=0;              
                             }
                        
                             sqlite3_close_v2(dbSqLite);         
                         }
                    }
                    
                    if( this->usersSSID[u].shorts[s].status == 3)   //usun skrot
                    {
                         sqlite3* dbSqLite;               
                         std::string baza = this->getAppHandle()->getDB();//this->getAppHandle()->getRunPath();
                        // baza.append("../gxh_base.db");
     
                          int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
                          if( r != SQLITE_OK )
                          {
                            this->getAppHandle()->show_error("app_module_wsdl","forceSynchronize","Cannot open sqlLite 1",0);                 
                          }else
                          {
                             std::string query="";
                             query.append("DELETE FROM ih_short WHERE const_function_symbol='");                                                       
                             query.append( gxh_IntToString( this->usersSSID[u].shorts[s].const_function_symbol   ) );
                             query.append("' AND resource_id='");
                               query.append( gxh_IntToString( this->usersSSID[u].shorts[s].resource_id   ) );
                             query.append("' AND px_group_id='");
                             query.append( gxh_IntToString( this->usersSSID[u].px_group_id   ) );
                             query.append("'; ");
                         
                             char *zErrMsg = 0;     
                             int rc = sqlite3_exec( dbSqLite , query.c_str(), 0, 0, &zErrMsg);
                             if( rc == SQLITE_OK ) {this->usersSSID[u].shorts[s].status=0;  this->usersSSID[u].shorts[s].free = true; this->usersSSID[u].shorts[s].const_function_symbol=0;                              if( rc == SQLITE_OK ) {this->usersSSID[u].shorts[s].status=0;  this->usersSSID[u].shorts[s].free = true; this->usersSSID[u].shorts[s].const_function_symbol=0;  this->usersSSID[u].shorts[s].resource_id=0;  }  }
                             if( rc != SQLITE_OK )  sqlite3_free(zErrMsg); 
                        
                             sqlite3_close_v2(dbSqLite);         
                         }
                        
                    }
                }
       
       
                
                /*
                if(this->usersSSID[u].toSave == true) //zaktualizuj dane profilewe uzytkownika..
                {
                         sqlite3* dbSqLite;               
                      
                         
                         std::string baza = this->getAppHandle()->getRunPath();
                         baza.append("../gxh_base.db");
     
                          int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
                          if( r != SQLITE_OK )
                          {
                            this->getAppHandle()->show_error("app_module_wsdl","forceSynchronize","Cannot open sqlLite 2",0);                 
                          }else
                          {
                             std::string query="";
                             query.append("UPDATE px_group SET data_profil='");                                                       
                             query.append( this->usersSSID[u].dataProfil  );                                                       
                             query.append( "' WHERE id='" );                        
                             query.append( gxh_IntToString( this->usersSSID[u].px_group_id  ) );                        
                             query.append("';");
                         
                             char *zErrMsg = 0;     
                             int rc = sqlite3_exec( dbSqLite , query.c_str(), 0, 0, &zErrMsg);                                                       
                             if( rc != SQLITE_OK )  sqlite3_free(zErrMsg); 
                             if( rc == SQLITE_OK ) 
                             {                                                               
                                 this->usersSSID[u].toSave = false;     
                             }
                        
                             sqlite3_close_v2(dbSqLite);         
                         }
                }*/
       }
      
     
     for(unsigned int u=0; u<MAX_USER_BUFFER; u++)
      {
          if(this->usersSSID[u].free == false)
          {
              std::string userInfo =" User: ";
              userInfo.append( this->usersSSID[u].ssid );
              userInfo.append(", ");
              userInfo.append( this->usersSSID[u].username );
              userInfo.append( " px_user_id: " );
              userInfo.append( gxh_IntToString(this->usersSSID[u].px_user_id)  );
              
              userInfo.append(" gz: ");
              for(unsigned int pp=0;pp<this->usersSSID[u].zone_grp_count;pp++)
              {                 
                  userInfo.append( this->usersSSID[u].zone_grups[pp].name);
                  userInfo.append(", ");
              }
              
              
               userInfo.append(" z: ");
              for(unsigned int pp=0;pp<this->usersSSID[u].zones_count;pp++)
              {                 
                  userInfo.append( this->usersSSID[u].zones[pp].name);
                  userInfo.append(", ");
              }
              
              userInfo.append(" p: ");
              for(unsigned int pp=0;pp<this->usersSSID[u].privilege_count;pp++)
              {                 
                  userInfo.append( this->usersSSID[u].privileges[pp].name);
                  userInfo.append(", ");
              }
              
              
              this->getAppHandle()->show_log_module("app_module_wsdl","forceSynchronize",userInfo.c_str(),10);   
          }
     } 
     
     this->getAppHandle()->show_log_module("app_module_wsdl","forceSynchronize","forceSynchronize stop",20);               
     //czyszczenie px_session..
}

//-------------------------------------------------------+-----------------------

bool app_module_wsdl::execute(gxh_message* inMsg,gxh_message* message_ret)  
{               
      gxh_message message;
      memcpy(&message, inMsg, sizeof(gxh_message));
    
      if(message.func_number != 2 )     //wsdl
      {                           
         return false;      
      }
      
      char headers[20][1024];
      int numHeader;
      char soapAction[1024];
      char soapEnvelop[1024 * 100];
      char gxhVersion[40]; //  
      char gxhPublicKey[40]; //  publiczny klucz..
      int sizeSopaEnv;
     
      int px_user_id;
      int px_group_id;
          
      bool result = false;
      
      this->parseHeader(&message, headers, soapAction, &numHeader,&sizeSopaEnv, soapEnvelop, gxhVersion, gxhPublicKey );
      
     
      if(strlen(gxhVersion) > 4) //spawdz jaką wersje obsługuję aplikacja kliencka...
      {
          
              std::string msg = "GXH API version: ";
              msg.append(gxhVersion);
              this->getAppHandle()->show_error("app_module_wsdl","execute", msg.c_str(), 2);
              
             // return wrong_version(&message,message_ret, verC);
           
      }else
      {
             this->getAppHandle()->show_error("app_module_wsdl","execute", "GXH API version not found", 2); 
      }
      
      
      if(strlen(gxhPublicKey) > 1) //spawdz jaką wersje obsługuję aplikacja kliencka...
      {
          
              //-----
                   std::streampos size;
                   char memblock[40];
                   memset(memblock,0,40);

                   std::ifstream fileKey ( "/var/www/html/secure/public_key.txt", std::ios::in|std::ios::binary|std::ios::ate);
                   if (fileKey.is_open())
                   {
                     size = fileKey.tellg();                    
                     fileKey.seekg (0, std::ios::beg);
                     fileKey.read (memblock, size);
                     fileKey.close();
                    
                     if(gxh_compare(gxhPublicKey,memblock) != 0)    
                     {
                            std::string msg = "GXH apiKey bad: ";
                            msg.append(gxhPublicKey);
                            msg.append("|" );
                            msg.append(memblock );
                            msg.append("|" );
                            this->getAppHandle()->show_error("app_module_wsdl","execute", msg.c_str(), 2);
                            
                            return wrong_apiKey(&message,message_ret); 
                     }
                     
                   }else
                   {
                      this->getAppHandle()->show_error("app_module_wsdl","execute", "GXH apiKey file not found!", 2); 
                      return wrong_apiKey(&message,message_ret); 
                   }
              //-----
          
             // std::string msg = "GXH apiKey: ";
             // msg.append(gxhPublicKey);
              //this->getAppHandle()->show_error("app_module_wsdl","execute", msg.c_str(), 2);          
      }else
      {
             this->getAppHandle()->show_error("app_module_wsdl","execute", "GXH apiKey not found in header", 2); 
             return wrong_apiKey(&message,message_ret); 
      }
      
      
      //----------------------------
      //
    //  std::cout<<"\n\nX:"<<soapAction;
      
      this->getAppHandle()->show_log_module("app_module_wsdl","execute", soapAction, 2);
      
      if(gxh_compare(soapAction,"gologin") == 0)                
      {                                 
                 char username[50];
                 int l2 = this->getParam("username",username,50,soapEnvelop);    
                 char password[50];
                 int l1 = this->getParam("password",password,50,soapEnvelop);   
                 
                 char type[2];
                 int l3 = this->getParam("type",type,2,soapEnvelop); 
                 
                 if(l1 > 0 && l2>0 && l3>0)
                 {                                 
                     result = this->goLogin(&message,message_ret,username,password, type);                                                                                         
                 }                         
                
                 if(!result) return this->wrong_param(&message, message_ret);
                 if(result)  return true; 
      }   
            
      //dalej tylko zalogowani użytkownicy..
      char sid[100];
      int l1 = this->getParam("sid",sid,100,soapEnvelop);   
      
      std::string strSid = std::string(sid);
      
     
      if(message.localhost == false)
      {
        if(l1>0)
        {
             for(int x=0; x<MAX_USER_BUFFER;x++)
             {                
               //   std::cout<<"\nUser "<<this->usersSSID[x].username.c_str()<<this->usersSSID[x].ssid<<" | "<< strSid <<", "<<this->usersSSID[x].px_group_id<<", "<<this->usersSSID[x].px_user_id;
               if(this->usersSSID[x].free == true)    continue;

                //   std::cout<<"\n"<<this->usersSSID[x].username<<"\n ";                         
                // std::cout<<this->usersSSID[x].session[p].ssid.c_str()<<" "<<strSid.c_str();

                   if( this->usersSSID[x].ssid.compare(strSid) == 0)
                   {                                        
                         px_user_id =  this->usersSSID[x].px_user_id;
                         px_group_id = this->usersSSID[x].px_group_id;                                                
                         break;
                   }


             }    
        }
      }else
      {
          px_user_id = 10000;
          px_group_id = 10000;
      }
      
      if(px_user_id <= 0 || px_group_id <= 0)
      {
                
          this->getAppHandle()->show_log_module("app_module_wsdl","execute", "user logout", 2);
          return this->logoutMsg(&message,message_ret);
      }
 
      
      
       if(gxh_compare(soapAction,"gorefresh") == 0)              
       {     
                 char lat[100];
                 int l2 = this->getParam("lat", lat, 100,soapEnvelop);   
                 char lng[100];
                 int l3 = this->getParam("lng", lng, 100,soapEnvelop); 
                 char lcTimeStr[100];
                 int l4 = this->getParam("locationTime", lcTimeStr,100,soapEnvelop);  
                 char lcTimeMsInt[100];
                 int l5 = this->getParam("locationTimeMs", lcTimeMsInt,100,soapEnvelop);  
                 
                 char wifi[100];
                 int l6 = this->getParam("wifiName", wifi,100,soapEnvelop);  
                 
                 char bluetooth[100];
                 int l7 = this->getParam("bluetoothName", bluetooth,100,soapEnvelop);  
                                                                                  
                 if(   l2>0 && l3>0 && l4>0 && l5>0 && l6>0 && l7>0 ) return this->goRefresh(&message, message_ret, px_user_id, px_group_id, gxh_StringToDouble( lat), gxh_StringToDouble( lng), wifi,bluetooth );       
                 return this->wrong_param(&message, message_ret);
       }
   
       if(gxh_compare(soapAction,"getinfo") == 0)    return this->getInfo(&message, message_ret, px_user_id, px_group_id); 
       if(gxh_compare(soapAction,"get_ver_app") == 0)    return this->getVerApp(&message, message_ret, px_user_id, px_group_id); 
       if(gxh_compare(soapAction,"get_all_short") == 0)    return this->getShort(&message, message_ret, px_user_id, px_group_id); 
      
      
       if(gxh_compare(soapAction,"add_short") == 0)                
            {               
                 char pFuncCode[100];
                 int l2 =this->getParam("pNumFunction", pFuncCode, 100,soapEnvelop);                           
                 char pResourceId[100];
                 int l3 = this->getParam("pResourceId",pResourceId,100,soapEnvelop );                           
                 char pParam1[300];
                 int l4  =this->getParam("pParam1", pParam1,300, soapEnvelop);                           
                 char pParam2[300];
                 int l5 = this->getParam("pParam2", pParam2, 300, soapEnvelop);                           
                 
                 if(  l2>0 && l3>0 && l4>0 && l5>0 )
                 {                                 
                   return this->addShort (&message, message_ret,px_group_id, gxh_StringToInt(pFuncCode),gxh_StringToInt(pResourceId),(const char*)pParam1, ( const char*)pParam2 );                     
                 }                                
                  return this->wrong_param(&message, message_ret);                
            }
      
      
      if(gxh_compare(soapAction,"get_all_camera") == 0)                
            {                                 
                return this->getAllCamera(px_user_id, px_group_id, &message, message_ret );                             
            } 
      
      if(gxh_compare(soapAction,"remove_short") == 0)                
            {               
                 char pFuncCode[100];
                 int l2 =this->getParam("pNumFunction", pFuncCode, 100,soapEnvelop);                           
                 char pResourceId[100];
                 int l3 = this->getParam("pResourceId",pResourceId,100,soapEnvelop );                                               
                 
                 if(  l2>0 && l3>0 )
                 {                                 
                   return this->removeShort (&message, message_ret,px_group_id, gxh_StringToInt(pFuncCode),gxh_StringToInt(pResourceId));                     
                 }                                
                  return this->wrong_param(&message, message_ret);                
            }
      
      
      
      
      
       if(gxh_compare(soapAction,"set_msg_status") == 0)                
            {                                      
                 char id[10];
                 int l2 = this->getParam("numMsg", id,10, soapEnvelop);  
                 
                 
                 char status[10];
                 int l3 = this->getParam("numStatus", status,10, soapEnvelop);  
                 
                 if(  l2>0 && l3>0 )
                 {                                 
                    return this->setMsgStatus( &message, message_ret ,gxh_StringToInt(id), gxh_StringToInt(status) );
                 }                                 
                                                                                                                                   
                 return this->wrong_param(&message, message_ret);     
            } 
      
      
     
      
         if(gxh_compare(soapAction,"get_all_history") == 0)                
            {                 
                 char dateTimeFrom[100];
                 int l2 = this->getParam("dateTimeFrom", dateTimeFrom, 100,soapEnvelop );                   
                 char dateTimeTo[100];
                 int l3 = this->getParam("dateTimeTo", dateTimeTo, 100, soapEnvelop);                   
                 
                 if( l2> 0 && l3>0 )
                 {                                 
                    return result = this->getAllHistory( &message, message_ret, px_user_id, px_group_id );
                 }                                                                                                                 
              
                   return this->wrong_param(&message, message_ret) ; 
            } 
      
      
      if(gxh_compare(soapAction,"go_reload_modules") == 0)    return this->reloadModule(&message, message_ret);                       
      if(gxh_compare(soapAction,"go_reload_ports") == 0)      return this->goReloadPorts(&message, message_ret);         
      if(gxh_compare(soapAction,"goclose") == 0)              return this->goClose(&message, message_ret);         
      
       if(gxh_compare(soapAction,"set_debug_level") == 0)                
            {                   
                 char level[20];
                 int l2 = this->getParam("level", level,20,soapEnvelop);     
                 
                 if(l2>0)
                 {                                 
                     result = this->setDebugLevel(&message, message_ret,level);                        
                 }                                                       
                 
                 if(!result) return this->wrong_param(&message, message_ret);
                 if(result)  return true; 
            }
      
       if(gxh_compare(soapAction,"get_all_user") == 0)              return this->getAllUser(&message, message_ret);   
       if(gxh_compare(soapAction,"get_all_home") == 0)              return this->getAllHome(&message, message_ret);   
      
      
       if(gxh_compare(soapAction,"set_address") == 0)                
       {  
                 char driverId[100];
                 int l2 = this->getParam("driverId",driverId,100,soapEnvelop);     
                 char parity[100];
                 int l3 = this->getParam("parity",parity,100,soapEnvelop);     
                 char dataBit[100];
                 int l4 = this->getParam("dataBit",dataBit,100,soapEnvelop);     
                 char stopBit[100];
                 int l5 =this->getParam("stopBit",stopBit,100,soapEnvelop);     
                 char speed[100];
                 int l6 = this->getParam("speed",speed,100,soapEnvelop);     
                 
                 if( l2>0 && l3>0 && l4>0 && l5>0 && l6>0 )
                 {                                 
                   return this->setAddress(&message,message_ret,driverId, parity,dataBit,stopBit, speed);                       
                 }                                                               
                 return this->wrong_param(&message, message_ret);
       }
      
      
      
       if(gxh_compare(soapAction,"port_click") == 0)                
       {                               
                 char level[100];
                 int l2 =this->getParam("portId", level, 100,soapEnvelop);                      
                 if( l2>0)
                 {                                 
                     return this->setClick(&message, message_ret,level);                                                                                
                 }                      
                 return this->wrong_param(&message, message_ret);              
       }
      
          if(gxh_compare(soapAction,"get_all_function") == 0)                
            {                                  
                 return this->getAllFunction(&message, message_ret );             
            }
      
      
          if(gxh_compare(soapAction,"remove_scene") == 0)                
            { 
                 char idScene[100];
                 int l2  = this->getParam("pIdScene", idScene, 100, soapEnvelop);                           
                 
                 if(  l2>0) return this->removeScene(&message, message_ret, gxh_StringToInt(idScene));                     
                 return this->wrong_param(&message, message_ret);            
            }
      
          if(gxh_compare(soapAction,"remove_action_scene") == 0)                
            { 
                 char idScene[100];
                 int l2 = this->getParam("pIdActionScene", idScene, 100, soapEnvelop);                           
                 
                 if(  l2>0 )return this->removeActionScene(&message,message_ret,gxh_StringToInt(idScene));                                                                                                                                
                 return this->wrong_param(&message, message_ret);   
            }  
      
      if(gxh_compare(soapAction,"get_all_scene_action") == 0)                
            {                
                 char numTop[100];
                 int l2 = this->getParam("numSceneId", numTop,100, soapEnvelop);     
                 
                 if(  l2>0 )return this->getAllSceneAction(&message,  message_ret, gxh_StringToInt(numTop) );                     
                                                                                 
                 return this->wrong_param(&message, message_ret);  
            }
      
      
        if(gxh_compare(soapAction,"add_action_scene") == 0)                
            {     
                 char idScene[100];
                 int l2 =this->getParam("pIdScene", idScene, 100, soapEnvelop);   
                 char pFuncCode[100];
                 int l3 = this->getParam("pFuncCode", pFuncCode, 100, soapEnvelop); 
                 char pResourceId[100];
                 int l4 = this->getParam("pResourceId", pResourceId, 100, soapEnvelop); 
                 char pParam1[300];
                 int l5 = this->getParam("pParam1", pParam1, 300, soapEnvelop);  
                 char pParam2[300];
                 int l6 = this->getParam("pParam2", pParam2, 300, soapEnvelop); 
                 
                 if(   l2>0 && l3>0 && l4>0 && l5>0 && l6>0 )
                 {                                 
                    return this->addSceneAction(&message, message_ret,  gxh_StringToInt(idScene),gxh_StringToInt(pFuncCode),gxh_StringToInt(pResourceId),pParam1, pParam2 );                     
                 }                                       
                 
                 return this->wrong_param(&message, message_ret);           
            }
      
      if(gxh_compare(soapAction,"get_all_scene") == 0)  
            {                                                       
                 return this->getAllScene(&message,message_ret );                                                 
            }
      
      
        if(gxh_compare(soapAction,"add_scene") == 0)                
            {  
                 char name[100];
                 int l2 = this->getParam("sceneName", name, 100,soapEnvelop); 
                 
                 if( l2>0 )return this->addScene(&message, message_ret, name );                                                                                                                                 
                 return this->wrong_param(&message, message_ret);
            }
      
      
      if(gxh_compare(soapAction,"get_version") == 0)          return this->getVersion(&message, message_ret);         
      if(gxh_compare(soapAction,"get_all_sector") == 0)       return this->getAllSector(px_user_id,px_group_id,&message, message_ret);         
      if(gxh_compare(soapAction,"get_all_zone") == 0)      
      {
          char numTop[100];
          int l2 = this->getParam("numSectorTop", numTop,100, soapEnvelop);  
          if(l2>0) return this->getAllZone(px_user_id, px_group_id,&message, message_ret, gxh_StringToInt(numTop) );      
          return this->wrong_param(&message, message_ret);
      }
      if(gxh_compare(soapAction,"get_all_light") == 0)      
      {
           
          char numTopZone[100];
          int l2 = this->getParam("numZoneTop", numTopZone,100,soapEnvelop);                  
          char numTopSector[100];
          int l3 = this->getParam("numSectorTop", numTopSector,100,soapEnvelop);  
          
          
          
          if(l2>0 && l3>0) return this->getAllLight(px_user_id, px_group_id ,&message, message_ret, gxh_StringToInt(numTopZone), gxh_StringToInt(numTopSector)  );         
          return this->wrong_param(&message, message_ret);
      }
      
       if(gxh_compare(soapAction,"get_all_outs") == 0)      
      {
           
          char numTopZone[100];
          int l2 = this->getParam("numZoneTop", numTopZone,100,soapEnvelop);                  
          char numTopSector[100];
          int l3 = this->getParam("numSectorTop", numTopSector,100,soapEnvelop);  
          
          
          
          if(l2>0 && l3>0) return this->getAllOuts(px_user_id,px_group_id ,&message, message_ret, gxh_StringToInt(numTopZone), gxh_StringToInt(numTopSector)  );         
          return this->wrong_param(&message, message_ret);
      }
    
      
      //do stref alarmowych ma dostęp wyłącznie osoba upoważniona...
      if(gxh_compare(soapAction,"get_all_alarm_zone") == 0)      
      {                                       
          return this->getAllAlarmZone(px_user_id,px_group_id ,&message, message_ret );         
       //   return this->wrong_param(&message, message_ret);
      }
      
      
      if(gxh_compare(soapAction,"get_all_blind") == 0)      
      {
          char numTopZone[100];
          int l2 = this->getParam("numZoneTop", numTopZone,100,soapEnvelop);                  
          char numTopSector[100];
          int l3 = this->getParam("numSectorTop", numTopSector,100,soapEnvelop);  
          if(l2>0 && l3>0) return this->getAllBlind(px_user_id, px_group_id,&message, message_ret, gxh_StringToInt(numTopZone), gxh_StringToInt(numTopSector)  );         
          return this->wrong_param(&message, message_ret);
      }
       if(gxh_compare(soapAction,"get_all_gate") == 0)      
      {
          char numTopZone[100];
          int l2 = this->getParam("numZoneTop", numTopZone,100,soapEnvelop);                  
          char numTopSector[100];
          int l3 = this->getParam("numSectorTop", numTopSector,100,soapEnvelop);  
          if(l2>0 && l3>0) return this->getAllGate( px_user_id,  px_group_id ,&message, message_ret, gxh_StringToInt(numTopZone), gxh_StringToInt(numTopSector)  );         
          return this->wrong_param(&message, message_ret);
      }
      
      
      if(gxh_compare(soapAction,"get_all_door") == 0)      
      {
          char numTopZone[100];
          int l2 = this->getParam("numZoneTop", numTopZone,100,soapEnvelop);                  
          char numTopSector[100];
          int l3 = this->getParam("numSectorTop", numTopSector,100,soapEnvelop);  
          if(l2>0 && l3>0) return this->getAllDoor(px_user_id, px_group_id,&message, message_ret, gxh_StringToInt(numTopZone), gxh_StringToInt(numTopSector)  );         
          return this->wrong_param(&message, message_ret);
      }
      
      
      if(gxh_compare(soapAction,"get_all_temp") == 0)                      
      {               
          char numTopZone[100];
          int l2 = this->getParam("numZoneTop", numTopZone,100,soapEnvelop);                  
          char numTopSector[100];
          int l3 = this->getParam("numSectorTop", numTopSector,100,soapEnvelop);  
          
                 if( l2>0 && l3>0)
                 {                                 
                    result = this->getAllTemp(&message, message_ret, gxh_StringToInt(numTopZone), gxh_StringToInt(numTopSector)  );                     
                    if(result)  return true; 
                 }                                                 
                 return this->wrong_param(&message, message_ret);               
      }
      
      
       if(gxh_compare(soapAction,"get_all_sensor") == 0)      
      {
          char numTopZone[100];
          int l2 = this->getParam("numZoneTop", numTopZone,100,soapEnvelop);                  
          char numTopSector[100];
          int l3 = this->getParam("numSectorTop", numTopSector,100,soapEnvelop);  
          if(l2>0 && l3>0) return this->getAllSensor(px_user_id,px_group_id ,&message, message_ret, gxh_StringToInt(numTopZone), gxh_StringToInt(numTopSector)  );         
          return this->wrong_param(&message, message_ret);
      }
      
      
                   
      if(gxh_compare(soapAction,"get_all_driver_vars") == 0)                
      {        
           char pId[100];
           int l2 = this->getParam("pIdDriver", pId, 100, soapEnvelop);                    
                 
           if(   l2>0 )return this->getAllDriverVar (&message,message_ret,gxh_StringToInt( pId) );                     
                                                                        
           return this->wrong_param(&message, message_ret); 
      }
      
      
      if(gxh_compare(soapAction,"get_all_driver_api") == 0)                
      {        
          char pId[100];
          int l2 = this->getParam("pIdDriver", pId,100,soapEnvelop);     
                                
          if( l2>0 )return result = this->getAllDriverApi(&message,message_ret, gxh_StringToInt( pId) );                                                              
          return this->wrong_param(&message, message_ret);                
      }
            
      
      if(gxh_compare(soapAction,"get_all_driver") == 0)      
      {    
          return this->getAllDriver(px_user_id,px_group_id,&message, message_ret );              
      }
      
      if(gxh_compare(soapAction,"ex_simply_func") == 0)      
      {
          char numFunc[100];
          int l2 = this->getParam("numFunction", numFunc, 100,soapEnvelop);     
          char numRes[100];
          int l3 = this->getParam("numResource", numRes, 100, soapEnvelop);     
                 
          if(  l2 >0 && l3> 0 )
          {                                 
                     result =  this->exeSimplyFunction(&message, message_ret, gxh_StringToInt(numFunc), gxh_StringToInt(numRes), px_group_id);                    
                     if(result)  return true;   
          }                                                                          
          return this->wrong_param(&message, message_ret);
      }
      
      
      if(gxh_compare(soapAction,"ex_param_func") == 0)      
      {                   
                 char numFunc[100];
                 int l2 = this->getParam("numFunction", numFunc, 100,soapEnvelop);     
                 char numRes[100];
                 int l3 = this->getParam("numResource", numRes, 100,soapEnvelop);     
                 char param1[300];
                 int l4 = this->getParam("param_1", param1, 300,soapEnvelop);    
                 char param2[300];
                 int l5 = this->getParam("param_2", param2, 300,soapEnvelop);    
                 
                 if(  l2>0 && l3>0 && l4>0 && l5>0 )
                 {                                 
                     result = this->exeParamFunction(&message, message_ret, gxh_StringToInt(numFunc), gxh_StringToInt(numRes), param1, param2,px_group_id);                     
                     if(result)  return true; 
                 }                                    
                 
                 if(!result) return this->wrong_param(&message,message_ret);                                 
            }
      
       if(gxh_compare(soapAction,"ex_driver_func") == 0)      
      {                   
                 char numDriver[100];
                 int l2 = this->getParam("numDriver", numDriver,100,soapEnvelop);     
                 char numFunction[100];
                 int l3 = this->getParam("numFunction", numFunction, 100,soapEnvelop);     
                 char param[300];
                 int l4 = this->getParam("param", param, 300,soapEnvelop);    
                
                 
                 if( l2>0 && l3>0 && l4>0 )
                 {                                 
                     result = this->exeDriverFunction(&message,message_ret, gxh_StringToInt(numDriver), gxh_StringToInt(numFunction),param );                     
                      if(result)  return true;    
                 }                                  
                
                 
                 if(!result) return this->wrong_param(&message,message_ret);                                       
      }
      
    
      if(gxh_compare(soapAction,"remove_timer") == 0)                                 
      {  
          char pIdTimer[10];
          int l1 = this->getParam("pIdTimer", pIdTimer,10,soapEnvelop); 
          
          if(l1 > 0 ) return this->removeTask(&message,message_ret, gxh_StringToInt(pIdTimer)  ); 
      }
      
         if(gxh_compare(soapAction,"ex_sys") == 0)                                 
      {  
          char num[10];
          int l1 = this->getParam("num", num,10,soapEnvelop); 
          
          if(l1 > 0 ) return this->executeSys(&message,message_ret, gxh_StringToInt(num)  ); 
      }
      
      
      if(gxh_compare(soapAction,"get_all_timer") == 0)      
      {  
          return this->getAllTimer(&message,message_ret);
      }
      
      if(gxh_compare(soapAction,"add_timer") == 0)      
      {                   
                 char pNumFunction[10];
                 int l1 = this->getParam("pNumFunction", pNumFunction,10,soapEnvelop);                  
                 
                 char pResourceId[9];
                 int l2 = this->getParam("pResourceId", pResourceId,9,soapEnvelop);    
                 
                 char pFirstStart[30];
                 int l3 = this->getParam("pFirstStart", pFirstStart,30,soapEnvelop);   
                 
                 char pRepeatWeek[2];
                 int l4 = this->getParam("pRepeatWeek", pRepeatWeek,2,soapEnvelop);   
                 
                 char pMonDay[2];
                 int l5 = this->getParam("pMonDay", pMonDay,2,soapEnvelop);  
                 
                 char pTueDay[2];
                 int l6 = this->getParam("pTueDay", pTueDay,2,soapEnvelop);  
                 
                 char pWedDay[2];
                 int l7 = this->getParam("pWedDay", pWedDay,2,soapEnvelop);  
                 
                 char pThuDay[2];
                 int l8 = this->getParam("pThuDay", pThuDay,2,soapEnvelop); 
                 
                 char pFriDay[2];
                 int l9 = this->getParam("pFriDay", pFriDay,2,soapEnvelop);
                 
                 char pSatDay[2];
                 int l10 = this->getParam("pSatDay", pSatDay,2,soapEnvelop);
                 
                 char pSunDay[2];
                 int l11 = this->getParam("pSunDay", pSunDay,2,soapEnvelop);
                 
                 char param1[255];
                 int l12 = this->getParam("param1", param1,255,soapEnvelop);
                 
                 char param2[500];
                 int l13 = this->getParam("param2", param2,500,soapEnvelop);
                
                 
                 if( l1>0 && l2>0 && l3>0 && l4>0 && l5>0 && l6>0 && l7>0 && l8>0 && l9>0 && l10>0 && l11>0 && l12>0 && l13>0 )
                 {                                 
                      result = this->addTask( &message,message_ret, gxh_StringToInt( pNumFunction), gxh_StringToInt( pResourceId), (const char*)pFirstStart, gxh_StringToInt( pRepeatWeek), gxh_StringToInt(pMonDay), gxh_StringToInt( pTueDay), gxh_StringToInt( pWedDay), gxh_StringToInt( pThuDay), gxh_StringToInt(pFriDay), gxh_StringToInt(pSatDay), gxh_StringToInt(pSunDay), (const char*) param1, (const char*) param2 );
                      if(result)  return true;    
                 }                                  
                
                 
                 if(!result) return this->wrong_param(&message,message_ret);                                       
            }
      
      
      //----------------------------
      //
  
      const char* response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Client</faultcode><faultstring>WSDL Module, please select function.</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>\n"; 
      gxh_message msg;
      msg.gxh_handle = message.gxh_handle;
      memcpy(msg.dataStruct, response, strlen(response) );
      msg.sizeDataStruct = sizeof(response) ;
      msg.system_code = GXH_MESSAGE_SUCCESS;
            
      memcpy(message_ret, &msg, sizeof(gxh_message));            
      return true;          
}
 

//------------------------------------------------------------------------------

bool app_module_wsdl::build(gxh_message* msgIn, gxh_message* message_ret, const char* response)
{
    
            std::string res = std::string(response);
    
            gxh_replaceAll( res , "&","&amp;" ); //replace & characters..
      
            gxh_message msg;
            msg.gxh_handle = msgIn->gxh_handle;
            
            if(strlen(res.c_str()) >= sizeof(msg.dataStruct) )
            {
                 this->getAppHandle()->show_error("app_module_wsdl","build","fatal error, data is too length...",3);
                 memset(msg.dataStruct, 0, sizeof(msg.dataStruct) );
            }
            else
            {
              memcpy(msg.dataStruct, res.c_str(), strlen(res.c_str()) );
            }
              
            msg.sizeDataStruct = strlen(res.c_str()) ;
            msg.system_code = GXH_MESSAGE_SUCCESS;
            
            
            memcpy(message_ret, &msg, sizeof(gxh_message));            
            return true; 
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

bool app_module_wsdl::logoutMsg(gxh_message* msgIn,gxh_message* message_ret)
{
      
        const char* response = "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Client</faultcode><faultstring>LOGOUT</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>\n"; 
        return this->build(msgIn, message_ret, response);     
}

//------------------------------------------------------------------------------

bool app_module_wsdl::errorMsg(gxh_message* msgIn,gxh_message* message_ret)
{      
        const char* response =   "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Client</faultcode><faultstring>DATA ERROR/faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>\n";
        return this->build(msgIn, message_ret, response);   
}

//------------------------------------------------------------------------------

bool app_module_wsdl::wrong_param(gxh_message* msgIn,gxh_message* message_ret)
{
 
               this->getAppHandle()->show_error("app_module_wsdl","wrong_param","Error execute function, check your params",0);
           
               const char* response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Client</faultcode><faultstring>SoapAction not found or sended data are incorrect</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>\n";
               
               return this->build(msgIn, message_ret, response);             
      
}

//------------------------------------------------------------------------------

bool app_module_wsdl::wrong_version(gxh_message* msgIn,gxh_message* message_ret, const char* version)
{
                           
               std::string msg =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Client</faultcode><faultstring>Version Error,";
                           msg.append(version);
                           msg.append ( "</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>\n");
               
               return this->build(msgIn, message_ret, msg.c_str());             
      
}
//------------------------------------------------------------------------------

bool app_module_wsdl::wrong_apiKey(gxh_message* msgIn,gxh_message* message_ret)
{
                           
               std::string msg =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Client</faultcode><faultstring>Bad apiKey</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>\n";                                                      
               
               return this->build(msgIn, message_ret, msg.c_str());             
      
}

//------------------------------------------------------------------------------

bool app_module_wsdl::executeSys(gxh_message* msgIn, gxh_message* message_ret, int id  )
{
     
           this->executeSystemConsole(msgIn, message_ret, id);
             
    
           std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:executeSysResponse><result  xsi:type=\"xsd:string\">OK";                                                                                                        
           response.append(  "</result></gxh:executeSysResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");                                                                                                        
                           
           return this->build(msgIn, message_ret, response.c_str());  ;  
}

//------------------------------------------------------------------------------

bool app_module_wsdl::getAllCamera(int px_user_id, int px_group_id ,gxh_message* msgIn,gxh_message* message_ret)
{
     std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:getAllCameraResponse><result>";                                                                                                        
  
     unsigned int count;
     ih_camera* rows = (ih_camera*)this->getAppHandle()->getResource( RES_VECTOR_CAMERA , &count); 
     
     
      for(unsigned int k=0; k<count;k++) 
              {
                 response.append( "<item>");        
                 
                 //-------------------------------------------------------------
                 response.append( "<pId xsi:type=\"xsd:int\">");                                                                                                                         
                 response.append( gxh_IntToString(rows[k].id)  ) ;                                                                                                     
                 response.append(  "</pId>");                                                                                                        
                 //-------------------------------------------------------------
                  
                 response.append( "<pName xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( rows[k].name  ) ;                                                                                                         
                 response.append(  "</pName>");                                                                                                        
                 //-------------------------------------------------------------      
                                   
                 response.append( "<pIP xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( rows[k].localIp  ) ;                                                                                                         
                 response.append(  "</pIP>");                                                                                                        
                 //-------------------------------------------------------------  
                                   
                 response.append( "<pExtraParam xsi:type=\"xsd:string\">");                                                                                                         
                // response.append( rows[k].extra_param ) ;                                                                                                         
                 response.append(  "</pExtraParam>");                                                                                                        
                 //-------------------------------------------------------------  
                                   
                 response.append( "<pProtocol xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( rows[k].protocol ) ;                                                                                                         
                 response.append(  "</pProtocol>");                                                                                                        
                 //-------------------------------------------------------------  
                 
                                   
                 response.append( "<pURL xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( rows[k].url ) ;                                                                                                         
                 response.append(  "</pURL>");                                                                                                        
                 //------------------------------------------------------------- 
                 //-------------------------------------------------------------
                 response.append( "<pVideoPhone xsi:type=\"xsd:int\">");                                                                                                                         
                 response.append(  "100" ) ;                                                                                               
                 response.append(  "</pVideoPhone>");
                 
                 //-------------------------------------------------------------
                 response.append( "<pLocalPort xsi:type=\"xsd:int\">");                                                                                                           
                 response.append( gxh_IntToString(rows[k].localPort)   ) ;                                                                                         
                 response.append(  "</pLocalPort>");
                 
                 
                 //-------------------------------------------------------------
                 response.append( "<pRemotePort xsi:type=\"xsd:int\">");                                                                                                                       
                 response.append( gxh_IntToString(rows[k].remotePort)   ) ;                                                                                             
                 response.append(  "</pRemotePort>");
                 
                                  
                 response.append( "</item>");                                                                                                        
              }
      
     
     
     response.append(  "</result></gxh:getAllCameraResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");   
     
     return this->build(msgIn, message_ret, response.c_str());     
}

//------------------------------------------------------------------------------

bool app_module_wsdl::getAllHistory(gxh_message* msgIn,gxh_message* message_ret, int px_user_id, int px_group_id )
{
      std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:getAllHistoryResponse><result>";                                                                                                        
  
      
                  sqlite3* dbSqLite;    
                  sqlite3_stmt* stmt; 
          
                  std::string baza = this->getAppHandle()->getDBTmp();this->getAppHandle()->getRunPath();
                  //baza.append("../gxh_base_tmp.db");
     
                  int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
                  if( r != SQLITE_OK )
                    {
                     this->getAppHandle()->show_error("app_module_wsdl","getAllHistory","Cannot open sqlLite 1",0);           
                    }
                    else
                    {
                       std::string query="SELECT id, const_function_symbol, resource_id, name, text_content, created_at, px_group_id FROM ih_history ORDER BY id DESC LIMIT 50; ";                        
                       this->getAppHandle()->show_log_data_base("app_module_wsdl","getAllHistory", query.c_str() ,6); 
     
                       const char* queryChar = query.c_str();        
                       if(sqlite3_prepare(dbSqLite , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
                        {
                         this->getAppHandle()->show_error("app_module_wsdl","getAllHistory", sqlite3_errstr( sqlite3_errcode(dbSqLite  ) ),4);          
                         sqlite3_finalize(stmt);                     
                        }else
                        {    
                          bool done = false;                              
                          while (!done)               
                            { 
                               int res = sqlite3_step (stmt);        
                               if(res ==  SQLITE_ROW)
                                {                                       
                                        const char* id           =  (const char*)sqlite3_column_text(stmt, 0);
                                        const char* constFunc    =  (const char*)sqlite3_column_text(stmt, 1);
                                        const char* resId        =  (const char*)sqlite3_column_text(stmt, 2);
                                        
                                        const char* name         =  (const char*)sqlite3_column_text(stmt, 3);
                                        const char* textCont     =  (const char*)sqlite3_column_text(stmt, 4);
                                        
                                        const char* created     =  (const char*)sqlite3_column_text(stmt, 5);
                                        
                                        const char* groupId     =  (const char*)sqlite3_column_text(stmt, 6);
                                        
                                        int px_group_id = gxh_StringToInt( std::string(groupId) );
                                        
                                        
                                            ///----------------------------------
                 ///----------------------------------
    
                
                 char functionName[255]; // Wlacznik swiatla
                 char resourceName[255];   // Lampa
                 char fName[100]; //light blind
                 char res_status[100];  //0 1
                 
                 char user[255];  //0 1
                 char zone[255];
                 int ih_zone_id = -1;
                                  
                 char paramName1[255];
                 char paramName2[255];
                 
                 memset(functionName, 0, 255);
                 memset(resourceName, 0, 255);
                 memset(fName, 0, 100);
                 memset(res_status, 0, 100);
                 
                 memset(user, 0, 255);
                 memset(zone, 0, 255);
                 memset(paramName1, 0, 255);
                 memset(paramName2, 0, 255);
                 
             
                       
                       unsigned int symbolCount = 0;
                       ih_sl_const_function_hash* symbolH = ( ih_sl_const_function_hash* )this->getAppHandle()->getResource( RES_VECTOR_SYMBOL_HASH , &symbolCount); 
                       
                       for(unsigned int p=0;p<symbolCount; p++)
                       {
                           if(symbolH[p].symbol ==  gxh_StringToInt( std::string(constFunc) )  )
                           {                           
                               memcpy(fName,symbolH[p].name, strlen(symbolH[p].name) );                               
                           }
                       }                       
                    
                       
                       unsigned int count = 0;
                       ih_sl_const_function_symbol* rowsF = (ih_sl_const_function_symbol*) this->getAppHandle()->getResource( RES_VECTOR_FUNC , &count); 
                       
                       for(unsigned w=0; w<count; w++)
                       {
                           if(rowsF[w].const_function_symbol == gxh_StringToInt( std::string(constFunc) )  )
                           {
                               memcpy(functionName, rowsF[w].description, strlen( rowsF[w].description) );
                               
                               memcpy(paramName1, rowsF[w].param1Name, strlen( rowsF[w].param1Name) );
                               memcpy(paramName2, rowsF[w].param2Name, strlen( rowsF[w].param2Name) );
                           }
                       }
      
                       
                        //---
                 
                       
                       if(gxh_compare(fName,"light") == 0) 
                         {
                                  unsigned int countT = 0;
                                  ih_lighting* lights   = (ih_lighting*)this->getAppHandle()->getResource(RES_VECTOR_LIGHT, &countT); 
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                         
                                      if(lights[b].id == gxh_StringToInt( std::string(resId) ) )
                                      {
                                          ih_zone_id = lights[b].ih_zone_id;
                                          memcpy(resourceName, lights[b].name, strlen(lights[b].name) );                                         
                                      }
                                  }
                         }
                        else
                            if(gxh_compare(fName,"outs") == 0) 
                         {
                                  unsigned int countT = 0;
                                  ih_outs* outs   = (ih_outs*)this->getAppHandle()->getResource(RES_VECTOR_OUTS, &countT); 
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                         
                                      if(outs[b].id == gxh_StringToInt( std::string(resId) ) )
                                      {
                                          ih_zone_id = outs[b].ih_zone_id;
                                          memcpy(resourceName, outs[b].name, strlen(outs[b].name) );                                         
                                      }
                                  }
                         }
                        else
                        if(gxh_compare(fName,"blind") == 0) 
                         {
                                  unsigned int countT = 0;
                                  ih_blind* blinds      = (ih_blind*)this->getAppHandle()->getResource(RES_VECTOR_BLIND, &countT); 
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                                      if(blinds[b].id ==  gxh_StringToInt( std::string(resId) ))
                                      {
                                         ih_zone_id = blinds[b].ih_zone_id;
                                          memcpy(resourceName, blinds[b].name, strlen(blinds[b].name) );                                       
                                      }
                                  }
                         }
                        else
                       if(gxh_compare(fName,"gate") == 0) 
                         {
                                  unsigned int countT = 0;
                                  ih_gate* gates        = (ih_gate*)this->getAppHandle()->getResource(RES_VECTOR_GATE, &countT); 
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                                      if(gates[b].id ==  gxh_StringToInt( std::string(resId) ) )
                                      {
                                          ih_zone_id = gates[b].from_ih_zone_id;
                                          memcpy(resourceName, gates[b].name, strlen(gates[b].name) );                                    
                                      }
                                  }
                         }
                       else
                       if(gxh_compare(fName,"sensor") == 0) 
                         {
                                  unsigned int countT = 0;
                                  ih_sensor* sensors    = (ih_sensor*)this->getAppHandle()->getResource(RES_VECTOR_SENSOR, &countT); 
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                                      if(sensors[b].id ==  gxh_StringToInt( std::string(resId) ) )
                                      {
                                          ih_zone_id = sensors[b].ih_zone_id;
                                          memcpy(resourceName, sensors[b].name, strlen(sensors[b].name) );
                                      }
                                  }
                         }else
                         if(gxh_compare(fName,"driver") == 0) 
                         {
                                  unsigned int countT = 0;
                                  ih_driver* driver    = (ih_driver*)this->getAppHandle()->getResource(RES_VECTOR_DRIVER, &countT); 
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                                      if(driver[b].id == gxh_StringToInt( std::string(resId) ) )
                                      {
                                          ih_zone_id = driver[b].ih_zone_id;
                                          memcpy(resourceName, driver[b].name, strlen(driver[b].name) );
                                      }
                                  }
                         }else
                         if(gxh_compare(fName,"user") == 0) 
                         {
                                  unsigned int countT = 0;
                                  user_gxh* users = (user_gxh*)this->getAppHandle()->getResource(RES_VECTOR_USER, &countT );              
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                                      if(users[b].px_group_id ==  gxh_StringToInt( std::string(resId) ) )
                                      {
                                          memcpy(resourceName, users[b].username, strlen(users[b].username) );
                                      }
                                  }
                         }else
                         if(gxh_compare(fName,"zone") == 0) 
                         {
                                  unsigned int countT = 0;
                                  ih_zone* zones = (ih_zone*)this->getAppHandle()->getResource(RES_VECTOR_ZONE, &countT );              
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                                      if(zones[b].id ==  gxh_StringToInt( std::string(resId) ) )
                                      {
                                          memcpy(resourceName, zones[b].name, strlen(zones[b].name) );
                                      }
                                  }
                         }else
                             if(gxh_compare(fName,"sector") == 0) 
                         {
                                  unsigned int countT = 0;
                                  ih_sector* sectors = (ih_sector*)this->getAppHandle()->getResource(RES_VECTOR_SECTOR, &countT );              
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                                      if(sectors[b].id ==  gxh_StringToInt( std::string(resId) ) )
                                      {
                                          memcpy(resourceName, sectors[b].name, strlen(sectors[b].name) );
                                      }
                                  }
                         }else
                         {
                                  const char* nameTmp = "Brak zasobu";
                                  memcpy(resourceName, nameTmp, strlen(nameTmp) );
                         }
                 
                       
                       if(px_group_id > 0)
                       {
                                   unsigned int countT = 0;
                                  user_gxh* users = (user_gxh*)this->getAppHandle()->getResource(RES_VECTOR_USER, &countT );              
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                                      if(users[b].px_group_id ==  px_group_id )
                                      {
                                          memcpy(user, users[b].username, strlen(users[b].username) );
                                      }
                                  }
                       }
                       
                       if(ih_zone_id > 0)
                       {
                                   unsigned int countT = 0;
                                  ih_zone* zones = (ih_zone*)this->getAppHandle()->getResource(RES_VECTOR_ZONE, &countT );              
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                                      if(zones[b].id == ih_zone_id )
                                      {
                                          memcpy(zone, zones[b].name, strlen(zones[b].name) );
                                      }
                                  }
                       }
            
                                      ///----------------------------------
                                     ///----------------------------------
                                   
                                   
                                   
                                        response.append( "<item>");        
                 
                                        //-------------------------------------------------------------
                                        response.append( "<pId xsi:type=\"xsd:int\">");                                                                                                                     
                                        if(id != NULL) response.append( id  ) ;                                                                                            
                                        response.append(  "</pId>");                                                                                                        
                 
                                        //-------------------------------------------------------------
                                        response.append( "<pFuncId xsi:type=\"xsd:int\">");                                                                                                                     
                                        if(constFunc != NULL) response.append(  constFunc ) ;    
                                        response.append(  "</pFuncId>");  
                                        //-------------------------------------------------------------
                                        //-------------------------------------------------------------
                                        response.append( "<pResourceId xsi:type=\"xsd:int\">");                                                                                                                     
                                        if(resId != NULL)response.append( resId ) ;                                                                                                         
                                        response.append(  "</pResourceId>");  
                                        //-------------------------------------------------------------
                  
                                        response.append( "<pNameFunction xsi:type=\"xsd:string\">");                                                                                                         
                                        response.append( functionName ) ;                                                                                                         
                                        response.append(  "</pNameFunction>");                                                                                                        
                                        //-------------------------------------------------------------  
                  
                                        response.append( "<pResourceName xsi:type=\"xsd:string\">");                                                                                                         
                                        response.append( resourceName ) ;                                                                                                         
                                        response.append(  "</pResourceName>");   
                                        //-------------------------------------------------------------
                  
                                        response.append( "<pUserId xsi:type=\"xsd:int\">");                                                                                                                          
                                        if(groupId != NULL)response.append(  groupId ) ;          
                                        response.append(  "</pUserId>");  
                                        //-------------------------------------------------------------   
                                   
                                        response.append( "<pParam1 xsi:type=\"xsd:string\">");                                                                                                         
                                        if(name != NULL)response.append( name ) ;                                                                                                         
                                        response.append(  "</pParam1>");                                                                                                        
                                        //-------------------------------------------------------------  
                                                    
                                        response.append( "<pParam2 xsi:type=\"xsd:string\">");                                                                                                         
                                        if(textCont != NULL)response.append( textCont ) ;                                                                                                         
                                        response.append(  "</pParam2>");                                                                                                        
                                        //-------------------------------------------------------------  
                 
                                  
                                        response.append( "<pDateTime xsi:type=\"xsd:string\">");                                                                                                         
                                        if(created != NULL)response.append( created  ) ;                                                                                                         
                                        response.append(  "</pDateTime>");                                                                                                        
                                        //------------------------------------------------------------- 
                                        
                                        //-------------------------------------------------------------  
                                                    
                                        response.append( "<pUserName xsi:type=\"xsd:string\">");                                                                                                         
                                       response.append( user ) ;                                                                                                         
                                        response.append(  "</pUserName>"); 
                                        
                                        //-------------------------------------------------------------  
                                                    
                                        response.append( "<pZoneName xsi:type=\"xsd:string\">");                                                                                                         
                                         response.append( zone ) ;                                                                                                         
                                        response.append(  "</pZoneName>"); 
                                        
                                             //-------------------------------------------------------------  
                                                    
                                        response.append( "<pParam1Name xsi:type=\"xsd:string\">");                                                                                                         
                                         response.append( paramName1 ) ;                                                                                                         
                                        response.append(  "</pParam1Name>"); 
                                        
                                           //-------------------------------------------------------------  
                                                    
                                        response.append( "<pParam2Name xsi:type=\"xsd:string\">");                                                                                                         
                                         response.append( paramName2 ) ;                                                                                                         
                                        response.append(  "</pParam2Name>");
                 
                                        response.append( "</item>");    
                                   
                                 }              
                              if(res == SQLITE_DONE)done = true;                                     
                            }           
                           sqlite3_finalize(stmt); 
                         }
                       
                       sqlite3_close_v2(dbSqLite);
                    }
      
      
      response.append(  "</result></gxh:getAllHistoryResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");    
      
      return this->build(msgIn, message_ret, response.c_str() );  
}

//------------------------------------------------------------------------------

 

 
//------------------------------------------------------------------------------

bool app_module_wsdl::setMsgStatus(gxh_message* msgIn, gxh_message* message_ret, int notice_id, int status  )
{
    
         unsigned int count = 0;
         ih_notice* notices = (ih_notice*)this->getAppHandle()->getResource(RES_VECTOR_NOTICE, &count ); 
         
         
         for(unsigned int i=0; i< count; i++)
         {
             if(notices[i].id == notice_id)
             {
                 //std::cout<<"\n\n\nOKKKKKKKKK";
                 
                 if(notices[i].checkResponse)
                 {
                    // std::cout<<"\nStatus: "<<status;
                     
                     if(status == 1)  //tak
                     {
                        gxh_message msgOut;
                         
                        if(notices[i].p1_const_function_response > 0) this->executeFunctionParam(msgIn, &msgOut , notices[i].p1_const_function_response , notices[i].p1_resource_id, std::string( notices[i].p1_param1), std::string( notices[i].p1_param2), -1, NULL );  
                     }                     
                     if(status == 0)  //nie
                     {
                        gxh_message msgOut;
                        if(notices[i].p2_const_function_response > 0)this->executeFunctionParam(msgIn, &msgOut , notices[i].p2_const_function_response , notices[i].p2_resource_id, std::string( notices[i].p2_param1), std::string( notices[i].p2_param2), -1, NULL );  
                     }
                 }                
                 break;
             }
         }
         
    
         std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:setMsgStatusResponse><result>";  
         
         response.append( "OK" ); 
        
         response.append(  "</result></gxh:setMsgStatusResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");  
       
         return this->build(msgIn, message_ret, response.c_str());
}

//------------------------------------------------------------------------------

bool app_module_wsdl::getAllHome(gxh_message* msgIn,gxh_message* message_ret)
{
             unsigned int count = 0;
             ih_home* homes = (ih_home*)this->getAppHandle()->getResource(RES_VECTOR_HOME, &count );  
    
        
              std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:getAllHomeResponse><result>";                                                                                                        
  
              for(unsigned int k=0;k<count;k++) 
              {
                 response.append( "<item>");        
                 
                 //-------------------------------------------------------------
                 response.append( "<pId xsi:type=\"xsd:int\">");                                                                                                                         
                 response.append(  gxh_IntToString(homes[k].id)  ) ;                                                                                                         
                 response.append(  "</pId>");                                                                                                        
                 //-------------------------------------------------------------
                  
                 response.append( "<pName xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( homes[k].name  ) ;                                                                                                         
                 response.append(  "</pName>");                                                                                                        
                 //-------------------------------------------------------------
                  
                 response.append( "<pAddr1 xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( homes[k].address_1  ) ;                                                                                                         
                 response.append(  "</pAddr1>");                                                                                                        
                 //-------------------------------------------------------------
                 
                 response.append( "<pAddr2 xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( homes[k].address_2  ) ;                                                                                                         
                 response.append(  "</pAddr2>");                                                                                                        
                 //-------------------------------------------------------------
                 
                 response.append( "<pLatitude xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( homes[k].latitude  ) ;                                                                                                         
                 response.append(  "</pLatitude>");                                                                                                        
                 //-------------------------------------------------------------
                 
                 response.append( "<pLongitude xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( homes[k].longitude  ) ;                                                                                                         
                 response.append(  "</pLongitude>");                                                                                                        
                 //-------------------------------------------------------------
                 
                 
                 response.append( "</item>");                                                                                                        
              }
              
              response.append(  "</result></gxh:getAllHomeResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");                                                                                                        
              
              return this->build(msgIn, message_ret, response.c_str());   
         
}

//------------------------------------------------------------------------------

bool app_module_wsdl::getAllUser(gxh_message* msgIn,gxh_message* message_ret)
{ 
         unsigned int size = 0;
        ih_driver_values* values = (ih_driver_values* )this->getAppHandle()->getResource(RES_VECTOR_DRIVER_VALUE, &size);
      
      
    
        std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:getAllUser><result>";                                                                                                        
  
        for(unsigned int k=0; k<MAX_USER_BUFFER;k++) 
        {
            if(this->usersSSID[k].free == true)  continue;
            
                 response.append( "<item>");        
                 
                 //-------------------------------------------------------------
                 response.append( "<pId xsi:type=\"xsd:int\">");                 
                 response.append( gxh_IntToString( this->usersSSID[k].px_group_id )  ) ;                                                                                                        
                 response.append(  "</pId>");                                                                                                        
                 //-------------------------------------------------------------
                  
                 response.append( "<pNick xsi:type=\"xsd:string\">");                                                                                                         
                 response.append(  this->usersSSID[k].username  ) ;                                                                                                         
                 response.append(  "</pNick>");                                                                                                        
                 //-------------------------------------------------------------   
                 
                 response.append( "<pName xsi:type=\"xsd:string\">");                                                                                                                       
                 response.append(  this->usersSSID[k].name  ) ;                                                                                                         
                 response.append(  "</pName>");                                                                                                        
                 //-------------------------------------------------------------
                   
                 response.append( "<pLastName xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( this->usersSSID[k].last_name  ) ;                                                                                                         
                 response.append(  "</pLastName>");      
                  //-------------------------------------------------------------
                 
                 response.append( "<pPhone xsi:type=\"xsd:string\">");                                                                                                                     
                 response.append( this->usersSSID[k].phone  ) ;                                                                                                         
                 response.append(  "</pPhone>");   
                 //-------------------------------------------------------------
                 
                 char lat[10];
                 char lng[10];
                 memset(lat,0,10);
                 memset(lng,0,10);
                 
                 for(unsigned int m=0; m<size; m++)
                 {
                     if(values[m].const_function == VAR_SYS_USER_GPS && values[m].px_group_id==this->usersSSID[k].px_group_id  )
                     {
                         int lt = strlen(values[m].value);
                         
                         if( lt >= 3)
                         {
                             bool tw = false;
                             int inx = 0;
                             
                             for(int z=0; z<lt;z++)
                             {
                                 if(values[m].value[z]==':') {tw=true; inx=0; continue;}
                                 
                                 if(!tw)
                                 {
                                     lat[inx] = values[m].value[z];
                                     inx++;
                                 }
                                 
                                 if(tw)
                                 {
                                     lng[inx] = values[m].value[z];
                                     inx++;
                                 }
                             }
                         }                         
                         break;
                     }
                 }
                 
                 response.append( "<pLatitude xsi:type=\"xsd:double\">");                                                                                                                       
                 response.append(  lat  ) ;                                                                                                         
                 response.append(  "</pLatitude>");   
                 //-------------------------------------------------------------
                 
                 response.append( "<pLongitude xsi:type=\"xsd:double\">");                                                                                                                       
                 response.append(  lng ) ;                                                                                                         
                 response.append(  "</pLongitude>");  
                 
                 
                 response.append( "</item>");                                                                                                        
              }
              
         response.append(  "</result></gxh:getAllUser></SOAP-ENV:Body></SOAP-ENV:Envelope>");                                                                                                         
         return this->build(msgIn, message_ret, response.c_str());            
}

//------------------------------------------------------------------------------

bool app_module_wsdl::removeShort(gxh_message* msgIn,gxh_message* message_ret,  int px_group_id, int funcCode, int resourceId  )
{
    
        unsigned int count = 0;
        user_gxh* users = (user_gxh*)this->getAppHandle()->getResource(RES_VECTOR_USER, &count );   
      
        unsigned int index = 0;
        bool isE = false;
        bool remove = false;
        
        for( unsigned int xx=0; xx<count; xx++)
        {
            if(users[xx].px_group_id == px_group_id ) { index = xx; isE=true; break; }
        }
        
        if(!isE)
        {
            return false;
        }    
        
        for(int s=0; s<MAX_SHORTS; s++)
        {
            if(!users[index].shorts[s].free )
            {
                if(users[index].shorts[s].const_function_symbol == funcCode && users[index].shorts[s].resource_id == resourceId  )
                {
                   users[index].shorts[s].status = 3;                    
                   remove = true;
                }
            }
        }
    
    
           std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:removeShortResponse>";
                 
               if(remove)  response.append("<result  xsi:type=\"xsd:string\">OK</result>");                                                          
               if(!remove)  response.append("<result  xsi:type=\"xsd:string\">ERROR</result>");                                                          
              
              response.append(  "</gxh:removeShortResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");                                                                                                        
                                                              
              return this->build(msgIn, message_ret, response.c_str());    
}

//------------------------------------------------------------------------------

bool app_module_wsdl::addShort(gxh_message* msgIn,gxh_message* message_ret,  int px_group_id, int funcCode, int resourceId, const char* param1, const char* param2 )
{
        unsigned int count = 0;
        user_gxh* users = (user_gxh*)this->getAppHandle()->getResource(RES_VECTOR_USER, &count );   
      
        unsigned int index = 0;
        bool isE = false;
        
        for( unsigned int xx=0; xx<count; xx++)
        {
            if(users[xx].px_group_id == px_group_id ) { index = xx; isE=true; break; }
        }
        
        if(!isE)
        {
            return false;
        }    
        
        for(int s=0; s<MAX_SHORTS; s++)
        {
            if(users[index].shorts[s].free )
            {
               users[index].shorts[s].free = false; 
               users[index].shorts[s].const_function_symbol = funcCode; 
               users[index].shorts[s].resource_id = resourceId; 
               
               memcpy(users[index].shorts[s].name, param1, strlen(param1));
               memcpy(users[index].shorts[s].text_content, param2, strlen(param2));
             
               users[index].shorts[s].status =2;
               break;
            }
        }
    
    
              std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:addShortResponse>";                                                                                                        
                 
                 response.append("<result  xsi:type=\"xsd:string\">OK</result>");                        
                 response.append("<pId   xsi:type=\"xsd:int\">");                                         
                 response.append( "0" );                                
                 response.append("</pId>");                        
       
              
              response.append(  "</gxh:addShortResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");                                                                                                        
                                                              
              return this->build(msgIn, message_ret, response.c_str());      
}

//------------------------------------------------------------------------------

bool app_module_wsdl::getShort(gxh_message* msgIn,gxh_message* message_ret, int px_user_id, int px_group_id )
{    
        unsigned int count = 0;
        user_gxh* users = (user_gxh*)this->getAppHandle()->getResource(RES_VECTOR_USER, &count );   
      
        int index = -1;
        
        for(unsigned int x=0; x<count; x++)
        {
            if(users[x].px_group_id == px_group_id ) { index = x; break; }
        }
        
        if(index == -1)
        {
            return false;
        }
            
        
                        
              std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:getAllShortResponse><result>";                                                                                                        
  
              for(int k=0; k<MAX_SHORTS;k++) 
              {
                 if(users[index].shorts[k].free == true) continue;
                 if(users[index].shorts[k].status == 3) continue; //do usuniecia
                 
                 
                 //------------------------------------------------------------------
                 //------------------------------------------------------------------
                
                 char functionName[255]; // Wlacznik swiatla
                 char resourceName[255];   // Lampa
                 char fName[100]; //light blind
                 char res_status[100];  //0 1
                 
                 memset(functionName, 0, 255);
                 memset(resourceName, 0, 255);
                 memset(fName, 0, 100);
                 memset(res_status, 0, 100);
                 
             
                       
                       unsigned int symbolCount = 0;
                       ih_sl_const_function_hash* symbolH = ( ih_sl_const_function_hash* )this->getAppHandle()->getResource( RES_VECTOR_SYMBOL_HASH , &symbolCount); 
                       
                       for(unsigned int p=0;p<symbolCount; p++)
                       {
                           if(symbolH[p].symbol ==users[index].shorts[k].const_function_symbol )
                           {                           
                               memcpy(fName,symbolH[p].name, strlen(symbolH[p].name) );                    
                           }
                       }
                       
                    
                       
                        //---
                 
                       
                       if(gxh_compare(fName,"light") == 0) 
                         {
                                  unsigned int countT = 0;
                                  ih_lighting* lights   = (ih_lighting*)this->getAppHandle()->getResource(RES_VECTOR_LIGHT, &countT); 
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                         
                                      if(lights[b].id ==  users[index].shorts[k].resource_id )
                                      {
                                          memcpy(resourceName, lights[b].name, strlen(lights[b].name) );
                                          
                                          char status[2]; status[0] = lights[b].status; status[1] = 0;
                                          memcpy(res_status, status, strlen(status));
                                      }
                                  }
                         }
                        else
                            if(gxh_compare(fName,"outs") == 0) 
                         {
                                  unsigned int countT = 0;
                                  ih_outs* outs   = (ih_outs*)this->getAppHandle()->getResource(RES_VECTOR_OUTS, &countT); 
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                         
                                      if(outs[b].id ==  users[index].shorts[k].resource_id )
                                      {
                                          memcpy(resourceName, outs[b].name, strlen(outs[b].name) );
                                          
                                          char status[2]; status[0] = outs[b].status; status[1] = 0;
                                          memcpy(res_status, status, strlen(status));
                                      }
                                  }
                         }
                        else
                        if(gxh_compare(fName,"blind") == 0) 
                         {
                                  unsigned int countT = 0;
                                  ih_blind* blinds      = (ih_blind*)this->getAppHandle()->getResource(RES_VECTOR_BLIND, &countT); 
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                                      if(blinds[b].id ==  users[index].shorts[k].resource_id )
                                      {
                                          memcpy(resourceName, blinds[b].name, strlen(blinds[b].name) );
                                          
                                          std::string st = gxh_IntToString( blinds[b].open_percent );
                                          const char* stTmp = st.c_str();
                                          memcpy(res_status, stTmp, strlen(stTmp));
                                      }
                                  }
                         }
                        else
                       if(gxh_compare(fName,"gate") == 0) 
                         {
                                  unsigned int countT = 0;
                                  ih_gate* gates        = (ih_gate*)this->getAppHandle()->getResource(RES_VECTOR_GATE, &countT); 
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                                      if(gates[b].id ==  users[index].shorts[k].resource_id )
                                      {
                                          memcpy(resourceName, gates[b].name, strlen(gates[b].name) );
                                          
                                          std::string st = gxh_IntToString( gates[b].open_percent );
                                          const char* stTmp = st.c_str();
                                          memcpy(res_status, stTmp, strlen(stTmp));
                                      }
                                  }
                         }
                       else
                       if(gxh_compare(fName,"sensor") == 0) 
                         {
                                  unsigned int countT = 0;
                                  ih_sensor* sensors    = (ih_sensor*)this->getAppHandle()->getResource(RES_VECTOR_SENSOR, &countT); 
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                                      if(sensors[b].id ==  users[index].shorts[k].resource_id )
                                      {
                                          memcpy(resourceName, sensors[b].name, strlen(sensors[b].name) );
                                      }
                                  }
                         }else
                         if(gxh_compare(fName,"driver") == 0) 
                         {
                                  unsigned int countT = 0;
                                  ih_driver* driver    = (ih_driver*)this->getAppHandle()->getResource(RES_VECTOR_DRIVER, &countT); 
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                                      if(driver[b].id ==  users[index].shorts[k].resource_id )
                                      {
                                          memcpy(resourceName, driver[b].name, strlen(driver[b].name) );
                                      }
                                  }
                         }else
                         if(gxh_compare(fName,"user") == 0) 
                         {
                                  unsigned int countT = 0;
                                  user_gxh* users = (user_gxh*)this->getAppHandle()->getResource(RES_VECTOR_USER, &countT );              
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                                      if(users[b].px_group_id ==  users[index].shorts[k].resource_id )
                                      {
                                          memcpy(resourceName, users[b].username, strlen(users[b].username) );
                                      }
                                  }
                         }else
                         {
                                  const char* nameTmp = "Brak zasobu";
                                  memcpy(resourceName, nameTmp, strlen(nameTmp) );
                         }
                 
                 //------------------------------------------------------------------
                 //------------------------------------------------------------------
                  
                 response.append( "<item>");        
                 
                 //-------------------------------------------------------------
                 response.append( "<pId xsi:type=\"xsd:int\">");                                                                                                                    
                 response.append( gxh_IntToString(users[index].shorts[k].id)  );                  
                 response.append(  "</pId>");                                                                                                        
                 //-------------------------------------------------------------
                               
                 response.append( "<pNumFunction xsi:type=\"xsd:int\">");                                                                                                          
                 response.append( gxh_IntToString(users[index].shorts[k].const_function_symbol)   ) ;  
                 response.append(  "</pNumFunction>");   
                 
                 //-------------------------------------------------------------
                               
                 response.append( "<pNameFunction xsi:type=\"xsd:string\">");                                                                                                                       
                 response.append( functionName ) ;                                                                                                 
                 response.append(  "</pNameFunction>");  
                 
                 //-------------------------------------------------------------
                               
                 response.append( "<pResourceId xsi:type=\"xsd:int\">");                                                                                                                        
                 response.append( gxh_IntToString( users[index].shorts[k].resource_id)   ) ;                                                                                                  
                 response.append(  "</pResourceId>");  
                 
                 //-------------------------------------------------------------
                               
                 response.append( "<pResourceName xsi:type=\"xsd:string\">");                                                                                                                       
                 response.append( resourceName  ) ;                                                                                                 
                 response.append(  "</pResourceName>");  
                 
                 //-------------------------------------------------------------
                               
                 response.append( "<pResourceType xsi:type=\"xsd:string\">");                                                                                                                       
                 response.append(fName  ) ;                                                                                                 
                 response.append(  "</pResourceType>");  
                 
                 //-------------------------------------------------------------
                               
                 response.append( "<pParam1 xsi:type=\"xsd:string\">");                                                                                                                       
                 response.append( users[index].shorts[k].name  ) ;                                                                                                 
                 response.append(  "</pParam1>");  
                 
                 //-------------------------------------------------------------
                               
                 response.append( "<pParam2 xsi:type=\"xsd:string\">");                                                                                                                       
                 response.append( users[index].shorts[k].text_content  ) ;                                                                                                 
                 response.append(  "</pParam2>");  
                 
                 //-------------------------------------------------------------
                               
                 response.append( "<pStatus xsi:type=\"xsd:string\">");                                                                                                                       
                 response.append( res_status  ) ;        //tutut                                                                                          
                 response.append(  "</pStatus>");  
                 
                
                 response.append( "</item>");                                                                                                        
              }
              
              response.append(  "</result></gxh:getAllShortResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");  
              
              return this->build(msgIn, message_ret, response.c_str());
}

//------------------------------------------------------------------------------

bool app_module_wsdl::getVerApp(gxh_message* msgIn,gxh_message* message_ret, int px_user_id, int px_group_id )
{
       unsigned int size = 0;
       ih_driver_values* rows = (ih_driver_values* )this->getAppHandle()->getResource(RES_VECTOR_DRIVER_VALUE, &size);
    
       int iNew = -1;
       int iNow = -1;
       
       for(unsigned int i=0; i<size; i++)
       {
         if(rows[i].ih_driver_id == -1 && rows[i].px_group_id == -1 && rows[i].const_function == VAR_VERSION_NOW)  
         {
             iNow = i;
         }
         
         if(rows[i].ih_driver_id == -1 && rows[i].px_group_id == -1 && rows[i].const_function == VAR_VERSION_NEW)  
         {
             iNew = i;
         }
       }
    
       std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:getVerAppResponse>";
             
       response.append(  "<new_version>");                                                                                                                       
       
       if(iNew >= 0)   response.append( rows[iNew].value );
       
       response.append(  "</new_version>");                                                                                                                       
       
       response.append(  "<now_version>");  
        if(iNow >= 0)   response.append( rows[iNow].value );
       response.append(  "</now_version>"); 
       
       response.append(  "</gxh:getVerAppResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");                                                                                                                       
       
       return this->build(msgIn, message_ret, response.c_str());
}

//------------------------------------------------------------------------------

bool app_module_wsdl::getInfo(gxh_message* msgIn,gxh_message* message_ret, int px_user_id, int px_group_id )
{
    
        unsigned int count = 0;
        user_gxh* users = (user_gxh*)this->getAppHandle()->getResource(RES_VECTOR_USER, &count );   
      
        int index = -1;
        
        for(unsigned int x=0; x<count; x++)
        {
            if(users[x].px_group_id == px_group_id ) { index = x; break; }
        }
            
        //----------------------------------------------------------------------                        
        std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:getInfoResponse><result>";                                                                                                        
  
              if(index != -1 ) 
              {     
                  
                 response.append( "<item>");        
                 
                 //-------------------------------------------------------------
                 response.append( "<pId xsi:type=\"xsd:int\">");                                                                                                                       
                 response.append( gxh_IntToString(users[index].px_group_id)  ) ;                                                                                         
                 response.append(  "</pId>");                                                                                                        
                 //-------------------------------------------------------------
                  
                 response.append( "<pNick xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( users[index].username ) ;                                                                                                         
                 response.append(  "</pNick>");                                                                                                        
                 //-------------------------------------------------------------                    
                 
                 response.append( "<pName xsi:type=\"xsd:string\">");                                                                                                                       
                 response.append(  users[index].name ) ;                                                                                                         
                 response.append(  "</pName>");                                                                                                        
                 //-------------------------------------------------------------
                   
                 response.append( "<pLastName xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( users[index].last_name  ) ;                                                                                                         
                 response.append(  "</pLastName>");      
                  //------------------------------------------------------------- 
                 response.append( "<pPhone xsi:type=\"xsd:string\">");                                                                                                                     
                 response.append( users[index].phone  ) ;                                                                                                         
                 response.append(  "</pPhone>");   
                 //-------------------------------------------------------------
                 
                 response.append( "<pLatitude xsi:type=\"xsd:double\">");                                                                                                                       
                 response.append(   "?" ) ;                                                                                                         
                 response.append(  "</pLatitude>");   
                 //-------------------------------------------------------------
                 response.append( "<pLongitude xsi:type=\"xsd:double\">");                                                                                                                       
                 response.append(  "?" ) ;                                                                                                         
                 response.append(  "</pLongitude>");                                                
                 //-------------------------------------------------------------
                 response.append( "<pPrivilege xsi:type=\"xsd:string\">"); //uprawnienia systemowe z px_group_privileges
                 //response.append(  '?'  ) ;                                                                                                                                                             
                 
                 for(unsigned int w=0;w< users[index].privilege_count; w++)
                 {
                    response.append( users[index].privileges[w].name);                                                                    
                    response.append(",");                                                                    
                 }
                 
                 response.append(  "</pPrivilege>");  
                 
                 //-------------------------------------------------------------
                 response.append( "<pPrivGroup xsi:type=\"xsd:string\">");   //uprawnienia stref..                                                                                                                    
                  
                 for(unsigned int w=0;w< users[index].zone_grp_count; w++)
                 {
                    response.append( users[index].zone_grups[w].name);                                                                    
                    response.append(",");                                                                    
                 }
                 
                 response.append(  "</pPrivGroup>");  
                 
                 //-------------------------------------------------------------
                 response.append( "<pPrivZone xsi:type=\"xsd:string\">");                                                                                                                                                                                                                                            
                 
                 for(unsigned int w=0;w< users[index].zones_count; w++)
                 {
                    response.append( users[index].zones[w].name);                                                                    
                    response.append(",");                                                                    
                 }
                 
                 
                 response.append(  "</pPrivZone>"); 
                 
                 //-------------------------------------------------------------
                 
                   //-------------------------------------------------------------                 
                 //-------------------------------------------------------------                                                 
              
                 std::string cPath = "";
                 cPath.append( this->getAppHandle()->getRunPath() );
                 cPath.append("../../files/awatars/");
                 cPath.append( users[index].awatar);
                 
                 
              
                 
                 this->getAppHandle()->show_log_module("app_module_wsdl","getInfo", cPath.c_str(),4  );

                 std::ifstream file (cPath.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
                 if (file.is_open())
                   {
                      
                        int size;
                        char * memblock;
                     
                        size = file.tellg();
                        memblock = new char [size];
                        file.seekg (0, std::ios::beg);
                        file.read (memblock, size);
                        file.close();                                               
                        
                        int newLength = 0;
                        char* base64 = gxh_base64_encode((const unsigned char*)memblock, size, &newLength);
                 
                        response.append( "<pAwatar xsi:type=\"xsd:string\">");                                                                                                         
                        response.append( base64 ) ;                                                                                                         
                        response.append(  "</pAwatar>"); 
                        
                        delete[] memblock;
                        delete[] base64;
                    }else
                    {
                        response.append( "<pAwatar xsi:type=\"xsd:string\">");                                                                                                         
                        response.append( "NONE" ) ;                                                                                                         
                        response.append(  "</pAwatar>"); 
                    }
                                                                                                                                        
                                   
                 //-------------------------------------------------------------
                 
                 
                 response.append( "</item>");                                                                                                        
              }
              
              response.append(  "</result></gxh:getInfoResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");                                                                                                        
 
              
             return this->build(msgIn, message_ret, response.c_str());
}

//------------------------------------------------------------------------------

bool app_module_wsdl::setDebugLevel(gxh_message* msgIn, gxh_message* message_ret,char* level)
{                    
        this->getAppHandle()->setDebugLevel( gxh_StringToInt( std::string(level) ) );
        
        std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:setDebugLevelResponse>";                                                      
        
                
        response.append("<result>");                                                  
        response.append("OK");                                                  
        response.append("</result>");                                                  
 
        
        response.append( "</gxh:setDebugLevelResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");                                                      
       
  
        return this->build(msgIn, message_ret, response.c_str());
 
}

//------------------------------------------------------------------------------

bool app_module_wsdl::setAddress(gxh_message* msgIn, gxh_message* message_ret,char* ih_driver_id, char* parity, char* dataBit, char* stopBit, char* speed)
{
        std::string response = "";
        
        unsigned int size = 0;
        ih_driver* first = (ih_driver*)this->getAppHandle()->getResource(RES_VECTOR_DRIVER, &size);
        int index = -1;
        
        int driverId = gxh_StringToInt(ih_driver_id);
        
        for(unsigned int x=0; x<size; x++)
        {
            if(first[x].id == driverId ) { index = x; break; }
        }
        
        
        if(index == -1)
        {
            response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:setAddressResponse>";                                                              
                
            response.append("<result>");                                                  
            response.append("DRIVER ERROR");                                                  
            response.append("</result>");                                                  
         
            response.append( "</gxh:setAddressResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");  
        }else
        {            
            bool isSet = this->changeAddressDriver(msgIn, gxh_StringToInt(ih_driver_id), (char*)first[index].address, gxh_StringToChar(first[index].data), gxh_StringToChar( first[index].parity), gxh_StringToChar( first[index].stop_bit), first[index].speed  );
              
            response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:setAddressResponse>";                                                              
                
            response.append("<result>");                                                  
            if(isSet) response.append("OK");                                                  
            if(!isSet) response.append("ERROR");                                                  
            response.append("</result>");                                                  
         
            response.append( "</gxh:setAddressResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");   
                   
        }                      
   
         return this->build(msgIn, message_ret, response.c_str());
}

//------------------------------------------------------------------------------

void app_module_wsdl::loadPrGroup()
{
    sqlite3* dbSqLite1;          
    std::string baza2 = this->getAppHandle()->getDB();//this->getAppHandle()->getRunPath();
    //baza2.append("../gxh_base.db");
     
    int r  =  sqlite3_open_v2(baza2.c_str(), &dbSqLite1, SQLITE_OPEN_READWRITE, NULL   );                                  
    if( r != SQLITE_OK )
    {
        this->getAppHandle()->show_error("app_module_wsdl","loadPrGroup","Cannot open sqlLite 1",0);    
        return;
    }
               
    sqlite3_stmt* stmt;    
                  
    for(int x=0; x<MAX_USER_BUFFER;x++)
    {      
       this->usersSSID[x].privilege_count = 0;
       this->usersSSID[x].zone_grp_count = 0;
       this->usersSSID[x].zones_count = 0; 
       this->usersSSID[x].driver_count = 0; 
        
       if( this->usersSSID[x].px_user_id > 0 && this->usersSSID[x].free == false )
       {
           //pobierz grupy uprawnien do panelu..            
           
           std::string query="SELECT px_hash_privilege.px_user_id,px_privileges_group.name, px_hash_privilege.px_privileges_group_id  FROM px_privileges_group, px_hash_privilege WHERE px_hash_privilege.px_privileges_group_id = px_privileges_group.id AND px_privileges_group.is_delete='0' AND px_hash_privilege.is_delete='0' ";  
           query.append(" AND px_hash_privilege.px_user_id='");
           query.append( gxh_IntToString( this->usersSSID[x].px_user_id ) );
           query.append("';");
               
           this->getAppHandle()->show_log_data_base("app_module_wsdl","loadPrGroup", query.c_str() ,6); 
           
           const char* queryChar = query.c_str();          
           if(sqlite3_prepare(dbSqLite1 , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
            {
              this->getAppHandle()->show_error("app_module_wsdl","loadPrGroup", sqlite3_errstr( sqlite3_errcode(dbSqLite1  ) ),4);          
              sqlite3_finalize(stmt);          
            }else
            {               
                bool done = false;       
                while (!done)               
                  { 
                    int res = sqlite3_step (stmt);       
                    if(res ==  SQLITE_ROW)
                     {                                         
                       const char* tmp1                                         = (const char*)sqlite3_column_text(stmt, 1 );
                       memset(this->usersSSID[x].privileges[this->usersSSID[x].privilege_count].name, 0, 50 );   
                       memcpy(this->usersSSID[x].privileges[this->usersSSID[x].privilege_count].name, tmp1, strlen(tmp1) );     
                       this->usersSSID[x].privilege_count++;
                     }               
                    if(res == SQLITE_DONE)done = true;                                     
                   }               
                sqlite3_finalize(stmt);                           
            }
           
           //pobierz uprawnienia do stref...         
           query="SELECT ih_privileges_hash.px_group_id, ih_privileges_group.id, ih_privileges_group.name FROM ih_privileges_hash,ih_privileges_group WHERE  ih_privileges_group.id=ih_privileges_hash.ih_privileges_group_id AND ih_privileges_group.is_delete='0' AND ih_privileges_hash.is_delete='0'";                 
           query.append(" AND ih_privileges_hash.px_group_id='");
           query.append( gxh_IntToString( this->usersSSID[x].px_group_id ) );
           query.append("';");
           this->getAppHandle()->show_log_data_base("app_module_wsdl","loadPrGroup", query.c_str() ,6);  
           
           queryChar = query.c_str();          
           if(sqlite3_prepare(dbSqLite1 , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
            {
              this->getAppHandle()->show_error("app_module_wsdl","loadPrGroup", sqlite3_errstr( sqlite3_errcode(dbSqLite1  ) ),4);          
              sqlite3_finalize(stmt);          
            }else
            {                
                 bool done = false;    
                 while (!done)               
                  { 
                      int res = sqlite3_step (stmt);        
                      if(res ==  SQLITE_ROW)
                       {                                                                                    
                          this->usersSSID[x].zone_grups[this->usersSSID[x].zone_grp_count].id     =  gxh_StringToInt( (const char*)sqlite3_column_text(stmt, 1 )) ;
                          
                          
                          const char* tmp1     = (const char*)sqlite3_column_text(stmt, 2 );
                          memset(this->usersSSID[x].zone_grups[this->usersSSID[x].zone_grp_count].name, 0, 50 );
                          memcpy(this->usersSSID[x].zone_grups[this->usersSSID[x].zone_grp_count].name, tmp1, strlen(tmp1) );  
                          this->usersSSID[x].zone_grp_count++;
                       }               
                      if(res == SQLITE_DONE)done = true;                                     
                  }
                  sqlite3_finalize(stmt);      
            }
           
           //pobierz uprawnienia do poszczególnych stref...
           for(unsigned int y=0; y<this->usersSSID[x].zone_grp_count; y++) 
           {
                    query="SELECT ih_privileges_group_zone.ih_privileges_group_id, ih_privileges_group_zone.ih_zone_id,  ih_zone.name, ih_zone.ih_sector_id, ih_zone.type FROM ih_privileges_group_zone, ih_zone WHERE  ih_privileges_group_zone.ih_zone_id=ih_zone.id AND ih_zone.is_delete='0' AND ih_privileges_group_zone.is_delete='0'";                 
                    query.append(" AND ih_privileges_group_zone.ih_privileges_group_id='");                    
                    query.append( gxh_IntToString( this->usersSSID[x].zone_grups[y].id ) );                    
                    query.append("';");                    
                    this->getAppHandle()->show_log_data_base("gxh_core","loadPrGroup", query.c_str() ,6);  
                    
                    queryChar = query.c_str();          
                    if(sqlite3_prepare(dbSqLite1 , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
                       {
                        this->getAppHandle()->show_error("gxh_core","loadPrGroup", sqlite3_errstr( sqlite3_errcode(dbSqLite1  ) ),4);          
                        sqlite3_finalize(stmt);                          
                       }else
                       {
                        
                         bool done = false;    
                         while (!done)               
                          { 
                              int res = sqlite3_step (stmt);        
                              if(res ==  SQLITE_ROW)
                                  {                                 
                                  
                                    this->usersSSID[x].zones[this->usersSSID[x].zones_count].id      = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 1) ) );
                                    
                                    const char* tmpX   = (const char*)sqlite3_column_text(stmt, 2 );
                                    memset(this->usersSSID[x].zones[this->usersSSID[x].zones_count].name, 0, 50);
                                    memcpy(this->usersSSID[x].zones[this->usersSSID[x].zones_count].name, tmpX, strlen(tmpX)); 
                                                                                                          
                                  
                                     this->usersSSID[x].zones[this->usersSSID[x].zones_count].ih_sector_id      = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 3) ) );
                                   
                                    
                                     this->usersSSID[x].zones_count++;
                                  }               
                                  if(res == SQLITE_DONE)done = true;                                     
                          }                        
                          sqlite3_finalize(stmt); 
                       }
           }
           
           // uprawnienia indywidualne do stref..
                    query="SELECT ih_zone_privilege.px_group_id, ih_zone_privilege.ih_zone_id,ih_zone.name, ih_zone.ih_sector_id FROM ih_zone_privilege, ih_zone WHERE ih_zone_privilege.ih_zone_id=ih_zone.id AND ih_zone_privilege.is_delete='0'";                 
                    query.append(" AND ih_zone_privilege.px_group_id='");                    
                    query.append( gxh_IntToString( this->usersSSID[x].px_group_id) );                    
                    query.append("';");                    
                    this->getAppHandle()->show_log_data_base("gxh_core","loadPrGroup", query.c_str() ,6);  
                    
                    queryChar = query.c_str();          
                    if(sqlite3_prepare(dbSqLite1 , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
                       {
                        this->getAppHandle()->show_error("gxh_core","loadPrGroup", sqlite3_errstr( sqlite3_errcode(dbSqLite1  ) ),4);          
                        sqlite3_finalize(stmt);                          
                       }else
                       {
                         bool done = false;    
                         while (!done)               
                          { 
                              int res = sqlite3_step (stmt);        
                              if(res ==  SQLITE_ROW)
                                  {                                 
                                  
                                    this->usersSSID[x].zones[this->usersSSID[x].zones_count].id      = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 1) ) );
                                    
                                    const char* tmpX   = (const char*)sqlite3_column_text(stmt, 2 );
                                    memset(this->usersSSID[x].zones[this->usersSSID[x].zones_count].name, 0, 50);
                                    memcpy(this->usersSSID[x].zones[this->usersSSID[x].zones_count].name, tmpX, strlen(tmpX)); 
                                  
                                    this->usersSSID[x].zones[this->usersSSID[x].zones_count].ih_sector_id      = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 3) ) );
                                   
                                  
                                     this->usersSSID[x].zones_count++;
                                  }               
                                  if(res == SQLITE_DONE)done = true;                                     
                          }                        
                          sqlite3_finalize(stmt); 
                       }
            //pobierz uprawnie ia do urzadzen..
                    query="SELECT ih_driver_id, ih_driver.name,ih_driver.ih_sl_driver_id FROM ih_driver_group_hash, ih_driver WHERE ih_driver_group_hash.is_delete='0' AND ih_driver.is_delete='0' AND  ih_driver_id=ih_driver.id AND px_group_id='";                    
                    query.append( gxh_IntToString( this->usersSSID[x].px_group_id) );                    
                    query.append("';");                    
                    this->getAppHandle()->show_log_data_base("gxh_core","loadPrGroup", query.c_str() ,6);  
                    
                    queryChar = query.c_str();          
                    if(sqlite3_prepare(dbSqLite1 , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
                       {
                        this->getAppHandle()->show_error("gxh_core","loadPrGroup", sqlite3_errstr( sqlite3_errcode(dbSqLite1  ) ),4);          
                        sqlite3_finalize(stmt);                          
                       }else
                       {
                         bool done = false;    
                         while (!done)               
                          { 
                              int res = sqlite3_step (stmt);        
                              if(res ==  SQLITE_ROW)
                                  {                                 
                                  
                                    this->usersSSID[x].drivers[ this->usersSSID[x].driver_count].ih_driver_id  = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 0) ) );
                                    
                                    const char* tmpX   = (const char*)sqlite3_column_text(stmt, 1 );
                                    memset(this->usersSSID[x].drivers[ this->usersSSID[x].driver_count].name, 0, 100);                      
                                    memcpy(this->usersSSID[x].drivers[ this->usersSSID[x].driver_count].name, tmpX, strlen(tmpX) );                      
                                    
                                    this->usersSSID[x].drivers[ this->usersSSID[x].driver_count].ih_sl_driver_id  = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 2 ) ) );
                                  
                                    this->usersSSID[x].driver_count++;
                                  }               
                                  if(res == SQLITE_DONE)done = true;                                     
                          }                        
                          sqlite3_finalize(stmt); 
                       }
                    
                               
           
       }
    }                 
                   
                           
    sqlite3_close_v2(dbSqLite1);     
    
    for(int x=0; x<MAX_USER_BUFFER;x++)
    {      
      // this->usersSSID[x].privilege_count = 0;
      // this->usersSSID[x].zone_grp_count = 0;
     //  this->usersSSID[x].zones_count = 0; 
        
       if( this->usersSSID[x].px_user_id > 0 && this->usersSSID[x].free == false )
       {
            std::string msgL = "GXH User: ";
            msgL.append(this->usersSSID[x].username);
             
            if(this->usersSSID[x].zones_count == 0)
            {
                msgL.append(", allow: everywhere");
            }
            else
            {
              msgL.append(", allow: ");
              for(unsigned int y=0; y<this->usersSSID[x].zones_count; y++)
              {
                   msgL.append(this->usersSSID[x].zones[y].name);
                   msgL.append(",");
              }
            }
                        
            this->getAppHandle()->show_log_module("gxh_core","loadPrGroup", msgL.c_str() ,4); 
       }
    }
}

//------------------------------------------------------------------------------

     
 bool app_module_wsdl::alowSector(int ih_sector_id, int px_group_id)
 {
     
    for(int x=0; x<MAX_USER_BUFFER;x++)
    {
        if(this->usersSSID[x].px_group_id == px_group_id)
        {
            if(this->usersSSID[x].zones_count > 0)
            {
                
                for(unsigned int mm=0; mm<this->usersSSID[x].zones_count; mm++ )
                {
                    if(this->usersSSID[x].zones[mm].ih_sector_id == ih_sector_id)
                    {
                        return true;
                    }
                }
                
                
            }else
            {
                return true; //nie ma indywidulnych stref wieć moze wszedzie..
            }
            
            break;
        }        
    }
     
     return false;
 }
     
//------------------------------------------------------------------------------
     
 bool app_module_wsdl::alowZone(int ih_zone_id, int px_group_id)
 {
       for(int x=0; x<MAX_USER_BUFFER;x++)
    {
        if(this->usersSSID[x].px_group_id == px_group_id)
        {
            if(this->usersSSID[x].zones_count > 0)
            {
                
                for(unsigned int mm=0; mm<this->usersSSID[x].zones_count; mm++ )
                {
                    if(this->usersSSID[x].zones[mm].id == ih_zone_id)
                    {
                        return true;
                    }
                }
                
                
            }else
            {
                return true; //nie ma indywidulnych stref wieć moze wszedzie..
            }
            
            break;
        }        
    }
     
     return false;
 }


//------------------------------------------------------------------------------

void app_module_wsdl::loadSessionToCache()
{  
     sqlite3* dbSqLite1;      
     
     int index = 0;
    
     std::string baza2 = this->getAppHandle()->getDBTmp();//this->getAppHandle()->getRunPath();
     //baza2.append("../gxh_base_tmp.db");
     
     int r  =  sqlite3_open_v2(baza2.c_str(), &dbSqLite1, SQLITE_OPEN_READWRITE, NULL   );                                  
     if( r != SQLITE_OK )
     {
          this->getAppHandle()->show_error("app_module_wsdl","loadSessionToCache","Cannot open sqlLite 2",0);             
     }else
     {
         sqlite3_stmt* stmt;     
         std::string query="SELECT px_user_id,session_id FROM px_session WHERE operation='login' AND is_delete='0' AND session_id LIKE '%GXH%' GROUP BY px_user_id LIMIT 200;";           
         this->getAppHandle()->show_log_data_base("app_module_wsdl", "loadSessionToCache",  query.c_str(), 6);
         
  
         const char* queryChar = query.c_str();        
         if(sqlite3_prepare(dbSqLite1 , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
         {
           this->getAppHandle()->show_error("app_module_wsdl","loadSessionToCache", sqlite3_errstr( sqlite3_errcode(dbSqLite1  ) ),4);                                   
         }else
         {
           
             bool done = false;       
             while (!done)               
             { 
               int res = sqlite3_step (stmt);        
               if(res ==  SQLITE_ROW)
               {                
                 int px_user_id    = gxh_StringToInt(  (const char*)sqlite3_column_text(stmt,0)   );                                 
                 const char* sessionid =  (const char*)sqlite3_column_text(stmt,1);      
                 
              
                 if(sessionid != NULL)                                                    
                 {
                   this->usersSSID[index].px_user_id = px_user_id;
                   this->usersSSID[index].px_group_id = -2;                 
                   this->usersSSID[index].ssid = std::string( sessionid);                                    
                   this->usersSSID[index].toSave = false;
                   this->usersSSID[index].free = false;                                                                  
                   this->getAppHandle()->show_log_module("app_module_wsdl", "loadSessionToCache 0x01",  this->usersSSID[index].ssid.c_str(), 5);                                    
                   index++;
                 }
                  
                 
               }               
               if(res == SQLITE_DONE)done = true;                                     
            }
         }     
         sqlite3_finalize(stmt);                   
         sqlite3_close_v2(dbSqLite1);
     }
     
     /////
     
    for(int x=0; x<MAX_USER_BUFFER;x++)
    {      
       if( this->usersSSID[x].px_group_id == -2 && this->usersSSID[x].free == false)
       {
             sqlite3* dbSqLite1;          
             std::string baza2 = this->getAppHandle()->getDB();//this->getAppHandle()->getRunPath();
             //baza2.append("../gxh_base.db");
     
             int r  =  sqlite3_open_v2(baza2.c_str(), &dbSqLite1, SQLITE_OPEN_READWRITE, NULL   );                                  
             if( r != SQLITE_OK )
               {
                  this->getAppHandle()->show_error("app_module_wsdl","loadSessionToCache","Cannot open sqlLite 1",0);             
               }else
               {
                  sqlite3_stmt* stmt;     
                  std::string query="SELECT px_user.id, px_user.group_id, px_user.username , px_group.name, px_group.last_name, px_group.telefon_kontaktowy, px_group.awatar, px_group.data_profil, px_user.is_delete FROM px_user, px_group WHERE px_user.group_id = px_group.id AND px_user.id='";        
                  query.append(gxh_IntToString( this->usersSSID[x].px_user_id ));
                  query.append("'; ");
         
                  this->getAppHandle()->show_log_data_base("app_module_wsdl", "loadSessionToCache",  query.c_str(), 6);
           
                  const char* queryChar = query.c_str();        
                  if(sqlite3_prepare(dbSqLite1 , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
                   {
                    this->getAppHandle()->show_error("app_module_wsdl","loadSessionToCache", sqlite3_errstr( sqlite3_errcode(dbSqLite1  ) ),4);                                   
                   }else
                   {
                          bool done = false;       
                          while (!done)               
                          { 
                               int res = sqlite3_step (stmt);        
                               if(res ==  SQLITE_ROW)
                               {                
                                  int px_user_id   = gxh_StringToInt(  (const char*)sqlite3_column_text(stmt,0)   );                                 
                                  int px_group_id  = gxh_StringToInt(  (const char*)sqlite3_column_text(stmt,1)   );                                 
                                  const char* tmp1 = (const char*)sqlite3_column_text(stmt,2);
                                  
                                  this->usersSSID[x].px_group_id = px_group_id;
                                  this->usersSSID[x].px_user_id = px_user_id;
                                  
                                  memcpy(this->usersSSID[x].username, tmp1, strlen(tmp1));
                                  
                                  const char* tmp3 = (const char*)sqlite3_column_text(stmt,3);
                                  memcpy(this->usersSSID[x].name, tmp3, strlen(tmp3));        
                                  
                                  const char* tmp4 = (const char*)sqlite3_column_text(stmt,4);
                                  memcpy(this->usersSSID[x].last_name, tmp4, strlen(tmp4)); 
                                  
                                  const char* tmp5 = (const char*)sqlite3_column_text(stmt,5);
                                  memcpy(this->usersSSID[x].phone, tmp5, strlen(tmp5)); 
                                  
                                  const char* tmp6 = (const char*)sqlite3_column_text(stmt,6);
                                  memcpy(this->usersSSID[x].awatar, tmp6, strlen(tmp6)); 
                                  
                                //   const char* tmp7 = (const char*)sqlite3_column_text(stmt,7);
                                 // memcpy(this->usersSSID[x].dataProfil, tmp7, strlen(tmp7)); 
                                                                   
                                  int is_delete   = gxh_StringToInt(  (const char*)sqlite3_column_text(stmt,8)   );       
                                  
                                  if(is_delete == 1)
                                  {
                                      this->usersSSID[x].px_group_id = -1;
                                      this->usersSSID[x].px_user_id = -1;
                                  }
                                  
                                  this->getAppHandle()->show_log_data_base("app_module_wsdl", "loadSessionToCache", (const char*) this->usersSSID[x].username , 3);
                                  
                                  
                                 
                                  
                               }               
                               if(res == SQLITE_DONE)done = true;                                     
                          }
                   }
                 
                  sqlite3_finalize(stmt);         
                  sqlite3_close_v2(dbSqLite1);
               }
       }
    }
    
    this->loadPrGroup();
    
    
    this->getAppHandle()->destroyResource(RES_VECTOR_USER);
    this->getAppHandle()->registerResource(this->usersSSID, MAX_USER_BUFFER, RES_VECTOR_USER); 
   
     
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

bool app_module_wsdl::setClick(gxh_message* msgIn, gxh_message* message_ret,char* ih_driver_port_id)
{                   
        unsigned int size = 0;
        ih_driver_port* first = (ih_driver_port*)this->getAppHandle()->getResource(RES_VECTOR_DRIVER_PORT, &size);
        int index = -1;
        
        int portId = gxh_StringToInt(ih_driver_port_id);
        
        for(unsigned int x=0; x<size; x++)
        {
            if(first[x].id == portId)
            {
                index = x;
                break;
            }
        }
      
        
         std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:setClickPortResponse>";                                                      
       
         if( index == -1)
         {
               response.append("<result>");                                                  
               response.append("ERROR PORT");                                                  
               response.append("</result>");  
         }else
         {
              if( this->setPortStatus( portId, IH_SET_PORT_CLICK ,-1,-1,-1 ))
              {
               response.append("<result>");                                                  
               response.append("OK");                                                  
               response.append("</result>"); 
              }else
              {
               response.append("<result>");                                                  
               response.append("ERROR");                                                  
               response.append("</result>"); 
              }
             
         }                                                                           
        
         response.append( "</gxh:setClickPortResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");        
         return this->build(msgIn, message_ret, response.c_str());
 
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

 bool app_module_wsdl::getAllFunction(gxh_message* msgIn, gxh_message* message_ret )
 {        
        unsigned int count = 0;
        ih_sl_const_function_symbol* rows = (ih_sl_const_function_symbol*) this->getAppHandle()->getResource( RES_VECTOR_FUNC , &count); 
      
                                      
              std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:getAllFunction><result>";                                                                                                        
  
              for(unsigned int k=0; k<count;k++) 
              {   
                 response.append( "<item>");        
                 
                 //-------------------------------------------------------------
                 response.append( "<pId>");                                                                                                                    
                 response.append(  gxh_IntToString(rows[k].id)   ) ;                                                                                                    
                 response.append(  "</pId>");                                                                                                        
                 //-------------------------------------------------------------
                  
                 response.append( "<pName xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( rows[k].description  ) ;                                                                                                         
                 response.append(  "</pName>");                                                                                                        
                 //-------------------------------------------------------------   
                 
                 response.append( "<pFuncId xsi:type=\"xsd:int\">");                                                                                                         
                 response.append( gxh_IntToString(rows[k].const_function_symbol)   ) ;           
                 response.append(  "</pFuncId>");                                                                                                        
                 //-------------------------------------------------------------
                   
                 response.append( "<pType xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( rows[k].type  ) ;                                                                                                         
                 response.append(  "</pType>");      
                  //-------------------------------------------------------------
                 
                 response.append( "<pFuncService xsi:type=\"xsd:string\">");                                                                                                                      
                 response.append( gxh_IntToString(rows[k].run_as_service)   ) ;                                                                                                     
                 response.append(  "</pFuncService>");   
                 //-------------------------------------------------------------
                 
                 response.append( "<pDelay xsi:type=\"xsd:int\">");                                                                                                                        
                 response.append( gxh_IntToString(rows[k].run_ms_delay)  ) ;                                                                                                 
                 response.append(  "</pDelay>");   
                 
                   
                 response.append( "<pResourceName xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( rows[k].resourceName  ) ;                                                                                                         
                 response.append(  "</pResourceName>");      
                  //-------------------------------------------------------------
                 
                  response.append( "<pParam1Name xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( rows[k].param1Name  ) ;                                                                                                         
                 response.append(  "</pParam1Name>"); 
                 
                 
                 response.append( "<pParam2Name xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( rows[k].param2Name  ) ;                                                                                                         
                 response.append(  "</pParam2Name>"); 
                 
                 response.append( "</item>");                                                                                                        
              }
              
              response.append(  "</result></gxh:getAllFunction></SOAP-ENV:Body></SOAP-ENV:Envelope>");      
                                  
              return this->build(msgIn, message_ret, response.c_str()); 
     
 } 


//------------------------------------------------------------------------------

bool app_module_wsdl::removeActionScene(gxh_message* msgIn,  gxh_message* message_ret, int pId )
{                       
              unsigned int size = 0;
              ih_scene_action* rows = (ih_scene_action*)this->getAppHandle()->getResource( RES_VECTOR_SCENE_ACTION , &size);  
    
            
               for(unsigned int k=0; k<size;k++) 
               {
                 if(rows[k].id == pId )
                 {
                     rows[k].status = 3; //go to remove...
                     rows[k].ih_scene_id = -1;
                     break;
                 }
               }
              
               
              
              std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:removeActionSceneResponse>";                                                                                                                            
              response.append("<result  xsi:type=\"xsd:string\">OK</result>");                                      
              response.append(  "</gxh:removeActionSceneResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");                                                                                                                       
              return this->build(msgIn, message_ret, response.c_str()); 
}

//------------------------------------------------------------------------------
 
 bool app_module_wsdl::getAllSceneAction(gxh_message* msgIn, gxh_message* message_ret, int idTop )
 {       
              unsigned int size = 0;
              ih_scene_action* rows = (ih_scene_action*)this->getAppHandle()->getResource( RES_VECTOR_SCENE_ACTION , &size);  
                                   
              std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:getAllSceneActionResponse><result>";                                                                                                        
  
              for(unsigned int k=0; k<size;k++) 
              {
                 if(rows[k].free) continue;
                 if(rows[k].status == 3) continue; //rows to delete... 
                 if(rows[k].ih_scene_id != idTop) continue; //rows to delete... 
                 
                 
                 ///----------------------------------
                 ///----------------------------------
    
                
                 char functionName[255]; // Wlacznik swiatla
                 char resourceName[255];   // Lampa
                 char fName[100]; //light blind
                 char res_status[100];  //0 1
                 
                 memset(functionName, 0, 255);
                 memset(resourceName, 0, 255);
                 memset(fName, 0, 100);
                 memset(res_status, 0, 100);
                 
             
                       
                       unsigned int symbolCount = 0;
                       ih_sl_const_function_hash* symbolH = ( ih_sl_const_function_hash* )this->getAppHandle()->getResource( RES_VECTOR_SYMBOL_HASH , &symbolCount); 
                       
                       for(unsigned int p=0;p<symbolCount; p++)
                       {
                           if(symbolH[p].symbol == rows[k].const_function_symbol )
                           {                           
                               memcpy(fName,symbolH[p].name, strlen(symbolH[p].name) );                               
                           }
                       }                       
                    
                       
                       unsigned int count = 0;
                       ih_sl_const_function_symbol* rowsF = (ih_sl_const_function_symbol*) this->getAppHandle()->getResource( RES_VECTOR_FUNC , &count); 
                       
                       for(unsigned w=0; w<count; w++)
                       {
                           if(rowsF[w].const_function_symbol ==rows[k].const_function_symbol )
                           {
                               memcpy(functionName, rowsF[w].description, strlen( rowsF[w].description) );
                           }
                       }
      
                       
                        //---
                 
                       
                       if(gxh_compare(fName,"light") == 0) 
                         {
                                  unsigned int countT = 0;
                                  ih_lighting* lights   = (ih_lighting*)this->getAppHandle()->getResource(RES_VECTOR_LIGHT, &countT); 
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                         
                                      if(lights[b].id == rows[k].resource_id )
                                      {
                                          memcpy(resourceName, lights[b].name, strlen(lights[b].name) );                                         
                                      }
                                  }
                         }
                        else
                            if(gxh_compare(fName,"outs") == 0) 
                         {
                                  unsigned int countT = 0;
                                  ih_outs* outs   = (ih_outs*)this->getAppHandle()->getResource(RES_VECTOR_OUTS, &countT); 
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                         
                                      if(outs[b].id == rows[k].resource_id )
                                      {
                                          memcpy(resourceName, outs[b].name, strlen(outs[b].name) );                                         
                                      }
                                  }
                         }
                        else
                        if(gxh_compare(fName,"blind") == 0) 
                         {
                                  unsigned int countT = 0;
                                  ih_blind* blinds      = (ih_blind*)this->getAppHandle()->getResource(RES_VECTOR_BLIND, &countT); 
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                                      if(blinds[b].id ==  rows[k].resource_id )
                                      {
                                          memcpy(resourceName, blinds[b].name, strlen(blinds[b].name) );                                       
                                      }
                                  }
                         }
                        else
                       if(gxh_compare(fName,"gate") == 0) 
                         {
                                  unsigned int countT = 0;
                                  ih_gate* gates        = (ih_gate*)this->getAppHandle()->getResource(RES_VECTOR_GATE, &countT); 
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                                      if(gates[b].id ==  rows[k].resource_id )
                                      {
                                          memcpy(resourceName, gates[b].name, strlen(gates[b].name) );                                    
                                      }
                                  }
                         }
                       else
                       if(gxh_compare(fName,"sensor") == 0) 
                         {
                                  unsigned int countT = 0;
                                  ih_sensor* sensors    = (ih_sensor*)this->getAppHandle()->getResource(RES_VECTOR_SENSOR, &countT); 
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                                      if(sensors[b].id ==  rows[k].resource_id )
                                      {
                                          memcpy(resourceName, sensors[b].name, strlen(sensors[b].name) );
                                      }
                                  }
                         }else
                         if(gxh_compare(fName,"driver") == 0) 
                         {
                                  unsigned int countT = 0;
                                  ih_driver* driver    = (ih_driver*)this->getAppHandle()->getResource(RES_VECTOR_DRIVER, &countT); 
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                                      if(driver[b].id ==  rows[k].resource_id )
                                      {
                                          memcpy(resourceName, driver[b].name, strlen(driver[b].name) );
                                      }
                                  }
                         }else
                         if(gxh_compare(fName,"output") == 0) 
                         {
                                  unsigned int countT = 0;
                                  ih_driver_port* driver    = (ih_driver_port*)this->getAppHandle()->getResource(RES_VECTOR_DRIVER_PORT, &countT); 
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                                      if(driver[b].id ==  rows[k].resource_id )
                                      {
                                          memcpy(resourceName, "PORT", 4  );
                                      }
                                  }
                         }else
                         if(gxh_compare(fName,"user") == 0) 
                         {
                                  unsigned int countT = 0;
                                  user_gxh* users = (user_gxh*)this->getAppHandle()->getResource(RES_VECTOR_USER, &countT );              
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                                      if(users[b].px_group_id ==  rows[k].resource_id )
                                      {
                                          memcpy(resourceName, users[b].username, strlen(users[b].username) );
                                      }
                                  }
                         }else
                         {
                                  const char* nameTmp = "Brak zasobu";
                                  memcpy(resourceName, nameTmp, strlen(nameTmp) );
                         }
                 
            
                 ///----------------------------------
                 ///----------------------------------
                  
                 response.append( "<item>");        
                 
                 //-------------------------------------------------------------
                 response.append( "<pId xsi:type=\"xsd:int\">");                                                                                                                          
                 response.append( gxh_IntToString(rows[k].id)   ) ;                                                                                                     
                 response.append(  "</pId>");                                                                                                                                                                                                                                        
                 //-------------------------------------------------------------
                 
                 response.append( "<pFuncId xsi:type=\"xsd:int\">");                                                                                                                       
                 response.append(  gxh_IntToString(rows[k].const_function_symbol)   ) ;                                                                                              
                 response.append(  "</pFuncId>");                                                                                                        
                 //-------------------------------------------------------------
                 
                 response.append( "<pResourceId xsi:type=\"xsd:int\">");                                                                                                                       
                 response.append(  gxh_IntToString(rows[k].resource_id)  ) ;                                                                                         
                 response.append(  "</pResourceId>");                                                                                                        
                 //-------------------------------------------------------------
                 
                 response.append( "<pIndex xsi:type=\"xsd:int\">");                                                                                                                        
                 response.append( gxh_IntToString(rows[k].order_by)   ) ;                                                                                                        
                 response.append(  "</pIndex>");                                                                                                        
                 //-------------------------------------------------------------
                 
                 response.append( "<pDelay xsi:type=\"xsd:int\">");                                                                                                                        
                 response.append(  gxh_IntToString(rows[k].delay_from_start_ms)  ) ;                                                                                                    
                 response.append(  "</pDelay>");                                                                                                        
                 //-------------------------------------------------------------
                   
                 response.append( "<pName xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( rows[k].name  ) ;                                                                                                         
                 response.append(  "</pName>");                                                                                                        
                 //-------------------------------------------------------------
                   
                 response.append( "<pText xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( rows[k].text_content ) ;                                                                                                         
                 response.append(  "</pText>");                                                                                                        
                 //-------------------------------------------------------------
                   
                 response.append( "<pSceneId xsi:type=\"xsd:int\">");                                                                                                                       
                 response.append( gxh_IntToString(rows[k].ih_scene_id)   ) ;                                                                                                      
                 response.append(  "</pSceneId>");                                                                                                        
                 //-------------------------------------------------------------
                   
                 response.append( "<pNameFunction xsi:type=\"xsd:string\">");                                                                                                         
                 response.append(functionName ) ;                                                                                                         
                 response.append(  "</pNameFunction>");                                                                                                        
                 //-------------------------------------------------------------                                                                     
                 
                   
                 response.append( "<pResourceName xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( resourceName ) ;                                                                                                         
                 response.append(  "</pResourceName>");                                                                                                        
                 //-------------------------------------------------------------    
                                           
                 
                 response.append( "</item>");                                                                                                        
              }
              
              response.append(  "</result></gxh:getAllSceneActionResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");                                                                                                                      
              return this->build(msgIn, message_ret, response.c_str()); 
 }

//------------------------------------------------------------------------------
 
//------------------------------------------------------------------------------

bool app_module_wsdl::addSceneAction(gxh_message* msgIn, gxh_message* message_ret, int idScene, int funcCode, int resourceId, char* param1, char* param2 )
{                      
              int idSceneAction = -1;              
              
              unsigned int size = 0;
              ih_scene_action* rows = (ih_scene_action*)this->getAppHandle()->getResource( RES_VECTOR_SCENE_ACTION , &size); 
              
              for(unsigned int x=1; x<size; x++)
              {
                  if(rows[x].free)
                  {
                      rows[x].free                  = false;
                      rows[x].id                    = x;
                      rows[x].ih_scene_id           = idScene;
                      rows[x].const_function_symbol = funcCode;
                      rows[x].resource_id           = resourceId;
                      
                      memset(rows[x].name,0, 500);
                      if(strlen(param1) < 500) memcpy(rows[x].name, param1, strlen(param1));
                      
                      memset(rows[x].text_content,0, 500);
                      if(strlen(param2) < 500) memcpy(rows[x].text_content, param2, strlen(param2));
                      
                      rows[x].status = 2; //add to base..
                      
                      idSceneAction = x;
                      break;
                  }
              }
    
             
              
              
              std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:addSceneActionResponse>";                                                                                                        
      
              if(idSceneAction >0)
              {
                 response.append("<result  xsi:type=\"xsd:string\">OK</result>");                        
                 response.append("<pId   xsi:type=\"xsd:int\">");                                          
                 response.append(gxh_IntToString(idSceneAction));                                   
                 response.append("</pId>");                        
              }else
              {
                 response.append("<result  xsi:type=\"xsd:string\">ERROR</result>");                        
                 response.append("<pId>-1</pId>");    
              }
              
              response.append(  "</gxh:addSceneActionResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");                                                                                                        
   
           
              
              return this->build(msgIn, message_ret, response.c_str());
      
}
 
 
//------------------------------------------------------------------------------

bool app_module_wsdl::removeScene(gxh_message* msgIn, gxh_message* message_ret, int pId )
{      
         
        unsigned int size = 0;
        ih_scene* rows = (ih_scene*)this->getAppHandle()->getResource( RES_VECTOR_SCENE , &size);                 
        
        for(unsigned int x=1;x<size; x++)
          {
                 if(!rows[x].free && rows[x].id == pId )
                 {
                    rows[x].status = 3; //to delete...                    
                 }
          }
        
        unsigned int size2 = 0;
        ih_scene_action* rows2 = (ih_scene_action*)this->getAppHandle()->getResource( RES_VECTOR_SCENE_ACTION , &size2);  
        
        
        for(unsigned int x=0; x<size2;x++)
        {
           if(rows2[x].ih_scene_id == pId) 
           {
               rows2[x].status = 3; //to delete...           
           }           
        }
        
       
        
        std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:removeSceneResponse>";                                                                                                                            
        response.append("<result  xsi:type=\"xsd:string\">OK</result>");                                      
        response.append(  "</gxh:removeSceneResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");                                                                                                                     
        return this->build(msgIn, message_ret, response.c_str());
         
}


//------------------------------------------------------------------------------

bool app_module_wsdl::removeTask(gxh_message* msgIn, gxh_message* message_ret, int id  )
{
      unsigned int size = 0;
      ih_task* rows = (ih_task*)this->getAppHandle()->getResource( RES_VECTOR_TASK , &size);  
      
      for(unsigned int x=0; x<size; x++)
      {
          if(rows[x].id == id)
          {
              rows[x].status = 3;
              
              //sleep(3); //czekaj na synfornizacje..
              
          }
      }
      
             std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:removeTask>";                                                                                                                            
        response.append("<result  xsi:type=\"xsd:string\">OK</result>");                                      
        response.append(  "</gxh:removeTask></SOAP-ENV:Body></SOAP-ENV:Envelope>");                                                                                                                     
        return this->build(msgIn, message_ret, response.c_str());
}

//------------------------------------------------------------------------------

bool app_module_wsdl::getAllTimer(gxh_message* msgIn, gxh_message* message_ret  )
{
      unsigned int size = 0;
      ih_task* rows = (ih_task*)this->getAppHandle()->getResource( RES_VECTOR_TASK , &size);  
      
         std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:getAllTimer><result>";                                                                                                        
  
              for(unsigned int k=0; k<size ;k++) 
              {
                 if(rows[k].free)   continue;
                 if(rows[k].status ==3)   continue; //is to delete..
                 
                 
                 
                   //------------------------------------------------------------------
                 //------------------------------------------------------------------
                
                 char functionName[255]; // Wlacznik swiatla
                 char resourceName[255];   // Lampa
                 char fName[100]; //light blind
                 char res_status[100];  //0 1
                 
                 memset(functionName, 0, 255);
                 memset(resourceName, 0, 255);
                 memset(fName, 0, 100);
                 memset(res_status, 0, 100);
                 
             
                       
                       unsigned int symbolCount = 0;
                       ih_sl_const_function_hash* symbolH = ( ih_sl_const_function_hash* )this->getAppHandle()->getResource( RES_VECTOR_SYMBOL_HASH , &symbolCount); 
                       
                       for(unsigned int p=0;p<symbolCount; p++)
                       {
                           if(symbolH[p].symbol == rows[k].const_function_symbol )
                           {                           
                               memcpy(fName,symbolH[p].name, strlen(symbolH[p].name) );                               
                           }
                       }                       
                    
                       
                       unsigned int count = 0;
                       ih_sl_const_function_symbol* rowsF = (ih_sl_const_function_symbol*) this->getAppHandle()->getResource( RES_VECTOR_FUNC , &count); 
                       
                       for(unsigned w=0; w<count; w++)
                       {
                           if(rowsF[w].const_function_symbol ==rows[k].const_function_symbol )
                           {
                               memcpy(functionName, rowsF[w].description, strlen( rowsF[w].description) );
                           }
                       }
      
                       
                        //---
                 
                       
                       if(gxh_compare(fName,"light") == 0) 
                         {
                                  unsigned int countT = 0;
                                  ih_lighting* lights   = (ih_lighting*)this->getAppHandle()->getResource(RES_VECTOR_LIGHT, &countT); 
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                         
                                      if(lights[b].id == rows[k].resource_id )
                                      {
                                          memcpy(resourceName, lights[b].name, strlen(lights[b].name) );                                         
                                      }
                                  }
                         }
                        else
                            if(gxh_compare(fName,"outs") == 0) 
                         {
                                  unsigned int countT = 0;
                                  ih_outs* outs   = (ih_outs*)this->getAppHandle()->getResource(RES_VECTOR_OUTS, &countT); 
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                         
                                      if(outs[b].id == rows[k].resource_id )
                                      {
                                          memcpy(resourceName, outs[b].name, strlen(outs[b].name) );                                         
                                      }
                                  }
                         }
                        else
                        if(gxh_compare(fName,"blind") == 0) 
                         {
                                  unsigned int countT = 0;
                                  ih_blind* blinds      = (ih_blind*)this->getAppHandle()->getResource(RES_VECTOR_BLIND, &countT); 
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                                      if(blinds[b].id ==  rows[k].resource_id )
                                      {
                                          memcpy(resourceName, blinds[b].name, strlen(blinds[b].name) );                                       
                                      }
                                  }
                         }
                        else
                       if(gxh_compare(fName,"gate") == 0) 
                         {
                                  unsigned int countT = 0;
                                  ih_gate* gates        = (ih_gate*)this->getAppHandle()->getResource(RES_VECTOR_GATE, &countT); 
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                                      if(gates[b].id ==  rows[k].resource_id )
                                      {
                                          memcpy(resourceName, gates[b].name, strlen(gates[b].name) );                                    
                                      }
                                  }
                         }
                       else
                       if(gxh_compare(fName,"sensor") == 0) 
                         {
                                  unsigned int countT = 0;
                                  ih_sensor* sensors    = (ih_sensor*)this->getAppHandle()->getResource(RES_VECTOR_SENSOR, &countT); 
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                                      if(sensors[b].id ==  rows[k].resource_id )
                                      {
                                          memcpy(resourceName, sensors[b].name, strlen(sensors[b].name) );
                                      }
                                  }
                         }else
                         if(gxh_compare(fName,"driver") == 0) 
                         {
                                  unsigned int countT = 0;
                                  ih_driver* driver    = (ih_driver*)this->getAppHandle()->getResource(RES_VECTOR_DRIVER, &countT); 
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                                      if(driver[b].id ==  rows[k].resource_id )
                                      {
                                          memcpy(resourceName, driver[b].name, strlen(driver[b].name) );
                                      }
                                  }
                         }else
                         if(gxh_compare(fName,"user") == 0) 
                         {
                                  unsigned int countT = 0;
                                  user_gxh* users = (user_gxh*)this->getAppHandle()->getResource(RES_VECTOR_USER, &countT );              
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                                      if(users[b].px_group_id ==  rows[k].resource_id )
                                      {
                                          memcpy(resourceName, users[b].username, strlen(users[b].username) );
                                      }
                                  }
                         }else
                         {
                                  const char* nameTmp = "Brak zasobu";
                                  memcpy(resourceName, nameTmp, strlen(nameTmp) );
                         }
                 
                 //------------------------------------------------------------------
                 //------------------------------------------------------------------
                 
                  
                 response.append( "<item>");        
                 
                 //-------------------------------------------------------------
                 response.append( "<pId xsi:type=\"xsd:int\">");                                                                                                                      
                 response.append( gxh_IntToString(rows[k].id)   ) ;                                                                                                        
                 response.append(  "</pId>");       
                 
                 //-------------------------------------------------------------
                 response.append( "<pNumFunction xsi:type=\"xsd:int\">");                                                                                                                        
                 response.append(gxh_IntToString(rows[k].const_function_symbol)  ) ;                                                                                                         
                 response.append(  "</pNumFunction>");   
                 
                 //-------------------------------------------------------------                  
                 response.append( "<pName xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( functionName ) ;                                                                                                         
                 response.append(  "</pName>");                                                                                                                        
                 
                 //-------------------------------------------------------------
                 response.append( "<pResourceId xsi:type=\"xsd:int\">");                                                                                                                  
                 response.append(gxh_IntToString(rows[k].resource_id)  ) ;                                                                                                      
                 response.append(  "</pResourceId>"); 
                 
                 //-------------------------------------------------------------
                 response.append( "<pResourceName xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( resourceName  ) ;                                                                                                         
                 response.append(  "</pResourceName>");  
                 
                 //-------------------------------------------------------------
                 response.append( "<pFirstStart xsi:type=\"xsd:dateTime\">");                                                                                                         
                 response.append( rows[k].work_time_str  ) ;                                                                                                         
                 response.append(  "</pFirstStart>");  
                 
                 //-------------------------------------------------------------
                 response.append( "<pRepeatWeek xsi:type=\"xsd:string\">");           
                 response.append( gxh_IntToString(rows[k].every_week)  ) ;                                                                                                          
                 response.append(  "</pRepeatWeek>"); 
                 
                 //-------------------------------------------------------------
                 response.append( "<pMonDay xsi:type=\"xsd:string\">");                                                                                                              
                 response.append( gxh_IntToString(rows[k].mon)   ) ;                                                                                                  
                 response.append(  "</pMonDay>");  
                 
                 //-------------------------------------------------------------
                 response.append( "<pTueDay xsi:type=\"xsd:string\">");                                                                                                                         
                 response.append( gxh_IntToString(rows[k].tue)  ) ;                                                                                                        
                 response.append(  "</pTueDay>");  
                 
                 //-------------------------------------------------------------
                 response.append( "<pWedDay xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( gxh_IntToString(rows[k].wed)   ) ;                                                                                         
                 response.append(  "</pWedDay>"); 
                 
                 //-------------------------------------------------------------
                 response.append( "<pThuDay xsi:type=\"xsd:string\">");                                                                                                                 
                 response.append(  gxh_IntToString(rows[k].thu)   ) ;                                                                                         
                 response.append(  "</pThuDay>"); 
                 
                 //-------------------------------------------------------------
                 response.append( "<pFriDay xsi:type=\"xsd:string\">");                                                                                                           
                 response.append( gxh_IntToString(rows[k].fri)  ) ;                                                                                              
                 response.append(  "</pFriDay>");
                 
                 
                 //-------------------------------------------------------------
                 response.append( "<pSatDay xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( gxh_IntToString(rows[k].sat)  ) ;                                                                                         
                 response.append(  "</pSatDay>");
                 
                 //-------------------------------------------------------------
                 response.append( "<pSunDay xsi:type=\"xsd:string\">");                                                                                                         
                 response.append(  gxh_IntToString(rows[k].sun)   ) ;                                                                                          
                 response.append(  "</pSunDay>");
                 
                 //-------------------------------------------------------------
                 response.append( "<pParam1 xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( rows[k].name  ) ;                                                                                                         
                 response.append(  "</pParam1>"); 
                 
                 //-------------------------------------------------------------
                 response.append( "<pParam2 xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( rows[k].text_content  ) ;                                                                                                         
                 response.append(  "</pParam2>"); 
                 
                 
                 
                 response.append( "</item>");                                                                                                        
              }
              
              response.append(  "</result></gxh:getAllTimer></SOAP-ENV:Body></SOAP-ENV:Envelope>");  
              
             return this->build(msgIn, message_ret, response.c_str());
}

//------------------------------------------------------------------------------

bool app_module_wsdl::addTask(gxh_message* msgIn, gxh_message* message_ret, int const_function_symbol, int resource_id, const char* time, int every_week, int mon, int tue, int wed, int thu, int fri, int sat, int sun, const char* param1, const char* param2 )
{
    
      unsigned int size = 0;
      ih_task* rows = (ih_task*)this->getAppHandle()->getResource( RES_VECTOR_TASK , &size); 
    
      for(unsigned int x=0; x<size; x++)
      {
          if(rows[x].free)
          {
              rows[x].free = false;           
              
              rows[x].const_function_symbol  = const_function_symbol;
              rows[x].resource_id  = resource_id;  
              
              memcpy(rows[x].work_time_str, time, strlen(time));
              
              rows[x].every_week  = every_week;
              rows[x].mon  = mon;
              rows[x].tue  = tue;
              rows[x].wed  = wed;
              rows[x].thu  = thu;
              rows[x].fri  = fri;
              rows[x].sat  = sat;
              rows[x].sun  = sun;
              
              memcpy(rows[x].name, param1, strlen(param1));
              memcpy(rows[x].text_content, param2, strlen(param2));
              
              rows[x].status  = 2; //add new..
              
            
              std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:addTimer>";                                                                                                                            
              response.append("<result  xsi:type=\"xsd:string\">OK</result>");                                      
              response.append(  "</gxh:addTimer></SOAP-ENV:Body></SOAP-ENV:Envelope>");                                                                                                                     
              return this->build(msgIn, message_ret, response.c_str());
          }
      }
      
      
              std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:addTimer>";                                                                                                                            
              response.append("<result  xsi:type=\"xsd:string\">NO SPACE</result>");                                      
              response.append(  "</gxh:addTimer></SOAP-ENV:Body></SOAP-ENV:Envelope>");                                                                                                                     
              return this->build(msgIn, message_ret, response.c_str());
}
   

//------------------------------------------------------------------------------

bool app_module_wsdl::addScene(gxh_message* msgIn, gxh_message* message_ret, char* name )
{            
              
             int idScene = -1;
             
             unsigned int size = 0;
             ih_scene* rows = (ih_scene*)this->getAppHandle()->getResource( RES_VECTOR_SCENE , &size);  
             
             for(unsigned int x=1;x<size; x++)
             {
                 if(rows[x].free)
                 {
                     rows[x].free = false;
                     memset(rows[x].name,0, 100);
                     if(strlen(name) < 100) memcpy(rows[x].name, name, strlen(name));
                     rows[x].id = x;
                     idScene = x;
                     rows[x].status = 2; //add to database..
                     break;
                 }
             }
              
              
             
              std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:addSceneResponse soapenv:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">";                                                                                                        
      
              if(idScene >0)
              {
                 response.append("<result  xsi:type=\"xsd:string\">OK</result>");                        
                 response.append("<pId   xsi:type=\"xsd:int\">");                                       
                 response.append( gxh_IntToString(idScene));                              
                 response.append("</pId>");                        
              }else
              {
                 response.append("<result  xsi:type=\"xsd:string\">ERROR</result>");                        
                 response.append("<pId>-1</pId>");    
              }
              
              response.append(  "</gxh:addSceneResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");                                                                                                        
            
              return this->build(msgIn,message_ret, response.c_str()); 
}

//------------------------------------------------------------------------------

 bool app_module_wsdl::getAllScene(gxh_message* msgIn, gxh_message* message_ret )
 { 
        unsigned int size = 0;
        ih_scene* rows = (ih_scene*)this->getAppHandle()->getResource( RES_VECTOR_SCENE , &size);  
        
                                       
              std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:getAllSceneResponse><result>";                                                                                                        
  
              for(unsigned int k=0; k<size;k++) 
              {
                 if(rows[k].free) continue;
                 if(rows[k].status == 3) continue; //rows to delete...
                  
                 response.append( "<item>");        
                 
                 //-------------------------------------------------------------
                 response.append( "<pId xsi:type=\"xsd:int\">");                                                                                                                         
                 response.append( gxh_IntToString(rows[k].id)   ) ;                                                                                                        
                 response.append(  "</pId>");                                                                                                        
                 //-------------------------------------------------------------
                  
                 response.append( "<pName xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( rows[k].name  ) ;                                                                                                         
                 response.append(  "</pName>");                                                                                                        
                 //-------------------------------------------------------------                                             
                 
                 
                 response.append( "</item>");                                                                                                        
              }              
              response.append(  "</result></gxh:getAllSceneResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");
                     
              return this->build(msgIn,message_ret, response.c_str());
 } 

//------------------------------------------------------------------------------

bool app_module_wsdl::exeDriverFunction(gxh_message* msgIn, gxh_message* message_ret, int idDriver, int numFunctionModule, char* param )
{       
    
         this->getAppHandle()->show_log_driver( "app_module_wsdl","exeDriverFunction","wsdl",3 );
    
         gxh_message msgRet;
         bool ret = this->executeFunctionDriver(msgIn,&msgRet,idDriver,numFunctionModule, std::string(param) );
        
         if(!ret)
         {
              std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:executeSimplyFunctionResponse><result  xsi:type=\"xsd:string\">NO RESPONSE FROM MODULE</result></gxh:executeSimplyFunctionResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>";
              return this->build(msgIn,message_ret, response.c_str());
         }
         
            
         std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:executeDriverFunctionResponse><result  xsi:type=\"xsd:string\">";                                                                                                        
         response.append( msgRet.dataStruct );
         response.append( "</result></gxh:executeDriverFunctionResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>" );                                                                                                        
         
                   
         return this->build(msgIn, message_ret, response.c_str());

}

//------------------------------------------------------------------------------

bool app_module_wsdl::exeParamFunction(gxh_message* msgIn, gxh_message* message_ret, int numFunction, int numResource, char* param1, char* param2, int px_group_id)
{      
  
         this->getAppHandle()->show_log_core( "app_module_wsdl","exeParamFunction","wsdl",3 );
    
         gxh_message msgRet;        
         bool ret = this->executeFunctionParam(msgIn, &msgRet,numFunction,numResource, std::string(param1), std::string(param2), px_group_id, NULL );                                                            
     
         
         if(!ret)
         {
              std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:executeSimplyFunctionResponse><result  xsi:type=\"xsd:string\">NO RESPONSE FROM MODULE</result></gxh:executeSimplyFunctionResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>";                                                                                                        
               
              return this->build(msgIn, message_ret, response.c_str());
         }
         
    
          
         std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:executeParamFunctionResponse><result  xsi:type=\"xsd:string\">";                                                                                                        
         response.append( msgRet.dataStruct );
         response.append( "</result></gxh:executeParamFunctionResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>" );                                                                                                        
         
  
         return this->build(msgIn, message_ret, response.c_str());
 
}

//------------------------------------------------------------------------------

bool app_module_wsdl::exeSimplyFunction(gxh_message* msgIn,gxh_message* message_ret, int numFunction, int numResource, int px_group_id)
{      
 
        this->getAppHandle()->show_log_module( "app_module_wsdl","exeSimplyFunction","wsdl",3);
    
        gxh_message msgRet;
        bool ret = this->executeFunction(msgIn, &msgRet,numFunction, numResource,px_group_id);
        
        if(!ret)
        {                     
              std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:executeSimplyFunctionResponse><result  xsi:type=\"xsd:string\">NO RESPONSE FROM MODULE</result></gxh:executeSimplyFunctionResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>";                                                                                                                          
              return this->build(msgIn, message_ret, response.c_str());
            
        }         
             
        std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:executeSimplyFunctionResponse><result  xsi:type=\"xsd:string\">";                                                                                                        
        response.append( msgRet.dataStruct );
        response.append( "</result></gxh:executeSimplyFunctionResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>" );                                                                                                                     
        return this->build(msgIn, message_ret, response.c_str());
 
}

//------------------------------------------------------------------------------

bool app_module_wsdl::getAllDriverVar(gxh_message* msgIn,gxh_message* message_ret, int idDriver )
{
      unsigned int size = 0;
      ih_driver_values* rows = (ih_driver_values* )this->getAppHandle()->getResource(RES_VECTOR_DRIVER_VALUE, &size);
      
      std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:getAllDriverVarsResponse><result>";                                                                                                        

      
        for(unsigned int k=0; k<size;k++) 
              {
                 if(rows[k].ih_driver_id != idDriver) continue;
            
                 response.append( "<item>");        
                 
                 //-------------------------------------------------------------
                 response.append( "<pId xsi:type=\"xsd:int\">");                                                                                                                      
                 response.append(  gxh_IntToString(rows[k].id)  ) ;                                                                                                     
                 response.append(  "</pId>");                                                                                                        
 
                 //-------------------------------------------------------------
                 response.append( "<pNumVar xsi:type=\"xsd:int\">");           
                 response.append( gxh_IntToString(rows[k].const_function)   ) ;                                                                                                        
                 response.append(  "</pNumVar>");  
                 
                 //-------------------------------------------------------------
                                    
                 response.append( "<pName xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( rows[k].name_param  ) ;                                                                                                         
                 response.append(  "</pName>");                                                                                                        
                 //-------------------------------------------------------------
                                    
                 response.append( "<pValue xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( rows[k].value  ) ;                                                                                                         
                 response.append(  "</pValue>");                                                                                                        
                 //-------------------------------------------------------------
               
                 response.append( "</item>");                                                                                                        
              }
      
      
       
      response.append(  "</result></gxh:getAllDriverVarsResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");                                                
      return this->build(msgIn, message_ret, response.c_str());
}

//------------------------------------------------------------------------------

bool app_module_wsdl::getAllDriverApi(gxh_message* msgIn,gxh_message* message_ret, int idDriver )
{
     unsigned size = 0;
     ih_sl_driver_api* first = (ih_sl_driver_api*)this->getAppHandle()->getResource(RES_VECTOR_DRIVER_API, &size);
     
     
     
     unsigned sizeVal = 0;
     ih_driver_values* firstVal =  (ih_driver_values*)this->getAppHandle()->getResource(RES_VECTOR_DRIVER_VALUE, &sizeVal);
     
    // std::cout<<"\n\n\nAPI size: "<<size<<"\n";
     
     std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:getAllDriverApiResponse><result>";
     
      for(unsigned int k=0; k<size ;k++) 
              {
              //  std::cout<<"\nId: "<<first[k].ih_driver_id<<", ID2: "<<idDriver;
                if(first[k].ih_driver_id != idDriver ) continue;
          
                 response.append( "<item>");        
                 
                 //-------------------------------------------------------------
                 response.append( "<pId xsi:type=\"xsd:int\">");                                                                                                                       
                 response.append( gxh_IntToString(first[k].id)  ) ;                                                                                                         
                 response.append(  "</pId>");                                                                                                        
                 
                 //-------------------------------------------------------------
                 response.append( "<pControlType xsi:type=\"xsd:string\">");                                                                                                                       
                 response.append( first[k].control_type  ) ;                                                                                                         
                 response.append(  "</pControlType>");   
                 
                 //-------------------------------------------------------------
                 char tmpVal[1000];
                 for(unsigned int y=0; y<sizeVal; y++) 
                 { 
                     if( firstVal[y].id ==  first[k].ih_driver_values_id )
                     {
                        memcpy(tmpVal,firstVal[y].value, strlen(firstVal[y].value) );  
                        y = sizeVal+1; //break;
                     }
                 }
                 
              
                 
                 response.append( "<pVal xsi:type=\"xsd:string\">");                                                                                                                       
                 response.append(  tmpVal ) ;                                                                                                         
                 response.append(  "</pVal>");  
                 
                 //-------------------------------------------------------------
                 response.append( "<pLabelName xsi:type=\"xsd:string\">");                                                                                                                       
                 response.append( first[k].label_name ) ;                                                                                                         
                 response.append(  "</pLabelName>"); 
                 
                 //-------------------------------------------------------------
                 response.append( "<pButtonName xsi:type=\"xsd:string\">");                                                                                                                       
                 response.append(first[k].button_name ) ;                                                                                                         
                 response.append(  "</pButtonName>"); 
                 
                 //-------------------------------------------------------------                 
                 response.append( "<pValueType xsi:type=\"xsd:string\">");                                                                                                                       
                 response.append( first[k].value_type ) ;                                                                                                         
                 response.append(  "</pValueType>"); 
                 
                 //-------------------------------------------------------------
                 response.append( "<pFuncNum xsi:type=\"xsd:string\">");                                                                                                                       
                 response.append(  gxh_IntToString(first[k].func_number) ) ;                                                                                                         
                 response.append(  "</pFuncNum>"); 
                 
                 //-------------------------------------------------------------
                 response.append( "<pFuncInterNum xsi:type=\"xsd:string\">");                                                                                                                       
                 response.append(  gxh_IntToString(first[k].func_internal_number) ) ;                                                                                                         
                 response.append(  "</pFuncInterNum>"); 
                 
                 //-------------------------------------------------------------
                 response.append( "<pSelVal xsi:type=\"xsd:string\">");                                                                                                                       
                 response.append(first[k].select_val ) ;                                                                                                         
                 response.append(  "</pSelVal>");
                 
                 //-------------------------------------------------------------
                 response.append( "<pRepVal xsi:type=\"xsd:string\">");                                                                                                                       
                 response.append( first[k].replace_val ) ;                                                                                                         
                 response.append(  "</pRepVal>");
                 
                 //-------------------------------------------------------------
                 response.append( "<pPrStandard xsi:type=\"xsd:string\">");                                                                                                                       
                 response.append(  gxh_IntToString(first[k].pr_standard) ) ;                                                                                                     
                 response.append(  "</pPrStandard>");
                 
                 //-------------------------------------------------------------
                 response.append( "<pPrStandardPlus xsi:type=\"xsd:string\">");                                                                                                                       
                 response.append( gxh_IntToString(first[k].pr_standard_plus) ) ;                                                                                                     
                 response.append(  "</pPrStandardPlus>");
                 
                 //-------------------------------------------------------------
                 response.append( "<pPrAdministrator xsi:type=\"xsd:string\">");                                                                                                                       
                 response.append( gxh_IntToString(first[k].pr_administrator) ) ;                                                                                                     
                 response.append(  "</pPrAdministrator>");
                 
                 //-------------------------------------------------------------
                 response.append( "<pPrSystem xsi:type=\"xsd:string\">");                                                                                                                       
                 response.append( gxh_IntToString(first[k].pr_system) ) ;                                                                                                     
                 response.append(  "</pPrSystem>");
                 
                 //-------------------------------------------------------------
                 response.append( "<pNumDriverVar xsi:type=\"xsd:string\">");                                                                                                                       
                 response.append( gxh_IntToString(first[k].ih_driver_values_id) ) ;                                                                                                     
                 response.append(  "</pNumDriverVar>");
                 
                
                 response.append( "</item>");                                                                                                        
              }                                                                                                                                                           
             
       
     
     
     
     response.append(  "</result><images>"); 
  
     
     
     response.append(  "</images></gxh:getAllDriverApiResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");  
     return this->build(msgIn,message_ret, response.c_str());     
}
 
//------------------------------------------------------------------------------

 bool app_module_wsdl::getAllDriver(int px_user_id, int px_group_id, gxh_message* msgIn, gxh_message* message_ret)
 {        

     unsigned int index = -1;
     
     for(unsigned int x=0; x<MAX_USER_BUFFER; x++)
     {
         if(this->usersSSID[x].px_group_id == px_group_id)
         {
             index = x;
             break;
         }
     }
     
     if(index < 0) return false;
     
     std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:getAllDriverResponse><result>";  
   
     unsigned size = 0;
     ih_driver* first = (ih_driver*)this->getAppHandle()->getResource(RES_VECTOR_DRIVER, &size);
     
     
     //do jakich urzadzen ma dostep ten urzytkownik..
     for(unsigned int x=0; x< this->usersSSID[index].driver_count;x++ )
     {
         unsigned int indexD = -1;
         for(unsigned int t=0; t<size; t++)
         {
             if(first[t].id == this->usersSSID[index].drivers[x].ih_driver_id )
             {
                 indexD = t;
                 t = size+1; //break..
             }
         }
         
         if(indexD < 0) continue; //driver not found..
         
        
         
                 response.append( "<item>");        
                 
                 //-------------------------------------------------------------
                 response.append( "<pId xsi:type=\"xsd:int\">");                                                                                                                         
                 response.append( gxh_IntToString(first[indexD].id)   ) ;              
                 response.append(  "</pId>");                                                                                                        
                 //-------------------------------------------------------------
                  
                 response.append( "<pName xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( first[indexD].name  ) ;                                                                                                         
                 response.append(  "</pName>");                                                                                                        
                 //-------------------------------------------------------------              
                  
                 std::string cPath = "";
                 cPath.append( this->getAppHandle()->getRunPath());
                 cPath.append("../../");
                 cPath.append(first[indexD].screen);
                 
              //   std::cout<<"\n\n"<<first[indexD].screen;
                                  
                 this->getAppHandle()->show_log_module("app_module_wsdl","getAllDriver", cPath.c_str(),4  );
                 

                 std::ifstream file (cPath.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
                 if (file.is_open())
                   {          
                       int size;
                       char * memblock;
                     
                        size = file.tellg();
                        memblock = new char [size];
                        file.seekg (0, std::ios::beg);
                        file.read (memblock, size);
                        file.close();   
                        
                
                        int newLength = 0;
                        char* base64 = gxh_base64_encode((const unsigned char*)memblock, size, &newLength);
                    
                        response.append( "<pImage xsi:type=\"xsd:string\">");                                                                                                         
                        response.append( base64 ) ;                                                                                                         
                        response.append(  "</pImage>");                                                                                                        
                 
                        delete[] memblock;
                        delete[] base64;
                        
                    }else
                    {
                        response.append( "<pImage xsi:type=\"xsd:string\">");                                                                                                         
                        response.append( "NONE" ) ;                                                                                                         
                        response.append(  "</pImage>"); 
                    }
                 
             
                 
                 response.append( "</item>");               
     }
    
     
     
     response.append(  "</result></gxh:getAllDriverResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");                       
     return this->build(msgIn,message_ret, response.c_str());
 }  

//------------------------------------------------------------------------------

 bool app_module_wsdl::getAllTemp(gxh_message* msgIn, gxh_message* message_ret, int idTop ,int idTopSector  )
 {
     unsigned int countS = 0;
     unsigned int countZ = 0;
     unsigned int countSn = 0;
  
     ih_sector* sectors    = (ih_sector*)this->getAppHandle()->getResource(RES_VECTOR_SECTOR, &countS);
     ih_zone* zones        = (ih_zone*)this->getAppHandle()->getResource(RES_VECTOR_ZONE, &countZ); 
     ih_driver* drivers    = (ih_driver*)this->getAppHandle()->getResource(RES_VECTOR_DRIVER, &countSn); 
     
     
     std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:getAllTempResponse><result>";                                                                                                        
  
    
     for(unsigned int s=0; s< countS; s++)         
     {                 
         if(idTopSector >0 && sectors[s].id != idTopSector) continue;
         
       //   std::cout<<"\nSektor: "<<sectors[s].name.c_str();
         
         for(unsigned int z=0; z<countZ; z++)
         {                          
              if( zones[z].ih_sector_id != sectors[s].id ) continue;
              if(idTop >0 && zones[z].id != idTop )       continue;                       
              
              for(unsigned int k=0; k<countSn;k++) 
              {                  
                  
                 if(drivers[k].ih_zone_id != zones[z].id)continue;
                 if(drivers[k].driver_symbol != 't' )continue;
                 
                  
                 response.append( "<item>");        
                 
                 //-------------------------------------------------------------
                 response.append( "<pId xsi:type=\"xsd:int\">");                                                                                                                    
                 response.append( gxh_IntToString(drivers[k].id)   ) ;               
                 response.append(  "</pId>");                                                                                                        
                 //-------------------------------------------------------------
                  
                 response.append( "<pName xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( drivers[k].name  ) ;                                                                                                         
                 response.append(  "</pName>");                                                                                                        
                 //-------------------------------------------------------------
                   
                 response.append( "<pValue xsi:type=\"xsd:string\">");                                                                                                                         
                 response.append( drivers[k].value ) ;                                                                                                         
                 response.append(  "</pValue>");                                                                                                        
                                                                                                                                        
                 //-------------------------------------------------------------              
                                   
                 response.append( "<pNumZoneTop xsi:type=\"xsd:int\">");            
                 response.append( gxh_IntToString(drivers[k].ih_zone_id) ) ;                                                                                                         
                 response.append(  "</pNumZoneTop>");                                                                                                              
                 //-------------------------------------------------------------
                
                 
                   response.append( "<pZoneName xsi:type=\"xsd:string\">");                                                                                                         
                   response.append( zones[z].name  ) ;                                                                                                         
                   response.append(  "</pZoneName>");  
                  
                 //-------------------------------------------------------------
                 
                 
                 response.append( "</item>");                                                                                                        
              }
         }//end zones
     }//end sectors
              
              response.append(  "</result></gxh:getAllTempResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");                                                                                                        
                           
             
             
             return this->build(msgIn, message_ret, response.c_str());
 }

//------------------------------------------------------------------------------

 bool app_module_wsdl::getAllSensor(int px_user_id, int px_group_id ,gxh_message* msgIn,gxh_message* message_ret, int idTop ,  int idTopSector)
 {
       
     unsigned int countS = 0;
     unsigned int countZ = 0;
     unsigned int countSn = 0;
  
     ih_sector* sectors    = (ih_sector*)this->getAppHandle()->getResource(RES_VECTOR_SECTOR, &countS);
     ih_zone* zones        = (ih_zone*)this->getAppHandle()->getResource(RES_VECTOR_ZONE, &countZ); 
     ih_sensor* sensors    = (ih_sensor*)this->getAppHandle()->getResource(RES_VECTOR_SENSOR, &countSn); 
     
     
  //uprawnienia do sektorów..............................
     ih_sector sectors_tmp[100];
     ih_zone zones_tmp[300];
     unsigned int countSTmp = 0;
     unsigned int countZTmp = 0;
     
     for(unsigned int s=0; s<countS; s++)
     {
         bool cS = this->alowSector( sectors[s].id, px_group_id );
         if(cS){
             memcpy(&sectors_tmp[countSTmp], &sectors[s], sizeof(ih_sector) );
             countSTmp++;
         }
     }
     
     for(unsigned int s=0; s<countZ; s++)
     {
         bool cS = this->alowZone( zones[s].id, px_group_id );
         if(cS){
             memcpy(&zones_tmp[countZTmp], &zones[s], sizeof(ih_zone) );
             countZTmp++;
         }
     }
     
     //niech dalsza częśc kodu korzystan z innych tabel..
     sectors = sectors_tmp;
     zones   = zones_tmp;
     countS  = countSTmp;
     countZ  = countZTmp;
     
   
     //-----------------------------------------------------
     
     
      std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:getAllSensorResponse><result>";                                                                                                        
  
     for(unsigned int s=0; s< countS; s++)         
     {                 
         if(idTopSector >0 && sectors[s].id != idTopSector) continue;
         
       //   std::cout<<"\nSektor: "<<sectors[s].name.c_str();
         
         for(unsigned int z=0; z<countZ; z++)
         {                                               
              if( zones[z].ih_sector_id != sectors[s].id ) continue;
              if(idTop >0 && zones[z].id != idTop )       continue;
              
                           
           
              for(unsigned int k=0; k<countSn;k++) 
              {
                     
                  
                 if(sensors[k].ih_zone_id != zones[z].id )   continue;
                  
                 response.append( "<item>");        
                 
                 //-------------------------------------------------------------
                 response.append( "<pId xsi:type=\"xsd:int\">");                                                                                                                   
                 response.append( gxh_IntToString(sensors[k].id)   ) ;                                                                                                  
                 response.append(  "</pId>");                                                                                                        
                 //-------------------------------------------------------------
                  
                 response.append( "<pName xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( sensors[k].name  ) ;                                                                                                         
                 response.append(  "</pName>");                                                                                                        
                 //-------------------------------------------------------------
                   
                 response.append( "<pStatus xsi:type=\"xsd:string\">");                                                                                                           
                 response.append( gxh_IntToString(  sensors[k].status ) ) ;                                                                                                         
                 response.append(  "</pStatus>");                                                                                                             
                 //-------------------------------------------------------------
                                    
                 response.append( "<pStatusTime xsi:type=\"xsd:datetime\">");                                                                                                         
                 response.append( sensors[k].status_time_str  ) ;                                                                                                         
                 response.append(  "</pStatusTime>");                                                                                                        
                 //-------------------------------------------------------------              
                                   
                 response.append( "<pNumZoneTop xsi:type=\"xsd:int\">");           
                 response.append( gxh_IntToString(sensors[k].ih_zone_id) ) ;                                                                                                         
                 response.append(  "</pNumZoneTop>");                                                                                                                        
                 //-------------------------------------------------------------
                 
                 response.append( "<pSymbol xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( sensors[k].symbol  ) ;                                                                                                         
                 response.append(  "</pSymbol>");                                                                                                        
                 //-------------------------------------------------------------   
               
                
                 response.append( "<pZoneName xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( zones[z].name  ) ;                                                                                                         
                 response.append(  "</pZoneName>");  
                 
                 
                 //-------------------------------------------------------------   
               
                 response.append( "<pAnalog xsi:type=\"xsd:int\">");                                                                                                         
                 response.append( gxh_IntToString(sensors[k].analogowy) ) ;                                                                                                          
                 response.append(  "</pAnalog>"); 
                 
                 //-------------------------------------------------------------   
               
                 response.append( "<pValue xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( sensors[k].wartosc  ) ;                                                                                                         
                 response.append(  "</pValue>"); 
                 //-------------------------------------------------------------   
          
                                   
                 response.append( "<pNumSectorTop xsi:type=\"xsd:int\">");                
                 response.append(  gxh_IntToString( sectors[s].id) ) ;                                                                                                         
                 response.append(  "</pNumSectorTop>");                                                                                                                       
                 //-------------------------------------------------------------                 
                
                 response.append( "<pSectorName xsi:type=\"xsd:string\">");                                                                                                         
                 response.append(  sectors[s].name  ) ; 
                 response.append(  "</pSectorName>");  
                 
                 //-------------------------------------------------------------   
                 
                 
                 response.append( "</item>");                                                                                                        
              }
         }
     }
              response.append(  "</result></gxh:getAllSensorResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");                                                                                                        
 
                               
              return this->build(msgIn, message_ret, response.c_str());
 
 } 

//------------------------------------------------------------------------------

bool app_module_wsdl::getAllDoor(int px_user_id, int px_group_id, gxh_message* msgIn, gxh_message* message_ret, int idTop, int idTopSector  )
{     
     unsigned int countS = 0;
     unsigned int countZ = 0;
     unsigned int countG = 0;
  
     ih_sector* sectors    = (ih_sector*)this->getAppHandle()->getResource(RES_VECTOR_SECTOR, &countS);
     ih_zone* zones        = (ih_zone*)this->getAppHandle()->getResource(RES_VECTOR_ZONE, &countZ); 
     ih_door* doors        = (ih_door*)this->getAppHandle()->getResource(RES_VECTOR_DOOR, &countG); 
     
     
       //uprawnienia do sektorów..............................
     ih_sector sectors_tmp[100];
     ih_zone zones_tmp[300];
     unsigned int countSTmp = 0;
     unsigned int countZTmp = 0;
     
     for(unsigned int s=0; s<countS; s++)
     {
         bool cS = this->alowSector( sectors[s].id, px_group_id );
         if(cS){
             memcpy(&sectors_tmp[countSTmp], &sectors[s], sizeof(ih_sector) );
             countSTmp++;
         }
     }
     
     for(unsigned int s=0; s<countZ; s++)
     {
         bool cS = this->alowZone( zones[s].id, px_group_id );
         if(cS){
             memcpy(&zones_tmp[countZTmp], &zones[s], sizeof(ih_zone) );
             countZTmp++;
         }
     }
     
     //niech dalsza częśc kodu korzystan z innych tabel..
     sectors = sectors_tmp;
     zones   = zones_tmp;
     countS  = countSTmp;
     countZ  = countZTmp;
     
   
     //-----------------------------------------------------
     
 
     
    std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:getAllDoorResponse><result>";                                                                                                        
  
     for(unsigned int s=0; s< countS; s++)         
     {            
        
         
         if(idTopSector >0 && sectors[s].id != idTopSector) continue;
         
         for(unsigned int z=0; z<countZ; z++)
         {                                
              
             
              if( zones[z].ih_sector_id != sectors[s].id ) continue;                                             
              if(idTop >0 && zones[z].id != idTop )       continue;            
              
              for(unsigned int k=0; k<countG;k++) 
              {
                 
                  
                 if(doors[k].ih_zone_id != zones[z].id) continue;
                  
                 response.append( "<item>");        
                 
                 //-------------------------------------------------------------
                 response.append( "<pId xsi:type=\"xsd:int\">");                                                                                                              
                 response.append( gxh_IntToString(doors[k].id)   ) ;                                                                                              
                 response.append(  "</pId>");                                                                                                        
                 //-------------------------------------------------------------
                  
                 response.append( "<pName xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( doors[k].name  ) ;                                                                                                         
                 response.append(  "</pName>");                                                                                                        
                                                                                                                                                                                                                                                   
                 //-------------------------------------------------------------              
               
                 response.append( "<pStatus xsi:type=\"xsd:int\">");             
                 response.append( gxh_IntToString(doors[k].status) ) ;                                                                                                         
                 response.append(  "</pStatus>"); 
                 
                 //-------------------------------------------------------------              
                                   
                 response.append( "<pNumZoneTop xsi:type=\"xsd:int\">");             
                 response.append( gxh_IntToString(doors[k].ih_zone_id) ) ;                                                                                                         
                 response.append(  "</pNumZoneTop>");                                                                                                         
                 //-------------------------------------------------------------
                    
                 response.append( "<pZoneName xsi:type=\"xsd:string\">");                                                                                                         
                 response.append(  zones[z].name ) ;                                                                                                         
                 response.append(  "</pZoneName>"); 
               
                 
                 //-------------------------------------------------------------                 
                 
                 response.append( "<pType xsi:type=\"xsd:int\">");             
                 response.append( gxh_IntToString(doors[k].type) ) ;                                                                                                         
                 response.append(  "</pType>");     
                 
                  //-------------------------------------------------------------              
                                   
                 response.append( "<pNumSectorTop xsi:type=\"xsd:int\">");                
                 response.append(  gxh_IntToString( sectors[s].id) ) ;                                                                                                         
                 response.append(  "</pNumSectorTop>");                                                                                                                       
                 //-------------------------------------------------------------                 
                
                 response.append( "<pSectorName xsi:type=\"xsd:string\">");                                                                                                         
                 response.append(  sectors[s].name  ) ; 
                 response.append(  "</pSectorName>");  
                 
                 //-------------------------------------------------------------                 
                 
                 response.append( "</item>");                                                                                                        
              }
         }//end zone
     }//end sectors
              response.append(  "</result></gxh:getAllDoorResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");                                                                                                        
  
              
              
              
             return this->build(msgIn, message_ret, response.c_str());

 }

 
 
//------------------------------------------------------------------------------

bool app_module_wsdl::getAllGate(int px_user_id,int px_group_id ,gxh_message* msgIn, gxh_message* message_ret, int idTop, int idTopSector  )
{     
     unsigned int countS = 0;
     unsigned int countZ = 0;
     unsigned int countG = 0;
  
     ih_sector* sectors    = (ih_sector*)this->getAppHandle()->getResource(RES_VECTOR_SECTOR, &countS);
     ih_zone* zones        = (ih_zone*)this->getAppHandle()->getResource(RES_VECTOR_ZONE, &countZ); 
     ih_gate* gates        = (ih_gate*)this->getAppHandle()->getResource(RES_VECTOR_GATE, &countG); 
     
     
     //uprawnienia do sektorów..............................
     ih_sector sectors_tmp[100];
     ih_zone zones_tmp[300];
     unsigned int countSTmp = 0;
     unsigned int countZTmp = 0;
     
     for(unsigned int s=0; s<countS; s++)
     {
         bool cS = this->alowSector( sectors[s].id, px_group_id );
         if(cS){
             memcpy(&sectors_tmp[countSTmp], &sectors[s], sizeof(ih_sector) );
             countSTmp++;
         }
     }
     
     for(unsigned int s=0; s<countZ; s++)
     {
         bool cS = this->alowZone( zones[s].id, px_group_id );
         if(cS){
             memcpy(&zones_tmp[countZTmp], &zones[s], sizeof(ih_zone) );
             countZTmp++;
         }
     }
     
     //niech dalsza częśc kodu korzystan z innych tabel..
     sectors = sectors_tmp;
     zones   = zones_tmp;
     countS  = countSTmp;
     countZ  = countZTmp;
     
   
     //-----------------------------------------------------
     
    std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:getAllGateResponse><result>";                                                                                                        
  
     for(unsigned int s=0; s< countS; s++)         
     {                 
         if(idTopSector >0 && sectors[s].id != idTopSector) continue;
         
         for(unsigned int z=0; z<countZ; z++)
         {                          
              if( zones[z].ih_sector_id != sectors[s].id ) continue;
              if(idTop >0 && zones[z].id != idTop )       continue;
           
              
              for(unsigned int k=0; k<countG;k++) 
              {
                 if(gates[k].from_ih_zone_id != zones[z].id) continue;
                  
                 response.append( "<item>");        
                 
                 //-------------------------------------------------------------
                 response.append( "<pId xsi:type=\"xsd:int\">");                                                                                                              
                 response.append( gxh_IntToString(gates[k].id)   ) ;                                                                                              
                 response.append(  "</pId>");                                                                                                        
                 //-------------------------------------------------------------
                  
                 response.append( "<pName xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( gates[k].name  ) ;                                                                                                         
                 response.append(  "</pName>");                                                                                                        
                 //-------------------------------------------------------------
                   
                 response.append( "<pStatus xsi:type=\"xsd:int\">");                                                                                                             
                 response.append(gxh_IntToString(  gates[k].open_percent ) ) ;                                                                                                         
                 response.append(  "</pStatus>");                                                                                                                  
                 //-------------------------------------------------------------
                                    
                 response.append( "<pStatusTime xsi:type=\"xsd:datetime\">");                                                                                                         
                 response.append( gates[k].status_time_str  ) ;                                                                                                         
                 response.append(  "</pStatusTime>");                                                                                                        
                 //-------------------------------------------------------------              
                                   
                 response.append( "<pNumZoneTop xsi:type=\"xsd:int\">");             
                 response.append( gxh_IntToString(gates[k].from_ih_zone_id) ) ;                                                                                                         
                 response.append(  "</pNumZoneTop>");                                                                                                         
                 //-------------------------------------------------------------
                    
                 response.append( "<pZoneName xsi:type=\"xsd:string\">");                                                                                                         
                 response.append(  zones[z].name ) ;                                                                                                         
                 response.append(  "</pZoneName>"); 
               
                 
                 //-------------------------------------------------------------                 
                 
                 response.append( "<pType xsi:type=\"xsd:int\">");             
                 response.append( gxh_IntToString(gates[k].typ) ) ;                                                                                                         
                 response.append(  "</pType>");     
                 
                  //-------------------------------------------------------------              
                                   
                 response.append( "<pNumSectorTop xsi:type=\"xsd:int\">");                
                 response.append(  gxh_IntToString( sectors[s].id) ) ;                                                                                                         
                 response.append(  "</pNumSectorTop>");                                                                                                                       
                 //-------------------------------------------------------------                 
                
                 response.append( "<pSectorName xsi:type=\"xsd:string\">");                                                                                                         
                 response.append(  sectors[s].name  ) ; 
                 response.append(  "</pSectorName>");  
                 
                 //-------------------------------------------------------------                 
                 
                 response.append( "</item>");                                                                                                        
              }
         }//end zone
     }//end sectors
              response.append(  "</result></gxh:getAllGateResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");                                                                                                        
  
              
              
              
             return this->build(msgIn, message_ret, response.c_str());

 }


//------------------------------------------------------------------------------

 bool app_module_wsdl::getAllBlind(int px_user_id,int px_group_id, gxh_message* msgIn, gxh_message* message_ret, int idTop, int idTopSector )
 {
     unsigned int countS = 0;
     unsigned int countZ = 0;
     unsigned int countB = 0;
  
     ih_sector* sectors    = (ih_sector*)this->getAppHandle()->getResource(RES_VECTOR_SECTOR, &countS);
     ih_zone* zones        = (ih_zone*)this->getAppHandle()->getResource(RES_VECTOR_ZONE, &countZ); 
     ih_blind* blinds      = (ih_blind*)this->getAppHandle()->getResource(RES_VECTOR_BLIND, &countB); 
     
     
     //uprawnienia do sektorów..............................
     ih_sector sectors_tmp[100];
     ih_zone zones_tmp[300];
     unsigned int countSTmp = 0;
     unsigned int countZTmp = 0;
     
     for(unsigned int s=0; s<countS; s++)
     {
         bool cS = this->alowSector( sectors[s].id, px_group_id );
         if(cS){
             memcpy(&sectors_tmp[countSTmp], &sectors[s], sizeof(ih_sector) );
             countSTmp++;
         }
     }
     
     for(unsigned int s=0; s<countZ; s++)
     {
         bool cS = this->alowZone( zones[s].id, px_group_id );
         if(cS){
             memcpy(&zones_tmp[countZTmp], &zones[s], sizeof(ih_zone) );
             countZTmp++;
         }
     }
     
     //niech dalsza częśc kodu korzystan z innych tabel..
     sectors = sectors_tmp;
     zones   = zones_tmp;
     countS  = countSTmp;
     countZ  = countZTmp;
     
   
     //-----------------------------------------------------
     
     
     std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:getAllBlindResponse><result>";                                                                                                        
     
  
     for(unsigned int s=0; s< countS; s++)         
     {                 
         if(idTopSector >0 && sectors[s].id != idTopSector) continue;
         
        
         for(unsigned int z=0; z<countZ; z++)
         {                          
              if( zones[z].ih_sector_id != sectors[s].id ) continue;
              if(idTop >0 && zones[z].id != idTop )       continue;
         
              
              for(unsigned int k=0; k<countB;k++) 
              {
                 if(blinds[k].ih_zone_id != zones[z].id  )  continue;
                  
                 response.append( "<item xsi:type=\"xsd:struct\">");        
                 
                 //-------------------------------------------------------------
                 response.append( "<pId xsi:type=\"xsd:int\">");                                                                                                                   
                 response.append( gxh_IntToString(blinds[k].id)  ) ;                                                                                           
                 response.append(  "</pId>");                                                                                                        
                 //-------------------------------------------------------------
                  
                 response.append( "<pName xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( blinds[k].name  ) ;                                                                                                         
                 response.append(  "</pName>");                                                                                                        
                 //-------------------------------------------------------------
                   
                 response.append( "<pStatus xsi:type=\"xsd:int\">");                                                                                                                    
                 response.append( gxh_IntToString(  blinds[k].open_percent ) ) ;                                                                                                         
                 response.append(  "</pStatus>");                                                                                                                         
                 //-------------------------------------------------------------
                                    
                 response.append( "<pStatusTime xsi:type=\"xsd:datetime\">");                                                                                                         
                 response.append( blinds[k].status_time_str  ) ;                                                                                                         
                 response.append(  "</pStatusTime>");                                                                                                        
                 //-------------------------------------------------------------              
                                   
                 response.append( "<pNumZoneTop xsi:type=\"xsd:int\">");                
                 response.append(  gxh_IntToString(blinds[k].ih_zone_id) ) ;                                                                                                         
                 response.append(  "</pNumZoneTop>");                                                                                                                       
                 //-------------------------------------------------------------                 
                
                 response.append( "<pZoneName xsi:type=\"xsd:string\">");                                                                                                         
                 response.append(  zones[z].name  ) ; 
                 response.append(  "</pZoneName>");  
                 
                 //-------------------------------------------------------------              
                                   
                 response.append( "<pNumSectorTop xsi:type=\"xsd:int\">");                
                 response.append(  gxh_IntToString( sectors[s].id) ) ;                                                                                                         
                 response.append(  "</pNumSectorTop>");                                                                                                                       
                 //-------------------------------------------------------------                 
                
                 response.append( "<pSectorName xsi:type=\"xsd:string\">");                                                                                                         
                 response.append(  sectors[s].name  ) ; 
                 response.append(  "</pSectorName>");  
                 
                 
                 //-------------------------------------------------------------
                 
                 
                 response.append( "</item>");                                                                                                        
              }
         }//end zones
     }//end sectors
              
    response.append(  "</result></gxh:getAllBlindResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");                                                                                                        
  
            
     return this->build(msgIn,message_ret, response.c_str());

 } 

//------------------------------------------------------------------------------

 
 
bool app_module_wsdl::getAllAlarmZone(int px_user_id, int px_group_id ,gxh_message* msgIn, gxh_message* message_ret )
{
 
     unsigned int countZ = 0; 
     unsigned int countS = 0; 
     ih_alarm_zone* zones        = (ih_alarm_zone*)this->getAppHandle()->getResource(RES_VECTOR_ALARM_ZONE, &countZ); 
     ih_alarm_device* devices        = (ih_alarm_device*)this->getAppHandle()->getResource(RES_VECTOR_ALARM_DEVICE, &countS); 
  
           
     
 
     //-----------------------------------------------------
     
    
     std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:getAllAlarmZoneResponse><result>";                                                                                                        
  
                   
 
 
              for(unsigned int k=0; k<countZ;k++) 
              {                           
                  
                 
                 response.append( "<item>");        
                 
                 //-------------------------------------------------------------
                 response.append( "<pId xsi:type=\"xsd:int\">");                                                                                                                       
                 response.append(  gxh_IntToString(zones[k].id)  ) ;                
                 response.append(  "</pId>");                                                                                                        
                 //-------------------------------------------------------------
                  
                 response.append( "<pName xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( zones[k].name  ) ;                                                                                                         
                 response.append(  "</pName>");                                                                                                        
                 //-------------------------------------------------------------        
 
                 response.append( "<pActive xsi:type=\"xsd:int\">");                                                                                                                       
                 response.append(  gxh_IntToString(zones[k].active)  ) ;                
                 response.append(  "</pActive>");   
                 
                 //-------------------------------------------------------------
                 
                 response.append( "<pOnType xsi:type=\"xsd:int\">");                                                                                                                       
                 response.append(  gxh_IntToString(zones[k].on_type)  ) ;                
                 response.append(  "</pOnType>"); 
                 
                 //-------------------------------------------------------------
                 std::string time = std::string(zones[k].on_time );
                 
                 response.append( "<pOnTime xsi:type=\"xsd:string\">");                                                                                                         
                 response.append(time ) ;                                                                                                         
                 response.append(  "</pOnTime>");   
                 
                 //-------------------------------------------------------------
                 std::string sensorName = "";
                 
                 if(zones[k].on_ih_alarm_device_id > 0 &&  zones[k].on_type > 0) //alarm jest uruchomony...
                 {
                     for(unsigned int s=0;s<countS;s++)
                     {
                         if( devices[s].id == zones[k].on_ih_alarm_device_id )
                         {
                             sensorName  = std::string(devices[s].name);
                             break;
                         }
                     }
                     
                 }
                 
                 response.append( "<pOnSensorName xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( sensorName  ) ;                                                                                                         
                 response.append(  "</pOnSensorName>");   
                 
                 //-------------------------------------------------------------
                 
                     
                 response.append( "</item>");      
                
              }
              
    
   
              
              
              response.append(  "</result></gxh:getAllAlarmZoneResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");                             
                              
             return this->build(msgIn,message_ret, response.c_str());    
} 
 
 
//------------------------------------------------------------------------------

bool app_module_wsdl::getAllOuts(int px_user_id, int px_group_id ,gxh_message* msgIn, gxh_message* message_ret, int idTop, int idTopSector )
{
     unsigned int countS = 0;
     unsigned int countZ = 0;
     unsigned int countL = 0;
  
     ih_sector* sectors    = (ih_sector*)this->getAppHandle()->getResource(RES_VECTOR_SECTOR, &countS);
     ih_zone* zones        = (ih_zone*)this->getAppHandle()->getResource(RES_VECTOR_ZONE, &countZ); 
     ih_outs* outs   = (ih_outs*)this->getAppHandle()->getResource(RES_VECTOR_OUTS, &countL); 
           
     
     
  //uprawnienia do sektorów..............................
     ih_sector sectors_tmp[100];
     ih_zone zones_tmp[300];
     unsigned int countSTmp = 0;
     unsigned int countZTmp = 0;
     
     for(unsigned int s=0; s<countS; s++)
     {
         bool cS = this->alowSector( sectors[s].id, px_group_id );
         if(cS){
             memcpy(&sectors_tmp[countSTmp], &sectors[s], sizeof(ih_sector) );
             countSTmp++;
         }
     }
     
     for(unsigned int s=0; s<countZ; s++)
     {
         bool cS = this->alowZone( zones[s].id, px_group_id );
         if(cS){
             memcpy(&zones_tmp[countZTmp], &zones[s], sizeof(ih_zone) );
             countZTmp++;
         }
     }
     
     //niech dalsza częśc kodu korzystan z innych tabel..
     sectors = sectors_tmp;
     zones   = zones_tmp;
     countS  = countSTmp;
     countZ  = countZTmp;
     
   
     //-----------------------------------------------------
     
    
     std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:getAllOutsResponse><result>";                                                                                                        
  
     for(unsigned int s=0; s< countS; s++)         
     {                 
         if(idTopSector >0 && sectors[s].id != idTopSector) continue;
         
       //   std::cout<<"\nSektor: "<<sectors[s].name.c_str();
     
         for(unsigned int z=0; z<countZ; z++)
         {                          
              if( zones[z].ih_sector_id != sectors[s].id ) continue;
              if(idTop >0 && zones[z].id != idTop )       continue;
        
              for(unsigned int k=0; k<countL;k++) 
              {                           
                 if( outs[k].ih_zone_id !=  zones[z].id ) continue;
              
                
                 
                 
                 response.append( "<item>");        
                 
                 //-------------------------------------------------------------
                 response.append( "<pId xsi:type=\"xsd:int\">");                                                                                                                       
                 response.append(  gxh_IntToString(outs[k].id)  ) ;                
                 response.append(  "</pId>");                                                                                                        
                 //-------------------------------------------------------------
                  
                 response.append( "<pName xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( outs[k].name  ) ;                                                                                                         
                 response.append(  "</pName>");                                                                                                        
                 //-------------------------------------------------------------
                  
                 response.append( "<pStatus xsi:type=\"xsd:string\">");                                                                                                                         
                 response.append( gxh_CharToString(  outs[k].status ) ) ;                                                                                                         
                 response.append(  "</pStatus>");                                                                                                         
                 //-------------------------------------------------------------         
                                   
                 response.append( "<pNumZoneTop xsi:type=\"xsd:int\">");            
                 response.append(  gxh_IntToString(outs[k].ih_zone_id) ) ;                                                                                                         
                 response.append(  "</pNumZoneTop>");                                                                                                              
                 //-------------------------------------------------------------
                
                 response.append( "<pZoneName xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( zones[z].name  ) ;     
                 response.append(  "</pZoneName>");  
                   
                  //-------------------------------------------------------------              
                                   
                 response.append( "<pNumSectorTop xsi:type=\"xsd:int\">");                
                 response.append(  gxh_IntToString( sectors[s].id) ) ;                                                                                                         
                 response.append(  "</pNumSectorTop>");                                                                                                                       
                 //-------------------------------------------------------------                 
                
                 response.append( "<pSectorName xsi:type=\"xsd:string\">");                                                                                                         
                 response.append(  sectors[s].name  ) ; 
                 response.append(  "</pSectorName>");
                 
                 //-------------------------------------------------------------
                 
                     
                 response.append( "</item>");      
                
              }
              
         }//end zones.. 
     } //end sectors...
              
              
              response.append(  "</result></gxh:getAllOutsResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");                             
                              
             return this->build(msgIn,message_ret, response.c_str());    
} 
 
//------------------------------------------------------------------------------

bool app_module_wsdl::getAllLight(int px_user_id,int px_group_id, gxh_message* msgIn, gxh_message* message_ret, int idTop, int idTopSector )
{
     unsigned int countS = 0;
     unsigned int countZ = 0;
     unsigned int countL = 0;
  
     ih_sector* sectors    = (ih_sector*)this->getAppHandle()->getResource(RES_VECTOR_SECTOR, &countS);
     ih_zone* zones        = (ih_zone*)this->getAppHandle()->getResource(RES_VECTOR_ZONE, &countZ); 
     ih_lighting* lights   = (ih_lighting*)this->getAppHandle()->getResource(RES_VECTOR_LIGHT, &countL); 
           
     
     //uprawnienia do sektorów..............................
     ih_sector sectors_tmp[100];
     ih_zone zones_tmp[300];
     unsigned int countSTmp = 0;
     unsigned int countZTmp = 0;
     
     for(unsigned int s=0; s<countS; s++)
     {
         bool cS = this->alowSector( sectors[s].id, px_group_id );
         if(cS){
             memcpy(&sectors_tmp[countSTmp], &sectors[s], sizeof(ih_sector) );
             countSTmp++;
         }
     }
     
     for(unsigned int s=0; s<countZ; s++)
     {
         bool cS = this->alowZone( zones[s].id, px_group_id );
         if(cS){
             memcpy(&zones_tmp[countZTmp], &zones[s], sizeof(ih_zone) );
             countZTmp++;
         }
     }
     
     //niech dalsza częśc kodu korzystan z innych tabel..
     sectors = sectors_tmp;
     zones   = zones_tmp;
     countS  = countSTmp;
     countZ  = countZTmp;
     
   
     //-----------------------------------------------------
     
    
     std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:getAllLightResponse><result>";                                                                                                        
  
     for(unsigned int s=0; s< countS; s++)         
     {                 
         if(idTopSector >0 && sectors[s].id != idTopSector) continue;
         
       //   std::cout<<"\nSektor: "<<sectors[s].name.c_str();
     
         for(unsigned int z=0; z<countZ; z++)
         {                          
              if( zones[z].ih_sector_id != sectors[s].id ) continue;
              if(idTop >0 && zones[z].id != idTop )       continue;
        
              for(unsigned int k=0; k<countL;k++) 
              {                           
                 if( lights[k].ih_zone_id !=  zones[z].id ) continue;
              
                
                 
                 
                 response.append( "<item>");        
                 
                 //-------------------------------------------------------------
                 response.append( "<pId xsi:type=\"xsd:int\">");                                                                                                                       
                 response.append(  gxh_IntToString(lights[k].id)  ) ;                
                 response.append(  "</pId>");                                                                                                        
                 //-------------------------------------------------------------
                  
                 response.append( "<pName xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( lights[k].name  ) ;                                                                                                         
                 response.append(  "</pName>");                                                                                                        
                 //-------------------------------------------------------------
                  
                 response.append( "<pStatus xsi:type=\"xsd:string\">");                                                                                                                         
                 response.append( gxh_CharToString(  lights[k].status ) ) ;                                                                                                         
                 response.append(  "</pStatus>");                                                                                                         
                 //-------------------------------------------------------------
                                    
                 response.append( "<pStatusTime xsi:type=\"xsd:datetime\">");                                                                                                         
                 response.append(  "NONE" ) ;                                                                                                         
                 response.append(  "</pStatusTime>");                                                                                                        
                 //-------------------------------------------------------------
               
                 response.append( "<pWat xsi:type=\"xsd:int\">");    
                 response.append( gxh_IntToString(lights[k].wat)  ) ;                                                                                                         
                 response.append(  "</pWat>");                                                                                                                         
                 //-------------------------------------------------------------
                                   
                 response.append( "<pNumZoneTop xsi:type=\"xsd:int\">");            
                 response.append(  gxh_IntToString(lights[k].ih_zone_id) ) ;                                                                                                         
                 response.append(  "</pNumZoneTop>");                                                                                                              
                 //-------------------------------------------------------------
                
                 response.append( "<pZoneName xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( zones[z].name  ) ;     
                 response.append(  "</pZoneName>");  
                   
                 
                  //-------------------------------------------------------------              
                                   
                 response.append( "<pNumSectorTop xsi:type=\"xsd:int\">");                
                 response.append(  gxh_IntToString( sectors[s].id) ) ;                                                                                                         
                 response.append(  "</pNumSectorTop>");                                                                                                                       
                 //-------------------------------------------------------------                 
                
                 response.append( "<pSectorName xsi:type=\"xsd:string\">");                                                                                                         
                 response.append(  sectors[s].name  ) ; 
                 response.append(  "</pSectorName>");
                 //-------------------------------------------------------------
                 
                     
                 response.append( "</item>");      
                
              }
              
         }//end zones.. 
     } //end sectors...
              
              
              response.append(  "</result></gxh:getAllLightResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");                             
                              
             return this->build(msgIn,message_ret, response.c_str());    
}

//------------------------------------------------------------------------------

 bool app_module_wsdl::getAllSector(int px_user_id, int px_group_id ,gxh_message* msgIn, gxh_message* message_ret)
 {       
        unsigned int count = 0; 
        ih_sector* sectors = (ih_sector*)this->getAppHandle()->getResource(RES_VECTOR_SECTOR, &count);
        
        
        
        //uprawnienia do sektorów..............................
           ih_sector sectors_tmp[100];
 
           unsigned int countSTmp = 0;           

           for(unsigned int s=0; s<count; s++)
           {
               bool cS = this->alowSector( sectors[s].id, px_group_id );
               if(cS){
                   memcpy(&sectors_tmp[countSTmp], &sectors[s], sizeof(ih_sector) );
                   countSTmp++;
               }
           }
 

           //niech dalsza częśc kodu korzystan z innych tabel..
           sectors = sectors_tmp; 
           count  = countSTmp; 


           //-----------------------------------------------------
       
          
              std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:getAllSectorResponse><result>";                                                                                                        
  
              for(unsigned int k=0; k<count;k++) 
              {                                   
                 response.append( "<item>");        
                 
                 //-------------------------------------------------------------
                 response.append( "<pId xsi:type=\"xsd:int\">");                                                                                                                         
                 response.append( gxh_IntToString(sectors[k].id)   ) ;                                                                                                        
                 response.append(  "</pId>");                                                                                                        
                 //-------------------------------------------------------------
                  
                 response.append( "<pName xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( sectors[k].name  ) ;                                                                                                         
                 response.append(  "</pName>");                                                                                                        
                 //-------------------------------------------------------------
                   
                 response.append( "<pType xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( sectors[k].type  ) ;                                                                                                         
                 response.append(  "</pType>");                                                                                                        
                 //-------------------------------------------------------------
                                    
                 response.append( "<pNumHomeTop xsi:type=\"xsd:int\">");                     
                 response.append( gxh_IntToString(sectors[k].ih_home_id) ) ;        
                 response.append(  "</pNumHomeTop>");                                                                                                        
                 //-------------------------------------------------------------                                 
                 
                 response.append( "</item>");                                                                                                        
              }
              
              response.append(  "</result></gxh:getAllSectorResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");                                                                                                                         
              
                                    
              return this->build(msgIn,message_ret, response.c_str());
 
 } 

//------------------------------------------------------------------------------
 
 bool app_module_wsdl::getAllZone(int px_user_id, int px_group_id,gxh_message* msgIn, gxh_message* message_ret, int idTop )
 {
        
       // int count = 0;
        //ih_zone zones[50]; 
        //this->getAppHandle()->getZoneIn(this->getDbSqLite(),idTop, 1, zones, &count, this->debugLevel,true);
        
        unsigned int count = 0;
        ih_zone* zones = (ih_zone*)this->getAppHandle()->getResource(RES_VECTOR_ZONE, &count);                            
        
         

            //uprawnienia do sektorów..............................                
               ih_zone zones_tmp[300];     
               unsigned int countZTmp = 0;

           

               for(unsigned int s=0; s<count; s++)
               {
                   bool cS = this->alowZone( zones[s].id, px_group_id );
                   if(cS){
                       memcpy(&zones_tmp[countZTmp], &zones[s], sizeof(ih_zone) );
                       countZTmp++;
                   }
               }

               //niech dalsza częśc kodu korzystan z innych tabel..            
               zones   = zones_tmp;               
               count  = countZTmp;


               //-----------------------------------------------------
        
        
        
        
              std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:getAllZoneResponse><result>";                                                                                                        
  
              for(unsigned int k=0; k<count;k++) 
              {
                 if  ( !( (zones[k].ih_sector_id == idTop) || idTop ==-1 ) ) continue;
                  
                 response.append( "<item>");        
                 
                 //-------------------------------------------------------------
                 response.append( "<pId xsi:type=\"xsd:int\">");                                                                                                                      
                 response.append( gxh_IntToString(zones[k].id)  ) ;                
                 response.append(  "</pId>");                                                                                                        
                 //-------------------------------------------------------------
                  
                 response.append( "<pName xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( zones[k].name  ) ;                                                                                                                         
                 response.append(  "</pName>");                                                                                                        
                 //-------------------------------------------------------------
                   
                 response.append( "<pType xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( zones[k].type  ) ;                                                                                                         
                 response.append(  "</pType>");                                                                                                        
                 //-------------------------------------------------------------
                                    
                 response.append( "<pNumSectorTop xsi:type=\"xsd:int\">");                 
                 response.append( gxh_IntToString(zones[k].ih_sector_id) ) ;               
                 response.append(  "</pNumSectorTop>");                                                                                                        
                 //-------------------------------------------------------------         
                                   
         
                 
                 
                 response.append( "</item>");                                                                                                        
              }
              
              response.append(  "</result></gxh:getAllZoneResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");                                                                                                                     
         
              
                                     
              return this->build(msgIn, message_ret, response.c_str());
 
 }  
 
//------------------------------------------------------------------------------
 
bool app_module_wsdl::goRefresh(gxh_message* msgIn, gxh_message* message_ret,int px_user_id,int px_group_id , double lat, double lng,  const char* wifi, const char* bluetooth  )
{
    unsigned int index = -1;
    
    for(unsigned int y=0; y<MAX_USER_BUFFER;y++)
    {
         if( this->usersSSID[y].px_group_id == px_group_id)
         {
             index = y; 
             break;
         }
    }
    
    if(index < 0)
    {
        this->getAppHandle()->show_log_module( "app_module_wsdl", "goRefresh", "Cannot find user", 5);         
        return false;
    }
    
    this->getAppHandle()->show_log_module( "app_module_wsdl", "goRefresh",  (const char*)this->usersSSID[index].username, 2);         
        
    
    //znajdz zmienna systemową powiazana z uzytkownikiem..            
    unsigned int size = 0;
    ih_driver_values* varArr =  ( ih_driver_values*)this->getAppHandle()->getResource(RES_VECTOR_DRIVER_VALUE, &size);
    
    for(unsigned int x=0; x < size; x++)
    {
       // std::cout<<"\nC: "<<varArr[x].const_function<<", "<<VAR_SYS_USER_GPS<<" | "<< varArr[x].px_group_id <<" | "<<px_group_id;
        
        if( varArr[x].free==false && varArr[x].const_function == VAR_SYS_USER_GPS && varArr[x].px_group_id == px_group_id)
        {   
           if(lat != 0 && lng != 0) 
           {
            std::string value =  gxh_DoubleToString(lat);
            value.append(":");
            value.append( gxh_DoubleToString(lng));   
           
            const char* val = value.c_str();
                      
            memset(varArr[x].value, 0, strlen(val)+1);
            memcpy(varArr[x].value, val, strlen(val));
            varArr[x].status    = 1; //update
            varArr[x].toHistory = true; 
           }
            //std::cout<<"\ngoRefresh "<<value.c_str()<<"\n\n";
        }
        
        
        //update wifi
        if( varArr[x].free==false && varArr[x].const_function == VAR_SYS_USER_WIFI && varArr[x].px_group_id == px_group_id)
        {                      
                      
           memset(varArr[x].value, 0, strlen(wifi)+1);
           
           memcpy(varArr[x].value, wifi, strlen(wifi));
           
           varArr[x].status    = 1; //update
           varArr[x].toHistory = false; 
           
           //std::cout<<"\ngoRefresh "<<value.c_str()<<"\n\n";
        }        
        
          //update bluetooth
        if( varArr[x].free==false && varArr[x].const_function == VAR_SYS_USER_BLUETOOTH && varArr[x].px_group_id == px_group_id)
        {                      
                      
           memset(varArr[x].value, 0, strlen(bluetooth)+1);
           
           memcpy(varArr[x].value, bluetooth, strlen(bluetooth));
           
           varArr[x].status    = 1; //update
           varArr[x].toHistory = false; 
           
           //std::cout<<"\ngoRefresh "<<value.c_str()<<"\n\n";
        }
    }
    
    std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:goRefreshResponse><result>";                                                                                                        
    response.append("OK");         
    response.append("</result>");         
    response.append("<notice>");  
    
    //////------
    unsigned int sizeN = 0;  
    ih_notice* notices =  (ih_notice*)this->getAppHandle()->getResource(RES_VECTOR_NOTICE, &sizeN);   
    
    for(unsigned int x=0; x<sizeN; x++)
    {
        if(notices[x].px_group_id == px_group_id && notices[x].readed == false && notices[x].free == false)
        {
              response.append( "<item>");        
                 
              std::cout<<"\n\nFrom: "<<notices[x].from ;
              
                 //-------------------------------------------------------------
                 response.append( "<pId xsi:type=\"xsd:int\">");                                                                                                                      
                 response.append( gxh_IntToString(notices[x].id)  ) ;  
                 response.append(  "</pId>");                                                                                                        
                 //-------------------------------------------------------------
                  
                 response.append( "<pTitle xsi:type=\"xsd:string\">");                                                                                                         
                 response.append(notices[x].title  ) ;                                                                                                         
                 response.append(  "</pTitle>");                                                                                                        
                 //-------------------------------------------------------------   
                 
                  response.append( "<pText xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( notices[x].content  ) ;                                                                                                         
                 response.append(  "</pText>");                                                                                                          
                 //-------------------------------------------------------------
                 
                 
                  response.append( "<pType xsi:type=\"xsd:string\">");                                                                                                         
                 response.append(  gxh_IntToString( notices[x].type)  ) ;                                                                                                         
                 response.append(  "</pType>");                                                                                                          
                 //-------------------------------------------------------------
                 
                  response.append( "<pDateTime xsi:type=\"xsd:datetime\">");                                                                                                         
                 response.append( notices[x].status_time_str  ) ;                                                                                                         
                 response.append(  "</pDateTime>");                                                                                                          
                 //-------------------------------------------------------------    
                 
                 response.append( "<pFuncId xsi:type=\"xsd:int\">");              
                 response.append( "0" ) ;                                                                                                         
                 response.append(  "</pFuncId>"); 
                 //-------------------------------------------------------------    
                 
                 response.append( "<pFuncModuleId xsi:type=\"xsd:int\">");                                                                                                                        
                 response.append( "0"  ) ;                                                                                                        
                 response.append(  "</pFuncModuleId>"); 
                 //-------------------------------------------------------------
                 
                  response.append( "<pUserResponse xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( "none" ) ;                                                                                                         
                 response.append(  "</pUserResponse>");                                                                                                          
                 //------------------------------------------------------------- 
                  
                 response.append( "<pResourceId xsi:type=\"xsd:int\">");                                                                                                                        
                 response.append( "0"  ) ; 
                 response.append(  "</pResourceId>"); 
                 //-------------------------------------------------------------
                
                  response.append( "<pFrom xsi:type=\"xsd:string\">");                                                                                                         
                 response.append( notices[x].from  ) ;                                                                                                         
                 response.append(  "</pFrom>");    
                 
                 //-------------------------------------------------------------
                
                 response.append( "</item>"); 
            
                 notices[x].readed = true;     //juz więcej nie pobierze..      
        }
    }
    
    
    response.append(  "</notice></gxh:goRefreshResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");                                                                                                                      
    
    return this->build(msgIn, message_ret, response.c_str());
 
} 
 
//------------------------------------------------------------------------------

bool app_module_wsdl::goLogin(gxh_message* msgIn, gxh_message* message_ret, char* username, char* pass, char* type)
{
    
    for(unsigned int m=0; m<strlen(username); m++)
    {
        if(username[m] == '\'') username[m] = '?';
    }
    
    for(unsigned int m=0; m<strlen(pass); m++)
    {
        if(pass[m] == '\'') pass[m] = '?';
    }
    
    for(unsigned int m=0; m<strlen(type); m++)
    {
        if(type[m] == '\'') type[m] = '?';
    }
    
    
 
    this->getAppHandle()->show_log_module("app_module_wsdl","goLogin","ok",2);

    sqlite3* dbSqLite;    
    int px_user_id = -1;
    int px_group_id = -1;
    
    std::string ssid = "S_";
    
    std::string baza = this->getAppHandle()->getDB();//this->getAppHandle()->getRunPath();
    //baza.append("../gxh_base.db");
     
     int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
     if( r != SQLITE_OK )
     {
          this->getAppHandle()->show_error("app_module_wsdl","goLogin","Cannot open sqlLite 1",0);             
     }else
     {    
         sqlite3_stmt* stmt;     
         std::string query="SELECT id,group_id FROM px_user WHERE username='"; 
         query.append(username);
         query.append("' AND haslo_encode='");
         query.append(pass);
         query.append("' AND is_active='1' AND is_delete='0'; ");
         
         this->getAppHandle()->show_log_data_base("app_module_wsdl", "goLogin",  query.c_str(), 6);
         
  
         const char* queryChar = query.c_str();        
         if(sqlite3_prepare(dbSqLite , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
         {
           this->getAppHandle()->show_error("app_module_wsdl","goLogin", sqlite3_errstr( sqlite3_errcode(dbSqLite  ) ),4);                                   
         }else
         {
             bool done = false;       
             while (!done)               
             { 
               int res = sqlite3_step (stmt);        
               if(res ==  SQLITE_ROW)
               {                
                   //<<"\nLog 1111";
                 px_user_id  = gxh_StringToInt(  (const char*)sqlite3_column_text(stmt,0)   );                                 
                 px_group_id = gxh_StringToInt(  (const char*)sqlite3_column_text(stmt,1)   );                                 
                 
                        //   std::cout<<"\nLog 222";
               }               
               if(res == SQLITE_DONE)done = true;                                     
            }
         }
     
          sqlite3_finalize(stmt);         
          sqlite3_close_v2(dbSqLite);
     }
     
   //  std::cout<<"\nLOGIN "<<px_user_id<<"|";
     
     if(px_user_id > 0 && px_group_id>0 )
     {
           sqlite3* dbSqLite1;         
    
           std::string baza2 = this->getAppHandle()->getDBTmp();//this->getAppHandle()->getRunPath();
           //baza2.append("../gxh_base_tmp.db");
     
           int r  =  sqlite3_open_v2(baza2.c_str(), &dbSqLite1, SQLITE_OPEN_READWRITE, NULL   );                                  
           if( r != SQLITE_OK )
             {
               this->getAppHandle()->show_error("app_module_wsdl","goLogin","Cannot open sqlLite 2",0);             
             }else
             {
               srand( time( NULL ) );
               int wylosowana_liczba =( std::rand() % 500 ) + 7;
                           
               ssid.append(gxh_IntToString(wylosowana_liczba));
               
               if(type[0] == '1' ) 
               {
                   ssid.append("_GXH_ANDROID_");
               }else
               if(type[0] == '2' )
               {
                   ssid.append("_GXH_IOS_");
               }else
               if(type[0] == '3' )
               {
                   ssid.append("_GXH_WEB_");
               }else
               if(type[0] == '4' )
               {
                   ssid.append("_GXH_OTH_");
               }else  ssid.append("_GXH_APP_");
               
               wylosowana_liczba =( std::rand() % 100 ) + 5;                           
               ssid.append(gxh_IntToString(wylosowana_liczba));
               
               wylosowana_liczba =( std::rand() % 900 ) + 10;                           
               ssid.append(gxh_IntToString(wylosowana_liczba));
               
               
               
               ssid.append(gxh_IntToString(px_user_id)) ;
          
               std::string query = "INSERT INTO px_session (id, session_id, px_user_id, operation,created_at, is_delete) VALUES (null,'";
               query.append(ssid);
               query.append("','");
               query.append(gxh_IntToString(px_user_id));
               query.append("','login',DATETIME('now','localtime'),'0'); ");
               

               
               
               //usuwamy by nie pamiętać zbyt dużo sesji tego uzytkownika .. szkoda pamięci..
               std::string queryDel = "DELETE FROM px_session WHERE px_user_id='";
               queryDel.append(gxh_IntToString(px_user_id));
               queryDel.append("' AND session_id LIKE '%GXH%'; ");
               
            
               //usun stare
                char *zErrMsg1 = 0;  
                
               this->getAppHandle()->show_log_data_base("app_module_wsdl", "goLogin",  queryDel.c_str(), 6);
                
                int rc1 = sqlite3_exec( dbSqLite1 , queryDel.c_str(), 0, 0, &zErrMsg1);
                if( rc1 != SQLITE_OK )
                  {        
                  //  std::cout<<queryDel.c_str()<<"\n";          
                    this->getAppHandle()->show_error("app_module_wsdl","goLogin", (const char*)zErrMsg1,0);
                    sqlite3_free(zErrMsg1);                                
                  }
               
                
                this->getAppHandle()->show_log_data_base("app_module_wsdl", "goLogin",  query.c_str(), 6);
                
                char *zErrMsg = 0;     
                int rc = sqlite3_exec( dbSqLite1 , query.c_str(), 0, 0, &zErrMsg);
                if( rc != SQLITE_OK )
                  {        
                    //std::cout<<query.c_str()<<"\n";          
                    this->getAppHandle()->show_error("app_module_wsdl","goLogin", (const char*)zErrMsg,0);
                    sqlite3_free(zErrMsg);                                
                  }else
                  {
                      
                   /*
                    
                         for(int y=0; y<MAX_USER_BUFFER; y++)
                         {
                          if(usersSSID[y].free)
                          {
                             usersSSID[y].free        = false;
                             usersSSID[y].ssid      = ssid;             
                             usersSSID[y].toSave      = true;
                             usersSSID[y].px_group_id = px_group_id;
                             usersSSID[y].px_user_id  = px_user_id;
                             
                             memset(usersSSID[y].username,0, 255);
                             memcpy(usersSSID[y].username,username, strlen(username));
                             break;
                             
                          }
                         } 
                     */
                    
                    
                  }                              
               sqlite3_close_v2(dbSqLite1);
             }
     }
     
     
     //this->loadPrGroup(); //zaladu uprawnienia......
     this->loadSessionToCache();
     
    
     if(px_user_id > 0)
     {
       std::string responseOk =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:goLoginResponse>";                                                             
       responseOk.append("<result  xsi:type=\"xsd:string\">");                                                      
       responseOk.append(ssid);                                                      
       responseOk.append("</result>");                                                                    
       responseOk.append("<privilege  xsi:type=\"xsd:string\">");   
       
      
       for(int i=0;i<MAX_USER_BUFFER;i++)
       {
          // std::cout<<"\n\nUser.....: "<<usersSSID[i].username<<" | userID "<<usersSSID[i].px_user_id<<" | groupID: "<<usersSSID[i].px_group_id ; //?????
           if(  usersSSID[i].px_user_id == px_user_id )
           {
               for(unsigned int w=0;w< usersSSID[i].privilege_count; w++)
               {
                  responseOk.append( usersSSID[i].privileges[w].name);                                                                    
                  responseOk.append(",");                                                                    
               }
              
           }
       }
       
       responseOk.append("</privilege>");                                                                           
       responseOk.append( "</gxh:goLoginResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");    
       
       return this->build(msgIn, message_ret, responseOk.c_str());        
      }         
               
    const char* response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Client</faultcode><faultstring>Login problem</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>\n";               
    return this->build(msgIn, message_ret, response); 
}

//------------------------------------------------------------------------------

bool app_module_wsdl::reloadModule(gxh_message* msgIn,gxh_message* message)
{
         this->reloadModules(msgIn,NULL);
    
         std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:goReloadModulesResponse soapenv:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><result  xsi:type=\"xsd:string\">";                                                      
         response.append("OK");  
         response.append( "</result></gxh:goReloadModulesResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");  
         
         return this->build(msgIn, message, response.c_str());
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

bool app_module_wsdl::goClose(gxh_message* msgIn,gxh_message* message_ret)
{
 
        gxh_message msg;               
        msg.func_number  =   0;      //for core
        msg.func_internal_number = 1; //close
        msg.gxh_handle   = msgIn->gxh_handle;
        msg.check_timeout        = 10;
        msg.execute_timeout      = 30 * 1000;                                                                
        this->getAppHandle()->sendGXHMessage(&msg);
               
        std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:goCloseResponse soapenv:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><result  xsi:type=\"xsd:string\">";                                                      
        response.append("OK");  
        response.append( "</result></gxh:goCloseResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");                                                             
        return this->build(msgIn, message_ret, response.c_str());      
}

//------------------------------------------------------------------------------

bool app_module_wsdl::goReloadPorts(gxh_message* msgIn,gxh_message* message_ret)
{
 
        gxh_message msgRet;
        bool ret = this->reloadPorts( msgIn, message_ret);
        
        if(ret)
        {  
         std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:goReloadPortsResponse soapenv:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><result  xsi:type=\"xsd:string\">";                                                      
         response.append(msgRet.dataStruct);  
         response.append( "</result></gxh:goReloadPortsResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");                                                            
         return this->build(msgIn, message_ret, response.c_str());
        }

        return false;
}

//------------------------------------------------------------------------------

bool app_module_wsdl::getVersion(gxh_message* msgIn,gxh_message* message_ret)
{
 
        std::string version = this->getAppHandle()->getVersion();
        std::string linuxN  = this->getAppHandle()->getLinux();
        std::string author  = this->getAppHandle()->getAuthor();      
      
                       
        std::string response =  "<?xml version=\"1.0\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:gxh=\"gxh\"><SOAP-ENV:Body><gxh:getVersionResponse>";                                                      
        
                
        response.append("<ver_gxh  xsi:type=\"xsd:string\">");                                                  
        response.append(version);                                                  
        response.append("</ver_gxh>");                                                  
        
        response.append("<ver_linux  xsi:type=\"xsd:string\">");                                                  
        response.append(linuxN);                                                  
        response.append("</ver_linux>");  
        
        response.append("<auth  xsi:type=\"xsd:string\">");                                                  
        response.append(author);                                                  
        response.append("</auth>");  
        
        
 
        
        response.append("<info xsi:type=\"xsd:string\">");                                                  
        response.append("");                                                  
        response.append("</info>");
        
        response.append( "</gxh:getVersionResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>");                                                      
       
      
        return this->build(msgIn, message_ret, response.c_str());
 
}

//------------------------------------------------------------------------------

int  app_module_wsdl::getParam(const char* name,char* out, int size,  char soapEnvelop[1024 * 100])
{
    std::string needed1 = "<";
    needed1.append(name);
    needed1.append(">");
    
    std::string needed2 = "<";
    needed2.append(name);
    needed2.append(" ");
    
    std::string text = std::string(soapEnvelop);
    
    
    int index = text.find(needed1);
    if(index == -1) index = text.find(needed2);
    
    if(index < 0) return index;
    
   
    
    int tLength = text.length();
    
    
    int startVal = -1;
    for(int w=index; w<tLength; w++ )
    {
        if(soapEnvelop[w]=='>') { startVal = w+1; break; }
    }
    
    
    
    int endVal = -1;
    for(int w=startVal; w<tLength; w++ )
    {
        if(soapEnvelop[w]=='<') { endVal = w+1; break; }
    }
         
    if(startVal>0 && endVal >0 )
    {
      int x=0;
      for(int w=startVal; w<endVal; w++)
      {
        out[x] = soapEnvelop[w];
        x++;
      }
      out[x-1] = 0;
    }
    
  
    return index;
    
}

//------------------------------------------------------------------------------

void app_module_wsdl::parseHeader(gxh_message* msgIn,char headers[20][1024], char soapAction[1024], int* numHeader,int* sizeSopaEnv, char soapEnvelop[1024 * 100],  char gxhVersion[40], char gxhPublicKey[40] )
{          
    int headerIndex = 0;
    int charNum = 0;
    const char* content = msgIn->dataStruct;
    unsigned int dataSize = msgIn->sizeDataStruct;
    int startSoapEnv = 0;
    
    //for(unsigned int w=0; w< strlen(content)-1; w++)
    for(unsigned int w=0; w< dataSize-1; w++)
    {     
            if( content[w] == '\r' && content[w+1] == '\n' && content[w+2] == '\r' && content[w+3] == '\n') //koniec naglowaka...
            {
                startSoapEnv = w+4;
                break;
            }
        
            if( content[w] == '\r' && content[w+1] == '\n') //koniec lini...
            {
                headers[headerIndex][charNum] = '\0';
                headerIndex++;
                w+=1;
                charNum = 0;
                
                if(headerIndex > 20) break; //więcej sie nie zmieści...
            }else
            {
             if(charNum < 1000)
             {   
               headers[headerIndex][charNum] = content[w];        
               charNum++;
           
             }
            }
    }
    
    *numHeader = headerIndex;
    soapAction[0] = '\0'; //domyslnie brak...
    gxhVersion[0] = '\0'; //domyslnie brak...
    gxhPublicKey[0] = '\0'; //domyslnie brak...
    
    
    //soapAction
    for(int w=0; w<*numHeader; w++)
    {
       
        this->getAppHandle()->show_log_module("app_module_wsdl","parseHeader",headers[w],6 );
        
        if(strlen(headers[w]) > 20) //SOAPAction: "http://83.220.118.22:7777/wsdl,gologin"
        {
            if(headers[w][0] == 'S' && headers[w][1] == 'O' && headers[w][2] == 'A' && headers[w][3] == 'P')
            {  
                bool goAdd = false;
                int pIndex=0;
                for(unsigned int p=0; p<strlen(headers[w]); p++)
                {
                    if(headers[w][p] == '"') goAdd = false;
                    
                    if(goAdd)
                    {
                        soapAction[pIndex] = headers[w][p];
                        pIndex++;                        
                    }else
                    {   
                      if(headers[w][p] == ',') goAdd = true;                     
                    }
                        
                }
                soapAction[pIndex]='\0';
            }
                   
        }
 
    }
    
   
    
    //GXH Version
    for(int w=0; w<*numHeader; w++)
    {
       
        this->getAppHandle()->show_log_module("app_module_wsdl","parseHeader",headers[w],6 );
        
        if(strlen(headers[w]) > 10) //SOAPAction: "http://83.220.118.22:7777/wsdl,gologin"
        {
            if(headers[w][0] == 'G' && headers[w][1] == 'X' && headers[w][2] == 'H' && headers[w][3] == ' ' && headers[w][4] == 'V')
            {  
               
                
                bool goAdd = false;
                int pIndex=0;
                for(unsigned int p=0; p<strlen(headers[w]); p++)
                {
                    //if(headers[w][p] == ' ') goAdd = false;
                    
                    if(goAdd)
                    {
                        gxhVersion[pIndex] = headers[w][p];
                        pIndex++;                        
                    }else
                    {   
                      if(headers[w][p] == ':')
                      {
                          goAdd = true;   
                          if(headers[w][p+1] == ' ') p++;
                      }
                    }
                        
                }
                gxhVersion[pIndex]='\0';
            }
                   
        }
 
    }
    
    //GXH public key
    for(int w=0; w<*numHeader; w++)
    {
       
        this->getAppHandle()->show_log_module("app_module_wsdl","parseHeader",headers[w],6 );
        
        if(strlen(headers[w]) > 6) //SOAPAction: "http://83.220.118.22:7777/wsdl,gologin"
        {
            if(headers[w][0] == 'a' && headers[w][1] == 'p' && headers[w][2] == 'i' && headers[w][3] == 'K' && headers[w][4] == 'e' && headers[w][5] == 'y' )
            {  
               
                
                bool goAdd = false;
                int pIndex=0;
                for(unsigned int p=0; p<strlen(headers[w]); p++)
                {
                    //if(headers[w][p] == ' ') goAdd = false;
                    
                    if(goAdd)
                    {
                        gxhPublicKey[pIndex] = headers[w][p];
                        pIndex++;                        
                    }else
                    {   
                      if(headers[w][p] == ':')
                      {
                          goAdd = true;   
                          if(headers[w][p+1] == ' ') p++;
                      }
                    }
                        
                }
                gxhPublicKey[pIndex]='\0';
            }
                   
        }
 
    }
    
    
    //przepisz xlsa..
    *sizeSopaEnv = 0;
    for(unsigned int w=startSoapEnv; w< strlen(content); w++)
    { 
        soapEnvelop[*sizeSopaEnv] = content[w];
        (*sizeSopaEnv)++;
    }
    
    soapEnvelop[*sizeSopaEnv] = '\0';
    
 }

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------