/* 
 * File:   app_driver_elpiast.cpp
 * Author: Blazej Kita, 
 * 
 * Created on 25 styczeń 2019, 14:02
 */

#include "app_driver_elpiast.h"


app_driver* get_module(gxh_scope* appHomeHandle, app_port* appPortHandle,int ih_driver_id, const char* name, const char* address, int delay_before_read_ms)
{         
    return new app_driver_elpiast(appHomeHandle, appPortHandle, ih_driver_id, name,address,delay_before_read_ms);
}
 
//-------------------------------------------------------------------------------


app_driver_elpiast::app_driver_elpiast(gxh_scope* appHomeHandle, app_port* appPortHandle,int ih_driver_id, const char* name, const char* address, int delay_before_read_ms) : app_driver(appHomeHandle, appPortHandle,ih_driver_id, name,address, delay_before_read_ms){
   
   // std::string msg = this->name;
   // msg.append(" has been loaded..");
        
   // this->getAppHandle()->show_log_driver( "app_driver_elpiast","app_driver_elpiast",msg.c_str(),0 );
   
   
               
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
            
            this->getAppHandle()->show_log_driver( "app_driver_elpiast","app_driver_elpiast",msg.c_str(),0 );
            
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
            
            this->getAppHandle()->show_log_driver( "app_driver_elpiast","app_driver_elpiast",msg.c_str(),0 );
            
            break;
        }
    }
    
           
     
     this->LDI1 = 0;
     this->LDI2 = 0;
     this->LDI3 = 0;
     this->LDI4 = 0;
     this->LDI5 = 0;
     this->LDI6 = 0;
     this->LDI7 = 0;
     this->LDI8 = 0; 
     
     this->LDI9 = 0; 
     this->LDI10 = 0; 
     this->LDI11 = 0; 
     this->LDI12 = 0; 
     this->LDI13 = 0; 
     this->LDI14 = 0; 
     this->LDI15 = 0; 
     this->LDI16 = 0; 
    
    
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
   
   for(int m=0; m<MAX_DELAY_MSG;m++)
   {
       this->msgDelay[m].time_execute = 0; //nic do wykonania...
   };
        
}


//-------------------------------------------------------------------------------
 

app_driver_elpiast::~app_driver_elpiast() {  
    
    
    char name[50];
    this->getName( (char*)name);
    std::string msg = std::string(name);
    msg.append(" has been deleted");
     
    delete[] this->history_ports;
 
    
    
    this->getAppHandle()->show_log_driver("app_driver_elpiast","app_driver_elpiast",msg.c_str(),0);
   
}


//-------------------------------------------------------------------------------


