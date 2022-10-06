

#include <sstream>
#include <iostream>
#include <cstring>
#include <vector>
#include <sys/time.h>
#include <dlfcn.h> 
#include <stdlib.h>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <algorithm>

//dla sieci

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/ioctl.h>


#include <fcntl.h>    /* For O_RDWR */
#include <unistd.h> 

#ifndef GXH_STRUCTURE_H
#define	GXH_STRUCTURE_H

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

/**
 *  Ustawia nowy adres urządzenia
 *  paramArray[0]  - addres '0'
 *  paramArray[1]  - addres '1'
 *  paramInt[0]    - ih_driver_id
 *  status w system_response: IH_NEW_ADDRESS_OK IH_NEW_ADDRESS_ERROR
 */


//
#define MAX_MODULE_FUNCTION 400
#define MAX_MODULE_BUFFER 200
#define MAX_FUNCTION_BUFFER 1000
#define MAX_PORT_FUNCTION_BUFFER 4000
#define MAX_RESOURCE_FUNCTION_BUFFER 500
#define MAX_GXH_MESSAGE_INTERRUPT 10
#define MAX_GXH_HANDLE_LIST 400
#define NULL_DRIVER 1000
#define MAX_COMPUTER_PORT 10
#define MAX_DRIVERS 200
#define MAX_PORT_IN_DRIVER 25
#define MAX_SLDRIVER 40
#define MAX_DRIVER_API 1000
#define MAX_DRIVER_IMAGE 150
#define MAX_DRIVER_VALUE 500
#define MAX_NOTICE_CACHE 50
#define MAX_SYMBOL_HASH_BUFFER 600
#define MAX_HISTORY_BUFFER 50
 

#define MAX_CLIENT_TCP 200
#define CONSOLE_PORT 7777

#define PORT_DOUBLE_CLICK_MS 3000
#define PORT_LONG_CLICK_MS 2000

#define SYNCHRONIZE_TIME 1*1000        /// co 1 sek klikaj suługi..

#define DELAY_PORT_SERIAL 5  //5ms




//-----------------

#define RES_VECTOR_FROM 50
#define RES_VECTOR_TO 200



#define RES_VECTOR_SECTOR 1   
#define RES_VECTOR_ZONE 2     
#define RES_VECTOR_USER 3   
#define RES_VECTOR_NOTICE 4

#define RES_VECTOR_FUNC 5
#define RES_VECTOR_SYMBOL_HASH 6

#define RES_VECTOR_HOME 7

#define RES_VECTOR_LIGHT 10
#define RES_VECTOR_BLIND 11
#define RES_VECTOR_GATE 12
#define RES_VECTOR_DRIVER 13
#define RES_VECTOR_DRIVER_PORT 14
#define RES_VECTOR_DRIVER_SL 15
#define RES_VECTOR_SENSOR 16
#define RES_VECTOR_SCENE 17
#define RES_VECTOR_SCENE_ACTION 18
#define RES_VECTOR_TASK 19
#define RES_VECTOR_DRIVER_API 20
#define RES_VECTOR_DRIVER_IMAGE 21
#define RES_VECTOR_DRIVER_VALUE 22
#define RES_VECTOR_OUTS 23
#define RES_VECTOR_HISTORY 24
#define RES_VECTOR_CAMERA 25
#define RES_VECTOR_ADV 26   
#define RES_VECTOR_DOOR 27
#define RES_VECTOR_INTERRUPT 28
#define RES_VECTOR_CARDS 29
#define RES_VECTOR_CARD_DRIVER 30
#define RES_VECTOR_ALARM_ZONE 31
#define RES_VECTOR_ALARM_DEVICE 32
#define RES_VECTOR_TIMER 33



//-----------------
//-----------------
//vars of system..

#define VAR_INTERNET_STATUS 10
#define VAR_CZAS_LETNI_ZIMOWY 11   //Z L
#define VAR_CZAS_ZACHODU_SLONCA 12   //16:02
#define VAR_CZAS_WSCHODU_SLONCA 13   //06:02
#define VAR_TEMP_W_CIENIU 14   //12
#define VAR_MOZLIWOSC_OPADOW 15   //  W PROCENTACH
#define VAR_WILGOTNOSC_POWIETRZA 16   //  W PROCENTACH

#define VAR_VERSION_NOW 30   //  aktualna wresja oprogramowania
#define VAR_VERSION_NEW 31   //dostępna wersja oprogramowania..
#define VAR_VERSION_ISNEW 32   //dostępna nowa wersja oprogramowania

#define VAR_LIGHT_ON_IN_HOME 50   //  gdzieś w domu jest zapalone światło, minimum jedno..


#define VAR_SYS_USER_GPS 100  //51.23:12.34 LAT:LONG
#define VAR_SYS_USER_WIFI 101  //nazwa ssid aktywnego połączenie wifi
#define VAR_SYS_USER_BLUETOOTH 102  //nazwa połączonego urządzenie bluetooth



//-----------------
//-----------------

#define IH_SET_NEW_ADDRESS     10000  
#define IH_NEW_ADDRESS_OK      10001
#define IH_NEW_ADDRESS_ERROR   10002
#define IH_UPDATE_ADDRESS      10003  
#define IH_RESULT_NOT_FOUND    10004 
#define IH_RESULT_OK           10006 
#define IH_RESULT_ERROR        10007 
#define IH_RESULT_MODULE_NOT_FOUND  10008 
#define IH_RESULT_SHARED_LIBRARY_ERROR  10009 
#define IH_UPDATE_SPEED  10010 //
#define IH_UPDATE_DATA_BIT  10011 //
#define IH_UPDATE_STOP_BIT  10012 //
#define IH_UPDATE_PARITY  10013 //

