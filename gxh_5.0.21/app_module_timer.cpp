/* 
 * File:   app_module_timer.cpp
 * Author: Blazej Kita
 * 
 * Created on 02 luty 2021, 14:31
 */

#include "app_module_timer.h"
#include "h_sunset.h"


 
app_module* get_module(gxh_scope* appHomeHandle )
{
    return new app_module_timer(appHomeHandle); 
};
 
//-------------------------------------------------------------------------------
  
app_module_timer::app_module_timer(gxh_scope* appHomeHandle) : app_module(appHomeHandle) 
{           
  
    this->getAppHandle()->show_log_module("app_module_timer","app_module_timer","start",2 );         
    
    
    
    this->timer_count = 0;
    this->getAppHandle()->destroyResource(RES_VECTOR_TIMER);
    
    sqlite3* dbSqLite;               
     
     
    std::string baza = this->getAppHandle()->getDB();  
 
     
    int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
    if( r != SQLITE_OK )
    {
          this->getAppHandle()->show_error("app_module_timer","app_module_timer","Cannot open sqlLite 1",0);            
    }
    else 
     {
    
        std::string query="SELECT ih_timer.id, ih_timer.name, ih_timer.sec, ih_timer.max_sec, ih_timer.updated_ts, ih_timer.active   FROM ih_timer WHERE ih_timer.is_delete='0'  ; ";                
        this->getAppHandle()->show_log_data_base("app_module_timer","app_module_timer", query.c_str() ,6);  
         
        sqlite3_stmt* stmt; 
    
        const char* queryChar = query.c_str();          
        if(sqlite3_prepare(dbSqLite , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
        {
          this->getAppHandle()->show_error("app_module_timer","app_module_timer", sqlite3_errstr( sqlite3_errcode(dbSqLite  ) ),4);          
          sqlite3_finalize(stmt);                 
        }else
        {    
           bool done = false;       
           while (!done)               
           { 
               int res = sqlite3_step (stmt);        
               if(res ==  SQLITE_ROW)
               {                       
                  this->timers[this->timer_count].id                 = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 0) ) );
                  
                  const char* tmp1                                = (const char*)sqlite3_column_text(stmt, 1 );                  
                  memset(this->timers[this->timer_count].name, 0, 100 );
                  if(tmp1 != NULL) memcpy(this->timers[this->timer_count].name, tmp1, strlen(tmp1));
                                   
                  
                  this->timers[this->timer_count].sec    = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 2) ) ); 
                  this->timers[this->timer_count].max_sec                = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 3) ) );
                  
                  if((const char*)sqlite3_column_text(stmt, 4) != NULL)
                  {
                     this->timers[this->timer_count].updated_ts                = gxh_StringToUnsignedLong( std::string( (const char*)sqlite3_column_text(stmt, 4) ) );
                  }else{
                         this->timers[this->timer_count].updated_ts = 0;
                  }
                  
                  this->timers[this->timer_count].active                = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 5) ) );
                  
                  this->timers[this->timer_count].toSave = false;
                  this->timer_count++;
                  
                // this->getAppHandle()->show_log_module("app_module_timer","app_module_timer", this->timers[this->timer_count-1].name ,5 );
                  
               }               
               
              if(res == SQLITE_DONE)done = true;                                     
            }
          sqlite3_finalize(stmt); 
         }
      }
    
    
     this->getAppHandle()->registerResource( this->timers ,this->timer_count, RES_VECTOR_TIMER );
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
     
     
  
     sqlite3_close_v2(dbSqLite);
     
}

//------------------------------------------------------------------------------

app_module_timer::~app_module_timer() 
{     
    this->getAppHandle()->show_log_module("app_module_timer","app_module_timer","stop",2 );
    
 
}


//-------------------------------------------------------+-----------------------

void app_module_timer::onClick(int ih_driver_port_id, int listener_status)
{
}

//-------------------------------------------------------+-----------------------

void app_module_timer::forceCancel()
{  
      this->getAppHandle()->show_log_module("app_module_timer","app_module_timer","forceCancel",2);
      
       
      this->forceSynchronize();
      this->getAppHandle()->destroyResource(RES_VECTOR_TIMER);
}

//-------------------------------------------------------+-----------------------

