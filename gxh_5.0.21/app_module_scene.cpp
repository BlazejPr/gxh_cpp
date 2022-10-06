/* 
 * File:   app_module_scene.cpp
 * Author: Blazej Kita
 * 
 * Created on 14 grudzień 2016, 14:31
 */

#include "app_module_scene.h"


 
app_module* get_module(gxh_scope* appHomeHandle )
{
    return new app_module_scene(appHomeHandle); 
};
 
//-------------------------------------------------------------------------------
  
app_module_scene::app_module_scene(gxh_scope* appHomeHandle) : app_module(appHomeHandle) 
{           
    this->getAppHandle()->show_log_module("app_module_scene","app_module_scene","start",2 );         
    
     loadToCache();   
}

//------------------------------------------------------------------------------

app_module_scene::~app_module_scene() 
{     
    this->getAppHandle()->show_log_module("app_module_scene","app_module_scene","stop",2 );
}


//-------------------------------------------------------+-----------------------

void app_module_scene::onClick(int ih_driver_port_id, int listener_status)
{
}

//-------------------------------------------------------+----------------------

void app_module_scene::loadToCache()
{
     
    this->getAppHandle()->destroyResource(RES_VECTOR_SCENE);
    this->getAppHandle()->destroyResource(RES_VECTOR_SCENE_ACTION);
    
   
    for(unsigned int xe=0; xe<MAX_BUFFER_SCENE;xe++)
    {
       this->scenes[xe].free = true; //domyslnie brak sceny
       this->scenes[xe].status = 0; //brak statusu..       
    }    
    
    for(unsigned int xt=0; xt<MAX_BUFFER_SCENE_ACTION;xt++)
    {
       this->scene_actions[xt].free = true; //domyslnie brak sceny
       this->scene_actions[xt].status = 0; //brak statusu..       
       this->scene_actions[xt].ih_scene_id = -1; // 
    } 
    
    for(unsigned int xw=0; xw<MAX_BUFFER_TASK; xw++)
    {
        this->tasks[xw].free = true;
        this->tasks[xw].status = 0;
    }
    
    //pobierz sceny z bazy danych..
     sqlite3* dbSqLite;    
     sqlite3_stmt* stmt; 
     
     
     std::string baza = this->getAppHandle()->getDB(); //this->getAppHandle()->getRunPath();
     //baza.append("../gxh_base.db");
     
     int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
     if( r != SQLITE_OK )
     {
          this->getAppHandle()->show_error("app_module_scene","app_module_scene","Cannot open sqlLite 1",0);           
     }
     else
     {
        //sceny...   
        std::string query="SELECT id,name FROM ih_scene WHERE is_delete='0';  ";    
        this->getAppHandle()->show_log_data_base("app_module_scene","app_module_scene", query.c_str() ,6); 
     
        const char* queryChar = query.c_str();        
        if(sqlite3_prepare(dbSqLite , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
        {
          this->getAppHandle()->show_error("app_module_scene","app_module_scene", sqlite3_errstr( sqlite3_errcode(dbSqLite  ) ),4);          
          sqlite3_finalize(stmt);                     
        }else
        {
    
          bool done = false;      
          while (!done)               
          { 
            int res = sqlite3_step (stmt);        
            if(res ==  SQLITE_ROW)
            {                                       
              unsigned int index                                      = gxh_StringToInt( std::string(  (const char*)sqlite3_column_text(stmt, 0) ) );      
             
              if(index > MAX_BUFFER_SCENE-1 ) continue;
              if(index < 0 ) continue;
             
              this->scenes[index].id  = index;
             
              const char* tmp1                                      = (const char*)sqlite3_column_text(stmt, 1); 
              memset(this->scenes[index].name, 0, 100);
              memcpy(this->scenes[index].name, tmp1, strlen(tmp1) );
             
              this->scenes[index].status = 0;
              this->scenes[index].free = false;                    
            }              
            if(res == SQLITE_DONE)done = true;                                     
           }     
       
           sqlite3_finalize(stmt); 
        }
        
        
         //akcje...
        std::string query1="SELECT id, ih_scene_id, const_function_symbol, resource_id, order_by, delay_from_start_ms, name,text_content FROM ih_scene_action WHERE is_delete='0';";
        this->getAppHandle()->show_log_data_base("app_module_scene","app_module_scene", query1.c_str() ,6); 
     
        const char* queryChar1 = query1.c_str();        
        if(sqlite3_prepare(dbSqLite , queryChar1 , strlen(queryChar1) , &stmt, NULL) != SQLITE_OK)
        {
          this->getAppHandle()->show_error("app_module_scene","app_module_scene", sqlite3_errstr( sqlite3_errcode(dbSqLite  ) ),4);          
          sqlite3_finalize(stmt);                     
        }else
        {
    
          bool done = false;      
          while (!done)               
          { 
            int res = sqlite3_step (stmt);        
            if(res ==  SQLITE_ROW)
            {                                       
              unsigned int index                                      = gxh_StringToInt( std::string(  (const char*)sqlite3_column_text(stmt, 0) ) );      
             
              if(index > MAX_BUFFER_SCENE_ACTION-1 ) continue;
              if(index < 0 ) continue;
             
              this->scene_actions[index].id  = index;
                                        
              this->scene_actions[index].ih_scene_id                  = gxh_StringToInt( std::string(  (const char*)sqlite3_column_text(stmt, 1) ) );    
              this->scene_actions[index].const_function_symbol        = gxh_StringToInt( std::string(  (const char*)sqlite3_column_text(stmt, 2) ) );    
              this->scene_actions[index].resource_id                  = gxh_StringToInt( std::string(  (const char*)sqlite3_column_text(stmt, 3) ) );    
              this->scene_actions[index].order_by                     = gxh_StringToInt( std::string(  (const char*)sqlite3_column_text(stmt, 4) ) );    
              this->scene_actions[index].delay_from_start_ms          = gxh_StringToInt( std::string(  (const char*)sqlite3_column_text(stmt, 5) ) );    
              
              const char* tmp1                                      = (const char*)sqlite3_column_text(stmt, 6); 
              memset(this->scene_actions[index].name, 0, 500);
              if( tmp1 != NULL && strlen(tmp1) < 500) memcpy(this->scene_actions[index].name, tmp1, strlen(tmp1) );
              
             
              
              const char* tmp2                                      = (const char*)sqlite3_column_text(stmt, 7); 
              memset(this->scene_actions[index].text_content, 0, 500);
              if( tmp2 != NULL && strlen(tmp2) < 500) memcpy(this->scene_actions[index].text_content, tmp2, strlen(tmp2) );
              
              this->scene_actions[index].status = 0;
              this->scene_actions[index].free = false;   
              
              //--
                       memset(this->scene_actions[index].functionName, 0, 200);
                       memset(this->scene_actions[index].resourceName, 0, 200);
               
      
                       
                       char fName[200];
                      // char rName[200];
                       
                       
                       unsigned int symbolCount = 0;
                       ih_sl_const_function_hash* symbolH = ( ih_sl_const_function_hash* )this->getAppHandle()->getResource( RES_VECTOR_SYMBOL_HASH , &symbolCount); 
                       
                       for(unsigned int p=0;p<symbolCount; p++)
                       {
                           if(symbolH[p].symbol == this->scene_actions[index].const_function_symbol )
                           {
                               memset(fName,0,200);
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
                         
                                      if(lights[b].id ==  this->scene_actions[index].resource_id )
                                      {
                                          memcpy(this->scene_actions[index].resourceName, lights[b].name, strlen(lights[b].name) );
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
                                      if(blinds[b].id ==  this->scene_actions[index].resource_id )
                                      {
                                          memcpy(this->scene_actions[index].resourceName, blinds[b].name, strlen(blinds[b].name) );
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
                                      if(gates[b].id ==  this->scene_actions[index].resource_id )
                                      {
                                          memcpy(this->scene_actions[index].resourceName, gates[b].name, strlen(gates[b].name) );
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
                                      if(sensors[b].id ==  this->scene_actions[index].resource_id )
                                      {
                                          memcpy(this->scene_actions[index].resourceName, sensors[b].name, strlen(sensors[b].name) );
                                      }
                                  }
                         }else
                         if(gxh_compare(fName,"driver") == 0) 
                         {
                                  unsigned int countT = 0;
                                  ih_driver* driver    = (ih_driver*)this->getAppHandle()->getResource(RES_VECTOR_DRIVER, &countT); 
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                                      if(driver[b].id ==  this->scene_actions[index].resource_id )
                                      {
                                          memcpy(this->scene_actions[index].resourceName, driver[b].name, strlen(driver[b].name) );
                                      }
                                  }
                         }else
                         if(gxh_compare(fName,"user") == 0) 
                         {
                                  unsigned int countT = 0;
                                  user_gxh* users = (user_gxh*)this->getAppHandle()->getResource(RES_VECTOR_USER, &countT );              
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                                      if(users[b].px_group_id ==  this->scene_actions[index].resource_id )
                                      {
                                          memcpy(this->scene_actions[index].resourceName, users[b].username, strlen(users[b].username) );
                                      }
                                  }
                         }else
                         {
                                  const char* nameTmp = "Brak zasobu";
                                  memcpy(this->scene_actions[index].resourceName, nameTmp, strlen(nameTmp) );
                         }
                       
                          //   std::cout<<"\nZasob: "<<fName<<", "<<this->scene_actions[index].resourceName;
                       //---
                       
                       
                       
                       unsigned int countF = 0;
                       ih_sl_const_function_symbol* funcs = (ih_sl_const_function_symbol*) this->getAppHandle()->getResource( RES_VECTOR_FUNC , &countF); 
                       
                       for(unsigned int r=0; r<countF; r++)
                       {
                           if(funcs[r].const_function_symbol ==  this->scene_actions[index].const_function_symbol )
                           {
                                  memcpy(this->scene_actions[index].functionName, funcs[r].description, strlen( funcs[r].description) );
                           }
                       }
 
                
                       
                      //  std::cout<<"\n\n"<<fName<<", "<<rName<<"\n";
              //--
              
            }              
            if(res == SQLITE_DONE)done = true;                                     
           }     
       
           sqlite3_finalize(stmt); 
        }
      
        
         //zaplanowane zadania.. //last_work - 0 niedziela itd..
        std::string query2="SELECT id, const_function_symbol, resource_id, mon, tue, wed, thu, fri, sat, sun, every_week, last_work, system, work_time, name, text_content FROM ih_task WHERE is_delete='0';";
        this->getAppHandle()->show_log_data_base("app_module_scene","app_module_scene", query2.c_str() ,6); 
     
        const char* queryChar2 = query2.c_str();        
        if(sqlite3_prepare(dbSqLite , queryChar2 , strlen(queryChar2) , &stmt, NULL) != SQLITE_OK)
        {
          this->getAppHandle()->show_error("app_module_scene","app_module_scene", sqlite3_errstr( sqlite3_errcode(dbSqLite  ) ),4);          
          sqlite3_finalize(stmt);                     
        }else{
            
          bool done = false;      
          while (!done)               
          { 
            int res = sqlite3_step (stmt);        
            if(res ==  SQLITE_ROW)
            {                                       
             unsigned int index              = gxh_StringToInt( std::string(  (const char*)sqlite3_column_text(stmt, 0) ) );      
             
             if(index > MAX_BUFFER_TASK-1 ) continue;
             if(index < 0 ) continue;
             
             this->tasks[index].id  = index;
             
             this->tasks[index].const_function_symbol           = gxh_StringToInt( std::string(  (const char*)sqlite3_column_text(stmt, 1) ) );      
             this->tasks[index].resource_id                     = gxh_StringToInt( std::string(  (const char*)sqlite3_column_text(stmt, 2) ) );      
             this->tasks[index].mon                             = gxh_StringToInt( std::string(  (const char*)sqlite3_column_text(stmt, 3) ) );      
             this->tasks[index].tue                             = gxh_StringToInt( std::string(  (const char*)sqlite3_column_text(stmt, 4) ) );      
             this->tasks[index].wed                             = gxh_StringToInt( std::string(  (const char*)sqlite3_column_text(stmt, 5) ) );      
             this->tasks[index].thu                             = gxh_StringToInt( std::string(  (const char*)sqlite3_column_text(stmt, 6) ) );      
             this->tasks[index].fri                             = gxh_StringToInt( std::string(  (const char*)sqlite3_column_text(stmt, 7) ) );      
             this->tasks[index].sat                             = gxh_StringToInt( std::string(  (const char*)sqlite3_column_text(stmt, 8) ) );      
             this->tasks[index].sun                             = gxh_StringToInt( std::string(  (const char*)sqlite3_column_text(stmt, 9) ) );      
             this->tasks[index].every_week                      = gxh_StringToInt( std::string(  (const char*)sqlite3_column_text(stmt, 10) ) );      
             this->tasks[index].last_work                       = gxh_StringToInt( std::string(  (const char*)sqlite3_column_text(stmt, 11) ) );      
             
            if(sqlite3_column_text(stmt, 12) != NULL) 
            {
                   this->tasks[index].system                          = gxh_StringToInt( std::string(  (const char*)sqlite3_column_text(stmt, 12) ) );      
            } else this->tasks[index].system  = -3;
             
             
             const char* tmp1                                   = (const char*)sqlite3_column_text(stmt, 13);
             memset(this->tasks[index].work_time_str,0,30 );
             if(tmp1 != NULL)memcpy(this->tasks[index].work_time_str,tmp1, strlen(tmp1) );
             
             const char* tmp2                                   = (const char*)sqlite3_column_text(stmt, 14);
             memset(this->tasks[index].name,0, 255 );
             memcpy(this->tasks[index].name,tmp2, strlen(tmp2) );
             
             const char* tmp3                                   = (const char*)sqlite3_column_text(stmt, 15);
             memset(this->tasks[index].name,0, 500 );
             memcpy(this->tasks[index].name,tmp3, strlen(tmp3) );
     
             
              this->tasks[index].status = 0;
              this->tasks[index].free = false;  
              
              //////////----------------
              
               
              //--
                       memset(this->tasks[index].functionName, 0, 100);
                       memset(this->tasks[index].resourceName, 0, 255);
               
      
                       
                       char fName[200];
                       // char rName[200];
                       
                       
                       unsigned int symbolCount = 0;
                       ih_sl_const_function_hash* symbolH = ( ih_sl_const_function_hash* )this->getAppHandle()->getResource( RES_VECTOR_SYMBOL_HASH , &symbolCount); 
                       
                       for(unsigned int p=0;p<symbolCount; p++)
                       {
                           if(symbolH[p].symbol == this->tasks[index].const_function_symbol )
                           {
                               memset(fName,0,200);
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
                         
                                      if(lights[b].id ==  this->tasks[index].resource_id )
                                      {
                                          memcpy(this->tasks[index].resourceName, lights[b].name, strlen(lights[b].name) );
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
                                      if(blinds[b].id ==  this->tasks[index].resource_id )
                                      {
                                          memcpy(this->tasks[index].resourceName, blinds[b].name, strlen(blinds[b].name) );
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
                                      if(gates[b].id ==  this->tasks[index].resource_id )
                                      {
                                          memcpy(this->tasks[index].resourceName, gates[b].name, strlen(gates[b].name) );
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
                                      if(sensors[b].id ==  this->tasks[index].resource_id )
                                      {
                                          memcpy(this->tasks[index].resourceName, sensors[b].name, strlen(sensors[b].name) );
                                      }
                                  }
                         }else
                         if(gxh_compare(fName,"driver") == 0) 
                         {
                                  unsigned int countT = 0;
                                  ih_driver* driver    = (ih_driver*)this->getAppHandle()->getResource(RES_VECTOR_DRIVER, &countT); 
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                                      if(driver[b].id ==  this->tasks[index].resource_id )
                                      {
                                          memcpy(this->tasks[index].resourceName, driver[b].name, strlen(driver[b].name) );
                                      }
                                  }
                         }else
                         if(gxh_compare(fName,"user") == 0) 
                         {
                                  unsigned int countT = 0;
                                  user_gxh* users = (user_gxh*)this->getAppHandle()->getResource(RES_VECTOR_USER, &countT );              
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                                      if(users[b].px_group_id ==  this->tasks[index].resource_id )
                                      {
                                          memcpy(this->tasks[index].resourceName, users[b].username, strlen(users[b].username) );
                                      }
                                  }
                         }else
                         if(gxh_compare(fName,"scene") == 0) 
                         {
                                  unsigned int countT = 0;
                                  ih_scene* rows = (ih_scene*)this->getAppHandle()->getResource( RES_VECTOR_SCENE , &countT);             
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                                      if(rows[b].id ==  this->tasks[index].resource_id )
                                      {
                                          memcpy(this->tasks[index].resourceName, rows[b].name, strlen(rows[b].name) );
                                      }
                                  }
                         }else
                         {
                                  const char* nameTmp = "Brak zasobu";
                                  memcpy(this->tasks[index].resourceName, nameTmp, strlen(nameTmp) );
                         }
                  
                       
                       unsigned int countF = 0;
                       ih_sl_const_function_symbol* funcs = (ih_sl_const_function_symbol*) this->getAppHandle()->getResource( RES_VECTOR_FUNC , &countF); 
                       
                       for(unsigned int r=0; r<countF; r++)
                       {
                           if(funcs[r].const_function_symbol ==  this->tasks[index].const_function_symbol )
                           {
                                  memcpy(this->tasks[index].functionName, funcs[r].description, strlen( funcs[r].description) );
                           }
                       }
                       
                          //   std::cout<<"\nZasob: "<<fName<<", "<<this->scene_actions[index].resourceName;
                       //---
              
              //////----------------

            
            }              
            if(res == SQLITE_DONE)done = true;                                     
           }     
       
           sqlite3_finalize(stmt); 
        }
        
        
        
      sqlite3_close_v2(dbSqLite);
     }
     
     
    this->getAppHandle()->registerResource( this->scenes , MAX_BUFFER_SCENE, RES_VECTOR_SCENE );
    this->getAppHandle()->registerResource( this->scene_actions , MAX_BUFFER_SCENE_ACTION, RES_VECTOR_SCENE_ACTION );
    this->getAppHandle()->registerResource( this->tasks , MAX_BUFFER_TASK, RES_VECTOR_TASK );
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
}

//-------------------------------------------------------+-----------------------

void app_module_scene::forceCancel()
{  
      this->getAppHandle()->show_log_module("app_module_scene","app_module_scene","forceCancel",2);
   
     
    
      this->forceSynchronize();
      this->getAppHandle()->destroyResource(RES_VECTOR_SCENE);
      this->getAppHandle()->destroyResource(RES_VECTOR_SCENE_ACTION);
      this->getAppHandle()->destroyResource(RES_VECTOR_TASK);
}

//-------------------------------------------------------+-----------------------

void app_module_scene::forceSynchronize()
{  
     this->getAppHandle()->show_log_module("app_module_scene","forceSynchronize","start",20);          
     
    
      
     
     
     //scene..
     for(unsigned int x=0; x<MAX_BUFFER_SCENE; x++)
     {
         if( !this->scenes[x].free && this->scenes[x].status==1 ) //update name..
         {
               //-------------------
                  sqlite3* dbSqLite;               
                  std::string baza = this->getAppHandle()->getDB(); //this->getAppHandle()->getRunPath();
                  //baza.append("../gxh_base.db");
     
                  int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
                  if( r != SQLITE_OK )
                    {
                     this->getAppHandle()->show_error("app_module_scene","forceSynchronize","Cannot open sqlLite 1",0);                 
                    }else
                    {
                         std::string query="";
                         query.append("UPDATE ih_scene SET name='");
                         query.append( this->scenes[x].name  );
                         query.append("' WHERE id='");
                         query.append( gxh_IntToString( this->scenes[x].id   ) );
                         query.append("'; ");
                         
                        char *zErrMsg = 0;     
                        int rc = sqlite3_exec( dbSqLite , query.c_str(), 0, 0, &zErrMsg);
                        if( rc == SQLITE_OK )  this->scenes[x].status=0;                                          
                        if( rc != SQLITE_OK )  sqlite3_free(zErrMsg); 
                        
                        sqlite3_close_v2(dbSqLite);         
                    }
               //-------------------
         }
         
         if( !this->scenes[x].free && this->scenes[x].status==2 ) //add new
         {
             
           //  std::cout<<"\n\nScene to add "<<this->scenes[x].name ;
             
                  //-------------------
                  sqlite3* dbSqLite;               
                  std::string baza = this->getAppHandle()->getDB(); //this->getAppHandle()->getRunPath();
                 // baza.append("../gxh_base.db");
     
                  int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
                  if( r != SQLITE_OK )
                    {
                     this->getAppHandle()->show_error("app_module_scene","forceSynchronize","Cannot open sqlLite 1",0);                 
                    }else
                    {
                         std::string query="";
                         query.append("INSERT INTO ih_scene (id,name,created_at,updated_at,is_delete) VALUES ('");
                         query.append( gxh_IntToString(x)  ); //index..
                         query.append("','");
                         query.append(  this->scenes[x].name );
                         query.append("', DATETIME('now','localtime') ,DATETIME('now','localtime'),'0'); ");
                         
                        char *zErrMsg = 0;     
                        int rc = sqlite3_exec( dbSqLite , query.c_str(), 0, 0, &zErrMsg);
                        if( rc == SQLITE_OK ) { this->scenes[x].status=0; this->scenes[x].free=false; this->scenes[x].id=(int)x;  }
                        if( rc != SQLITE_OK )  sqlite3_free(zErrMsg); 
                        
                        sqlite3_close_v2(dbSqLite);     
                    }
                    //-------------------
         }
         
         if( !this->scenes[x].free && this->scenes[x].status==3 ) //delete scene..
         {
                    //-------------------
                  sqlite3* dbSqLite;               
                  std::string baza = this->getAppHandle()->getDB();//this->getAppHandle()->getRunPath();
                 // baza.append("../gxh_base.db");
     
                  int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
                  if( r != SQLITE_OK )
                    {
                     this->getAppHandle()->show_error("app_module_scene","forceSynchronize","Cannot open sqlLite 1",0);                 
                    }else
                    {
                         std::string query="";
                         query.append("DELETE FROM ih_scene WHERE id='");
                         query.append( gxh_IntToString(x)  ); //index..                  
                         query.append("';"  ); //index..                  
                         
                        char *zErrMsg = 0;     
                        int rc = sqlite3_exec( dbSqLite , query.c_str(), 0, 0, &zErrMsg);
                        if( rc == SQLITE_OK ) { this->scenes[x].status=0;   this->scenes[x].free=true; this->scenes[x].id=(int)x;  }
                        if( rc != SQLITE_OK )  sqlite3_free(zErrMsg); 
                        
                        sqlite3_close_v2(dbSqLite);     
                    }
                    //-------------------
         }
     }
     
     
     
      //action scene
     for(unsigned int x=0; x<MAX_BUFFER_SCENE_ACTION; x++)
     {
         if( !this->scene_actions[x].free && this->scene_actions[x].status==1 ) //update name..
         {
               //-------------------
                  sqlite3* dbSqLite;               
                  std::string baza = this->getAppHandle()->getDB();//this->getAppHandle()->getRunPath();
                 // baza.append("../gxh_base.db");
     
                  int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
                  if( r != SQLITE_OK )
                    {
                     this->getAppHandle()->show_error("app_module_scene","forceSynchronize","Cannot open sqlLite 1",0);                 
                    }else
                    {
                     this->scene_actions[x].status =  0;   
                         
                      //  char *zErrMsg = 0;     
                      //  int rc = sqlite3_exec( dbSqLite , query.c_str(), 0, 0, &zErrMsg);
                      //  if( rc == SQLITE_OK )  this->scene_actions[x].status=0;                                          
                       // if( rc != SQLITE_OK )  sqlite3_free(zErrMsg); 
                      sqlite3_close_v2(dbSqLite);     
                    }
               //-------------------
         }
         
         if( !this->scene_actions[x].free && this->scene_actions[x].status==2 ) //add new
         {
                  //-------------------
                  sqlite3* dbSqLite;               
                  std::string baza = this->getAppHandle()->getDB();//this->getAppHandle()->getRunPath();
                 // baza.append("../gxh_base.db");
     
                  int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
                  if( r != SQLITE_OK )
                    {
                     this->getAppHandle()->show_error("app_module_scene","forceSynchronize","Cannot open sqlLite 1",0);                 
                    }else
                    {
                        std::string query = "INSERT INTO ih_scene_action (id, ih_scene_id, const_function_symbol, resource_id, order_by, delay_from_start_ms, name,text_content, created_at, updated_at, is_delete ) VALUES ('";
                        query.append( gxh_IntToString( this->scene_actions[x].id ) );                        
                        query.append( "','" );                        
                        query.append( gxh_IntToString( this->scene_actions[x].ih_scene_id ) );                        
                        query.append( "','" );  
                        query.append( gxh_IntToString( this->scene_actions[x].const_function_symbol ) );                        
                        query.append( "','" ); 
                        query.append( gxh_IntToString( this->scene_actions[x].resource_id ) );                        
                        query.append( "','" ); 
                        query.append( gxh_IntToString( this->scene_actions[x].order_by ) );                        
                        query.append( "','" ); 
                        query.append( gxh_IntToString( this->scene_actions[x].delay_from_start_ms ) );                        
                        query.append( "','" ); 
                        query.append( this->scene_actions[x].name ); 
                        query.append( "','" ); 
                        query.append( this->scene_actions[x].text_content ); 
                        query.append("', DATETIME('now','localtime') ,DATETIME('now','localtime'),'0'); ");
                        
                        char *zErrMsg = 0;     
                        int rc = sqlite3_exec( dbSqLite , query.c_str(), 0, 0, &zErrMsg);
                        if( rc == SQLITE_OK ) { this->scene_actions[x].status=0; this->scene_actions[x].free=false; this->scene_actions[x].id=(int)x;  }
                        if( rc != SQLITE_OK )  sqlite3_free(zErrMsg); 
                        
                       memset(this->scene_actions[x].functionName, 0, 200);
                       memset(this->scene_actions[x].resourceName, 0, 200);
               
      
                       
                       char fName[200];
                      // char rName[200];
                       
                       unsigned int symbolCount = 0;
                       ih_sl_const_function_hash* symbolH = ( ih_sl_const_function_hash* )this->getAppHandle()->getResource( RES_VECTOR_SYMBOL_HASH , &symbolCount); 
                       
                       for(unsigned int p=0;p<symbolCount; p++)
                       {
                           if(symbolH[p].symbol == this->scene_actions[x].const_function_symbol )
                           {
                               memset(fName,0,200);
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
                                      if(lights[b].id ==  this->scene_actions[x].resource_id )
                                      {
                                          memcpy(this->scene_actions[x].resourceName, lights[b].name, strlen(lights[b].name) );
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
                                      if(blinds[b].id ==  this->scene_actions[x].resource_id )
                                      {
                                          memcpy(this->scene_actions[x].resourceName, blinds[b].name, strlen(blinds[b].name) );
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
                                      if(gates[b].id ==  this->scene_actions[x].resource_id )
                                      {
                                          memcpy(this->scene_actions[x].resourceName, gates[b].name, strlen(gates[b].name) );
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
                                      if(sensors[b].id ==  this->scene_actions[x].resource_id )
                                      {
                                          memcpy(this->scene_actions[x].resourceName, sensors[b].name, strlen(sensors[b].name) );
                                      }
                                  }
                         }else
                         if(gxh_compare(fName,"driver") == 0) 
                         {
                                  unsigned int countT = 0;
                                  ih_driver* driver    = (ih_driver*)this->getAppHandle()->getResource(RES_VECTOR_DRIVER, &countT); 
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                                      if(driver[b].id ==  this->scene_actions[x].resource_id )
                                      {
                                          memcpy(this->scene_actions[x].resourceName, driver[b].name, strlen(driver[b].name) );
                                      }
                                  }
                         }else
                         if(gxh_compare(fName,"user") == 0) 
                         {
                                  unsigned int countT = 0;
                                  user_gxh* users = (user_gxh*)this->getAppHandle()->getResource(RES_VECTOR_USER, &countT );    
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                                      if(users[b].px_group_id ==  this->scene_actions[x].resource_id )
                                      {
                                          memcpy(this->scene_actions[x].resourceName, users[b].username, strlen(users[b].username) );
                                      }
                                  }
                         }else
                         {
                                  const char* nameTmp = "Brak zasobu";
                                  memcpy(this->scene_actions[x].resourceName, nameTmp, strlen(nameTmp) );
                         }
                       
                       //---
                      
                       
                       unsigned int countF = 0;
                       ih_sl_const_function_symbol* funcs = (ih_sl_const_function_symbol*) this->getAppHandle()->getResource( RES_VECTOR_FUNC , &countF); 
                       
                       for(unsigned int r=0; r<countF; r++)
                       {
                           if(funcs[r].const_function_symbol ==  this->scene_actions[x].const_function_symbol )
                           {
                                  memcpy(this->scene_actions[x].functionName, funcs[r].description, strlen( funcs[r].description) );
                           }
                       }
 
                 
                        
                       sqlite3_close_v2(dbSqLite);     
               
                    }
                    //-------------------
         }
         
         if( !this->scene_actions[x].free && this->scene_actions[x].status==3 ) //delete scene..
         {
                    //-------------------
                  sqlite3* dbSqLite;               
                  std::string baza = this->getAppHandle()->getDB();//this->getAppHandle()->getRunPath();
                 // baza.append("../gxh_base.db");
     
                  int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
                  if( r != SQLITE_OK )
                    {
                     this->getAppHandle()->show_error("app_module_scene","forceSynchronize","Cannot open sqlLite 1",0);                 
                    }else
                    {
                        std::string query = "DELETE FROM ih_scene_action WHERE id='";   
                        query.append(gxh_IntToString(  this->scene_actions[x].id ) );
                        query.append("'; ");
                      
                        char *zErrMsg = 0;     
                        int rc = sqlite3_exec( dbSqLite , query.c_str(), 0, 0, &zErrMsg);
                        if( rc == SQLITE_OK ) { this->scene_actions[x].status=0;   this->scene_actions[x].free=true; this->scene_actions[x].id=(int)x;  }
                        if( rc != SQLITE_OK )  sqlite3_free(zErrMsg); 
                        
                        sqlite3_close_v2(dbSqLite);                             
                    }
                    //-------------------
         }
     }
     
     
     //zaplanowane zadania
     for(unsigned int x=0; x<MAX_BUFFER_TASK; x++)
     {
          if( !this->tasks[x].free && this->tasks[x].status==1 ) //update name.. last work..
          {
              //-------------------
                  sqlite3* dbSqLite;               
                  std::string baza = this->getAppHandle()->getDB();//this->getAppHandle()->getRunPath();
                 // baza.append("../gxh_base.db");
     
                  int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
                  if( r != SQLITE_OK )
                    {
                     this->getAppHandle()->show_error("app_module_scene","forceSynchronize","Cannot open sqlLite 1",0);                 
                    }else
                    {
                        std::string query = "UPDATE ih_task SET const_function_symbol='";
                        query.append(    gxh_IntToString( this->tasks[x].const_function_symbol )   );
                        query.append("', resource_id='");
                        query.append( gxh_IntToString( this->tasks[x].resource_id ) );                                                  
                        query.append("', mon='");
                        query.append( gxh_IntToString( this->tasks[x].mon ) );   
                        query.append("', tue='");
                        query.append( gxh_IntToString( this->tasks[x].tue ) );  
                        query.append("', wed='");
                        query.append( gxh_IntToString( this->tasks[x].wed ) );  
                        query.append("', thu='");
                        query.append( gxh_IntToString( this->tasks[x].thu ) ); 
                        query.append("', fri='");
                        query.append( gxh_IntToString( this->tasks[x].fri ) ); 
                        query.append("', sat='");
                        query.append( gxh_IntToString( this->tasks[x].sat ) ); 
                        query.append("', sun='");
                        query.append( gxh_IntToString( this->tasks[x].sun ) ); 
                        query.append("', every_week='");
                        query.append( gxh_IntToString( this->tasks[x].every_week ) ); 
                        query.append("', last_work='");
                        query.append( gxh_IntToString( this->tasks[x].last_work ) ); 
                        query.append("', system='");
                        query.append( gxh_IntToString( this->tasks[x].system ) ); 
                        query.append("', work_time='");
                        query.append(  this->tasks[x].work_time_str  ); 
                        query.append("', name='");
                        query.append(  this->tasks[x].name  );
                        query.append("', text_content='");
                        query.append(  this->tasks[x].text_content  );
                        query.append(  "' WHERE id='" );
                        query.append( gxh_IntToString( x ) ); 
                        query.append( "'; "); 
                 
                     
                        
                        char *zErrMsg = 0;     
                        int rc = sqlite3_exec( dbSqLite , query.c_str(), 0, 0, &zErrMsg);
                        if( rc == SQLITE_OK ) 
                        {
                            this->tasks[x].status=0; 
                            this->tasks[x].free=false;   
                        }
                        if( rc != SQLITE_OK )  
                        {
                            std::cout<<"\n\nZap "<<query.c_str();
                            std::cout<<zErrMsg;
                            sqlite3_free(zErrMsg); 
                        }
                        
                        sqlite3_close_v2(dbSqLite);     
                    }//---
          }
          
          if( !this->tasks[x].free && this->tasks[x].status==2 ) //add new
          {  
                  //-------------------
                  sqlite3* dbSqLite;               
                  std::string baza =this->getAppHandle()->getDB();// this->getAppHandle()->getRunPath();
                  //baza.append("../gxh_base.db");
     
                  int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
                  if( r != SQLITE_OK )
                    {
                     this->getAppHandle()->show_error("app_module_scene","forceSynchronize","Cannot open sqlLite 1",0);                 
                    }else
                    {
                        this->tasks[x].last_work = -1;
                      
                        std::string query = "INSERT INTO ih_task ( id, const_function_symbol, resource_id, mon, tue, wed, thu, fri, sat, sun, every_week, last_work, system, work_time, name, text_content,created_at,updated_at, is_delete ) VALUES ('";
                        query.append( gxh_IntToString( x ) );                        
                        query.append( "','" );                                    
                        query.append( gxh_IntToString( this->tasks[x].const_function_symbol ) );                        
                        query.append( "','" );  
                        query.append( gxh_IntToString( this->tasks[x].resource_id ) );                        
                        query.append( "','" );  
                        query.append( gxh_IntToString( this->tasks[x].mon ) );                        
                        query.append( "','" );  
                        query.append( gxh_IntToString( this->tasks[x].tue ) );                        
                        query.append( "','" );  
                        query.append( gxh_IntToString( this->tasks[x].wed ) );
                        query.append( "','" ); 
                        query.append( gxh_IntToString( this->tasks[x].thu ) );
                        query.append( "','" );
                        query.append( gxh_IntToString( this->tasks[x].fri ) );
                        query.append( "','" );
                        query.append( gxh_IntToString( this->tasks[x].sat ) );
                        query.append( "','" );
                        query.append( gxh_IntToString( this->tasks[x].sun) );
                        query.append( "','" );
                        query.append( gxh_IntToString( this->tasks[x].every_week ) );
                        query.append( "','" );
                        query.append( gxh_IntToString( this->tasks[x].last_work ) );
                        query.append( "','" );
                        query.append( gxh_IntToString( this->tasks[x].system ) );
                        query.append( "','" );
                        query.append(  this->tasks[x].work_time_str  );
                        query.append( "','" );
                        query.append(  this->tasks[x].name  );
                        query.append( "','" );
                        query.append(  this->tasks[x].text_content  );                       
                        query.append("', DATETIME('now','localtime') ,DATETIME('now','localtime'),'0'); ");
                        
                        char *zErrMsg = 0;     
                        int rc = sqlite3_exec( dbSqLite , query.c_str(), 0, 0, &zErrMsg);
                        if( rc == SQLITE_OK ) { this->tasks[x].status=0; this->tasks[x].free=false; this->tasks[x].id=(int)x;  }
                        if( rc != SQLITE_OK )  sqlite3_free(zErrMsg); 
                        
                        sqlite3_close_v2(dbSqLite);   
                        
                        
                        
                        //---------
                       
                         //////////----------------
              
               
              //--
                       memset(this->tasks[x].functionName, 0, 100);
                       memset(this->tasks[x].resourceName, 0, 255);
               
      
                       
                       char fName[200];
                       // char rName[200];
                       
                       
                       unsigned int symbolCount = 0;
                       ih_sl_const_function_hash* symbolH = ( ih_sl_const_function_hash* )this->getAppHandle()->getResource( RES_VECTOR_SYMBOL_HASH , &symbolCount); 
                       
                       for(unsigned int p=0;p<symbolCount; p++)
                       {
                           if(symbolH[p].symbol == this->tasks[x].const_function_symbol )
                           {
                               memset(fName,0,200);
                               memcpy(fName,symbolH[p].name, strlen(symbolH[p].name) );
                           }
                       }
                       
                      
                       
                        //---
                 
                      // std::cout<<"\n\n\n"<<fName<<" | "<<this->tasks[x].resource_id<<"\n\n\n";
                       
                       if(gxh_compare(fName,"light") == 0) 
                         {
                                  unsigned int countT = 0;
                                  ih_lighting* lights   = (ih_lighting*)this->getAppHandle()->getResource(RES_VECTOR_LIGHT, &countT); 
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                         
                                      if(lights[b].id ==  this->tasks[x].resource_id )
                                      { 
                                          memcpy(this->tasks[x].resourceName, lights[b].name, strlen(lights[b].name) );                                           
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
                                      if(blinds[b].id ==  this->tasks[x].resource_id )
                                      {
                                          memcpy(this->tasks[x].resourceName, blinds[b].name, strlen(blinds[b].name) );
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
                                      if(gates[b].id ==  this->tasks[x].resource_id )
                                      {
                                          memcpy(this->tasks[x].resourceName, gates[b].name, strlen(gates[b].name) );
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
                                      if(sensors[b].id ==  this->tasks[x].resource_id )
                                      {
                                          memcpy(this->tasks[x].resourceName, sensors[b].name, strlen(sensors[b].name) );
                                      }
                                  }
                         }else
                         if(gxh_compare(fName,"driver") == 0) 
                         {
                                  unsigned int countT = 0;
                                  ih_driver* driver    = (ih_driver*)this->getAppHandle()->getResource(RES_VECTOR_DRIVER, &countT); 
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                                      if(driver[b].id ==  this->tasks[x].resource_id )
                                      {
                                          memcpy(this->tasks[x].resourceName, driver[b].name, strlen(driver[b].name) );
                                      }
                                  }
                         }else
                         if(gxh_compare(fName,"user") == 0) 
                         {
                                  unsigned int countT = 0;
                                  user_gxh* users = (user_gxh*)this->getAppHandle()->getResource(RES_VECTOR_USER, &countT );              
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                                      if(users[b].px_group_id ==  this->tasks[x].resource_id )
                                      {
                                          memcpy(this->tasks[x].resourceName, users[b].username, strlen(users[b].username) );
                                      }
                                  }
                         }else
                         if(gxh_compare(fName,"scene") == 0) 
                         {
                                  unsigned int countT = 0;
                                  ih_scene* rows = (ih_scene*)this->getAppHandle()->getResource( RES_VECTOR_SCENE , &countT);           
                                  
                                  for(unsigned int b=0; b<countT; b++) 
                                  {
                                      if(rows[b].id ==  this->tasks[x].resource_id )
                                      {
                                          memcpy(this->tasks[x].resourceName, rows[b].name, strlen(rows[b].name) );
                                      }
                                  }
                         }else
                         {
                                  const char* nameTmp = "Brak zasobu";
                                  memcpy(this->tasks[x].resourceName, nameTmp, strlen(nameTmp) );
                         }
                       
                       
                       unsigned int countF = 0;
                       ih_sl_const_function_symbol* funcs = (ih_sl_const_function_symbol*) this->getAppHandle()->getResource( RES_VECTOR_FUNC , &countF); 
                       
                       for(unsigned int r=0; r<countF; r++)
                       {
                           if(funcs[r].const_function_symbol ==  this->tasks[x].const_function_symbol )
                           {
                                  memcpy(this->tasks[x].functionName, funcs[r].description, strlen( funcs[r].description) );
                           }
                       }
                       
                          //   std::cout<<"\nZasob: "<<fName<<", "<<this->scene_actions[index].resourceName;
                       //---
              
              //////----------------
                        
                        //---------
                        
                        
                        
                    }//---
          }
          
          if( !this->tasks[x].free && this->tasks[x].status==3 ) //delete
          {
                 //-------------------
                  sqlite3* dbSqLite;               
                  std::string baza = this->getAppHandle()->getDB();//this->getAppHandle()->getRunPath();
                 // baza.append("../gxh_base.db");
     
                  int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
                  if( r != SQLITE_OK )
                    {
                     this->getAppHandle()->show_error("app_module_scene","forceSynchronize","Cannot open sqlLite 1",0);                 
                    }else
                    {
                        std::string query = "DELETE FROM ih_task WHERE id='";   
                        query.append(gxh_IntToString(  this->tasks[x].id ) );
                        query.append("'; ");
                      
                        char *zErrMsg = 0;     
                        int rc = sqlite3_exec( dbSqLite , query.c_str(), 0, 0, &zErrMsg);
                        if( rc == SQLITE_OK ) { this->tasks[x].status=0;   this->tasks[x].free=true; this->tasks[x].id=(int)x;  }
                        if( rc != SQLITE_OK )  sqlite3_free(zErrMsg); 
                        
                        sqlite3_close_v2(dbSqLite);                             
                    }
                    //-------------------
          }
     }
     
     
     
     //wykonaj zaplanowane zadania...
     
     
     
     this->getAppHandle()->show_log_module("app_module_scene","forceSynchronize","stop",20);          
}

//-------------------------------------------------------+-----------------------


bool app_module_scene::execute(gxh_message* inMsg,gxh_message* message_ret)  
{               
      gxh_message message;
      memcpy(&message, inMsg, sizeof(gxh_message));
    
     if(message.func_number == IH_MODULE_FUNCTION_RUN_SCENE )     //uruchom scene..
           {
              ih_driver_port_function functionList;
              memcpy(&functionList, message.dataStruct, sizeof(ih_driver_port_function) );
              
              this->runScene(functionList.resource_id);
              
              
              const char* response = "OK"; 
              gxh_message msg;
              msg.gxh_handle = message.gxh_handle;
              memcpy(msg.dataStruct, response, strlen(response) );
              msg.sizeDataStruct = sizeof(response) ;
              msg.system_code = GXH_MESSAGE_SUCCESS;
            
              memcpy(message_ret, &msg, sizeof(gxh_message));            
              return true;
           }
   
     if(message.func_number == IH_MODULE_FUNCTION_RUN_SCENE_TIME )     //uruchum zaplanowane zadanie..
           {              
               this->runTask();
           }
     
          const char* response = "SCENE MODULE"; 
          gxh_message msg;
          msg.gxh_handle = message.gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;
          
}

 
//------------------------------------------------------------------------------

 void app_module_scene::runTask()
 {
     
     
      
     time_t     now = time(0);
     struct tm *aTime = localtime(&now);
     int dayOfWeek = aTime->tm_wday; // 0 sunday
     
   //  std::cout<<"\n\nSprawdz Zadania\n\n";
     
     for(unsigned int x=0; x<MAX_BUFFER_TASK;x++)
     {
          if(!this->tasks[x].free)  
          {
              if( this->tasks[x].last_work !=  dayOfWeek)
              {
                  //------------ // 2018-07-05 14:16:00
                  char hour[3];
                  char minute[3];
                  
                  memset(hour,0,3);
                  memset(minute,0,3);
                  
                  hour[0] = this->tasks[x].work_time_str[11];
                  hour[1] = this->tasks[x].work_time_str[12];
                  
                  minute[0] = this->tasks[x].work_time_str[14];
                  minute[1] = this->tasks[x].work_time_str[15];
                  
                  int h = gxh_StringToInt( std::string(hour) );
                  int m = gxh_StringToInt( std::string(minute) );
                  
                  int seconds = (h * 3600);
                  seconds += m * 60;
                  
                  int secondsNow = (aTime->tm_hour * 3600);
                  secondsNow += aTime->tm_min * 60;
                  
                  //------------
                  
                   
              //    std::cout<<"\n\nXX:"<<seconds<<" YY: "<<secondsNow<<" roznica "<<(seconds - secondsNow )<<" | "<<h<<":"<<m;
                  
                   
                  
                  if(this->tasks[x].mon == 1 && dayOfWeek == 1 && seconds <= secondsNow  )
                  {
                       gxh_message inM;
                       gxh_message outM;
                       this->executeFunctionParam(&inM , &outM,this->tasks[x].const_function_symbol , this->tasks[x].resource_id ,  this->tasks[x].name  ,   this->tasks[x].text_content, NULL );
                       
                       this->tasks[x].last_work = dayOfWeek;
                       this->tasks[x].status = 1;
                       if(this->tasks[x].every_week == 0)   this->tasks[x].status = 3; //usun do wykorzystania jednorazowego..
                       if(this->tasks[x].every_week != 0)   this->tasks[x].status = 1; //aktualizuj
                       
                       continue;
                  }
                  
                  if(this->tasks[x].tue == 1 && dayOfWeek == 2 && seconds <= secondsNow  )
                  {
                      gxh_message inM;
                      gxh_message outM;
                      this->executeFunctionParam(&inM , &outM,this->tasks[x].const_function_symbol , this->tasks[x].resource_id ,  this->tasks[x].name  ,   this->tasks[x].text_content,NULL );
                       
                      this->tasks[x].last_work = dayOfWeek;
                      this->tasks[x].status = 1;
                      if(this->tasks[x].every_week == 0)   this->tasks[x].status = 3; //usun do wykorzystania jednorazowego..
                      if(this->tasks[x].every_week != 0)   this->tasks[x].status = 1; //aktualizuj
                      continue;
                  }
                  
                  if(this->tasks[x].wed == 1 && dayOfWeek == 3 && seconds <= secondsNow  )
                  {
                      gxh_message inM;
                       gxh_message outM;
                       this->executeFunctionParam(&inM , &outM,this->tasks[x].const_function_symbol , this->tasks[x].resource_id ,  this->tasks[x].name  ,   this->tasks[x].text_content, NULL );
                       
                       this->tasks[x].last_work = dayOfWeek;
                       this->tasks[x].status = 1;
                       if(this->tasks[x].every_week == 0)   this->tasks[x].status = 3; //usun do wykorzystania jednorazowego..
                       if(this->tasks[x].every_week != 0)   this->tasks[x].status = 1; //aktualizuj
                       continue;
                  }
                  
                  if(this->tasks[x].thu == 1 && dayOfWeek == 4 && seconds <= secondsNow  )
                  {
                     //  std::cout<<"\n\n\n\nStart...."<<this->tasks[x].work_time_str;
                       gxh_message inM;
                       gxh_message outM;
                       this->executeFunctionParam(&inM , &outM,this->tasks[x].const_function_symbol , this->tasks[x].resource_id ,  this->tasks[x].name  ,   this->tasks[x].text_content, NULL );
                       
                        this->tasks[x].last_work = dayOfWeek;
                        this->tasks[x].status = 1;
                        if(this->tasks[x].every_week == 0)   this->tasks[x].status = 3; //usun do wykorzystania jednorazowego..
                        if(this->tasks[x].every_week != 0)   this->tasks[x].status = 1; //aktualizuj
                         continue;
                  }
                  
                  if(this->tasks[x].fri == 1 && dayOfWeek == 5 && seconds <= secondsNow  )
                  {
                      gxh_message inM;
                       gxh_message outM;
                       this->executeFunctionParam(&inM , &outM,this->tasks[x].const_function_symbol , this->tasks[x].resource_id ,  this->tasks[x].name  ,   this->tasks[x].text_content, NULL );
                       
                        this->tasks[x].last_work = dayOfWeek;
                        this->tasks[x].status = 1;
                        if(this->tasks[x].every_week == 0)   this->tasks[x].status = 3; //usun do wykorzystania jednorazowego..
                        if(this->tasks[x].every_week != 0)   this->tasks[x].status = 1; //aktualizuj
                         continue;
                  }
                  
                  if(this->tasks[x].sat == 1 && dayOfWeek == 6 && seconds <= secondsNow  )
                  {
                      gxh_message inM;
                       gxh_message outM;
                       this->executeFunctionParam(&inM , &outM,this->tasks[x].const_function_symbol , this->tasks[x].resource_id ,  this->tasks[x].name  ,   this->tasks[x].text_content, NULL );
                       
                        this->tasks[x].last_work = dayOfWeek;
                        this->tasks[x].status = 1;
                        if(this->tasks[x].every_week == 0)   this->tasks[x].status = 3; //usun do wykorzystania jednorazowego..
                        if(this->tasks[x].every_week != 0)   this->tasks[x].status = 1; //aktualizuj
                         continue;
                  }
                  
                  if(this->tasks[x].sun == 1 && dayOfWeek == 0 && seconds <= secondsNow  )
                  {
                      gxh_message inM;
                       gxh_message outM;
                       this->executeFunctionParam(&inM , &outM,this->tasks[x].const_function_symbol , this->tasks[x].resource_id ,  this->tasks[x].name  ,   this->tasks[x].text_content, NULL );
                       
                        this->tasks[x].last_work = dayOfWeek;
                        this->tasks[x].status = 1;
                        if(this->tasks[x].every_week == 0)   this->tasks[x].status = 3; //usun do wykorzystania jednorazowego..
                        if(this->tasks[x].every_week != 0)   this->tasks[x].status = 1; //aktualizuj
                         continue;
                  }
                  
                 
              }
          }
     };
 }

//------------------------------------------------------------------------------
 
void app_module_scene::runScene(int idScene)
{
        unsigned int size = 0;
        ih_scene* rows = (ih_scene*)this->getAppHandle()->getResource( RES_VECTOR_SCENE , &size);  
        
        for(unsigned int x=0; x<size; x++)
        {
            if(rows[x].id == idScene)
            {
                   unsigned int size2 = 0;
                   ih_scene_action* rows2 = (ih_scene_action*)this->getAppHandle()->getResource( RES_VECTOR_SCENE_ACTION , &size2);  
                   
                   for(unsigned y=0; y<size2; y++)
                   {
                       if(rows2[y].ih_scene_id != idScene) continue; 
                       
                       gxh_message inM;
                       gxh_message outM;
                       this->executeFunctionParam(&inM , &outM, rows2[y].const_function_symbol , rows2[y].resource_id ,  rows2[y].name  ,   rows2[y].text_content, NULL );
                   }
            }
        }
}

//------------------------------------------------------------------------------
