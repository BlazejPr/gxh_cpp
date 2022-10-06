/* 
 * File:   app_port_serial.cpp
 * Author: Blazej Kita
 * 
 * Created on 8 wrzesień 2016, 10:53
 */

#include "app_port_serial.h"



void* runThreadPort(void* portHandle)
{
    app_port_serial* core = (app_port_serial*)portHandle;
    core->runThread(core);
    return NULL;
}


app_port_serial::app_port_serial(gxh_scope* appHomeHandle,const char* nameOnComputer,const char* pathToPort,const char* portType, int ih_computer_port_id,const char* parity,const char* stop_bit, int speed,const char* data) : app_port(appHomeHandle, nameOnComputer, pathToPort,portType, ih_computer_port_id,  parity, stop_bit, speed, data)
{
    
     def_parity    = parity;
     def_stop_bit  = stop_bit;
     def_speed     = speed;
     def_data      = data;
    
   
     this->workCorrect = false;
 
  //   this->commandQueue   = new app_zc_queue(appHomeHandle);
   
    
    //opern port
     std::string tmp;
     tmp.append("\n-----------------------------------------------------\n");
     tmp.append( this->name_on_computer);   
     tmp.append(" ");
     tmp.append(this->path);
     tmp.append(" ");  
     tmp.append( gxh_IntToString(this->speed));  
     
 
    
     this->getAppHandle()->show_log_port("app_port_serial","app_port_serial","Trying to open port ",0  ); 
     this->getAppHandle()->show_log_port("app_port_serial","app_port_serial",tmp.c_str() ,0); 
    
     
    //otworz port
     
     this->fd = open ( this->path, O_RDWR | O_NOCTTY );
     if (this->fd < 0)
     {
        this->getAppHandle()->show_error("app_port_serial","app_port_serial","Cannot open serial port",0);
        this->workCorrect = false;
        return;
     }          
      
     std::cout<<"\n\n\n\nTyp: ";
     std::cout<<this->typ<<", \n\n";
             
             
     //if( strcmp(this->typ,"RS485") == 0 ) 
     if( this->typ[2] == '4' && this->typ[3] == '8' ) //rs485
     {
         this->getAppHandle()->show_log_port("app_port_serial","app_port_serial", "Change to rs485 mode",0); 
  
         if (ioctl (fd, TIOCGRS485, &rs485conf) < 0) {        
                   this->getAppHandle()->show_error("app_port_serial","app_port_serial"," TIOCGRS485 ioctl not supported",0);
         }else
         {
       
          rs485conf.flags ==  (1<<0); // SER_RS485_ENABLED;        
          rs485conf.delay_rts_before_send = 0; 
          rs485conf.delay_rts_after_send = 0;
          if (ioctl (fd, TIOCSRS485, &rs485conf) < 0) {
                // printf("Error: TIOCSRS485 ioctl not supported.\n");
                 this->getAppHandle()->show_error("app_port_serial","app_port_serial"," TIOCSRS485 ioctl not supported",3);
          }
         }

         
         fcntl(fd, F_SETFL, 0); //czyta tak dlugo poki naplywaja nowe bity...
                 
     }else //ustaw zwykły rs232
     {
       this->getAppHandle()->show_log_port("app_port_serial","app_port_serial", "Change to rs232 mode",0); 
       /*
         if (ioctl (fd, TIOCGRS485, &rs485conf) < 0) {        
                   this->getAppHandle()->show_error("app_port_serial","app_port_serial"," TIOCGRS485 ioctl not supported",0);
         }
       
         rs485conf.flags ==  (1<<1); // SER_RS485_ENABLED;        
       //  rs485conf.delay_rts_before_send = 0; 
        // rs485conf.delay_rts_after_send = 0;
         if (ioctl (fd, TIOCSRS485, &rs485conf) < 0) {
                 printf("Error: TIOCSRS485 ioctl not supported.\n");
                 this->getAppHandle()->show_error("app_port_serial","app_port_serial"," TIOCSRS485 ioctl not supported",0);
         }
       */
         fcntl(fd, F_SETFL, 0); //czyta tak dlugo poki naplywaja nowe bity...
         //fcntl(fd_serialport, F_SETFL, FNDELAY);  //zwroc wynik odczytu natychmiast.... //tu zmienic....
         
         
     }
     
     
     
     
     struct termios tty;
     
     //ustaw parametry portu
      memset (&tty, 0, sizeof tty);
      if (tcgetattr (this->fd, &tty) != 0)
      {
 
          this->workCorrect = false;
          close(this->fd);
          this->getAppHandle()->show_error("app_port_serial","app_port_serial","Cannot read termios",0);
          return;
      }
     
  
     
     //ilosc bitow danych..
     if(this->data[0] == '8')  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;   
     if(this->data[0] == '7')  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS7;   
     if(this->data[0] == '6')  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS6;   
     if(this->data[0] == '5')  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS5;   
     
     tty.c_iflag &= ~IGNBRK;         // disable break processing
     tty.c_lflag = 0;                // no signaling chars, no echo,//wejscie raw
     
     
     tty.c_oflag = 0;                // no remapping, no delays  //wyjscie raw
     tty.c_cc[VMIN]  = 0;            // read doesn't block
     tty.c_cc[VTIME] = 0;            // 0.2 seconds read timeout
     
     if( strcmp(this->typ,"RS232") == 0 ) 
     {
           tty.c_oflag = 0;                // no remapping, no delays
           tty.c_cc[VMIN]  = 0;            // read doesn't block
           tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout
     }
     
     
     
     
     tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl
     tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,// enable reading
     
     
     //test parzystosci..
     tty.c_cflag &= ~(PARENB | PARODD);                //bez parzystosci     
     if(this->parity[0] == 'e'){ tty.c_cflag |= PARENB;    this->getAppHandle()->show_log_port("app_port_serial","app_port_serial", "Change to even ",0); };   //parzystosc
     if(this->parity[0] == 'o'){ tty.c_cflag |= PARODD;     this->getAppHandle()->show_log_port("app_port_serial","app_port_serial", "Change to odd ",0);  };   //nieparzystosc
     
     tty.c_cflag &= ~CSTOPB;  //tylko jeden bit stopu
     this->getAppHandle()->show_log_port("app_port_serial","app_port_serial", "Change to 1 stop bit default ",0); 
     //if( strcmp(this->stop_bit,"1") == 0 ) { tty.c_cflag &= CSTOPB;   this->getAppHandle()->show_log_port("app_port_serial","app_port_serial", "Change to 1 stop bit ");  };     
     if( strcmp(this->stop_bit,"2") == 0 ) { tty.c_cflag &= CSTOPB;   this->getAppHandle()->show_log_port("app_port_serial","app_port_serial", "Change to 2 stop bit ",0);  };
     
     tty.c_cflag &= ~CRTSCTS;
     
     
   
     
     
     
     if(this->speed == 1200)  
     {
         cfsetospeed (&tty, B1200);  cfsetispeed (&tty, B1200);   
         this->getAppHandle()->show_log_port("app_port_serial","app_port_serial", "Set speed B1200 ! ",0); 
     }
     if(this->speed == 2400)  
     {
         cfsetospeed (&tty, B2400);  cfsetispeed (&tty, B2400);      
         this->getAppHandle()->show_log_port("app_port_serial","app_port_serial", "Set speed B2400 ! ",0); 
     }
     if(this->speed == 4800)  
     {
         cfsetospeed (&tty, B4800);  cfsetispeed (&tty, B4800);  
         this->getAppHandle()->show_log_port("app_port_serial","app_port_serial", "Set speed B4800! ",0); 
     }
     if(this->speed == 9600)  
     {
         cfsetospeed (&tty, B9600);  cfsetispeed (&tty, B9600);    
         this->getAppHandle()->show_log_port("app_port_serial","app_port_serial", "Set speed B9600! ",0); 
     }
     if(this->speed == 19200) 
     {
         cfsetospeed (&tty, B19200); cfsetispeed (&tty, B19200);   
         this->getAppHandle()->show_log_port("app_port_serial","app_port_serial", "Set speed B19200! ",0); 
     }
     if(this->speed == 38400) 
     { 
         cfsetospeed (&tty, B38400); cfsetispeed (&tty, B38400);     
         this->getAppHandle()->show_log_port("app_port_serial","app_port_serial", "Set speed B38400! ",0); 
     }
     if(this->speed == 57600) 
     { 
         cfsetospeed (&tty, B57600); cfsetispeed (&tty, B57600);    
         this->getAppHandle()->show_log_port("app_port_serial","app_port_serial", "Set speed B57600! ",0); 
     }
     if(this->speed == 115200)
     {
         cfsetospeed (&tty, B115200);  cfsetispeed (&tty, B115200);   
         this->getAppHandle()->show_log_port("app_port_serial","app_port_serial", "Set speed B115200! ",0); 
     }
     
     
     //-----------------------------------
   
      
     //-----------------------------------
     
     
     
     if (tcsetattr (fd, TCSANOW, &tty) != 0)
     {
       
          this->workCorrect = false;          
          close(this->fd);
          this->getAppHandle()->show_error("app_port_serial","app_port_serial","Cannot save termios",0);
          return;
     }else
     {
          this->getAppHandle()->show_log_port("app_port_serial","app_port_serial", "Data termios has been saved! ",0); 
     }
     
      
    //koniec ustawien portu...  
     this->workCorrect = true;
     
     
    this->getAppHandle()->show_log_port("app_port_serial","app_port_serial","Loading drivers... ",0);   
    this->loadDrivers();
     
    
    //--------------------------------------------------------------------------
}