void app_module_timer::forceSynchronize()
{  
     this->getAppHandle()->show_log_module("app_module_timer","forceSynchronize","start",20);          
     
 
    unsigned int countT = 0;
    ih_timer* rows   = (ih_timer*)this->getAppHandle()->getResource(RES_VECTOR_TIMER, &countT); 
    
    for(unsigned int k=0; k<countT; k++)
    {
        if(rows[k].toSave == false) continue;
        
        
             std::string msg = "Update status for ";
             msg.append( rows[k].name);             
             this->getAppHandle()->show_log_module("app_module_timer","forceSynchronize",msg.c_str(),16); 
             
             //----
             sqlite3* dbSqLite;  
             
             std::string baza = this->getAppHandle()->getDB();//this->getAppHandle()->getRunPath();
             //baza.append("../gxh_base.db");
     
             int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );      //tu coś nie gra..                            
             if( r != SQLITE_OK )
             {
               this->getAppHandle()->show_error("app_module_timer","forceSynchronize","Cannot open sqlLite 1",0);                 
             }else
             { 
                 
                 std::string strSec =  gxh_IntToString(rows[k].sec);
                 std::string upTs =  gxh_IntToString(rows[k].updated_ts);
                
       
                 std::string query="";
                 query.append("UPDATE ih_timer SET updated_ts=DATETIME('now','localtime'), sec='");
                 query.append( strSec  );
                 query.append("', updated_ts='");
                 query.append(upTs);
                 query.append("', active='");
                 query.append( gxh_IntToString(rows[k].active) );
                 query.append("' WHERE is_delete='0' AND id='");
                 query.append( gxh_IntToString(rows[k].id) );
                 query.append("'; ");
                   
                 
           
                 char *zErrMsg = 0;     
                 int rc = sqlite3_exec( dbSqLite , query.c_str(), 0, 0, &zErrMsg);
                 if( rc != SQLITE_OK )
                     {        
                        std::cout<<query.c_str()<<"\n";          
                        this->getAppHandle()->show_error("app_module_timer","forceSynchronize", (const char*)zErrMsg,0);
                        sqlite3_free(zErrMsg);                                
                        sqlite3_close_v2(dbSqLite);   
                     }else
                     {  
                        rows[k].toSave = false;
                        sqlite3_close_v2(dbSqLite);  
                     }                                                                    
             }
        
        
    }
     
     
     this->getAppHandle()->show_log_module("app_module_timer","forceSynchronize","stop",20);          
}

//-------------------------------------------------------+-----------------------


bool app_module_timer::execute(gxh_message* inMsg,gxh_message* message_ret)  
{ 
    
          
      gxh_message message;
      memcpy(&message, inMsg, sizeof(gxh_message));
    
     
   
       if(message.func_number == IH_MODULE_FUNCTION_TIMER_ZERO )    //wyzeruj Timer 
           {                 
               return this->goZeroTime(&message, message_ret);
           }            
      
     
       if(message.func_number == IH_MODULE_FUNCTION_TIMER_UP )    // zwiększ wartość timera.. i odśwież inne elementy
           {                
               return this->goRefresh(&message, message_ret);
           }
      
      
        if(message.func_number == IH_MODULE_FUNCTION_TIMER_START ) // zacznij odliczanie + ustaw 00:00 startowe..
           {                
               return this->changeStatus(&message, message_ret,1);
           }
      
       if(message.func_number == IH_MODULE_FUNCTION_TIMER_STOP )  // zatrzymaj odliczanie + ustaw 00:00 startowe..
           {                
               return this->changeStatus(&message, message_ret,0);
           }
      
         
      
       
      
     
          const char* response = "TIMER MODULE"; 
          gxh_message msg;
          msg.gxh_handle = message.gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;
          
}

//-------------------------------------------------------+-----------------------

 bool app_module_timer::changeStatus(gxh_message* inMsg,gxh_message* message, char status)
 {
     unsigned int countT = 0;
     ih_timer* rows   = (ih_timer*)this->getAppHandle()->getResource(RES_VECTOR_TIMER, &countT); 
    
     ih_driver_port_function functionList;
     memcpy(&functionList,inMsg->dataStruct, sizeof(ih_driver_port_function) );
    
    for(unsigned int k=0; k<countT; k++)
    {
       
         
         if(rows[k].id == functionList.resource_id)
         {
               this->getAppHandle()->show_log_module("app_module_timer","goZeroTime", rows[k].name ,8);          
              
               if(status == 1)
               {
                  rows[k].sec = 0;    
                  rows[k].active = 1; 
               }
               
               if(status == 0)
               {
                  rows[k].sec = 0;    
                  rows[k].active = 0;    
               }
               
               
                rows[k].updated_ts = gxh_getSecondsFrom1900();
                rows[k].toSave = true;
              
                const char* response = "OK"; 
                gxh_message msg;
                msg.gxh_handle = inMsg->gxh_handle;
                memcpy(msg.dataStruct, response, strlen(response) );
                msg.sizeDataStruct = sizeof(response) ;
                msg.system_code = GXH_MESSAGE_SUCCESS;

                memcpy(message, &msg, sizeof(gxh_message));            
                return true;
         }         
         
    }
     
     
          const char* response = "TIMER NOT FOUND"; 
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message, &msg, sizeof(gxh_message));            
          return true;
    
 }

