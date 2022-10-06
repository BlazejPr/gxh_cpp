
/* 
 * File:   app_driver.h
 * Author: Blazej Kita
 *
 * Created on 30 sierpień 2016, 14:35
 */

#include "app_port.h"
#include "app_modbus_rtu.h"


#ifndef APP_DRIVER_H
#define	APP_DRIVER_H

#define	APP_DRIVER_EXECUTE_RESUME 0
#define	APP_DRIVER_EXECUTE_ONLY 1
#define	APP_DRIVER_RESUME_ONLY 2

 

class app_driver {
public:
    app_driver(gxh_scope* appHomeHandle, app_port* appPortHandle,int ih_driver_id, const char* name, const char* address, int delay_before_read_ms);
    virtual ~app_driver();
    
    /**
     * Execute command received form core
     * @param command
     */
    virtual void execute(gxh_message* command);
    
    /**
     * Execute task in loop
     * @return true if device executed task, false if nothing to do 
     */
    virtual bool resume();    
    
    /**
     * 
     * @return 0 = APP_DRIVER_EXECUTE_RESUME , 1 = APP_DRIVER_EXECUTE_ONLY, 2 = APP_DRIVER_RESUME_ONLY
     */
    int getOperationType(); 

    
private:    
    gxh_scope* appHandle;
    app_port* appPort;
    int debugLevel;
    
    
    int operationTyp;
     
    int ih_driver_id;
    char name[50];
    char address[10];
    int delay_before_read_ms;
    
protected:
           
    
    
    /**
     *  APP_DRIVER_EXECUTE_ONLY - Device only waits to command to execute it  (go to change status port)
     *  APP_DRIVER_RESUME_ONLY  - Device only read status in loop running on port. (check status port)
     *  
     * @param integer 0 = APP_DRIVER_EXECUTE_RESUME , 1 = APP_DRIVER_EXECUTE_ONLY, 2 = APP_DRIVER_RESUME_ONLY
     */
    void setOperationType(int type); 
    
    /**
     * Get device ID
     * @return int
     */  
    int getIhDriverId();   
    
    /**
     * set device ID
     * @param ih_driver_id
     */    
    void setIhDriverId(int ih_driver_id);   
    
    /**
     * Return the name of device
     * @param out pointer to buffer
     * @return int strlen(name)
     */
    int getName(char* out);  
    
    /**
     * Return the address rs485 of device
     * @param out
     * @return int strlen(address);
     */
    int getAddress(char* out);       
 
    /**
     * 
     * @return 
     */
    char* getAddressPtr(); 
    
    /**
     * How long port have to wait before read
     * @return 
     */
    int getDelayBeforeRead();     
    
    /**
     * 
     * @param delay
     */
    void setDelayBeforeRead(int delay);
    /**
     * Return handle to gxhCore - gxh_scope
     * @return gxh_scope* 
     */
    gxh_scope* getAppHandle();
    
    /**
     * Return handle to port
     * @return app_port*
     */
    app_port* getPortHandle();
};


extern "C" app_driver* get_moduleDriver(gxh_scope* appHomeHandle, app_port* appPortHandle,int ih_driver_id,const char*, const char*, int delay_before_read_ms );


#endif	/* APP_DRIVER_H */