//stany pojedynczych portow...
#define IH_SET_PORT_ON              11000
#define IH_SET_PORT_OFF             11001
#define IH_SET_PORT_CLICK           11002
#define IH_SET_PORT_DOUBLE_CLICK    11003
#define IH_SET_PORTS_STATUS         11004 //ustawia jeden z ponizszych statusow dla wielu portow..
 



// FOR CORE

#define IH_RELOAD_MODULE       100
#define IH_RELOAD_DRIVER       101
#define IH_SERVICE_STOP        103
#define IH_SERVICE_START       104
#define IH_PORT_STOP           105
#define IH_PORT_START          106
#define IH_SHOW_TASK           107
#define IH_SET_LEVEL           108
#define IH_RELOAD_FUNCTION     109


#define IH_SWITCH_ON_CLICK     12001
#define IH_SWITCH_OFF_CLICK    12002
#define IH_SWITCH_DOUBLE_CLICK 12003
#define IH_SWITCH_LONG_CLICK   12004


#define IH_SWITCH_BISTABILE_ON_NO     '0'
#define IH_SWITCH_BISTABILE_OFF_NO    '1'
#define IH_SWITCH_BISTABILE_ON_NC     'a'
#define IH_SWITCH_BISTABILE_OFF_NC    'b'

#define  IH_SWITCH_MONOSTABILE_ON_OFF_CLICK_NO  '2'
#define  IH_SWITCH_MONOSTABILE_ON_OFF_DOUBLE_CLICK_NO  '3'
#define  IH_SWITCH_MONOSTABILE_ON_OFF_LONG_CLICK_NO  '4'

#define  IH_SWITCH_MONOSTABILE_ON_OFF_CLICK_NC  'e'
#define  IH_SWITCH_MONOSTABILE_ON_OFF_DOUBLE_CLICK_NC  'f'
#define  IH_SWITCH_MONOSTABILE_ON_OFF_LONG_CLICK_NC  'g'

#define  IH_PORT_BISTABILE_OUT_ON   '5'
#define  IH_PORT_BISTABILE_OUT_OFF  '6'
#define  IH_PORT_MONOSTABILE_ON_OFF_CLICK  '7'
#define  IH_PORT_MONOSTABILE_ON_OFF_DOUBLE_CLICK  '8'

 
 
/**
 * Włącznik światła, standardowe włącz wyłącz światło
 */
#define IH_MODULE_FUNCTION_LIGHT_SWITCH    100
/**
 * Port wyjścia oświetlenia
 */
#define  IH_MODULE_FUNCTION_LIGHT_OUT      101


/**
 * Wymuś wyłączenie oswietlenia
 */
#define  IH_MODULE_FUNCTION_LIGHT_ON       103
/**
 * Wymuś włączenie oświetlenia
 */
#define  IH_MODULE_FUNCTION_LIGHT_OFF      102


/**
 * Zapal wszystkie światła w domu
 */
#define  IH_MODULE_FUNCTION_LIGHT_ON_HOME  104

/**
 * Zgaś wszystkie światła w domu
 */
#define  IH_MODULE_FUNCTION_LIGHT_OFF_HOME  105


/**
 * Zapal wszystkie światła w sektorze (piętrze)
 */
#define  IH_MODULE_FUNCTION_LIGHT_ON_SECTOR  106

/**
 * Zgaś wszystkie światła w sektorze (piętrze)
 */
#define  IH_MODULE_FUNCTION_LIGHT_OFF_SECTOR  107

/**
 * Zapal wszystkie światła w strefie (pomieszczeniu)
 */
#define  IH_MODULE_FUNCTION_LIGHT_ON_ZONE  108

/**
 * Zgaś wszystkie światła w strefie (pomieszczeniu)
 */
#define  IH_MODULE_FUNCTION_LIGHT_OFF_ZONE  109


#define  IH_MODULE_FUNCTION_LIGHT_INTERRUPT_ON  110
#define  IH_MODULE_FUNCTION_LIGHT_INTERRUPT_OFF  111



/**
 * Włącznik rolety - góra
 */
#define  IH_MODULE_FUNCTION_BLIND_SWITCH_UP  120
/**
 * Włacznk rolety - dół
 */
#define  IH_MODULE_FUNCTION_BLIND_SWITCH_DOWN  121

/**
 * Wyjście zamknięcia rolety
 */
#define  IH_MODULE_FUNCTION_BLIND_CLOSE  122

/**
 * Wyjście otwarcia rolety
 */
#define  IH_MODULE_FUNCTION_BLIND_OPEN  123


/**
 * Potwierdznie otwarcia rolety - 100%
 */
#define  IH_MODULE_FUNCTION_BLIND_OPEN_OK  124

/**
 * Potwierdznie zamkniecia rolety
 */
#define  IH_MODULE_FUNCTION_BLIND_CLOSE_OK  125




/**
 * Otwórz rolety w budynku
 */
#define  IH_MODULE_FUNCTION_BLIND_100_HOME  126

/**
 * Zamknij rolety w budynku
 */