app_port_serial::~app_port_serial() {
      
    if(  this->workCorrect == false)
    {
        this->getAppHandle()->show_log_port("app_port_serial","~app_port_serial","Destroy port...................100%  " ,0);  
        return;
    }
    
    this->deleteDrivers();       
    close(this->fd);
    
    this->getAppHandle()->show_log_port("app_port_serial","~app_port_serial","Destroy port...................100%  " ,0);      
}

//------------------------------------------------------------------------------

void app_port_serial::startLoop()
{
    this->goRunning = true;
    
    if(this->workCorrect)
    {
        //create main thread of core
        this->getAppHandle()->show_log_port("app_port_serial","app_port_serial","Creating core thread",0);    
       
    
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); //po zakonczeniu zwolni sie od razu pamiec...
    
        this->goRunning = true;
    
        int rc = pthread_create(&this->mainThread, &attr,  &runThreadPort, (void*)this );
        if (rc)
         {
           this->workCorrect = false;
           close(this->fd);
           this->getAppHandle()->show_error( "app_port_serial","app_port_serial","Unable to create core thread!" ,0);       
           pthread_attr_destroy(&attr);
           return;
         }else
         {
              pthread_detach( this->mainThread );//2022-03
              pthread_setname_np(this->mainThread,"RS");
         }
    
         pthread_attr_destroy(&attr); 
    }else
    {
         this->getAppHandle()->show_error( "app_port_serial","app_port_serial","Unable to create core thread! 0x990123" ,0);   
    }
}

