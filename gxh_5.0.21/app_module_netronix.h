/* 
 * File:   app_module_netronix.h
 * Author: Blazej Kita
 *
 * Created on 30 lipiec 2018, 14:31
 */

#include "app_module.h"

#ifndef APP_MODULE_NETRONIX_H
#define	APP_MODULE_NETRONIX_H

#define APP_DRIVER_INTERRUPT 610
#define APP_DRIVER_READ_ACTIVE 611
#define APP_DRIVER_READ_NOTACTIVE 612
#define APP_DRIVER_READ_CARD 100

#define MAX_CARDS_BUFFER 500
#define MAX_CARD_DRIVER_BUFF 1000


struct card_read_h
{
    std::string query;
    bool toSave; 
};

#define MAX_HISTORY_SYNCH 20

 

class app_module_netronix : public app_module{
public:
              app_module_netronix(gxh_scope* appHomeHandle); 
              virtual ~app_module_netronix();       
   
    
                /**
              * Execute interrupt from GXHSystem
              * @param inMsg
              * @param outMsg
              * @return 
              */
             bool execute(gxh_message* inMsg, gxh_message* outMsg);
             
             /**
              * Method is call when GXHSystem is going to delete object from memory
              */
             void forceCancel();      
             void forceSynchronize();  
             void onClick(int ih_driver_port_id, int listener_status);
 
    
 private: 
            card_read_h last_readed_card[MAX_HISTORY_SYNCH];
            
     
            ih_nfc_card cards[MAX_CARDS_BUFFER];
            ih_nfc_card_driver card_drivers[MAX_CARD_DRIVER_BUFF];
     
            int cards_count;
            int card_driver_count;
     
            bool readCard(gxh_message* inMsg,gxh_message* message_ret) ;
            bool updateAddress(gxh_message* inMsg,gxh_message* message_ret) ;
       
   
};



extern "C" app_module* get_module(gxh_scope* appHomeHandle);

#endif	/* APP_MODULE_NETRONIX_H */