#define  IH_MODULE_FUNCTION_BLIND_0_HOME  127


/**
 * Otwórz wszystkie rolety w sektorze (piętrze)
 */
#define  IH_MODULE_FUNCTION_BLIND_100_SECTOR  128

/**
 * Zamknij wszystkie rolety w sektorze (piętrze)
 */
#define  IH_MODULE_FUNCTION_BLIND_0_SECTOR  129

/**
 * Otwórz wszystkie rolety w strefie (pomieszczeniu)
 */
#define  IH_MODULE_FUNCTION_BLIND_100_ZONE  130

/**
 * Zamknij wszystkie rolety w strefie (pomieszczeniu)
 */
#define  IH_MODULE_FUNCTION_BLIND_0_ZONE  131


#define  IH_MODULE_FUNCTION_TIMER_INTERRUPT_MAX  361
 


/**
 * Przycisk otwarcia bramy
 */
#define  IH_MODULE_FUNCTION_GATE_CLICK  140


/**
 * Port otwarcia bramy
 */
#define  IH_MODULE_FUNCTION_GATE_OPEN_PORT  141

/**
 * Potwierdzenie otwarcia bramy
 */
#define  IH_MODULE_FUNCTION_GATE_OPEN_OK  142

/**
 * Potwierdznie zamkniecia bramy
 */
#define  IH_MODULE_FUNCTION_GATE_CLOSE_OK  143

/**
 * Wyjscie otwarcia bramy
 */
#define  IH_MODULE_FUNCTION_GATE_OPEN_TWO_PORT  144
/**
 * Wyjście zamkniecia brmy
 */
#define  IH_MODULE_FUNCTION_GATE_CLOSE_TWO_PORT  145
/**
 * Wymus otwarcie bramy
 */
#define  IH_MODULE_FUNCTION_GATE_FORCE_OPEN  146

/**
 */
#define  IH_MODULE_FUNCTION_GATE_FORCE_CLOSE  147

#define  IH_MODULE_FUNCTION_GATE_OPEN_INTTERUPT  148
#define  IH_MODULE_FUNCTION_GATE_CLOSE_INTTERUPT  149


#define  IH_MODULE_FUNCTION_DOOR_OPEN  151
#define  IH_MODULE_FUNCTION_DOOR_OUT_CLICK  152 // Wyjście otwarcia drzwi (click)
#define  IH_MODULE_FUNCTION_DOOR_OUT_1sek  153 //  Wyjście otwarcia drzwi (ON 1 sek
#define  IH_MODULE_FUNCTION_DOOR_OUT_5sek  154 // Wyjście otwarcia drzwi (ON 5 sek) 
#define  IH_MODULE_FUNCTION_DOOR_OUT_10sek  155 // Wyjście otwarcia drzwi (ON 10 sek) 
#define  IH_MODULE_FUNCTION_DOOR_OPENED  156 // Potwierdzenie otwarcia drzwi
#define  IH_MODULE_FUNCTION_DOOR_CLOSED  157 // Potwierdzenie zamkniecia.

#define  IH_MODULE_FUNCTION_DOOR_ON_P  158 // Otwarto Drzwi - konektor
#define  IH_MODULE_FUNCTION_DOOR_OFF_P  159 // Zamknieto Drzwi - konektor
 

 


/**
 * Czujnik typu ON/off
 */
#define  IH_MODULE_FUNCTION_STANDARD_SENSOR  180



#define  IH_MODULE_FUNCTION_ACTIVE_SILENT_ALARM_ZONE  209
#define  IH_MODULE_FUNCTION_ACTIVE_ALARM_ZONE  210
#define  IH_MODULE_FUNCTION_DEZACTIVE_ALARM_ZONE  211
#define  IH_MODULE_FUNCTION_ACTIVE_ALARM_ALLZONE  212
#define  IH_MODULE_FUNCTION_DEZACTIVE_ALARM_ALLZONE  213
#define  IH_MODULE_FUNCTION_ALARM_ALERT_ZONE  214

#define  IH_MODULE_FUNCTION_ALARM_OUT_ACTIVE  215 //3 sec
#define  IH_MODULE_FUNCTION_ALARM_OUT_DEZACTIVE  216 //3sec
#define  IH_MODULE_FUNCTION_ALARM_ALERT_ZONE_P  217
#define  IH_MODULE_FUNCTION_ALARM_ALERT_ZONE_PSEC  232
#define  IH_MODULE_FUNCTION_ALARM_ALERT_SENSOR  218


#define  IH_MODULE_FUNCTION_ALARM_ACTIVE  226
#define  IH_MODULE_FUNCTION_ALARM_DEZACTIVE  227

#define  IH_MODULE_FUNCTION_ALARM_SPEAKER_OUT  223

#define  IH_MODULE_FUNCTION_ALARM_ON_SPEAKER  224
#define  IH_MODULE_FUNCTION_ALARM_ON  225
#define  IH_MODULE_FUNCTION_ALARM_OFF_SPEAKER  230

#define  IH_MODULE_FUNCTION_ALARM_MOVE_SENSOR  220
#define  IH_MODULE_FUNCTION_ALARM_MOVE_SMOKE  221
#define  IH_MODULE_FUNCTION_ALARM_MOVE_GAS  222

#define  IH_MODULE_FUNCTION_ALARM_SMOKE_P  228
#define  IH_MODULE_FUNCTION_ALARM_GAS_P  229