//------------------------------------------------------------------------------

void app_port_serial::stopLoop()
{     
    if(this->workCorrect)
    {
         this->getAppHandle()->show_log_port("app_port_serial","~app_port_serial","Destroy port " ,0 ); 
         this->getAppHandle()->show_log_port("app_port_serial","~app_port_serial",this->name_on_computer,0  ); 
                 
         this->goRunning = false;
         usleep(1000 * 100); 
         pthread_cancel(this->mainThread);       
         usleep(1000 * 100); 
    }
}

//------------------------------------------------------------------------------

void app_port_serial::checkPortDebug()
{
    
}

//------------------------------------------------------------------------------





void app_port_serial::loadDrivers() 
{    
    this->countDrivers = 0;
    
    this->driver_handles = NULL;
    this->countDriverHandles = 0;
    
    //------------
     
     
    this->getDrivers(this->getIhComputerPortId(), &this->countDrivers, 0, this->drivers);    //pobierz wszystkie urzadzanie od adresu 00 do 99
    
    
    int countTmpDriver = 0;
    ih_driver tmpDriver[10];
    this->getDrivers( this->getIhComputerPortId(), &countTmpDriver, 1,tmpDriver );    
    
    
    this->driver_handles = new ih_driver_handle[countTmpDriver];
    this->countDriverHandles = countTmpDriver;
    
     
   
    for(int i=0; i<this->countDriverHandles; i++)  
    {
        
        this->driver_handles[i].loaded = false; 
        memcpy(this->driver_handles[i].module, tmpDriver[i].module, strlen( tmpDriver[i].module) );
        
        std::string moduleName= this->getAppHandle()->getRunPath();
        moduleName.append( tmpDriver[i].module  );
        
        
         this->driver_handles[i].handle = dlopen (moduleName.c_str(), RTLD_LAZY);
        if (!this->driver_handles[i].handle) 
        {
              this->getAppHandle()->show_error("app_port_serial","loadDrivers"," Cannot load shared library:",0);       
              std::cout<<"Error:"<<dlerror();
              this->getAppHandle()->show_error("app_port_serial","loadDrivers",moduleName.c_str(),0);       
        }else
        {

          this->driver_handles[i].getModule = (cosine_func1_ptr)dlsym(this->driver_handles[i].handle, "get_module");  
          if ((this->driver_handles[i].error = dlerror()) != NULL)  
          {
               this->getAppHandle()->show_error( "app_port_serial","loadDrivers","Cannot get function from driver",0);             
          }else
          {               
              this->driver_handles[i].loaded = true;  
 
              //sprawdz czy moduł ma zgłaszać jakieś OFF
              //select   ih_driver_port_function.id from ih_driver_port_function, ih_driver_port, ih_driver WHERE ih_driver_port_function.ih_driver_port_id=ih_driver_port.id AND ih_driver_port.ih_driver_id=ih_driver.id  AND ih_driver.is_delete='0' AND ih_driver_port_function.is_delete='0' AND ih_driver_port.is_delete='0' AND (ih_driver_port_function.port_type='1' OR ih_driver_port_function.port_type='b' OR ih_driver_port_function.port_type='4' OR ih_driver_port_function.port_type='g')  AND ih_driver.id='63'
          }                 
           
        } 
        
    }
     
    this->driver_list = new driver_item[this->countDrivers]; //tablica sterownikow urzadzen..
 
    
    for(int w=0; w<this->countDrivers; w++) 
    {
        this->driver_list[w].loaded = false;
        this->driver_list[w].gxhHandle = -1;
        this->driver_list[w].drivers = &this->drivers[w];
        
        
        for(int ix=0; ix<this->countDriverHandles; ix++)  
        {             
           
           if(  gxh_compare( (const char*)this->driver_handles[ix].module, (const char*)this->driver_list[w].drivers->module)==0   && this->driver_handles[ix].loaded )       
           {
                
               //this->getAppHandle()->show_log_port((char* )"app_port_serial",(char* )"loadModule",(char* )"Loaded......");   
               //this->getAppHandle()->show_log_port((char* )"app_port_serial",(char* )"loadModule",(char* )this->driver_list[w].drivers->name.c_str());   
               
               this->driver_list[w].driver_moudles = this->driver_handles[ix].getModule(this->getAppHandle(), this, this->driver_list[w].drivers->id, this->driver_list[w].drivers->name,this->driver_list[w].drivers->address, this->driver_list[w].drivers->delay_before_read_ms ) ;                                              
               this->driver_list[w].loaded = true;             
               
               std::string msgD = "Module ";
               msgD.append( this->driver_list[w].drivers->address);              
               this->driver_list[w].gxhHandle =  this->getAppHandle()->createGXHHandle(msgD.c_str(),this->driver_list[w].drivers->id, 1);
               
               ix = this->countDriverHandles+1;
           }
           
        }
        
    }
    
  
  
}

