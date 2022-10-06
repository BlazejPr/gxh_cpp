/* 
 * File:   app_module_provent.h
 * Author: Blazej Kita
 *
 * Created on 07 czerwiec 2018, 10:40
 */

#include "app_module.h"

#ifndef APP_MODULE_PROVENT_H
#define	APP_MODULE_PROVENT_H


struct proventData
{
      int ih_driver_id;
      char response[255];
      char stan; // 0 - stan nieznany, 1 - włącz, 2 - wyłącz..
      int paramInt; //bieg wentylacji i inne podobne..
      
      bool wietrzenie;
      bool stan_filtrow;
      
      bool stan_wentylacji;
      bool stan_nawiewu;
      bool stan_wywiewu;
      bool stan_bypassu;
      unsigned short bieg_wentylacji;
      unsigned short tryb_pracy_centrali;
      unsigned short ustawiony_rodzaj_wentylacji;
      unsigned short tryb_pracy_wentylacji;
      unsigned short czas_wietrzenia;
      unsigned short temperatura_bypass;
      
      int gxhHandle;
      bool retSuccess;
};

class app_module_provent : public app_module{
public:
             app_module_provent(gxh_scope* appHomeHandle ); 
             virtual ~app_module_provent();       
   
               /**
              * Execute interrupt from GXHSystem
              * @param inMsg
              * @param outMsg
              * @return 
              */
             bool execute(gxh_message* inMsg, gxh_message* outMsg);
             
             /**
              * Method is call when GXHSystem is going to delete object from memory. Method call from clientThread
              */
             void forceCancel();
             
             /**
              * Synchronize, method call from serviceThread
              */
             void forceSynchronize();
             void onClick(int ih_driver_port_id, int listener_status);
 
 
};


extern "C" app_module* get_module(gxh_scope* appHomeHandle);

#endif	/* APP_MODULE_PROVENT_H */