#define  IH_MODULE_FUNCTION_ALARM_N_SMOKE_Z  233
#define  IH_MODULE_FUNCTION_ALARM_N_SMOKE_ZSEC  234

#define  IH_MODULE_FUNCTION_ALARM_N_GAS_Z  235
#define  IH_MODULE_FUNCTION_ALARM_N_GAS_ZSEC  236

#define  IH_MODULE_FUNCTION_ALARM_N_SHORT  237

#define  IH_MODULE_FUNCTION_ALARM_NOBJ_SHORT  238
#define  IH_MODULE_FUNCTION_ALARM_NOBJ  239



#define  IH_MODULE_FUNCTION_RUN_SCENE  240
#define  IH_MODULE_FUNCTION_RUN_SCENE_TIME  241


#define  IH_MODULE_FUNCTION_ETH_SMS_ERR_SYS  304


#define  IH_MODULE_FUNCTION_OUTS_ON 320 //wymus włącznie

#define  IH_MODULE_FUNCTION_OUTS_OFF 321 //wymus wyłącznie

#define  IH_MODULE_FUNCTION_OUTS_CHANGE 322 //zmień stan na przeciwny

#define  IH_MODULE_FUNCTION_OUTS_OUT 323 //wyjście gniazdka


#define  IH_MODULE_FUNCTION_TIMER_ZERO 360 //wyzeruj timer
#define  IH_MODULE_FUNCTION_TIMER_UP 362 //odśwież - zwiększ wartość

#define  IH_MODULE_FUNCTION_TIMER_START 374 //rozpocznij liczenie + wyzeruj
#define  IH_MODULE_FUNCTION_TIMER_STOP 375 //zatrzymaj liczenie + wyzeruj..


/**
 * Zakończ aplikacje
 */
#define  IH_MODULE_FUNCTION_CLOSE  0


#define  IH_MODULE_FUNCTION_LIGHT_BACKUP  9 // przywroc pierwotny stan oświetlenia po przywrócenia zasilania.

/**
 * Restart komputera
 */
#define  IH_MODULE_FUNCTION_RESTART  10


/**
 * Restart aplikacji
 */
#define  IH_MODULE_FUNCTION_RESTART_APP  11


/**
 * Wykonaj kopie zapasowa danych
 */
#define  IH_MODULE_FUNCTION_COPY_BASE  12

/**
 * Załaduj ostatnio kopie danych
 */
#define  IH_MODULE_FUNCTION_COPY_LOAD  13


/**
 * Sprawdz aktualizacje
 */
#define  IH_MODULE_FUNCTION_CHECK_VER  14


/**
 * Wykonaj aktualizacje systemu
 */
#define  IH_MODULE_FUNCTION_GO_UPDATE  15

/**
 * Sprawdz zaawansowane funkcje zdarzeń
 */
#define  IH_MODULE_FUNCTION_ADV_TEST  16

/**
 * Funkcja sprzątająca system, stare logi itd..
 */
#define  IH_MODULE_FUNCTION_CLEAN_SYSTEM  17


#define  IH_MODULE_FUNCTION_PORT_GO_ON  18
#define  IH_MODULE_FUNCTION_PORT_GO_OFF  19
#define  IH_MODULE_FUNCTION_PORT_GO_CLICK  20
#define  IH_MODULE_FUNCTION_CHECK_INTERNET_STATUS  21
#define  IH_MODULE_FUNCTION_NOTICE_QUERY  22
#define  IH_MODULE_FUNCTION_NOTICE_UPDATE  23 // powiadomienie o nowej wersji..
#define  IH_MODULE_FUNCTION_NOTICE_MSG  24 // zwykłe powiadomienie..

#define  IH_MODULE_FUNCTION_PORT_GO_ON_1sek  30 // ustaw stan na porcie wyjscia wysoko..
#define  IH_MODULE_FUNCTION_PORT_GO_ON_2sek  31 // ustaw stan na porcie wyjscia wysoko..
#define  IH_MODULE_FUNCTION_PORT_GO_ON_3sek  32 // ustaw stan na porcie wyjscia wysoko..
#define  IH_MODULE_FUNCTION_PORT_GO_ON_4sek  33 // ustaw stan na porcie wyjscia wysoko..
#define  IH_MODULE_FUNCTION_PORT_GO_ON_5sek 34 // ustaw stan na porcie wyjscia wysoko.. 
#define  IH_MODULE_FUNCTION_PORT_GO_ON_6sek 35 // ustaw stan na porcie wyjscia wysoko.. 
#define  IH_MODULE_FUNCTION_PORT_GO_ON_7sek 36 // ustaw stan na porcie wyjscia wysoko.. 
#define  IH_MODULE_FUNCTION_PORT_GO_ON_8sek 37 // ustaw stan na porcie wyjscia wysoko.. 
#define  IH_MODULE_FUNCTION_PORT_GO_ON_9sek 38 // ustaw stan na porcie wyjscia wysoko.. 
#define  IH_MODULE_FUNCTION_PORT_GO_ON_10sek 39 // ustaw stan na porcie wyjscia wysoko.. 

