/* 
 * File:   app_driver_provent.h
 * Author: Blazej Kita
 *
 * Created on 06 czerwiec 2018, 13:05
 */

#include "app_driver.h"

#ifndef APP_DRIVER_PROVENT_H
#define	APP_DRIVER_PROVENT_H

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

class app_driver_provent : public app_driver{
public:
    app_driver_provent(gxh_scope* appHomeHandle, app_port* appPortHandle,int ih_driver_id,const char* name, const char* address, int delay_before_read_ms);
    virtual ~app_driver_provent();
 
    void execute(gxh_message* command);
    bool resume();
    
   
   
  private:
       
    ih_sl_driver ihSlDriver;
    int timer;           
        
    void wietrzenie(bool status,proventData* wskData );
    void bieg_wentylacji(int nr_bieg, proventData* wskData );
    void tryb_pracy_centrali(int tryb,proventData* wskData );
    void rodzaj_wentylacji(int rodzaj, proventData* wskData);
    void bypass(int status, proventData* wskData);
    void tryb_wentylacji(int tryb, proventData* wskData);
        
};


extern "C" app_driver* get_module(gxh_scope* appHomeHandle, app_port* appPortHandle,int ih_driver_id, const char*, const char*, int delay_before_read_ms);


#endif	/* APP_DRIVER_PROVENT_H */


