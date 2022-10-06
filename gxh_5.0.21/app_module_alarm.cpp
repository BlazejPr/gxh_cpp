/* 
 * File:   app_module_alarm.cpp
 * Author: Blazej Kita
 * 
 * Created on 08 styczeń 2020, 14:31
 */

#include "app_module_alarm.h"


 
app_module* get_module(gxh_scope* appHomeHandle )
{
    return new app_module_alarm(appHomeHandle); 
};
 
//-------------------------------------------------------------------------------
  
app_module_alarm::app_module_alarm(gxh_scope* appHomeHandle) : app_module(appHomeHandle) 
{               
    this->getAppHandle()->show_log_module("app_module_alarm","app_module_alarm","start",2 );         
    
    
    
    
    
    this->alarm_zones_count = 0;
    this->getAppHandle()->destroyResource(RES_VECTOR_ALARM_ZONE);
    
    this->alarm_devices_count = 0;
    this->getAppHandle()->destroyResource(RES_VECTOR_ALARM_DEVICE);
    
    
    
     sqlite3* dbSqLite;               
     
     
     std::string baza = this->getAppHandle()->getDB(); //this->getAppHandle()->getRunPath();
     //baza.append("../gxh_base.db");
     
     int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
     if( r != SQLITE_OK )
     {
          this->getAppHandle()->show_error("app_module_alarm","app_module_alarm","Cannot open sqlLite 1",0);            
     }
     else 
     {
    
        std::string query="SELECT ih_alarm_zone.id, ih_alarm_zone.name, ih_alarm_zone.active, ih_alarm_zone.on_type, ih_alarm_zone.on_time, ih_alarm_zone.on_ih_alarm_device_id FROM ih_alarm_zone WHERE  ih_alarm_zone.is_delete='0'; ";                
        this->getAppHandle()->show_log_data_base("app_module_alarm","app_module_alarm", query.c_str() ,6);  
         
        sqlite3_stmt* stmt; 
    
        const char* queryChar = query.c_str();          
        if(sqlite3_prepare(dbSqLite , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
        {
          this->getAppHandle()->show_error("app_module_alarm","app_module_alarm", sqlite3_errstr( sqlite3_errcode(dbSqLite  ) ),4);          
          sqlite3_finalize(stmt);                 
        }else
        {    
           bool done = false;       
           while (!done)               
           { 
               int res = sqlite3_step (stmt);        
               if(res ==  SQLITE_ROW)
               {                       
                 if(this->alarm_zones_count > 20)  break;//to save..
                 
                  this->alerm_zones[this->alarm_zones_count].toSave = false;  
                          
                  this->alerm_zones[this->alarm_zones_count].id                 = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 0) ) );
                  
                  const char* tmp1                                = (const char*)sqlite3_column_text(stmt, 1 );                  
                  if(tmp1 != NULL)
                  {
                  memset(this->alerm_zones[this->alarm_zones_count].name, 0, 100 );
                  memcpy(this->alerm_zones[this->alarm_zones_count].name, tmp1, strlen(tmp1));
                  }
                                   
                  const char* tmp2     = (const char*)sqlite3_column_text(stmt, 2 );                           
                  if(tmp2 != NULL)
                  {
                    this->alerm_zones[this->alarm_zones_count].active  = gxh_StringToInt( std::string(  tmp2 ) ); 
                  }else   this->alerm_zones[this->alarm_zones_count].active=0;
          
                  const char* tmp3     = (const char*)sqlite3_column_text(stmt, 3 );                           
                  if(tmp3 != NULL)
                  {
                    this->alerm_zones[this->alarm_zones_count].on_type  = gxh_StringToInt( std::string(  tmp3 ) ); 
                  }else   this->alerm_zones[this->alarm_zones_count].on_type=0;
                  
                  const char* tmp4                                = (const char*)sqlite3_column_text(stmt, 4 );                  
                  if(tmp4 != NULL)
                  {
                    memset(this->alerm_zones[this->alarm_zones_count].on_time, 0, 25 );
                    if(strlen(tmp4) >= 10 ) memcpy(this->alerm_zones[this->alarm_zones_count].on_time, tmp4, 19);//data ma 16 cyfr..%Y-%m-%d %H:%M:%S 0000-00-00 00:00:00
                  }
                  
                  
                  const char* tmp5     = (const char*)sqlite3_column_text(stmt, 5 );                           
                  if(tmp5 != NULL)
                  {
                    this->alerm_zones[this->alarm_zones_count].on_ih_alarm_device_id  = gxh_StringToInt( std::string(  tmp5 ) ); 
                  }else   this->alerm_zones[this->alarm_zones_count].on_ih_alarm_device_id=-1;
                  
                  
                  this->alarm_zones_count++;
               }               
              if(res == SQLITE_DONE)done = true;                                     
            }
          sqlite3_finalize(stmt); 
         }
        
        
        //DEVICES...
        
        query="SELECT ih_alarm_device.id, ih_alarm_device.name, ih_alarm_device.type,ih_alarm_device.ih_alarm_zone_id FROM ih_alarm_device WHERE  ih_alarm_device.is_delete='0'; ";                
        this->getAppHandle()->show_log_data_base("app_module_alarm","app_module_alarm", query.c_str() ,6);  
         
       // sqlite3_stmt* stmt; 
    
        queryChar = query.c_str();          
        if(sqlite3_prepare(dbSqLite , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
        {
          this->getAppHandle()->show_error("app_module_alarm","app_module_alarm", sqlite3_errstr( sqlite3_errcode(dbSqLite  ) ),4);          
          sqlite3_finalize(stmt);                 
        }else
        {    
           bool done = false;       
           while (!done)               
           { 
               int res = sqlite3_step (stmt);        
               if(res ==  SQLITE_ROW)
               {                       
                 if(this->alarm_devices_count > 40)  break;//to save..
                 
                  this->alerm_devices[this->alarm_devices_count].toSave = false;          
                          
                  this->alerm_devices[this->alarm_devices_count].id                 = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 0) ) );
                  
                  const char* tmp1                  = (const char*)sqlite3_column_text(stmt, 1 );                  
                  memset(this->alerm_devices[this->alarm_devices_count].name, 0, 100 );
                  memcpy(this->alerm_devices[this->alarm_devices_count].name, tmp1, strlen(tmp1));
                                   
                  const char* tmp2     = (const char*)sqlite3_column_text(stmt, 2 );                                           
                  memset(this->alerm_devices[this->alarm_devices_count].type, 0, 100 );
                  memcpy(this->alerm_devices[this->alarm_devices_count].type, tmp2, strlen(tmp2));
                    
                  this->alerm_devices[this->alarm_devices_count].ih_alarm_zone_id                 = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 3) ) );
          
                  
                 this->getAppHandle()->show_log_module("app_module_alarm","app_module_alarm, devices",this->alerm_devices[this->alarm_devices_count].name,2 );         
                 
                  
                  this->alarm_devices_count++;
               }               
              if(res == SQLITE_DONE)done = true;                                     
            }
          sqlite3_finalize(stmt); 
         }
        
        
      }
    
    
     this->getAppHandle()->registerResource( this->alerm_zones ,this->alarm_zones_count, RES_VECTOR_ALARM_ZONE );
     this->getAppHandle()->registerResource( this->alerm_devices ,this->alarm_devices_count, RES_VECTOR_ALARM_DEVICE );
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
     
     
  
     sqlite3_close_v2(dbSqLite);
     
     this->wyzerujCzasy(0);
     
}