#define  IH_MODULE_FUNCTION_PORT_GO_ON_1min 50 // ustaw stan na wysoki 1min.
#define  IH_MODULE_FUNCTION_PORT_GO_ON_2min 51 // ustaw stan na wysoki 1min.
#define  IH_MODULE_FUNCTION_PORT_GO_ON_3min 52 // ustaw stan na wysoki 1min.
#define  IH_MODULE_FUNCTION_PORT_GO_ON_4min 53 // ustaw stan na wysoki 1min.
#define  IH_MODULE_FUNCTION_PORT_GO_ON_5min 54 // ustaw stan na wysoki 1min.
#define  IH_MODULE_FUNCTION_PORT_GO_ON_6min 55 // ustaw stan na wysoki 1min.
#define  IH_MODULE_FUNCTION_PORT_GO_ON_7min 56 // ustaw stan na wysoki 1min.
#define  IH_MODULE_FUNCTION_PORT_GO_ON_8min 57 // ustaw stan na wysoki 1min.
#define  IH_MODULE_FUNCTION_PORT_GO_ON_9min 58 // ustaw stan na wysoki 1min.
#define  IH_MODULE_FUNCTION_PORT_GO_ON_10min 59 // ustaw stan na wysoki 1min.





 
    
    #define GXH_MESSAGE_SUCCESS  0   //
    #define GXH_MESSAGE_ERROR    100
    #define GXH_MESSAGE_TEXT     2   //show code in text...
    #define GXH_MESSAGE_REDIRECT 3   //REDIRECT
    #define GXH_MESSAGE_INTERRUPT 4   //przerwanie wysłane przez zasób..
    
    #define GXH_MAX_DATA_STRUCT_SIZE  (1024 * 200)
  
    struct gxh_message
    {
   
             unsigned int  func_number;
             unsigned short  func_internal_number;            
             //char dataStruct[1024 * 200];//0.5 MB .//w port function to samo
              char dataStruct[1024 * 200];// po optymalizacji 0.5 MB .//w port function to samo
             unsigned int  sizeDataStruct;
             unsigned int  gxh_handle;    
             short int  system_code;
             int  userId;      
             double time_ms;
    
             
             short int check_timeout;  // 10 -go check, 0 no
             unsigned long long int execute_timeout; //usługa nie może się wykonać dłużeni niż ten czas..
             
             bool localhost;
    };
   
    
    
    struct gxh_on_click
    {
       int  func_number;  //module and driver number...
       int ih_driver_id;
       int func_internal_number; //type of click..
       int ih_driver_port_id;
    };
    
    
    struct gxh_interrrupt_res
    {
        unsigned int resource_id;
    };
    
  
 
    
    struct system_driver_command
    {
        int ih_driver_id;
        int ih_module_function;
        char param[1024];
        bool* result;
        bool* response;
        char response_data[255];
    };
    
    //--------------------------------------------------------------------------
  
    struct ih_sl_const_function_symbol
    {
        int id;
        int const_function_symbol;
        char description[255];
        char module[255];
        int run_as_service;
        int run_ms_delay;
        unsigned long last_running;
        char type[255];
        char resourceName[255];
        
        char param1Name[255];
        char param2Name[255];
    };
    
    //--------------------------------------------------------------------------
   
    struct ih_sl_const_function_hash
    {         
        char name[255];
        int symbol;      
    };
    
    //--------------------------------------------------------------------------
  
    struct ih_sl_const_function_param__DEL
    {
        int id;
        std::string resource_name;
        int resource_id;
        std::string extra_param;
        std::string extra_param2;
    };
    
    //--------------------------------------------------------------------------
    
    struct ih_camera
    {
       int id;
       char name[100];
       char localIp[255];
      // char remoteIp[255];
       
       int localPort;
       int remotePort;
       
       char url[1024];
       char user[50];
       char password[50];
       
       char protocol[10];
       
       
    };
    
    //--------------------------------------------------------------------------
  
    struct ih_card_data_reader
    {
        int ih_driver_id;
        char data[100]; //card code
        unsigned int sizeData; 
        unsigned long int timestamp;
    };
    
    //--------------------------------------------------------------------------
  
    struct ih_computer_port
    {
        int id;
        char name_on_computer[50];
        char path[255];
        char typ[20];
        int   ih_computer_id;
        char parity[2];
        char stop_bit[2];
        int speed;
        char data[2];
    };
    
      
    struct system_data_driver_param
    {
        char address[2];
        int ih_driver_id;
        
        
        char dataBit;
        
        /**
         * n, e, o
         */
        char parity; 

        char stopBit;

        int speed;       
        
        void* param;
        
        bool* result;
        bool* response;
    };
    
    
    
    struct ih_alarm_zone
    {
        int id;
        char name[100];
        int active; // 0 - brak uzbrojenia, 1- uzbrojone, 2-uzbrojone cichy alarm..
        bool toSave; 
        int on_type;
        char on_time[25];
        int on_ih_alarm_device_id; //kto uruchomił alarm..    
              
    };
    
     struct ih_alarm_device
    {
        int id;
        int ih_alarm_zone_id;
        char name[300];
        char type[10];
        bool toSave; 
        
        unsigned long timestampLastChange;
        unsigned long timestampFirst;
        int countMs;
    };
    
    
    
    struct ih_driver
    {
        int id;
        char address[10];
        char name[50];
        int const_function_driver;
        char module[50];  
        int delay_before_read_ms;
        
        char parity[2];
        char stop_bit[2];
        int speed;
        char data[2];
        
        char value[100];
        bool toSave;  //jesli true to trzeba zakutalizować ostatnia zaktualizowaną wartość i dodać do historii..
        
        int ih_zone_id;
        int ih_sector_id;
   
        
        char zone_name[50];
        
 
        
        int ih_sl_driver_id;
        
        int ih_computer_port_id;
        
        char driver_symbol; //1 - modul IO, r - rekuperator, t-czujnik temperatury, o- centralka alarmowa
        char screen[100];
        char name_firm[100];
        char www_firm[255];
        char author[255];
        
    };
    
        
    struct ih_driver_values
    {
        int id;
        int ih_driver_id;
        int px_group_id;
        char name_param[255];
        char value[1000]; 
        int const_function;
        
        int status;     // 0 -none 1 update name, 2 - add to base.. 3 - delete from base
        bool toHistory;  //dododaje nowy stan do historii..

        bool free;        
    };
    
    
    struct ih_driver_values_history__DEL
    {
        int id;
        int ih_driver_values_id;
        std::string value;      
        unsigned long created_at;                      
    };
    
 
    
    struct ih_sl_driver_api
    {
        int id;
        int ih_driver_id;
        char control_type[30];
        int ih_driver_values_id;
        char label_name[100];
        char button_name[50];
        char value_type[100];
        int func_number;
        int func_internal_number;
        char select_val[1000];
        char replace_val[1000];
        int pr_standard;
        int pr_standard_plus;
        int pr_administrator;
        int pr_system;
    };
    
    
      
    struct ih_sl_driver_image
    {
        int id;
        int ih_sl_driver_id;
        char name[500];     
        char path[1000];     
    };
    
  
    
    
    struct ih_driver_port
    {
        int id;
        int num;
        char type;
        int ih_driver_id;
        char status_input;
        char status_output;
        long status_time;
        char reverts;
 
    };
    
    
    
    struct ih_sl_driver
    {
        int id;
        char name[50];
        char name_firm[100];
        char www_firm[100];
        char module[50];
        char author[100];
        char is_modul_io; 
        int const_function_driver;
    };
    
    
    
    
     struct ih_resource_function
    {
        int id;  
      
        int in_const_function_symbol;
        int in_resource_id;        
        char in_fName[50];
        char in_rName[100];
        
        
        int const_function_symbol;
        int resource_id;        
        char fName[50];
        char rName[100];
        
        
        char port_typ;
        int switch_delay_ms;
        int time_to_off_ms;
        int delay_from_start_ms;
        int repeat;   
        char name[250];
        char text_content[500];
        
        char time_active_from[10];
        char time_active_to[10];
        
        int mon;
        int tue;
        int wed;
        int thu;
        int fri;
        int sat;
        int sun;
              
    };
    
    
    
    
    struct ih_driver_port_function
    {
        int id;
        int ih_driver_port_id;
        int const_function_symbol;
        int resource_id;        
        char fName[50];
        char rName[100];
        char port_typ;
        int switch_delay_ms;
        int time_to_off_ms;
        int delay_from_start_ms;
        int repeat;
   
        char name[250];
        char text_content[500];
        
       // int result_const_function_symbol;
       // int result_resource_id;
       // char result_name[250];
       // char result_text_content[500];
        
        //int msg_type;
        
        void* ex_tmp_struct;
        
      // char dataStruct[1024 * 200];//0.5 MB .//w port function to samo
       //unsigned int  sizeDataStruct;
        
    };
    
    
    struct ih_home
    {
        int id;
        char name[255];
        char address_1[255];
        char address_2[255];
        char zip_code[10];
        char city[255];
        char latitude[20];
        char longitude[20];
        char owner_name[255];
    };
    
    
    struct ih_sector
    {
        int id;
        char name[100];   
        char type[10];
        int ih_home_id;        
    };
    
    struct ih_zone
    {
        int id;
        char name[100];   
        char type[10];
        int ih_sector_id;        
    };
    
    
    struct ih_dynamic_menu__DELLL
    {
        int id;
        std::string name;
        std::string access_from_privileges_group;
        int ih_driver_id;
    };
    
    struct ih_lighting
    {
        int id;
        char name[100];
        int ih_zone_id;
        int ih_sector_id;
        int ih_home_id;
        char status; // '1' on '0' off
        unsigned long status_time;      
        unsigned long status_time_last;      
        int wat;
        
        bool toSave;
     
    };
    
    
 
   struct ih_outs
    {
        int id;
        char name[100];
        int ih_zone_id;
        int ih_sector_id;
        int ih_home_id;
        char status; // '1' on '0' off    
               
        bool toSave;
     
    };
    
 
    
      struct ih_blind
    {
        int id;
        char name[100];
        int ih_zone_id;
        int open_percent;
        unsigned long status_time;   
        std::string status_time_str; 
        char last_operation[10];
        
       // char zone_name[100];
        
        int ih_home_id;
        int ih_sector_id;
        
        bool toSave;
       
    };
    
    
     struct ih_gate
    {
        int id;
        char name[100];
        int from_ih_zone_id;        
        int open_percent;
        unsigned long status_time;  
        std::string status_time_str;
        
        //std::string zone_name;
        
        int ih_home_id;
        int ih_sector_id;
        
        bool toSave;
        
        int typ;
    };
    
    
    struct ih_door
    {
       int id;
       int ih_zone_id;
       char name[100];
       int status; //1 - zamkniete, 2 - otwarte
       int type;
       
       bool toSave;
    };
    
    
    
    struct ih_timer
    {
       int id;       
       char name[100];
       int max_sec;
       int sec;
       unsigned long updated_ts;
       int active; // i tak pobiera tylko aktywne..
       int type;
       
       bool toSave;
    };    
    
    
    struct ih_sensor
    {
        int id;
        char name[100];
        int ih_zone_id;
        int status;
        unsigned long status_time;      
        char status_time_str[15];
        char symbol[2];
        
      //  std::string zone_name;
        
        int analogowy;
       char wartosc[255];
       
       int ih_driver_values_id;
        
        bool toSave;
    };
    
  
    struct ih_history
    {
        bool free;
        bool toSave;
        
        int const_function_symbol;
        int resource_id;
        
        char name[500];
        char text_content[1000];
        
        int px_group_id;
        
    };
    
    
    struct ih_scene
    {
        int id;   //id o 1 większe niż indeks w tabeli lokalnej..
        char name[100];        
        
        int status; // 0 -none 1 update name, 2 - add to base.. 3 - delete from base
        bool free; //wpis wypelniony lub nie..
    };
    
    
    struct ih_scene_action
    {
        int id;
        int const_function_symbol;
        int resource_id;
        int order_by;
        int delay_from_start_ms;
        
        char name[500];
        char text_content[500];
        int ih_scene_id;
  
        
        int status; // 0 -none 1 update name, 2 - add to base.. 3 - delete from base
        bool free; //wpis wypelniony lub nie.. //potrzebne dlatego byc mieć stałe id dla akcji przypisanych do sceny..
        
        char functionName[200];
        char resourceName[200];
    };
    
    
    
    struct ih_task
    {
        int id;
        int const_function_symbol;
        char functionName[100];
        int resource_id;
        char resourceName[255];
        
        
        
        unsigned long work_time; //czas od 1900
        char work_time_str[30];   // 12:44:11
        
        int mon;
        int tue;
        int wed;
        int thu;
        int fri;
        int sat;
        int sun;
        int every_week;
        int last_work;
        int system;        
        
        char name[255];
        char text_content[500];
        
        int status; // 0 -none 1 update status itd.., 2 - add to base.. 3 - delete from base
        bool free;
    };
    
    
    
  
 
    
    

    
    struct ih_adv_function__DEL
    {
        int id;
        std::string name;     
        int all_ok;
        int all_nok;
    };
    
    struct ih_adv_function_in_out_DEL
    {
        int id;
        int ih_adv_function_id;
        char type_in_out;
        int ih_lighting_id;
        int ih_blind_id;
        int ih_sensor_id;
        int ih_gate_id;
        int ih_driver_value_id;
        int wartosc_jest_rowna;
        int wartosc_jest_mniejsza;
        int wartosc_jest_wieksza;
        std::string wartosc;
        int const_function_symbol;
        int ih_scene_id;
        int ih_driver_port_id;
        std::string name;
        std::string text_content;
        int px_group_id;
        int resource_id;
        int typ_wartosci;
        
        
        int msg_module_function_response;
        int msg_const_function_response;
        int msg_resource_id;
        int msg_notice_type;
        std::string msg_param1;
        std::string msg_param2;
    };
    
    
    struct ih_change_port_info
    {
        bool* status;
        int ih_driver_port_id;
        bool* response_from_driver;
        
        int switch_delay_ms; //określa czas pomiędzi poszczegolnymi kliknieiciamia ON,OFF ----> czas ---> ON OFF
        int time_to_off_ms;  //okresla jak długo ma trwać stan wysoki.... ON-->czas-->>OFF
        int delay_from_start_ms; //okresla opoznienie od startu....
    };
    
    
    /*
    struct ih_change_port_info_arrayXX
    {
        int count;
        
        bool** status;
        int* ih_driver_port_id;
        int* status_to_set;
    };
     * 
     */
    