void app_driver_elpiast::execute(gxh_message* command)
{
  
    gxh_message message;
    memcpy(&message, command, sizeof(gxh_message) );
   
    
    struct timeval  tv;
    gettimeofday(&tv, NULL);  
    double time_in_mill =  (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 ;
    
    message.time_ms = time_in_mill;// czas zgloszenia do sterownika....
               
    // std::cout<<"\n\n\nChange status for port driver...............................................\n";
   // std::cout<<this->ihSlDriver.name<<"\n";
    
    if(message.func_internal_number == IH_SET_PORT_ON || message.func_internal_number == IH_SET_PORT_OFF ||  message.func_internal_number == IH_SET_PORT_CLICK || message.func_internal_number == IH_SET_PORT_DOUBLE_CLICK)  
    {
         this->getAppHandle()->show_log_driver("app_driver_elpiast","execute","change port status",11);
        
        if( gxh_compare( (const char*)this->ihSlDriver.name,"ElPiast-8DI8DO") == 0 )  this->setPortStatusEl8I8O(&message);                                  
        if( gxh_compare( (const char*)this->ihSlDriver.name,"ElPiast-16DO") == 0 )  this->setPortStatusEl8I8O(&message);     //to sama metoda do ustawiania stanu portu..                                                                      
                                
        return; 
    }
    
    
    
    if(message.func_internal_number == IH_UPDATE_ADDRESS)
    { 
      this->updateAddress(&message);  
      return; 
    } 
    
  
    this->getAppHandle()->show_log_driver ("app_driver_elpiast","execute","Unexpected command",0);              
}


//-------------------------------------------------------------------------------


bool app_driver_elpiast::resume()
{
   
    
 
     gxh_message messageDelay;
     bool isMsg = this->getMessageDelay(&messageDelay);
     if(isMsg)
     {
         this->execute(&messageDelay);
     }
    
    //--------------------------------------------------------------------------
    //---------------------- -----------------------------------
    
    if( gxh_compare( this->ihSlDriver.name, "ElPiast-8DI8DO" ) ==0)
    {
        return this->checkPortStatusDI8DO8();        
    }
   
      if( gxh_compare( this->ihSlDriver.name, "ElPiast-16DI" ) ==0)
    {
        return this->checkPortStatusDI16();        
    }
    
        
    
    
    return false; //pomin sprawdzenie kolejnego urzadzenie....
   
}

// -----------------------------------------------------------------------------

void app_driver_elpiast::updateAddress(gxh_message* command)
{
      
    this->getAppHandle()->show_error("app_driver_elpiast","updateAddress","not available",0);     
}



//-------------------------------------------------------------------------------

int app_driver_elpiast::getPortStatus()
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
    
      char addr[5];
      this->getAddress((char*)addr);
    
    
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
     
 
    
    
  //  this->getAppHandle()->show_log_driver("app_driver_elpiast","getPortStatus", (char*)buffer);  
      
    //opróznij buffor..
    this->getPortHandle()->flush();  
   
 
    
    this->getPortHandle()->writeData(buffer,inC+5); 
    
    
   
    
    
    usleep (1000* this->getDelayBeforeRead() );  //100 mili sekund...    
    
    
    char rBuffer[12];
    memset(rBuffer,'\0',12);
    int r = this->getPortHandle()->readData(rBuffer, 10);  // np. >0000FECR
    
      
    if(r > 7) // >0000FE CR  
    {
          //this->getAppHandle()->show_log_driver("app_driver_elpiast","getPortStatus / odczytano", (char*)rBuffer,5);  
          
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
        this->getAppHandle()->show_error("app_driver_elpiast","getPortStatus brak odpowiedzi na ", (char*)buffer,0);      
        this->getAppHandle()->show_error("app_driver_elpiast","getPortStatus brak odpowiedzi::: ", (char*)rBuffer,0);      
        this->getAppHandle()->show_error("app_driver_elpiast","getPortStatus brak odpowiedzi::: ", (char*)(gxh_IntToString(r).c_str()),0);    
    }
     
     
  }
    
    
   return -1;  
    
}

//-------------------------------------------------------------------------------

int app_driver_elpiast::HexToTwoByte(char* hex)
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

int app_driver_elpiast::HexCharToInt(char hex)
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
    
    
    this->getAppHandle()->show_error( "app_driver_elpiast","HexCharToInt","Nie mozna zamienic liczby hex ascii na integer!",0);
    return 0; //bez zmian..
}

//-------------------------------------------------------------------------------

bool app_driver_elpiast::addMessageDelay(gxh_message* command, int msDelay, int func_internal_number)
{     
    
   for(int m=0; m<MAX_DELAY_MSG;m++)
   {
      if( this->msgDelay[m].time_execute == 0 )
      {  
           struct timeval tp;
           gettimeofday(&tp, NULL);
           long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;   
           long int msdelay = ms + msDelay;
           
           this->msgDelay[m].time_execute = msdelay;
           memcpy(&this->msgDelay[m].message, command, sizeof(gxh_message) );
           
           if(func_internal_number>0) this->msgDelay[m].message.func_internal_number = func_internal_number;
           
          this->getAppHandle()->show_log_driver("app_driver_elpiast","setPortStatusEl","addMessageDelay - ok ",4);
           
           return true;
      }
      
   };
    
    
   this->getAppHandle()->show_log_driver("app_driver_elpiast","setPortStatusEl","addMessageDelay - error ",5);
   return false;
}

//-------------------------------------------------------------------------------

