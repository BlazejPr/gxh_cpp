/* 
 * File:   gxh_core.cpp
 * Author: Blazej Kita,
 *
 * Created on 15 may 2018, 10:50
 */

#include "gxh_core.h"

//------------------------------------------------------------------------------

//struct thread interrupt
struct gxh_th_intr
{
    gxh_core* handle;
    unsigned int index;
};

//------------------------------------------------------------------------------

void* goThread(void* coreHandle)
{   
    ((gxh_core*)coreHandle)->runThread(  );
    return NULL;
};

void* goThreadInterrupt(void* intr_str)
{   
    gxh_th_intr* intr = (gxh_th_intr*)intr_str;
    
    return intr->handle->runThreadInterrupt( intr_str ); 
};


void* goThreadService(void* coreHandle)
{   
    ((gxh_core*)coreHandle)->runThreadService(   );
    return NULL;
};


void* goThreadSynchronize(void* coreHandle)
{   
    ((gxh_core*)coreHandle)->runThreadSynchronize(   );
    return NULL;
};


//------------------------------------------------------------------------------

gxh_core::gxh_core(int argc, char** argv) : gxh_scope(argc, argv) 
{
      this->thInterrupt = true; //keep interrupt htread working..
      this->thService   = true; //keep interrupt htread working..
      this->thSynchronize = true;
      
      this->show_log_core("gxh_core", "gxh_core", "starting...", 0);
      
      
      this->checkVersionGXH(); //najpierw spraw w bazie danych czy są tam potrzebne dla tej wersji moduły i sterowniki....
    
      
     //sprawdz czy można otworzyć baze danych...
      
     sqlite3* dbSqLite;                 
     std::string baza = this->getDB();//this->getRunPath();
     //baza.append("../gxh_base.db");
     
     int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
     if( r != SQLITE_OK )
     {
          this->show_error("gxh_core","gxh_core","Cannot open sqlLite 1",0);   
          return;
     }else
     {
          sqlite3_close_v2(dbSqLite);
     };
     
     
     sqlite3* dbSqLite2;                 
     std::string baza2 = this->getDBTmp(); // this->getRunPath();
     //baza2.append("../gxh_base_tmp.db");
     
     int r2  =  sqlite3_open_v2(baza2.c_str(), &dbSqLite2, SQLITE_OPEN_READWRITE, NULL   );                                  
     if( r2 != SQLITE_OK )
     {
          this->show_error("gxh_core","gxh_core","Cannot open sqlLite 2",0);   
          return;
     }else
     {
          sqlite3_close_v2(dbSqLite2);
     };
      
      //------------------------------------------------------------------------
      
      
       pthread_mutexattr_t m_attr;
       pthread_mutexattr_init(&m_attr);		/* inicjalizacja atrybutów mutexa */         
       pthread_mutex_init(&this->gxh_response_muteks, &m_attr);    
       pthread_mutex_init(&this->gxh_handle_muteks, &m_attr);    
       
      
      //start main thread...
       pthread_attr_t attr;
       pthread_attr_init(&attr);       
       int rc = pthread_create(&this->mainThread, &attr,  &goThread, (void*)this );
       if (rc)
       {    
             this->thInterrupt = false;
             this->thService   = false;
             this->thSynchronize = false;
             this->show_error("gxh_core", "gxh_core", "cannot start main thread", 0);
       }else
       {
           pthread_setname_np(this->mainThread,"INT");
       }
      pthread_attr_destroy(&attr);  
       
      
      this->executeThreadTask = true;
      
      
   
       
       
 
       
       //resource vector
       for(int i=0; i<100;i++)
       {
           this->res_verctor[i].free = true; //free
       }
       
 
       //interrupt..       
       for(int i=0; i<MAX_GXH_MESSAGE_INTERRUPT;i++)
       {
           this->gxh_msg_interrupt_buf[i].status = 1; //free
       }
       
       //response..
       for(int i=0; i<MAX_GXH_MESSAGE_INTERRUPT;i++)
       {
           this->gxh_msg_response_buf[i].status = 1; //free
       }
       
       for(int i=0; i<MAX_HISTORY_BUFFER;i++)
       {
           this->histories[i].free = true;
           this->histories[i].toSave = false;
       }       
       this->registerResource(this->histories, MAX_HISTORY_BUFFER, RES_VECTOR_HISTORY);
    
        
      
       //gxh handles..
       for(int i=0; i<MAX_GXH_HANDLE_LIST;i++)
       {
           this->gxh_list[i].status = 1; //free
       }
       
       
       //notice...
       for(int i=0; i<MAX_NOTICE_CACHE;i++)
       {
           memset(&this->notices[i], 0, sizeof(ih_notice) );
           this->notices[i].free = true; 
           this->notices[i].readed = false; 
           this->notices[i].px_group_id = 0;
           
           this->notices[i].checkResponse = false;
           
           memset( this->notices[i].user_response,0, sizeof(this->notices[i].user_response) );
           
           this->notices[i].p1_const_function_response = 0;
           this->notices[i].p1_module_function_response = 0;
           this->notices[i].p1_resource_id = 0;
           
           memset( this->notices[i].p1_param1,0, sizeof(this->notices[i].p1_param1) );
           memset( this->notices[i].p1_param2,0, sizeof(this->notices[i].p1_param2) );
           
           
           this->notices[i].p2_const_function_response = 0;
           this->notices[i].p2_module_function_response = 0;
           this->notices[i].p2_resource_id = 0;
           
           memset( this->notices[i].p2_param1,0, sizeof(this->notices[i].p2_param1) );
           memset( this->notices[i].p2_param2,0, sizeof(this->notices[i].p2_param2) );
     
       }
       this->registerResource(this->notices, MAX_NOTICE_CACHE, RES_VECTOR_NOTICE);
       
       
       
         
       
       
       //modules..
       this->modules_count       = 0;       
       this->port_function_count = 0;    
       this->resource_function_count = 0;    
       this->sectors_count       = 0;
       this->zones_count         = 0;
       this->functions_count         = 0;
       
       
       for(int m=0;m<MAX_MODULE_BUFFER;m++) 
       {
           this->modules[m].ready = 0;         //moduł domyślnie niezaładowany..
           this->modules[m].module_name = "?";
           this->modules[m].module_function_count = 0;  //no reference function..
       }
       this->loadModule(); //load modules and function...
       
       this->computer_ports_count = 0;
       this->driver_ports_count   = 0;
       this->drivers_count        = 0;
       this->sldriver_count       = 0;
       this->driver_api_count     = 0;
       this->driver_image_count   = 0;
       this->driver_value_count   = 0;
       
       sleep(1); //niech połączenia do bazy zdążą się zamknąć.. by uniknać [loadPorts, msg: disk I/O error.[0m]
       this->loadPorts();
       
       
        //global settings for acutal verison.....................................
        //zmienne systemowe dostepne w tej wersji..
        const char* ver = "5";
        
        
          this->createBase(); // utworzy pliki bazy danych.. jeśi nieistnieje..
          
          this->clearDataBaseTmp(); //wyczyść dane tmp..
          
     
          
          
        
         //utworz baze danych..
          this->createTable("CREATE TABLE IF NOT EXISTS ih_adv_function(id INTEGER PRIMARY KEY, name TEXT, created_at TEXT, updated_at TEXT, is_delete INTEGER, description TEXT, all_ok INTEGER, all_nok INTEGER, active INTEGER);", false);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_scene_action(id INTEGER PRIMARY KEY, ih_scene_id INTEGER, const_function_symbol INTEGER, resource_id INTEGER, created_at TEXT, updated_at TEXT, is_delete INTEGER, order_by INTEGER, delay_from_start_ms INTEGER, name TEXT, text_content TEXT );", false);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_adv_function_in_out(id INTEGER PRIMARY KEY, ih_adv_function_id INTEGER, type_in_out INTEGER, ih_lighting_id INTEGER, ih_blind_id INTEGER, ih_sensor_id INTEGER, ih_gate_id INTEGER, ih_driver_values_id INTEGER, wartosc_jest_rowna INTEGER, wartosc_jest_mniejsza INTEGER, wartosc_jest_wieksza INTEGER, wartosc TEXT, created_at TEXT, updated_at TEXT, is_delete INTEGER, const_function_symbol INTEGER, ih_scene_id INTEGER, ih_driver_port_id INTEGER, name TEXT, text_content TEXT, px_group_id INTEGER, resource_id INTEGER, typ_wartosci INTEGER, msg_module_function_response INTEGER, msg_const_function_response INTEGER, msg_resource_id INTEGER, msg_notice_type INTEGER, msg_param1 TEXT, msg_param2 TEXT, ih_outs_id );", false);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_sector(id INTEGER PRIMARY KEY, name TEXT, type TEXT, ih_home_id INTEGER, created_at TEXT, updated_at TEXT, is_delete INTEGER, screen TEXT, orderby TEXT, visible TEXT);", false);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_adv_var(id INTEGER PRIMARY KEY, ih_adv_function_id INTEGER, type_ion TEXT, resource_name TEXT, resource_id INTEGER, wartosc_jest_rowna INTEGER, wartosc_jest_wieksza INTEGER, wartosc_jest_mniejsza INTEGER, wartosc TEXT, typ_wartosci TEXT, out_const_function_symbol INTEGER, out_resource_id INTEGER, out_param1 TEXT, out_param2 TEXT, created_at TEXT, updated_at TEXT, is_delete INTEGER, p1_const_function_symbol INTEGER, p1_resource_id INTEGER, p1_param1 TEXT, p1_param2 TEXT, p2_const_function_symbol INTEGER, p2_resource_id INTEGER, p2_param1 TEXT, p2_param2 TEXT);", false);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_sensor(id INTEGER PRIMARY KEY, name TEXT, ih_zone_id INTEGER, status TEXT, status_time TEXT, created_at TEXT, updated_at TEXT, is_delete INTEGER, symbol TEXT, px_left INTEGER, px_top INTEGER, analogowy INTEGER, wartosc TEXT, ih_driver_values_id INTEGER);", false);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_beacon(id INTEGER PRIMARY KEY,code TEXT, name TEXT, ih_zone_id INTEGER, details TEXT, created_at TEXT, updated_at TEXT, is_delete INTEGER, auto_find INTEGER, uid TEXT );", false);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_short(id INTEGER PRIMARY KEY, px_group_id INTEGER, const_function_symbol INTEGER, resource_id INTEGER, name TEXT, text_content TEXT, created_at TEXT, updated_at TEXT, is_delete INTEGER );", false);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_blind(id INTEGER PRIMARY KEY,name TEXT, ih_zone_id INTEGER, created_at TEXT, updated_at TEXT, is_delete INTEGER, open_percent INTEGER, status_time TEXT, px_top INTEGER, px_left INTEGER, last_operation TEXT, orderby INTEGER );", false);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_sl_const_function_hash(id INTEGER PRIMARY KEY, name TEXT, symbol TEXT, created_at TEXT, updated_at TEXT, is_delete INTEGER);", false);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_camers(id INTEGER PRIMARY KEY, ih_home_id INTEGER, name TEXT, ip TEXT, login TEXT, password TEXT, channel TEXT, extra_param TEXT, protocol TEXT, created_at TEXT, updated_at TEXT, is_delete INTEGER, url TEXT, videophone INTEGER, local_port TEXT, remote_port TEXT );", false);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_sl_const_function_param(id INTEGER PRIMARY KEY, resource_name TEXT, resource_id INTEGER, ih_sl_const_function_symbol_id INTEGER, extra_param TEXT, created_at TEXT, updated_at TEXT, is_delete INTEGER, extra_param2 TEXT);", false);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_computer(id INTEGER PRIMARY KEY, ih_home_id INTEGER, name TEXT, system TEXT, soft_version TEXT, created_at TEXT, updated_at TEXT, is_delete INTEGER, soft_is_running INTEGER, screen TEXT, lan_interface TEXT, wifi TEXT, ip TEXT);", false);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_sl_const_function_symbol(id INTEGER PRIMARY KEY, description TEXT, const_function_symbol INTEGER, created_at TEXT, updated_at TEXT, is_delete INTEGER, type TEXT, module TEXT, order_by INTEGER, used TEXT, used_from_mobile TEXT, run_as_service TEXT, run_ms_delay INTEGER, author_name TEXT, author_firm TEXT, author_email TEXT, author_phone TEXT, author_www TEXT, author_key TEXT, app_min_version TEXT, info TEXT, logo TEXT, param2_name CHAR(255), param1_name CHAR(255));", false);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_computer_port(id INTEGER PRIMARY KEY, name_on_computer TEXT, path TEXT, typ TEXT, ih_computer_id INTEGER, created_at TEXT, updated_at TEXT, is_delete INTEGER, parity TEXT, stop_bit TEXT, speed INTEGER, data TEXT, is_active INTEGER );", false);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_sl_driver(id INTEGER PRIMARY KEY, name TEXT, created_at TEXT, updated_at TEXT, is_delete INTEGER, const_function_driver INTEGER, screen TEXT, name_firm TEXT, www_firm TEXT, module TEXT, author TEXT, driver_symbol TEXT, number_of_input INTEGER, number_of_output INTEGER, installed INTEGER,is_modul_io INTEGER,app_min_version TEXT, info TEXT, vars TEXT, menu TEXT, menu_ikon TEXT, pr_default TEXT, default_addres TEXT, default_speed TEXT, default_data_bit TEXT, default_bit_stop TEXT, default_parity TEXT, default_delay_ms INTEGER, api TEXT);", false);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_driver(id INTEGER PRIMARY KEY, address TEXT, ih_sl_driver_id INTEGER, ih_computer_port_id INTEGER, created_at TEXT, updated_at TEXT, is_delete INTEGER, readed_value TEXT, name TEXT, last_active_read TEXT, parity TEXT, stop_bit TEXT, speed INTEGER, data TEXT, ih_zone_id INTEGER, installed INTEGER, px_left INTEGER, px_top INTEGER, delay_before_read_ms INTEGER, is_active INTEGER);", false);
          // this->createTable("CREATE TABLE IF NOT EXISTS ih_sl_driver_api(id INTEGER PRIMARY KEY,ih_driver_id INTEGER,control_type TEXT, ih_driver_values_id INTEGER, label_name TEXT, button_name TEXT, value_type TEXT, func_number INTEGER, func_internal_number INTEGER, select_val TEXT, replace_val TEXT, created_at TEXT, updated_at TEXT, is_delete INTEGER, pr_standard TEXT, pr_standard_plus TEXT, pr_administrator TEXT, pr_system TEXT );", false);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_driver_group_hash(id INTEGER PRIMARY KEY, ih_driver_id INTEGER, px_group_id INTEGER, created_at TEXT,updated_at TEXT, is_delete INTEGER);", false);
          // this->createTable("CREATE TABLE IF NOT EXISTS ih_sl_driver_image(id INTEGER PRIMARY KEY,ih_sl_driver_id INTEGER, name TEXT, path TEXT, created_at TEXT, updated_at TEXT, is_delete INTEGER );", false);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_driver_port(id INTEGER PRIMARY KEY, num INTEGER, type TEXT, ih_driver_id INTEGER, created_at TEXT, updated_at TEXT, is_delete INTEGER, status_input TEXT, status_output TEXT, status_time TEXT, reverts INTEGER);", false);
          // this->createTable("CREATE TABLE IF NOT EXISTS ih_sl_driver_interface(id INTEGER PRIMARY KEY,ih_sl_driver_id INTEGER, var TEXT, action_function TEXT, button TEXT, select_val TEXT, ifvar TEXT, chart TEXT, created_at TEXT, updated_at TEXT, is_delete INTEGER, showifvar TEXT, admin TEXT, module_function TEXT );", false);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_driver_port_function(id INTEGER PRIMARY KEY,ih_driver_port_id INTEGER,const_function_symbol INTEGER, resource_id INTEGER, created_at TEXT, updated_at TEXT, is_delete INTEGER, port_type INTEGER, switch_delay_ms INTEGER, time_to_off_ms INTEGER, delay_from_start_ms INTEGER, repeat INTEGER, name TEXT, text_content TEXT );", false);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_switch(id INTEGER PRIMARY KEY, name TEXT, ih_zone_id INTEGER, created_at TEXT, updated_at TEXT, is_delete INTEGER);", false);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_driver_values(id INTEGER PRIMARY KEY, ih_driver_id INTEGER, name_param TEXT, value TEXT, created_at TEXT, updated_at TEXT, is_delete INTEGER, const_function INTEGER, px_group_id INTEGER );", false);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_task(id INTEGER PRIMARY KEY,const_function_symbol INTEGER, resource_id INTEGER, mon TEXT, tue TEXT, wed TEXT, thu TEXT, fri TEXT, sat TEXT, sun TEXT, every_week TEXT, last_work TEXT, system TEXT, created_at TEXT, updated_at TEXT, is_delete  INTEGER, work_time TEXT, name TEXT, text_content TEXT, px_group_id INTEGER);", false);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_dynamic_menu(id INTEGER PRIMARY KEY, name TEXT, ikon_name TEXT, unique_action_name TEXT, access_from_privileges_group TEXT, created_at TEXT, updated_at TEXT, is_delete INTEGER, ih_driver_id INTEGER );", false);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_zone(id INTEGER PRIMARY KEY,name TEXT, ih_sector_id INTEGER, type TEXT,created_at TEXT, updated_at TEXT, is_delete INTEGER, screen TEXT, orderby INTEGER, visible TEXT  );", false);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_gate(id INTEGER PRIMARY KEY, name TEXT, from_ih_zone_id INTEGER, to_ih_zone_id INTEGER, created_at TEXT, updated_at TEXT, is_delete INTEGER, open_percent INTEGER, status_time TEXT, px_top INTEGER, px_left INTEGER, last_operation TEXT, type NUMERIC);", false);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_zone_privilege(id INTEGER PRIMARY KEY, px_group_id INTEGER, ih_zone_id INTEGER, is_delete INTEGER, created_at TEXT, updated_at TEXT, description TEXT);", false);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_home(id INTEGER PRIMARY KEY, name TEXT, address_1 TEXT, address_2 TEXT, zip_code TEXT, city TEXT, global_cloud_id INTEGER, owner_name TEXT, owner_contact TEXT, created_at TEXT, updated_at TEXT, is_delete INTEGER, latitude TEXT, longitude TEXT, first_config_completed INTEGER, config_step INTEGER, product_key TEXT, product_active TEXT, diler_code TEXT );", false);
          this->createTable("CREATE TABLE IF NOT EXISTS plg_pliki(id INTEGER PRIMARY KEY,typ TEXT, typ_id INTEGER, created_at TEXT, updated_at TEXT, is_delete INTEGER, typ_pliku TEXT, crypt TEXT, expired_to TEXT, info_dodatkowe TEXT, nazwa_wlasna_pliku TEXT,nazwa_pliku TEXT, sciezka_do_pliku TEXT, nazwa_widoczna TEXT  );", false);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_lighting(id INTEGER PRIMARY KEY, name TEXT, ih_zone_id INTEGER, created_at TEXT, updated_at TEXT, is_delete INTEGER, status INTEGER, status_time TEXT, px_top TEXT, px_left TEXT,wat TEXT, orderby INTEGER);", false);
          this->createTable("CREATE TABLE IF NOT EXISTS px_group(id INTEGER PRIMARY KEY, nick TEXT, name TEXT, last_name TEXT, created_at TEXT, updated_at TEXT, is_delete INTEGER, px_group_id INTEGER, ulica TEXT, kod_pocztowy TEXT, miasto TEXT, typ TEXT, domyslny_parking INTEGER, telefon_klienta TEXT, telefon_kontaktowy TEXT, info_dodatkowe TEXT, ih_home_id INTEGER, latitude NUMERIC,longitude NUMERIC, last_online TEXT, data_profil TEXT, awatar TEXT);", false);          
          this->createTable("CREATE TABLE IF NOT EXISTS px_group_beacon(id INTEGER PRIMARY KEY,px_group_id INTEGER, ih_beacon_id INTEGER, is_near INTEGER, created_at TEXT, updated_at TEXT, is_delete INTEGER, distance NUMERIC );", false);    
          this->createTable("CREATE TABLE IF NOT EXISTS px_hash_privilege(id INTEGER PRIMARY KEY,px_user_id INTEGER,px_privileges_group_id INTEGER, created_at TEXT, updated_at TEXT, is_delete INTEGER );", false);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_outs(id INTEGER PRIMARY KEY, name TEXT, ih_zone_id INTEGER, created_at TEXT, updated_at TEXT, is_delete INTEGER, status INTEGER, orderby INTEGER);", false);
          this->createTable("CREATE TABLE IF NOT EXISTS px_logs(id INTEGER PRIMARY KEY,ip TEXT, browser TEXT, system TEXT, px_user_id INTEGER, created_at TEXT, updated_at TEXT, is_delete INTEGER, operation TEXT, proxy TEXT, http_x_forward_for TEXT );", false);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_privileges_group(id INTEGER PRIMARY KEY, name TEXT, description TEXT, ih_home_id INTEGER, created_at TEXT, updated_at TEXT, is_delete INTEGER);", false);
          this->createTable("CREATE TABLE IF NOT EXISTS px_privilege_list(id INTEGER PRIMARY KEY,privilege_number INTEGER, controller TEXT, action TEXT, created_at TEXT, updated_at TEXT, is_delete INTEGER, privilege_number_top INTEGER, menu_visible TEXT, menu_name TEXT, description TEXT, position TEXT, global_title TEXT, icon TEXT );", false);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_privileges_group_zone(id INTEGER PRIMARY KEY, ih_privileges_group_id INTEGER, ih_zone_id INTEGER, created_at TEXT, updated_at TEXT, is_delete INTEGER);", false);
          this->createTable("CREATE TABLE IF NOT EXISTS px_privileges_group(id INTEGER PRIMARY KEY,privileges TEXT, description TEXT, created_at TEXT, updated_at TEXT, is_delete INTEGER, name TEXT, framework_user_info TEXT );", false);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_privileges_hash(id INTEGER PRIMARY KEY,px_group_id INTEGER, ih_privileges_group_id INTEGER, ih_home_id INTEGER, created_at TEXT, updated_at TEXT, is_delete INTEGER );", false);
          this->createTable("CREATE TABLE IF NOT EXISTS px_user(id INTEGER PRIMARY KEY,username TEXT, nick TEXT, password TEXT, salt TEXT, group_type TEXT, crypt TEXT, created_at TEXT, updated_at TEXT, is_delete INTEGER, is_active INTEGER, admin INTEGER, group_id INTEGER, opis TEXT, haslo_encode TEXT );", false);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_scene(id INTEGER PRIMARY KEY,name TEXT, created_at TEXT, updated_at TEXT, is_delete INTEGER, ih_home_id INTEGER, px_group_id INTEGER );", false);
          
          this->createTable("CREATE TABLE IF NOT EXISTS ih_doors(id INTEGER PRIMARY KEY, name TEXT, ih_zone_id INTEGER, created_at TEXT, updated_at TEXT, is_delete INTEGER, status INTEGER,type INTEGER, orderby INTEGER);", false);          
          this->createTable("CREATE TABLE IF NOT EXISTS ih_resource_function(id INTEGER PRIMARY KEY, in_resource_id INTEGER, in_const_function_symbol INTEGER, resource_id INTEGER, const_function_symbol INTEGER, created_at TEXT, updated_at TEXT, is_delete INTEGER, port_type INTEGER, switch_delay_ms INTEGER, time_to_off_ms INTEGER, delay_from_start_ms INTEGER, repeat INTEGER, name TEXT, text_content TEXT,info TEXT );", false);          
          
          this->createTable("CREATE TABLE IF NOT EXISTS ih_nfc_card(id INTEGER PRIMARY KEY, code TEXT, name TEXT, type TEXT, px_group_id INTEGER, is_active INTEGER, created_at TEXT, updated_at TEXT, is_delete INTEGER);", false);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_nfc_card_driver(id INTEGER PRIMARY KEY, ih_nfc_card_id INTEGER, ih_driver_id INTEGER, created_at TEXT, updated_at TEXT, is_delete INTEGER);", false);          
          this->createTable("CREATE TABLE IF NOT EXISTS ih_nfc_card_func(id INTEGER PRIMARY KEY, ih_nfc_card_id INTEGER, type INTEGER,const_function_symbol INTEGER, resource_id INTEGER,port_type INTEGER, switch_delay_ms INTEGER, time_to_off_ms INTEGER, delay_from_start_ms INTEGER, repeat INTEGER, name TEXT, text_content TEXT, created_at TEXT, updated_at TEXT, is_delete INTEGER);", false);          
          //this->createTable("", false);
          
          this->createTable("CREATE TABLE IF NOT EXISTS ih_driver_port_history(id INTEGER PRIMARY KEY, ih_driver_port_id INTEGER, created_at TEXT, updated_at TEXT,is_delete INTEGER, status INTEGER);", true);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_history(id INTEGER PRIMARY KEY, app_www TEXT, const_function_symbol INTEGER, resource_id INTEGER, px_group_id INTEGER, px_user_id INTEGER, created_at TEXT, updated_at TEXT, is_delete INTEGER, name TEXT, text_content TEXT, status_from TEXT, status_to TEXT, info TEXT);", true);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_notice(id INTEGER PRIMARY KEY, px_group_id INTEGER, readed INTEGER, type INTEGER, title TEXT, content TEXT, created_at TEXT, updated_at TEXT, is_delete INTEGER, user_response TEXT, module_function_response INTEGER, const_function_response INTEGER, resource_id INTEGER, msg_from TEXT, msg_param1 TEXT, msg_param2 TEXT);", true);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_driver_values_history(id INTEGER PRIMARY KEY, ih_driver_values_id INTEGER, value TEXT, created_at TEXT,updated_at TEXT, is_delete INTEGER);", true);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_lighting_h(id INTEGER PRIMARY KEY, ih_lighting_id INTEGER, status TEXT,czas_sek INTEGER, wat INTEGER, created_at TEXT, updated_at TEXT, is_delete INTEGER );", true);
          this->createTable("CREATE TABLE IF NOT EXISTS px_session(id INTEGER PRIMARY KEY, session_id TEXT, px_user_id INTEGER, operation TEXT, created_at TEXT, is_delete INTEGER);", true);
          this->createTable("CREATE TABLE IF NOT EXISTS ih_nfc_card_event(id INTEGER PRIMARY KEY, ih_nfc_card_id INTEGER, ih_driver_id INTEGER, px_group_id INTEGER, active INTEGER, code TEXT , type TEXT ,updated_at TEXT, created_at TEXT, is_delete INTEGER);", true);
          
          
          //od wersji 5.0.10
          this->createTable("CREATE TABLE IF NOT EXISTS ih_alarm_zone(id INTEGER PRIMARY KEY, name TEXT, type TEXT ,updated_at TEXT, created_at TEXT, is_delete INTEGER);", false);
          this->createTable("ALTER TABLE ih_alarm_zone ADD COLUMN active INTEGER;", false);
          this->createTable("ALTER TABLE ih_alarm_zone ADD COLUMN on_type INTEGER;", false); //typ włączonego alarmu lub '0'
          this->createTable("ALTER TABLE ih_alarm_zone ADD COLUMN on_time TEXT;", false); //kiedy alarm został włączony
          this->createTable("ALTER TABLE ih_alarm_zone ADD COLUMN on_ih_alarm_device_id INTEGER;", false); //kiedy alarm został włączony
          this->createTable("CREATE TABLE IF NOT EXISTS ih_alarm_device(id INTEGER PRIMARY KEY, name TEXT,ih_alarm_zone_id INTEGER, type TEXT ,updated_at TEXT, created_at TEXT, is_delete INTEGER);", false);                   
          
          this->createTable("ALTER TABLE ih_resource_function ADD COLUMN time_active_from TEXT;", false); //w jakis godzinach jest ta funkcja aktywna..
          this->createTable("ALTER TABLE ih_resource_function ADD COLUMN time_active_to TEXT;", false);   //w jakis godzinach jest ta funkcja aktywna..           
          this->createTable("ALTER TABLE ih_resource_function ADD COLUMN mon INTEGER;", false);   //w jakis godzinach jest ta funkcja aktywna..
          this->createTable("ALTER TABLE ih_resource_function ADD COLUMN tue INTEGER;", false);   //w jakis godzinach jest ta funkcja aktywna..
          this->createTable("ALTER TABLE ih_resource_function ADD COLUMN wed INTEGER;", false);   //w jakis godzinach jest ta funkcja aktywna..
          this->createTable("ALTER TABLE ih_resource_function ADD COLUMN thu INTEGER;", false);   //w jakis godzinach jest ta funkcja aktywna..
          this->createTable("ALTER TABLE ih_resource_function ADD COLUMN fri INTEGER;", false);   //w jakis godzinach jest ta funkcja aktywna..
          this->createTable("ALTER TABLE ih_resource_function ADD COLUMN sat INTEGER;", false);   //w jakis godzinach jest ta funkcja aktywna..
          this->createTable("ALTER TABLE ih_resource_function ADD COLUMN sun INTEGER;", false);   //w jakis godzinach jest ta funkcja aktywna..
          
          //wprowadzenie obsługi TIMERów 5.0.10 
          this->createTable("CREATE TABLE IF NOT EXISTS ih_timer(id INTEGER PRIMARY KEY, name TEXT, sec INTEGER , max_sec INTEGER, updated_ts INTEGER, active INTEGER ,updated_at TEXT, created_at TEXT, is_delete INTEGER);", false);

          
          //this->createTable("", true);
          
           //tutaj dodamy sesje dla asteriska.. na stałe          
           std::string query = "INSERT INTO px_session (id, session_id, px_user_id, operation,created_at, is_delete) VALUES (null,'";
           query.append("S_192_GXH_ANDROID_1003701131");
           query.append("','");
           query.append(gxh_IntToString(1131));
           query.append("','login',DATETIME('now','localtime'),'0'); ");
          
            this->createTable(query.c_str(), true);
          ///------------------------
          
          
          
          
       
          this->registerVarSys("Internet status", VAR_INTERNET_STATUS , -1, -1, "1" );
          this->registerVarSys("Czas letni - zimowy", VAR_CZAS_LETNI_ZIMOWY , -1, -1, "N" );
          this->registerVarSys("Czas zachodu słońca", VAR_CZAS_ZACHODU_SLONCA , -1, -1, "20:00" );
          this->registerVarSys("Czas wschodu słońca", VAR_CZAS_WSCHODU_SLONCA , -1, -1, "06:00" );
          this->registerVarSys("Temperatura w cieniu", VAR_TEMP_W_CIENIU , -1, -1, "12" );
          this->registerVarSys("Możliwość opadów (w %)", VAR_MOZLIWOSC_OPADOW , -1, -1, "0" );
          this->registerVarSys("Wilogotność powietrza (w %)", VAR_WILGOTNOSC_POWIETRZA , -1, -1, "0" );          
          this->registerVarSys("Aktualna wersja oprogramowania", VAR_VERSION_NOW , -1, -1, this->getVersion().c_str() );
          this->registerVarSys("Dostępna wersja oprogramowania", VAR_VERSION_NEW , -1, -1, this->getVersion().c_str() );
          this->registerVarSys("Aktualizacje gotowe do pobrania", VAR_VERSION_ISNEW , -1, -1,  "1" );          
          this->registerVarSys("Ilość włączonych świateł", VAR_LIGHT_ON_IN_HOME , -1, -1,  "0" );
 
       
          
       
          
          //funciton....
          bool addNew = false;
          
          //wsdl
          if(this->registerFunctionSys( 2, "Obsługa wsdla", "P", "app_module_wsdl.so", "0", "0", "none", "none", "other", ver))addNew = true;
          
          //app_module_system.so
          if(this->registerFunctionSys( 9, "Restart oświetlenia", "P", "app_module_system.so", "0", "-1", "none", "none", "other", ver))addNew = true;
          if(this->registerFunctionSys( 10, "Restart komputera", "I", "app_module_system.so", "0", "0", "none", "none", "other", ver))addNew = true;
          if(this->registerFunctionSys( 12, "Wykonaj kopie zapasową", "I", "app_module_system.so", "0", "0", "none", "none", "other", ver))addNew = true;
          if(this->registerFunctionSys( 13, "Załaduj kopie zapasową", "I", "app_module_system.so", "0", "0", "none", "none", "other", ver))addNew = true;
          if(this->registerFunctionSys( 14, "Sprawdź aktualizacje", "I", "app_module_system.so", "1", "1200000", "none", "none", "other", ver))addNew = true;
          if(this->registerFunctionSys( 15, "Aktualizuj system", "I", "app_module_system.so", "0", "0", "none", "none", "other", ver))addNew = true;
          if(this->registerFunctionSys( 16, "Sprawdz zaawansowane funkcje zdarzeń", "P", "app_module_system.so", "1", "2000", "none", "none", "other", ver))addNew = true;
          if(this->registerFunctionSys( 17, "Sprzątaj system, stare logi, błędy, cache", "P", "app_module_system.so", "1", "130000", "none", "none", "other", ver))addNew = true;
          if(this->registerFunctionSys( 18, "Ustaw stan wysoki na porcie", "I", "app_module_system.so", "0", "0", "none", "none", "output", ver))addNew = true;
          if(this->registerFunctionSys( 19, "Ustaw stan niski na porcie", "I", "app_module_system.so", "0", "0", "none", "none", "output", ver))addNew = true;
          if(this->registerFunctionSys( 20, "Kliknięcie na porcie", "I", "app_module_system.so", "0", "0", "none", "none", "output", ver))addNew = true;
          if(this->registerFunctionSys( 21, "Sprawdź status połączenia z internetem", "P", "app_module_system.so", "1", "60000", "none", "none", "other", ver))addNew = true;
          if(this->registerFunctionSys( 22, "Pytanie Tak/Nie", "I", "app_module_system.so", "0", "0", "Tytuł", "Treść", "user", ver))addNew = true;
          if(this->registerFunctionSys( 23, "Powiadomienie o nowej wersji", "I", "app_module_system.so", "0", "0", "Tytuł", "Treść", "user", ver))addNew = true;
          if(this->registerFunctionSys( 24, "Powiadomienie - wiadomość", "I", "app_module_system.so", "0", "0", "Tytuł", "Treść", "user", ver))addNew = true;
          
          if(this->registerFunctionSys( 30, "Ustaw stan wysoki 1 sek.", "I", "app_module_system.so", "0", "0", "none", "none", "output", ver))addNew = true;
          if(this->registerFunctionSys( 31, "Ustaw stan wysoki 2 sek.", "I", "app_module_system.so", "0", "0", "none", "none", "output", ver))addNew = true;
          if(this->registerFunctionSys( 32, "Ustaw stan wysoki 3 sek.", "I", "app_module_system.so", "0", "0", "none", "none", "output", ver))addNew = true;
          if(this->registerFunctionSys( 33, "Ustaw stan wysoki 4 sek.", "I", "app_module_system.so", "0", "0", "none", "none", "output", ver))addNew = true;
          if(this->registerFunctionSys( 34, "Ustaw stan wysoki 5 sek.", "I", "app_module_system.so", "0", "0", "none", "none", "output", ver))addNew = true;
          if(this->registerFunctionSys( 35, "Ustaw stan wysoki 6 sek.", "I", "app_module_system.so", "0", "0", "none", "none", "output", ver))addNew = true;
          if(this->registerFunctionSys( 36, "Ustaw stan wysoki 7 sek.", "I", "app_module_system.so", "0", "0", "none", "none", "output", ver))addNew = true;
          if(this->registerFunctionSys( 37, "Ustaw stan wysoki 8 sek.", "I", "app_module_system.so", "0", "0", "none", "none", "output", ver))addNew = true;
          if(this->registerFunctionSys( 38, "Ustaw stan wysoki 9 sek.", "I", "app_module_system.so", "0", "0", "none", "none", "output", ver))addNew = true;
          if(this->registerFunctionSys( 39, "Ustaw stan wysoki 10 sek.", "I", "app_module_system.so", "0", "0", "none", "none", "output", ver))addNew = true;
          
          if(this->registerFunctionSys( 50, "Ustaw stan wysoki 1 min.", "I", "app_module_system.so", "0", "0", "none", "none", "output", ver))addNew = true;
          if(this->registerFunctionSys( 51, "Ustaw stan wysoki 2 min.", "I", "app_module_system.so", "0", "0", "none", "none", "output", ver))addNew = true;
          if(this->registerFunctionSys( 52, "Ustaw stan wysoki 3 min.", "I", "app_module_system.so", "0", "0", "none", "none", "output", ver))addNew = true;
          if(this->registerFunctionSys( 53, "Ustaw stan wysoki 4 min.", "I", "app_module_system.so", "0", "0", "none", "none", "output", ver))addNew = true;
          if(this->registerFunctionSys( 54, "Ustaw stan wysoki 5 min.", "I", "app_module_system.so", "0", "0", "none", "none", "output", ver))addNew = true;
          if(this->registerFunctionSys( 55, "Ustaw stan wysoki 6 min.", "I", "app_module_system.so", "0", "0", "none", "none", "output", ver))addNew = true;
          if(this->registerFunctionSys( 56, "Ustaw stan wysoki 7 min.", "I", "app_module_system.so", "0", "0", "none", "none", "output", ver))addNew = true;
          if(this->registerFunctionSys( 57, "Ustaw stan wysoki 8 min.", "I", "app_module_system.so", "0", "0", "none", "none", "output", ver))addNew = true;
          if(this->registerFunctionSys( 58, "Ustaw stan wysoki 9 min.", "I", "app_module_system.so", "0", "0", "none", "none", "output", ver))addNew = true;
          if(this->registerFunctionSys( 59, "Ustaw stan wysoki 10 min.", "I", "app_module_system.so", "0", "0", "none", "none", "output", ver))addNew = true;
          
          //app_module_light.so
          if(this->registerFunctionSys( 100, "Włącznik oświetlenia", "I", "app_module_light.so", "0", "0", "none", "none", "light", ver))addNew = true;
          if(this->registerFunctionSys( 101, "Wyjście oświetlenia", "I", "app_module_light.so", "0", "0", "none", "none", "light", ver))addNew = true;
          if(this->registerFunctionSys( 102, "Wymuś wyłaczenie oświetlenia", "I", "app_module_light.so", "0", "0", "none", "none", "light", ver))addNew = true;
          if(this->registerFunctionSys( 103, "Wymuś właczenie oświetlenia", "I", "app_module_light.so", "0", "0", "none", "none", "light", ver))addNew = true;
          if(this->registerFunctionSys( 104, "Zapal wszystkie światła w domu", "I", "app_module_light.so", "0", "0", "none", "none", "home", ver))addNew = true;
          if(this->registerFunctionSys( 105, "Zgaś wszystkie światła w domu", "I", "app_module_light.so", "0", "0", "none", "none", "home", ver))addNew = true;
          if(this->registerFunctionSys( 106, "Zapal wszystkie światła w sektorze (piętrze)", "I", "app_module_light.so", "0", "0", "none", "none", "sector", ver))addNew = true;
          if(this->registerFunctionSys( 107, "Zgaś wszystkie światła w sektorze (piętrze)", "I", "app_module_light.so", "0", "0", "none", "none", "sector", ver))addNew = true;
          if(this->registerFunctionSys( 108, "Zapal wszystkie światła w strefie (pomieszczeniu)", "I", "app_module_light.so", "0", "0", "none", "none", "zone", ver))addNew = true;
          if(this->registerFunctionSys( 109, "Zgaś wszystkie światła w strefie (pomieszczeniu)", "I", "app_module_light.so", "0", "0", "none", "none", "zone", ver))addNew = true;
          if(this->registerFunctionSys( 110, "Zapalono światło", "P", "app_module_light.so", "0", "0", "none", "none", "light", ver))addNew = true;
          if(this->registerFunctionSys( 111, "Zgaszono światło", "P", "app_module_light.so", "0", "0", "none", "none", "light", ver))addNew = true;
          
          //app_module_blind.so
          if(this->registerFunctionSys( 120, "Włącznik rolet góra", "I", "app_module_blind.so", "0", "0", "none", "none", "blind", ver))addNew = true;
          if(this->registerFunctionSys( 121, "Włącznik rolet dół", "I", "app_module_blind.so", "0", "0", "none", "none", "blind", ver))addNew = true;
          if(this->registerFunctionSys( 122, "Wyjście zamknięcia rolety", "I", "app_module_blind.so", "0", "0", "none", "none", "blind", ver))addNew = true;
          if(this->registerFunctionSys( 123, "Wyjście otwarcia rolety", "I", "app_module_blind.so", "0", "0", "none", "none", "blind", ver))addNew = true;
          if(this->registerFunctionSys( 124, "Potwierdzenie otwarcia rolety", "I", "app_module_blind.so", "0", "0", "none", "none", "blind", ver))addNew = true;
          if(this->registerFunctionSys( 125, "Potwierdzenie zamknięcia rolety", "I", "app_module_blind.so", "0", "0", "none", "none", "blind", ver))addNew = true;
          if(this->registerFunctionSys( 126, "Otwórz wszystkie rolety w domu", "I", "app_module_blind.so", "0", "0", "none", "none", "home", ver))addNew = true;
          if(this->registerFunctionSys( 127, "Zamknij wszystkie rolety w domu", "I", "app_module_blind.so", "0", "0", "none", "none", "home", ver))addNew = true;
          if(this->registerFunctionSys( 128, "Otwórz wszystkie rolety w sektorze (piętrze)", "I", "app_module_blind.so", "0", "0", "none", "none", "sector", ver))addNew = true;
          if(this->registerFunctionSys( 129, "Zamknij wszystkie rolety w sektorze (piętrze)", "I", "app_module_blind.so", "0", "0", "none", "none", "sector", ver))addNew = true;
          if(this->registerFunctionSys( 130, "Otworz wszystkie rolety w strefie (pomieszczeniu)", "I", "app_module_blind.so", "0", "0", "none", "none", "zone", ver))addNew = true;
          if(this->registerFunctionSys( 131, "Zamknij wszystkie rolety w strefie (pomieszczeniu)", "I", "app_module_blind.so", "0", "0", "none", "none", "zone", ver))addNew = true;
          
          
          //app_module_gate.so
          if(this->registerFunctionSys( 140, "Włącznik bramy", "I", "app_module_gate.so", "0", "0", "none", "none", "gate", ver))addNew = true;
          if(this->registerFunctionSys( 141, "Wyjście otwarcia bramy", "I", "app_module_gate.so", "0", "0", "none", "none", "gate", ver))addNew = true;
          if(this->registerFunctionSys( 142, "Potwierdzenie otwarcia bramy", "I", "app_module_gate.so", "0", "0", "none", "none", "gate", ver))addNew = true;
          if(this->registerFunctionSys( 143, "Potwierdznie zamkniecia bramy", "I", "app_module_gate.so", "0", "0", "none", "none", "gate", ver))addNew = true;          
          if(this->registerFunctionSys( 144, "Wyjście otwarcia bramy (osobne porty)", "O", "app_module_gate.so", "0", "0", "none", "none", "gate", ver))addNew = true;
          if(this->registerFunctionSys( 145, "Wyjście zamknięcia bramy (osobne porty)", "O", "app_module_gate.so", "0", "0", "none", "none", "gate", ver))addNew = true;
          if(this->registerFunctionSys( 146, "Wymuś otwarcie bramy (osobne porty)", "I", "app_module_gate.so", "0", "0", "none", "none", "gate", ver))addNew = true;
          if(this->registerFunctionSys( 147, "Wymuś zamknięcie bramy (osobne porty)", "I", "app_module_gate.so", "0", "0", "none", "none", "gate", ver))addNew = true; 
          if(this->registerFunctionSys( 148, "Alert - wymuszenie otwarcia bramy", "P", "app_module_gate.so", "0", "0", "none", "none", "gate", ver))addNew = true; 
          if(this->registerFunctionSys( 149, "Alert - wymuszenie zamkniecia bramy", "P", "app_module_gate.so", "0", "0", "none", "none", "gate", ver))addNew = true; 
          
          if(this->registerFunctionSys( 151, "Otwarcie drzwi", "I", "app_module_door.so", "0", "0", "none", "none", "door", ver))addNew = true; 
          if(this->registerFunctionSys( 152, "Wyjście otwarcia drzwi (click)", "O", "app_module_door.so", "0", "0", "none", "none", "door", ver))addNew = true; 
          if(this->registerFunctionSys( 153, "Wyjście otwarcia drzwi (ON 1 sek)", "O", "app_module_door.so", "0", "0", "none", "none", "door", ver))addNew = true; 
          if(this->registerFunctionSys( 154, "Wyjście otwarcia drzwi (ON 5 sek)", "O", "app_module_door.so", "0", "0", "none", "none", "door", ver))addNew = true; 
          if(this->registerFunctionSys( 155, "Wyjście otwarcia drzwi (ON 10 sek)", "O", "app_module_door.so", "0", "0", "none", "none", "door", ver))addNew = true; 
          if(this->registerFunctionSys( 156, "Potwierdzenie otwarcia drzwi", "I", "app_module_door.so", "0", "0", "none", "none", "door", ver))addNew = true; 
          if(this->registerFunctionSys( 157, "Potwierdzenie zamknięcia drzwi", "I", "app_module_door.so", "0", "0", "none", "none", "door", ver))addNew = true; 
          if(this->registerFunctionSys( 158, "Otwarto drzwi - konektor", "P", "app_module_door.so", "0", "0", "none", "none", "door", ver))addNew = true; 
          if(this->registerFunctionSys( 159, "Zamknięto drzwi - konektor", "P", "app_module_door.so", "0", "0", "none", "none", "door", ver))addNew = true; 
          
          
          //app_module_sensor.so
          if(this->registerFunctionSys( 180, "Czujnik typu on/off", "I", "app_module_sensor.so", "0", "0", "none", "none", "sensor", ver))addNew = true;
          
          if(this->registerFunctionSys( 209, "Uzbrojenie cichego alarmu w strefie", "I", "app_module_alarm.so", "0", "0", "none", "none", "alarm_zone", ver))addNew = true;
          if(this->registerFunctionSys( 210, "Uzbrojenie alarmu w strefie", "I", "app_module_alarm.so", "0", "0", "none", "none", "alarm_zone", ver))addNew = true;
          if(this->registerFunctionSys( 211, "Dezaktywacja uzbrojenia alarmu w strefie", "I", "app_module_alarm.so", "0", "0", "none", "none", "alarm_zone", ver))addNew = true;
          if(this->registerFunctionSys( 212, "Uzbrojenie wszystkich stref", "I", "app_module_alarm.so", "0", "0", "none", "none", "other", ver))addNew = true;
          if(this->registerFunctionSys( 213, "Dezaktywacja uzbrojenia we wszystkich strefach", "I", "app_module_alarm.so", "0", "0", "none", "other", "other", ver))addNew = true;
          if(this->registerFunctionSys( 214, "Alert - naruszenie strefy", "I", "app_module_alarm.so", "0", "0", "none", "none", "alarm_zone", ver))addNew = true;
          if(this->registerFunctionSys( 215, "Wyjście aktywacji alarmu w strefie (3 sek ON)", "O", "app_module_alarm.so", "0", "0", "none", "none", "alarm_zone", ver))addNew = true;
          if(this->registerFunctionSys( 216, "Wyjście dezaktywacji alarmu w strefie (3 sek ON)", "O", "app_module_alarm.so", "0", "0", "none", "none", "alarm_zone", ver))addNew = true; 
          if(this->registerFunctionSys( 217, "Alert - naruszenie strefy", "P", "app_module_alarm.so", "0", "0", "none", "none", "alarm_zone", ver))addNew = true;
          if(this->registerFunctionSys( 218, "Alert - wzbudzenie czujki", "P", "app_module_alarm.so", "0", "0", "none", "none", "alarm_device", ver))addNew = true;
          if(this->registerFunctionSys( 219, "Alert - sabotaż urządzenia", "P", "app_module_alarm.so", "0", "0", "none", "none", "alarm_device", ver))addNew = true;
          if(this->registerFunctionSys( 220, "Wejście czujnika ruchu", "I", "app_module_alarm.so", "0", "0", "none", "none", "alarm_device", ver))addNew = true; 
          if(this->registerFunctionSys( 221, "Wejście czujnika dymu", "I", "app_module_alarm.so", "0", "0", "none", "none", "alarm_device", ver))addNew = true; 
          if(this->registerFunctionSys( 222, "Wejście czujnika gazu", "I", "app_module_alarm.so", "0", "0", "none", "none", "alarm_device", ver))addNew = true;
          if(this->registerFunctionSys( 223, "Alarm - Wyjście syreny alarmowej", "O", "app_module_alarm.so", "0", "0", "none", "none", "alarm_device", ver))addNew = true;          
          if(this->registerFunctionSys( 224, "Uruchom syrenę alarmową", "I", "app_module_alarm.so", "0", "0", "none", "none", "alarm_device", ver))addNew = true;
          if(this->registerFunctionSys( 225, "Alert - uruchomiono alarm", "P", "app_module_alarm.so", "0", "0", "none", "none", "alarm_zone", ver))addNew = true;
          if(this->registerFunctionSys( 226, "Alarm został aktywowany", "P", "app_module_alarm.so", "0", "0", "none", "none", "none", ver))addNew = true;
          if(this->registerFunctionSys( 227, "Alarm został dezaktywowany", "P", "app_module_alarm.so", "0", "0", "none", "none", "none", ver))addNew = true;
          if(this->registerFunctionSys( 228, "Wykryto dym", "P", "app_module_alarm.so", "0", "0", "none", "none", "alarm_device", ver))addNew = true;
          if(this->registerFunctionSys( 229, "Wykryto gaz", "P", "app_module_alarm.so", "0", "0", "none", "none", "alarm_device", ver))addNew = true;
          if(this->registerFunctionSys( 230, "Wyłącz syrenę alarmową", "I", "app_module_alarm.so", "0", "0", "none", "none", "alarm_device", ver))addNew = true;
          if(this->registerFunctionSys( 231, "Wejście sabotażu urządzenia", "I", "app_module_alarm.so", "0", "0", "none", "none", "alarm_device", ver))addNew = true;
          if(this->registerFunctionSys( 232, "Alert - naruszenie strefy uzbrojonej", "P", "app_module_alarm.so", "0", "0", "none", "none", "alarm_zone", ver))addNew = true;
          if(this->registerFunctionSys( 233, "Wykryto dym w strefie", "P", "app_module_alarm.so", "0", "0", "none", "none", "alarm_zone", ver))addNew = true;
          if(this->registerFunctionSys( 234, "Wykryto dym w strefie uzbrojonej", "P", "app_module_alarm.so", "0", "0", "none", "none", "alarm_zone", ver))addNew = true;
          if(this->registerFunctionSys( 235, "Wykryto gaz w strefie", "P", "app_module_alarm.so", "0", "0", "none", "none", "alarm_zone", ver))addNew = true;
          if(this->registerFunctionSys( 236, "Wykryto gaz w strefie uzbrojonej", "P", "app_module_alarm.so", "0", "0", "none", "none", "alarm_zone", ver))addNew = true;
          if(this->registerFunctionSys( 237, "Krótki alarm ostrzegawczy - strefa", "P", "app_module_alarm.so", "0", "0", "none", "none", "alarm_zone", ver))addNew = true;
          if(this->registerFunctionSys( 238, "Krótki alarm ostrzegawczy - obiekt", "P", "app_module_alarm.so", "0", "0", "none", "none", "home", ver))addNew = true;
          if(this->registerFunctionSys( 239, "Alarm naruszono jedną ze stref", "P", "app_module_alarm.so", "0", "0", "none", "none", "home", ver))addNew = true;
          
          //app_module_scene.so
          if(this->registerFunctionSys( 240, "Uruchom scenę", "P", "app_module_scene.so", "1", "2500", "Tytuł", "Treść", "other", ver))addNew = true;
          if(this->registerFunctionSys( 241, "Wykonaj akcje o wybranej porze", "P", "app_module_scene.so", "1", "2500", "Tytuł", "Treść", "other", ver))addNew = true;
          
          //app_module_ethernet.so
          if(this->registerFunctionSys( 301, "Wyślij wiadomość e-mail", "I", "app_module_ethernet.so", "0", "0", "Tytuł", "Treść", "user", ver))addNew = true;
          if(this->registerFunctionSys( 302, "Wywołaj link http - skrypt php", "I", "app_module_ethernet.so", "0", "0", "Param 1", "Param 2", "other", ver))addNew = true;
          if(this->registerFunctionSys( 303, "Wyślij wiadomość SMS", "I", "app_module_ethernet.so", "0", "0", "Tytuł", "Treść", "user", ver))addNew = true;          
          
          if(this->registerFunctionSys( 320, "Ustaw stan wysoki gniazdka", "I", "app_module_outs.so", "0", "0", "Tytuł", "Treść", "user", ver))addNew = true;          
          if(this->registerFunctionSys( 321, "Ustaw stan niski gniazdka", "I", "app_module_outs.so", "0", "0", "Tytuł", "Treść", "user", ver))addNew = true;          
          if(this->registerFunctionSys( 322, "Zmień stan na przeciwny gniazdka", "I", "app_module_outs.so", "0", "0", "Tytuł", "Treść", "user", ver))addNew = true;          
          if(this->registerFunctionSys( 323, "Wyjście gniazdka", "O", "app_module_outs.so", "0", "0", "Tytuł", "Treść", "user", ver))addNew = true;          
          
          if(this->registerFunctionSys( 360, "Wyzeruj TIMER", "I", "app_module_timer.so", "0", "0", "Parametr1", "Parametr2", "timer", ver))addNew = true;     
          if(this->registerFunctionSys( 361, "Osiągnięto wartość maksymalną TIMERa", "P", "app_module_timer.so", "0", "0", "Parametr1", "Parametr2", "timer", ver))addNew = true;     
          if(this->registerFunctionSys( 362, "TIMER", "P", "app_module_timer.so", "1", "1000", "Parametr1", "Parametr2", "timer", ver))addNew = true;     
          if(this->registerFunctionSys( 363, "Wschód słońca", "P", "app_module_timer.so", "0", "0", "Parametr1", "Parametr2", "home", ver))addNew = true;  
          if(this->registerFunctionSys( 364, "Zachód słońca", "P", "app_module_timer.so", "0", "0", "Parametr1", "Parametr2", "home", ver))addNew = true;  
          if(this->registerFunctionSys( 365, "Słońce jest najwyżej (ok. 12:00)", "P", "app_module_timer.so", "0", "0", "Parametr1", "Parametr2", "home", ver))addNew = true;  
          if(this->registerFunctionSys( 366, "Godzina do wschódu słońca", "P", "app_module_timer.so", "0", "0", "Parametr1", "Parametr2", "home", ver))addNew = true;  
          if(this->registerFunctionSys( 367, "Godzina do zachód słońca", "P", "app_module_timer.so", "0", "0", "Parametr1", "Parametr2", "home", ver))addNew = true;  
          if(this->registerFunctionSys( 368, "Wschód słońca - żeglarski", "P", "app_module_timer.so", "0", "0", "Parametr1", "Parametr2", "home", ver))addNew = true;  
          if(this->registerFunctionSys( 369, "Zachód słońca - żeglarski", "P", "app_module_timer.so", "0", "0", "Parametr1", "Parametr2", "home", ver))addNew = true;  
          if(this->registerFunctionSys( 370, "Wschód słońca - cywilny", "P", "app_module_timer.so", "0", "0", "Parametr1", "Parametr2", "home", ver))addNew = true;  
          if(this->registerFunctionSys( 371, "Zachód słońca - cywilny", "P", "app_module_timer.so", "0", "0", "Parametr1", "Parametr2", "home", ver))addNew = true;  
          if(this->registerFunctionSys( 372, "Wschód słońca - astronomiczny", "P", "app_module_timer.so", "0", "0", "Parametr1", "Parametr2", "home", ver))addNew = true;  
          if(this->registerFunctionSys( 373, "Zachód słońca - astronomiczny", "P", "app_module_timer.so", "0", "0", "Parametr1", "Parametr2", "home", ver))addNew = true; 
          if(this->registerFunctionSys( 374, "Timer Start (od 00:00)", "I", "app_module_timer.so", "0", "0", "Parametr1", "Parametr2", "timer", ver))addNew = true; 
          if(this->registerFunctionSys( 375, "Timer Stop (ustaw 00:00)", "I", "app_module_timer.so", "0", "0", "Parametr1", "Parametr2", "timer", ver))addNew = true; 
          
          
          if(this->registerFunctionSys( 500, "Zmiana stanu portu, zmiana ustawień modułu techbase", "P", "app_module_techbase.so", "0", "0", "none", "none", "driver", ver))addNew = true;          
          if(this->registerFunctionSys( 520, "Zmiana stanu portu, zmiana ustawień modułu elpiast", "P", "app_module_elpiast.so", "0", "0", "none", "none", "driver", ver))addNew = true;          
                    
          if(this->registerFunctionSys( 1200, "Obsługa kamer IP", "P", "app_module_camera.so", "0", "0", "none", "none", "camera", ver))addNew = true;
          
          if(this->registerFunctionSys( 3100, "Rekuperator PRO-VENT S3", "P", "app_module_provent.so", "0", "0", "none", "none", "driver", ver))addNew = true;
          if(this->registerFunctionSys( 3101, "Włącznik wietrzenia", "I", "app_module_provent.so", "0", "0", "none", "none", "driver", ver))addNew = true;
          if(this->registerFunctionSys( 3102, "Włącznik wentylacji / Zwiększ bieg", "I", "app_module_provent.so", "0", "0", "none", "none", "driver", ver))addNew = true;
          if(this->registerFunctionSys( 3103, "Włącznik wentylacji / Zmniejsz bieg", "I", "app_module_provent.so", "0", "0", "none", "none", "driver", ver))addNew = true;
          
          if(this->registerFunctionSys( 4000, "Obsługa centrali", "P", "app_module_satel.so", "0", "0", "none", "none", "driver", ver))addNew = true;
          if(this->registerFunctionSys( 4001, "Włącz alarm", "I", "app_module_satel.so", "0", "0", "none", "none", "driver", ver))addNew = true;
          if(this->registerFunctionSys( 4002, "Wyłącz alarm", "I", "app_module_satel.so", "0", "0", "none", "none", "driver", ver))addNew = true;
          
       
          
          
          if(addNew)
          {
                    //reload module..
                    this->executeThreadTask = false;                    
                    this->modules_count = 0;
                    this->functions_count = 0;
                    this->symbols_count = 0;
                    this->port_function_count = 0;
                    this->resource_function_count = 0;
                    sleep(2);  //czekaj az usługa się wstrzyma a inne wątki zakończą zadania
                                                  
                    //delete currently use module
                    for(int m=0;m<MAX_MODULE_BUFFER;m++) 
                    {
                        if( this->modules[m].ready == 1 )
                        {
                          this->modules[m].ready = 0;
                          this->modules[m].moduleObject->forceCancel();
                    //    usleep(1000 * 500); //owait 500 ms to cancel currently task
                          delete this->modules[m].moduleObject; 
                          dlclose( this->modules[m].handle );
                          this->modules[m].module_name = "?";
                        }
                    }                                              
                    this->loadModule();
                    this->show_log_core("gxh_core", "gxh_core", "reloadModule", 0);                    
                    this->executeThreadTask = true;
          }
          
       //global settings for acutal verison.....................................
       
       
       
       
       
      this->ethernet = new app_port_ethernet(this,1) ;
      this->ethernet->startLoop();
       
       
       // zadania wykonane po uruchomineie wszystkiego...
      
      int numParams = 0;
      char** params = this->getParams(&numParams);
      
      if(numParams > 3) // ./gxh ++++++++++ ./ Y   ./gxh ++++++++++ ./ N
      {
          if(params[3][0] =='Y')
          {             
              unsigned int count = 0;
              user_gxh* users = (user_gxh*)this->getResource(RES_VECTOR_USER, &count );              
              for(unsigned int u=0; u< count; u++)
              {
                 if(users[u].free == false ) this->addNotice( users[u].px_group_id ,"GXH", "Powiadomienie","System GXH został zaktualizowany", IH_MODULE_FUNCTION_NOTICE_NORMAL, NULL);          
              }
          }
          
          if(params[3][0] =='N')
          {             
              unsigned int count = 0;
              user_gxh* users = (user_gxh*)this->getResource(RES_VECTOR_USER, &count );              
              for(unsigned int u=0; u< count; u++)
              {
                 if(users[u].free == false ) this->addNotice( users[u].px_group_id ,"GXH", "Powiadomienie","Przepraszamy, wystąpił problem z aktualizacją systemu GXH.", IH_MODULE_FUNCTION_NOTICE_NORMAL, NULL);          
              }
          }
          
          if(params[3][0] =='M' && numParams >4 )
          {             
              unsigned int count = 0;
              user_gxh* users = (user_gxh*)this->getResource(RES_VECTOR_USER, &count );              
              for(unsigned int u=0; u< count; u++)
              {
                 if(users[u].free == false ) this->addNotice( users[u].px_group_id ,"GXH", "Powiadomienie", (const char*)params[4] , IH_MODULE_FUNCTION_NOTICE_NORMAL,NULL );          
              }
          }
      }
       //................................................
      
      
      
      //Te procesy uruchamiamy po uruchomineiu wszystkiego...
      //-----------------------
      //-----------------------
      //-----------------------
      pthread_attr_t attr2;
      pthread_attr_init(&attr2);      
      int rc2 = pthread_create(&this->mainThreadService, &attr2,  &goThreadService, (void*)this );
      if (rc2)
       {    
            this->thInterrupt = false;
            this->thService   = false;
            this->thSynchronize = false;
            this->show_error("gxh_core", "gxh_core", "cannot start service thread", 0);
       }else
       {
           pthread_setname_np(this->mainThreadService,"SER");
       }
       pthread_attr_destroy(&attr2);    
       
   
       
       
      pthread_attr_t attr3;
      pthread_attr_init(&attr3);      
      int rc3 = pthread_create(&this->mainThreadSynchronize, &attr3,  &goThreadSynchronize, (void*)this );
      if (rc3)
       {    
            this->thInterrupt = false;
            this->thService   = false;
            this->thSynchronize = false;
            this->show_error("gxh_core", "gxh_core", "cannot start synchronize thread", 0);
       } else
       {
          pthread_setname_np(this->mainThreadSynchronize,"SYN");
       }
      pthread_attr_destroy(&attr3);
                    
       
      if(this->thInterrupt == false || this->thService == false || this->thSynchronize == false )return; //close app..
       
      //-----------------------
      //-----------------------
      //-----------------------    
      
      
       this->show_log_core("gxh_core", "gxh_core", "gxh core started", 0);       
       pthread_join( this->mainThread, NULL); //program will by close when service is die
       this->show_log_core("gxh_core", "gxh_core", "gxh core stoped", 0);   
       
}