#define  IH_MODULE_FUNCTION_NOTICE_SUCCESS   100
#define  IH_MODULE_FUNCTION_NOTICE_ERROR     101
#define  IH_MODULE_FUNCTION_NOTICE_NORMAL    102
#define  IH_MODULE_FUNCTION_NOTICE_PORT_ON   103
#define  IH_MODULE_FUNCTION_NOTICE_PORT_OFF  104
#define  IH_MODULE_FUNCTION_NOTICE_QUERY_YES_NO  105 //system daje mozliwosc zwrocenia odpowiedzi 1 0 // positive negative  // yes no 1- pozitiwe
#define  IH_MODULE_FUNCTION_NOTICE_QUERY_OK      106 //system daje mozliwosc zwrocenia odpowiedzi 1 0 // positive negative //ok anuluj
#define  IH_MODULE_FUNCTION_NOTICE_VIDEO_MSG   107 //system daje mozliwosc zwrocenia odpowiedzi 1 0 // positive negative //ok anuluj

#define  IH_MODULE_FUNCTION_NOTICE_CAM_QUERY_YES_NO  110 //zamiast wiadomości wyswietlony ma zostać widok z kamery |||  1 0 // positive negative  // yes no 1- pozitiwe
#define  IH_MODULE_FUNCTION_NOTICE_CAM_QUERY_OK      111 //zamiast wiadomości ma zostać wyświetlony widok z kamery system daje mozliwosc zwrocenia odpowiedzi 1 0 // positive negative //ok anuluj
    