bool app_driver_elpiast::getMessageDelay(gxh_message* command)
{
 
   struct timeval tp;
   gettimeofday(&tp, NULL);
   long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;   
    
   for(int m=0; m<MAX_DELAY_MSG;m++)
   {
      if( this->msgDelay[m].time_execute > 0 &&  this->msgDelay[m].time_execute<ms  ) //ta wiadomosc powinna został obsłużoina..
      {
          
           memcpy(command, &this->msgDelay[m].message, sizeof(gxh_message) );                                 
           this->msgDelay[m].time_execute = 0;
           
           this->getAppHandle()->show_log_driver("app_driver_elpiast","setPortStatusEl","getMessageDelay - ok ",4);
           
           return true;
      }
      
   };
    
    
    
    return false;
}

//-------------------------------------------------------------------------------

bool app_driver_elpiast::clearMessageDelay(gxh_message* command)
{
 
   ih_change_port_info portInfo;
   memcpy(&portInfo, command->dataStruct, sizeof(ih_change_port_info) );   
   
   
   for(int m=0; m<MAX_DELAY_MSG;m++)
   {
      if( this->msgDelay[m].time_execute > 0 )  
      {
                                             
           ih_change_port_info portInfoOld;
           memcpy(&portInfoOld, this->msgDelay[m].message.dataStruct, sizeof(ih_change_port_info) ); 
           
           if(portInfoOld.ih_driver_port_id == portInfo.ih_driver_port_id)
           {          
             this->msgDelay[m].time_execute = 0;           
             this->getAppHandle()->show_log_driver("app_driver_elpiast","setPortStatusEl","clearMessageDelay - ok ",4);           
           }           
      }
      
   };
    
    
    
    return true;
}

//-------------------------------------------------------------------------------

 

void app_driver_elpiast::setPortStatusEl8I8O(gxh_message* command)
{       
    
  
    
        ih_change_port_info portInfo;
        memcpy(&portInfo, command->dataStruct, sizeof(ih_change_port_info) );
        
        bool status = false;
    
        //ustaw stan wysoki..
        if(command->func_internal_number == IH_SET_PORT_ON )
        {
             this->clearMessageDelay(command);
            
            status = this->sendPortStatus(command, IH_SET_PORT_ON);
           
            *portInfo.response_from_driver = true;
            *portInfo.status = status;
        }
        
        //ustaw stan niski
        if(command->func_internal_number== IH_SET_PORT_OFF )
        {
            this->clearMessageDelay(command);
            
            status = this->sendPortStatus(command, IH_SET_PORT_OFF);
            
            *portInfo.response_from_driver = true;
            *portInfo.status = status;
        }
        
        //kliknij..
        if(command->func_internal_number == IH_SET_PORT_CLICK )
        {
             this->clearMessageDelay(command);
            
              status = this->sendPortStatus(command, IH_SET_PORT_ON);             
              
              *portInfo.response_from_driver = true;
              *portInfo.status = status;
              
              if(status)
              {
              // usleep(1000 * portInfo.time_to_off_ms);  //
              // status = this->sendPortStatus(command, IH_SET_PORT_OFF);            
               this->addMessageDelay(command, portInfo.time_to_off_ms , IH_SET_PORT_OFF);
              }
            
        }
        
        //podwojne klikniecie...
        if(command->func_internal_number == IH_SET_PORT_DOUBLE_CLICK )
        {       
            this->clearMessageDelay(command);
            
            status =  this->sendPortStatus(command, IH_SET_PORT_ON);
            
            *portInfo.response_from_driver = true;
            *portInfo.status = status;
            
            
            if(status)
            {
            //usleep(1000 * portInfo.time_to_off_ms);  //
            //status =  this->sendPortStatus(command, IH_SET_PORT_OFF);            
              this->addMessageDelay(command, portInfo.time_to_off_ms , IH_SET_PORT_OFF);
        
            
            
           // usleep(1000 * portInfo.switch_delay_ms);  //            
           // status =  this->sendPortStatus(command, IH_SET_PORT_ON);
           
               this->addMessageDelay(command, portInfo.time_to_off_ms + portInfo.switch_delay_ms , IH_SET_PORT_ON);
            
            
           // usleep(1000 * portInfo.time_to_off_ms);  //
          //  status = this->sendPortStatus(command, IH_SET_PORT_OFF);
            
                this->addMessageDelay(command, portInfo.time_to_off_ms + portInfo.switch_delay_ms +  portInfo.time_to_off_ms , IH_SET_PORT_OFF);
            }
           
        }
 
        if(status)
        {
            this->getAppHandle()->show_log_driver("app_driver_elpiast","setPortStatusEl","set status success",7);
        };
     
       
}