//------------------------------------------------------------------------------

gxh_core::~gxh_core() 
{  
     this->show_log_core("gxh_core", "gxh_core", "stop", 0); 
     
     pthread_mutex_destroy(&this->gxh_response_muteks);     
     pthread_mutex_destroy(&this->gxh_handle_muteks);     
}

//------------------------------------------------------------------------------

std::string gxh_core::getVersion()
{
    return std::string("5.0.5");  // majro minor release
    
    // x.y.z
    
    // x brak kompatybilności wstecz przy zmianie wersji..
    
    // y kompatybilne wstecze, dodanie nowych funkcji..
    
    // z poprawione błędy.. i wproawdzone usprawnienia, lepsza efektywność
}

//------------------------------------------------------------------------------

std::string gxh_core::getKey()
{
    return "?";
}

//------------------------------------------------------------------------------

std::string gxh_core::getComp()
{
       std::string key = this->getRunPath();
       key.append("../gxh_comp"); 
       
       std::ifstream t( key.c_str() );
       t.seekg(0, std::ios::end);
       size_t size = t.tellg();
       std::string buffer(size, ' ');
       t.seekg(0);
       t.read(&buffer[0], size-1);           
       
       
       return buffer;
}

//------------------------------------------------------------------------------

std::string  gxh_core::getAuthor()
{
    return std::string("B. Kita, GXH System");
}

