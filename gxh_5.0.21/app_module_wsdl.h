/* 
 * File:   app_module_wsdl.h
 * Author: Blazej Kita
 *
 * Created on 22 maj 2018, 14:31
 */

#include "app_module.h"

#ifndef APP_MODULE_WSDL_H
#define	APP_MODULE_WSDL_H

 

#define MAX_USER_BUFFER 500






class app_module_wsdl : public app_module{
public:
              app_module_wsdl(gxh_scope* appHomeHandle); 
              virtual ~app_module_wsdl();       
   
    
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
     
     bool firstSynchronize;
     
     user_gxh usersSSID[MAX_USER_BUFFER]; //
          
   
        
     void parseHeader(gxh_message* msgIn,char headers[20][1024], char soapAction[1024], int* numHeader,int* sizeSopaEnv, char soapEnvelop[1024 * 100],   char gxhVersion[40], char gxhPublicKey[40] );
     int getParam(const char* name, char* out, int size,  char soapEnvelop[1024 * 100]);
    

     bool logoutMsg(gxh_message* msgIn,gxh_message* message);  
     bool errorMsg(gxh_message* msgIn,gxh_message* message); 
     bool wrong_param(gxh_message* msgIn,gxh_message* message); 
     bool wrong_version(gxh_message* msgIn,gxh_message* message, const char* version); 
     bool wrong_apiKey(gxh_message* msgIn,gxh_message* message); 
     
     bool build(gxh_message* msgIn, gxh_message* message_ret, const char* response);
     
     void loadSessionToCache();
     void loadPrGroup();
     
     bool alowSector(int ih_sector_id, int px_group_id);
     bool alowZone(int ih_sector_id, int px_group_id);
     
     bool setDebugLevel(gxh_message* msgIn, gxh_message* message_ret,char* level);
     
     bool goLogin(gxh_message* msgIn,gxh_message* message_ret, char* username, char* pass, char* type);
     
     bool goRefresh(gxh_message* msgIn, gxh_message* message_ret,int px_user_id,int px_group_id,  double lat, double lng, const char* wifi, const char* bluetooth );
     
     bool getVerApp(gxh_message* msgIn,gxh_message* message_ret, int px_user_id, int px_group_id );
     
     bool getInfo(gxh_message* msgIn,gxh_message* message_ret, int px_user_id, int px_group_id );
     bool getShort(gxh_message* msgIn,gxh_message* message_ret, int px_user_id, int px_group_id );     
     bool addShort(gxh_message* msgIn,gxh_message* message_ret,  int px_group_id, int funcCode, int resourceId, const char* param1, const char* param2 );     
     bool removeShort(gxh_message* msgIn,gxh_message* message_ret,  int px_group_id, int funcCode, int resourceId  );     
     
     
     
     bool getAllHistory(gxh_message* msgIn,gxh_message* message_ret, int px_user_id, int px_group_id );
     
     bool getAllUser(gxh_message* msgIn,gxh_message* message_ret);
     bool getAllHome(gxh_message* msgIn,gxh_message* message_ret);
     bool getAllCamera(int px_user_id,int px_group_id,gxh_message* msgIn,gxh_message* message_ret);
     
     bool reloadModule(gxh_message* msgIn,gxh_message* message); 
     bool goReloadPorts(gxh_message* msgIn, gxh_message* message_ret);
     bool getVersion(gxh_message* msgIn, gxh_message* message_ret);
     bool goClose(gxh_message* msgIn, gxh_message* message_ret);
     bool getAllSector(int px_user_id,int px_group_id ,gxh_message* msgIn, gxh_message* message_ret);
     bool getAllZone(int px_user_id, int px_group_id ,gxh_message* msgIn, gxh_message* message_ret, int numTop);
     bool getAllLight(int px_user_id, int px_group_id ,gxh_message* msgIn, gxh_message* message_ret, int idTop, int idTopSector );
     bool getAllOuts(int px_user_id,int px_group_id ,gxh_message* msgIn, gxh_message* message_ret, int idTop, int idTopSector );
     bool getAllBlind(int px_user_id,int px_group_id, gxh_message* msgIn, gxh_message* message_ret, int idTop, int idTopSector );
     bool getAllGate(int px_user_id,int px_group_id,gxh_message* msgIn, gxh_message* message_ret, int idTop, int idTopSector  );
     bool getAllDoor(int px_user_id, int px_group_id,gxh_message* msgIn, gxh_message* message_ret, int idTop, int idTopSector  );
     bool getAllSensor(int px_user_id,int px_group_id ,gxh_message* msgIn, gxh_message* message_ret, int idTop, int idTopSector  );
     bool getAllTemp(gxh_message* msgIn, gxh_message* message_ret, int idTop ,int idTopSector  );
     bool getAllDriver(int px_user_id, int px_group_id,gxh_message* msgIn, gxh_message* message_ret);
     bool getAllDriverApi(gxh_message* msgIn,gxh_message* message_ret, int idDriver );
     bool getAllDriverVar (gxh_message* msgIn,gxh_message* message_ret, int idDriver );  
     
     bool getAllAlarmZone(int px_user_id, int px_group_id,gxh_message* msgIn, gxh_message* message_ret);
     
     bool exeSimplyFunction(gxh_message* msgIn,gxh_message* message_ret, int numFunction, int numResource, int px_group_id);
     bool exeParamFunction(gxh_message* msgIn, gxh_message* message_ret, int numFunction, int numResource, char* param1, char* param2, int px_group_id);
     bool exeDriverFunction(gxh_message* msgIn, gxh_message* message_ret, int idDriver, int numFunctionModule, char* param );
     
     bool setAddress(gxh_message* msgIn, gxh_message* message_ret,char* ih_driver_id, char* parity, char* dataBit, char* stopBit, char* speed);
     bool setClick(gxh_message* msgIn, gxh_message* message_ret,char* ih_driver_port_id);
     
     bool getAllScene(gxh_message* msgIn, gxh_message* message_ret );
     bool addScene(gxh_message* msgIn, gxh_message* message_ret, char* name );
     bool removeScene(gxh_message* msgIn, gxh_message* message_ret, int pId );
 
     bool getAllSceneAction(gxh_message* msgIn, gxh_message* message_ret, int idTop );
     bool addSceneAction(gxh_message* msgIn, gxh_message* message_ret, int idScene, int funcCode, int resourceId, char* param1, char* param2 );
     bool removeActionScene(gxh_message* msgIn,  gxh_message* message_ret, int pId );
     bool getAllFunction(gxh_message* msgIn, gxh_message* message_ret );
     
     bool addTask(gxh_message* msgIn, gxh_message* message_ret, int const_function_symbol, int resource_id, const char* time, int every_week, int mon, int tue, int wed, int thu, int fri, int sat, int sun, const char* param1, const char* param2 );
     bool getAllTimer(gxh_message* msgIn, gxh_message* message_ret  );
     bool removeTask(gxh_message* msgIn, gxh_message* message_ret, int id  );
     
     bool executeSys(gxh_message* msgIn, gxh_message* message_ret, int id  );
     
     bool setMsgStatus(gxh_message* msgIn, gxh_message* message_ret, int notice_id, int status  );
     
     
};



extern "C" app_module* get_module(gxh_scope* appHomeHandle);

#endif	/* APP_MODULE_WSDL_H */