//-------------------------------------------------------+-----------------------


bool app_module_timer::goRefresh(gxh_message* inMsg,gxh_message* message)
{
    
    unsigned int countT = 0;
    ih_timer* rows   = (ih_timer*)this->getAppHandle()->getResource(RES_VECTOR_TIMER, &countT); 
    
    for(unsigned int k=0; k<countT; k++)
    {
         if( rows[k].active != 1) continue;
        
        
         this->getAppHandle()->show_log_module("app_module_timer","goUpdateTime", rows[k].name ,8);          
         
         
         int max_sec = rows[k].max_sec;
         int sec = rows[k].sec;
         unsigned long  updated_ts = rows[k].updated_ts;
         
         if(updated_ts == 0) //pierwszy start...
         {
             rows[k].sec = 1;
             rows[k].updated_ts = gxh_getSecondsFrom1900();
             rows[k].toSave = true;
             continue;
         }                                    
         //----------------------
       
         if(updated_ts > 0) //....
         {
             unsigned long tsNow = gxh_getSecondsFrom1900();
             unsigned long tsDiff = tsNow - updated_ts;
             
             if(tsDiff > 0)
             {
                int newVal = sec + tsDiff;
                
                if(tsDiff >= 10) //coś się przywiesiło.. zerujemy timer...
                {
                    newVal = 0;
                }
                
                if(newVal >= max_sec)
                {
                    this->sendInterrrupt(IH_MODULE_FUNCTION_TIMER_INTERRUPT_MAX, rows[k].id);                    
                    newVal = 0;
                }
                 
                rows[k].sec = newVal;
                rows[k].updated_ts = gxh_getSecondsFrom1900();
                rows[k].toSave = true;
                continue;
             }
         }
         
         //----------------------
         
         
    }
 
   
    
    
    
    //wylicz wschodzy i zachody słońca..
    unsigned int countH = 0;
    ih_home* home   = (ih_home*)this->getAppHandle()->getResource(RES_VECTOR_HOME, &countH);  
    
    if(countH  == 1)
    {
        std::string lat = std::string( home->latitude ) ;
        std::string lng = std::string( home->longitude ) ;
        
        double dLat = gxh_StringToDouble(lat);
        double dLng = gxh_StringToDouble(lng);
        
        time_t theTime = time(NULL);
        struct tm *aTime = localtime(&theTime);
        
        int day = aTime->tm_mday;
        int month = aTime->tm_mon + 1; // Month is 0 - 11, add 1 to get a jan-dec 1-12 concept
        int year = aTime->tm_year + 1900; // Year is # years since 1900
        
        int ts = aTime->tm_hour * 60;
        ts += aTime->tm_min ;
         
   
        
        
       // std::cout<<"\n\nLat "<<lat<<" Lng "<<lng<<" || "<<dLat<<" / "<<dLng<<"\n";
        
        SunSet sun;
        
        sun.setPosition(dLat, dLng, 1); //timeZone +1
        //time..
        sun.setCurrentDate(year, month,  day );
        
        
   //     https://github.com/buelowp/sunset/blob/master/examples/particle/example.ino#L40
        
        int sunrise = static_cast<int>(sun.calcSunrise());
        int sunset = static_cast<int>(sun.calcSunset());
        int civilsunrise = static_cast<int>(sun.calcCivilSunrise()); //widać już trochę słońca
        int civilsunset = static_cast<int>(sun.calcCivilSunset());
        int nauticalsunrise = static_cast<int>(sun.calcNauticalSunrise()); //morski wschod slonca, tuż przed wschodem gdy widać dobrze gwiazdy
        int nauticalsunset = static_cast<int>(sun.calcNauticalSunset());
        int astrosunrise = static_cast<int>(sun.calcAstronomicalSunrise()); //astronomiczny wschod slonca - słońce jest już tak wyskoko że nie widać najaśniejszych gwiazd
        int astrosunset = static_cast<int>(sun.calcAstronomicalSunset());
        int customsunrise =static_cast<int>( sun.calcCustomSunrise(90.0)); //standarowy wschod slonca
       // double customsunset = sun.calcCustomSuset(90.0);
        
    
        // std::cout<<"Wschod "<<(sunrise/60)<<":"<<(sunrise%60)<<"   Zachod "<<(sunset/60)<<":"<<(sunset%60)<<" ToActive: "<< (ts/60)<<":"<< (ts%60)<<"\n\n";
        
        
        if( ts == sunrise  )
        {
               this->getAppHandle()->show_log_module("app_module_timer","goRefresh", "Wschód słońca"  ,3);          
               this->sendInterrrupt(363, home->id);  //wschód słońca
        }
        
        if( ts == sunset )
        {
               this->getAppHandle()->show_log_module("app_module_timer","goRefresh", "Zachód słońca"  ,3);   
               this->sendInterrrupt(364, home->id);  //zachód słońca
        }
        
        int sunriseM1 = sunrise - 3600;
        int sunsetM1 = sunset - 3600;
        
        if( ts == sunriseM1 )
        {
               this->getAppHandle()->show_log_module("app_module_timer","goRefresh", "Godzina do wchodu słońca"  ,3);  
               this->sendInterrrupt(366, home->id);  //wschód słońca
        }
        
        if( ts == sunsetM1 )
        {
               this->getAppHandle()->show_log_module("app_module_timer","goRefresh", "Godzina do zachodu słońca"  ,3);  
               this->sendInterrrupt(367, home->id);  //zachód słońca
        }
      
      
        //cywilny
        if( ts == civilsunrise  )
        {
               this->getAppHandle()->show_log_module("app_module_timer","goRefresh", "Wschód słońca - cywilny"  ,3);  
               this->sendInterrrupt(370, home->id);  //wschód słońca 
        }
        
        if( ts == civilsunset   )
        {
               this->getAppHandle()->show_log_module("app_module_timer","goRefresh", "Zachód słońca - cywilny"  ,3); 
               this->sendInterrrupt(371, home->id);  //zachód słońca
        }
        
        
        //zegalrski
        if( ts == nauticalsunrise  )
        {
                 this->getAppHandle()->show_log_module("app_module_timer","goRefresh", "Wschód słońca - żeglarski"  ,3);  
               this->sendInterrrupt(368, home->id);  //wschód słońca 
        }
        
        if( ts == nauticalsunset   )
        {
               this->getAppHandle()->show_log_module("app_module_timer","goRefresh", "Zachód słońca - żeglarski"  ,3); 
               this->sendInterrrupt(369, home->id);  //zachód słońca
        }
        
        
        //astronomiczny
        if( ts == astrosunrise )
        {
               this->getAppHandle()->show_log_module("app_module_timer","goRefresh", "Wschód słońca - astronomiczny"  ,3); 
               this->sendInterrrupt(372, home->id);  //wschód słońca 
        }
        
        if( ts == astrosunset  )
        {
               this->getAppHandle()->show_log_module("app_module_timer","goRefresh", "Zachód słońca - astronomiczny"  ,3); 
               this->sendInterrrupt(373, home->id);  //zachód słońca
        }
        
    }
    
    
    return false;
}