std::string  gxh_core::getLinux()
{
    return std::string("Linux");
}

//------------------------------------------------------------------------------

 void gxh_core::closeApplication()
 {
     this->show_log_core( "gxh_core", "closeApplication", "start", 2);
      
     
     this->thService = false; //close service thread...
     this->thSynchronize = false;
     sleep(1);
     
     this->show_log_core( "gxh_core", "closeApplication", "delete modules", 2);
     //delete currently use module
     for(int m=0;m<MAX_MODULE_BUFFER;m++) 
     {
        if( this->modules[m].ready == 1 )
        {
            this->modules[m].ready = 0;
            this->modules[m].moduleObject->forceCancel();
            usleep(1000 * 500); //owait 500 ms to cancel currently task
            delete this->modules[m].moduleObject; 
            dlclose( this->modules[m].handle );
            this->modules[m].module_name = "?";
        }
     }     
     this->modules_count = 0;
     
     
     this->show_log_core( "gxh_core", "closeApplication", "delete ports", 2);
     //close ports...
     for(unsigned int i=0; i<this->computer_ports_count; i++)
     {                                   
          if( (gxh_compare( (const char*)this->computer_ports[i].typ, "RS485") == 0  )  || (gxh_compare( (const char*)this->computer_ports[i].typ, "RS232") == 0  )   )
          {             
              this->ports[i]->stopLoop();
              sleep(2);
              delete this->ports[ i ];              
          }     
     }     
     this->computer_ports_count = 0;  
     
     this->show_log_core( "gxh_core", "closeApplication", "delete ethernet", 2);
     //close ethernet..    
     this->ethernet->stopLoop();      
     sleep(2);   
     delete this->ethernet;   
     
     
     this->show_log_core( "gxh_core", "closeApplication", "finish", 2);
    sleep(2);   
 }

//------------------------------------------------------------------------------

