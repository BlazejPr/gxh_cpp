/* 
 * File:   gxh_scope.cpp
 * Author: Blazej Kita
 *
 * Created on 15 may 2018, 10:50
 */


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

//dla sieci

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/ioctl.h>


#include <fcntl.h>    /* For O_RDWR */
#include <unistd.h> 

#include "sqlite3.h"
#include "gxh_structure.h"

#ifndef GXH_SCOPE_H
#define	GXH_SCOPE_H




class gxh_scope
{
    private:        
       int argc;             //number of params
       char** argv;          //params
       
       char runPath[1024];
       
       unsigned short debug_level;
    
       
       unsigned long ms_log_core;
       unsigned long ms_log_module;
       unsigned long ms_log_driver;
       unsigned long ms_log_data_base;
       unsigned long ms_log_console;
       unsigned long ms_log_port;       
    public:       
         /**
          * 
          * @param argc
          * @param argv
          */
         gxh_scope(int argc, char** argv) ;    
         
         /**
          * 
          */
         virtual ~gxh_scope();
        
         /**
          * 
          * @param argc
          * @return 
          */
         char** getParams(int* argc);
         
         /**
          * 
          * @param buffer
          */
         std::string getRunPath();
         /**
          * Return actual version of GXH System
          * @param buffer
          */
         virtual std::string getVersion() = 0;
         
         std::string getDB();
         std::string getDBTmp();
         
         std::string exec(const char* cmd);
         
         /**
          * 
          * @param className
          * @param functionName
          * @param msg
          * @param level
          */
         void show_error(const char* className, const char* functionName, const char* msg, int level);
         
         /**
          * 
          * @param className
          * @param functionName
          * @param msg
          * @param level
          */
         void show_log_core(const char* className, const char* functionName, const char* msg, int level);
         
         /**
          * 
          * @param className
          * @param functionName
          * @param msg
          * @param level
          */
         void show_log_module(const char* className, const char* functionName, const char* msg, int level);
         
         /**
          * 
          * @param className
          * @param functionName
          * @param msg
          * @param level
          */
         void show_log_driver(const char* className, const char* functionName, const char* msg, int level);
         
         /**
          * 
          * @param className
          * @param functionName
          * @param msg
          * @param level
          */
         void show_log_data_base(const char* className, const char* functionName, const char* msg, int level);
         
         /**
          * 
          * @param className
          * @param functionName
          * @param msg
          * @param level
          */
         void show_log_console(const char* className, const char* functionName, const char* msg, int level);
         
         /**
          * 
          * @param className
          * @param functionName
          * @param msg
          * @param level
          */
         void show_log_port(const char* className, const char* functionName, const char* msg, int level);
         
         
         virtual void addHistory(int const_function_symbol, int resource_id, char* name, char* text_content, int px_group_id) = 0;
         
          virtual void get_resource_name(sqlite3* dbHandle, char* ret,int bufSize, int const_function_symbol) = 0;
          virtual void get_resource_name_func( sqlite3* dbHandle,int resource_id ,char* ret, char* resName, int resNameSize) = 0;
         
         /**
          * Send interrupt to gxhCore
          * @param gxhMessage
          * @return 
          */
         virtual bool sendGXHMessage( gxh_message* gxhMessage) = 0;
         
         /**
          * 
          * @param gxhMessage
          * @return 
          */
         virtual bool addResponse( gxh_message* gxhMessage) = 0;
         
         /**
          * Return command for driver by gxhHandle id
          * @param gxhHandle
          * @param gxhMessage
          * @param time_out_ms
          * @return 
          */
         virtual bool receiveGXHMessage( unsigned int gxhHandle, gxh_message* gxhMessage, int time_out_ms) = 0 ;
         
         /**
          * Method is responsible for executing operation from another module
          * @param gxhMessage
          * @param response
          * @return true is response
          */
         virtual bool execute( gxh_message* gxhMessage,  gxh_message* response) = 0;
         