//-------------------------------------------------------+-----------------------


bool app_module_timer::goZeroTime(gxh_message* inMsg,gxh_message* message)
{
    
    unsigned int countT = 0;
    ih_timer* rows   = (ih_timer*)this->getAppHandle()->getResource(RES_VECTOR_TIMER, &countT); 
    
     ih_driver_port_function functionList;
     memcpy(&functionList,inMsg->dataStruct, sizeof(ih_driver_port_function) );
    
    for(unsigned int k=0; k<countT; k++)
    {
       
         
         if(rows[k].id == functionList.resource_id)
         {
            this->getAppHandle()->show_log_module("app_module_timer","goZeroTime", rows[k].name ,8);          
              
              rows[k].sec = 0;
              rows[k].updated_ts = gxh_getSecondsFrom1900();
              rows[k].toSave = true;
              
                const char* response = "OK"; 
                gxh_message msg;
                msg.gxh_handle = inMsg->gxh_handle;
                memcpy(msg.dataStruct, response, strlen(response) );
                msg.sizeDataStruct = sizeof(response) ;
                msg.system_code = GXH_MESSAGE_SUCCESS;

                memcpy(message, &msg, sizeof(gxh_message));            
                return true;
         }         
         
    }
     
     
          const char* response = "TIMER NOT FOUND"; 
          gxh_message msg;
          msg.gxh_handle = inMsg->gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message, &msg, sizeof(gxh_message));            
          return true;
    
}

//-------------------------------------------------------+-----------------------