//-------------------------------------------------------------------------------

 

//-------------------------------------------------------------------------------

bool app_driver_elpiast::sendPortStatus(gxh_message* command, int portStatus)
{
    
   
    
    ih_change_port_info portInfo; 
    memcpy(&portInfo, command->dataStruct, sizeof(ih_change_port_info) ); 
    
    
   //  std::cout<<"\n\n111PortId "<<portInfo.ih_driver_port_id;
    
   int numPort = -1;
    
    for(int m=0;m<this->num_ports; m++)
    {
        if(this->ports[m].id == portInfo.ih_driver_port_id) numPort = this->ports[m].num;
    }
       
    
    if(numPort == -1)
    {                
        //*portInfo->status = true; //stan portu nie zostal zmieniony....
        //*portInfo->response_from_driver = true;
        return false;
    }   
    
  
  // std::cout<<"\n\nPortId "<<this->ports[numPort].id<<" , "<<this->ports[numPort].num;
     
    char buffer[300];
    char rBuffer[300];
    
    memset(rBuffer,0,300);
    memset(buffer,0,300);
    
     app_modbus_rtu rtu; 
     rtu.setSlaveAddress(   (char)gxh_StringToInt( this->getAddressPtr() ) );   
     rtu.setFunctionMultipleRegister(); // zapisz rejestr...
     
     if(numPort == 0)  rtu.setDataAddress( 180);   
     if(numPort == 1)  rtu.setDataAddress( 182);   
     if(numPort == 2)  rtu.setDataAddress( 184);   
     if(numPort == 3)  rtu.setDataAddress( 186);   
     if(numPort == 4)  rtu.setDataAddress( 188);   
     if(numPort == 5)  rtu.setDataAddress( 190);   
     if(numPort == 6)  rtu.setDataAddress( 192);   
     if(numPort == 7)  rtu.setDataAddress( 194);   
     if(numPort == 8)  rtu.setDataAddress( 196);   
     if(numPort == 9)  rtu.setDataAddress( 198);   
     if(numPort == 10) rtu.setDataAddress( 200);   
     if(numPort == 11) rtu.setDataAddress( 202);   
     if(numPort == 12) rtu.setDataAddress( 204);   
     if(numPort == 13) rtu.setDataAddress( 206);   
     if(numPort == 14) rtu.setDataAddress( 208);   
     if(numPort == 15) rtu.setDataAddress( 210);   
     
     rtu.setNumRegisterToWrite(2); //dwa rejestry po 16 bitów
   
     if(portStatus== IH_SET_PORT_ON )
     { 
         rtu.setRegisterValue(1); //
         rtu.setRegisterValue1(0);
         
         std::string msg = "IH_SET_PORT_ON, ADDR: ";
         msg.append(this->getAddressPtr());
         msg.append(", DO: ");     
         msg.append( gxh_IntToString( this->ports[numPort].num) );        
         this->getAppHandle()->show_log_driver("app_driver_elpiast","sendPortStatus",msg.c_str(),0); 
     }else
     {
         rtu.setRegisterValue(0); //
         rtu.setRegisterValue1(0);
         
         std::string msg = "IH_SET_PORT_OFF, ADDR: ";
         msg.append(this->getAddressPtr() );
         msg.append(", DO: ");     
         msg.append( gxh_IntToString( this->ports[numPort].num) );        
         this->getAppHandle()->show_log_driver("app_driver_elpiast","sendPortStatus",msg.c_str(),0); 
     }
     
     int size = rtu.get(buffer);        
 
    
    // char buffTmp[400];        
    // memset(buffTmp,0,400);
   //  rtu->toASCII(buffer, size, buffTmp);  
  //   this->getAppHandle()->show_log_driver ("app_driver_elpiast","sendPortStatus", (const char*)buffTmp ,0); 
     
 
                             
    this->getPortHandle()->flush();      
    this->getPortHandle()->writeData(buffer,size);  // pisz do portu
    
    
   
    usleep (1000* this->getDelayBeforeRead()   );       
    
    
    int r = this->getPortHandle()->readData(rBuffer, 300);   

   // std::cout<<"\n\nOdczytano: "<<r<<"\n\n";
    

    if(r >= 8)    
    {                
             char buffTmp[300];                
             memset(buffTmp,0,300);                
             rtu.toASCII(rBuffer, r, buffTmp);  
             this->getAppHandle()->show_log_driver ("app_driver_elpiast","sendPortStatus odpowiedz...", (const char*)buffTmp ,0);    
        
        
        bool CRC16 = rtu.setResponse(rBuffer, r); //sprawdź czy otrzymana ramka zawiera właściwą sumę kontrolną.
        if(CRC16)
        {
           return true;   
        }
         else
        {
           this->getAppHandle()->show_log_driver ("app_driver_elpiast","sendPortStatus", "Odpowiedz od urzadzenia zawiera bledna sume CRC - zakładam że klikneło ??" ,0);
           return true;
         //  return false;
        }
      
        
    }
 
    
    return false;
   
}