//------------------------------------------------------------------------------

app_module_alarm::~app_module_alarm() 
{     
    this->getAppHandle()->show_log_module("app_module_alarm","app_module_alarm","stop",2 );
}


//-------------------------------------------------------+-----------------------

void app_module_alarm::onClick(int ih_driver_port_id, int listener_status)
{
}

//-------------------------------------------------------+-----------------------

void app_module_alarm::forceCancel()
{  
      this->getAppHandle()->show_log_module("app_module_alarm","app_module_alarm","forceCancel",2);
      
    
      this->forceSynchronize();
      this->getAppHandle()->destroyResource(RES_VECTOR_GATE);
}

//-------------------------------------------------------+-----------------------

void app_module_alarm::forceSynchronize()
{  
     this->getAppHandle()->show_log_module("app_module_alarm","forceSynchronize","start",20);      
     
     
     
    for(int x=0; x<this->alarm_zones_count; x++)
     {
      
        
         if(this->alerm_zones[x].toSave==true)
         {
             std::string msg = "Update alarm zone status for ";
             msg.append( this->alerm_zones[x].name);             
             this->getAppHandle()->show_log_module("app_module_alarm","forceSynchronize",msg.c_str(),6); 
             
             //----
             sqlite3* dbSqLite;  
             
             std::string baza = this->getAppHandle()->getDB();//this->getAppHandle()->getRunPath();
             //baza.append("../gxh_base.db");
     
             int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );      //tu coś nie gra..                            
             if( r != SQLITE_OK )
             {
               this->getAppHandle()->show_error("app_module_alarm","forceSynchronize","Cannot open sqlLite 1",0);                 
             }else
             {                                           
                 std::string statusTMP = "0";
                 if(this->alerm_zones[x].active =='1' || this->alerm_zones[x].active==1) statusTMP = "1";
                 if(this->alerm_zones[x].active =='2' || this->alerm_zones[x].active==2) statusTMP = "2";
       
                 std::string query="";
                 query.append("UPDATE ih_alarm_zone SET  active='");
                 query.append( statusTMP  );
                 query.append( "', on_type='"  );                                                   
                 query.append(   gxh_IntToString( this->alerm_zones[x].on_type)   );                                                   
                 query.append( "', on_time='"  );   
                 query.append( std::string( this->alerm_zones[x].on_time)   );   
                 query.append( "', on_ih_alarm_device_id='"  );   
                 query.append( gxh_IntToString( this->alerm_zones[x].on_ih_alarm_device_id)   );
                 query.append("' WHERE is_delete='0' AND id='");
                 query.append( gxh_IntToString(this->alerm_zones[x].id) );
                 query.append("'; ");
                   
                 
           
                 char *zErrMsg = 0;     
                 int rc = sqlite3_exec( dbSqLite , query.c_str(), 0, 0, &zErrMsg);
                 if( rc != SQLITE_OK )
                     {        
                        std::cout<<query.c_str()<<"\n";          
                        this->getAppHandle()->show_error("app_module_alarm","forceSynchronize", (const char*)zErrMsg,0);
                        sqlite3_free(zErrMsg);                                
                        sqlite3_close_v2(dbSqLite);                                                 
                        
                     }else
                     {                                                            
                        this->alerm_zones[x].toSave = false; 
                        sqlite3_close_v2(dbSqLite);   
                      
                     }              
             }
                                     
             //----
         }
     
       
        
     }
     
     
     
    
    //sprawdczy czy jaksa czujka trzyma stan wysoki..
     for(int index=0; index<this->alarm_devices_count; index++)
     {
         
          if( this->alerm_devices[index].countMs > 0)
            {
               
                    struct timeval tp;
                    gettimeofday(&tp, NULL);
                    unsigned long  useconds = tp.tv_sec * 1000 + tp.tv_usec / 1000;// milisekundy..

                    unsigned long diffUsec = -1;
                    unsigned long diffSec  = -1;              

                    
                        diffUsec = useconds - this->alerm_devices[index].countMs;
                        if(diffUsec > 0)  diffSec = diffUsec / 1000; //zamień na sekundy....
                    
        
                        if(diffSec > 5) //czujka wykrywa ruch ponad 5 sek...
                        {
                        
                              int ih_alarm_zone_id = -1;      
                              int indexZone  = -1;
                               ih_alarm_zone_id  = this->alerm_devices[index].ih_alarm_zone_id;
                               

                                for(int x=0; x<this->alarm_zones_count; x++)
                                {    
                                    if(this->alerm_zones[x].id == ih_alarm_zone_id) 
                                    {
                                        indexZone = x;            
                                        break; }
                                }

                            
                                           //czy strefa jest uzbrojona.???? - alarm głośny....
                                //wykroty ruch + strefa jest uzbrojona... = włącz alarm...... jeśli nie był wcześniej uruchomiony to go uruchamiamy..
                                if(    this->alerm_zones[indexZone].on_type!=this->alerm_zones[indexZone].active && (this->alerm_zones[indexZone].active=='1' ||  this->alerm_zones[indexZone].active==1)  )
                                {

                                    std::string info2 = "Alerm - Naruszenie czujnika ruchu " ;
                                    info2.append( this->alerm_devices[index].name);
                                    info2.append(" w uzbrojonej strefie ");
                                    info2.append( this->alerm_zones[indexZone].name  );
                                    this->getAppHandle()->show_log_module("app_module_alarm","noticeSpeaker",info2.c_str() ,0 );   

                                    //wyslij informacje o naruszeniu alarmu...
                                    this->sendInterrrupt(IH_MODULE_FUNCTION_ALARM_ON,    this->alerm_zones[indexZone].id );
                                    //naruszenie sterfy uzbrojonej...
                                    this->sendInterrrupt(IH_MODULE_FUNCTION_ALARM_ALERT_ZONE_PSEC,    this->alerm_zones[indexZone].id );

                                    //uruchomiono alarm w domu
                                    this->sendInterrrupt(IH_MODULE_FUNCTION_ALARM_NOBJ,    1 );
                                    //uruchom syrene alarmową..
                                    this->noticeSpeaker( 0);  //syrena wyje cały czas    


                                    time_t rawtime;
                                    struct tm * timeinfo;
                                    char buffer[80];

                                    time (&rawtime);
                                    timeinfo = localtime(&rawtime);

                                    strftime(buffer,sizeof(buffer),"%Y-%m-%d %H:%M:%S",timeinfo);
                                    std::string str(buffer);

                                    this->alerm_zones[indexZone].on_type = this->alerm_zones[indexZone].active;
                                    this->alerm_zones[indexZone].on_ih_alarm_device_id = this->alerm_devices[index].id;
                                    memcpy(this->alerm_zones[indexZone].on_time, str.c_str(), strlen(str.c_str()) );   
                                    this->alerm_zones[indexZone].toSave = true;
                                }      


                               //cichy alarm bez syreny... jeśli nie był uruchomiony wcześniej uruchamiamy...
                                if(   this->alerm_zones[indexZone].on_type!=this->alerm_zones[indexZone].active && (this->alerm_zones[indexZone].active=='2' ||  this->alerm_zones[indexZone].active==2)  )
                                {

                                    std::string info2 = "Alerm - Naruszenie czujnika ruchu " ;
                                    info2.append( this->alerm_devices[index].name);
                                    info2.append(" w uzbrojonej strefie ");
                                    info2.append( this->alerm_zones[indexZone].name  );
                                    this->getAppHandle()->show_log_module("app_module_alarm","noticeSpeaker",info2.c_str() ,0 );   


                                    //wyslij informacje o naruszeniu alarmu...
                                    this->sendInterrrupt(IH_MODULE_FUNCTION_ALARM_ON,    this->alerm_zones[indexZone].id );

                                    //naruszenie sterfy uzbrojonej...
                                    this->sendInterrrupt(IH_MODULE_FUNCTION_ALARM_ALERT_ZONE_PSEC,    this->alerm_zones[indexZone].id );

                                      //uruchomiono alarm w domu
                                    this->sendInterrrupt(IH_MODULE_FUNCTION_ALARM_NOBJ,    1 );

                                    time_t rawtime;
                                    struct tm * timeinfo;
                                    char buffer[80];

                                    time (&rawtime);
                                    timeinfo = localtime(&rawtime);

                                    strftime(buffer,sizeof(buffer),"%Y-%m-%d %H:%M:%S",timeinfo);
                                    std::string str(buffer);

                                    this->alerm_zones[indexZone].on_type = this->alerm_zones[indexZone].active;
                                    this->alerm_zones[indexZone].on_ih_alarm_device_id = this->alerm_devices[index].id;
                                    memcpy(this->alerm_zones[indexZone].on_time, str.c_str(), strlen(str.c_str()) );
                                    this->alerm_zones[indexZone].toSave = true;
                                } 


                                
                        
                        }
                
            }
         
     }
     
     
     
     this->getAppHandle()->show_log_module("app_module_alarm","forceSynchronize","stop",20);          
}

