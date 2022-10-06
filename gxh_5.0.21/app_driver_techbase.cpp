/* 
 * File:   app_driver_techbase.cpp
 * Author: Blazej Kita
 * 
 * Created on 14 wrzesień 2016, 15:05
 */

#include "app_driver_techbase.h"


app_driver* get_module(gxh_scope* appHomeHandle, app_port* appPortHandle,int ih_driver_id, const char* name, const char* address, int delay_before_read_ms)
{         
    return new app_driver_techbase(appHomeHandle, appPortHandle, ih_driver_id, name,address,delay_before_read_ms);
}
 
//-------------------------------------------------------------------------------


app_driver_techbase::app_driver_techbase(gxh_scope* appHomeHandle, app_port* appPortHandle,int ih_driver_id, const char* name, const char* address, int delay_before_read_ms) : app_driver(appHomeHandle, appPortHandle,ih_driver_id, name,address, delay_before_read_ms){
   
   // std::string msg = this->name;
   // msg.append(" has been loaded..");
        
   // this->getAppHandle()->show_log_driver( "app_driver_techbase","app_driver_techbase",msg.c_str(),0 );
 
               
    this->num_ports = 0; 
    int tmpP = 0;
    ih_driver_port* portsArray = this->getAppHandle()->getPortsDriver(&tmpP);
    
    for(int x=0; x<tmpP; x++)
    {
        if(portsArray[x].ih_driver_id == this->getIhDriverId() )
        {
           memcpy( &this->ports[this->num_ports], &portsArray[x], sizeof(ih_driver_port) ) ;
           this->num_ports++;
        }
    }
    
    
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
            
            this->getAppHandle()->show_log_driver( "app_driver_techbase","app_driver_techbase",msg.c_str(),0 );
            
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
            
            this->getAppHandle()->show_log_driver( "app_driver_techbase","app_driver_techbase",msg.c_str(),0 );
            
            break;
        }
    }
    
           
     
    this->di_actual_state = 0;
    this->di_last_state   = 0;
       
    
    
    this->history_ports = new history_port[this->num_ports];
    
    for(int w=0; w<this->num_ports; w++)
    {
        for(int w2=0; w2<6; w2++)
        {
          this->history_ports[w].info[w2].status = false;
          this->history_ports[w].info[w2].time   = 0;
          this->history_ports[w].info[w2].option = 0;
        }        
    }
 
    
    this->licznik = 0;
    this->stan = false;
    
    this->statusCache = false; //brak zapamietanego aktualnego stanu portów..
    memset(this->statusBuffer,0,12);
  
    
   //.. 
   this->setOperationType(APP_DRIVER_EXECUTE_RESUME);
    
        
}


//-------------------------------------------------------------------------------
 

app_driver_techbase::~app_driver_techbase() {  
    
    char name[100];
    this->getName((char*)name);
    
    std::string msg = std::string(name);
    msg.append(" has been deleted");
     
    delete[] this->history_ports;
 
    
    
    this->getAppHandle()->show_log_driver("app_driver_techbase","app_driver_techbase",msg.c_str(),0);
   
}


//-------------------------------------------------------------------------------


