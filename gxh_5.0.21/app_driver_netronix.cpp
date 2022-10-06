/* 
 * File:   app_driver_netronix.cpp
 * Author: Blazej Kita
 * 
 * Created on 25 styczeń 2019, 14:02
 */

#include "app_driver_netronix.h"


app_driver* get_module(gxh_scope* appHomeHandle, app_port* appPortHandle,int ih_driver_id, const char* name, const char* address, int delay_before_read_ms)
{         
    return new app_driver_netronix(appHomeHandle, appPortHandle, ih_driver_id, name,address,delay_before_read_ms);
}
 
//-------------------------------------------------------------------------------


app_driver_netronix::app_driver_netronix(gxh_scope* appHomeHandle, app_port* appPortHandle,int ih_driver_id, const char* name, const char* address, int delay_before_read_ms) : app_driver(appHomeHandle, appPortHandle,ih_driver_id, name,address, delay_before_read_ms)
{
    this->getAppHandle()->show_log_driver("app_driver_netronix","app_driver_netronix","Start" ,0);   
       
       
    this->setDelayBeforeRead( 70); //wati 70ms..for response
    
    int tmpD = 0;
    ih_driver* iD = this->getAppHandle()->getDrivers(&tmpD);
    for(int x=0; x<tmpD;x++)
    {
        if(iD[x].id == this->getIhDriverId() )
        {
            memcpy(&this->driver, &iD[x], sizeof(ih_driver) );
            
            std::string msg = "Load ";
            msg.append(this->driver.name);
            msg.append(" | ");
            msg.append(this->driver.name_firm);
            msg.append(", ");
            msg.append(this->driver.author);
            msg.append(" on address ");
            msg.append(this->driver.address);
            
            this->getAppHandle()->show_log_driver( "app_driver_netronix","app_driver_netronix",msg.c_str(),0 );
            
            break;
        }
    }
     
    
     int tmpC = 0;        
    ih_sl_driver* slD = this->getAppHandle()->getSlDriver(&tmpC);    
    for(int x=0; x<tmpC; x++)
    {        
        if(slD[x].id == this->driver.ih_sl_driver_id)
        {
            memcpy(&this->ihSlDriver, &slD[x], sizeof(ih_sl_driver) );
            
            std::string msg = "Load software for ";
            msg.append(this->driver.name);
            msg.append(" | ");
            msg.append(this->ihSlDriver.name_firm);
            msg.append(", ");
            msg.append(this->ihSlDriver.author);        
            
            this->getAppHandle()->show_log_driver( "app_driver_netronix","app_driver_netronix",msg.c_str(),0 );
            
            break;
        }
    }
    
    

    
}

//-------------------------------------------------------------------------------

app_driver_netronix::~app_driver_netronix() {  
    
    char name[50];
    this->getName((char*)name );
    
    std::string msg = std::string(name);
    msg.append(" has been deleted");
 
 
        
    this->getAppHandle()->show_log_driver("app_driver_netronix","app_driver_netronix",msg.c_str(),0);   
}

//-------------------------------------------------------------------------------

void app_driver_netronix::execute(gxh_message* command)
{
  
    gxh_message message;
    memcpy(&message, command, sizeof(gxh_message) );
    
    
    if(message.func_internal_number == IH_UPDATE_ADDRESS)
    { 
      this->updateAddress(&message);  
      return; 
    } 
    
    if(message.func_internal_number == IH_UPDATE_SPEED)
    { 
      this->updateSpeed(&message);  
      return; 
    } 
    
    if(message.func_internal_number == IH_UPDATE_DATA_BIT)
    { 
      this->updateDataBit(&message);  
      return; 
    } 
    
    if(message.func_internal_number == IH_UPDATE_STOP_BIT)
    { 
      this->updateStopBit(&message);  
      return; 
    } 
    
    if(message.func_internal_number == IH_UPDATE_PARITY)
    { 
      this->updateParity(&message);  
      return; 
    } 
    
    
    this->getAppHandle()->show_log_driver ("app_driver_netronix","execute","Unexpected command",0);   
   
}

//-------------------------------------------------------------------------------