void app_port_serial::deleteDrivers() 
{    
 
   int tmpC = this->countDrivers;
   this->countDrivers = 0;
   

    
   for(int i=0; i<tmpC; i++)  
    {       
         if(this->driver_list[i].loaded )
         {  
             this->driver_list[i].loaded = false; 
             this->getAppHandle()->destroyGXHHandle(  this->driver_list[i].gxhHandle );                            
             delete this->driver_list[i].driver_moudles;             
         }
    }
   
    
   delete[] this->driver_list;
    
         
     
    
    for(int i=0; i<this->countDriverHandles; i++)  
    {              
        if(this->driver_handles[i].loaded == false) continue;
        
        dlclose(  this->driver_handles[i].handle );
        this->driver_handles[i].loaded = false;                
    }    
      
                         
    this->countDriverHandles = 0; 
    
    delete[] this->driver_handles;
     
  
}


 



int app_port_serial::readData(char* buffer, int size)
{
    int n = read (this->fd, buffer, size); 
    return n;
}

int app_port_serial::writeData(char* buffer, int size)
{
    return write (this->fd, buffer, size);     
}

void app_port_serial::flush()
{
    tcflush(this->fd,TCIOFLUSH);
}

bool app_port_serial::changePortParameter( const char* parity, const char* stop_bit, int speed, const char* data)
{
    return this->changePortParameter(parity[0],stop_bit[0],speed, data[0]);
}

bool app_port_serial::changePortParameter( char parity, char stop_bit, int speed, char data)
{
       
       std::string msg = "set new parameter for port  ";                                            
       msg.append( gxh_CharToString( data) );       
       msg.append( ", " );
          
       msg.append( gxh_CharToString( parity ) );       
       msg.append( "," );
              
       msg.append( gxh_CharToString( stop_bit ) );       
       msg.append( " " );
              
       msg.append( gxh_IntToString(speed) );       
    
       this->getAppHandle()->show_log_port("app_port_serial","changePortParameter",msg.c_str(),9);   
       
       
       //zamknij akutalny port...
       close(this->fd);
       
       
       //nowe ustawienia portu...
       
       
     this->fd = open ( this->path, O_RDWR | O_NOCTTY );
     if (this->fd < 0)
     {
        this->getAppHandle()->show_error("app_port_serial","changePortParameter","Cannot open serial port",0);     
        return false;
     }          
 
     
     
     if( strcmp(this->typ,"RS485") == 0 ) 
     {
        // this->getAppHandle()->show_log_port("app_port_serial","changePortParameter", "Change to rs485 mode"); 
  
         if (ioctl (fd, TIOCGRS485, &rs485conf) < 0) {        
                   this->getAppHandle()->show_error("app_port_serial","changePortParameter"," TIOCGRS485 ioctl not supported",9);
         }
         else
         {
           rs485conf.flags ==  (1<<0); // SER_RS485_ENABLED;        
           rs485conf.delay_rts_before_send = 0; 
           rs485conf.delay_rts_after_send = 0;
           if (ioctl (fd, TIOCSRS485, &rs485conf) < 0) {
                // printf("Error: TIOCSRS485 ioctl not supported.\n");
                 this->getAppHandle()->show_error("app_port_serial","changePortParameter"," TIOCSRS485 ioctl not supported",9);
           }

           fcntl(fd, F_SETFL, 0);
         }       
     }else //ustaw zwykły rs232
     {
        // this->getAppHandle()->show_log_port("app_port_serial","changePortParameter", "Change to rs232 mode"); 
  
         if (ioctl (fd, TIOCGRS485, &rs485conf) < 0) {        
                   this->getAppHandle()->show_error("app_port_serial","changePortParameter"," TIOCGRS485 ioctl not supported",9);
         }
       
         rs485conf.flags ==  (1<<1); // SER_RS485_ENABLED;        
       //  rs485conf.delay_rts_before_send = 0; 
        // rs485conf.delay_rts_after_send = 0;
         if (ioctl (fd, TIOCSRS485, &rs485conf) < 0) {
               //  printf("Error: TIOCSRS485 ioctl not supported.\n");
                 this->getAppHandle()->show_error("app_port_serial","changePortParameter"," TIOCSRS485 ioctl not supported",9);
         }

         fcntl(fd, F_SETFL, 0);
         
         
     }
     
     
     
     
     struct termios tty;
     
     //ustaw parametry portu
      memset (&tty, 0, sizeof tty);
      if (tcgetattr (this->fd, &tty) != 0)
      {
          this->goRunning = false;
          close(this->fd);
          this->getAppHandle()->show_error("app_port_serial","changePortParameter","Cannot read termios",0);
          return false;
      }
     
     
   
     
     //ilosc bitow danych..
        
     //ilosc bitow danych..
     if(data == '8')  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;   
     if(data == '7')  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS7;   
     if(data == '6')  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS6;   
     if(data == '5')  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS5;   
     
     tty.c_iflag &= ~IGNBRK;         // disable break processing
     tty.c_lflag = 0;                // no signaling chars, no echo,
     
     
     tty.c_oflag = 0;                // no remapping, no delays
     tty.c_cc[VMIN]  = 0;            // read doesn't block
     tty.c_cc[VTIME] = 0;            // 0.2 seconds read timeout
     
     tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl
     tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,// enable reading
     
     
     //test parzystosci..
     tty.c_cflag &= ~(PARENB | PARODD);                //bez parzystosci     
     if(parity == 'e'){ tty.c_cflag |= PARENB;    };   //parzystosc
     if(parity == 'o'){ tty.c_cflag |= PARODD;    };   //nieparzystosc
     
     tty.c_cflag &= ~CSTOPB;  //tylko jeden bit stopu
   
     //if( strcmp(this->stop_bit,"1") == 0 ) { tty.c_cflag &= CSTOPB;    };     
     if( stop_bit == '2' ) { tty.c_cflag &= CSTOPB;   };
     
     tty.c_cflag &= ~CRTSCTS;
     
     
     if(speed == 1200)  { cfsetospeed (&tty, B1200);  cfsetispeed (&tty, B1200);   }
     if(speed == 2400)  { cfsetospeed (&tty, B2400);  cfsetispeed (&tty, B2400);      }
     if(speed == 4800)  { cfsetospeed (&tty, B4800);  cfsetispeed (&tty, B4800);  }
     if(speed == 9600)  { cfsetospeed (&tty, B9600);  cfsetispeed (&tty, B9600);    }
     if(speed == 19200) { cfsetospeed (&tty, B19200); cfsetispeed (&tty, B19200);   }
     if(speed == 38400) { cfsetospeed (&tty, B38400); cfsetispeed (&tty, B38400);     }
     if(speed == 57600) { cfsetospeed (&tty, B57600); cfsetispeed (&tty, B57600);    }
     if(speed == 115200){ cfsetospeed (&tty, B115200);  cfsetispeed (&tty, B115200);   }
     
     
     if (tcsetattr (fd, TCSANOW, &tty) != 0)
     {
          this->goRunning = false;
          close(this->fd);
          this->getAppHandle()->show_error("app_port_serial","changePortParameter","Cannot save termios",0);
          return false;
     }
     
     
     
     
    //koniec ustawien portu...  
       
       
    return false;
}