#define  IH_MODULE_FUNCTION_NOTICE_USER_UPDATE      112 //Jeśli jest nowsza wersja oprogramowania system pyta czy ją pobrać. 

    
 
    struct ih_notice_ex
    {
      //dla wartośco tak
        int p1_const_function_response;
        int p1_module_function_response;
        int p1_resource_id;
                       
        char p1_param1[255];
        char p1_param2[500];
        
        
        //dla wartośco nie
        int p2_const_function_response;
        int p2_module_function_response;
        int p2_resource_id;
                       
        char p2_param1[255];
        char p2_param2[500];  
    };

    
    struct ih_notice
    {
        bool free;
        
        int id;
        int px_group_id;
        bool readed;
        int type;
        char title[50];
        char content[1024];
        
        unsigned long status_time;  
        char status_time_str[15];  
       
        char from[50];
        
        
        
        bool checkResponse; //sprwdz status odpowiedzi...        
        char user_response[255]; //NUM:1 -odpowiedz tak, NUM:0 - odpowiedz nie, NUM:3 - zwykłe ok. lub inna TEXT:jakas wartosc zakodowana w tekscie..
         
        
        //dla wartośco tak
        int p1_const_function_response;
        int p1_module_function_response;
        int p1_resource_id;
                       
        char p1_param1[255];
        char p1_param2[500];
        
        
        //dla wartośco nie
        int p2_const_function_response;
        int p2_module_function_response;
        int p2_resource_id;
                       
        char p2_param1[255];
        char p2_param2[500];
    };
    
    
    //--------------------------------------------------------------------------
 
    struct privilege //do jekiej grupy uprawnien należy
{    
    char name[50];
};