void app_driver_netronix::updateAddress(gxh_message* command)
{
    
    
       
    system_driver_command driverParam;
    memcpy(&driverParam, command->dataStruct, sizeof(system_driver_command) );
    
    this->getAppHandle()->show_log_driver ("app_driver_netronix","updateAddress", driverParam.param ,0);  
    
    
    unsigned short newAdr =  (unsigned short)gxh_StringToInt( std::string( driverParam.param) ) ;
    
    std::cout<<"\n\nAdress: "<<newAdr<<" | \n";
    
    if( newAdr > 9)
    {
           this->getAppHandle()->show_error("app_driver_netronix","updateAddress","bad address max. 99",4);
           *driverParam.response = true;
           *driverParam.result = false;
           return;
    }
    
    char buffer[300];     
    char rBuffer[300];  
    
    
            memset(buffer,0,300);
            memset(rBuffer,0,300);  

            app_modbus_rtu rtu;
            rtu.setSlaveAddress(  (char)gxh_StringToInt( std::string( this->getAddressPtr() ) ) );            
           // rtu.setSlaveAddress(  1 );            //adres domyślny urzadzenia..
            rtu.setFunctionForceSingleRegister(); // odczyta rejestr
            rtu.setDataAddress( 1050 );      //start adres...
            rtu.setRegisterValue( newAdr  );
            int size = rtu.get(buffer);          

            this->getPortHandle()->flush();      
            this->getPortHandle()->writeData(buffer,size);  // pisz do portu

            usleep (1000* this->getDelayBeforeRead()   );       

            int r = this->getPortHandle()->readData(rBuffer, 300); 

            if( r > 7 )    
            {              
                for(int m=0; m<r; m++)
                {            
                   printf("%02x",   (int)((unsigned char)rBuffer[m] ) );    
                  std::cout<<" | ";
                }         
                std::cout<<"\n";  
                
                if( (unsigned short)rBuffer[5] == newAdr )
                {
                  *driverParam.response = true;
                  *driverParam.result = true;
                
                  this->getAppHandle()->show_log_driver ("app_driver_netronix","updateAddress", "Adres został ustawiony" ,0);  
                  return;
                }
            }
            
           // std::cout<<"\nUstawinie adresu "<< driverParam.param<<" | "<< gxh_StringToInt( std::string( driverParam.param) ) <<" r "<<r;
    
            *driverParam.response = true;
            *driverParam.result = false;
            this->getAppHandle()->show_log_driver ("app_driver_netronix","updateAddress", "Adres nie został ustawiony" ,0);  
}

//-------------------------------------------------------------------------------

void app_driver_netronix::updateSpeed(gxh_message* command)
{
    this->getAppHandle()->show_log_driver ("app_driver_netronix","updateSpeed",".....??",0);    
}

//-------------------------------------------------------------------------------

void app_driver_netronix::updateDataBit(gxh_message* command)
{
    this->getAppHandle()->show_log_driver ("app_driver_netronix","updateDataBit",".....??",0);    
}

//-------------------------------------------------------------------------------

void app_driver_netronix::updateStopBit(gxh_message* command)
{
    this->getAppHandle()->show_log_driver ("app_driver_netronix","updateStopBit",".....??",0);    
}

//-------------------------------------------------------------------------------

void app_driver_netronix::updateParity(gxh_message* command)
{
    this->getAppHandle()->show_log_driver ("app_driver_netronix","updateParity",".....??",0);    
}

//-------------------------------------------------------------------------------