bool app_port_serial::setDefaultPortParameter( )
{
    /*
       std::string msg = "set default parameter for port  ";                               
       msg.append( this->def_data );
       msg.append( ", " );
       msg.append( this->def_parity );
       msg.append( "," );
       msg.append( this->def_stop_bit );
       msg.append( " " );
       
       char* tmpSpeed = gxh_IntToString(this->def_speed);
       msg.append( tmpSpeed  );
       
       delete[] tmpSpeed;
    
       this->getAppHandle()->show_log_port("app_port_serial","setDefaultPortParameter",msg.c_str());   
      */ 
       
       
         //zamknij akutalny port...
       close(this->fd);
       
       
       //nowe ustawienia portu...
       
       
     this->fd = open ( this->path, O_RDWR | O_NOCTTY | O_SYNC);
     if (this->fd < 0)
     {
        this->getAppHandle()->show_error("app_port_serial","setDefaultPortParameter","Cannot open serial port",9);        
        return false;
     }          
     
     
     if( strcmp(this->typ,"RS485") == 0 ) 
     {
        // this->getAppHandle()->show_log_port("app_port_serial","setDefaultPortParameter", "Change to rs485 mode"); 
  
         if (ioctl (fd, TIOCGRS485, &rs485conf) < 0) {        
                   this->getAppHandle()->show_error("app_port_serial","setDefaultPortParameter"," TIOCGRS485 ioctl not supported",9);
         }
       
         rs485conf.flags ==  (1<<0); // SER_RS485_ENABLED;        
         rs485conf.delay_rts_before_send = 0; 
         rs485conf.delay_rts_after_send = 0;
         if (ioctl (fd, TIOCSRS485, &rs485conf) < 0) {
               //  printf("Error: TIOCSRS485 ioctl not supported.\n");
                 this->getAppHandle()->show_error("app_port_serial","setDefaultPortParameter"," TIOCSRS485 ioctl not supported",9);
         }

         fcntl(fd, F_SETFL, 0);
                 
     }else //ustaw zwykły rs232
     {
       //  this->getAppHandle()->show_log_port("app_port_serial","setDefaultPortParameter", "Change to rs232 mode"); 
  
         if (ioctl (fd, TIOCGRS485, &rs485conf) < 0) {        
                   this->getAppHandle()->show_error("app_port_serial","setDefaultPortParameter"," TIOCGRS485 ioctl not supported",9);
         }
       
         rs485conf.flags ==  (1<<1); // SER_RS485_ENABLED;        
       //  rs485conf.delay_rts_before_send = 0; 
        // rs485conf.delay_rts_after_send = 0;
         if (ioctl (fd, TIOCSRS485, &rs485conf) < 0) {
                 //printf("Error: TIOCSRS485 ioctl not supported.\n");
                 this->getAppHandle()->show_error("app_port_serial","setDefaultPortParameter"," TIOCSRS485 ioctl not supported",9);
         }

         fcntl(fd, F_SETFL, 0);
         
         
     }
     
     
     
     
     struct termios tty;
     
     //ustaw parametry portu
      memset (&tty, 0, sizeof tty);
      if (tcgetattr (this->fd, &tty) != 0)
      {
          this->goRunning = false;
          this->getAppHandle()->show_error("app_port_serial","setDefaultPortParameter","Cannot read termios",9);
          return false;
      }
     
   
     
 
     //ilosc bitow danych..
     if(this->def_data[0] == '8')  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;   
     if(this->def_data[0]  == '7')  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS7;   
     if(this->def_data[0]  == '6')  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS6;   
     if(this->def_data[0]  == '5')  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS5;   
     
     tty.c_iflag &= ~IGNBRK;         // disable break processing
     tty.c_lflag = 0;                // no signaling chars, no echo,
     
     
     tty.c_oflag = 0;                // no remapping, no delays
     tty.c_cc[VMIN]  = 0;            // read doesn't block
     tty.c_cc[VTIME] = 0;            // 0.2 seconds read timeout
     
     tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl
     tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,// enable reading
     
     
     //test parzystosci..
     tty.c_cflag &= ~(PARENB | PARODD);                //bez parzystosci     
     if(this->def_parity[0] == 'e'){ tty.c_cflag |= PARENB;    };   //parzystosc
     if(this->def_parity[0] == 'o'){ tty.c_cflag |= PARODD;     };   //nieparzystosc
     
     tty.c_cflag &= ~CSTOPB;  //tylko jeden bit stopu    
     //if( strcmp(this->stop_bit,"1") == 0 ) { tty.c_cflag &= CSTOPB;   this->getAppHandle()->show_log_port("changePortParameter","app_port_serial", "Change to 1 stop bit ");  };     
     if( this->def_stop_bit[0] == '2' ) { tty.c_cflag &= CSTOPB;     };
     
     tty.c_cflag &= ~CRTSCTS;
 
       
     if(this->def_speed == 1200)  { cfsetospeed (&tty, B1200);  cfsetispeed (&tty, B1200);   }
     if(this->def_speed == 2400)  { cfsetospeed (&tty, B2400);  cfsetispeed (&tty, B2400);   }
     if(this->def_speed == 4800)  { cfsetospeed (&tty, B4800);  cfsetispeed (&tty, B4800);   }
     if(this->def_speed == 9600)  { cfsetospeed (&tty, B9600);  cfsetispeed (&tty, B9600);   }
     if(this->def_speed == 19200) { cfsetospeed (&tty, B19200); cfsetispeed (&tty, B19200);   }
     if(this->def_speed == 38400) { cfsetospeed (&tty, B38400); cfsetispeed (&tty, B38400);   }
     if(this->def_speed == 57600) { cfsetospeed (&tty, B57600); cfsetispeed (&tty, B57600);   }
     if(this->def_speed == 115200){ cfsetospeed (&tty, B115200);  cfsetispeed (&tty, B115200);   }
     
     if (tcsetattr (fd, TCSANOW, &tty) != 0)
     {
          this->goRunning = false;
          this->getAppHandle()->show_error("app_port_serial","setDefaultPortParameter","Cannot save termios",9);
          return false;
     }
     
     
     
     
    //koniec ustawien portu... 
     
       
       
    
    return true;
}
  