//-------------------------------------------------------+-----------------------


bool app_module_alarm::execute(gxh_message* inMsg,gxh_message* message_ret)  
{ 
    
          
      gxh_message message;
      memcpy(&message, inMsg, sizeof(gxh_message));
    
      
     
         
          if(message.func_number == IH_MODULE_FUNCTION_ACTIVE_ALARM_ZONE )     //aktywacja alarmu w strefie...
           {
          
                return this->goActiveAlarmZone(&message, message_ret);
           }
      
          if(message.func_number == IH_MODULE_FUNCTION_ACTIVE_SILENT_ALARM_ZONE )     //aktywacja alarmu w strefie...
           {
          
                return this->goActiveAlarmZone2(&message, message_ret);
           }
      
          if(message.func_number == IH_MODULE_FUNCTION_DEZACTIVE_ALARM_ZONE )     //dezaktywacja alarmu w strefie...
           {
          
                 return this->goDezactiveAlarmZone(&message, message_ret);
           }
      
           if(message.func_number == IH_MODULE_FUNCTION_ACTIVE_ALARM_ALLZONE )     //aktywacja alarmu we  wszystkich sterfach..
           {
          
               return this->goActiveAlarmZoneAll(&message, message_ret);
           }      
      
           if(message.func_number == IH_MODULE_FUNCTION_DEZACTIVE_ALARM_ALLZONE )     //dezaktywacja alarmu we  wszystkich sterfach..
           {          
              return this->goDezactiveAlarmZoneAll(&message, message_ret);
           }
      
           if(message.func_number == IH_MODULE_FUNCTION_ALARM_ALERT_ZONE )     //aktywacja naruszenia strefy..
           {          
               // return this->goActive(&message, message_ret);
           }
      
         
          if(message.func_number == IH_MODULE_FUNCTION_ALARM_ON_SPEAKER )     //uruchom syrene alarmową...
           {          
                return this->onSpeaker(&message, message_ret);
           
           }
      
      
           if(message.func_number == IH_MODULE_FUNCTION_ALARM_OFF_SPEAKER )     //wyłącz syrene alarmową...
           {          
                return this->offSpeaker(&message, message_ret);
           }
      
           if(message.func_number == IH_MODULE_FUNCTION_ALARM_MOVE_SENSOR )     //aktywacja naruszenia strefy.. czujnik ruchu
           {          
                return this->moveSensor(&message, message_ret);
           }
      
           if(message.func_number == IH_MODULE_FUNCTION_ALARM_MOVE_SMOKE )     //aktywacja naruszenia strefy.. czujnik dymu
           {          
                return this->moveSensor(&message, message_ret);
           }
      
          if(message.func_number == IH_MODULE_FUNCTION_ALARM_MOVE_GAS )     //aktywacja naruszenia strefy.. czujnik  gazu
           {          
                return this->moveSensor(&message, message_ret);
           }
     
     
          const char* response = "ALARM MODULE"; 
          gxh_message msg;
          msg.gxh_handle = message.gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;
          
}

//-----------------------------------------------------------------------------