bool app_driver_netronix::resume()
{
    this->getAppHandle()->show_log_driver("app_driver_netronix","resume","OK" ,25);   
  
    
    char buffer[300];     
    char rBuffer[300];  
    
    
    char isNew = -1;
    
    //sprawdz czy odczytano nowy kod..
    memset(buffer,0,300);
    memset(rBuffer,0,300);  

    app_modbus_rtu rtu1;
    rtu1.setSlaveAddress(  (char)gxh_StringToInt( this->getAddressPtr() ) );            
    rtu1.setFunctionReadCoil(); // odczyta rejestr
    rtu1.setDataAddress( 1004 );      //start adres...    
    rtu1.setNumberOfCoils(1); //one ceil
    int size1 = rtu1.get(buffer);          

    this->getPortHandle()->flush();      
    this->getPortHandle()->writeData(buffer,size1);  // pisz do portu
    
   
    usleep (1000* this->getDelayBeforeRead()   );        
  

 
    
    //int r1 = this->getPortHandle()->readData(rBuffer, 300); 
    int r1 = this->getPortHandle()->readData(rBuffer, 300); 
    
   
    
 
         //  std::cout<<"\nR: "<<r1<<"\n";
         //  for(int m=0; m<r1; m++)
             // {            
             //     printf("%02x",   (int)((unsigned char)rBuffer[m] ) );    
             //     std::cout<<" | ";
             // }         
             // std::cout<<"\n";  
         
    if(r1 > 4)
    { 
        
           isNew = rBuffer[3];  // 0 || 1
         //  std::cout<<"\nS, "<<r1<<": ";
           //for(int m=0; m<r1; m++)
             //{            
               //   printf("%02x",   (int)((unsigned char)rBuffer[m] ) );    
                //  std::cout<<" | ";
             //  }         
            //  std::cout<<"\n";  
    }
    
   
           
              
    if(isNew == -1 )  this->getAppHandle()->show_log_driver ("app_driver_netronix","readCArd","notconnect",0);  
    if( isNew != 1) return false; //nic nowego..
    
    
    
       char code[50];   
       code[0] = 0;
       code[1] = 0;
       code[2] = 0;
       code[3] = 0;
       code[8] = 0;


        for(int w=0; w< 4; w++)
        {

                

            memset(buffer,0,300);
            memset(rBuffer,0,300);  

            app_modbus_rtu rtu;
            rtu.setSlaveAddress(  (char)gxh_StringToInt( this->getAddressPtr() ) );            
            rtu.setFunctionInputHoldingRegister(); // odczyta rejestr
            rtu.setDataAddress( 1000 + w );      //start adres...
            rtu.setNumberOfRegister( 1 );  //1 rejestr = 2 bajty
            int size = rtu.get(buffer);          

            this->getPortHandle()->flush();      
            this->getPortHandle()->writeData(buffer,size);  // pisz do portu

            usleep (1000* this->getDelayBeforeRead()   );       

            //this->getPortHandle()->readData(rBuffer, 1); 
 
            int r = this->getPortHandle()->readData(rBuffer, 300); 

            if( r == 7 )    
            {  
                std::cout<<"P"<<w<<": ";
                for(int m=0; m<r; m++)
                {            
                     printf("%02x",   (int)((unsigned char)rBuffer[m] ) );    
                     std::cout<<" | ";
                }         
                std::cout<<"\n";        
                if(w == 0) code[ 7 ] = rBuffer[ 4 ];
                if(w == 1) code[ 6 ] = rBuffer[ 4 ];
                if(w == 2) code[ 5 ] = rBuffer[ 4 ];
                if(w == 3) code[ 4 ] = rBuffer[ 4 ];
            }else
            {
              this->getAppHandle()->show_log_driver("app_driver_netronix","odczyt","err" ,25);                
              return false;
            }

        


        }
     
       
        //wyslij informacje o odczytaniu karty do Core'a aplikacji...
               //lets interrupt...
        
          //          00 00 b6 30 d4 32
        //usb   00 00 00 00 B6 30 D4 32
       //       00 00 00 00 b6 30 d4 32
       
       
              char hexCode[50];
              int sizeC = 0;
              
              this->convertToHex(code, hexCode, 8, &sizeC );
              
             
              
              
              ih_card_data_reader dataR;
              dataR.ih_driver_id =  this->getIhDriverId();
              memcpy(dataR.data, hexCode, sizeC);
              dataR.sizeData = sizeC;
              dataR.timestamp = gxh_getTimestamp();
                      
              
              gxh_message gxhMessage;
              gxhMessage.gxh_handle = NULL_DRIVER; //don't response to me.....
              gxhMessage.func_number = APP_DRIVER_INTERRUPT; //send to module netronix
              gxhMessage.func_internal_number = APP_DRIVER_READ_CARD;  
              memcpy(gxhMessage.dataStruct, &dataR,  sizeof(ih_card_data_reader) ); //put card code
              gxhMessage.sizeDataStruct = sizeof(ih_card_data_reader)  ;
              gxhMessage.system_code = GXH_MESSAGE_INTERRUPT;
              gxhMessage.check_timeout = 0;
              
                                       
              bool res = this->getAppHandle()->sendGXHMessage(&gxhMessage); //max priority              
              
              if(!res)
              {
                  this->getAppHandle()->show_error("app_driver_netronix","resume","Cannot add message",4);
              }
              
              //end interrupt
       
        
       
        //--------------
        for(int m=0; m<8; m++)
        {                        
           if( code[m]  == 0) code[m] = '0';
        }
        
        
        std::string cardC = "Odczytano kart zblizeniowa! ";
        cardC.append(code);
        
        
        this->getAppHandle()->show_log_driver( "app_driver_netronix","app_driver_netronix", cardC.c_str() ,0 );
        
        
            //oznacz karte jako odczytaną.. 
       
            memset(buffer,0,300);
            memset(rBuffer,0,300);  

            app_modbus_rtu rtu2;
            rtu2.setSlaveAddress(  (char)gxh_StringToInt( this->getAddressPtr() ) );            
            rtu2.setFunctionForceSingleCoil(); // zapisz
            rtu2.setDataAddress( 1004 );      //start adres...    
            rtu2.setCoilStatus(false);
            int size2 = rtu2.get(buffer);          

            this->getPortHandle()->flush();      
            this->getPortHandle()->writeData(buffer,size2);  // pisz do portu 
            
            usleep (1000* this->getDelayBeforeRead()   );       

            int r2 = this->getPortHandle()->readData(rBuffer, 300); 

            //if( r2 == 7 )    
            //{                             
            //}
           
     
}

//-------------------------------------------------------------------------------

 void app_driver_netronix::convertToHex(char* in, char* out, int sizeIn, int* sizeOut)
 { 
     std::string hex = "";
     
     for(int m=0; m<sizeIn; m++)
     {
         //std::cout<<"\nL:"<<m<<": "<<(int)in[m];
         std::stringstream sstream;
         sstream   << std::hex << (int)in[m];
         
         std::string sHex = sstream.str();
         if(sHex.length() == 1) hex.append("0");
         hex.append(sHex);                  
     }
     
    
      char* tmp = (char*)hex.c_str();
     
      for(int m=0; m<hex.length(); m++)
         {
                  switch(tmp[m])
                  {
                      case 'a': { tmp[m] = 'A'; break;}
                      case 'b': { tmp[m] = 'B'; break;}
                      case 'c': { tmp[m] = 'C'; break;}
                      case 'd': { tmp[m] = 'D'; break;}
                      case 'e': { tmp[m] = 'E'; break;}
                      case 'f': { tmp[m] = 'F'; break;}
                  }
         }
     
     
     *sizeOut = hex.length();
     memcpy(out, hex.c_str(), hex.length() );
 }

//-------------------------------------------------------------------------------