bool app_port_serial::checkParameter(const char* parity, const char* stop_bit, int speed, const char* data)
{
    if(this->def_parity[0] != parity[0] || this->def_stop_bit[0] != stop_bit[0] || this->def_speed != speed || this->def_data[0] != data[0] )return true;        
    return false;
}


 
void* app_port_serial::runThread(app_port* appPort) 
{
 
 
 this->getAppHandle()->show_log_port("app_port_serial","runThread", "start...",1);
 
 if( this->countDrivers   == 0 ) this->goRunning = false;
    
 while(this->goRunning)   
 {
        
     //pierwsza połówka..
      for(int i=0; i< (int)(this->countDrivers/2) ; i++ ) 
      {          
          if(!this->goRunning) return NULL;
          
          if(!this->driver_list[i].loaded) continue;
          
          if(this->driver_list[i].driver_moudles->getOperationType() == APP_DRIVER_EXECUTE_RESUME ||  this->driver_list[i].driver_moudles->getOperationType() == APP_DRIVER_EXECUTE_ONLY )  
          {
            
              gxh_message msgCommand;               
              while(this->getAppHandle()->receiveGXHMessage(this->driver_list[i].gxhHandle,&msgCommand,0 ))
              {
                if(this->driver_list[i].loaded )
                {             
                 // bool changed = this->checkParameter(this->driver_list[i].drivers->parity.c_str(),this->driver_list[i].drivers->stop_bit.c_str(), this->driver_list[i].drivers->speed, this->driver_list[i].drivers->data.c_str() ) ;
                //  if(changed) this->changePortParameter( this->driver_list[i].drivers->parity.c_str(),this->driver_list[i].drivers->stop_bit.c_str(), this->driver_list[i].drivers->speed, this->driver_list[i].drivers->data.c_str() ) ;               
                
                  this->driver_list[i].driver_moudles->execute(&msgCommand);
         
                //  if(changed) this->setDefaultPortParameter();
                  if(!this->goRunning) return NULL;
                 }                                                    
              }
           
          }
       
          
        if(this->driver_list[i].driver_moudles->getOperationType() == APP_DRIVER_EXECUTE_RESUME ||  this->driver_list[i].driver_moudles->getOperationType() == APP_DRIVER_RESUME_ONLY )  
        {
          if(this->driver_list[i].loaded )
          {             
             //  bool isCheck = false;
              
             //  bool changed = this->checkParameter(this->driver_list[i].drivers->parity.c_str(),this->driver_list[i].drivers->stop_bit.c_str(), this->driver_list[i].drivers->speed, this->driver_list[i].drivers->data.c_str() ) ;
              // if(changed) this->changePortParameter( this->driver_list[i].drivers->parity.c_str(),this->driver_list[i].drivers->stop_bit.c_str(), this->driver_list[i].drivers->speed, this->driver_list[i].drivers->data.c_str() ) ;
                  //isCheck = this->driver_list[i].driver_moudles->resume();  
                  this->driver_list[i].driver_moudles->resume();  
             //  if(changed) this->setDefaultPortParameter();         
                  
                  if(!this->goRunning) return NULL;
          }                               
        }
        
      }
     
     if(!this->goRunning) return NULL; 
      
    
      //druga połówka..
      for(int i=(int)(this->countDrivers/2) ; i< this->countDrivers ; i++ ) 
      {          
          if(!this->driver_list[i].loaded) continue;
          
          
          if(this->driver_list[i].driver_moudles->getOperationType() == APP_DRIVER_EXECUTE_RESUME ||  this->driver_list[i].driver_moudles->getOperationType() == APP_DRIVER_EXECUTE_ONLY )  
          {
            
              gxh_message msgCommand;                    
              while(this->getAppHandle()->receiveGXHMessage(this->driver_list[i].gxhHandle,&msgCommand,0 ))
              {
                if(this->driver_list[i].loaded )
                {             
                 // bool changed = this->checkParameter(this->driver_list[i].drivers->parity.c_str(),this->driver_list[i].drivers->stop_bit.c_str(), this->driver_list[i].drivers->speed, this->driver_list[i].drivers->data.c_str() ) ;
                 // if(changed) this->changePortParameter( this->driver_list[i].drivers->parity.c_str(),this->driver_list[i].drivers->stop_bit.c_str(), this->driver_list[i].drivers->speed, this->driver_list[i].drivers->data.c_str() ) ;                                  
                  this->driver_list[i].driver_moudles->execute(&msgCommand);                     
                 // if(changed) this->setDefaultPortParameter();
                  
                   if(!this->goRunning) return NULL;
                 }                                                  
              } 
            
          }
       
          
        if(this->driver_list[i].driver_moudles->getOperationType() == APP_DRIVER_EXECUTE_RESUME ||  this->driver_list[i].driver_moudles->getOperationType() == APP_DRIVER_RESUME_ONLY )  
        {
          if(this->driver_list[i].loaded )
          {             
             //  bool isCheck = false;
              
            //   bool changed = this->checkParameter(this->driver_list[i].drivers->parity.c_str(),this->driver_list[i].drivers->stop_bit.c_str(), this->driver_list[i].drivers->speed, this->driver_list[i].drivers->data.c_str() ) ;
              // if(changed) this->changePortParameter( this->driver_list[i].drivers->parity.c_str(),this->driver_list[i].drivers->stop_bit.c_str(), this->driver_list[i].drivers->speed, this->driver_list[i].drivers->data.c_str() ) ;
                //  isCheck = this->driver_list[i].driver_moudles->resume();  
                  this->driver_list[i].driver_moudles->resume();  
              // if(changed) this->setDefaultPortParameter();         
                  
               if(!this->goRunning) return NULL;
          }                               
        }
        
      }
      
     if(!this->goRunning) return NULL;
    
     if( this->countDrivers  < 2  )  
     {
         usleep(1000 * 200 );         
     } 
     else
     if( this->countDrivers == 2  )  
     {
         usleep(1000 * 100 );          
     }     
     else
     if(  this->countDrivers == 3  )  
     {
         usleep(1000 * 50 );           
     } 
     else 
     {
         if(!this->goRunning) return NULL;
         usleep(1000 * DELAY_PORT_SERIAL );  
     }         
    
  }  
 
    this->getAppHandle()->show_log_port("app_port_serial","runThread", "stop...",1);
   
    
    return NULL;
}

