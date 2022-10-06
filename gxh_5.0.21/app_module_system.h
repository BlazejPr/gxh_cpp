/* 
 * File:   app_module_system.h
 * Author: Blazej Kita
 *
 * Created on 23 lipiec 2018, 14:31
 */

#include "app_module.h"


#ifndef APP_MODULE_SYSTEM_H
#define	APP_MODULE_SYSTEM_H

#include <math.h>

#include <netdb.h>


struct adv_function
{
    int id;
    char name[50];
    
 
    
    int all_ok;
    int all_nok;
    
    bool toSave;
    
    int active;
};

struct adv_var
{
    int id;
    int ih_adv_function_id;
    char type_ion[2];
    char resource_name[50];
    int resource_id;
    int wartosc_jest_rowna;
    int wartosc_jest_wieksza;
    int wartosc_jest_mniejsza;
    char wartosc[255];
    int typ_wartosci;
    int out_const_function_symbol;
    int out_resource_id;
    char out_param1[255];
    char out_param2[1024];        
    
    ih_notice_ex notice_ex;
};
 

class app_module_system : public app_module{
public:
         app_module_system(gxh_scope* appHomeHandle); 
         virtual ~app_module_system();      
   
         bool execute(gxh_message* inMsg, gxh_message* outMsg);   
 
         void forceCancel();
         void forceSynchronize();     
         void onClick(int ih_driver_port_id, int listener_status);
    
 private:    
     
         adv_function func[50];
         int count_func;
         
         adv_var func_var[100];
         int count_func_var;
         
         void getHost(char* from, char* to,char* param);
         
         bool checkVersion(gxh_message* inMsg, gxh_message* outMsg);
         
     
         bool lightBackup(gxh_message* inMsg, gxh_message* outMsg);  
         bool restartHardware(gxh_message* inMsg, gxh_message* outMsg);  
         bool restartApp(gxh_message* inMsg, gxh_message* outMsg);  
         
         bool notice_query(gxh_message* inMsg, gxh_message* outMsg);  
         bool notice(gxh_message* inMsg, gxh_message* outMsg);  
         bool notice_version(gxh_message* inMsg, gxh_message* outMsg);  
         
         bool goUpdate(gxh_message* inMsg, gxh_message* outMsg);  
         /**
          * 
          * @param inMsg
          * @param outMsg
          * @param status 0 - off, 1 - on, 2- click
          * @return 
          */
         bool setPort(gxh_message* inMsg, gxh_message* outMsg, int status);  
         
         bool setPortClick(gxh_message* inMsg, gxh_message* outMsg, int sek);  
  
         bool check_internet(gxh_message* inMsg, gxh_message* outMsg); 
         
         bool check_adv(gxh_message* inMsg, gxh_message* outMsg);
         
         
         int locationToMetrs(const char* location);
         double Distance( double lat1,  double lon1,  double lat2,  double lon2) ;
 
};



extern "C" app_module* get_module(gxh_scope* appHomeHandle);

#endif	/* APP_MODULE_SYSTEM_H */