         /**
          * 
          * @param resName
          * @param resId
          * @param resType, 1 - ih_driver_id, 2 - socket number, 3 - module local handle..
          * @return 0 if error
          */
         virtual unsigned int  createGXHHandle(const char* resName, int resId, char resType) = 0;
         
         /**
          * 
          * @param gxhHandle
          */
         virtual void destroyGXHHandle(unsigned int gxhHandle) = 0;
         
         /**
          * 
          * @param resId
          * @param resType
          * @return 
          */
         virtual unsigned int getGXHHandleByIdAndtype(int resId, char resType) = 0;
         
         /**
          * Method returns address of first element of array with porrt function. Please don't use delete[] for pointer.
          * @param count, number of element array
          * @return address of first element
          */
         virtual ih_driver_port_function* getDriverPortFunction(int* count) = 0;
         
         /**
          * Method returns address of first element of array with computer port. Please don't use delete[] for pointer.
          * @param count number of element array
          * @return  address of first element
          */
         virtual ih_computer_port* getComputerPorts(int* count) = 0;
         
         /**
          * Method returns addres of first element of array with drivers
          * @param count
          * @return 
          */
         virtual ih_driver* getDrivers(int* count) = 0;
        
         /**
          * Method returns addres of first element of array with ports of driver
          * @param count
          * @return 
          */
         virtual ih_driver_port* getPortsDriver(int* count) =0;
         
         /**
          * Get list software of driver
          * @param count
          * @return 
          */
         virtual ih_sl_driver* getSlDriver(int* count) = 0;
         
         /**
          * Register pointer to local module resource
          * @param ptr
          * @param size
          * @return 
          */
         virtual bool registerResource(void* ptr, unsigned int size, unsigned int type) = 0;        
         
         /**
          * Method returns pointer to first elemtent of resource array
          * @param type
          * @param size
          * @return 
          */
         virtual void* getResource(unsigned int type, unsigned int* size) = 0;
         
         /**
          * 
          * @param type
          * @return 
          */
         virtual bool destroyResource(unsigned int type) = 0;        
         
         /**
          * 
          * @return 
          */
         virtual std::string getLinux() = 0;
         
         /**
          */
         virtual std::string getAuthor() = 0;
         
         virtual std::string getKey() = 0;
         
         virtual std::string getComp() = 0;
         
         /**
          * 
          * @param level 1-20
          */
         void setDebugLevel(int level);
         int getDebugLevel();
         
         /**
          * 
          * @param px_group_id
          * @param title
          * @param msg
          * @param type IH_MODULE_FUNCTION_NOTICE_NORMAL itd..
          * @param const_function_response
          * @param module_function_response
          * @param resource_id
          * @return true if send or false if not
          */
         virtual bool addNotice(int px_group_id,const char* from, const char* title, const char* msg, int type, ih_notice_ex* notice_ex ) = 0;
         
         
         /**
          * Metoda dodaje nową zmienną systemową. Jeśli już istnieje nie zostanie dodana. Uwaga metoda powinna zostać wywołana jedynie w inniej metodzie synchronicznej. Kontroler lub forceSynchronize..
          * @param name
          * @param uniqueId
          * @param px_group_id
          * @param ih_driver_id
          * @param value
          * @return 
          */
         virtual bool registerVarSys(const char* name, int uniqueId, int px_group_id, int ih_driver_id, const char* value ) = 0;
         
         
         /**
          * 
          * @param uniqueId Nume funkcji
          * @param description Nazwa funkcji
          * @param type I O P
          * @param module module.so
          * @param run_as_service  1 0
          * @param delay_ms -1 -2 0
          * @param param1Name Tytul
          * @param param2Name Treść
          * @resName light, blind, other....
          * @return true if is already exist..
          */
         virtual bool registerFunctionSys( unsigned int uniqueId, const char* description, const char* type, const char* module, const char* run_as_service, const char* delay_ms, const char* param1Name, const char* param2Name, const char* resName, const char* min_version ) = 0;
};

#endif	/* GXH_SCOPE_H */