//------------------------------------------------------------------------------

void app_port_serial::getDrivers(int ih_computer_port_id,int* count, int type, ih_driver* rows ) 
{         
    
    sqlite3* dbSqLite;    
 
    
    std::string baza = this->getAppHandle()->getDB();//this->getAppHandle()->getRunPath();
    //baza.append("../gxh_base.db");
     
     int r  =  sqlite3_open_v2(baza.c_str(), &dbSqLite, SQLITE_OPEN_READWRITE, NULL   );                                  
     if( r != SQLITE_OK )
     {
          this->getAppHandle()->show_error("app_port_serial","getDrivers","Cannot open sqlLite 1",0);   
          return;
     }
    
    
        //versja..
     std::string version = this->getAppHandle()->getVersion(); //5.0 || 5.1 itd..
     const char* verTmp = version.c_str();
     char verBuf[5];
     int ixc =0;
     memset(verBuf,0,5);
     for(unsigned int cx=0; cx<strlen(verTmp); cx++) 
     {
         if(verTmp[cx] == '.') break;
         verBuf[ixc] = verTmp[cx];
         ixc++;
     }
     
     //version..
     
    // std::string queryM = "SELECT module FROM ih_sl_const_function_symbol WHERE is_delete='0' AND used='1' AND app_min_version LIKE '";  
     
    
    std::string query="";
    std::string queryTmp="";
   
    if(type == 0)
    {
      query.append("SELECT ih_driver.id,ih_driver.address, ih_driver.name, ih_sl_driver.const_function_driver, ih_sl_driver.module, ih_driver.delay_before_read_ms, ih_driver.parity, ih_driver.stop_bit, ih_driver.speed, ih_driver.data   FROM ih_driver, ih_sl_driver WHERE ih_driver.is_delete='0'  AND ih_driver.is_active='1'  AND ih_driver.ih_sl_driver_id=ih_sl_driver.id AND ih_driver.ih_computer_port_id='");
      query.append(  gxh_IntToString(ih_computer_port_id) );
      query.append( "' AND ih_sl_driver.app_min_version LIKE '");      
      query.append(verBuf);
      query.append( "%' ORDER BY address ASC; ");      
    }
    
    if( type == 1)
    {     
      query.append("SELECT   ih_sl_driver.module FROM ih_driver, ih_sl_driver WHERE ih_driver.is_delete='0' AND ih_driver.is_active='1'  AND  ih_driver.ih_sl_driver_id=ih_sl_driver.id AND ih_driver.ih_computer_port_id='");
      query.append( gxh_IntToString(ih_computer_port_id));
      query.append( "' AND ih_sl_driver.app_min_version LIKE '");      
      query.append(verBuf);
      query.append("%' GROUP BY ih_sl_driver.module; ");  
    }
  
    
    
    this->getAppHandle()->show_log_data_base("app_port_serial","getDrivers", query.c_str() ,6);    
    this->getAppHandle()->show_log_data_base("app_port_serial","getDrivers", queryTmp.c_str() ,6);    
   
     
    *count = 0;
 
     
        
    sqlite3_stmt* stmt; 
    
    const char* queryChar = query.c_str();    
    if( sqlite3_prepare(  dbSqLite, queryChar , strlen(queryChar) , &stmt, NULL) != SQLITE_OK) 
    {
        this->getAppHandle()->show_error("app_port_serial","getDrivers", queryChar,0);
        *count = 0;   
        sqlite3_finalize(stmt);   
        sqlite3_close(dbSqLite);
        return;       
    }
   
    bool done = false;
    
    
    while (!done)               
    { 
       int res = sqlite3_step (stmt);      
       
         if(res ==  SQLITE_ROW)
         {                             
            if(type == 0)
            {
                
             rows[*count].id                     = gxh_StringToInt( std::string((const char*)sqlite3_column_text(stmt, 0)   ) );      
             
             const char* tmp1                    = (const char*)sqlite3_column_text(stmt, 1);
             memset(rows[*count].address,0,10);
             memcpy(rows[*count].address, tmp1, strlen(tmp1));
             
             const char* tmp2                    = (const char*)sqlite3_column_text(stmt, 2);
             memset(rows[*count].name,0,50);
             memcpy(rows[*count].name, tmp2, strlen(tmp2));
             
             rows[*count].const_function_driver  = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 3)   ) );      
             
             const char* tmp3                    = (const char*)sqlite3_column_text(stmt, 4);
             memset(rows[*count].module,0,50);
             memcpy(rows[*count].module, tmp3, strlen(tmp3));
             
             rows[*count].delay_before_read_ms   = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 5)   ) );      
             
             const char* tmp4                    = (const char*)sqlite3_column_text(stmt, 6);
             memset(rows[*count].parity,0,2);
             memcpy(rows[*count].parity, tmp4, strlen(tmp4) );
             
             const char* tmp5                    = (const char*)sqlite3_column_text(stmt, 7);
             memset(rows[*count].stop_bit,0,2);
             memcpy(rows[*count].stop_bit, tmp5, strlen(tmp5) );
             
             rows[*count].speed                  = gxh_StringToInt( std::string( (const char*)sqlite3_column_text(stmt, 8)  ) );  
             
             
             const char* tmp6                    = (const char*)sqlite3_column_text(stmt, 9);
             memset(rows[*count].data,0,2);
             memcpy(rows[*count].data, tmp6, strlen(tmp6));
            }
                                
            if(type == 1)
            {      
               const char* tmp7                    = (const char*)sqlite3_column_text(stmt, 0);
               memset(rows[*count].module,0,50);
               memcpy(rows[*count].module, tmp7, strlen(tmp7));
            
            }
              
            
            (*count)++;
        } 
           
        if(res == SQLITE_DONE)done = true;                            
        
    }
 
  
    sqlite3_finalize(stmt);   
    sqlite3_close(dbSqLite);
}

 
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------