void gxh_core::loadModule()
{
     this->destroyResource(RES_VECTOR_SECTOR);
     this->destroyResource(RES_VECTOR_ZONE);
     this->destroyResource(RES_VECTOR_INTERRUPT);
     this->destroyResource(RES_VECTOR_FUNC);
     this->destroyResource(RES_VECTOR_SYMBOL_HASH);
     this->destroyResource(RES_VECTOR_HOME);
    
    //delete currently use module
     for(int m=0;m<MAX_MODULE_BUFFER;m++) 
     {
        if( this->modules[m].ready == 1 )
        {
            this->modules[m].ready = 0;
            this->modules[m].moduleObject->forceCancel();
            usleep(1000 * 500); //owait 500 ms to cancel currently task
            delete this->modules[m].moduleObject; 
            dlclose( this->modules[m].handle );
            this->modules[m].module_name = "?";
        }
     }
     
     this->modules_count = 0;
     
     //get list of module from data base
     sqlite3* dbSqLite;
     
     std::string baza = this->getDB();//this->getRunPath();
     //baza.append("../gxh_base.db");
     
     int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
     if( r != SQLITE_OK )
     {
          this->show_error("gxh_core","loadModule","Cannot open sqlLite 1",0);   
          
          this->show_error("gxh_core","loadModule", "FATAL!!!! Restarting...... 0xLoadModule000" ,2);  
          exit(0);
          
          return NULL;
     }
     
     sqlite3_stmt* stmt; 
     
     
     //versja..
     std::string version = this->getVersion(); //5.0 || 5.1 itd..
     const char* verTmp = version.c_str();
     char verBuf[5];
     int ixc =0;
     memset(verBuf,0,5);
     for(unsigned int cx=0; cx<strlen(verTmp); cx++) 
     {
         if(verTmp[cx] == '.') break;
         verBuf[ixc] = verTmp[cx];
         ixc++;
     }
     
     //version..
     
     std::string queryM = "SELECT module FROM ih_sl_const_function_symbol WHERE is_delete='0' AND used='1' AND app_min_version LIKE '";
     queryM.append(verBuf);
     queryM.append("%' ");
     queryM.append(" GROUP BY module;");
     
     const char* queryChar =  queryM.c_str(); //"SELECT module FROM ih_sl_const_function_symbol WHERE is_delete='0' AND used='1' GROUP BY module;";
     if(sqlite3_prepare(dbSqLite , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
     {
         this->show_error("gxh_core","loadModule", sqlite3_errstr( sqlite3_errcode(dbSqLite  ) ),4); 
         sqlite3_finalize(stmt);   
         sqlite3_close_v2(dbSqLite); 
         
         this->show_error("gxh_core","loadModule", "FATAL!!!! Restarting...... 0xLoadModule001" ,2);  
         exit(0);
         
         return;
     }     
     
     bool done = false;       
   
     while (!done)               
     { 
       int res = sqlite3_step (stmt);      
       if(res ==  SQLITE_ROW)
         {   
                  const char* module  = (const char*)sqlite3_column_text(stmt, 0);  
                  this->modules[this->modules_count].module_name = std::string(module);
                  this->modules[this->modules_count].ready = 2; //wait for create object..
                  this->modules_count++;
         }               
       if(res == SQLITE_DONE)done = true;                                    
     }     
     
     
     //load dynamic library and crate object of module
     
     for(unsigned int m=0;m<this->modules_count;m++) 
     {
        if( this->modules[m].ready == 2 )
        {
            std::string path = this->getRunPath();
            path.append( this->modules[m].module_name );
            
            this->modules[m].handle = dlopen ( path.c_str(), RTLD_LAZY);
            
            if (!this->modules[m].handle) 
            {
                this->show_error("gxh_core",  "loadModule", this->modules[m].module_name.c_str() , 0);
                std::cout<<"Error:"<<dlerror()<<"\n";
            }else
            {
                 //get address of method form dynamic library
                 this->modules[m].getModule = (cosine_func_ptr)dlsym(this->modules[m].handle, "get_module");  
                 
                 if(this->modules[m].getModule != NULL)
                 {
                        this->modules[m].moduleObject = (app_module*) ( this->modules[m].getModule(this) );                            
                        
                        if(this->modules[m].moduleObject != NULL) this->modules[m].ready = 1; //can use..
                        
                 }else
                 {
                    this->show_error("gxh_core",  "loadModule", "Cannot find get_module method in dynamic library" , 0);
                 }
            }
        }
     }
     
     //referece functions to module..
     
     for(unsigned int m=0;m<this->modules_count;m++) 
     {
        if( this->modules[m].ready == 1 )
        {
            this->modules[m].module_function_count = 0;
            //------------------------------
            std::string query ="SELECT description, const_function_symbol,type,run_as_service, run_ms_delay  FROM ih_sl_const_function_symbol WHERE is_delete='0' AND used='1' AND module='";
            query.append( this->modules[m].module_name);
            //tu xxx query.append("';");
            query.append("' GROUP BY const_function_symbol;");
            
            const char* queryChar = query.c_str();
            
            if(sqlite3_prepare(dbSqLite , queryChar , strlen(queryChar) , &stmt, NULL) == SQLITE_OK)
            {
                bool done2 = false;       
   
                while (!done2)               
                  { 
                    int res = sqlite3_step (stmt);      
                    if(res ==  SQLITE_ROW)
                    {   
                       
                        if( this->modules[m].module_function_count >= MAX_MODULE_FUNCTION) 
                        {
                            this->show_error("gxh_core","loadModules", "Too many function in one module!" ,1);       
                            break;
                        }
                        
                        
                       const char* description    =  (const char*)sqlite3_column_text(stmt, 0);  
                       int const_function_symbol  =   gxh_StringToInt(std::string( (const char*)sqlite3_column_text(stmt, 1)));  
                       char type                  =   gxh_StringToChar(std::string( (const char*)sqlite3_column_text(stmt, 2)));  
                       int run_as_service         =   gxh_StringToInt(std::string( (const char*)sqlite3_column_text(stmt, 3)));  
                       int run_ms_delay           =   gxh_StringToInt(std::string( (const char*)sqlite3_column_text(stmt, 4)));  
                    
                     
                       
        
                         
                       this->modules[m].functions[ this->modules[m].module_function_count ].description = std::string(description);
                       
                                 
                       
                       this->modules[m].functions[ this->modules[m].module_function_count ].const_function_symbol = (unsigned int)const_function_symbol;
                       this->modules[m].functions[ this->modules[m].module_function_count ].type = type;
                       this->modules[m].functions[ this->modules[m].module_function_count ].run_as_service = run_as_service;
                       this->modules[m].functions[ this->modules[m].module_function_count ].run_ms_delay = run_ms_delay;
                       this->modules[m].functions[ this->modules[m].module_function_count ].run_ms_last = 0;
                       
                       this->modules[m].module_function_count++;
                       
                             
                       
                       std::string info = "Reference function ";
                       info.append(description);
                       info.append(" to ");
                       info.append( this->modules[m].module_name  );
                       info.append( ", "  );
                       
                         
                       
                       info.append( this->modules[m].module_name);
                       info.append( ", ");
                       info.append( gxh_IntToString( run_as_service) );
                       info.append( ", ");
                       info.append( gxh_IntToString( run_ms_delay) );
                       
                       this->show_log_core("gxh_core", "loadModules",  info.c_str(), 0);
                       
                  
                       
                    }               
                    if(res == SQLITE_DONE)done2 = true;                                    
                   }  
            }                                
            //------------------------------   
        }
      }     
     
     sleep(1);
                   
    
     
    sqlite3_finalize(stmt); 
    
    
        
  
     ///-----------------------
     this->sectors_count = 0;
     
    std::string query="SELECT id, name, type, ih_home_id FROM ih_sector WHERE is_delete='0' ORDER BY orderby, name ASC;";        
    
    queryChar = query.c_str();          
    if(sqlite3_prepare(dbSqLite , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
    {
         this->show_error("gxh_core","loadModules", sqlite3_errstr( sqlite3_errcode(dbSqLite  ) ),4);          
         sqlite3_finalize(stmt);                
         
         this->show_error("gxh_core","loadModule", "FATAL!!!! Restarting...... 0xLoadModule002" ,2);  
         exit(0);         
         
    }else
    {    
       bool done = false;    
    
       while (!done)               
       { 
         int res = sqlite3_step (stmt);        
         if(res ==  SQLITE_ROW)
         {         
             this->sectors[this->sectors_count].id                 = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 0) ) );
             
             const char* tmp1                                      = (const char*)sqlite3_column_text(stmt, 1 ); 
             memset(this->sectors[this->sectors_count].name,0,100 );
             memcpy(this->sectors[this->sectors_count].name, tmp1, strlen(tmp1) );              
             
     
             
             const char* tmp2                                      = (const char*)sqlite3_column_text(stmt,2);  
             memset(this->sectors[this->sectors_count].type, 0, 10 );
             memcpy(this->sectors[this->sectors_count].type, tmp2, strlen(tmp2) );
             
             this->sectors[this->sectors_count].ih_home_id         = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 3) ) );                        
             this->sectors_count++;
         } 
              
          if(res == SQLITE_DONE)done = true;                                     
        }

       sqlite3_finalize(stmt);   
       
       this->registerResource((void*)this->sectors, this->sectors_count, RES_VECTOR_SECTOR);
    }
    
    
     
     
    //----------------
    this->zones_count  = 0;
    
    query="SELECT id, name, type, ih_sector_id FROM ih_zone WHERE is_delete='0'  ORDER BY orderby, name DESC; "; 
               
    this->show_log_data_base("gxh_core","loadModule", query.c_str() ,6);  
     
    queryChar = query.c_str();          
    if(sqlite3_prepare(dbSqLite , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
    {
         this->show_error("gxh_core","loadModule", sqlite3_errstr( sqlite3_errcode(dbSqLite  ) ),4);          
         sqlite3_finalize(stmt);    
         
         this->show_error("gxh_core","loadModule", "FATAL!!!! Restarting...... 0xLoadModule003" ,2);  
         exit(0);              
    }else
    {    
        bool done = false; 
    
        while (!done)               
        { 
          int res = sqlite3_step (stmt);
        
          if(res ==  SQLITE_ROW)
          {                                      
             this->zones[this->zones_count ].id                 = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 0) ) );
             
             const char* tmp1                                   =  (const char*)sqlite3_column_text(stmt, 1 );   
             memset(this->zones[this->zones_count ].name, 0, 100 );
             memcpy(this->zones[this->zones_count ].name, tmp1, strlen(tmp1));
             
             const char* tmp2                                   =  (const char*)sqlite3_column_text(stmt,2);
             memset(this->zones[this->zones_count ].type, 0, 10 );
             memcpy(this->zones[this->zones_count ].type, tmp2, strlen(tmp2) );
             
             
             this->zones[this->zones_count ].ih_sector_id       = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 3) ) );                        
             this->zones_count ++;
          } 
              
          if(res == SQLITE_DONE)done = true;                                     
         }

      sqlite3_finalize(stmt);
      this->registerResource((void*)this->zones, this->zones_count, RES_VECTOR_ZONE);
    }
     
    
    
    //home.. 
     this->homes_count = 0;
          
     query="SELECT id, name, address_1, address_2, zip_code, city, owner_name, latitude, longitude FROM ih_home WHERE is_delete='0' AND id='1' "; 
               
    this->show_log_data_base("gxh_core","loadModule", query.c_str() ,6);  
     
    queryChar = query.c_str();          
    if(sqlite3_prepare(dbSqLite , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
    {
         this->show_error("gxh_core","loadModule", sqlite3_errstr( sqlite3_errcode(dbSqLite  ) ),4);          
         sqlite3_finalize(stmt); 

         this->show_error("gxh_core","loadModule", "FATAL!!!! Restarting...... 0xLoadModule004" ,2);  
         exit(0);   
    }else
    {      
        bool done = false; 
    
        while (!done)               
        {  
          int res = sqlite3_step (stmt);
       
          if(res ==  SQLITE_ROW)
          {                      
              
             this->homes[this->homes_count].id  = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 0) ) );             
             
           
             
             const char* tmp1  =  (const char*)sqlite3_column_text(stmt, 1);  
             memset( this->homes[this->homes_count].name, 0, 255 );            
             if(tmp1 != NULL) memcpy( this->homes[this->homes_count].name, tmp1, strlen(tmp1) );      
             
            
             const char* tmp2  =  (const char*)sqlite3_column_text(stmt, 2);
             memset( this->homes[this->homes_count].address_1, 0, 255 );             
             if(tmp2 != NULL) memcpy( this->homes[this->homes_count].address_1, tmp2, strlen(tmp2) );   
             
             const char* tmp3  =  (const char*)sqlite3_column_text(stmt, 3);
             memset( this->homes[this->homes_count].address_2, 0, 255 );             
             if(tmp3 != NULL) memcpy( this->homes[this->homes_count].address_2, tmp3, strlen(tmp3) ); 
            
             const char* tmp4  =  (const char*)sqlite3_column_text(stmt, 4);
             memset( this->homes[this->homes_count].zip_code, 0, 10 );             
             if(tmp4 != NULL) memcpy( this->homes[this->homes_count].zip_code, tmp4, strlen(tmp4) ); 
            
             const char* tmp5  =  (const char*)sqlite3_column_text(stmt, 5);
             memset( this->homes[this->homes_count].city, 0, 255 );             
             if(tmp5 != NULL) memcpy( this->homes[this->homes_count].city, tmp5, strlen(tmp5) );
             
             const char* tmp6  =  (const char*)sqlite3_column_text(stmt, 6);
             memset( this->homes[this->homes_count].owner_name, 0, 255 );             
             if(tmp6 != NULL) memcpy( this->homes[this->homes_count].owner_name, tmp6, strlen(tmp6) );
               ;
             
             const char* tmp7  =  (const char*)sqlite3_column_text(stmt, 7);
             memset( this->homes[this->homes_count].latitude, 0, 20 );             
             if(tmp7 != NULL) memcpy( this->homes[this->homes_count].latitude, tmp7, strlen(tmp7) );
             
             const char* tmp8  =  (const char*)sqlite3_column_text(stmt, 8);
             memset( this->homes[this->homes_count].longitude, 0, 20 );             
             if(tmp8 != NULL) memcpy( this->homes[this->homes_count].longitude, tmp8, strlen(tmp8) );
             
             
             this->homes_count ++;
             if( this->homes_count >= 1) done = true;
          } 
              
          if(res == SQLITE_DONE)done = true;                                     
         }

      sqlite3_finalize(stmt);
      this->registerResource((void*)this->homes, this->homes_count, RES_VECTOR_HOME);
    }
    
   
    
     //----------------
    this->functions_count  = 0;
    
    query="SELECT ih_sl_const_function_symbol.id, description, const_function_symbol,type,run_as_service, run_ms_delay,name, param1_name,param2_name  FROM ih_sl_const_function_symbol, ih_sl_const_function_hash WHERE ih_sl_const_function_symbol.is_delete='0' AND used='1' AND symbol=ih_sl_const_function_symbol.const_function_symbol GROUP BY const_function_symbol ORDER BY const_function_symbol ASC; "; 
               
    this->show_log_data_base("gxh_core","loadModule", query.c_str() ,6);  
     
    queryChar = query.c_str();          
    if(sqlite3_prepare(dbSqLite , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
    {
         this->show_error("gxh_core","loadModule", sqlite3_errstr( sqlite3_errcode(dbSqLite  ) ),4);          
         sqlite3_finalize(stmt);     
         
         this->show_error("gxh_core","loadModule", "FATAL!!!! Restarting...... 0xLoadModule005" ,2);  
         exit(0);            
         
    }else
    {    
        bool done = false; 
    
        while (!done)               
        { 
          int res = sqlite3_step (stmt);
        
          if(res ==  SQLITE_ROW)
          {                                      
             this->functions[this->functions_count ].id      = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 0) ) );
             
             const char* tmp1                                = (const char*)sqlite3_column_text(stmt,1) ;
             memset(this->functions[this->functions_count ].description, 0, 255);
             if(strlen(tmp1) < 255) memcpy(this->functions[this->functions_count ].description, tmp1, strlen(tmp1) );
          
             this->functions[this->functions_count ].const_function_symbol      = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 2) ) );
             
             
             const char* tmp2                                = (const char*)sqlite3_column_text(stmt,3) ;
             memset(this->functions[this->functions_count ].type, 0, 255);
             if(strlen(tmp2) < 255) memcpy(this->functions[this->functions_count ].type, tmp2, strlen(tmp2) );
             
             this->functions[this->functions_count ].run_as_service      = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 4) ) );
             this->functions[this->functions_count ].run_ms_delay        = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 5) ) );
             
             
             const char* tmp3                                = (const char*)sqlite3_column_text(stmt,6) ;            
             memset(this->functions[this->functions_count ].resourceName, 0, 255);
             if(strlen(tmp3) < 255) memcpy(this->functions[this->functions_count ].resourceName, tmp3, strlen(tmp3) );                                      
             
             
             const char* tmp4                                = (const char*)sqlite3_column_text(stmt,7) ;            
             memset(this->functions[this->functions_count ].param1Name, 0, 255);
             if(tmp4 != NULL && strlen(tmp4) < 255) memcpy(this->functions[this->functions_count ].param1Name, tmp4, strlen(tmp4) );
             
             
             const char* tmp5                                = (const char*)sqlite3_column_text(stmt,8) ;            
             memset(this->functions[this->functions_count ].param2Name, 0, 255);
             if(tmp5 != NULL && strlen(tmp5) < 255) memcpy(this->functions[this->functions_count ].param2Name, tmp5, strlen(tmp5) );
             
             this->functions_count ++;
          } 
              
          if(res == SQLITE_DONE)done = true;                                     
         }

      sqlite3_finalize(stmt);
      this->registerResource((void*)this->functions, this->functions_count, RES_VECTOR_FUNC);
    }
    
    
     
    //----------------
    this->symbols_count  = 0;
    
    query="SELECT name, symbol FROM ih_sl_const_function_hash WHERE is_delete='0';"; 
               
    this->show_log_data_base("gxh_core","loadModule", query.c_str() ,6);  
     
    queryChar = query.c_str();          
    if(sqlite3_prepare(dbSqLite , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
    {
         this->show_error("gxh_core","loadModule", sqlite3_errstr( sqlite3_errcode(dbSqLite  ) ),4);          
         sqlite3_finalize(stmt);        
         
         this->show_error("gxh_core","loadModule", "FATAL!!!! Restarting...... 0xLoadModule006" ,2);  
         exit(0);          
    }else
    {    
        bool done = false; 
    
        while (!done)               
        { 
          int res = sqlite3_step (stmt);
        
          if(res ==  SQLITE_ROW)
          {       
             
             if(this->symbols_count > MAX_SYMBOL_HASH_BUFFER-1) 
             {
                 this->show_error("gxh_core","loadModule","Zbyt maly buffor 0x9233434Z34",1);
                 continue;
             } 
             
              const char* tmp1 =  (const char*)sqlite3_column_text(stmt, 0);
              memset( this->symbols[this->symbols_count ].name, 0, 255 );
              memcpy( this->symbols[this->symbols_count ].name, tmp1, strlen(tmp1)  );
             
              this->symbols[this->symbols_count ].symbol      = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 1) ) );                      
              this->symbols_count ++;
              
             
          } 
              
          if(res == SQLITE_DONE)done = true;                                     
         }

      sqlite3_finalize(stmt);
      this->registerResource((void*)this->symbols, this->symbols_count, RES_VECTOR_SYMBOL_HASH);
    }
     
     

     sqlite3_close_v2(dbSqLite);    
          
     ////////////
     sleep(1);
     this->loadResourceFunction();
     sleep(1);
     this->loadFunction();    
}

//------------------------------------------------------------------------------