//-------------------------------------------------------------------------------
 
//------------------------------------------------------------------------------


bool app_driver_elpiast::checkPortStatusDI16()
{     
    
   // std::cout<<"\n\n\nOK>...........................................\n";
    
    char buffer[300];
    char rBuffer[300];
    
    memset(rBuffer,0,300);
    memset(buffer,0,300);
    
     app_modbus_rtu rtu;
     rtu.setSlaveAddress(   (char)gxh_StringToInt( this->getAddressPtr() ) );   
     rtu.setFunctionInputRegister(); // odczyta input register..
     rtu.setDataAddress( 0);      //start adres...
     rtu.setNumberOfRegister(2  * 8 * 2 );   //16 dinów.. // 32 po poprawce..
     int size = rtu.get(buffer);        
     
     
    int MM = gxh_StringToInt( this->getAddressPtr() );
 
 
   
    //  app_modbus_rtu rtuB;
    //  char buffTmp[400];        
    //  memset(buffTmp,0,400);
    //  rtuB.toASCII(buffer, size, buffTmp);  
    //  this->getAppHandle()->show_log_driver ("app_driver_elpiast","checkPortStatusDI16 send", (const char*)buffTmp ,0); 
   
    
     
     
                             
    this->getPortHandle()->flush();      
    this->getPortHandle()->writeData(buffer,size);  // pisz do portu
    
          
 
    usleep (1000* this->getDelayBeforeRead()   );       
    

    
     char empty[10];
      this->getPortHandle()->readData(rBuffer, size);   //odczytaj wyslane..
 
    
    int r = this->getPortHandle()->readData(rBuffer, 300);   

   
  

    if(r>60)    
    {          
        
      //   app_modbus_rtu rtuA;
      //   std::cout<<"\nTest:: "<<r<<"\n";            
        // char buffTmp[300];                
        // memset(buffTmp,0,300);                
         //rtuA.toASCII(rBuffer, r, buffTmp);  
         //this->getAppHandle()->show_log_driver ("app_driver_elpiast","checkPortStatusDI16 crc ok, response: ", (const char*)buffTmp ,0);     
        
      bool CRC16 = rtu.setResponse(rBuffer, r); //sprawdź czy otrzymana ramka zawiera właściwą sumę kontrolną.
        
       
        
          
    
        
     //   if(CRC16)        //tutux
        {               
            
         
            
              unsigned short DI1 = rtu.getRegister(0); //przeskakujemy co 16 bitów..
              unsigned short DI2 = rtu.getRegister(2); //przeskakujemy co 16 bitów..
              unsigned short DI3 = rtu.getRegister(4); //przeskakujemy co 16 bitów..
              unsigned short DI4 = rtu.getRegister(6); //przeskakujemy co 16 bitów..
              unsigned short DI5 = rtu.getRegister(8); //przeskakujemy co 16 bitów..
              unsigned short DI6 = rtu.getRegister(10); //przeskakujemy co 16 bitów..
              unsigned short DI7 = rtu.getRegister(12); //przeskakujemy co 16 bitów..
              unsigned short DI8 = rtu.getRegister(14); //przeskakujemy co 16 bitów.. 
           
              
              unsigned short DI9 = rtu.getRegister(16); //przeskakujemy co 16 bitów.. 
              unsigned short DI10 = rtu.getRegister(18); //przeskakujemy co 16 bitów.. 
              unsigned short DI11 = rtu.getRegister(20); //przeskakujemy co 16 bitów.. 
              unsigned short DI12 = rtu.getRegister(22); //przeskakujemy co 16 bitów.. 
              unsigned short DI13 = rtu.getRegister(24); //przeskakujemy co 16 bitów.. 
              unsigned short DI14 = rtu.getRegister(26); //przeskakujemy co 16 bitów.. 
              unsigned short DI15 = rtu.getRegister(28); //przeskakujemy co 16 bitów.. 
              unsigned short DI16 = rtu.getRegister(30); //przeskakujemy co 16 bitów.. 
              
              
              
              
              if(DI1 != 0 && DI1 != this->LDI1)   this->changePortListener(0,true);  
              if(DI1 == 0 && DI1 != this->LDI1)   this->changePortListener(0,false);  
              
              if(DI2 != 0 && DI2 != this->LDI2)   this->changePortListener(1,true);  
              if(DI2 == 0 && DI2 != this->LDI2)   this->changePortListener(1,false); 
              
              if(DI3 != 0 && DI3 != this->LDI3)   this->changePortListener(2,true);  
              if(DI3 == 0 && DI3 != this->LDI3)   this->changePortListener(2,false); 
              
              if(DI4 != 0 && DI4 != this->LDI4)   this->changePortListener(3,true);  
              if(DI4 == 0 && DI4 != this->LDI4)   this->changePortListener(3,false); 
              
              if(DI5 != 0 && DI5 != this->LDI5)   this->changePortListener(4,true);  
              if(DI5 == 0 && DI5 != this->LDI5)   this->changePortListener(4,false); 
              
              if(DI6 != 0 && DI6 != this->LDI6)   this->changePortListener(5,true);  
              if(DI6 == 0 && DI6 != this->LDI6)   this->changePortListener(5,false); 
              
              if(DI7 != 0 && DI7 != this->LDI7)   this->changePortListener(6,true);  
              if(DI7 == 0 && DI7 != this->LDI7)   this->changePortListener(6,false); 
              
              if(DI8 != 0 && DI8 != this->LDI8)   this->changePortListener(7,true);  
              if(DI8 == 0 && DI8 != this->LDI8)   this->changePortListener(7,false); 
              
              
              
              if(DI9 != 0 && DI9 != this->LDI9)   this->changePortListener(8,true);  
              if(DI9 == 0 && DI9 != this->LDI9)   this->changePortListener(8,false); 
              
              if(DI10 != 0 && DI10 != this->LDI10)   this->changePortListener(9,true);  
              if(DI10 == 0 && DI10 != this->LDI10)   this->changePortListener(9,false); 
              
              if(DI11 != 0 && DI11 != this->LDI11)   this->changePortListener(10,true);  
              if(DI11 == 0 && DI11 != this->LDI11)   this->changePortListener(10,false); 
              
              if(DI12 != 0 && DI12 != this->LDI12)   this->changePortListener(11,true);  
              if(DI12 == 0 && DI12 != this->LDI12)   this->changePortListener(11,false); 
              
              if(DI13 != 0 && DI13 != this->LDI13)   this->changePortListener(12,true);  
              if(DI13 == 0 && DI13 != this->LDI13)   this->changePortListener(12,false); 
              
              if(DI14 != 0 && DI14 != this->LDI14)   this->changePortListener(13,true);  
              if(DI14 == 0 && DI14 != this->LDI14)   this->changePortListener(13,false); 
              
              if(DI15 != 0 && DI15 != this->LDI15)   this->changePortListener(14,true);  
              if(DI15 == 0 && DI15 != this->LDI15)   this->changePortListener(14,false); 
              
              if(DI16 != 0 && DI16 != this->LDI16)   this->changePortListener(15,true);  
              if(DI16 == 0 && DI16 != this->LDI16)   this->changePortListener(15,false); 
            
              
              this->LDI1 = DI1;
              this->LDI2 = DI2;
              this->LDI3 = DI3;
              this->LDI4 = DI4;
              this->LDI5 = DI5;
              this->LDI6 = DI6;
              this->LDI7 = DI7;
              this->LDI8 = DI8;
              
              this->LDI9 = DI9;
              this->LDI10 = DI10;
              this->LDI11 = DI11;
              this->LDI12 = DI12;
              this->LDI13 = DI13;
              this->LDI14 = DI14;
              this->LDI15 = DI15;
              this->LDI16 = DI16;
              
        
        }
        //else
       // {
           //  if(MM == 7) this->getAppHandle()->show_error("app_driver_elpiast","checkPortStatusDI16","CRC problem...",1);
       // }
        
    } else
    {
     //  if(MM == 13) this->getAppHandle()->show_error("app_driver_elpiast","checkPortStatusDI16","brak odpowiedzi od urzadzenia",1);
    }
    
    
 
    
    return false;
}

