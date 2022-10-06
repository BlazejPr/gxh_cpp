

#ifndef APP_MODULE_H
#define	APP_MODULE_H

#include "gxh_scope.h"

class app_module {
    
    private:   
             gxh_scope* appHandle;    
             std::string moduleName;
    
    protected:             
             /**
              * Return object of scope
              * @return 
              */
             gxh_scope* getAppHandle();
             
             /**
              * 
              * @param gxhHandle
              * @param intMsg
              * @param message
              * @return 
              */
             bool closeApp(gxh_message* intMsg, gxh_message* message);
             
        
    
             /**
              * 
              * @param gxhHandle
              * @param intMsg
              * @param message
              * @return 
              */
             bool reloadModules( gxh_message* intMsg,gxh_message* message);
    
             /**
              * 
              * @param gxhHandle
              * @param intMsg
              * @param message
              * @return 
              */
             bool reloadPorts(gxh_message* intMsg, gxh_message* message);
             
             
             /**
              * 
              * @param intMsg
              * @param message
              * @param num 100 - run apache, 101 - wylacz panel www, 200 - run ftp, 201- stop ftp, 300 - run ssh, 301 - sotp ssh
              * @return 
              */
             bool executeSystemConsole(gxh_message* intMsg, gxh_message* message, int num);
             
             /**
              * @param gxh_message* intMsg
              * Metoda wykonuje jedna funkcje systemowa.
              * @param ih_sl_const_function_symbol identyfikator funkcji
              * @param resource_id id zasobu
              * @return 
              */
              bool executeFunction(gxh_message* intMsg,gxh_message* message ,int ih_sl_const_function_symbol, int resource_id);     
              
                         /**
              * 
              * @param ih_driver_id
              * @param newAddresss
              * @param dataBit
              * @param parity
              * @param stopBit
              * @param speed
              * @return 
              */
             bool changeAddressDriver(gxh_message* intMsg,int ih_driver_id, char* newAddresss, char dataBit, char parity, char stopBit, int speed);
    
              /**
               * @param gxh_message* intMsg
               * @param ih_sl_const_function_symbol identyfikator funkcji
               * @param resource_id identyfikator zasobu
               * @param px_group_id identyfikator uzytkownika
               * @return 
               */
               bool executeFunction(gxh_message* intMsg,gxh_message* message, int ih_sl_const_function_symbol, int resource_id, int px_group_id); 
        
               /**
                * Metoda wykonuje jedna funkcje systemowa.
                * @param ih_sl_const_function_symbol identyfikator funkcji
                * @param resource_id id zasobu
                * @param name, nazwa funkcji, np. włącznik górny
                * @param text_content, informacje dodatkowe
                * @return 
                */
               bool executeFunctionParam(gxh_message* intMsg,gxh_message* message, int ih_sl_const_function_symbol, int resource_id, std::string name, std::string text_content,  void* ex_tmp_struct);     
    
                /**
                 * @param gxh_message* intMsg
                 * @param ih_sl_const_function_symbol identyfikator funkcji
                 * @param resource_id identyfikator zasobu
                 * @param name parametr 1
                 * @param text_content parametr 2
                 * @return 
                 */
               bool executeFunctionParam( gxh_message* intMsg,gxh_message* message,int ih_sl_const_function_symbol, int resource_id, std::string name, std::string text_content, int px_group_id,  void* ex_tmp_struct);     
    
               
               /**
                * Metoda wysyła przerwanie systemowe do cora..
                * @param ih_sl_const_function_symbol, np. swiatło zostało zapalone..
                * @param resource_id
                * @return 
                */
               bool sendInterrrupt(int ih_sl_const_function_symbol, int resource_id);
               
               /**
                * 
                * @param gxh_message* intMsg
                * @param ih_sl_const_function_symbol
                * @param resource_id
                * @param name
                * @param text_content
                * @param px_group_id
                * @param result_const_function_symbol
                * @param result_resource_id
                * @param result_name
                * @param result_text_content
                * @param msg_type
                * @return 
               */
            //   bool executeFunctionParamWithResult(gxh_message* intMsg,gxh_message* message, int ih_sl_const_function_symbol, int resource_id, std::string name, std::string text_content, int px_group_id, int result_const_function_symbol,int result_resource_id, std::string result_name, std::string result_text_content,  int msg_type ); 
               /**
                * 
                * @param gxh_message* intMsg
                * @param ih_sl_const_function_symbol ih_sl_const_function_symbol identyfikator funkcji
                * @param ih_module_function numer funkcjie wewnetrzej modułu
                * @param resource_id resource_id id zasobu
                * @param name  nazwa funkcji, np. włącznik górny
                * @param text_content  informacje dodatkowe
                * @return 
                */
                bool executeFunctionParam(gxh_message* intMsg,gxh_message* message, int ih_sl_const_function_symbol,int ih_module_function, int resource_id, std::string name, std::string text_content,  void* ex_tmp_struct);     
    
                /**
                 * metoda kieruje wykonanie polecenia do okroślonego modułu - urządzenia. 
                 * @param ih_driver_id
                 * @param ih_module_function
                 * @param param
                 * @return 
                 */
                 bool executeFunctionDriver(gxh_message* intMsg,gxh_message* message, int ih_driver_id, int ih_module_function, std::string param); 
                 
 
                 
             
      public:          
             app_module(gxh_scope* appHomeHandle); 
             virtual ~app_module();  
             
             /**
              * Execute interrupt from GXHSystem
              * @param inMsg
              * @param outMsg
              * @return 
              */
             virtual bool execute(gxh_message* inMsg, gxh_message* outMsg);
             
             /**
              * Method is call when GXHSystem is going to delete object from memory. Method call from clientThread
              */
             virtual void forceCancel();
             
             /**
              * Synchronize, method call from serviceThread
              */
             virtual void forceSynchronize();
                          
             /**
              * 
              * @param ih_driver_port_id
              * @param listener_status
              */
             virtual void onClick(int ih_driver_port_id, int listener_status);
             
             /**
              * Send information to core
              * @param ih_driver_port_id
              * @param listener_status
              */
             virtual void changePortListener(int ih_driver_port_id, int listener_status);
                         
             /**
              * 
              * @param ih_driver_port_id
              * @param statusType  IH_SET_PORT_CLICK itp
              * @param switch_delay_ms  dla duouble click przerwa pomiędzy kliknięciami..
              * @param time_to_off_ms ja długo jest podtrzymywane stan wysoki..
              * @param delay_from_start_ms // opoznieie od startu..
              * @return 
              */                        
             bool setPortStatus(int ih_driver_port_id, int statusType, int switch_delay_ms, int time_to_off_ms, int delay_from_start_ms  );

    
};


extern "C" app_module* get_moduleStandard(gxh_scope* appHomeHandle);

#endif	/* APP_MODULE_H */

    