void gxh_core::loadFunction()
{
     sqlite3* dbSqLite;
     
     this->port_function_count = 0;
     
     std::string baza = this->getDB();//this->getRunPath();
     //baza.append("../gxh_base.db");
     
     int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
     if( r != SQLITE_OK )
     {
          this->show_error("gxh_core","loadModule","Cannot open sqlLite 1",0);   
          this->show_error("gxh_core","loadFunction", "FATAL!!!! Restarting...... 0x11134a" ,2); 
          exit(0);
          return NULL;
     }
     
     sqlite3_stmt* stmt; 
     
     std::string query  = "SELECT id, ih_driver_port_id,  const_function_symbol, resource_id,port_type, switch_delay_ms, time_to_off_ms, delay_from_start_ms, repeat, name, text_content FROM ih_driver_port_function WHERE is_delete='0' ORDER BY ih_driver_port_id ASC; "; 
     const char* queryChar = query.c_str();
     
     if(sqlite3_prepare(dbSqLite , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
    {
         this->show_error("gxh_core","loadFunction", sqlite3_errstr( sqlite3_errcode(dbSqLite  ) ),4);          
         this->show_error("gxh_core","loadFunction", "FATAL!!!! Restarting...... 0x9232222234a" ,2);  
         sqlite3_finalize(stmt);    
         sqlite3_close_v2(dbSqLite);
         exit(0);
         return;
    }
     
     
    bool done = false; 
   
   
    while (!done)               
    { 
       int res = sqlite3_step (stmt);
        
         if(res ==  SQLITE_ROW)
         { 
           bool letBreak = false;
           for(int u=0; u<11;u++) { if( ((const char*)sqlite3_column_text(stmt,u))  == NULL )  {letBreak = true; break;}; }
           if(letBreak)  
           {
               this->show_error("gxh_core","loadFunction", "empty result!!!!!",5);                
               continue;
           } 
             
           
           if(this->port_function_count > MAX_PORT_FUNCTION_BUFFER)
           {
               this->show_error("gxh_core","loadFunction", "Buffer Error 0xZZZ3333323342AC",5);  
               break;
           }
           
             
           memset (this->port_function[this->port_function_count].name,'\0',250);
           memset (this->port_function[this->port_function_count].text_content,'\0',500);
        
        
           if( ((const char*)sqlite3_column_text(stmt,0))  != NULL ) this->port_function[this->port_function_count].id                    = gxh_StringToInt( (const char*)sqlite3_column_text(stmt, 0) );                              
           if( ((const char*)sqlite3_column_text(stmt,1))  != NULL )this->port_function[this->port_function_count].ih_driver_port_id      = gxh_StringToInt( (const char*)sqlite3_column_text(stmt, 1)  );                              
           if( ((const char*)sqlite3_column_text(stmt,2))  != NULL )this->port_function[this->port_function_count].const_function_symbol  = gxh_StringToInt( (const char*)sqlite3_column_text(stmt, 2)  );                              
           if( ((const char*)sqlite3_column_text(stmt,3))  != NULL )this->port_function[this->port_function_count].resource_id            = gxh_StringToInt( (const char*)sqlite3_column_text(stmt, 3)  );                              
           if( ((const char*)sqlite3_column_text(stmt,4))  != NULL )this->port_function[this->port_function_count].port_typ               = gxh_StringToChar( (const char*)sqlite3_column_text(stmt, 4)  );                              
           if( ((const char*)sqlite3_column_text(stmt,5))  != NULL )this->port_function[this->port_function_count].switch_delay_ms        = gxh_StringToInt((const char*)sqlite3_column_text(stmt, 5)  );                              
           if( ((const char*)sqlite3_column_text(stmt,6))  != NULL )this->port_function[this->port_function_count].time_to_off_ms         = gxh_StringToInt( (const char*)sqlite3_column_text(stmt, 6)  );                              
           if( ((const char*)sqlite3_column_text(stmt,7))  != NULL )this->port_function[this->port_function_count].delay_from_start_ms    = gxh_StringToInt( (const char*)sqlite3_column_text(stmt, 7)  );                              
           if( ((const char*)sqlite3_column_text(stmt,8))  != NULL )this->port_function[this->port_function_count].repeat                 = gxh_StringToInt( (const char*)sqlite3_column_text(stmt, 8)  );                              
        
            if( ((const char*)sqlite3_column_text(stmt,9))  != NULL )
            {
             const char* tmp1               = (const char*)sqlite3_column_text(stmt, 9) ;
             if(tmp1 != NULL) memcpy(this->port_function[this->port_function_count].name, tmp1, strlen(tmp1));
            }
        
            if( ((const char*)sqlite3_column_text(stmt,10))  != NULL )
            {
             const char* tmp2               =  (const char*)sqlite3_column_text(stmt, 10);
             if(tmp2 != NULL)  memcpy(this->port_function[this->port_function_count].text_content, tmp2, strlen(tmp2) );
            }
                 
 
             this->get_resource_name( dbSqLite,this->port_function[this->port_function_count].fName, 50, this->port_function[this->port_function_count].const_function_symbol );          
             this->get_resource_name_func( dbSqLite, this->port_function[this->port_function_count].resource_id ,this->port_function[this->port_function_count].fName, this->port_function[this->port_function_count].rName , 100);
                      
           
             
             std::string info = "Function port | ";
             info.append(this->port_function[this->port_function_count].fName );
             info.append(" | ");
             info.append( this->port_function[this->port_function_count].rName );                       
             this->show_log_core("gxh_core", "loadFunction",  info.c_str(), 20);
             
             this->port_function_count++;
         } 
              
         if(res == SQLITE_DONE)done = true;                            
        
    }  
     
     
    sqlite3_finalize(stmt);   
    sqlite3_close_v2(dbSqLite);
}
 
//------------------------------------------------------------------------------

void gxh_core::loadResourceFunction()
{
     sqlite3* dbSqLite;
     
     this->resource_function_count = 0;
     
     std::string baza = this->getDB();//this->getRunPath();
     //baza.append("../gxh_base.db");
     
     int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
     if( r != SQLITE_OK )
     {
          this->show_error("gxh_core","loadResourceFunction","Cannot open sqlLite 1",0);   
          this->show_error("gxh_core","loadFunction", "FATAL!!!! Restarting...... 0x1222224a" ,2); 
          exit(0);
          return NULL;
     }
     
     sqlite3_stmt* stmt; 
     
     std::string query  = "SELECT id, in_resource_id, in_const_function_symbol, resource_id, const_function_symbol,  name, text_content, time_active_from, time_active_to, mon, tue,wed,thu,fri,sat,sun FROM ih_resource_function  WHERE is_delete='0';"; 
     const char* queryChar = query.c_str();
     
     if(sqlite3_prepare(dbSqLite , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
    {
         this->show_error("gxh_core","loadResourceFunction", sqlite3_errstr( sqlite3_errcode(dbSqLite  ) ),4);          
         this->show_error("gxh_core","loadResourceFunction", "FATAL!!!! Restarting...... 0x923rf3434a" ,2);          
         
         sqlite3_finalize(stmt);    
         sqlite3_close_v2(dbSqLite);
         exit(0);
         return;
    }
     
     
    bool done = false; 
   
   
    while (!done)               
    { 
       int res = sqlite3_step (stmt);
        
         if(res ==  SQLITE_ROW)
         { 
           bool letBreak = false;
           for(int u=0; u<6;u++) { if( ((const char*)sqlite3_column_text(stmt,u))  == NULL )  {letBreak = true; break;}; }
           if(letBreak)  
           {
               this->show_error("gxh_core","loadResourceFunction", "empty result!!!!!",5);                
               continue;
           } 
           
           if(this->resource_function_count > MAX_RESOURCE_FUNCTION_BUFFER)
           {
               this->show_error("gxh_core","loadResourceFunction", "Buffer Error 0x923342AC",5);  
               break;
           }
             
             
           memset (this->resource_function[this->resource_function_count].name,'\0',250);
           memset (this->resource_function[this->resource_function_count].text_content,'\0',500);
           
           memset (this->resource_function[this->resource_function_count].time_active_from,'\0',10);
           memset (this->resource_function[this->resource_function_count].time_active_to,'\0',10);
        
        
           if( ((const char*)sqlite3_column_text(stmt,0))  != NULL ) this->resource_function[this->resource_function_count].id                           = gxh_StringToInt( (const char*)sqlite3_column_text(stmt, 0) );                                                    
           if( ((const char*)sqlite3_column_text(stmt,1))  != NULL )this->resource_function[this->resource_function_count].in_resource_id                = gxh_StringToInt( (const char*)sqlite3_column_text(stmt, 1)  );                              
           if( ((const char*)sqlite3_column_text(stmt,2))  != NULL )this->resource_function[this->resource_function_count].in_const_function_symbol      = gxh_StringToInt( (const char*)sqlite3_column_text(stmt, 2)  );                              
           if( ((const char*)sqlite3_column_text(stmt,3))  != NULL )this->resource_function[this->resource_function_count].resource_id                   = gxh_StringToInt( (const char*)sqlite3_column_text(stmt, 3)  );                              
           if( ((const char*)sqlite3_column_text(stmt,4))  != NULL )this->resource_function[this->resource_function_count].const_function_symbol         = gxh_StringToInt( (const char*)sqlite3_column_text(stmt, 4)  );                              

            if( ((const char*)sqlite3_column_text(stmt,5))  != NULL )
            {
              const char* tmp1               = (const char*)sqlite3_column_text(stmt, 5) ;
              if(tmp1 != NULL) memcpy(this->resource_function[this->resource_function_count].name, tmp1, strlen(tmp1));
            }
        
            if( ((const char*)sqlite3_column_text(stmt,6))  != NULL )
            {
              const char* tmp2               =  (const char*)sqlite3_column_text(stmt, 6);
              if(tmp2 != NULL)  memcpy(this->resource_function[this->resource_function_count].text_content, tmp2, strlen(tmp2) );
            }                             
           
            if( ((const char*)sqlite3_column_text(stmt,7))  != NULL )
            {
               const char* tmp3               =  (const char*)sqlite3_column_text(stmt, 7);
               if(tmp3 != NULL)  memcpy(this->resource_function[this->resource_function_count].time_active_from, tmp3, strlen(tmp3) );
            }
           
           if( ((const char*)sqlite3_column_text(stmt,8))  != NULL )
            {
               const char* tmp4               =  (const char*)sqlite3_column_text(stmt, 8);
               if(tmp4 != NULL)  memcpy(this->resource_function[this->resource_function_count].time_active_to, tmp4, strlen(tmp4) );
            }
                    
           
           if( ((const char*)sqlite3_column_text(stmt,9))  != NULL )
               this->resource_function[this->resource_function_count].mon = gxh_StringToInt( (const char*)sqlite3_column_text(stmt, 9)  );                              
           
           if( ((const char*)sqlite3_column_text(stmt,10))  != NULL )
               this->resource_function[this->resource_function_count].tue = gxh_StringToInt( (const char*)sqlite3_column_text(stmt, 10)  );             

           if( ((const char*)sqlite3_column_text(stmt,11))  != NULL )
               this->resource_function[this->resource_function_count].wed = gxh_StringToInt( (const char*)sqlite3_column_text(stmt, 11)  );             
           
           if( ((const char*)sqlite3_column_text(stmt,12))  != NULL )
               this->resource_function[this->resource_function_count].thu = gxh_StringToInt( (const char*)sqlite3_column_text(stmt, 12)  );             
           
           if( ((const char*)sqlite3_column_text(stmt,13))  != NULL )
               this->resource_function[this->resource_function_count].fri = gxh_StringToInt( (const char*)sqlite3_column_text(stmt, 13)  );             
           
           if( ((const char*)sqlite3_column_text(stmt,14))  != NULL )
               this->resource_function[this->resource_function_count].sat = gxh_StringToInt( (const char*)sqlite3_column_text(stmt, 14)  );              
           
           if( ((const char*)sqlite3_column_text(stmt,15))  != NULL )
               this->resource_function[this->resource_function_count].sun = gxh_StringToInt( (const char*)sqlite3_column_text(stmt, 15)  );             

           
            this->show_log_core("gxh_core", "loadResourceFunction", "OK ", 20);
             
            this->resource_function_count++;
         } 
              
         if(res == SQLITE_DONE)done = true;                            
        
    }  
     
     
    sqlite3_finalize(stmt);   
    sqlite3_close_v2(dbSqLite);
    
    this->registerResource((void*)this->resource_function, this->resource_function_count, RES_VECTOR_INTERRUPT);
}
 

//------------------------------------------------------------------------------

void gxh_core::loadPorts()
{
    
     for(unsigned int i=0; i<this->computer_ports_count; i++)
      {                    
          if( (gxh_compare( (const char*)this->computer_ports[i].typ, "RS485") == 0  )  || (gxh_compare( (const char*)this->computer_ports[i].typ, "RS232") == 0  )   )
          {             
              this->ports[i]->stopLoop();
              sleep(2);
              delete this->ports[ i ];
          }                           
      }
    
     
    sqlite3* dbSqLite;    
    sqlite3_stmt* stmt; 
    
    std::string baza = this->getDB(); //this->getRunPath();
    //baza.append("../gxh_base.db");
     
     int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
     if( r != SQLITE_OK )
     {
          this->show_error("gxh_core","loadPorts","Cannot open sqlLite 1",0);   
          
          this->show_error("gxh_core","loadPorts", "FATAL!!!! Restarting...... 0xLoadPort000" ,2);  
          exit(0);
          
          return NULL;
     }
    
 
    
    //pobierz porty komputera..
    this->show_log_core( "gxh_core", "loadPorts", "Get computer ports from data base", 6);
    this->computer_ports_count = 0;
     
    std::string query= "SELECT id, name_on_computer, path, typ, ih_computer_id, parity, stop_bit, speed, data FROM ih_computer_port WHERE is_delete='0' AND is_active='1' AND ih_computer_id='1'; "; 
  
    
    const char* queryChar = query.c_str();            
    if(sqlite3_prepare(dbSqLite , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
    {
         this->show_error("gxh_core","loadPorts", sqlite3_errstr( sqlite3_errcode(dbSqLite  ) ),4);          
         sqlite3_finalize(stmt);               
         
          this->show_error("gxh_core","loadPorts", "FATAL!!!! Restarting...... 0xLoadPort001" ,2);  
          exit(0);
         
    }else
    {    
        bool done = false;      
        while (!done)               
        { 
          int res = sqlite3_step (stmt);             
          if(res ==  SQLITE_ROW)
           {             
            this->computer_ports[this->computer_ports_count].id               = gxh_StringToInt( (const char*)sqlite3_column_text(stmt, 0)  );
            
             const char* tmp1                                                 = (const char*)sqlite3_column_text(stmt, 1);
             memset(this->computer_ports[this->computer_ports_count].name_on_computer,0,  50);
             memcpy( this->computer_ports[this->computer_ports_count].name_on_computer, tmp1, strlen(tmp1) );
             
            
            
            const char* tmp2                                                  =   (const char*)sqlite3_column_text(stmt, 2 );
            memset(this->computer_ports[this->computer_ports_count].path,0,255);
            memcpy(this->computer_ports[this->computer_ports_count].path, tmp2, strlen(tmp2) );
            
            const char* tmp3                                                  = (const char*)sqlite3_column_text(stmt, 3);
            memset(this->computer_ports[this->computer_ports_count].typ,0,2);
            memcpy(this->computer_ports[this->computer_ports_count].typ, tmp3, strlen(tmp3));
            
            this->computer_ports[this->computer_ports_count].ih_computer_id   = gxh_StringToInt( (const char*)sqlite3_column_text(stmt, 4) );
            
            const char* tmp4                                                  = (const char*)sqlite3_column_text(stmt, 5);
            memset(this->computer_ports[this->computer_ports_count].parity,0,2);
            memcpy(this->computer_ports[this->computer_ports_count].parity, tmp4, strlen(tmp4) );
            
            const char* tmp5                                                  = (const char*)sqlite3_column_text(stmt, 6);
            memset(this->computer_ports[this->computer_ports_count].stop_bit,0,2);
            memcpy(this->computer_ports[this->computer_ports_count].stop_bit, tmp5, strlen(tmp5));
            
            this->computer_ports[this->computer_ports_count].speed            = gxh_StringToInt( (const char*)sqlite3_column_text(stmt, 7) );
            
            const char* tmp6                                                  = (const char*)sqlite3_column_text(stmt, 8);
            memset(this->computer_ports[this->computer_ports_count].data,0,2);
            memcpy(this->computer_ports[this->computer_ports_count].data, tmp6, strlen(tmp6)  );
            
            this->show_log_core( "gxh_core", "loadPorts",  (const char*)this->computer_ports[this->computer_ports_count].name_on_computer , 6);
            
            this->computer_ports_count++;
            
            if( this->computer_ports_count >= MAX_COMPUTER_PORT-1)
            {
                this->show_error("gxh_core","loadPorts","No space for computer port",0); 
                done = true;               
            }
            
           }               
          if(res == SQLITE_DONE)done = true;                                    
        }        
        sqlite3_finalize(stmt);         
    }
      
    //pobierz urzadzenia...
    this->show_log_core( "gxh_core", "loadPorts", "Get drivers", 6);
    this->drivers_count = 0;    
    this->destroyResource(RES_VECTOR_DRIVER);
    
    
    
    query="SELECT ih_driver.id,ih_driver.address, ih_driver.name, ih_sl_driver.const_function_driver, ih_sl_driver.module, ih_driver.delay_before_read_ms, ih_driver.parity, ih_driver.stop_bit, ih_driver.speed, ih_driver.data, ih_driver.ih_zone_id, ih_sl_driver.driver_symbol, ih_zone.ih_sector_id, ih_sl_driver.screen, ih_sl_driver.name_firm, ih_sl_driver.www_firm, ih_sl_driver.author, ih_driver.ih_sl_driver_id   FROM ih_driver, ih_sl_driver, ih_zone WHERE ih_driver.ih_zone_id=ih_zone.id AND ih_driver.is_delete='0'  AND ih_driver.is_active='1'  AND ih_driver.ih_sl_driver_id=ih_sl_driver.id AND ih_sl_driver.is_delete='0'  ORDER BY address ASC;"; 
    
    queryChar = query.c_str();                
    if(sqlite3_prepare(dbSqLite , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
    {
         this->show_error("gxh_core","loadPorts", sqlite3_errstr( sqlite3_errcode(dbSqLite  ) ),4);          
         sqlite3_finalize(stmt);       
         
         this->show_error("gxh_core","loadPorts", "FATAL!!!! Restarting...... 0xLoadPort002" ,2);  
         exit(0);
    }else
    {   
        bool done = false;        
        while (!done)               
        { 
          int res = sqlite3_step (stmt);             
          if(res ==  SQLITE_ROW)
          {                                                                               
             this->drivers[this->drivers_count].id                     = gxh_StringToInt( std::string((const char*)sqlite3_column_text(stmt, 0)   ) );      
             
             const char* tmp1                                          = (const char*)sqlite3_column_text(stmt, 1);
             memset(this->drivers[this->drivers_count].address, 0, 10);
             memcpy(this->drivers[this->drivers_count].address, tmp1, strlen(tmp1));
             
             const char* tmp2                                          = (const char*)sqlite3_column_text(stmt, 2);
             memset(this->drivers[this->drivers_count].name, 0, 50 );
             memcpy(this->drivers[this->drivers_count].name, tmp2, strlen(tmp2) );
             
             
             this->drivers[this->drivers_count].const_function_driver  = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 3)   ) );      
             
             const char* tmp3                                          = (const char*)sqlite3_column_text(stmt, 4);
             memset(this->drivers[this->drivers_count].module, 0, 50 );
             memcpy(this->drivers[this->drivers_count].module, tmp3, strlen(tmp3) );
             
             this->drivers[this->drivers_count].delay_before_read_ms   = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 5)   ) );      
             
             const char* tmp4                                          = (const char*)sqlite3_column_text(stmt, 6);
             memset(this->drivers[this->drivers_count].parity, 0, 2 );
             memcpy(this->drivers[this->drivers_count].parity, tmp4, strlen(tmp4) );
             
             const char* tmp5                                          = (const char*)sqlite3_column_text(stmt, 7);
             memset(this->drivers[this->drivers_count].stop_bit, 0, 2 );
             memcpy(this->drivers[this->drivers_count].stop_bit, tmp5, strlen(tmp5) );
             
             this->drivers[this->drivers_count].speed                  = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 8)  ) );  
             
             const char* tmp6                                          = (const char*)sqlite3_column_text(stmt, 9);
             memset(this->drivers[this->drivers_count].data, 0, 2 );
             memcpy(this->drivers[this->drivers_count].data, tmp6, strlen(tmp6) );
             
             this->drivers[this->drivers_count].ih_zone_id             = gxh_StringToInt( std::string((const char*)sqlite3_column_text(stmt, 10)   ) );  
             this->drivers[this->drivers_count].driver_symbol          = gxh_StringToChar( std::string( (const char*)sqlite3_column_text(stmt, 11) ) ); 
             this->drivers[this->drivers_count].ih_sector_id           = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 12)  ) ); 
             
             const char* tmp7                                          = (const char*)sqlite3_column_text(stmt, 13);
             memset(this->drivers[this->drivers_count].screen, 0, 100 );
             memcpy(this->drivers[this->drivers_count].screen, tmp7, strlen(tmp7) );
             
             const char* tmp8                                          = (const char*)sqlite3_column_text(stmt, 14);
             memset(this->drivers[this->drivers_count].name_firm, 0, 100 );
             memcpy(this->drivers[this->drivers_count].name_firm, tmp8, strlen(tmp8) );
             
             const char* tmp9                                          = (const char*)sqlite3_column_text(stmt, 15);
             memset(this->drivers[this->drivers_count].www_firm, 0, 255 );
             memcpy(this->drivers[this->drivers_count].www_firm, tmp9, strlen(tmp9));
             
             const char*  tmp10                                        = (const char*)sqlite3_column_text(stmt, 16);
             memset(this->drivers[this->drivers_count].author, 0,255 );
             memcpy(this->drivers[this->drivers_count].author, tmp10, strlen(tmp10) );
             
             this->drivers[this->drivers_count].ih_sl_driver_id        = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 17)  ) ); 
             
             
             this->drivers[this->drivers_count].toSave = false;
             memset(this->drivers[this->drivers_count].value,0, 100);
             
             
             std::string msg = "Find ";
             msg.append( this->drivers[this->drivers_count].name );
             msg.append(" address: ");
             msg.append( this->drivers[this->drivers_count].address  );
             
             this->show_log_core( "gxh_core", "loadPorts",   msg.c_str(), 6);
             this->drivers_count++;
             
             if( this->drivers_count >= MAX_DRIVERS-1)
            {
                this->show_error("gxh_core","loadPorts","No space for drivers",0); 
                done = true;
            }
           } 
           
           if(res == SQLITE_DONE)done = true;                                    
          }
        
          sqlite3_finalize(stmt);       
          
          this->registerResource(this->drivers, this->drivers_count, RES_VECTOR_DRIVER);
      }
 
     //---------- pobierz słownik sterowniów
    
     //---------- pobierz porty poszczegolych urzadzen..
    this->driver_ports_count = 0;
    this->show_log_core( "gxh_core", "loadPorts", "Get ports of driver", 6);
    this->destroyResource(RES_VECTOR_DRIVER_PORT);
    
  
    query="";
    query.append("SELECT id, num, type, ih_driver_id, status_input, status_output,status_time,reverts FROM ih_driver_port WHERE is_delete='0'  ORDER BY num ASC;");
 
          
         
         queryChar = query.c_str();        
         if(sqlite3_prepare(dbSqLite , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
         {
           this->show_error("gxh_core","loadPorts", sqlite3_errstr( sqlite3_errcode(dbSqLite  ) ),4);          
           sqlite3_finalize(stmt);                       
           
                    
           this->show_error("gxh_core","loadPorts", "FATAL!!!! Restarting...... 0xLoadPort003" ,2);  
           exit(0);
           
         }else
         {
             bool done = false;        
             while (!done)               
             { 
               int res = sqlite3_step (stmt);               
               if(res ==  SQLITE_ROW)
               {                         
                   this->driver_ports[this->driver_ports_count].id                     = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt,0)  ) );        
                   this->driver_ports[this->driver_ports_count].num                    = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt,1)  ) );            
                   this->driver_ports[this->driver_ports_count].type                   = gxh_StringToChar( std::string( (const char*)sqlite3_column_text(stmt,2)   ) );         
                   this->driver_ports[this->driver_ports_count].ih_driver_id           = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt,3)   ) );         
                   this->driver_ports[this->driver_ports_count].status_input           = gxh_StringToChar( std::string( (const char*)sqlite3_column_text(stmt,4)   ) );          
                   this->driver_ports[this->driver_ports_count].status_output          = gxh_StringToChar( std::string( (const char*)sqlite3_column_text(stmt,5)   ) );            
                   this->driver_ports[this->driver_ports_count].status_time            = gxh_StringToDouble( std::string( (const char*)sqlite3_column_text(stmt,6)  ) );               
                   this->driver_ports[this->driver_ports_count].reverts                = gxh_StringToChar( std::string( (const char*)sqlite3_column_text(stmt,7)   ) ); 
                   this->driver_ports_count++;
                   
                   
                  if( this->driver_ports_count >=   ((MAX_DRIVERS * MAX_PORT_IN_DRIVER)-1)  )
                  {
                   this->show_error("gxh_core","loadPorts","No space for driver ports",0); 
                   done = true;
                  }
                   
               }               
               if(res == SQLITE_DONE)done = true;                                    
             }
             sqlite3_finalize(stmt); 
             
             this->registerResource(this->driver_ports, this->driver_ports_count, RES_VECTOR_DRIVER_PORT);
         }
 
         
    
    //--pobierz rozne sterowniki urzadzen..
     this->sldriver_count = 0;
     this->destroyResource(RES_VECTOR_DRIVER_SL);
     this->show_log_core( "gxh_core", "loadPorts", "Get software of driver", 6);
     
     
     
      query=" SELECT ih_sl_driver.id, ih_sl_driver.name, ih_sl_driver.name_firm, ih_sl_driver.www_firm, ih_sl_driver.module, ih_sl_driver.author, ih_sl_driver.is_modul_io, ih_sl_driver.const_function_driver FROM ih_sl_driver, ih_driver WHERE  ih_driver.ih_sl_driver_id =  ih_sl_driver.id AND ih_sl_driver.is_delete='0' AND ih_driver.is_delete='0' ; "; 
  
      queryChar = query.c_str();        
      
   
      
         if(sqlite3_prepare(dbSqLite , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
         {
           this->show_error("gxh_core","loadPorts", sqlite3_errstr( sqlite3_errcode(dbSqLite  ) ),4);          
           sqlite3_finalize(stmt);  
           
           this->show_error("gxh_core","loadPorts", "FATAL!!!! Restarting...... 0xLoadPort004" ,2);  
           exit(0);
           
         }else
         {
           bool done = false;       
           while (!done)               
           {  
             int res = sqlite3_step (stmt);        
             if(res ==  SQLITE_ROW)
             {
               
               this->sldriver[this->sldriver_count].id                     = gxh_StringToInt(  (const char*)sqlite3_column_text(stmt,0)   );                      
               
               const char* tmp1                                            = (const char*)sqlite3_column_text(stmt,1);               
               memset(this->sldriver[this->sldriver_count].name, 0, 50);
               memcpy(this->sldriver[this->sldriver_count].name, tmp1, strlen(tmp1));
               
               const char* tmp2                                            = (const char*)sqlite3_column_text(stmt,2);
               memset(this->sldriver[this->sldriver_count].name_firm, 0, 100);
               memcpy(this->sldriver[this->sldriver_count].name_firm, tmp2, strlen(tmp2) );
               
               const char* tmp3                                            = (const char*)sqlite3_column_text(stmt,3);
               memset(this->sldriver[this->sldriver_count].www_firm, 0, 100);
               memcpy(this->sldriver[this->sldriver_count].www_firm, tmp3, strlen(tmp3) );
               
               
               const char* tmp4                                            = (const char*)sqlite3_column_text(stmt,4);
               memset(this->sldriver[this->sldriver_count].module, 0, 50    );
               memcpy(this->sldriver[this->sldriver_count].module, tmp4, strlen(tmp4)   );
               
               const char* tmp5                                            =(const char*)sqlite3_column_text(stmt,5);
               memset(this->sldriver[this->sldriver_count].author,0, 100 );
               memcpy(this->sldriver[this->sldriver_count].author,tmp5, strlen(tmp5) );
               
               this->sldriver[this->sldriver_count].is_modul_io            = gxh_StringToChar (std::string(  (const char*)sqlite3_column_text(stmt,6) ) );  
               this->sldriver[this->sldriver_count].const_function_driver  = gxh_StringToInt (std::string(  (const char*)sqlite3_column_text(stmt,7) ) );  
               this->sldriver_count++;
               
               if( this->sldriver_count >=   MAX_SLDRIVER-1  )
                  {
                   this->show_error("gxh_core","loadPorts","No space for software of driver",0); 
                   done = true;
                  }
               
              
             }               
             if(res == SQLITE_DONE)done = true;                                     
            }
           
       
           
            sqlite3_finalize(stmt);              
            this->registerResource(this->sldriver, this->sldriver_count, RES_VECTOR_DRIVER_SL);
         }
 
  
 
      //---- pobierz api dla urzadzen....
      this->driver_api_count = 0;
      this->destroyResource(RES_VECTOR_DRIVER_SL);
      
      /*
      this->show_log_core( "gxh_core", "loadPorts", "Get API'S of driver", 6);
      
      query= "select id, ih_driver_id, control_type, ih_driver_values_id, label_name, button_name, value_type, func_number, func_internal_number, select_val, replace_val, pr_standard, pr_standard_plus, pr_administrator, pr_system from ih_sl_driver_api WHERE is_delete='0' ; ";  
      this->show_log_data_base("gxh_core","loadPorts", query.c_str() ,6);  
              
      queryChar = query.c_str();        
      if(sqlite3_prepare(dbSqLite , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
      {
         this->show_error("gxh_core","loadPorts", sqlite3_errstr( sqlite3_errcode(dbSqLite  ) ),4);          
         sqlite3_finalize(stmt);                  
      }else
      {
         bool done = false;        
         while (!done)               
         { 
              int res = sqlite3_step (stmt);        
              if(res ==  SQLITE_ROW)
              {       
                 this->driver_api[this->driver_api_count].id                      = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt,0)  ) );                              
                 this->driver_api[this->driver_api_count].ih_driver_id            = gxh_StringToInt(  std::string( (const char*)sqlite3_column_text(stmt,1)  ) );             
                 
                 const char* tmp1                                                 = (const char*)sqlite3_column_text(stmt,2);
                 
                // std::cout<<"\n\nType "<<tmp1<<" id: "<<this->driver_api[this->driver_api_count].id<<" driverId<< "<<this->driver_api[this->driver_api_count].ih_driver_id;
                 
                 memset(this->driver_api[this->driver_api_count].control_type, 0, 30 );
                 memcpy(this->driver_api[this->driver_api_count].control_type, tmp1, strlen(tmp1));
                 
                 this->driver_api[this->driver_api_count].ih_driver_values_id     = gxh_StringToInt(  std::string( (const char*)sqlite3_column_text(stmt,3)  ) );       
                 
                 const char* tmp2                                                 = (const char*)sqlite3_column_text(stmt,4);
                 memset(this->driver_api[this->driver_api_count].label_name, 0, 100);
                 memcpy(this->driver_api[this->driver_api_count].label_name, tmp2, strlen(tmp2));
                 
                 const char* tmp3                                                 = (const char*)sqlite3_column_text(stmt,5);
                 memset(this->driver_api[this->driver_api_count].button_name, 0, 50 );
                 memcpy(this->driver_api[this->driver_api_count].button_name, tmp3, strlen(tmp3));
                 
                 const char* tmp4                                                 = (const char*)sqlite3_column_text(stmt,6);
                 memset(this->driver_api[this->driver_api_count].value_type, 0, 100);
                 memcpy(this->driver_api[this->driver_api_count].value_type, tmp4, strlen(tmp4));
                 
                 this->driver_api[this->driver_api_count].func_number             = gxh_StringToInt(  std::string( (const char*)sqlite3_column_text(stmt,7)  ));   
                 this->driver_api[this->driver_api_count].func_internal_number    = gxh_StringToInt(  std::string( (const char*)sqlite3_column_text(stmt,8)  ) );   
                 
                 const char* tmp5                                                 = (const char*)sqlite3_column_text(stmt,9);
                 memset(this->driver_api[this->driver_api_count].select_val, 0, 1000 );
                 memcpy(this->driver_api[this->driver_api_count].select_val, tmp5, strlen(tmp5) );
                 
                 const char* tmp6                                                 = (const char*)sqlite3_column_text(stmt,10);
                 memset(this->driver_api[this->driver_api_count].replace_val, 0, 1000 );
                 memcpy(this->driver_api[this->driver_api_count].replace_val, tmp6, strlen(tmp6));
                 
                 this->driver_api[this->driver_api_count].pr_standard             = gxh_StringToInt(  std::string( (const char*)sqlite3_column_text(stmt,11)  ) );   
                 this->driver_api[this->driver_api_count].pr_standard_plus        = gxh_StringToInt(  std::string( (const char*)sqlite3_column_text(stmt,12)  ));   
                 this->driver_api[this->driver_api_count].pr_administrator        = gxh_StringToInt(  std::string( (const char*)sqlite3_column_text(stmt,13)  ) );   
                 this->driver_api[this->driver_api_count].pr_system               = gxh_StringToInt(  std::string( (const char*)sqlite3_column_text(stmt,14)  ));                                        
                 
                 //  std::cout<<"\n"<<this->driver_api[this->driver_api_count].label_name<<", "<<this->driver_api[this->driver_api_count].control_type;
                 
                 this->driver_api_count++;
                 
                 if( this->driver_api_count >=   MAX_DRIVER_API-1  )
                  {
                   this->show_error("gxh_core","loadPorts","No space for driver api",0); 
                   done = true;
                  }                                                 
                      
              }               
             if(res == SQLITE_DONE)done = true;                                     
          }       
          sqlite3_finalize(stmt);                   
      }
      
      this->registerResource(this->driver_api, this->driver_api_count, RES_VECTOR_DRIVER_API);
      
      
      //.....dirver image..............
      this->driver_image_count = 0;
      this->destroyResource(RES_VECTOR_DRIVER_IMAGE);
      this->show_log_core( "gxh_core", "loadPorts", "Get images of api", 6);
      
      query = "SELECT id, ih_sl_driver_id, name, path FROM ih_sl_driver_image WHERE is_delete='0'; ";  
      this->show_log_data_base("gxh_core","loadPorts", query.c_str() ,6);  
      
      queryChar = query.c_str();        
      if(sqlite3_prepare(dbSqLite , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
      {
         this->show_error("gxh_core","loadPorts", sqlite3_errstr( sqlite3_errcode(dbSqLite  ) ),4);          
         sqlite3_finalize(stmt);                   
      }else
      {
          bool done = false;       
          while (!done)               
          { 
             int res = sqlite3_step (stmt);        
             if(res ==  SQLITE_ROW)
              {                                                                        
                this->driver_image[this->driver_image_count].id                  = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt,0)  ) ); 
                this->driver_image[this->driver_image_count].ih_sl_driver_id     = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt,1)  ) );                      
                
                const char* tmp1                                                 = (const char*)sqlite3_column_text(stmt,1);
                memset(this->driver_image[this->driver_image_count].name, 0, 500 );
                memcpy(this->driver_image[this->driver_image_count].name, tmp1, strlen(tmp1));
                
                const char* tmp2                                                 = (const char*)sqlite3_column_text(stmt,1);
                memset(this->driver_image[this->driver_image_count].path, 0, 1000);
                memcpy(this->driver_image[this->driver_image_count].path, tmp2, strlen(tmp2) );
                
                driver_image_count++;
                
                if( this->driver_image_count >=   MAX_DRIVER_IMAGE-1  )
                  {
                   this->show_error("gxh_core","loadPorts","No space for driver image",0); 
                   done = true;
                  }
                
              }               
              if(res == SQLITE_DONE)done = true;                                     
          }
          sqlite3_finalize(stmt); 
      }
      
      this->registerResource(this->driver_image, this->driver_image_count, RES_VECTOR_DRIVER_IMAGE);
       */
      
      
      
      //pobierz zminne systemowe powiazane z urzadzeniami...
      this->driver_value_count = 0;
      this->destroyResource(RES_VECTOR_DRIVER_VALUE);
      this->show_log_core( "gxh_core", "loadPorts", "Get values of driver", 6);
      
      for(unsigned int wx=0; wx<MAX_DRIVER_VALUE; wx++) 
      {
          this->driver_values[wx].free           = true;
          this->driver_values[wx].status         = 0;
          this->driver_values[wx].ih_driver_id   = -1;
          this->driver_values[wx].px_group_id    = -1;
          this->driver_values[wx].const_function = 0;
      }
      
      query = "SELECT id, ih_driver_id,name_param,value, const_function,px_group_id FROM ih_driver_values WHERE is_delete='0'";  
      this->show_log_data_base("gxh_core","loadPorts", query.c_str() ,6);  
      
      queryChar = query.c_str();        
      if(sqlite3_prepare(dbSqLite , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
      {
         this->show_error("gxh_core","loadPorts", sqlite3_errstr( sqlite3_errcode(dbSqLite  ) ),4);          
         sqlite3_finalize(stmt);    
         
         this->show_error("gxh_core","loadPorts", "FATAL!!!! Restarting...... 0xLoadPort005" ,2);  
         exit(0);
           
      }else
      {
          bool done = false;       
          while (!done)               
          { 
             int res = sqlite3_step (stmt);        
             if(res ==  SQLITE_ROW)
              {                          
                if(sqlite3_column_text(stmt,0) == NULL) continue;
                if(sqlite3_column_text(stmt,1) == NULL) continue;
                if(sqlite3_column_text(stmt,4) == NULL) continue;
                if(sqlite3_column_text(stmt,5) == NULL) continue;
                 
                this->driver_values[this->driver_value_count].id                  = gxh_StringToInt( std::string(  (const char*)sqlite3_column_text(stmt,0) )  );                                                     
                this->driver_values[this->driver_value_count].ih_driver_id        = gxh_StringToInt( std::string(  (const char*)sqlite3_column_text(stmt,1) ) );                                                     
                
                const char* tmp1                                                  = (const char*)sqlite3_column_text(stmt,2);
                memset(this->driver_values[this->driver_value_count].name_param, 0, 255 );
                memcpy(this->driver_values[this->driver_value_count].name_param, tmp1, strlen(tmp1) );
                
                const char* tmp2                                                  = (const char*)sqlite3_column_text(stmt, 3);
                memset(this->driver_values[this->driver_value_count].value, 0, 1000 );
                memcpy(this->driver_values[this->driver_value_count].value, tmp2, strlen(tmp2) );
                
                this->driver_values[this->driver_value_count].const_function      = gxh_StringToInt( std::string(  (const char*)sqlite3_column_text(stmt,4) ));    
                this->driver_values[this->driver_value_count].px_group_id         = gxh_StringToInt( std::string(  (const char*)sqlite3_column_text(stmt,5) ));                
                this->driver_values[this->driver_value_count].toHistory           = false;                                  
                
                this->driver_values[ this->driver_value_count ].free = false;
                this->driver_values[ this->driver_value_count ].status = 0;
                
                
                std::cout<<"\nLoaded: "<<  this->driver_values[this->driver_value_count].id <<" | "<<tmp1;
                
                this->driver_value_count++;                                
                if( this->driver_value_count >=   MAX_DRIVER_VALUE-1  )
                  {
                   this->show_error("gxh_core","loadPorts","No space for driver value",0); 
                   done = true;
                  }
                
                
                                  
              }               
              if(res == SQLITE_DONE)done = true;                                     
          }
              
          sqlite3_finalize(stmt); 
          
      }
      
         
      this->driver_value_count =  MAX_DRIVER_VALUE;          
      this->registerResource(this->driver_values, MAX_DRIVER_VALUE , RES_VECTOR_DRIVER_VALUE);
         
      
 
    
      //załaduj porty
      for(unsigned int i=0; i<this->computer_ports_count; i++)
      {                    
          if( (gxh_compare( (const char*)this->computer_ports[i].typ, "RS485") == 0  )  || (gxh_compare((const char*)this->computer_ports[i].typ, "RS232") == 0  )   )
          {   
            
              this->ports[ i ] = new app_port_serial (this,(const char*)this->computer_ports[i].name_on_computer, (const char*)this->computer_ports[i].path,  (const char*)this->computer_ports[i].typ,this->computer_ports[i].id, (const char*)this->computer_ports[i].parity, (const char*)this->computer_ports[i].stop_bit, this->computer_ports[i].speed , (const char*)this->computer_ports[i].data);            
              //this->ports[i]->startLoop();
          }                           
      }
      
      sleep(1);
      
        //uruchom wątki dla portów..
      for(unsigned int i=0; i<this->computer_ports_count; i++)
      {                    
          if( (gxh_compare( (const char*)this->computer_ports[i].typ, "RS485") == 0  )  || (gxh_compare((const char*)this->computer_ports[i].typ, "RS232") == 0  )   )
          {   
            
             // this->ports[ i ] = new app_port_serial (this,(const char*)this->computer_ports[i].name_on_computer, (const char*)this->computer_ports[i].path,  (const char*)this->computer_ports[i].typ,this->computer_ports[i].id, (const char*)this->computer_ports[i].parity, (const char*)this->computer_ports[i].stop_bit, this->computer_ports[i].speed , (const char*)this->computer_ports[i].data);            
              this->ports[i]->startLoop();
          }                           
      }
    
     //-----
    
    
           
    sqlite3_close_v2(dbSqLite);
}

//------------------------------------------------------------------------------

bool gxh_core::registerVarSys(const char* name, int uniqueId, int px_group_id, int ih_driver_id, const char* value )
{
    
      unsigned int size = 0;
      ih_driver_values* varArr =  ( ih_driver_values*)this->getResource(RES_VECTOR_DRIVER_VALUE, &size);
      
      
      for(unsigned int x=0; x<size; x++)
      {
         if(varArr[x].const_function == uniqueId && varArr[x].px_group_id == px_group_id && varArr[x].ih_driver_id == ih_driver_id)   
         {
             std::string msg = "VarSys is already exist: ";
             msg.append(name);
             this->show_log_core( "gxh_core", "registerVarSys", msg.c_str(), 4);
             
             return false; //zmienna już istnieje.....
         }
      }
      
      int freeIndex = -1;
      
      for(unsigned int x=0; x<size; x++)
      {
          if(varArr[x].free)
          {
              x = freeIndex; 
              break;
          }
      }
      
      if(freeIndex == -1)
      {
          this->show_error("gxh_core","registerVarSys", "brak miejsca dla nowej zmiennej systemowej",4); 
          return false;
      }
      


      if(strlen(value) > 999)
      {
          this->show_error("gxh_core","registerVarSys", "var sys max 1000 chars.",4);  
          return false;
      }
      
      if(strlen(name) > 254)
      {
          this->show_error("gxh_core","registerVarSys", "name max 255 chars.",4);  
          return false;
      }
      
      
            varArr[freeIndex].free = false;
            varArr[freeIndex].id = -1;
                                       
           
            memset(varArr[freeIndex].value, 0, strlen(value)+1);              
            memcpy(varArr[freeIndex].value, value, strlen(value));              
            varArr[freeIndex].toHistory = false; 
                         
            varArr[freeIndex].const_function = uniqueId;
            varArr[freeIndex].ih_driver_id = ih_driver_id;
            varArr[freeIndex].px_group_id = px_group_id;                                                
                         
           
            memset(varArr[freeIndex].name_param, 0, strlen(name)+1);      
            memcpy(varArr[freeIndex].name_param, name,  strlen(name) );
                         
            varArr[freeIndex].status    = 2; //add to base..
                         
            
             std::string msg = "VarSys new: ";
             msg.append(name);
             this->show_log_core( "gxh_core", "registerVarSys", msg.c_str(), 4);
    
    return true; 
}

//------------------------------------------------------------------------------


bool gxh_core::createBase()
{
    std::string baza = this->getDB();//this->getRunPath();
    //baza.append("../gxh_base.db");  
    
    std::string baza2 = this->getDBTmp();//his->getRunPath();
    //baza2.append("../gxh_base_tmp.db");  
    
    //baza 1..    
    std::ifstream infile(baza.c_str());
    if(!infile.good()) //brak pliku..
    {
          std::ofstream o(baza.c_str());                  
          this->show_log_core("gxh_core","createBase", "gxh_base.db" ,1); 
    }else
    {
        this->show_log_core("gxh_core","createBase", "gxh_base.db is already exist" ,1); 
    }
    
    
    //baza 2
    std::ifstream infile2(baza2.c_str());
    if(!infile2.good()) //brak pliku..
    {
        std::ofstream o(baza2.c_str());
        this->show_log_core("gxh_core","createBase", "gxh_base_tmp.db" ,1); 
    }else
    {
                this->show_log_core("gxh_core","createBase", "gxh_base_tmp.db is already exist" ,1); 
    }
    
    
    return true;
}

//------------------------------------------------------------------------------

void gxh_core::clearDataBaseTmp()
{
    
    //this->deleteTmpQuery("DELETE FROM px_session WHERE created_at < datetime('now','localtime','-30 day') AND session_id NOT LIKE '%GXH%';");
    this->deleteTmpQuery("DELETE FROM ih_driver_port_history WHERE created_at < datetime('now','localtime','-10 day');");
    this->deleteTmpQuery("DELETE FROM ih_lighting_h WHERE created_at < datetime('now','localtime','-10 day');");
    this->deleteTmpQuery("DELETE FROM ih_driver_values_history WHERE created_at < datetime('now','localtime','-10 day');");
    this->deleteTmpQuery("DELETE FROM ih_nfc_card_event WHERE created_at < datetime('now','localtime','-10 day');");
    this->deleteTmpQuery("DELETE FROM ih_history WHERE created_at < datetime('now','localtime','-10 day');");
    this->deleteTmpQuery("DELETE FROM ih_notice WHERE created_at < datetime('now','localtime','-10 day');");
    
    // resize file tmp.. ;)
    /*
    this->exec("rm /var/www/html/db/tmp_x.db"); //if exist..
    this->exec("sqlite3 /var/www/html/db/gxh_base_tmp.db  \".dump\" | grep -v TRANSACTION | grep -v ROLLBACK | grep -v COMMIT |  sqlite3 /var/www/html/db/tmp_x.db");           
    this->exec("rm /var/www/html/db/gxh_base_tmp.db"); 
    this->exec("sqlite3 /var/www/html/db/tmp_x.db  \".dump\" | sqlite3 /var/www/html/db/gxh_base_tmp.db"); 
    this->exec("chmod 775 /var/www/html/db/gxh_base_tmp.db"); 
    this->exec("chown www-data /var/www/html/db/gxh_base_tmp.db"); 
    this->exec("chgrp www-data /var/www/html/db/gxh_base_tmp.db"); 
    this->exec("rm /var/www/html/db/tmp_x.db"); //
   */
}

//------------------------------------------------------------------------------

 
bool gxh_core::deleteTmpQuery(const char* schema)
{
    bool add = false;
    sqlite3* dbSqLite;               
                                               
        std::string baza = this->getDBTmp();
     
        int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
        if( r != SQLITE_OK )
        {                         
             this->show_error("app_module_wsdl","deleteTmpQuery","Cannot open sqlLite tmp ",0);                 
        }else
        {
            
            char *zErrMsg = 0;     
            int rc = sqlite3_exec( dbSqLite , schema, 0, 0, &zErrMsg);                                                       
            if( rc != SQLITE_OK )  
            {
                this->show_error("gxh_core","deleteTmpQuery",schema,4); 
                this->show_error("gxh_core","deleteTmpQuery",zErrMsg,4); 
                sqlite3_free(zErrMsg); 
            }
            if( rc == SQLITE_OK ) 
               {                                                                                  
                add = true;
                
                    std::string msg = "Tmp deleted:  ";
                    msg.append( schema );        
                    this->show_log_core("gxh_core","deleteTmpQuery", msg.c_str() ,1); 
                
               }                         
            sqlite3_close_v2(dbSqLite);         
        }
        
        return add;
}

//------------------------------------------------------------------------------

bool gxh_core::createTable(const char* schema, bool tmp)
{
    bool add = false;
    sqlite3* dbSqLite;               
                                               
        std::string baza = "";
        if(!tmp)baza =  this->getDB();
        if( tmp)baza =  this->getDBTmp();
     
        int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
        if( r != SQLITE_OK )
        {
            if(!tmp) this->show_error("app_module_wsdl","createTable","Cannot open sqlLite ",0);                 
            if( tmp) this->show_error("app_module_wsdl","createTable","Cannot open sqlLite tmp ",0);                 
        }else
        {
            
            char *zErrMsg = 0;     
            int rc = sqlite3_exec( dbSqLite , schema, 0, 0, &zErrMsg);                                                       
            if( rc != SQLITE_OK )  
            {
                this->show_error("gxh_core","createTable",schema,4); 
                this->show_error("gxh_core","createTable",zErrMsg,4); 
                sqlite3_free(zErrMsg); 
            }
            if( rc == SQLITE_OK ) 
               {                                                                                  
                add = true;
                
                    std::string msg = "Table created:  ";
                    msg.append( schema );        
                    this->show_log_core("gxh_core","createTable", msg.c_str() ,1); 
                
               }                         
            sqlite3_close_v2(dbSqLite);         
        }
        
        return add;
}

//------------------------------------------------------------------------------

bool gxh_core::registerFunctionSys( unsigned int uniqueId, const char* description, const char* type, const char* module, const char* run_as_service, const char* delay_ms, const char* param1Name, const char* param2Name, const char* resName , const char* min_version)
{
    
     std::string modulFile = this->getRunPath();
    modulFile.append(module);  
     
    
    //baza 1..    
    std::ifstream infile(modulFile.c_str());
    if(!infile.good()) //brak pliku..
    {            
          std::string info = "Cannot register function: ";
          info.append(description);
          info.append(", library not found ");
          info.append(module);          
          this->show_error("gxh_core","registerFunctionSys", info.c_str()  ,1); 
          return false;
    } 
    
    
    
    
    
    int indexModule   = -1;
    int indexFunction = -1;
    
     for(unsigned int m=0;m<this->modules_count;m++) 
     {         
             for(unsigned int y=0; y< this->modules[m].module_function_count; y++)
             {
                 if(this->modules[m].functions[y].const_function_symbol == uniqueId)
                 {
                     indexModule = m;
                     indexFunction = y;
                     
                     m = this->modules_count + 1; //break;
                     break;
                 }
             }        
     }    
    
    if(indexModule > -1 && indexFunction > -1)
    {
       std::string msg = "Function is already exist: ";
       msg.append( description );
        
       this->show_log_core("gxh_core","registerFunctionSys", msg.c_str() ,1);  
       return false; 
    }
    
    /////    
    bool add = false;   
    
    //Dodaj funkcje do bazy danych....    
        sqlite3* dbSqLite;               
                                               
        std::string baza = this->getDB(); //this->getRunPath();
        //baza.append("../gxh_base.db");
     
        int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
        if( r != SQLITE_OK )
        {
            this->show_error("app_module_wsdl","registerFunctionSys","Cannot open sqlLite 1",0);                 
        }else
        {
            std::string query="";
            query.append("INSERT INTO ih_sl_const_function_symbol (id,description,const_function_symbol,created_at,updated_at,is_delete,type, module, order_by, used,used_from_mobile, run_as_service ,run_ms_delay ,  author_name ,author_firm , author_email ,author_phone ,author_www ,author_key,app_min_version,info,logo,param2_name,param1_name) VALUES(null,'");      
            query.append(description);
            query.append("','");
            query.append(gxh_IntToString(uniqueId));
            query.append("',DATETIME('now','localtime'), DATETIME('now','localtime'),'0','");
            query.append(type);
            query.append("','");
            query.append(module);
            query.append("','1','1','1','");
            query.append(run_as_service);
            query.append("','");
            query.append(delay_ms);
            query.append("','GXH System','GXH System','kontakt@gxh.pl','?','http://gxh.pl','?','");
            query.append(min_version);
            query.append("','Brak dodatkowych informacji o tej funkcji.','?','");
            query.append(param2Name);
            query.append("','");
            query.append(param1Name);
            query.append("');");
         
    
            
            char *zErrMsg = 0;     
            int rc = sqlite3_exec( dbSqLite , query.c_str(), 0, 0, &zErrMsg);                                                       
            if( rc != SQLITE_OK )  
            {
                this->show_error("gxh_core","registerFunctionSys",query.c_str(),4); 
                this->show_error("gxh_core","registerFunctionSys",zErrMsg,4); 
                sqlite3_free(zErrMsg); 
            }
            if( rc == SQLITE_OK ) 
               {                                                                                  
                add = true;
                
                    std::string msg = "Function has beed added ";
                    msg.append( description );        
                    this->show_log_core("gxh_core","registerFunctionSys", msg.c_str() ,1); 
                
               }                         
            sqlite3_close_v2(dbSqLite);         
        }
    
        
        
        r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
        if( r != SQLITE_OK )
        {
            this->show_error("app_module_wsdl","registerFunctionSys","Cannot open sqlLite 1",0);                 
        }else
        {
            std::string query="";
            query.append("INSERT INTO ih_sl_const_function_hash (id,name, symbol, created_at, updated_at, is_delete) VALUES(null,'");      
            query.append(resName);            
            query.append("','");
            query.append(gxh_IntToString(uniqueId));
            query.append("', DATETIME('now','localtime'), DATETIME('now','localtime'),'0') ");
         
    
            
            char *zErrMsg = 0;     
            int rc = sqlite3_exec( dbSqLite , query.c_str(), 0, 0, &zErrMsg);                                                       
            if( rc != SQLITE_OK )  
            {
                this->show_error("gxh_core","registerFunctionSys",query.c_str(),4); 
                this->show_error("gxh_core","registerFunctionSys",zErrMsg,4); 
                sqlite3_free(zErrMsg); 
            }
            if( rc == SQLITE_OK ) 
               {                                                                                  
                add = true;
                
                   std::string msg = "Hash has beed added ";
                    msg.append( resName );        
                    this->show_log_core("gxh_core","registerFunctionSys", msg.c_str() ,1); 
               }                         
            sqlite3_close_v2(dbSqLite);         
        }
        
        /////
    
    return add;
}

//------------------------------------------------------------------------------

bool gxh_core::addNotice(int px_group_id,const char* from, const char* title, const char* msg, int type, ih_notice_ex* notice_ex)
{
    bool isFree = false;
    
    for(unsigned int x=0; x< MAX_NOTICE_CACHE; x++)
    {
        if(this->notices[x].free )
        {
            isFree = true;
            break;
        }
    }
    
    if(!isFree) //wyczyść wszystkie powiadomienia..
    {
       //notice...
       for(int i=0; i<MAX_NOTICE_CACHE;i++)
       {
           memset(&this->notices[i], 0, sizeof(ih_notice) );
           this->notices[i].free = true; 
           this->notices[i].readed = false; 
           this->notices[i].px_group_id = 0; 
       }
    }
    
    for(unsigned int x=0; x< MAX_NOTICE_CACHE; x++)
    {
        if(this->notices[x].free )
        {
           
            
           this->notices[x].px_group_id = px_group_id;
           this->notices[x].type = type;
           
           memcpy(this->notices[x].from, from, strlen(from) );
           
           int size = 50; // max
           if(strlen(title) < 50) size = strlen(title);
           memset(this->notices[x].title, 0, 50);
           memcpy(this->notices[x].title, title, size);
           
           unsigned int size2 = 1024;
           if(strlen(msg)  < size2) size2 = strlen(msg) ;
           
           memset(this->notices[x].content, 0, 1024 );
           memcpy(this->notices[x].content, msg, size2 );
           
           std::string tmpT = gxh_currentDateTime();           
           
           unsigned int size3 = 15;
           if(strlen(tmpT.c_str() ) < 15) size3= strlen(tmpT.c_str() );
           
           
           memcpy(this->notices[x].status_time_str, tmpT.c_str(), size3 );
                                
            
           this->notices[x].free   = false; 
           this->notices[x].readed = false; 
           
           
           
           this->notices[x].checkResponse = false;
           
           memset( this->notices[x].user_response,0, sizeof(this->notices[x].user_response) );
           
           this->notices[x].p1_const_function_response = 0;
           this->notices[x].p1_module_function_response = 0;
           this->notices[x].p1_resource_id = 0;
           
           memset( this->notices[x].p1_param1,0, sizeof(this->notices[x].p1_param1) );
           memset( this->notices[x].p1_param2,0, sizeof(this->notices[x].p1_param2) );
           
           
           this->notices[x].p2_const_function_response = 0;
           this->notices[x].p2_module_function_response = 0;
           this->notices[x].p2_resource_id = 0;
           
           memset( this->notices[x].p2_param1,0, sizeof(this->notices[x].p2_param1) );
           memset( this->notices[x].p2_param2,0, sizeof(this->notices[x].p2_param2) );
           
           
           std::string msg =" new message from ";
           msg.append(from);
           
           this->show_log_core( "gxh_core", "addNotice", msg.c_str(), 4);
           
           
           if(notice_ex != NULL)
           {
                 this->notices[x].checkResponse = true;
            
                 this->notices[x].p1_const_function_response = notice_ex->p1_const_function_response;
                 this->notices[x].p1_module_function_response = notice_ex->p1_module_function_response;
                 this->notices[x].p1_resource_id = notice_ex->p1_resource_id;
                 
                 memcpy( this->notices[x].p1_param1, notice_ex->p1_param1, sizeof( notice_ex->p1_param1 ) );
                 memcpy( this->notices[x].p1_param2, notice_ex->p1_param2, sizeof( notice_ex->p1_param2 ) );
                 
                 
                 this->notices[x].p2_const_function_response = notice_ex->p2_const_function_response;
                 this->notices[x].p2_module_function_response = notice_ex->p2_module_function_response;
                 this->notices[x].p2_resource_id = notice_ex->p2_resource_id;
                 
                 memcpy( this->notices[x].p2_param1, notice_ex->p2_param1, sizeof( notice_ex->p2_param1 ) );
                 memcpy( this->notices[x].p2_param2, notice_ex->p2_param2, sizeof( notice_ex->p2_param2 ) );
                 
           }
           
           
           return true;
        }                                
    }
    
    return false;
}

//------------------------------------------------------------------------------

ih_sl_driver* gxh_core::getSlDriver(int* count)
{
    *count = sldriver_count;
    return this->sldriver;
}

//------------------------------------------------------------------------------

ih_computer_port* gxh_core::getComputerPorts(int* count)
{
    *count = this->computer_ports_count;
    return  this->computer_ports;
}

//------------------------------------------------------------------------------

ih_driver* gxh_core::getDrivers(int* count)
{
    *count = this->drivers_count;
    return this->drivers;
}

//------------------------------------------------------------------------------

 ih_driver_port* gxh_core::getPortsDriver(int* count)
 {
     *count = this->driver_ports_count;
     return this->driver_ports;
 }

//------------------------------------------------------------------------------

void gxh_core::get_resource_name( sqlite3* dbHandle, char* ret,int bufSize, int const_function_symbol)
{
   
    memset(ret,0, bufSize);
    
    std::string query = "SELECT name FROM ih_sl_const_function_hash WHERE is_delete='0' AND symbol='";
    query.append( gxh_IntToString( const_function_symbol ) );
    query.append("'; ");
    
    this->show_log_data_base("gxh_core","get_resource_name", query.c_str(),14 );    
        
  
    
    sqlite3_stmt* stmt; 
    
    const char* queryChar = query.c_str();       
    if(sqlite3_prepare(dbHandle , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
    {
         this->show_error("gxh_core","get_resource_name", sqlite3_errstr( sqlite3_errcode(dbHandle  ) ),4); 
         sqlite3_finalize(stmt);   
         return;
    }
    
    bool done = false;  
   
    while (!done)               
    { 
       int res = sqlite3_step (stmt);
      
         if(res ==  SQLITE_ROW)
         {                                   
            const char* text  = (const char*)sqlite3_column_text(stmt, 0);  
            memcpy(ret, text, strlen(text));      
         } 
              
          if(res == SQLITE_DONE)done = true;                            
        
    }

    sqlite3_finalize(stmt);   
     
}
 
//------------------------------------------------------------------------------

void gxh_core::get_resource_name_func(sqlite3* dbHandle, int resource_id, char* ret, char* resName, int resNameSize)
{
     std::string query = "";
     memset(resName, 0, resNameSize);
    
     
     //oświetlenie....
     if(gxh_compare(ret,"light") == 0) 
     {
         query = "SELECT ih_lighting.name, ih_zone.name as zone FROM ih_lighting, ih_zone WHERE  ih_zone.id=ih_lighting.ih_zone_id AND ih_lighting.id='";
         query.append( gxh_IntToString(resource_id) );
         query.append("' ");                  
     }
     else
     if(gxh_compare(ret,"blind") == 0) 
     {
         query = "SELECT ih_blind.name, ih_zone.name as zone FROM ih_blind, ih_zone WHERE  ih_zone.id=ih_blind.ih_zone_id AND  ih_blind.id='";
         query.append( gxh_IntToString(resource_id) );
         query.append("' ");                  
     }else
     if(gxh_compare(ret,"gate") == 0) 
     {
         query = "SELECT ih_gate.name, ih_zone.name as zone FROM ih_gate, ih_zone WHERE  ih_zone.id=ih_gate.from_ih_zone_id AND ih_gate.id='";
         query.append( gxh_IntToString(resource_id) );
         query.append("' ");                  
     }else
     if(gxh_compare(ret,"sensor") == 0) 
     {
         query = "SELECT ih_sensor.name, ih_zone.name as zone FROM ih_sensor, ih_zone WHERE  ih_zone.id=ih_sensor.ih_zone_id AND ih_sensor.id='";
         query.append( gxh_IntToString(resource_id) );
         query.append("' ");                  
     }else
     {
         return;
     }
     
     query.append("; ");    
     
  
    this->show_log_data_base("gxh_core","get_resource_name_func", query.c_str(),12 );  
    
    
     
    sqlite3_stmt* stmt; 
    
    const char* queryChar = query.c_str();       
    if(sqlite3_prepare(dbHandle , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
    {
         this->show_error("gxh_core","get_resource_name_func", sqlite3_errstr( sqlite3_errcode(dbHandle  ) ),4); 
         sqlite3_finalize(stmt);   
         return;
    }
    
    bool done = false;  
   
    while (!done)               
    { 
       int res = sqlite3_step (stmt);
       
         if(res ==  SQLITE_ROW)
         {         
            const char* text  = (const char*)sqlite3_column_text(stmt, 0);  
            memcpy(resName, text, strlen(text));
         } 
              
          if(res == SQLITE_DONE)done = true;                            
        
    }

    sqlite3_finalize(stmt);   
          
}

//------------------------------------------------------------------------------

unsigned int  gxh_core::createGXHHandle(const char* resName, int resId, char resType)
{
    pthread_mutex_lock(&this->gxh_handle_muteks);       
    
    unsigned int handle = 0;
    
    for(unsigned int i=1; i<MAX_GXH_HANDLE_LIST;i++)
    {
       if( this->gxh_list[i].status == 1) //free
       {
           this->gxh_list[i].status = 2; //lock
           
           this->gxh_list[i].gxh_handle = i;
           this->gxh_list[i].resId  = resId;
           this->gxh_list[i].resType = resType;
           this->gxh_list[i].msg_queue   = new app_zc_queue(this);
           memcpy(this->gxh_list[i].name, resName, strlen(resName) );
           
           
           this->gxh_list[i].status = 3; //used.. 
           handle = i;
           
           std::string msg = "New gxhHandle: ";
           msg.append( gxh_IntToString(handle) );
           msg.append(" for ");
           msg.append(resName);
           
           this->show_log_core( "gxh_core", "createGXHHandle", msg.c_str() , 6);
           
           pthread_mutex_unlock(&this->gxh_handle_muteks);     
           return handle;
       }
    }
    
    this->show_error("gxh_core", "createGXHHandle", "No space for new handle", 5);
    
    pthread_mutex_unlock(&this->gxh_handle_muteks);     
    return handle; //error
}

//------------------------------------------------------------------------------

void gxh_core::destroyGXHHandle(unsigned int gxhHandle)
{
   pthread_mutex_lock(&this->gxh_handle_muteks);       
   
   for(unsigned int i=1; i<MAX_GXH_HANDLE_LIST;i++)
    {
       if( this->gxh_list[i].status == 3 &&   this->gxh_list[i].gxh_handle == gxhHandle ) //not free
       {  
           this->gxh_list[i].status = 2; //lock
           
           std::string msg = "Destroy gxhHandle: ";
           msg.append( gxh_IntToString(gxhHandle) );
           msg.append(" for ");
           msg.append(this->gxh_list[i].name);
           
           this->gxh_list[i].gxh_handle = 0;
           this->gxh_list[i].resId  = 0;  
           this->gxh_list[i].resType = 0;
           memset(this->gxh_list[i].name, 0, 255 );
           delete this->gxh_list[i].msg_queue;
          
           this->gxh_list[i].status = 1; //feee                   
           
           this->show_log_core( "gxh_core", "destroyGXHHandle", msg.c_str() , 6);
   
           pthread_mutex_unlock(&this->gxh_handle_muteks);       
           return;
       }
    }
   
    pthread_mutex_unlock(&this->gxh_handle_muteks);       
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

unsigned int gxh_core::getGXHHandleByIdAndtype(int resId, char resType)
{ 
        
    for(int i=0; i<MAX_GXH_HANDLE_LIST; i++)
    {
        if(this->gxh_list[i].resId == resId && this->gxh_list[i].resType==resType  && this->gxh_list[i].status == 3 )
        {           
            return gxh_list[i].gxh_handle;                    
        }
    }
         
    return 0;
}
 

//------------------------------------------------------------------------------

bool gxh_core::registerResource(void* ptr, unsigned int size,unsigned int type)
{
    if(type >= 100) 
    {
        this->show_error("gxh_core", "registerResource", "Cannot register resource 0x000A1", 6);
        return false;
    }
    
    if(    this->res_verctor[type].free == false) 
    {
        this->show_error("gxh_core", "registerResource", "Cannot register resource 0x0001", 6);
        return false;
    }
    
    this->res_verctor[type].ptr    = ptr;
    this->res_verctor[type].lenght = size;
    this->res_verctor[type].free = false;
    
    this->show_log_core( "gxh_core", "registerResource", "OK" , 6);
    
    return true;
}

//------------------------------------------------------------------------------

bool gxh_core::destroyResource(unsigned int type)
{
    if(this->res_verctor[type].free == true) 
    {
        std::string msg = "Resource is empty: ";
        msg.append(gxh_IntToString(type));
        
        this->show_error("gxh_core", "destroyResource", msg.c_str(), 6);
        return false;
    }
    
        this->res_verctor[type].ptr = NULL;
        this->res_verctor[type].free = true;
        this->res_verctor[type].lenght = 0;
        
        std::string msg ="res: " ;
        msg.append(gxh_IntToString(type));
                
        this->show_log_core( "gxh_core", "destroyResource", msg.c_str() , 6);
        
        return true;
}

//------------------------------------------------------------------------------

void* gxh_core::getResource(unsigned int type, unsigned int* size)
{
    *size = 0;
    
     if(type >= 100) return NULL;   
     if(  this->res_verctor[type].free == true)
     {
         std::string msg = "Resource is empty: ";
         msg.append(gxh_IntToString(type));
                 
         
         this->show_error("gxh_core", "getResource", msg.c_str(), 6);
         return NULL;
     }
     
     
     *size =  this->res_verctor[type].lenght;
     return  this->res_verctor[type].ptr ; 
}

//------------------------------------------------------------------------------

bool gxh_core::getMessageForCore(gxh_message* ptr, unsigned int* index)
{
    
     for(unsigned int i=0; i<MAX_GXH_MESSAGE_INTERRUPT;i++)
     {
           if(this->gxh_msg_interrupt_buf[i].status == 3) //go execute
           {
               this->gxh_msg_interrupt_buf[i].status = 2; //lock
               memcpy( ptr, &this->gxh_msg_interrupt_buf[i].message,  sizeof(gxh_message) );
               *index = i;
               //this->gxh_msg_interrupt_buf[i].status = 1; //free  //wywołane po zakończeniu wątku.....
               return true;
           }
     }        
    return false;
}

//------------------------------------------------------------------------------

bool gxh_core::sendGXHMessage( gxh_message* gxhMessage)
{        
    if(gxhMessage->system_code == GXH_MESSAGE_REDIRECT)
    {
        return this->addResponse(gxhMessage);       
    }
    
     for(int i=0; i<MAX_GXH_MESSAGE_INTERRUPT;i++)
     {
           if(this->gxh_msg_interrupt_buf[i].status == 1)
           {
               this->gxh_msg_interrupt_buf[i].status = 2; //lock
               memcpy(  &this->gxh_msg_interrupt_buf[i].message, gxhMessage, sizeof(gxh_message) );
               this->gxh_msg_interrupt_buf[i].status = 3; //go execute..
               return true;
           }
     }   
    
    this->show_error("gxh_core", "sendGXHMessage", "No space for new messages", 6);
    
    return false;
}

//------------------------------------------------------------------------------

bool gxh_core::addResponse( gxh_message* gxhMessage)
{    
    
    if(gxhMessage->gxh_handle == NULL_DRIVER  )
    {
          this->show_error("gxh_core", "addResponse", "Response for null driver...", 16);
          return false;
    }
    
    if(gxhMessage->gxh_handle > MAX_GXH_HANDLE_LIST)
    {
          this->show_error("gxh_core", "addResponse", "Bad index for gxhHandle", 6);
          return false;
    }
  
    
  pthread_mutex_lock(&this->gxh_response_muteks);                      
  if( this->gxh_list[gxhMessage->gxh_handle].msg_queue->add(gxhMessage) >0) 
  {
      pthread_mutex_unlock(&this->gxh_response_muteks); 
      return true;
  }
    
  pthread_mutex_unlock(&this->gxh_response_muteks); 
  return false;
}

//------------------------------------------------------------------------------

bool gxh_core::receiveGXHMessage( unsigned int gxhHandle, gxh_message* gxhMessage, int time_out_ms)
{                     
    
    int ms = 0;    
    
    if(gxhHandle > MAX_GXH_HANDLE_LIST)
    {
          this->show_error("gxh_core", "receiveGXHMessage", "Bad index for gxhHandle", 6);
          return false;
    }
   
 
    for(int a=0; a<10000; a++)
    {  
    
        pthread_mutex_lock(&this->gxh_response_muteks);   
        if(this->gxh_list[gxhHandle].msg_queue->get(gxhMessage))
        {    
            pthread_mutex_unlock(&this->gxh_response_muteks);   
            return true;
        }
         pthread_mutex_unlock(&this->gxh_response_muteks);   
        
         
        if(ms >= time_out_ms) 
        {                      
            return false;
        }
         
         usleep(1000 * 2); //sleep 2 ms
         ms += 2;      
    }
         
    return false;
}

//------------------------------------------------------------------------------

ih_driver_port_function* gxh_core::getDriverPortFunction(int* count)
{
    *count = this->port_function_count;
    return this->port_function;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void* gxh_core::runThread()
{        
   
    this->show_log_core("gxh_core", "runThread", "starting main thread for interrupts", 0);
  
    while(this->thInterrupt)
    {
        gxh_message message;
        unsigned int index_buf = -1;
        bool isMsg = this->getMessageForCore(&message, &index_buf );
        
        if(isMsg && index_buf >=0 )
        {
            
            if(message.func_number == 0) //info for core..
            {
              //  std::cout<<"\n\nInfo for core: "<<message.func_internal_number;
                
                if(message.func_internal_number == 1) //close application..
                {
                  this->closeApplication();
                  this->show_log_core("gxh_core", "runThread", "stop main thread for interrupts", 0);    
                  return NULL;
                }
                
                 if(message.func_internal_number == 2) //restart komputer
                {
                  this->exec("reboot");
                  this->closeApplication();
                  this->show_log_core("gxh_core", "runThread", "restart computer", 0);    
                  return NULL;
                }
                
                if(message.func_internal_number == 10) //informacja dla Core o onClick, wykonaj akcje onClick w modułach w osobnym wątku..
                {                                                      
                   gxh_th_intr intr;
                   intr.handle = this;
                   intr.index  = index_buf;
            
                   pthread_attr_t attr2;
                   pthread_attr_init(&attr2);      
                   pthread_attr_setdetachstate(&attr2, PTHREAD_CREATE_DETACHED);  //po zakonczeniu zwolnij całą pamięc
                   int rc2 = pthread_create(&this->gxh_msg_interrupt_buf[index_buf].thread, &attr2,  &goThreadInterrupt, (void*)&intr );
                   if (rc2)
                     {                  
                       this->show_error("gxh_core", "runThread", "cannot start service thread", 0);
                     }else
                     {
                        pthread_detach( this->gxh_msg_interrupt_buf[index_buf].thread );//2022-03
                     }
                   pthread_attr_destroy(&attr2);   
                   usleep(1000 * 10); // czas na rozpoaczenie wątku... niech zainicjuje wszystkei zmienne po swojej stronie..                  
                   continue;
                }
                
                 if(message.func_internal_number == 20) //reload module
                {        
                    this->executeThreadTask = false;
                    
                    this->modules_count = 0;
                    this->functions_count = 0;
                    this->symbols_count = 0;
                    this->port_function_count = 0;
                    //sleep(2);  //czekaj az usługa się wstrzyma a inne wątki zakończą zadania
                    usleep(1000 * 100); //100 ms na zakonczenie watku..
                             
                     
                     //delete currently use module
                    for(int m=0;m<MAX_MODULE_BUFFER;m++) 
                    {
                        if( this->modules[m].ready == 1 )
                        {
                          this->modules[m].ready = 0;
                          this->modules[m].moduleObject->forceCancel();
                      //    usleep(1000 * 500); //owait 500 ms to cancel currently task
                          delete this->modules[m].moduleObject; 
                          dlclose( this->modules[m].handle );
                          this->modules[m].module_name = "?";
                        }
                    }                         
                     
                    this->loadModule();
                    this->show_log_core("gxh_core", "runThread", "reloadModule", 0);                    
                    
                    this->executeThreadTask = true;
                   
                }
                
                 if(message.func_internal_number == 3) //reload ports
                {            
                    this->executeThreadTask = false;
                    
                    this->drivers_count = 0;
                    this->driver_ports_count = 0;
                    this->sldriver_count = 0;
                    this->driver_api_count = 0;
                    this->driver_image_count = 0;
                    this->driver_value_count = 0;           
                    //sleep(2);  //czekaj az usługa się wstrzyma a inne wątki zakończą zadania
                    usleep(1000 * 100); //100 ms na zakonczenie watku..
                                        
                     
                     //close ports...
                    for(unsigned int i=0; i<this->computer_ports_count; i++)
                     {                                                 
                      if( ((const char*)gxh_compare(this->computer_ports[i].typ, "RS485") == 0  )  || (gxh_compare( (const char*)this->computer_ports[i].typ, "RS232") == 0  )   )
                      {             
                       this->ports[i]->stopLoop();
                       this->show_log_core("gxh_core", "runThread", "reloadPorts waiting...", 0);                       
                       delete this->ports[ i ];              
                      }     
                    }     
                  this->computer_ports_count = 0;
                  
                  this->loadPorts();
                  this->show_log_core("gxh_core", "runThread", "reloadPorts", 0);                    
                  
                  this->executeThreadTask = true;
                 
                }
                
                
                this->gxh_msg_interrupt_buf[index_buf].status = 1; //is free
                memset( &this->gxh_msg_interrupt_buf[index_buf].message,0, sizeof(gxh_message) );
                continue;
            }
            
            // std::cout<<"\nNew message: "<<message.func_number<<" inx: "<<index_buf<<"\n";
            
             gxh_th_intr intr;
             intr.handle = this;
             intr.index  = index_buf;
            
             pthread_attr_t attr2;
             pthread_attr_init(&attr2);      
             pthread_attr_setdetachstate(&attr2, PTHREAD_CREATE_DETACHED);  //po zakonczeniu zwolnij całą pamięc
             int rc2 = pthread_create(&this->gxh_msg_interrupt_buf[index_buf].thread, &attr2,  &goThreadInterrupt, (void*)&intr );
             if (rc2)
             {                  
                 this->show_error("gxh_core", "runThread", "cannot start service thread", 0);
             }else
             {
                  pthread_detach( this->gxh_msg_interrupt_buf[index_buf].thread );//2022-03
             }
             pthread_attr_destroy(&attr2);   
             usleep(1000 * 10); // czas na rozpoaczenie wątku... niech zainicjuje wszystkei zmienne po swojej stronie..
            
        }
        
        //usleep(1000 * 5); //5 ms 
        usleep(20); //5 ms 
    }

    this->show_log_core("gxh_core", "runThread", "stop main thread for interrupts", 0);    
    return NULL;
}

//------------------------------------------------------------------------------

bool gxh_core::execute( gxh_message* gxhMessage,  gxh_message* response)
{
    gxh_message message;
    memcpy(&message, gxhMessage, sizeof(gxh_message) );
    
    bool exOk = false;
  
    //znajdz moduł i funkcje potrzebną do wykonana i przetworzenia odebranej wiadomości..
    for(unsigned int m=0; m<this->modules_count; m++)
    {
       for(unsigned int f=0; f< this->modules[m].module_function_count; f++)
       {
           
           if(this->modules[m].functions[f].const_function_symbol == message.func_number )
           {
               
               if(this->modules[m].ready == 1 && this->modules[m].moduleObject != NULL )
               {
                       exOk = true;
                       
                       gxh_message res;                    
                       bool isResponse = this->modules[m].moduleObject->execute(&message, &res);
                       
                       if(isResponse && response != NULL )
                       {
                           memcpy(response, &res, sizeof(gxh_message) );                                                                                
                           return true;
                       }else
                       {                           
                           return false;
                       }
               }else
               {
                       this->show_error("gxh_core", "execute", "Sorry, module is not read for use", 6);
               }
               
               //break loop 
               f = this->modules[m].module_function_count + 1;
               m = this->modules_count + 1;
           }
       }                                
    }
      
    if(!exOk) 
    {
        this->show_error("gxh_core", "execute", "GXH cannot find module of function", 6);
        
          if(message.gxh_handle > 0) //urzadzenie oczekuje odpowiedzi..
        {
             gxh_message response;   
             response.gxh_handle = message.gxh_handle;
             response.system_code = GXH_MESSAGE_ERROR;
             const char* msg = "GXH cannot find module or function";
             memcpy(response.dataStruct, msg, strlen(msg) );
             response.sizeDataStruct = strlen(msg);
             
             this->addResponse( &response );
        }
        
    }
    
    return false;
}

//------------------------------------------------------------------------------

void* gxh_core::runThreadInterrupt(void* intr_str)
{  
    gxh_th_intr* intr = (gxh_th_intr*)intr_str;
    int index_buf = intr->index;
    
    gxh_message message;
    memcpy(&message, &this->gxh_msg_interrupt_buf[index_buf].message, sizeof(gxh_message));
    
     
    
    this->gxh_msg_interrupt_buf[index_buf].status = 4; //is executing now
    
    
    
    if(  message.func_number == 0 && message.func_internal_number == 10)          
    {                          
        struct tmListener
        {
         int ih_driver_port_id;
         int listener_status;
        };
                           
       tmListener tm;
       memcpy(&tm, message.dataStruct, sizeof(tmListener));
                                 
       
       for(unsigned int m=0; m<this->modules_count; m++)
       {
         for(unsigned int f=0; f< this->modules[m].module_function_count; f++)
         {
              if(this->modules[m].ready == 1 && this->modules[m].moduleObject != NULL )
              {
                    this->modules[m].moduleObject->onClick(tm.ih_driver_port_id, tm.listener_status); 
              }
         }
       }
       
       
       //clear....
       this->gxh_msg_interrupt_buf[index_buf].status = 1; //is free
       memset( &this->gxh_msg_interrupt_buf[index_buf].message,0, sizeof(gxh_message) );   
       return NULL;
    }
    
    
            
    //this->show_log_core("gxh_core", "runThreadInterrupt", "execute............", 0);    
   
    bool exOk = false;
  
    //znajdz moduł i funkcje potrzebną do wykonana i przetworzenia odebranej wiadomości..
    for(unsigned int m=0; m<this->modules_count; m++)
    {
       for(unsigned int f=0; f< this->modules[m].module_function_count; f++)
       {
           
           if(this->modules[m].functions[f].const_function_symbol == message.func_number )
           {
               
               if(this->modules[m].ready == 1 && this->modules[m].moduleObject != NULL )
               {                                                         
                       exOk = true;
                       
                       gxh_message response; 
                       bool isResponse = this->modules[m].moduleObject->execute(&message, &response);
                       
                       if(isResponse && response.gxh_handle>0)
                       {
                           this->addResponse( &response );
                       }
                       
                        //break loop
                          f = this->modules[m].module_function_count + 1;
                          m = this->modules_count + 1;
               }else
               {
                       this->show_error("gxh_core", "runThreadInterrupt", "Sorry, module is not ready for use", 6);
               }
               
                //break loop
               f = this->modules[m].module_function_count + 1;
               m = this->modules_count + 1;
           }
       }                                
    }
      
    if(!exOk)
    {
        std::string msg = "GXH cannot find module or function ";
        msg.append(gxh_IntToString(message.func_number ));
        
        
        this->show_error("gxh_core", "runThreadInterrupt", msg.c_str(), 6);                
        
        if(message.gxh_handle > 0) //urzadzenie oczekuje odpowiedzi..
        {
             gxh_message response;   
             response.gxh_handle = message.gxh_handle;
             response.system_code = GXH_MESSAGE_ERROR;
             const char* msg = "GXH cannot find module or function";
             memcpy(response.dataStruct, msg, strlen(msg) );
             response.sizeDataStruct = strlen(msg);
             
             this->addResponse( &response );
        }
        
    }
    
    
    //clear....
    this->gxh_msg_interrupt_buf[index_buf].status = 1; //is free
    memset( &this->gxh_msg_interrupt_buf[index_buf].message,0, sizeof(gxh_message) );
    
    return NULL;
}

//------------------------------------------------------------------------------

void* gxh_core::runThreadSynchronize()
{  
    this->show_log_core("gxh_core", "runThread", "starting  thread for synchronize", 0);
    sleep(6); //usługa się uruchamia po uruchomieniu wszystkich modułów portów itd..
 
    
      while(this->thSynchronize)
      {
        
          while(!this->executeThreadTask)
          {
            this->show_log_core("gxh_core","gxh_core","service wait...", 3);
            sleep(10);           
          }
          
          
          this->show_log_core("gxh_core", "runThread", "DB Synchronize", 0);                    
          this->forceSynchronize();
            
          //synchronize modules..                        
          for(unsigned int m=0;m<this->modules_count;m++) 
          {
            if(this->executeThreadTask == false) break;  
              
            if( this->modules[m].ready == 1 &&    this->modules[m].moduleObject != NULL )
            {
              // this->show_log_core("gxh_core", "runThread", ".. Synchronize module....", 0); 
               this->modules[m].moduleObject->forceSynchronize();
            }
          }
                    
          usleep(1000 * 1000 * 120); //120 sek          
      }
    
    
    
    this->show_log_core("gxh_core", "runThread", "stop  thread for synchronize", 0);
    return NULL;
}

//------------------------------------------------------------------------------

void* gxh_core::runThreadService()
{  
   
    
    this->show_log_core("gxh_core", "runThread", "starting  thread for services", 0);
    sleep(3); //usługa się uruchamia po uruchomieniu wszystkich modułów portów itd..

    
    //-----START UP---------------------------------------------
    //usługi uruchamiana na stercie programu / komputera..
    
      //service function...
        for(unsigned int m=0;m<this->modules_count;m++) 
        {
             if( this->modules[m].ready == 1 &&    this->modules[m].moduleObject != NULL )
             {
                 for(unsigned int func=0; func<this->modules[m].module_function_count; func++  )
                 {                                                            
                           if( this->modules[m].functions[func].run_ms_delay  == -1  )// na starcie..
                            {                                                              
                               gxh_message inMsg;
                               inMsg.func_number = this->modules[m].functions[func].const_function_symbol;
                               
                               gxh_message message_ret;                               
                               this->modules[m].moduleObject->execute(&inMsg,&message_ret) ;                         
                            }
                     
                 }
             }
        }
    
    //-------START UP END--------------------------------
    
    
    unsigned long long int lastTimer = 1;

    while(this->thService)
    {
        
        while(!this->executeThreadTask)
        {
            this->show_log_core("gxh_core","gxh_core","service wait...", 3);
            sleep(10);           
        }
        
        //service function...
        for(unsigned int m=0;m<this->modules_count;m++) 
        {
             if(this->executeThreadTask == false) break;
             
             if( this->modules[m].ready == 1 &&    this->modules[m].moduleObject != NULL )
             {
                 for(unsigned int func=0; func<this->modules[m].module_function_count; func++  )
                 {
                     if(this->executeThreadTask == false) break;
                     
                     if( this->modules[m].functions[func].run_ms_delay >0 && this->modules[m].functions[func].run_as_service == 1 )
                     {
                           unsigned long timeNow = gxh_getSecondsFrom1900();
                           int delay = ((int)(this->modules[m].functions[func].run_ms_delay/1000));
                    
                           if(this->modules[m].functions[func].run_ms_last < (timeNow- delay )  )
                            {                               
                               //------
                                 // dodać funkcje podczas urucominiea i na zakonczenie watku..
                               gxh_message inMsg;
                               inMsg.func_number = this->modules[m].functions[func].const_function_symbol;
                               
                               gxh_message message_ret;
                               
                                this->modules[m].moduleObject->execute(&inMsg,&message_ret) ; 
                               //------
                               this->modules[m].functions[func].run_ms_last = timeNow; 
                            }
                     }
                 }
             }
        }
         
       
       // gxh_message msg;
       // msg.func_number = 100; //włącznik światła
       // this->sendGXHMessage( &msg);                   
        
      /*
        struct timeval  tv;
        gettimeofday(&tv, NULL);  
        unsigned long long int time_in_mill  =  (tv.tv_sec) * 1000 ;
        time_in_mill +=  (tv.tv_usec) / 1000 ;
        
        unsigned long long int diff = time_in_mill;
        unsigned long long int rtime =  SYNCHRONIZE_TIME; //1 sek...
        diff -= rtime;
        
        if( (lastTimer < diff ) && lastTimer>0   )   
        {  
          //synchronize local data.. driver values itd..
          this->forceSynchronize();
            
          //synchronize modules..                        
          for(unsigned int m=0;m<this->modules_count;m++) 
          {
            if(this->executeThreadTask == false) break;  
              
            if( this->modules[m].ready == 1 &&    this->modules[m].moduleObject != NULL )
            {
               this->modules[m].moduleObject->forceSynchronize();
            }
          }                  
           lastTimer = time_in_mill;   
        }                     
        */
        
        
        
        usleep(1000 * 1000); //1 sek                
    }
    
    
    //-----TURN OFF---------------------------------------------
    //usługi uruchamiana na zakonczenie programu / komputera..
    
      //service function...
        for(unsigned int m=0;m<this->modules_count;m++) 
        {
             if( this->modules[m].ready == 1 &&    this->modules[m].moduleObject != NULL )
             {
                 for(unsigned int func=0; func<this->modules[m].module_function_count; func++  )
                 {                                                            
                           if( this->modules[m].functions[func].run_ms_delay  == -2  )// na zakonczenie
                            {                                                              
                               gxh_message inMsg;
                               inMsg.func_number = this->modules[m].functions[func].const_function_symbol;
                               
                               gxh_message message_ret;                               
                               this->modules[m].moduleObject->execute(&inMsg,&message_ret) ;                         
                            }
                     
                 }
             }
        }
    
    //-------TURN OFF END--------------------------------
    
    
    
    this->show_log_core("gxh_core", "runThread", "stop  thread for services", 0);  
    
    return NULL;
}

//------------------------------------------------------------------------------

void gxh_core::addHistory(int const_function_symbol, int resource_id, char* name, char* text_content, int px_group_id)
{
     bool added = false;
    
     for(int i=0; i<MAX_HISTORY_BUFFER;i++)
       {         
          if( this->histories[i].free )
          {                               
              
              this->histories[i].free = false;
              this->histories[i].toSave = false;
              
              this->histories[i].const_function_symbol = const_function_symbol;
              this->histories[i].resource_id = resource_id;
              
              
              memset(this->histories[i].name,0, 500);
              if( strlen(name) < 500)memcpy(this->histories[i].name,name, strlen(name));
                            
              memset(this->histories[i].text_content,0, 1000);
              if( strlen(text_content) < 1000) memcpy(this->histories[i].text_content,text_content, strlen(text_content));
              
              this->histories[i].px_group_id = px_group_id;
              
              this->histories[i].toSave = true;
              
              added = true;
              break;
          }
       }
     
     if(!added) this->show_error("gxh_core","addHistory","Cannot add to history",4);
}

//------------------------------------------------------------------------------

void  gxh_core::synchronizeHistory(  )
{
    
    //jesli jest niski poziom debugowania nie zapisujemy histori klikniecia portu..
    if(this->getDebugLevel() < 7 )
    {           
            for(int i=0; i<MAX_HISTORY_BUFFER;i++)
                  {         
                     if( !this->histories[i].free && this->histories[i].toSave )
                     { 
                           this->histories[i].free = true;
                           this->histories[i].toSave = false; 
                     }
                  }
        
        return;
    }
    
   
 for(int i=0; i<MAX_HISTORY_BUFFER;i++)
       {         
          if( !this->histories[i].free && this->histories[i].toSave )
          {                                         
                      sqlite3* dbSqLite2;                    
    
                      std::string baza = this->getDBTmp();//this->getRunPath();
                      //baza.append("../gxh_base_tmp.db");
     
                      int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite2, SQLITE_OPEN_READWRITE, NULL   );                                  
                      if( r != SQLITE_OK )
                       {
                         this->show_error("gxh_core","exeToHistory","Cannot open sqlLite 2",0);                      
                       }else
                       {
                        std::string query="";
                        query.append("INSERT INTO ih_history (app_www,const_function_symbol, resource_id,name,text_content,px_group_id, created_at, updated_at, is_delete ) VALUES ('");               
                        query.append("GXG"); 
                        query.append("','");    
                        query.append(gxh_IntToString( this->histories[i].const_function_symbol ));    
                        query.append("','");    
                        query.append( gxh_IntToString( this->histories[i].resource_id ) );  
                        query.append("','");    
                        query.append( this->histories[i].name ); 
                        query.append("','");    
                        query.append( this->histories[i].text_content );
                        query.append("','");    
                        query.append( gxh_IntToString( this->histories[i].px_group_id ) );  
                        query.append("', DATETIME('now','localtime') ,DATETIME('now','localtime') ,'0' );");       
                        
                        this->show_log_data_base("gxh_core","synchronizeHistory", query.c_str(),5);
                                    
                         char *zErrMsg = 0;     
                         int rc = sqlite3_exec( dbSqLite2 , query.c_str(), 0, 0, &zErrMsg);
                         if( rc == SQLITE_OK )
                         {  
                             this->histories[i].free = true;
                             this->histories[i].toSave = false;
                         }else
                         {
                              std::cout<<query.c_str()<<"\n";          
                              this->show_error("gxh_core","synchronizeHistory", (const char*)zErrMsg,0);
                              sqlite3_free(zErrMsg);                                                          
                         }
                         sqlite3_close_v2(dbSqLite2);  
        
                       }                                    
          }
    }
 
 
 //wyczyść historie jeśli jest za dużo rekordów..
 
 
                      sqlite3* dbSqLite2;                    
    
                      std::string baza = this->getDBTmp();//this->getRunPath();
                      //baza.append("../gxh_base_tmp.db");
     
                      int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite2, SQLITE_OPEN_READWRITE, NULL   );                                  
                      if( r != SQLITE_OK )
                      {
                         this->show_error("gxh_core","synchronizeHistory","Cannot open sqlLite 1",0);                      
                      }else
                      {
                          sqlite3_stmt* stmt; 
                          int lastId = -1;
                          std::string query="select id from ih_history ORDER BY id DESC LIMIT 1;";                            
                         
                          const char* queryChar = query.c_str();       
                          if(sqlite3_prepare(dbSqLite2 , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
                           {
                             this->show_error("gxh_core","synchronizeHistory", sqlite3_errstr( sqlite3_errcode(dbSqLite2  ) ),4);                                        
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
                            
                            
                            if(lastId > 100)
                            {
                                std::string query = "DELETE FROM ih_history;";
                                
                                char *zErrMsg = 0;     
                                int rc = sqlite3_exec( dbSqLite2 , query.c_str(), 0, 0, &zErrMsg);
                                if( rc != SQLITE_OK )
                                   {  
                                      this->show_error("gxh_core","synchronizeHistory", (const char*)zErrMsg ,0);   
                                       sqlite3_free(zErrMsg);    
                                   } 
                            }
                            
                          sqlite3_close_v2(dbSqLite2);                          
                       }
 
}

//------------------------------------------------------------------------------

 void gxh_core::forceSynchronize()
 {                 
     
     
     
       this->show_log_core("gxh_core", "forceSynchronize", "checking.....", 10);
       
       //przepisz zmienne systemowe do czyjników analogowych..
       unsigned int sSize = 0;
       ih_sensor* firstS = (ih_sensor*) this->getResource(RES_VECTOR_SENSOR, &sSize);
      
       
       for(unsigned int x=0; x<this->driver_value_count; x++)                
       {
         if(this->driver_values[x].status == 1 ) //aktualizuj dane...
         {
             for(unsigned int y=0; y<sSize; y++)
             {
               if( firstS[y].ih_driver_values_id >0 && firstS[y].ih_driver_values_id == this->driver_values[x].id && firstS[y].analogowy>0 )
               {
                   if(strlen(this->driver_values[x].value) < 255)
                   {
                     memcpy(firstS[y].wartosc,this->driver_values[x].value, strlen(this->driver_values[x].value) );
                     firstS[y].toSave = true;
                   }
               }
             }
         }
       }
       
 
      
       for(unsigned int x=0; x<this->driver_value_count; x++)                
       {
           
           
           if(this->driver_values[x].free==false && this->driver_values[x].status == 1 ) //zaktualizuj dane
           {    
                 sqlite3* dbSqLite1;        
                 
            //     std::cout<<"\n\nAktualizuj zmienna systemowa: "<<this->driver_values[x].name_param<<", "<<this->driver_values[x].value<<"\n";
    
                 std::string baza = this->getDB(); //this->getRunPath();
                 //baza.append("../gxh_base.db");
     
                 int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite1, SQLITE_OPEN_READWRITE, NULL   );      
                 if( r != SQLITE_OK )
                 { 
                    this->show_error("gxh_core","forceSynchronize","Cannot open sqlLite 1",0);                      
                 }else //sprawdzić czy jest jakiś sensor powiązany z zmienna systemową..
                 {  
                     
                     std::string query = "UPDATE ih_driver_values SET value='";  
                     query.append( this->driver_values[x].value  );   
                     query.append("' WHERE const_function='");     
                     query.append(gxh_IntToString(this->driver_values[x].const_function));    
                     query.append("' AND ih_driver_id='");   
                     query.append(gxh_IntToString(this->driver_values[x].ih_driver_id));    
                     query.append("' AND px_group_id='");  
                     query.append(gxh_IntToString(this->driver_values[x].px_group_id));    
                     query.append("'; "); 
                     
                     //std::cout<<"\nAktualizuj..."<<this->driver_values[x].value<<"\n\n";
                     
                   //  std::cout<<query.c_str()<<"\n\n";
                     
                     char *zErrMsg = 0;     
                     int rc = sqlite3_exec( dbSqLite1 , query.c_str(), 0, 0, &zErrMsg);
                     if( rc == SQLITE_OK )
                     {  
                          this->driver_values[x].status = 0;  ///gotowe, zaktualizwane
                     }else
                     {
                          sqlite3_free(zErrMsg);    
                     }
                     sqlite3_close_v2(dbSqLite1);    
                     
                    this->show_log_core("gxh_core", "forceSynchronize", this->driver_values[x].name_param, 5);
                 }
                                   
                 if(this->driver_values[x].toHistory ) //dodaj do historii..
                 {
                      sqlite3* dbSqLite2;                    
    
                      std::string baza = this->getDBTmp(); //this->getRunPath();
                      //baza.append("../gxh_base_tmp.db");
     
                      int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite2, SQLITE_OPEN_READWRITE, NULL   );                                  
                      if( r != SQLITE_OK )
                       {
                         this->show_error("gxh_core","forceSynchronize","Cannot open sqlLite 1",0);                      
                       }else
                       {
                        std::string query="";
                        query.append("INSERT INTO ih_driver_values_history (ih_driver_values_id, value, created_at, updated_at,is_delete) VALUES ('");
                        query.append(   gxh_IntToString(this->driver_values[x].id) );
                        query.append("','"); 
                        query.append(  this->driver_values[x].value  );    
                        query.append("', DATETIME('now','localtime') ,DATETIME('now','localtime'),'0'  );");               
                                    
                         char *zErrMsg = 0;     
                         int rc = sqlite3_exec( dbSqLite2 , query.c_str(), 0, 0, &zErrMsg);
                         if( rc == SQLITE_OK )
                         {  
                            this->driver_values[x].toHistory = false;                                                       
                            
                         } else
                         {
                               sqlite3_free(zErrMsg);    
                         }
                         sqlite3_close_v2(dbSqLite2);  
                         
                        this->show_log_core("gxh_core", "forceSynchronize", this->driver_values[x].name_param, 5);
                       }                                                     
                 }                            
           }
           
           
           if(this->driver_values[x].free==false && this->driver_values[x].status == 2 ) //dodaj do bazy
           {  
                 sqlite3* dbSqLite1;           
                 
                // std::cout<<"\n\nSynchronizacja zmiennej....";
    
                 std::string baza =  this->getDB(); //this->getRunPath();
                 //baza.append("../gxh_base.db");
     
                 int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite1, SQLITE_OPEN_READWRITE, NULL   );                                  
                 if( r != SQLITE_OK )
                 {
                    this->show_error("gxh_core","forceSynchronize","Cannot open sqlLite 1",0);                      
                 }else //sprawdzić czy jest jakiś sensor powiązany z zmienna systemową..
                 {
                     
                     std::string query = "INSERT INTO ih_driver_values (id, ih_driver_id, name_param, value, const_function, px_group_id, created_at,updated_at, is_delete) VALUES (null,'";
                     query.append( gxh_IntToString(this->driver_values[x].ih_driver_id)  );
                     query.append( "','"  );
                     query.append( this->driver_values[x].name_param );
                     query.append("','" );
                     query.append( this->driver_values[x].value  );
                     query.append( "','"  );
                     query.append( gxh_IntToString(this->driver_values[x].const_function)   );
                     query.append( "','"   );
                     query.append( gxh_IntToString(this->driver_values[x].px_group_id)   );
                     query.append("', DATETIME('now','localtime') ,DATETIME('now','localtime'),'0'  );");
           
                    // std::cout<<"\n\nZapytanie "<<query.c_str();
                                    
                     char *zErrMsg = 0;     
                     int rc = sqlite3_exec( dbSqLite1 , query.c_str(), 0, 0, &zErrMsg);
                     if( rc == SQLITE_OK )
                     {  
                          this->driver_values[x].status = 0;  ///gotowe, zaktualizwane
                     }else
                     {
                         sqlite3_free(zErrMsg);    
                     }
                     sqlite3_close_v2(dbSqLite1);    
                     
                    this->show_log_core("gxh_core", "forceSynchronize", this->driver_values[x].name_param, 5);
                 }
                                                                             
           }
       }
 

                      //wyczyść historie zmiennych systemowych.....
                      sqlite3* dbSqLite2;                    
    
                      std::string baza = this->getDBTmp();  //this->getRunPath();
                      //baza.append("../gxh_base_tmp.db");
     
                      int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite2, SQLITE_OPEN_READWRITE, NULL   );                                  
                      if( r != SQLITE_OK )
                      {
                         this->show_error("gxh_core","forceSynchronize","Cannot open sqlLite 1",0);                      
                      }else
                      {
                          sqlite3_stmt* stmt; 
                          int lastId = -1;
                          std::string query="select * from ih_driver_values_history ORDER BY id DESC LIMIT 1;";                            
                         
                          const char* queryChar = query.c_str();       
                          if(sqlite3_prepare(dbSqLite2 , queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK)
                           {
                             this->show_error("gxh_core","forceSynchronize", sqlite3_errstr( sqlite3_errcode(dbSqLite2  ) ),4);                                        
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
                            
                            
                            if(lastId > 1000)
                            {
                                std::string query = "DELETE FROM ih_driver_values_history;";
                                
                                char *zErrMsg = 0;     
                                int rc = sqlite3_exec( dbSqLite2 , query.c_str(), 0, 0, &zErrMsg);
                                if( rc != SQLITE_OK )
                                   {  
                                      this->show_error("gxh_core","forceSynchronize", (const char*)zErrMsg ,0);   
                                       sqlite3_free(zErrMsg);    
                                   } 
                            }
                            
                          sqlite3_close_v2(dbSqLite2);                          
                       }
                      
                     
                      
     this->synchronizeHistory();                      
     
 
     
 }  

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
 
//------------------------------------------------------------------------------
/**
 * Sprawdz czy dla tej wersji aplikacji śa dodane wszystkie niezbędne moduły....
 * // sprawdź zmienne systemowe i inne niezbędne rzeczy..
 */
void gxh_core::checkVersionGXH() 
{
    
    
}
 
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------