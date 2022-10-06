/* 
 * File:   gxh_scope.cpp
 * Author: Blazej Kita
 *
 * Created on 15 may 2018, 10:50
 */

#include "gxh_scope.h"


//------------------------------------------------------------------------------

gxh_scope::gxh_scope(int argc, char** argv) 
{
    this->argc = argc;
    this->argv = argv;
    
    this->debug_level = 1;        
    
    if(argc > 2) //.gxh ++++++ ./
    {
        this->debug_level = (unsigned short)strlen(argv[1]);
        strcpy(this->runPath, argv[2] );
    }             
    
    struct timeval tp;
    gettimeofday(&tp, NULL);
    long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    
    this->ms_log_core      = ms;
    this->ms_log_module    = ms;
    this->ms_log_driver    = ms;
    this->ms_log_data_base = ms;
    this->ms_log_console   = ms;
    this->ms_log_port      = ms;
    
    //std::cout<<"\nGXH Starting, debug level: "<<this->debug_level<<"\n";
}

//------------------------------------------------------------------------------

gxh_scope::~gxh_scope() 
{    
}

//------------------------------------------------------------------------------

std::string gxh_scope::getRunPath()
{
    return std::string(this->runPath);
}

//------------------------------------------------------------------------------

char** gxh_scope::getParams(int* argc)
{   
    *argc = this->argc;
    return this->argv;
}

//------------------------------------------------------------------------------

void gxh_scope::setDebugLevel(int level)
{
    this->debug_level = level;
}   

//------------------------------------------------------------------------------

int gxh_scope::getDebugLevel()
{
    return this->debug_level;
}  

//------------------------------------------------------------------------------

 std::string gxh_scope::getDB()
 {
   return "/var/www/html/db/gxh_base.db";   
 }
 std::string gxh_scope::getDBTmp()
 {
     return "/var/www/html/db/gxh_base_tmp.db";
 }

//------------------------------------------------------------------------------

void gxh_scope::show_error(const char* className, const char* functionName, const char* msg, int level)
{
    if(level < this->debug_level)
    {
        time_t t = time(0);   // get time now
        struct tm * now = localtime(&t);            
        std::cout <<"\033[1;31m"<<(now->tm_year+1900)<<"-"<<(now->tm_mon+1)<<"-"<<now->tm_mday<<" "<<now->tm_hour<<":"<<now->tm_min<<":"<<now->tm_sec<<" | Object: "<<className<<", Task: "<<functionName<<", msg: "<<msg<<"\033[0m\n";   
    }
}

 

//------------------------------------------------------------------------------


void gxh_scope::show_log_core(const char* className, const char* functionName, const char* msg, int level)
{
    if(level < this->debug_level)
    {                      
      struct timeval tp;
      gettimeofday(&tp, NULL);
      long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    
      unsigned long r = ms - this->ms_log_core; 
      this->ms_log_core = ms;
    
      time_t t = time(0);   // get time now
      struct tm * now = localtime(&t);  
      std::cout <<"\033[1;33m"<<(now->tm_year+1900)<<"-"<<(now->tm_mon+1)<<"-"<<now->tm_mday<<" "<<now->tm_hour<<":"<<now->tm_min<<":"<<now->tm_sec<<":"<<ms<<", R: "<<r<<" "<<" | Object: "<<className<<", Task: "<<functionName<<", msg: "<<msg<<"\033[0m\n";     
    }
}

//------------------------------------------------------------------------------

void gxh_scope::show_log_module(const char* className, const char* functionName, const char* msg, int level )
{
    if(level < this->debug_level)
    {                      
      struct timeval tp;
      gettimeofday(&tp, NULL);
      long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    
      unsigned long r = ms - this->ms_log_module; 
      this->ms_log_module = ms;
    
      time_t t = time(0);   // get time now
      struct tm * now = localtime(&t);  
      std::cout <<"\033[1;34m"<<(now->tm_year+1900)<<"-"<<(now->tm_mon+1)<<"-"<<now->tm_mday<<" "<<now->tm_hour<<":"<<now->tm_min<<":"<<now->tm_sec<<":"<<ms<<", R: "<<r<<" "<<" | Object: "<<className<<", Task: "<<functionName<<", msg: "<<msg<<"\033[0m\n";     
    }
}