bool app_module_alarm::goActiveAlarmZone(gxh_message* inMsg,gxh_message* message_ret) 
{  
      
    
     ih_driver_port_function functionList;
     memcpy(&functionList,inMsg->dataStruct, sizeof(ih_driver_port_function) );
     
     
     int index = -1;
     
     for(int x=0; x<this->alarm_zones_count; x++)
     {
         if(this->alerm_zones[x].id == functionList.resource_id) { index = x; break; }
     }
     
     if( index == -1) 
     {
          const char* response = "ALARM ZONE NOT FOUND"; 
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;
     }
     
     
     
     this->wyzerujCzasy(this->alerm_zones[index].id );
     
     
     
         this->getAppHandle()->show_log_module("app_module_alarm","goActiveAlarmZone",this->alerm_zones[index].name ,1 );         
         
     
         
          //ustaw strefe na aktywną..
           this->alerm_zones[index].active = 1;
           this->alerm_zones[index].toSave = true;           
           this->sendInterrrupt(IH_MODULE_FUNCTION_ALARM_ACTIVE,   this->alerm_zones[index].id );
           
          
          int countPortOut = 0;         
          ih_driver_port_function functionListOut[20];   
          
          int sizeS = 0;
          ih_driver_port_function* tmpS = this->getAppHandle()->getDriverPortFunction(&sizeS);
          
          for(int x=0; x<sizeS; x++)
          {
              if(tmpS[x].resource_id == functionList.resource_id &&  //znajdz porty wyjscia... do aktywacji alarmu..
                      (
                         (tmpS[x].const_function_symbol==IH_MODULE_FUNCTION_ALARM_OUT_ACTIVE )                                   
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
         
             isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id , IH_SET_PORT_CLICK,-1,1000 * 5,-1  );       
                        
             
             if(isOk)
             {
                 //
             }
              
                   
          }
       
          this->noticeSpeaker( 1);
          
          const char* response = "ZONE LOCK"; 
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;   
    
}

//-----------------------------------------------------------------------------

bool app_module_alarm::goActiveAlarmZone2(gxh_message* inMsg,gxh_message* message_ret) 
{  
      
    
     ih_driver_port_function functionList;
     memcpy(&functionList,inMsg->dataStruct, sizeof(ih_driver_port_function) );
     
     
     int index = -1;
     
     for(int x=0; x<this->alarm_zones_count; x++)
     {
         if(this->alerm_zones[x].id == functionList.resource_id) { index = x; break; }
     }
     
     if( index == -1) 
     {
          const char* response = "ALARM ZONE NOT FOUND"; 
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;
     }
     
     this->wyzerujCzasy(this->alerm_zones[index].id );
     
     
     
         this->getAppHandle()->show_log_module("app_module_alarm","goActiveAlarmZone silent alarm",this->alerm_zones[index].name ,1 );         
         
         
          //ustaw strefe na aktywną..
           this->alerm_zones[index].active = 2; //SILENT.. ALARM...
           this->alerm_zones[index].toSave = true;           
           this->sendInterrrupt(IH_MODULE_FUNCTION_ALARM_ACTIVE,   this->alerm_zones[index].id );
           
          
          int countPortOut = 0;         
          ih_driver_port_function functionListOut[20];   
          
          int sizeS = 0;
          ih_driver_port_function* tmpS = this->getAppHandle()->getDriverPortFunction(&sizeS);
          
          for(int x=0; x<sizeS; x++)
          {
              if(tmpS[x].resource_id == functionList.resource_id &&  //znajdz porty wyjscia... do aktywacji alarmu..
                      (
                         (tmpS[x].const_function_symbol==IH_MODULE_FUNCTION_ALARM_OUT_ACTIVE )                                   
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
         
             isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id , IH_SET_PORT_CLICK,-1,1000 * 5,-1  );       
                        
             
             if(isOk)
             {
                 //
             }
              
                   
          }
       
          this->noticeSpeaker( 1);
          
          const char* response = "ZONE LOCK (SILENT)"; 
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;   
    
}


//-----------------------------------------------------------------------------

bool app_module_alarm::goActiveAlarmZoneAll(gxh_message* inMsg,gxh_message* message_ret) 
{  
      
    
     ih_driver_port_function functionList;
     memcpy(&functionList,inMsg->dataStruct, sizeof(ih_driver_port_function) );
     
     
           
       for(int index=0; index<this->alarm_zones_count; index++)
          {
           
             this->wyzerujCzasy(this->alerm_zones[index].id );
           
           //ustaw strefe na aktywną..
           this->alerm_zones[index].active = 1;
           this->alerm_zones[index].toSave = true;
          this->sendInterrrupt(IH_MODULE_FUNCTION_ALARM_ACTIVE,   this->alerm_zones[index].id );
           
           
                    this->getAppHandle()->show_log_module("app_module_alarm","goActiveAlarmZoneAll",this->alerm_zones[index].name ,1 );         


                     int countPortOut = 0;         
                     ih_driver_port_function functionListOut[20];   

                     int sizeS = 0;
                     ih_driver_port_function* tmpS = this->getAppHandle()->getDriverPortFunction(&sizeS);

                     for(int x=0; x<sizeS; x++)
                     {
                         if(tmpS[x].resource_id == this->alerm_zones[index].id &&  //znajdz porty wyjscia... do aktywacji alarmu..
                                 (
                                    (tmpS[x].const_function_symbol==IH_MODULE_FUNCTION_ALARM_OUT_ACTIVE )                                   
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

                        isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id , IH_SET_PORT_CLICK,-1,1000 * 5,-1  );       


                        if(isOk)
                        {
                            //
                        }


                     }
          }
                     
                     
          this->noticeSpeaker( 1);
     
          const char* response = "ALL ZONE LOCK"; 
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;   
    
}


//-----------------------------------------------------------------------------

void app_module_alarm::wyzerujCzasy(int zone_id)
{
     for(int x=0; x<this->alarm_devices_count; x++)
     {
         if(zone_id == 0 ||   this->alerm_devices[x].ih_alarm_zone_id==zone_id  )
         {
             this->alerm_devices[x].timestampLastChange = 0;
             this->alerm_devices[x].timestampFirst = 0;
             this->alerm_devices[x].countMs = 0;
         }
     }
    
}

//-----------------------------------------------------------------------------

bool app_module_alarm::goDezactiveAlarmZone(gxh_message* inMsg,gxh_message* message_ret) 
{  
      
    
     ih_driver_port_function functionList;
     memcpy(&functionList,inMsg->dataStruct, sizeof(ih_driver_port_function) );
     
     
     int index = -1;
     
     for(int x=0; x<this->alarm_zones_count; x++)
     {
         if(this->alerm_zones[x].id == functionList.resource_id) { index = x; break; }
     }
     
     if( index == -1) 
     {
          const char* response = "ALARM ZONE NOT FOUND"; 
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;
     }
     
     this->wyzerujCzasy(this->alerm_zones[index].id );
     
     
         this->getAppHandle()->show_log_module("app_module_alarm","goDezactiveAlarmZone",this->alerm_zones[index].name ,1 );         
           
         //ustaw strefe na aktywną..
           this->alerm_zones[index].active = 0;
           this->alerm_zones[index].on_type = -1;
           this->alerm_zones[index].on_ih_alarm_device_id = -1;                                
           this->alerm_zones[index].toSave = true;
           this->sendInterrrupt(IH_MODULE_FUNCTION_ALARM_DEZACTIVE,   this->alerm_zones[index].id );
          
          int countPortOut = 0;         
          ih_driver_port_function functionListOut[20];   
          
          int sizeS = 0;
          ih_driver_port_function* tmpS = this->getAppHandle()->getDriverPortFunction(&sizeS);
          
          for(int x=0; x<sizeS; x++)
          {
              if(tmpS[x].resource_id == functionList.resource_id &&  //znajdz porty wyjscia... do aktywacji alarmu..
                      (
                         (tmpS[x].const_function_symbol==IH_MODULE_FUNCTION_ALARM_OUT_DEZACTIVE )                                   
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
         
             isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id , IH_SET_PORT_CLICK,-1,1000 * 5,-1  );       
                        
             
             if(isOk)
             {
                 //
             }
              
                   
          }
          
          this->noticeSpeaker( 1);
       
          const char* response = "ZONE UNLOCK"; 
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;   
    
}


//-----------------------------------------------------------------------------

bool app_module_alarm::goDezactiveAlarmZoneAll(gxh_message* inMsg,gxh_message* message_ret) 
{  
      
    
     ih_driver_port_function functionList;
     memcpy(&functionList,inMsg->dataStruct, sizeof(ih_driver_port_function) );
     
     
           
       for(int index=0; index<this->alarm_zones_count; index++)
          {
                    this->getAppHandle()->show_log_module("app_module_alarm","goDezactiveAlarmZoneAll",this->alerm_zones[index].name ,1 );         

                    
                    this->wyzerujCzasy(this->alerm_zones[index].id );

                    //ustaw strefe na aktywną..
                    this->alerm_zones[index].active = 0;
                    this->alerm_zones[index].on_type = -1;
                    this->alerm_zones[index].on_ih_alarm_device_id = -1;    
                    this->alerm_zones[index].toSave = true;
                    this->sendInterrrupt(IH_MODULE_FUNCTION_ALARM_DEZACTIVE,   this->alerm_zones[index].id );
                    
                     int countPortOut = 0;         
                     ih_driver_port_function functionListOut[20];   

                     int sizeS = 0;
                     ih_driver_port_function* tmpS = this->getAppHandle()->getDriverPortFunction(&sizeS);

                     for(int x=0; x<sizeS; x++)
                     {
                         if(tmpS[x].resource_id == this->alerm_zones[index].id &&  //znajdz porty wyjscia... do aktywacji alarmu..
                                 (
                                    (tmpS[x].const_function_symbol==IH_MODULE_FUNCTION_ALARM_OUT_DEZACTIVE )                                   
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

                        isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id , IH_SET_PORT_CLICK,-1,1000 * 5,-1  );       


                        if(isOk)
                        {
                            //
                        }


                     }
          }
                     
           
          this->noticeSpeaker( 1);
     
          const char* response = "ALL ZONE UNLOCK"; 
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;   
    
}



//-----------------------------------------------------------------------------

bool app_module_alarm::onSpeaker(gxh_message* inMsg,gxh_message* message_ret) 
{  
      
    
     ih_driver_port_function functionList;
     memcpy(&functionList,inMsg->dataStruct, sizeof(ih_driver_port_function) );
     


      int index = -1;
     
     for(int x=0; x<this->alarm_devices_count; x++)
     {
         if(this->alerm_devices[x].id == functionList.resource_id) { index = x; break; }
     }
     
     if( index == -1) 
     {
          const char* response = "SPEAKER NOT FOUND"; 
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;
     }
     
     
      this->getAppHandle()->show_log_module("app_module_alarm","onSpeaker",this->alerm_devices[index].name ,1 );         
             
                  
              
      const char* response = "SOUND ERROR"; 
      
                     int countPortOut = 0;         
                     ih_driver_port_function functionListOut[20];   

                     int sizeS = 0;
                     ih_driver_port_function* tmpS = this->getAppHandle()->getDriverPortFunction(&sizeS);

                     for(int x=0; x<sizeS; x++)
                     {
                         if(tmpS[x].resource_id == this->alerm_devices[index].id &&  //znajdz porty wyjscia... do aktywacji alarmu..
                                 (
                                    (tmpS[x].const_function_symbol==IH_MODULE_FUNCTION_ALARM_SPEAKER_OUT )                                   
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

                        isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id , IH_SET_PORT_ON ,-1,-1,-1  );       


                        if(isOk)
                        {
                            // this->sendInterrrupt(IH_MODULE_FUNCTION_ALARM_DEZACTIVE,   this->alerm_zones[index].id );
                           response = "SOUND ON"; 
                        }


                     }
                 
                     
    
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;   
    
}




//----------------------------------------------------------------------------

bool app_module_alarm::offSpeaker(gxh_message* inMsg,gxh_message* message_ret) 
{  
      
    
     ih_driver_port_function functionList;
     memcpy(&functionList,inMsg->dataStruct, sizeof(ih_driver_port_function) );
     


      int index = -1;
     
     for(int x=0; x<this->alarm_devices_count; x++)
     {
         if(this->alerm_devices[x].id == functionList.resource_id) { index = x; break; }
     }
     
     if( index == -1) 
     {
          const char* response = "SPEAKER NOT FOUND"; 
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;
     }
     
     
      this->getAppHandle()->show_log_module("app_module_alarm","offSpeaker",this->alerm_devices[index].name ,1 );         
             
        
      const char* response = "SOUND ERROR"; 
                               
                     int countPortOut = 0;         
                     ih_driver_port_function functionListOut[20];   

                     int sizeS = 0;
                     ih_driver_port_function* tmpS = this->getAppHandle()->getDriverPortFunction(&sizeS);

                     for(int x=0; x<sizeS; x++)
                     {
                         if(tmpS[x].resource_id == this->alerm_devices[index].id &&  //znajdz porty wyjscia... do aktywacji alarmu..
                                 (
                                    (tmpS[x].const_function_symbol==IH_MODULE_FUNCTION_ALARM_SPEAKER_OUT )                                   
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

                        isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id , IH_SET_PORT_OFF ,-1,-1,-1  );       


                        if(isOk)
                        {
                            // this->sendInterrrupt(IH_MODULE_FUNCTION_ALARM_DEZACTIVE,   this->alerm_zones[index].id );
                            response = "SOUND OFF"; 
                        }


                     }
                 
                     
       
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;   
    
}




//-----------------------------------------------------------------------------

bool app_module_alarm::moveSensor(gxh_message* inMsg,gxh_message* message_ret)
{
     ih_driver_port_function functionList;
     memcpy(&functionList,inMsg->dataStruct, sizeof(ih_driver_port_function) );
               
     
     bool off = false;
     if( functionList.port_typ== IH_SWITCH_BISTABILE_OFF_NO ) off = true; //znikneło połączenie z czujką
     if( functionList.port_typ== IH_SWITCH_MONOSTABILE_ON_OFF_CLICK_NO ) off = true; //znikneło połączenie z czujką
     if( functionList.port_typ== IH_SWITCH_MONOSTABILE_ON_OFF_DOUBLE_CLICK_NO ) off = true; //znikneło połączenie z czujką


      int index = -1;
      int indexZone = -1;
      int ih_alarm_zone_id = -1;
     
      
     for(int x=0; x<this->alarm_devices_count; x++)
     {
         if(this->alerm_devices[x].id == functionList.resource_id) 
         {
             index = x;
             ih_alarm_zone_id  = this->alerm_devices[x].ih_alarm_zone_id;
             break; }
     }
     
     if( index == -1) 
     {
          const char* response = "MOVE SENSOR NOT FOUND"; 
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;
     }
     
       
     for(int x=0; x<this->alarm_zones_count; x++)
     {    
         if(this->alerm_zones[x].id == ih_alarm_zone_id) 
         {
             indexZone = x;            
             break; }
     }
     
     if( indexZone == -1) 
     {
          const char* response = "ALARM ZONE FOR SENSOR NOT FOUND"; 
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;
     }
          
      
     //czasy.........................
     //czasy.........................
     //czasy.........................
     struct timeval tp;
     gettimeofday(&tp, NULL);
     unsigned long  useconds = tp.tv_sec * 1000 + tp.tv_usec / 1000;// milisekundy..
     
     unsigned long diffUsec = -1;
     unsigned long diffSec  = -1;
     unsigned long secFromFirst  = 0; //czes który upłynał od pierwszego wykrycaia
     
     if(  this->alerm_devices[index].timestampLastChange > 0 && useconds > this->alerm_devices[index].timestampLastChange )
     {
         diffUsec = useconds - this->alerm_devices[index].timestampLastChange;
         if(diffUsec > 0)  diffSec = diffUsec / 1000; //zamień na sekundy....
     }
        
     
     //pierwsze wzbudenie czujki.. lub ostatnie wzbudzenie było 15min temu...
     //if( this->alerm_devices[index].timestampFirst == 0 || diffSec > 900)
     if( this->alerm_devices[index].timestampFirst == 0 && off)
     {
         this->alerm_devices[index].timestampFirst = useconds;
         this->alerm_devices[index].countMs =useconds;            
         
         std::cout<<"\n\nPonownie odczanie dla tej czujki!!!!!! ZEROWANIE CZASU STARTU !\n\n";
     }
  
     if(!off)     this->alerm_devices[index].countMs = 0; //nie licz.. bo juz poscila..
     
     
     this->alerm_devices[index].timestampLastChange = useconds;
     
     secFromFirst = useconds -  this->alerm_devices[index].timestampFirst;
     secFromFirst /= 1000; //na sekundy...
     
     std::cout<<"\n\nCzas od pierwszego wykrycia "<<secFromFirst<<" sec.\n\n";
     //czasy......koniec...
     //czasy......koniec...
     //czasy......koniec...
      
      
      
      //przerwanie naruszenie strefy... 
      this->sendInterrrupt(IH_MODULE_FUNCTION_ALARM_ALERT_ZONE_P,   this->alerm_zones[indexZone].id );
         //przerwanie naruszenia czujnika...
      this->sendInterrrupt(IH_MODULE_FUNCTION_ALARM_ALERT_SENSOR,   this->alerm_devices[index].id );
      
     std::string info = "Naruszenie czujnika ruchu " ;
     info.append( this->alerm_devices[index].name);
     info.append(" w strefie ");
     info.append( this->alerm_zones[indexZone].name  );
     this->getAppHandle()->show_log_module("app_module_alarm","noticeSpeaker", info.c_str() ,0 );   
      
     
     
      //alarm osczegawczy - strefy są uzbrojone a to jest pierwsze wzbudzenie czujki...
      if( secFromFirst==0 && (this->alerm_zones[indexZone].active=='1' ||  this->alerm_zones[indexZone].active==1  ||  this->alerm_zones[indexZone].active=='2' ||  this->alerm_zones[indexZone].active==2 )   )
      {
            
          std::string info2 = "Alerm  osczegawczy - Naruszenie czujnika ruchu " ;
          info2.append( this->alerm_devices[index].name);
          info2.append(" w uzbrojonej strefie ");
          info2.append( this->alerm_zones[indexZone].name  );
          this->getAppHandle()->show_log_module("app_module_alarm","noticeSpeaker",info2.c_str() ,0 );   
          
          //wyslij informacje o naruszeniu alarmu...
       //   this->sendInterrrupt(IH_MODULE_FUNCTION_ALARM_ON,    this->alerm_zones[indexZone].id );
          //naruszenie sterfy uzbrojonej...
        //  this->sendInterrrupt(IH_MODULE_FUNCTION_ALARM_ALERT_ZONE_PSEC,    this->alerm_zones[indexZone].id );
          //uruchom syrene alarmową..
          this->noticeSpeaker( 1 );  //krótkie powiadomienie....  
          
         //krótki alarm ostrzegawczy... 
         this->sendInterrrupt(IH_MODULE_FUNCTION_ALARM_N_SHORT,   this->alerm_zones[indexZone].id );
         this->sendInterrrupt(IH_MODULE_FUNCTION_ALARM_NOBJ_SHORT,  1);//dom ma id 1
          
        //  time_t rawtime;
        //  struct tm * timeinfo;
         // char buffer[80];

        //  time (&rawtime);
         // timeinfo = localtime(&rawtime);

       //   strftime(buffer,sizeof(buffer),"%Y-%m-%d %H:%M:%S",timeinfo);
       //   std::string str(buffer);
          
        //  this->alerm_zones[indexZone].on_type = this->alerm_zones[indexZone].active;
         // this->alerm_zones[indexZone].on_ih_alarm_device_id = this->alerm_devices[index].id;
         // memcpy(this->alerm_zones[indexZone].on_time, str.c_str(), strlen(str.c_str()) );   
        //  this->alerm_zones[indexZone].toSave = true;
          
            const char* response = "HELLO!"; 
            gxh_message msg;
            msg.gxh_handle = inMsg->gxh_handle;
            memcpy(msg.dataStruct, response, strlen(response) );
            msg.sizeDataStruct = sizeof(response) ;
            msg.system_code = GXH_MESSAGE_SUCCESS;

            memcpy(message_ret, &msg, sizeof(gxh_message));            
            return true; 
      }
     
     
      //czy strefa jest uzbrojona.???? - alarm głośny....
      //wykroty ruch + strefa jest uzbrojona... = włącz alarm...... jeśli nie był wcześniej uruchomiony to go uruchamiamy..
      if( secFromFirst>5  &&     this->alerm_zones[indexZone].on_type!=this->alerm_zones[indexZone].active && (this->alerm_zones[indexZone].active=='1' ||  this->alerm_zones[indexZone].active==1)  )
      {
            
          std::string info2 = "Alerm - Naruszenie czujnika ruchu " ;
          info2.append( this->alerm_devices[index].name);
          info2.append(" w uzbrojonej strefie ");
          info2.append( this->alerm_zones[indexZone].name  );
          this->getAppHandle()->show_log_module("app_module_alarm","noticeSpeaker",info2.c_str() ,0 );   
          
          //wyslij informacje o naruszeniu alarmu...
          this->sendInterrrupt(IH_MODULE_FUNCTION_ALARM_ON,    this->alerm_zones[indexZone].id );
          //naruszenie sterfy uzbrojonej...
          this->sendInterrrupt(IH_MODULE_FUNCTION_ALARM_ALERT_ZONE_PSEC,    this->alerm_zones[indexZone].id );
          
          //uruchomiono alarm w domu
          this->sendInterrrupt(IH_MODULE_FUNCTION_ALARM_NOBJ,    1 );
          //uruchom syrene alarmową..
          this->noticeSpeaker( 0);  //syrena wyje cały czas    
          
          
          time_t rawtime;
          struct tm * timeinfo;
          char buffer[80];

          time (&rawtime);
          timeinfo = localtime(&rawtime);

          strftime(buffer,sizeof(buffer),"%Y-%m-%d %H:%M:%S",timeinfo);
          std::string str(buffer);
          
          this->alerm_zones[indexZone].on_type = this->alerm_zones[indexZone].active;
          this->alerm_zones[indexZone].on_ih_alarm_device_id = this->alerm_devices[index].id;
          memcpy(this->alerm_zones[indexZone].on_time, str.c_str(), strlen(str.c_str()) );   
          this->alerm_zones[indexZone].toSave = true;
      }      
     
     
     //cichy alarm bez syreny... jeśli nie był uruchomiony wcześniej uruchamiamy...
      if( secFromFirst>5  &&   this->alerm_zones[indexZone].on_type!=this->alerm_zones[indexZone].active && (this->alerm_zones[indexZone].active=='2' ||  this->alerm_zones[indexZone].active==2)  )
      {
            
          std::string info2 = "Alerm - Naruszenie czujnika ruchu " ;
          info2.append( this->alerm_devices[index].name);
          info2.append(" w uzbrojonej strefie ");
          info2.append( this->alerm_zones[indexZone].name  );
          this->getAppHandle()->show_log_module("app_module_alarm","noticeSpeaker",info2.c_str() ,0 );   
          
      
          //wyslij informacje o naruszeniu alarmu...
          this->sendInterrrupt(IH_MODULE_FUNCTION_ALARM_ON,    this->alerm_zones[indexZone].id );
          
          //naruszenie sterfy uzbrojonej...
          this->sendInterrrupt(IH_MODULE_FUNCTION_ALARM_ALERT_ZONE_PSEC,    this->alerm_zones[indexZone].id );
          
            //uruchomiono alarm w domu
          this->sendInterrrupt(IH_MODULE_FUNCTION_ALARM_NOBJ,    1 );
          
          time_t rawtime;
          struct tm * timeinfo;
          char buffer[80];

          time (&rawtime);
          timeinfo = localtime(&rawtime);

          strftime(buffer,sizeof(buffer),"%Y-%m-%d %H:%M:%S",timeinfo);
          std::string str(buffer);
          
          this->alerm_zones[indexZone].on_type = this->alerm_zones[indexZone].active;
          this->alerm_zones[indexZone].on_ih_alarm_device_id = this->alerm_devices[index].id;
          memcpy(this->alerm_zones[indexZone].on_time, str.c_str(), strlen(str.c_str()) );
          this->alerm_zones[indexZone].toSave = true;
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

//-----------------------------------------------------------------------------

bool app_module_alarm::smokeSensor(gxh_message* inMsg,gxh_message* message_ret)
{
     ih_driver_port_function functionList;
     memcpy(&functionList,inMsg->dataStruct, sizeof(ih_driver_port_function) );
     


      int index = -1;
      int indexZone = -1;
      int ih_alarm_zone_id = -1;
     
      
     for(int x=0; x<this->alarm_devices_count; x++)
     {
         if(this->alerm_devices[x].id == functionList.resource_id) 
         {
             index = x;
             ih_alarm_zone_id  = this->alerm_devices[x].ih_alarm_zone_id;
             break; }
     }
     
     if( index == -1) 
     {
          const char* response = "SMOKE SENSOR NOT FOUND"; 
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;
     }
     
      
     for(int x=0; x<this->alarm_zones_count; x++)
     {
         if(this->alerm_zones[x].id == ih_alarm_zone_id) 
         {
             indexZone = x;            
             break; }
     }
     
     if( indexZone == -1) 
     {
          const char* response = "ALARM ZONE FOR SENSOR NOT FOUND"; 
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;
     }
            
          
      
     struct timeval tp;
     gettimeofday(&tp, NULL);
     unsigned long  useconds = tp.tv_sec * 1000 + tp.tv_usec / 1000;// milisekundy..
     
     this->alerm_devices[index].timestampLastChange = useconds;  
      
      
      std::string info = "Naruszenie czujnika dymu " ;
     info.append( this->alerm_devices[index].name);
     info.append(" w strefie ");
     info.append( this->alerm_zones[indexZone].name  );
     this->getAppHandle()->show_log_module("app_module_alarm","smokeSensor", info.c_str() ,0 );   
      
     
 
         //przerwanie wykrytu dym
      this->sendInterrrupt(IH_MODULE_FUNCTION_ALARM_SMOKE_P,   this->alerm_devices[index].id );
      this->sendInterrrupt(IH_MODULE_FUNCTION_ALARM_N_SMOKE_Z,   this->alerm_zones[indexZone].id );
     
      //czy strefa jest uzbrojona.????
      //wykroty ruch + strefa jest uzbrojona... = włącz alarm......                  
      if( this->alerm_zones[indexZone].active=='1' ||  this->alerm_zones[indexZone].active==1 || this->alerm_zones[indexZone].active=='2' ||  this->alerm_zones[indexZone].active==2   )
      {
      
            std::string info2 = "Naruszenie czujnika dymu " ;
            info2.append( this->alerm_devices[index].name);
            info2.append(" w uzbrojonej strefie ");
            info2.append( this->alerm_zones[indexZone].name  );
            this->getAppHandle()->show_log_module("app_module_alarm","smokeSensor", info2.c_str() ,0 );   
          
  
             this->sendInterrrupt(IH_MODULE_FUNCTION_ALARM_N_SMOKE_ZSEC,   this->alerm_zones[indexZone].id );
      //   this->noticeSpeaker( 400);  //syrena wyje cały czas
          
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

//-----------------------------------------------------------------------------

bool app_module_alarm::gasSensor(gxh_message* inMsg,gxh_message* message_ret)
{
     ih_driver_port_function functionList;
     memcpy(&functionList,inMsg->dataStruct, sizeof(ih_driver_port_function) );
     


      int index = -1;
      int indexZone = -1;
      int ih_alarm_zone_id = -1;
     
      
     for(int x=0; x<this->alarm_devices_count; x++)
     {
         if(this->alerm_devices[x].id == functionList.resource_id) 
         {
             index = x;
             ih_alarm_zone_id  = this->alerm_devices[x].ih_alarm_zone_id;
             break; }
     }
     
     if( index == -1) 
     {
          const char* response = "GAS SENSOR NOT FOUND"; 
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;
     }
     
      
     for(int x=0; x<this->alarm_zones_count; x++)
     {
         if(this->alerm_zones[x].id == ih_alarm_zone_id) 
         {
             indexZone = x;            
             break; }
     }
     
     if( indexZone == -1) 
     {
          const char* response = "ALARM ZONE FOR SENSOR NOT FOUND"; 
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;
     }
            
         
     struct timeval tp;
     gettimeofday(&tp, NULL);
     unsigned long  useconds = tp.tv_sec * 1000 + tp.tv_usec / 1000;// milisekundy..
     
     this->alerm_devices[index].timestampLastChange = useconds;

      
     std::string info = "Naruszenie czujnika gazu " ;
     info.append( this->alerm_devices[index].name);
     info.append(" w strefie ");
     info.append( this->alerm_zones[indexZone].name  );
     this->getAppHandle()->show_log_module("app_module_alarm","gasSensor", info.c_str() ,0 );
      
     
 
     //przerwanie wykryto gaz...
     this->sendInterrrupt(IH_MODULE_FUNCTION_ALARM_GAS_P,   this->alerm_devices[index].id );
     this->sendInterrrupt(IH_MODULE_FUNCTION_ALARM_N_GAS_Z,  this->alerm_zones[indexZone].id );
     
     
      //czy strefa jest uzbrojona.????
      //wykroty ruch + strefa jest uzbrojona... = włącz alarm......
      if( this->alerm_zones[indexZone].active=='1' ||  this->alerm_zones[indexZone].active==1  )
      {
      
            std::string info2 = "Naruszenie czujnika gazu " ;
            info2.append( this->alerm_devices[index].name);
            info2.append(" w uzbrojonej strefie ");
            info2.append( this->alerm_zones[indexZone].name  );
            this->getAppHandle()->show_log_module("app_module_alarm","gasSensor", info2.c_str() ,0 );
    
            this->sendInterrrupt(IH_MODULE_FUNCTION_ALARM_N_GAS_ZSEC,  this->alerm_zones[indexZone].id );
          
         //this->noticeSpeaker( 400);  //syrena wyje cały czas
          
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

//-----------------------------------------------------------------------------


bool app_module_alarm::noticeSpeaker( int num) 
{  
      
     
     int index = -1;
     
     for(int x=0; x<this->alarm_devices_count; x++)
     {
         if( this->alerm_devices[x].type[0]=='S' ) { index = x; break; }
     }
     
     if( index == -1) 
     {    
          this->getAppHandle()->show_log_module("app_module_alarm","noticeSpeaker","Not found speaker!" ,1 );      
          return false;
     }
     
     
      this->getAppHandle()->show_log_module("app_module_alarm","noticeSpeaker",this->alerm_devices[index].name ,1 );         
             
                  
                               
                     int countPortOut = 0;         
                     ih_driver_port_function functionListOut[20];   

                     int sizeS = 0;
                     ih_driver_port_function* tmpS = this->getAppHandle()->getDriverPortFunction(&sizeS);

                     for(int x=0; x<sizeS; x++)
                     {
                         if(tmpS[x].resource_id == this->alerm_devices[index].id &&  //znajdz porty wyjscia... do aktywacji alarmu..
                                 (
                                    (tmpS[x].const_function_symbol==IH_MODULE_FUNCTION_ALARM_SPEAKER_OUT )                                   
                                 )

                                 )
                         {
                             memcpy( &functionListOut[countPortOut], &tmpS[x], sizeof(ih_driver_port_function) );
                             countPortOut++;
                         }
                     }
                     
                     
                     if(countPortOut <= 0)
                     {
                           this->getAppHandle()->show_log_module("app_module_alarm","noticeSpeaker","Not found out port!" ,1 );         
                     }
                     
                     
                     if( num == 0) //syrena wyje cały czas...
                     {                              
                                   for(int p=0; p< countPortOut; p++)
                                   {

                                      bool isOk = false;
                                      isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id , IH_SET_PORT_ON ,-1,-1,-1  );       
                                      if(isOk)
                                      {
                                          // this->sendInterrrupt(IH_MODULE_FUNCTION_ALARM_DEZACTIVE,   this->alerm_zones[index].id );
                                      }
                                   }
                                   
                            return true;   
                     }

                  
                     if( num < 100 && num > 0) //pojedyncze sygnały.......
                     {
                                int loop = num;
                                if(loop < 1) loop = 1;
                                if(loop > 5) loop = 5;

                                for(int kk=0;kk<loop; kk++)
                                {
                                   for(int p=0; p< countPortOut; p++)
                                   {

                                      bool isOk = false;
                                      isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id , IH_SET_PORT_CLICK ,500,-1,-1  );       
                                      if(isOk)
                                      {
                                          // this->sendInterrrupt(IH_MODULE_FUNCTION_ALARM_DEZACTIVE,   this->alerm_zones[index].id );
                                      }
                                   }


                                   if(loop >1) sleep(1);
                                }
                                
                                return true;
                     }
                     
                     
                     if( num < 0) //syrena wyje prez num min
                     {                              
                                   for(int p=0; p< countPortOut; p++)
                                   {

                                      bool isOk = false;
                                      isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id , IH_SET_PORT_ON ,-1,-1,-1  );       
                                      if(isOk)
                                      {
                                          // this->sendInterrrupt(IH_MODULE_FUNCTION_ALARM_DEZACTIVE,   this->alerm_zones[index].id );
                                      }
                                   }
                                   
                                   int xMin = -1 * num;
                                   sleep(60*xMin);
                                   
                                   for(int p=0; p< countPortOut; p++)
                                   {

                                      bool isOk = false;
                                      isOk = this->setPortStatus( functionListOut[p].ih_driver_port_id , IH_SET_PORT_OFF ,-1,-1,-1  );       
                                      if(isOk)
                                      {
                                          // this->sendInterrrupt(IH_MODULE_FUNCTION_ALARM_DEZACTIVE,   this->alerm_zones[index].id );
                                      }
                                   }
                                   
                         return true;
                     }
                     
                     
                   
                 
                     
                  
          return true;   
    
}



//-----------------------------------------------------------------------------
