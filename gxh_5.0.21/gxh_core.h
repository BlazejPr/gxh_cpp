/* 
 * File:   gxh_core.cpp
 * Author: Blazej Kita
 *
 * Created on 15 may 2018, 10:50
 */

#include "gxh_scope.h"
#include "app_module.h"
#include "app_zc_queue.h"
#include "app_zi_queue.h"
#include "app_port_ethernet.h"
#include "app_port_serial.h"


//modules...............................

typedef  app_module* (*cosine_func_ptr)(gxh_scope*); 

struct function_item
{
    std::string description;            //Włącznik światła
    unsigned int const_function_symbol; //100
    char type;                          //I O P
    char run_as_service;                // '1' lub '0'
    unsigned long run_ms_delay;         // -3 czas opóźnienia..
    unsigned long run_ms_last;          //potrzebne dla uslug..    
};

struct module_item
{
    std::string module_name;
    function_item functions[MAX_MODULE_FUNCTION]; //max 40 funkcji w jednym module...
    unsigned int module_function_count;
    app_module* moduleObject;
    char ready;                 // 0 - nie mozna korzystac z modulu, 1 - mozna korzystac z modulu..., 2 - czeka na otworzenie obiektu..
    cosine_func_ptr getModule;
    void *handle;
};
//end modules............................




//interrupt   for core..
struct gxh_msg_info
{
    gxh_message message;
    char status; // 1 - wolne, mozna zapisac lub nadpisać, 2 - blokada - nie ruszać..., 3 od wykonania.., 4- wykonuje..
    pthread_t thread;
    long int thread_created_at;   //czas utworzenia watku timestam ms
    long int thread_max_time;     //maksmalny czas wykonania wątkyu w ms
};

struct gxh_msg_info_cd //command
{
    gxh_message message;
    char status; // 1 - wolne, mozna zapisac lub nadpisać, 2 - blokada - nie ruszać..., 3 od wykonania.., 4- wykonuje.. 
};

//end interrupts.........................

//gxh handel...
struct gxh_handle_list_src
{
    unsigned int gxh_handle; 
    char status; // 1 - wolne, mozna zapisac lub nadpisać, 2 - blokada - nie ruszać..., 3 zajete..
    int  resId;
    char resType; // 1 - driver, 2 socket, 3 - other..    
    app_zc_queue* msg_queue; //tylko jedna wiadomość.. w przyszłości można rozszerzyć...
    char name[255];
};


struct resorce_vec ////
{
    void* ptr;
    unsigned int lenght;
    bool free;
};

//end gxh handle...


class gxh_core : public gxh_scope
{
    private:
            unsigned short thInterrupt;  // 0 - go running, 1 - go close, 2 - go restart
            unsigned short thService;    // 0 - go running, 1 - go close, 2 - go restart     
            unsigned short thSynchronize;    // 0 - go running, 1 - go close, 2 - go restart     
    
            pthread_t mainThread;
            pthread_t mainThreadService;    
            pthread_t mainThreadSynchronize;  
             
            bool executeThreadTask;
            
            pthread_mutex_t gxh_response_muteks;  
            pthread_mutex_t gxh_handle_muteks;  
            
            //modules
            module_item modules[MAX_MODULE_BUFFER]; //def 100
            unsigned int modules_count;
            
            ih_sl_const_function_symbol functions[MAX_FUNCTION_BUFFER];
            unsigned int functions_count;
            
    
            ih_sl_const_function_hash symbols[MAX_SYMBOL_HASH_BUFFER];
            unsigned int symbols_count;
            
            void loadModule();
            
            //z modułami ładuję sie rozkład pomieszczeń...
            ih_sector sectors[100];
            unsigned int sectors_count;
            ih_zone   zones[400];
            unsigned int zones_count;
            
            ih_home   homes[1];
            unsigned int homes_count;
            
            
            
            //ports and drivers..
            ih_computer_port computer_ports[MAX_COMPUTER_PORT]; //max 10 ports..
            unsigned int computer_ports_count;
            
            app_port* ports[MAX_COMPUTER_PORT];
            app_port_ethernet* ethernet;
            
            ih_driver drivers[MAX_DRIVERS];
            unsigned int drivers_count;
            
            ih_driver_port driver_ports[ MAX_DRIVERS * MAX_PORT_IN_DRIVER ];
            unsigned int driver_ports_count;
            
            ih_sl_driver sldriver[MAX_SLDRIVER]; // max 40 róznych sterowników
            unsigned int sldriver_count;
                      
            ih_sl_driver_api driver_api[MAX_DRIVER_API];
            unsigned int driver_api_count;
            