//------------------------------------------------------------------------------

void gxh_scope::show_log_driver(const char* className, const char* functionName, const char* msg, int level)
{
    if(level < this->debug_level)
    {                      
      struct timeval tp;
      gettimeofday(&tp, NULL);
      long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    
      unsigned long r = ms - this->ms_log_driver; 
      this->ms_log_driver = ms;
    
      time_t t = time(0);   // get time now
      struct tm * now = localtime(&t);  
      std::cout <<"\033[1;32m"<<(now->tm_year+1900)<<"-"<<(now->tm_mon+1)<<"-"<<now->tm_mday<<" "<<now->tm_hour<<":"<<now->tm_min<<":"<<now->tm_sec<<":"<<ms<<", R: "<<r<<" "<<" | Object: "<<className<<", Task: "<<functionName<<", msg: "<<msg<<"\033[0m\n";     
    } 
}

//------------------------------------------------------------------------------

void gxh_scope::show_log_data_base(const char* className, const char* functionName, const char* msg, int level)
{
    if(level < this->debug_level)
    {                      
      struct timeval tp;
      gettimeofday(&tp, NULL);
      long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    
      unsigned long r = ms - this->ms_log_data_base; 
      this->ms_log_data_base = ms;
    
      time_t t = time(0);   // get time now
      struct tm * now = localtime(&t);  
      std::cout <<"\033[1;37m"<<(now->tm_year+1900)<<"-"<<(now->tm_mon+1)<<"-"<<now->tm_mday<<" "<<now->tm_hour<<":"<<now->tm_min<<":"<<now->tm_sec<<":"<<ms<<", R: "<<r<<" "<<" | Object: "<<className<<", Task: "<<functionName<<", msg: "<<msg<<"\033[0m\n";     
    } 
}

//------------------------------------------------------------------------------

void gxh_scope::show_log_console(const char* className, const char* functionName, const char* msg, int level)
{
     if(level < this->debug_level)
    {                      
      struct timeval tp;
      gettimeofday(&tp, NULL);
      long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    
      unsigned long r = ms - this->ms_log_console; 
      this->ms_log_console = ms;
    
      time_t t = time(0);   // get time now
      struct tm * now = localtime(&t);  
      std::cout <<"\033[1;35m"<<(now->tm_year+1900)<<"-"<<(now->tm_mon+1)<<"-"<<now->tm_mday<<" "<<now->tm_hour<<":"<<now->tm_min<<":"<<now->tm_sec<<":"<<ms<<", R: "<<r<<" "<<" | Object: "<<className<<", Task: "<<functionName<<", msg: "<<msg<<"\033[0m\n";     
    } 
}

//------------------------------------------------------------------------------

void gxh_scope::show_log_port(const char* className, const char* functionName, const char* msg, int level)
{
      if(level < this->debug_level)
    {                      
      struct timeval tp;
      gettimeofday(&tp, NULL);
      long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    
      unsigned long r = ms - this->ms_log_port; 
      this->ms_log_port = ms;
      
    
      time_t t = time(0);   // get time now
      struct tm * now = localtime(&t);  
      std::cout <<"\033[1;36m"<<(now->tm_year+1900)<<"-"<<(now->tm_mon+1)<<"-"<<now->tm_mday<<" "<<now->tm_hour<<":"<<now->tm_min<<":"<<now->tm_sec<<":"<<ms<<", R: "<<r<<" "<<" | Object: "<<className<<", Task: "<<functionName<<", msg: "<<msg<<"\033[0m\n";     
    }   
}

//------------------------------------------------------------------------------

std::string gxh_scope::exec(const char* cmd) {
    
    
    
    FILE* pipe = popen(cmd, "r");
    if (!pipe) return "";
    char buffer[128];
    std::string result = "";
    while(!feof(pipe)) {
        if(fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    pclose(pipe);
    return result;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