void app_driver_techbase::execute(gxh_message* command)
{
  
    gxh_message message;
    memcpy(&message, command, sizeof(gxh_message) );
   
    
    struct timeval  tv;
    gettimeofday(&tv, NULL);  
    double time_in_mill =  (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 ;
    
    message.time_ms = time_in_mill;// czas zgloszenia do sterownika....
               
//std::cout<<"\n\n\nChange status for port............................................."    ;
    
    if(message.func_internal_number == IH_SET_PORT_ON || message.func_internal_number == IH_SET_PORT_OFF ||  message.func_internal_number == IH_SET_PORT_CLICK || message.func_internal_number == IH_SET_PORT_DOUBLE_CLICK)  
    {
         this->getAppHandle()->show_log_driver("app_driver_techbase","execute","change port status",11);
        
        if( gxh_compare( (const char*)this->ihSlDriver.name,"I-7061D") == 0 )  this->setPortStatus7061D(&message);                                  
        if( gxh_compare( (const char*)this->ihSlDriver.name,"I-7055D") == 0 )  this->setPortStatus7055D(&message);                                  
        return; 
    }
    
    
    
    if(message.func_internal_number == IH_UPDATE_ADDRESS)
    {         
       
      this->updateAddress(&message);  
      return; 
    } 
    
  
    this->getAppHandle()->show_log_driver ("app_driver_techbase","execute","Unexpected command",0);              
}


//-------------------------------------------------------------------------------


bool app_driver_techbase::resume()
{ 
   
    //--------------------------------------------------------------------------
    //---------------------- -----------------------------------
    
    if( gxh_compare( this->ihSlDriver.name, "I-7051D" ) ==0)
    {
        return this->checkPortStatus7051D();        
    }
    
    
    if( gxh_compare( this->ihSlDriver.name, "I-7055D" ) ==0)
    {
        this->checkPortStatus7055D();
        return false;
    }
    
    
    return false; //pomin sprawdzenie kolejnego urzadzenie....
   
}

// -----------------------------------------------------------------------------

void app_driver_techbase::updateAddress(gxh_message* command)
{
     system_data_driver_param driverParam;
     memcpy(&driverParam, command->dataStruct, sizeof( system_data_driver_param ) );

     
       int ih_driver_id = driverParam.ih_driver_id;
       
        
        
       std::string msg = " ------------------------- set address for driver ";                   
       msg.append( gxh_IntToString(ih_driver_id) );        
       msg.append( ", " );        
       msg.append("8"); 
       msg.append( ", " );      
       msg.append(  "n" );       
       msg.append( "," );       
       msg.append( "1");       
       msg.append( " " );  
       msg.append( "19200"  );// ??
 
       
     
        
        
       this->getAppHandle()->show_log_driver("app_driver_techbase","execute",msg.c_str(),0); 
       
       //ustawienia domyślne dla modułu w trybie ini..
       this->getPortHandle()->changePortParameter('n','1',9600,'8');          
      
       
       
       this->getPortHandle()->flush();
       
       //tu zmien parametry
       
       //-----------------------------------------------------------------------
   //   const char* adr = this->address.c_str();
       char adrr[4];
       this->getAddress((char*)adrr);
       
       char buffer[15];
       buffer[0] = '%';
       buffer[1] = '0'; //adr[0];  //old address  w trybie ini adres 0
       buffer[2] = '0'; //adr[1];  //old address
       buffer[3] = adrr[0]; //new address
       buffer[4] = adrr[1];  //new address
       buffer[5] = '4'; //stała dla dio 'Type code, should be 40 for DIO module..'
       buffer[6] = '0';
      
       int defSpeed = 19200;
       
       defSpeed = driverParam.speed;
       
       if(defSpeed== 1200)
       {
         buffer[7] = '0';   //new bound rate
         buffer[8] = '3';   // new bound rate
         this->getAppHandle()->show_log_driver("app_driver_techbase","execute", "Speed selected 1200" ,0); 
       }
       
       if(defSpeed== 2400)
       {
         buffer[7] = '0';   //new bound rate
         buffer[8] = '4';   // new bound rate         
         this->getAppHandle()->show_log_driver("app_driver_techbase","execute", "Speed selected 2400" ,0); 
       }
       
       if(defSpeed == 4800)
       {
         buffer[7] = '0';   //new bound rate
         buffer[8] = '5';   // new bound rate
         
         this->getAppHandle()->show_log_driver("app_driver_techbase","execute", "Speed selected 4800" ,0); 
       }
       
       if(defSpeed == 9600)
       {
         buffer[7] = '0';   //new bound rate
         buffer[8] = '6';   // new bound rate
         
         this->getAppHandle()->show_log_driver("app_driver_techbase","execute", "Speed selected 9600" ,0); 
       }
       
       if(defSpeed == 19200)
       {
         buffer[7] = '0';   //new bound rate
         buffer[8] = '7';   // new bound rate
         
         this->getAppHandle()->show_log_driver("app_driver_techbase","execute", "Speed selected 19200" ,0); 
       }
       
       if(defSpeed == 38400)
       {
         buffer[7] = '0';   //new bound rate
         buffer[8] = '8';   // new bound rate
         
         this->getAppHandle()->show_log_driver("app_driver_techbase","execute", "Speed selected 38400" ,0); 
       }
       
       if(defSpeed == 57600)
       {
         buffer[7] = '0';   //new bound rate
         buffer[8] = '9';   // new bound rate
         
        this->getAppHandle()->show_log_driver("app_driver_techbase","execute", "Speed selected 57600" ,0); 
       }
       
       if(defSpeed == 115200)
       {
         buffer[7] = '0';   //new bound rate
         buffer[8] = 'A';   // new bound rate
         
         this->getAppHandle()->show_log_driver("app_driver_techbase","execute", "Speed selected 115200" ,0); 
       }
       
       buffer[9]  = '0';   //counter checksum
       buffer[10] = '0';   // counter checksum
       
       buffer[9]  = '4';   //enable check sum..
       buffer[10] = '0';   //enable checksum..
       
       
       buffer[11] =  13;   // cr
       
      // %
      // 00 //adr
      // 10 //new adr
      // 40 //wartosc staloa
      // 0A //115200
      // 40 //enable check sum..
       
       this->getAppHandle()->show_log_driver("app_driver_techbase","execute",buffer,0);  
       
      this->getPortHandle()->flush();  
       this->getPortHandle()->writeData(buffer,12); 
    
       usleep (1000* 400);  //100 mili sekund...    
    
       char rBuffer[15];
       memset(rBuffer,0,15);
       int r = this->getPortHandle() ->readData(rBuffer, 15);   
    
     
       if(r>2)
       {
           this->getAppHandle()->show_log_driver("app_driver_techbase","execute","Odczytano..................................",0);  
           this->getAppHandle()->show_log_driver("app_driver_techbase","execute, Odpowiedz modulu", rBuffer,0 );   
           std::cout<<"\nOdpowiedz std: "<<r<<", "<<rBuffer;

           char addr[5];
           this->getAddress((char*)addr );
           
           if(rBuffer[0] == '!' && rBuffer[1] ==addr[0] && rBuffer[2] == addr[1]  )
           {
                 this->getAppHandle()->show_log_driver("app_driver_techbase","execute", "Adres zostal zmieniony!!!!",0 );   
                 *((bool*)driverParam.response) = true;
                 *((bool*)driverParam.result) = true;
           }
           
       }else
       {
           this->getAppHandle()->show_error("app_driver_techbase","execute","brak odpowiedzi modulu na zmiane adresu!",0);
           
                *((bool*)driverParam.response) = true;
                *((bool*)driverParam.result) = false;
       }
       
       //-----------------------------------------------------------------------
       
       
       this->getPortHandle()->setDefaultPortParameter();
       
       
        
       return;
}



//-------------------------------------------------------------------------------

int app_driver_techbase::getPortStatus()
{

   this->statusCache = false; // w chwilki obecnej nie korzystaj z cacha poniewaz slabo sie to sprawdza... 
  if(this->statusCache && ( gxh_compare( this->ihSlDriver.name, "I-7061D" ) ==0) )  //aktualny stan portów jest zapamiętamy w cahu..
  {
          char Hex[5];
          Hex[0] = this->statusBuffer[1];
          Hex[1] = this->statusBuffer[2];
          Hex[2] = this->statusBuffer[3];
          Hex[3] = this->statusBuffer[4];
          Hex[4] = 0;
        
          int portVal = this->HexToTwoByte(Hex);          
          return portVal;
  }
    
    
    
    //odczytaj obecny stan portu...
   
  for(int w=0; w<5; w++) 
  {
    
  //  const char* adr = this->address;
    char addr[5];
    this->getAddress((char*)addr );
    
    char buffer[10];
    buffer[0] = '@';
    buffer[1] = addr[0]; // '0';
    buffer[2] = addr[1];  //'1';
    buffer[3] = 13;   
    
    int sum = buffer[0] + buffer[1] + buffer[2];              
    sum = sum & 0x0ff; //zamieniec na ASCII np. 45h  --> 45CR
    
    //Zamien na hex. 
    std::stringstream sstream;
    sstream << std::hex << sum;
    std::string result = sstream.str();   
 
    //zamien na duze znaki...
    char* resultC = (char*)result.c_str();
    
    
    //kopiuj sume kontrolna.... jesli jest potrzebna...
    int inC = 0;
    while(resultC[inC])
    {
        resultC[inC] = std::toupper( resultC[inC] );
        inC++;
    }
     
    for(int i=0; i<inC; i++)
    {
        buffer[i+3] = resultC[i];
    }
    
     buffer[inC+3] = 13; //enter po sumie kontrolnej..
     
 
    
    
  //  this->getAppHandle()->show_log_driver("app_driver_techbase","getPortStatus", (char*)buffer);  
      
    //opróznij buffor..
    this->getPortHandle()->flush();  
   
 
    
    this->getPortHandle()->writeData(buffer,inC+5); 
    
    
   
    
    
    usleep (1000* this->getDelayBeforeRead() );  //100 mili sekund...    
    
    
    char rBuffer[12];
    memset(rBuffer,'\0',12);
    int r = this->getPortHandle()->readData(rBuffer, 10);  // np. >0000FECR
    
      
    if(r > 7) // >0000FE CR  
    {
          this->getAppHandle()->show_log_driver("app_driver_techbase","getPortStatus / odczytano", (char*)rBuffer,5);  
          
          char Hex[5];
          Hex[0] = rBuffer[1];
          Hex[1] = rBuffer[2];
          Hex[2] = rBuffer[3];
          Hex[3] = rBuffer[4];
          Hex[4] = 0;
        
          int portVal = this->HexToTwoByte(Hex);
          
          memset(this->statusBuffer,'\0',12);
          memcpy(this->statusBuffer,rBuffer,12 );
          this->statusCache = true; //teraz bedzie bazować na tej wersji...
          
          return portVal;
          
    }else 
    {
        this->getAppHandle()->show_error("app_driver_techbase","getPortStatus brak odpowiedzi na ", (char*)buffer,0);      
        this->getAppHandle()->show_error("app_driver_techbase","getPortStatus brak odpowiedzi::: ", (char*)rBuffer,0);      
        this->getAppHandle()->show_error("app_driver_techbase","getPortStatus brak odpowiedzi::: ", (char*)(gxh_IntToString(r).c_str()),0);    
    }
     
     
  }
    
    
   return -1;  
    
}

//-------------------------------------------------------------------------------

int app_driver_techbase::HexToTwoByte(char* hex)
{
    int tmp = 0;
    int result = 0;
    
    tmp = this->HexCharToInt(hex[3]);  
    tmp = tmp << 16;    
    result |= tmp;            
 
    
    tmp = this->HexCharToInt(hex[2]);  
    tmp = tmp << 12;    
    result |= tmp;
    
    
    
   //  std::string test = "HextToTwoByte: ";
   // test.append(hex);
   // test.append(" ,");
   // test.append(this->IntToString(tmp));
   // test.append(" ,");
   // test.append(this->IntToString(result));
    
  //  this->show_error("app_home","HexToTwoByte", test.c_str());
    
    
    tmp = this->HexCharToInt(hex[1]);  
    tmp = tmp << 8;    
    result |= tmp;           
    
    
    tmp = this->HexCharToInt(hex[0]);  
    tmp = tmp << 4;
    result |= tmp;
    
    return result;
}

//-------------------------------------------------------------------------------

int app_driver_techbase::HexCharToInt(char hex)
{
    if(hex == '0') return 0;
    if(hex == '1') return 1;
    if(hex == '2') return 2;
    if(hex == '3') return 3;
    if(hex == '4') return 4;
    if(hex == '5') return 5;
    if(hex == '6') return 6;
    if(hex == '7') return 7;
    if(hex == '8') return 8;
    if(hex == '9') return 9;
    if(hex == 'A') return 10;
    if(hex == 'B') return 11;
    if(hex == 'C') return 12;
    if(hex == 'D') return 13;
    if(hex == 'E') return 14;
    if(hex == 'F') return 15;
    
    
    this->getAppHandle()->show_error( "app_driver_techbase","HexCharToInt","Nie mozna zamienic liczby hex ascii na integer!",0);
    return 0; //bez zmian..
}

//-------------------------------------------------------------------------------

 

void app_driver_techbase::setPortStatus7061D(gxh_message* command)
{       
    
        ih_change_port_info portInfo;
        memcpy(&portInfo, command->dataStruct, sizeof(ih_change_port_info) );
        
        bool status = false;
    
        //ustaw stan wysoki..
        if(command->func_internal_number == IH_SET_PORT_ON )
        {
            status = this->sendPortStatus(command, IH_SET_PORT_ON);
        }
        
        //ustaw stan niski
        if(command->func_internal_number== IH_SET_PORT_OFF )
        {
            status = this->sendPortStatus(command, IH_SET_PORT_OFF);
        }
        
        //kliknij..
        if(command->func_internal_number == IH_SET_PORT_CLICK )
        {
              status = this->sendPortStatus(command, IH_SET_PORT_ON);
              //usleep(1000 * 90);  //poczekaj 50 ms..
              usleep(1000 * portInfo.time_to_off_ms);  //
              status = this->sendPortStatus(command, IH_SET_PORT_OFF);
              
            
        }
        
        //podwojne klikniecie...
        if(command->func_internal_number == IH_SET_PORT_DOUBLE_CLICK )
        {             
            status =  this->sendPortStatus(command, IH_SET_PORT_ON);
            //usleep(1000 * 90);  //poczekaj 50 ms..
            usleep(1000 * portInfo.time_to_off_ms);  //
            status =  this->sendPortStatus(command, IH_SET_PORT_OFF);            
            //usleep(1000 * 90);  //poczekaj 50 ms..
            usleep(1000 * portInfo.switch_delay_ms);  //
            
            status =  this->sendPortStatus(command, IH_SET_PORT_ON);
            //usleep(1000 * 90);  //poczekaj 50 ms..
            usleep(1000 * portInfo.time_to_off_ms);  //
            status = this->sendPortStatus(command, IH_SET_PORT_OFF);
            
           
        }
 
        if(status)
        {
            this->getAppHandle()->show_log_driver("app_driver_techbase","setPortStatus7055D","set status success",7);
        };
     
}



//-------------------------------------------------------------------------------

 

void app_driver_techbase::setPortStatus7055D(gxh_message* command)
{       
    
      //  ih_change_port_info portInfo; 
     //   memcpy(&portInfo, command->dataStruct, sizeof(ih_change_port_info) );
        
        bool status = false;
    
        //ustaw stan wysoki..
        if(command->func_internal_number == IH_SET_PORT_ON )
        {
            status = this->sendPortStatus7055(command, IH_SET_PORT_ON);
        }
        
        //ustaw stan niski
        if(command->func_internal_number == IH_SET_PORT_OFF )
        {
            status = this->sendPortStatus7055(command, IH_SET_PORT_OFF);
        }
        
        //kliknij..
        if(command->func_internal_number == IH_SET_PORT_CLICK )
        {
              status = this->sendPortStatus7055(command, IH_SET_PORT_ON);
              usleep(1000 * 90);  //poczekaj 50 ms..
              status = this->sendPortStatus7055(command, IH_SET_PORT_OFF);
              
            
        }
        
        //podwojne klikniecie...
        if(command->func_internal_number == IH_SET_PORT_DOUBLE_CLICK )
        {
             
            status =  this->sendPortStatus7055(command, IH_SET_PORT_ON);
            usleep(1000 * 90);  //poczekaj 50 ms..
            status =  this->sendPortStatus7055(command, IH_SET_PORT_OFF);
            usleep(1000 * 90);  //poczekaj 50 ms..
            status =  this->sendPortStatus7055(command, IH_SET_PORT_ON);
            usleep(1000 * 90);  //poczekaj 50 ms..
            status = this->sendPortStatus7055(command, IH_SET_PORT_OFF);
            
           
        }
        
        if(status)
        {
            this->getAppHandle()->show_log_driver("app_driver_techbase","setPortStatus7055D","set status success",7);
        };
  
}



//-------------------------------------------------------------------------------

bool app_driver_techbase::sendPortStatus(gxh_message* command, int portStatus)
{
    ih_change_port_info portInfo; 
    memcpy(&portInfo, command->dataStruct, sizeof(ih_change_port_info) );
    
   
    int actualPortState = this->getPortStatus();        
    if(actualPortState == -1) return false;
    
    
    int numPort = -1;
    
    for(int m=0;m<this->num_ports; m++)
    {
        if(this->ports[m].id == portInfo.ih_driver_port_id) numPort = this->ports[m].num;
    }
       
    
    if(numPort == -1)
    {                
        //*portInfo->status = true; //stan portu nie zostal zmieniony....
        //*portInfo->response_from_driver = true;
        return true;
    }                  
        
   
    
    char addr[5];
    this->getAddress((char*)addr );
    
    char buffer2[20];
    buffer2[0] = '@';
    buffer2[1] =  addr[0]; // '0';
    buffer2[2] =  addr[1];  //'1';
    
    
    
    
    if(portStatus== IH_SET_PORT_ON )
    {
      int statusToSet = this->HexPortToInt(numPort); //binarny wysoki stan portu do ustawienia..                
      int readyToSend = actualPortState | statusToSet; 
      
      char* toSave = this->IntPortToChar(readyToSend);
        
      buffer2[3] = toSave[0];
      buffer2[4] = toSave[1];
    
      buffer2[5] = toSave[2];
      buffer2[6] = toSave[3];
      
      delete[] toSave;
      
      std::string msg = "IH_SET_PORT_ON, ADDR: ";
      msg.append(this->getAddressPtr() );
      msg.append(", DO: ");     
      msg.append( gxh_IntToString( this->ports[numPort].num) );
  
      
      this->getAppHandle()->show_log_driver("app_driver_techbase","sendPortStatus",msg.c_str(),0); 
    }
    
    
    if(portStatus == IH_SET_PORT_OFF )
    {
      int statusToSet = this->HexPortToInt(numPort); //binarny wysoki stan portu do ustawienia..                
      int readyToSend = (actualPortState | statusToSet) & (~statusToSet) ;   
      
  
  
      char* toSave = this->IntPortToChar(readyToSend);
        
      buffer2[3] = toSave[0];
      buffer2[4] = toSave[1];
    
      buffer2[5] = toSave[2];
      buffer2[6] = toSave[3];
      
      delete[] toSave;
      
      std::string msg = "IH_SET_PORT_OFF, ADDR: ";
      msg.append(this->getAddressPtr() );
      msg.append(", DO: ");     
      msg.append( gxh_IntToString( this->ports[numPort].num) ); 
      
      this->getAppHandle()->show_log_driver("app_driver_techbase","sendPortStatus",msg.c_str(),0); 
    }
    
    
    
    
    
    
    buffer2[7] = 13;   
    
    int sum2 = buffer2[0] + buffer2[1] + buffer2[2] + buffer2[3] + buffer2[4] + buffer2[5] + + buffer2[6];              
    sum2 = sum2 & 0x0ff; //zamieniec na ASCII np. 45h  --> 45CR
    
    
    //Zamien na hex. 
    std::stringstream sstream2;
    sstream2 << std::hex << sum2;
    std::string result2 = sstream2.str();   
 
    //zamien na duze znaki...
    char* resultC2 = (char*)result2.c_str();
    
    
    //kopiuj sume kontrolna.... jesli jest potrzebna...
    int inC2 = 0;
    while(resultC2[inC2])
    {
        resultC2[inC2] = std::toupper( resultC2[inC2] );
        inC2++;
    }
     
    for(int i=0; i<inC2; i++)
    {
        buffer2[i+7] = resultC2[i];
    }
    
    buffer2[inC2+7] = 13; //enter po sumie kontrolnej..          
    
   // this->getAppHandle()->show_log_driver("app_driver_techbase","setPortStatus7061D", (char*)buffer2);  
      
    //opróznij buffor..
    this->getPortHandle()->flush();  
    
    this->getPortHandle()->writeData(buffer2,inC2+8);  // this->getAppHandle()->show_log_driver("app_driver_techbase","setPortStatus7061D ............", (char*)buffer2,0);  
    
    //----
    
    this->statusBuffer[0] = '>';
    this->statusBuffer[1] = buffer2[3];
    this->statusBuffer[2] = buffer2[4];
    this->statusBuffer[3] = buffer2[5];
    this->statusBuffer[4] = buffer2[6];
    this->statusCache = true;
    
    //----
    
           
    usleep (1000* this->getDelayBeforeRead() );  //100 mili sekund...    czekamy na prawidłowe wysłanie polecenia..
    
   
    char rBuffer2[12];
    int r2 = this->getPortHandle()->readData(rBuffer2, 10);   
    
    if(r2 > 0)
    {
        //this->getAppHandle()->show_log_driver("app_driver_techbase","setPortStatus7061D ............", (char*)rBuffer2);                     
       *portInfo.status = true; //stan portu nie zostal zmieniony....                       
       *portInfo.response_from_driver = true;   //gorzej jak wątek modułu już się zakończy wtedy gdzie zapisać zmienna....         
       return true;
    }else
    {
        this->getAppHandle()->show_error("app_driver_techbase","setPortStatus","Nie mozna odczytac sumy kontrolnej",0);
    }
    
    
    return true; //// zakładamy ze ok..
}


//-------------------------------------------------------------------------------

bool app_driver_techbase::sendPortStatus7055(gxh_message* command, int portStatus)
{
    ih_change_port_info portInfo; 
    memcpy(&portInfo, command->dataStruct, sizeof(ih_change_port_info) );
    
   
    int actualPortState = this->getPortStatus();
    if(actualPortState == -1) return false;
    
    
    int numPort = -1;
    
    for(int m=0;m<this->num_ports; m++)
    {
        if(this->ports[m].id == portInfo.ih_driver_port_id) numPort = this->ports[m].num;
    }
       
    
    if(numPort == -1)
    {
        
        
        //*portInfo->status = true; //stan portu nie zostal zmieniony....
        //*portInfo->response_from_driver = true;
        return true;
    }                  
        
   
    
    char addr[5];
    this->getAddress((char*)addr);
    
    char buffer2[20];
    buffer2[0] = '#';
    buffer2[1] = addr[0]; // '0';
    buffer2[2] = addr[1];  //'1';
    
    
    
    
    if(portStatus== IH_SET_PORT_ON )
    {
      int statusToSet = this->HexPortToInt(numPort); //binarny wysoki stan portu do ustawienia..                
      int readyToSend = actualPortState | statusToSet; 
      
      char* toSave = this->IntPortToChar(readyToSend);
        
      buffer2[3] = '0';
      buffer2[4] = '0';
    
      buffer2[5] = toSave[2];
      buffer2[6] = toSave[3];
      
      delete[] toSave;
      
 
      std::string msg = "IH_SET_PORT_ON ";
      msg.append( gxh_IntToString(actualPortState));
      msg.append(" ");  
      msg.append(  gxh_IntToString(statusToSet) );
      
       
      this->getAppHandle()->show_log_driver("app_driver_techbase","sendPortStatus7055",msg.c_str(),0); 
    }
    
    
    if(portStatus == IH_SET_PORT_OFF )
    {
      int statusToSet = this->HexPortToInt(numPort); //binarny wysoki stan portu do ustawienia..                
      int readyToSend = (actualPortState | statusToSet) & (~statusToSet) ;   
      
  
  
      char* toSave = this->IntPortToChar(readyToSend);
        
      buffer2[3] = '0';
      buffer2[4] = '0';
    
      buffer2[5] = toSave[2];
      buffer2[6] = toSave[3];
      
      
      delete[] toSave;
      
   
      
      std::string msg = "IH_SET_PORT_OFF "; 
      msg.append(gxh_IntToString(actualPortState)); 
      msg.append(" ");      
      msg.append(gxh_IntToString(statusToSet) );            
      
      this->getAppHandle()->show_log_driver("app_driver_techbase","sendPortStatus7055",msg.c_str(),0); 
    }
    
    
    
    
    
    
    buffer2[7] = 13;   
    
    int sum2 = buffer2[0] + buffer2[1] + buffer2[2] + buffer2[3] + buffer2[4] + buffer2[5] + + buffer2[6];              
    sum2 = sum2 & 0x0ff; //zamieniec na ASCII np. 45h  --> 45CR
    
    
    //Zamien na hex. 
    std::stringstream sstream2;
    sstream2 << std::hex << sum2;
    std::string result2 = sstream2.str();   
 
    //zamien na duze znaki...
    char* resultC2 = (char*)result2.c_str();
    
    
    //kopiuj sume kontrolna.... jesli jest potrzebna...
    int inC2 = 0;
    while(resultC2[inC2])
    {
        resultC2[inC2] = std::toupper( resultC2[inC2] );
        inC2++;
    }
     
    for(int i=0; i<inC2; i++)
    {
        buffer2[i+7] = resultC2[i];
    }
    
    buffer2[inC2+7] = 13; //enter po sumie kontrolnej..          
    
   // this->getAppHandle()->show_log_driver("app_driver_techbase","sendPortStatus7055", (char*)buffer2,0);  
      
    //opróznij buffor..
    this->getPortHandle()->flush();  
    
    this->getPortHandle()->writeData(buffer2,inC2+8); 
    
    
     //----
    
    this->statusBuffer[0] = '>';
    this->statusBuffer[1] = buffer2[3];
    this->statusBuffer[2] = buffer2[4];
    this->statusBuffer[3] = buffer2[5];
    this->statusBuffer[4] = buffer2[6];
    this->statusCache = true;
    
    //----
    
    
    
    
    usleep (1000* this->getDelayBeforeRead() );  //100 mili sekund...    
    
    char rBuffer2[12];
    int r2 = this->getPortHandle()->readData(rBuffer2, 10);   
    
    if(r2 > 0)
    {
        this->getAppHandle()->show_log_driver("app_driver_techbase","sendPortStatus7055 ............", (char*)rBuffer2,1);                     
       *portInfo.status = true; //stan portu nie zostal zmieniony....                       
       *portInfo.response_from_driver = true;           
       return true;
    }else
    {
         this->getAppHandle()->show_error("app_driver_techbase","sendPortStatus7055","Nie mozna odczytac sumy kontrolnej",0);
    }
    
    return false;
}


//-------------------------------------------------------------------------------

bool app_driver_techbase::checkPortStatus7051D()
{     
    
    char addr[5];
    this->getAddress( (char*)addr );
    
    char buffer[10];
    buffer[0] = '@';
    buffer[1] = addr[0]; // '0';
    buffer[2] = addr[1];  //'1';
    buffer[3] = 13;   
    
    int sum = buffer[0] + buffer[1] + buffer[2];              
    sum = sum & 0x0ff; //zamieniec na ASCII np. 45h  --> 45CR
    
    //Zamien na hex. 
    std::stringstream sstream;
    sstream << std::hex << sum;
    std::string result = sstream.str();   
 
    //zamien na duze znaki...
    char* resultC = (char*)result.c_str();
    
    
    //kopiuj sume kontrolna.... jesli jest potrzebna...
    int inC = 0;
    while(resultC[inC])
    {
        resultC[inC] = std::toupper( resultC[inC] );
        inC++;
    }
     
    for(int i=0; i<inC; i++)
    {
        buffer[i+3] = resultC[i];
    }
    
     buffer[inC+3] = 13; //enter po sumie kontrolnej..
     
 
    
    
    //this->getAppHandle()->show_log_driver("app_driver_techbase","resume", (char*)buffer);  
      
    //opróznij buffor..
    this->getPortHandle()->flush();  
    
    this->getPortHandle()->writeData(buffer,inC+5); 
    
    
    usleep (1000* this->getDelayBeforeRead() );  //100 mili sekund...    
    
    char rBuffer[12];
    int r = this->getPortHandle()->readData(rBuffer, 10);   
    
     
   // std::cout<<"\n\nOdczytano "<<r<<", "<<rBuffer;
    
    if(r == 8) // >0000FE CR  
    {                   
      char Hex[5];
      Hex[0] = rBuffer[1];
      Hex[1] = rBuffer[2];
      Hex[2] = rBuffer[3];
      Hex[3] = rBuffer[4];
      Hex[4] = 0;
        
      this->di_actual_state = this->HexToTwoByte(Hex);
      
      
      if(this->di_actual_state !=  this->di_last_state)
      {
        
       bool retStatus = true; //jeśli zwraca true to port przerywa dalsze wykonywanie resume i wykonuje execute...   
          
       this->getAppHandle()->show_log_driver("app_driver_techbase","checkPortStatus7051D", "Check....." ,9 );       
       
       
       if ( ( (this->di_actual_state ^ this->di_last_state) & 0b00000000000000010000000000000000) == 0b00000000000000010000000000000000 )  
       {
            if (  (this->di_actual_state & 0b00000000000000010000000000000000) == 0b00000000000000010000000000000000 ) this->changePortListener(0,true);       
            if (  (this->di_actual_state & 0b00000000000000010000000000000000) != 0b00000000000000010000000000000000 ) { this->changePortListener(0,false);   retStatus=false; }
       }
       
       if ( ((this->di_actual_state ^ this->di_last_state) & 0b00000000000000100000000000000000) == 0b00000000000000100000000000000000 ) 
       {
            if (  (this->di_actual_state & 0b00000000000000100000000000000000) == 0b00000000000000100000000000000000 ) this->changePortListener(1,true);
            if (  (this->di_actual_state & 0b00000000000000100000000000000000) != 0b00000000000000100000000000000000 ) { this->changePortListener(1,false);   retStatus=false; }
       }
       
       if ( ( (this->di_actual_state ^ this->di_last_state) & 0b00000000000001000000000000000000) == 0b00000000000001000000000000000000 ) 
       {
            if (  (this->di_actual_state & 0b00000000000001000000000000000000) == 0b00000000000001000000000000000000 )   this->changePortListener(2,true);
            if (  (this->di_actual_state & 0b00000000000001000000000000000000) != 0b00000000000001000000000000000000 )  { this->changePortListener(2,false);   retStatus=false; }
       }
       
       if ((  (this->di_actual_state ^ this->di_last_state) & 0b00000000000010000000000000000000)== 0b00000000000010000000000000000000 ) 
       {
            if (  (this->di_actual_state & 0b00000000000010000000000000000000) == 0b00000000000010000000000000000000 )this->changePortListener(3,true);
            if (  (this->di_actual_state & 0b00000000000010000000000000000000) != 0b00000000000010000000000000000000 ){ this->changePortListener(3,false);   retStatus=false; }
       }
       
       //
       if ( ( (this->di_actual_state ^ this->di_last_state) & 0b00000000000000000001000000000000) == 0b00000000000000000001000000000000 ) 
       {
           if (  (this->di_actual_state & 0b00000000000000000001000000000000) == 0b00000000000000000001000000000000 )this->changePortListener(4,true);
           if (  (this->di_actual_state & 0b00000000000000000001000000000000) != 0b00000000000000000001000000000000 ){ this->changePortListener(4,false);   retStatus=false; }
       }
       
       if (( (this->di_actual_state ^ this->di_last_state) & 0b00000000000000000010000000000000) == 0b00000000000000000010000000000000 )
       {
           if (  (this->di_actual_state & 0b00000000000000000010000000000000) == 0b00000000000000000010000000000000 ) this->changePortListener(5,true);
           if (  (this->di_actual_state & 0b00000000000000000010000000000000) != 0b00000000000000000010000000000000 ) { this->changePortListener(5,false);   retStatus=false; }
       }
       if( ( (this->di_actual_state ^ this->di_last_state) & 0b00000000000000000100000000000000) == 0b00000000000000000100000000000000 ) 
       {
          if (  (this->di_actual_state & 0b00000000000000000100000000000000) == 0b00000000000000000100000000000000 ) this->changePortListener(6,true);
          if (  (this->di_actual_state & 0b00000000000000000100000000000000) != 0b00000000000000000100000000000000 ) { this->changePortListener(6,false);   retStatus=false; }
       }
       if (( (this->di_actual_state ^ this->di_last_state) & 0b00000000000000001000000000000000) == 0b00000000000000001000000000000000 ) 
       {
          if (  (this->di_actual_state & 0b00000000000000001000000000000000) == 0b00000000000000001000000000000000 )  this->changePortListener(7,true);
          if (  (this->di_actual_state & 0b00000000000000001000000000000000) != 0b00000000000000001000000000000000 )  { this->changePortListener(7,false);   retStatus=false; }
       }
       
       //       
       if ( ( (this->di_actual_state ^ this->di_last_state) & 0b00000000000000000000000100000000)== 0b00000000000000000000000100000000 ) 
       {
          if (  (this->di_actual_state & 0b00000000000000000000000100000000) == 0b00000000000000000000000100000000 )  this->changePortListener(8,true);
          if (  (this->di_actual_state & 0b00000000000000000000000100000000) != 0b00000000000000000000000100000000 ) { this->changePortListener(8,false);   retStatus=false; }
       }
       
       
       if ( ( (this->di_actual_state ^ this->di_last_state) &0b00000000000000000000001000000000) == 0b00000000000000000000001000000000 ) 
       {
          if (  (this->di_actual_state & 0b00000000000000000000001000000000) == 0b00000000000000000000001000000000 )  this->changePortListener(9,true);       
          if (  (this->di_actual_state & 0b00000000000000000000001000000000) != 0b00000000000000000000001000000000 ) { this->changePortListener(9,false);   retStatus=false; }  
       }
       
       if (( (this->di_actual_state ^ this->di_last_state) & 0b00000000000000000000010000000000) == 0b00000000000000000000010000000000 ) 
       {
          if (  (this->di_actual_state & 0b00000000000000000000010000000000) == 0b00000000000000000000010000000000 ) this->changePortListener(10,true);
          if (  (this->di_actual_state & 0b00000000000000000000010000000000) != 0b00000000000000000000010000000000 ){ this->changePortListener(10,false);   retStatus=false; }
       }
       
       if ( ( (this->di_actual_state ^ this->di_last_state) & 0b00000000000000000000100000000000) == 0b00000000000000000000100000000000 ) 
       {
          if (  (this->di_actual_state & 0b00000000000000000000100000000000) == 0b00000000000000000000100000000000 )  this->changePortListener(11,true);
          if (  (this->di_actual_state & 0b00000000000000000000100000000000) != 0b00000000000000000000100000000000 ) { this->changePortListener(11,false);   retStatus=false; }
       }
       
       //
       if ( ( (this->di_actual_state ^ this->di_last_state) & 0b00000000000000000000000000010000) == 0b00000000000000000000000000010000 )
       {
            if (  (this->di_actual_state & 0b00000000000000000000000000010000) == 0b00000000000000000000000000010000 )  this->changePortListener(12,true);                
            if (  (this->di_actual_state & 0b00000000000000000000000000010000) != 0b00000000000000000000000000010000 ) { this->changePortListener(12,false);   retStatus=false; }            
       }
       
       if (( (this->di_actual_state ^ this->di_last_state) & 0b00000000000000000000000000100000) == 0b00000000000000000000000000100000 )
       {
            if (  (this->di_actual_state & 0b00000000000000000000000000100000) == 0b00000000000000000000000000100000 )  this->changePortListener(13,true);
            if (  (this->di_actual_state & 0b00000000000000000000000000100000) != 0b00000000000000000000000000100000 ) { this->changePortListener(13,false);   retStatus=false; }
       }
       
       if (( (this->di_actual_state ^ this->di_last_state) & 0b00000000000000000000000001000000) == 0b00000000000000000000000001000000 ) 
       {
           if (  (this->di_actual_state & 0b00000000000000000000000001000000) == 0b00000000000000000000000001000000 )   this->changePortListener(14,true);
           if (  (this->di_actual_state & 0b00000000000000000000000001000000) != 0b00000000000000000000000001000000 )  { this->changePortListener(14,false);   retStatus=false; }
       }
       
       
       if (( (this->di_actual_state ^ this->di_last_state) & 0b00000000000000000000000010000000) == 0b00000000000000000000000010000000 )
       {
          if (  (this->di_actual_state & 0b00000000000000000000000010000000) == 0b00000000000000000000000010000000 )   this->changePortListener(15,true);
          if (  (this->di_actual_state & 0b00000000000000000000000010000000) != 0b00000000000000000000000010000000 )  { this->changePortListener(15,false);   retStatus=false; }
       }
          
       
       
         this->di_last_state = this->di_actual_state;
         return retStatus;
      }            
      this->di_last_state = this->di_actual_state;
      
    }        
    
    
    return false;
}

//-------------------------------------------------------------------------------

bool app_driver_techbase::checkPortStatus7055D()
{
     
    
    char addr[5];
    this->getAddress(addr);
    
    char buffer[10];
    buffer[0] = '@';
    buffer[1] = addr[0]; // '0';
    buffer[2] = addr[1];  //'1'; 
    buffer[3] = 13;   
    
    int sum = buffer[0] + buffer[1] + buffer[2];              
    sum = sum & 0x0ff; //zamieniec na ASCII np. 45h  --> 45CR
    
    //Zamien na hex. 
    std::stringstream sstream;
    sstream << std::hex << sum;
    std::string result = sstream.str();   
 
    //zamien na duze znaki...
    char* resultC = (char*)result.c_str();
    
    
    //kopiuj sume kontrolna.... jesli jest potrzebna...
    int inC = 0;
    while(resultC[inC])
    {
        resultC[inC] = std::toupper( resultC[inC] );
        inC++;
    }
     
    for(int i=0; i<inC; i++)
    {
        buffer[i+3] = resultC[i];
    }
    
     buffer[inC+3] = 13; //enter po sumie kontrolnej..
     
 
    
    
    //this->getAppHandle()->show_log_driver("app_driver_techbase","resume", (char*)buffer);  
      
    //opróznij buffor..
    this->getPortHandle()->flush();  
    
    this->getPortHandle()->writeData(buffer,inC+5); 
    
    
    usleep (1000* this->getDelayBeforeRead() );  //100 mili sekund...    
    
    char rBuffer[12];
    int r = this->getPortHandle()->readData(rBuffer, 10);   
    
     
   //  this->getAppHandle()->show_log_driver("app_driver_techbase","REEEEEEEEEEEDDD", (char*)rBuffer);  
    
    if(r == 8) // >0000FE CR  
    {                   
      char Hex[5];
      Hex[0] = rBuffer[1];
      Hex[1] = rBuffer[2];
      Hex[2] = rBuffer[3];
      Hex[3] = rBuffer[4];   
      Hex[4] = 0;
        
      this->di_actual_state = this->HexToTwoByte(Hex);
      
       //  this->getAppHandle()->show_log_driver("app_driver_techbase","xxxxxxxxxxxxxxxxxxxxxxx", (char*)Hex);  
         
         
      if(this->di_actual_state !=  this->di_last_state)
      {
        bool retStatus = true;  
          
        this->getAppHandle()->show_log_driver("app_driver_techbase","checkPortStatus7055D", "Check....." ,9 );   
                
            
       
       if ( ( (this->di_actual_state ^ this->di_last_state) & 0b00000000000000010000000000000000) == 0b00000000000000010000000000000000 )  
       {
            if (  (this->di_actual_state & 0b00000000000000010000000000000000) == 0b00000000000000010000000000000000 ) this->changePortListener(0,true);       
            if (  (this->di_actual_state & 0b00000000000000010000000000000000) != 0b00000000000000010000000000000000 )  { this->changePortListener(0,false);   retStatus=false; }       
       }
       
       if ( ((this->di_actual_state ^ this->di_last_state) & 0b00000000000000100000000000000000) == 0b00000000000000100000000000000000 ) 
       {
            if (  (this->di_actual_state & 0b00000000000000100000000000000000) == 0b00000000000000100000000000000000 ) this->changePortListener(1,true);
            if (  (this->di_actual_state & 0b00000000000000100000000000000000) != 0b00000000000000100000000000000000 ){ this->changePortListener(1,false);   retStatus=false; }
       }
       
       if ( ( (this->di_actual_state ^ this->di_last_state) & 0b00000000000001000000000000000000) == 0b00000000000001000000000000000000 ) 
       {
            if (  (this->di_actual_state & 0b00000000000001000000000000000000) == 0b00000000000001000000000000000000 )   this->changePortListener(2,true);
            if (  (this->di_actual_state & 0b00000000000001000000000000000000) != 0b00000000000001000000000000000000 )  { this->changePortListener(2,false);   retStatus=false; }
       }
       
       if ((  (this->di_actual_state ^ this->di_last_state) & 0b00000000000010000000000000000000)== 0b00000000000010000000000000000000 ) 
       {
            if (  (this->di_actual_state & 0b00000000000010000000000000000000) == 0b00000000000010000000000000000000 )this->changePortListener(3,true);
            if (  (this->di_actual_state & 0b00000000000010000000000000000000) != 0b00000000000010000000000000000000 ){ this->changePortListener(3,false);   retStatus=false; }
       }
       
       //
       if ( ( (this->di_actual_state ^ this->di_last_state) & 0b00000000000000000001000000000000) == 0b00000000000000000001000000000000 ) 
       {
           if (  (this->di_actual_state & 0b00000000000000000001000000000000) == 0b00000000000000000001000000000000 )this->changePortListener(4,true);
           if (  (this->di_actual_state & 0b00000000000000000001000000000000) != 0b00000000000000000001000000000000 ){ this->changePortListener(4,false);   retStatus=false; }
       }
       
       if (( (this->di_actual_state ^ this->di_last_state) & 0b00000000000000000010000000000000) == 0b00000000000000000010000000000000 )
       {
           if (  (this->di_actual_state & 0b00000000000000000010000000000000) == 0b00000000000000000010000000000000 ) this->changePortListener(5,true);
           if (  (this->di_actual_state & 0b00000000000000000010000000000000) != 0b00000000000000000010000000000000 ){ this->changePortListener(5,false);   retStatus=false; }
       }
       if( ( (this->di_actual_state ^ this->di_last_state) & 0b00000000000000000100000000000000) == 0b00000000000000000100000000000000 ) 
       {
          if (  (this->di_actual_state & 0b00000000000000000100000000000000) == 0b00000000000000000100000000000000 ) this->changePortListener(6,true);
          if (  (this->di_actual_state & 0b00000000000000000100000000000000) != 0b00000000000000000100000000000000 ) { this->changePortListener(6,false);   retStatus=false; }
       }
       if (( (this->di_actual_state ^ this->di_last_state) & 0b00000000000000001000000000000000) == 0b00000000000000001000000000000000 ) 
       {
          if (  (this->di_actual_state & 0b00000000000000001000000000000000) == 0b00000000000000001000000000000000 )  this->changePortListener(7,true);
          if (  (this->di_actual_state & 0b00000000000000001000000000000000) != 0b00000000000000001000000000000000 ) { this->changePortListener(7,false);   retStatus=false; }
       }
       
       
      
       
         this->di_last_state = this->di_actual_state;
         return retStatus;
      }            
      this->di_last_state = this->di_actual_state;
      
    }        
    
    
    return false;
}

//-------------------------------------------------------------------------------

void app_driver_techbase::sendMsgToCore(int func_internal_number, int ih_driver_port_id)
{
               //lets interrupt...
              gxh_on_click gxhOnClick;
              gxhOnClick.func_number = this->ihSlDriver.const_function_driver;;
              gxhOnClick.ih_driver_id = this->getIhDriverId();
              gxhOnClick.func_internal_number = func_internal_number;
              gxhOnClick.ih_driver_port_id = ih_driver_port_id;
              
              gxh_message gxhMessage;
              gxhMessage.gxh_handle = -1; //don't response to me.....
              gxhMessage.func_number = gxhOnClick.func_number;
              gxhMessage.func_internal_number = gxhOnClick.func_internal_number;
              memcpy(gxhMessage.dataStruct, &gxhOnClick, sizeof(gxh_on_click) );
              gxhMessage.sizeDataStruct =  sizeof(gxh_on_click) ;
              gxhMessage.system_code = GXH_MESSAGE_SUCCESS;
              gxhMessage.check_timeout = 0;
              
            
                           
              bool res = this->getAppHandle()->sendGXHMessage(&gxhMessage); //max priority              
              
              if(!res)
              {
                  this->getAppHandle()->show_error("app_driver_techbase","sendMsgToCore","Cannot add message",4);
              }
              
              //end interrupt
}

//-------------------------------------------------------------------------------

void app_driver_techbase::scrollHistory(unsigned long time, bool status, char option, int index)
{
    
    for(int w=5; w>=1; w--)
    {
          this->history_ports[index].info[w].time =    this->history_ports[index].info[w-1].time;
          this->history_ports[index].info[w].status =  this->history_ports[index].info[w-1].status;
          this->history_ports[index].info[w].option =  this->history_ports[index].info[w-1].option;
    }
    
    /*
          this->history_ports[index].info[5].time =    this->history_ports[index].info[4].time;
          this->history_ports[index].info[5].status =  this->history_ports[index].info[4].status;
          this->history_ports[index].info[5].option =  this->history_ports[index].info[4].option;
    
          this->history_ports[index].info[4].time =    this->history_ports[index].info[3].time;
          this->history_ports[index].info[4].status =  this->history_ports[index].info[3].status;
          this->history_ports[index].info[4].option =  this->history_ports[index].info[3].option;
     
          this->history_ports[index].info[3].time =    this->history_ports[index].info[2].time;
          this->history_ports[index].info[3].status =  this->history_ports[index].info[2].status;
          this->history_ports[index].info[3].option =  this->history_ports[index].info[2].option;
     
          this->history_ports[index].info[2].time =    this->history_ports[index].info[1].time;
          this->history_ports[index].info[2].status =  this->history_ports[index].info[1].status;
          this->history_ports[index].info[2].option =  this->history_ports[index].info[1].option;
     
          this->history_ports[index].info[1].time =    this->history_ports[index].info[0].time;
          this->history_ports[index].info[1].status =  this->history_ports[index].info[0].status;
          this->history_ports[index].info[1].option =  this->history_ports[index].info[0].option;
     */
          this->history_ports[index].info[0].time = time;
          this->history_ports[index].info[0].status = status;
          this->history_ports[index].info[0].option = option;
}

//-------------------------------------------------------------------------------


void app_driver_techbase::changePortListener(int num,bool status)
{
    
      std::string  msgC = " analiza zmiany stanu portu nr ";
      msgC.append(gxh_IntToString(num));
      if(status) msgC.append(" na ON ");
      if(!status) msgC.append(" na OFF ");
      
      this->getAppHandle()->show_log_driver("app_driver_techbase","changePortListener", (char*)msgC.c_str(),5 );  
      
      
    
      int doubleClickDelay = PORT_DOUBLE_CLICK_MS;
  //    int longClickDelay   = PORT_LONG_CLICK_MS;
      bool doubleClick     = false;
      bool longClick       = false;
      char option          = 0;
      int index            = -1;
    
      struct timeval tp;
      gettimeofday(&tp, NULL);
      unsigned long  useconds = tp.tv_sec * 1000 + tp.tv_usec / 1000;// milisekundy..
      
      
      for(int w=0; w<this->num_ports; w++)
      {
          if(  (this->ports[w].num == num) && (this->ports[w].type == 'I' ) ) index = w;
      }
      
      
      
           
      if( !(this->history_ports[index].info[0].time < ((unsigned long)(useconds-100)) )  )
      {                         
         this->getAppHandle()->show_error("app_driver_techbase","changePortListener","0x009012112",4 );  
         // return; //drgania sprzetowe... zbyt szybko cos zostalo klikniete.. 100ms...
      }
          
    
      std::string msg2 = "";
              
      
      if(this->ports[index].reverts == '1' ) 
      {
          status = !status;        
         // std::string msgA = "Revers: ";
         // msgA.append(  gxh_IntToString( this->ports[index].num ));
         // this->getAppHandle()->show_log_driver("app_driver_techbase","changePortListener", msgA.c_str() );   
      }
      
      if(this->ports[index].reverts == '0' ) 
      {                     
         // std::string msgA = "No revers: ";
         // msgA.append(  gxh_IntToString( this->ports[index].num ));
        //  this->getAppHandle()->show_log_driver("app_driver_techbase","changePortListener", msgA.c_str() );    
      }
      
      
      if(status) //reaguj tylko na zbocze wysokie.. jesli revers to odwrotnie..
      {                     
          
          if(          
             //( (this->history_ports[index].info[0].option == 'c' || this->history_ports[index].info[0].option == 'f' )  && (this->history_ports[index].info[0].time >= (unsigned long)(useconds-doubleClickDelay) )  )                                                        
             ( (this->history_ports[index].info[0].option == 'c' || (this->history_ports[index].info[0].option == 'f' && this->history_ports[index].info[1].option == 'c') )  && (this->history_ports[index].info[0].time >= (unsigned long)(useconds-doubleClickDelay) )  )                                                        
            )     
          doubleClick=true;                            
                          
          
          if(doubleClick) 
          {
             
              
              msg2.append("Double Click, ADR: ");
              msg2.append( this->getAddressPtr() );
              msg2.append(", DI: ");              
              msg2.append( gxh_IntToString( this->ports[index].num ) );   
              option = 'd';                                          
             
              this->sendMsgToCore(IH_SWITCH_DOUBLE_CLICK, this->ports[index].id);
              
          }
          
          
          if(!doubleClick)
          {             
              msg2.append("On Click, ADR: ");
              msg2.append( this->getAddressPtr() );
              msg2.append(", DI: ");           
              msg2.append( gxh_IntToString( this->ports[index].num )); 
              option = 'c';                    
                
          
              this->sendMsgToCore(IH_SWITCH_ON_CLICK, this->ports[index].id);
          }
               
           this->getAppHandle()->show_log_driver("app_driver_techbase","changePortListener", (char*)msg2.c_str(),0 );   
     
  
      
          this->scrollHistory(useconds, status, option, index);
      }

      
      if(!status) //reaguj tylko na zbocze niske w przypadku akcji LONGClick
      {   
         if(          
             //( (this->history_ports[index].info[0].option == 'c') && (this->history_ports[index].info[0].time < (unsigned long)(useconds-(int(longClickDelay/2)) ) )   && (!(this->history_ports[index].info[0].time < (unsigned long)(useconds-longClickDelay - longClickDelay ) ))     )                                                        
             ( (this->history_ports[index].info[0].option == 'c') && ( (this->history_ports[index].info[0].time+1000) < useconds )   && ( (this->history_ports[index].info[0].time+6000) > useconds )   )  //min 1 sek max 5 sek..
           )     
          longClick=true;  
          
          if(longClick) 
          {
              
              msg2.append("Long Click, ADR: ");
              msg2.append( this->getAddressPtr() );
              msg2.append(", DI: ");         
              msg2.append( gxh_IntToString( this->ports[index].num ) ); 
              option = 'l';
              
           
              this->sendMsgToCore(IH_SWITCH_LONG_CLICK, this->ports[index].id);
              
              
              this->getAppHandle()->show_log_driver("app_driver_techbase","changePortListener", (char*)msg2.c_str() ,0);   
                 
              
              
      
              this->scrollHistory(useconds, status, option, index);
          }
         
         
         if(!longClick) 
          {              
             // usleep(1000 * 50 ); //off click jest mniej istotne i moze byc wysłane z opznieniem..
                                      
             option = 'f';
         
             
              //do cora zgłaszamy OFF click jeśli stan ON trwał długa.. np. dla czujnika I/O
            if
              (                                                                    
              ( (this->history_ports[index].info[0].time < (unsigned long)(useconds - 5000) )   )     
              )        
                
              {
                  msg2.append("Off Click, ADR: ");
                  msg2.append( this->getAddressPtr() );
                  msg2.append(", DI: ");             
                  msg2.append(  gxh_IntToString( this->ports[index].num ) );                 
                  
                  this->sendMsgToCore(IH_SWITCH_OFF_CLICK, this->ports[index].id);
                  
                  this->getAppHandle()->show_log_driver("app_driver_techbase","changePortListener", (char*)msg2.c_str() ,0);   
              }
                                                                                 
              
              
              this->scrollHistory(useconds, status, option, index);
          }                                                  
      }
       
  
}

 
 
//------------------------------------------------------------------------------

int app_driver_techbase::HexPortToInt(int numPort)
{
 
     
    if(numPort == 0)  return 0b00000000000000010000000000000000;
    if(numPort == 1)  return 0b00000000000000100000000000000000;
    if(numPort == 2)  return 0b00000000000001000000000000000000;
    if(numPort == 3)  return 0b00000000000010000000000000000000;
    
    if(numPort == 4)  return 0b00000000000000000001000000000000;
    if(numPort == 5)  return 0b00000000000000000010000000000000;
    if(numPort == 6)  return 0b00000000000000000100000000000000;
    if(numPort == 7)  return 0b00000000000000001000000000000000;
    
    if(numPort == 8)  return 0b00000000000000000000000100000000;
    if(numPort == 9)  return 0b00000000000000000000001000000000;
    if(numPort == 10) return 0b00000000000000000000010000000000;
    if(numPort == 11) return 0b00000000000000000000100000000000;
    
    if(numPort == 12) return 0b00000000000000000000000000010000;
    if(numPort == 13) return 0b00000000000000000000000000100000;
    if(numPort == 14) return 0b00000000000000000000000001000000;
    if(numPort == 15) return 0b00000000000000000000000010000000;
    
    return 0;
}

//------------------------------------------------------------------------------


char app_driver_techbase::IntToHexChar(int num)
{
    if(num == 0) return '0';
    if(num == 1) return '1';
    if(num == 2) return '2';
    if(num == 3) return '3';
    if(num == 4) return '4';
    if(num == 5) return '5';
    if(num == 6) return '6';
    if(num == 7) return '7';
    if(num == 8) return '8';
    if(num == 9) return '9';
    if(num == 10) return 'A';
    if(num == 11) return 'B';
    if(num == 12) return 'C';
    if(num == 13) return 'D';
    if(num == 14) return 'E';
    if(num == 15) return 'F';
    
    return '0';
}

//------------------------------------------------------------------------------

char* app_driver_techbase::IntPortToChar(int portStatus)
{
    
    
    
    int C0_3 = portStatus & 0b00000000000011110000000000000000;
    C0_3 = C0_3 >> 16;
    
    char C0_3Char = this->IntToHexChar(C0_3);
    
    
    
    int C4_7 = portStatus & 0b00000000000000001111000000000000;                           
    C4_7 = C4_7 >> 12;
    
    char C4_7Char = this->IntToHexChar(C4_7);
    
    
    
    int C8_11 = portStatus & 0b00000000000000000000111100000000;                                  
    C8_11 = C8_11 >> 8;
    
    char C8_11Char = this->IntToHexChar(C8_11);
    
    
    int C12_15 = portStatus & 0b00000000000000000000000011110000;                                                               
    C12_15 = C12_15 >> 4;
    
    char C12_15Char = this->IntToHexChar(C12_15);
    
    char* znaki = new char[4];
    
    
    znaki[3] = C0_3Char;
    znaki[2] = C4_7Char;
    znaki[1] = C8_11Char;
    znaki[0] = C12_15Char;
                         
    return znaki;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
 


 