            ih_sl_driver_image driver_image[MAX_DRIVER_IMAGE];
            unsigned int driver_image_count;
            
            ih_driver_values driver_values[MAX_DRIVER_VALUE];
            unsigned int driver_value_count;
            
            void loadPorts(); //load computer ports, ethernet, driver,...
            
            ih_notice notices[MAX_NOTICE_CACHE];
            
            ih_history histories[MAX_HISTORY_BUFFER];
            
            //vectors..
            
            resorce_vec res_verctor[RES_VECTOR_TO];
            
            //--------
            
            
            ih_driver_port_function port_function[MAX_PORT_FUNCTION_BUFFER];
            unsigned int port_function_count;
            void loadFunction();   
            
            
            ih_resource_function resource_function[MAX_RESOURCE_FUNCTION_BUFFER];
            unsigned int resource_function_count = 0;
            void loadResourceFunction();  
            
            gxh_msg_info gxh_msg_interrupt_buf[MAX_GXH_MESSAGE_INTERRUPT];
            gxh_msg_info_cd gxh_msg_response_buf[MAX_GXH_MESSAGE_INTERRUPT];
            
            
            gxh_handle_list_src gxh_list[MAX_GXH_HANDLE_LIST];
            
            
            bool getMessageForCore(gxh_message* ptr, unsigned int* index);
            
            void closeApplication();     
            
            void synchronizeHistory( );
            
            void checkVersionGXH();
     public:
         
            void addHistory(int const_function_symbol, int resource_id, char* name, char* text_content, int px_group_id);
         
            void get_resource_name(sqlite3* dbHandle, char* ret,int bufSize, int const_function_symbol);
            void get_resource_name_func( sqlite3* dbHandle,int resource_id ,char* ret, char* resName, int resNameSize);
         /**
          * 
          * @param argc
          * @param argv
          */
          gxh_core(int argc, char** argv); 
     
          /**
           * 
           */
          ~gxh_core();
          
          /**
           * 
           * @param buffer
           */
          std::string getVersion();
          std::string getLinux();
          std::string getAuthor();
          std::string getKey();
          std::string getComp();
          
          void* runThread();
          void* runThreadInterrupt( void* intr_str );
          void* runThreadService();  
          void* runThreadSynchronize();  
          
          void forceSynchronize(); //synchronizuje dane urzadzen i zmiennych systemowych....
          
          bool sendGXHMessage( gxh_message* gxhMessage);          
          bool addResponse( gxh_message* gxhMessage);       
          bool receiveGXHMessage( unsigned int gxhHandle, gxh_message* gxhMessage, int time_out_ms);
          unsigned int  createGXHHandle(const char* resName, int resId, char resType);
          void destroyGXHHandle(unsigned int gxhHandle);
          unsigned int getGXHHandleByIdAndtype(int resId, char resType);
          
          bool registerResource(void* ptr, unsigned int size, unsigned int type);
          void* getResource(unsigned int type, unsigned int* size);
          bool destroyResource(unsigned int type);
          
          bool execute( gxh_message* gxhMessage,  gxh_message* response);
          
          bool addNotice(int px_group_id,const char* from, const char* title, const char* msg, int type, ih_notice_ex* notice_ex );
          
          ih_driver_port_function* getDriverPortFunction(int* count);
          ih_computer_port* getComputerPorts(int* count);
          ih_driver* getDrivers(int* count);
          ih_driver_port* getPortsDriver(int* count);
          ih_sl_driver* getSlDriver(int* count);
         
          
           bool createBase();
          
          /**
           * Tworzy tabele na podstawie aplikacji..
           * @param name
           * @param tmp - baza mniej znacząca..
           * @return 
           */
           bool createTable(const char* schema, bool tmp);

      
           void clearDataBaseTmp(); 
           /**
            * 
            * @param schema
            * @return 
            */
           bool deleteTmpQuery(const char* schema);           
       
           
           /**
          * Metoda dodaje nową zmienną systemową. Jeśli już istnieje nie zostanie dodana.
          * @param name
          * @param uniqueId
          * @param px_group_id
          * @param ih_driver_id
          * @param value
          * @return 
          */
          bool registerVarSys(const char* name, int uniqueId, int px_group_id, int ih_driver_id, const char* value );
          
          /**
           * 
           * @param uniqueId
           * @param description
           * @param type
           * @param module
           * @param run_as_service
           * @param delay_ms
           * @param param1Name
           * @param param2Name
           * @param resName
           * @return 
           */
          bool registerFunctionSys( unsigned int uniqueId, const char* description, const char* type, const char* module, const char* run_as_service, const char* delay_ms, const char* param1Name, const char* param2Name, const char* resName, const char* min_version );
};