struct zone //do jakich stref ma dostęp użytkownik - ih_privileges_group
{ 
    int id;
    char name[50];
    int ih_sector_id;
};

struct zone_grp  //do jakiej grupy uprawnien stref należy..
{ 
    int id;
    char name[50];
};


struct driver_pr //przechowuje uprawnienia do poszczegolnych zainstlowanych urzadzen...
{   
    int ih_driver_id;
    int ih_sl_driver_id;
    char name[100];
};



  struct ih_short
    {
        int id;
        int const_function_symbol;
        int resource_id;
        
        char name[255];
        char text_content[500];
        
      
        
        int status; // 0 -none 1 update status itd.., 2 - add to base.. 3 - delete from base
        bool free;
    };
    
#define MAX_SHORTS 20
    
  /*
  struct user_gxh_session
  {
      std::string ip;
      std::string ssid;
  };
   */
    
  struct user_gxh
 {
    int px_user_id;
    int px_group_id;
    
    //std::string ssid;
    
    
    //user_gxh_session session[10];
    //int session_count;   
    //char dataProfil[2048];  //przechowuje tymczasowe dane uzytkownika
    std::string ssid;
    
    bool toSave;
    bool free;
    char name[100];
    char last_name[100];
    char username[255];
    char phone[30];
    char awatar[255];
    
    privilege privileges[20];
    unsigned int privilege_count;
    
    zone zones[100];
    unsigned int zones_count;
    
    zone_grp zone_grups[50];
    unsigned int zone_grp_count;
    
    driver_pr drivers[20];
    unsigned int driver_count;
    
    ih_short shorts[MAX_SHORTS];
 };
  
    
    //--------------------------------------------------------------------------
    
    struct ih_nfc_card
    {
        int id;
        char code[100];
        char name[70];
        int type;
        int px_group_id;
        int is_active;
    };
   
    
    struct ih_nfc_card_driver
    {
        int id;
        int ih_nfc_card_id;
        int ih_driver_id;       
    };
    
   //----------------------------------------------------------------------------
    
    struct ih_beacon__DEL
    {
        int id;
        std::string code;
        std::string name;
        std::string details;
        int ih_zone_id;
    };
    
    struct px_group_beacon__DEL
    {
        int id;
        int px_group_id;
        int ih_beacon_id;
    };
    
    //--------------------------------------------------------------------------
    



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//Convertion
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------



std::string gxh_IntToString(int value);

//------------------------------------------------------------------------------

 std::string gxh_DoubleToString(double value);

//------------------------------------------------------------------------------
 
 std::string gxh_UnsignedLongToString(unsigned long value);
 
 
//------------------------------------------------------------------------------

 std::string gxh_CharToString(char c);
 
//------------------------------------------------------------------------------
 
char gxh_StringToChar(std::string value);
 
//------------------------------------------------------------------------------

 
 int gxh_StringToInt(std::string value);

//------------------------------------------------------------------------------

 double gxh_StringToDouble(std::string value);
 
//------------------------------------------------------------------------------

 unsigned long gxh_StringToUnsignedLong(std::string value);
 
//------------------------------------------------------------------------------

 bool gxh_replace(std::string& str, const std::string& from, const std::string& to);
 
//------------------------------------------------------------------------------

bool gxh_replaceAll(std::string& str, const std::string& from, const std::string& to);
 
//------------------------------------------------------------------------------

int gxh_compare(const char* string, const char* needed);

//------------------------------------------------------------------------------

long int gxh_getTimestamp();
//------------------------------------------------------------------------------

unsigned long gxh_getSecondsFrom1900( );

//------------------------------------------------------------------------------
std::string gxh_currentDateTime();
//------------------------------------------------------------------------------
char* gxh_base64_encode(const unsigned char *data, int input_length, int *output_length);
//------------------------------------------------------------------------------

#endif	/* GXH_STRUCTURE_H */