//-------------------------------------------------------------------------------
 

//-------------------------------------------------------------------------------

bool app_driver_elpiast::checkPortStatusDI8DO8()
{     
    
      int MM = gxh_StringToInt( this->getAddressPtr() );
    
    char buffer[300];
    char rBuffer[300];
    
    memset(rBuffer,0,300);
    memset(buffer,0,300);
    
     app_modbus_rtu rtu;
     rtu.setSlaveAddress(   (char)gxh_StringToInt( this->getAddressPtr() ) );   
     rtu.setFunctionInputRegister(); // odczyta input register..
     rtu.setDataAddress( 0);      //start adres...
     rtu.setNumberOfRegister(2  * 8);   //8 diny..
     int size = rtu.get(buffer);        
 
    
  if(MM == 13) 
    {
      char buffTmp[400];        
      memset(buffTmp,0,400);
      rtu.toASCII(buffer, size, buffTmp);  
      this->getAppHandle()->show_log_driver ("app_driver_elpiast","checkPortStatusDI8DO8", (const char*)buffTmp ,0); 
    }
     
     
                             
    this->getPortHandle()->flush();      
    this->getPortHandle()->writeData(buffer,size);  // pisz do portu
    
    usleep (1000* this->getDelayBeforeRead()   );       
    
    
    int r = this->getPortHandle()->readData(rBuffer, 300);   
        
    
    
    

    if(r>0)    
    {                           
        bool CRC16 = rtu.setResponse(rBuffer, r); //sprawdź czy otrzymana ramka zawiera właściwą sumę kontrolną.
        if(CRC16)
        {
            //std::cout<<"\n\nOdpowiedz prawidłowa";
            
            if(MM == 13)
            {
              char buffTmp[300];                
              memset(buffTmp,0,300);                
              rtu.toASCII(rBuffer, r, buffTmp);  
              this->getAppHandle()->show_log_driver ("app_driver_elpiast","checkPortStatusDI8DO8 odpowiedz...", (const char*)buffTmp ,0);              
            }
              
              unsigned short DI1 = rtu.getRegister(0); //przeskakujemy co 16 bitów..
              unsigned short DI2 = rtu.getRegister(2); //przeskakujemy co 16 bitów..
              unsigned short DI3 = rtu.getRegister(4); //przeskakujemy co 16 bitów..
              unsigned short DI4 = rtu.getRegister(6); //przeskakujemy co 16 bitów..
              unsigned short DI5 = rtu.getRegister(8); //przeskakujemy co 16 bitów..
              unsigned short DI6 = rtu.getRegister(10); //przeskakujemy co 16 bitów..
              unsigned short DI7 = rtu.getRegister(12); //przeskakujemy co 16 bitów..
              unsigned short DI8 = rtu.getRegister(14); //przeskakujemy co 16 bitów.. 
              
              if(DI1 != 0 && DI1 != this->LDI1)   this->changePortListener(0,true);  
              if(DI1 == 0 && DI1 != this->LDI1)   this->changePortListener(0,false);  
              
              if(DI2 != 0 && DI2 != this->LDI2)   this->changePortListener(1,true);  
              if(DI2 == 0 && DI2 != this->LDI2)   this->changePortListener(1,false); 
              
              if(DI3 != 0 && DI3 != this->LDI3)   this->changePortListener(2,true);  
              if(DI3 == 0 && DI3 != this->LDI3)   this->changePortListener(2,false); 
              
              if(DI4 != 0 && DI4 != this->LDI4)   this->changePortListener(3,true);  
              if(DI4 == 0 && DI4 != this->LDI4)   this->changePortListener(3,false); 
              
              if(DI5 != 0 && DI5 != this->LDI5)   this->changePortListener(4,true);  
              if(DI5 == 0 && DI5 != this->LDI5)   this->changePortListener(4,false); 
              
              if(DI6 != 0 && DI6 != this->LDI6)   this->changePortListener(5,true);  
              if(DI6 == 0 && DI6 != this->LDI6)   this->changePortListener(5,false); 
              
              if(DI7 != 0 && DI7 != this->LDI7)   this->changePortListener(6,true);  
              if(DI7 == 0 && DI7 != this->LDI7)   this->changePortListener(6,false); 
              
              if(DI8 != 0 && DI8 != this->LDI8)   this->changePortListener(7,true);  
              if(DI8 == 0 && DI8 != this->LDI8)   this->changePortListener(7,false); 
            
              
              this->LDI1 = DI1;
              this->LDI2 = DI2;
              this->LDI3 = DI3;
              this->LDI4 = DI4;
              this->LDI5 = DI5;
              this->LDI6 = DI6;
              this->LDI7 = DI7;
              this->LDI8 = DI8;
        
        } 
        
    }     
    return false;
}

//-------------------------------------------------------------------------------
 
//-------------------------------------------------------------------------------

void app_driver_elpiast::sendMsgToCore(int func_internal_number, int ih_driver_port_id)
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
                  this->getAppHandle()->show_error("app_driver_elpiast","sendMsgToCore","Cannot add message",4);
              }
              
              //end interrupt
}

//-------------------------------------------------------------------------------

void app_driver_elpiast::scrollHistory(unsigned long time, bool status, char option, int index)
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


void app_driver_elpiast::changePortListener(int num,bool status)
{
    
      std::string  msgC = " analiza zmiany stanu portu nr ";
      msgC.append(gxh_IntToString(num));
      if(status) msgC.append(" na ON ");
      if(!status) msgC.append(" na OFF ");
      
      this->getAppHandle()->show_log_driver("app_driver_elpiast","changePortListener", (char*)msgC.c_str(),5 );  
      
      
    
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
         this->getAppHandle()->show_error("app_driver_elpiast","changePortListener","0x009012112",4 );  
         // return; //drgania sprzetowe... zbyt szybko cos zostalo klikniete.. 100ms...
      }
          
    
      std::string msg2 = "";
              
      
      if(this->ports[index].reverts == '1' ) 
      {
          status = !status;        
         // std::string msgA = "Revers: ";
         // msgA.append(  gxh_IntToString( this->ports[index].num ));
         // this->getAppHandle()->show_log_driver("app_driver_elpiast","changePortListener", msgA.c_str() );   
      }
      
      if(this->ports[index].reverts == '0' ) 
      {                     
         // std::string msgA = "No revers: ";
         // msgA.append(  gxh_IntToString( this->ports[index].num ));
        //  this->getAppHandle()->show_log_driver("app_driver_elpiast","changePortListener", msgA.c_str() );    
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
               
           this->getAppHandle()->show_log_driver("app_driver_elpiast","changePortListener", (char*)msg2.c_str(),0 );   
     
  
      
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
              
              
              this->getAppHandle()->show_log_driver("app_driver_elpiast","changePortListener", (char*)msg2.c_str() ,0);   
                 
              
              
      
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
                  
                  this->getAppHandle()->show_log_driver("app_driver_elpiast","changePortListener", (char*)msg2.c_str() ,0);   
              }
                                                                                 
              
              
              this->scrollHistory(useconds, status, option, index);
          }                                                  
      }
       
  
}

 
 
//------------------------------------------------------------------------------

int app_driver_elpiast::HexPortToInt(int numPort)
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


char app_driver_elpiast::IntToHexChar(int num)
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
//------------------------------------------------------------------------------
 


 
