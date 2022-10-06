
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   app_modbus_rtu.cpp
 * Author: Blazej Kita
 * 
 * Created on 27 lutego 2017, 08:18
 */

#include "app_modbus_rtu.h"


app_modbus_rtu::app_modbus_rtu( ) {
    
     
    
}


app_modbus_rtu::~app_modbus_rtu() {
      
 
    
}

 


unsigned short app_modbus_rtu::CRC16 (const char *nData, int wLength)
{
    static const unsigned short wCRCTable[] = {
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
    0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
    0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
    0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
    0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
    0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
    0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
    0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
    0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
    0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
    0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
    0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
    0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
    0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
    0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
    0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
    0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
    0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
    0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
    0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
    0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
    0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
    0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
    0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
    0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
    0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
    0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
    0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
    0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
    0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
    0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
    0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040 };

    unsigned char nTemp;
    unsigned short wCRCWord = 0xFFFF;

    while (wLength--)
    {
        nTemp = *nData++ ^ wCRCWord;
        wCRCWord >>= 8;
        wCRCWord  ^= wCRCTable[(nTemp & 0xFF)];
    }
    return wCRCWord;
} // End: CRC16




void app_modbus_rtu::setSlaveAddress(char addr)
{
    this->slaveAddress = addr;
}


void app_modbus_rtu::setDataAddress(unsigned short addr)
{
    this->dataAddress = addr ;
}

void app_modbus_rtu::setNumberOfCoils(unsigned short number)
{
    this->totalCoils = number;
}

void app_modbus_rtu::setNumberOfRegister(unsigned short number)
{
    this->totalCoils = number;
}

void app_modbus_rtu::setNumRegisterToWrite(unsigned long int value)
{
    this->numRegisterToWrite = value;
}



void app_modbus_rtu::setFunctionReadCoil() 
{
    this->function = 1;
}

void app_modbus_rtu::setFunctionReadInput() 
{
    this->function = 2;
}


void app_modbus_rtu::setFunctionInputHoldingRegister() 
{
    this->function =3;
}

void app_modbus_rtu::setFunctionInputRegister() 
{
    this->function =4;
}


void app_modbus_rtu::setFunctionForceSingleCoil() 
{
    this->function = 5;
}

void app_modbus_rtu::setFunctionMultipleRegister() 
{
    this->function = 16;
}



void app_modbus_rtu::setFunctionForceSingleRegister() 
{
    this->function = 6;
}

std::string app_modbus_rtu::IntToHex(int number)
{
    std::string hexStr;

    /// integer value to hex-string
    std::stringstream sstream;
    sstream << "0x"
            << std::setfill ('0') << std::setw(2)
    << std::hex << (int)number;

    hexStr= sstream.str();
    sstream.clear();    //clears out the stream-string

    return  hexStr;
}


void app_modbus_rtu::toASCII(char* buff, int size, char* buffOut)
{
 
   
    
   char ascii[2024];
   int index = 0;
    
   for(int w=0; w<size; w++)
   {
       std::string hexRet = this->IntToHex(buff[w]);
       const char* tmp = hexRet.c_str();
       
       for(unsigned int i=0; i<std::strlen(tmp);i++)
       {
           ascii[index] = tmp[i];
           index++;
       }        
       
     //  ascii[index] = ' ';
     //  ascii[index+1] = '|';
     //  ascii[index+2] = ' ';
      // index+=3;;
   }    
   ascii[index] = 0;      
    
   memcpy(buffOut, ascii, index );
}

 

unsigned short app_modbus_rtu::getRegister(int number_of_register)
{
 
    
    
    int index = number_of_register*2;
    convert um;
    um.znaki[1] = this->resData[index];    
    um.znaki[0] = this->resData[index+1];    
    
    return um.us;
}


bool app_modbus_rtu::getCoilStatus(int number_of_byte,int number_of_bit)
{        
    char tmp = this->resData[number_of_byte];
       
    if( ( number_of_bit == 0 ) && ( (tmp & 0x00000001) == 0x00000001 ) ) return true;
    if( ( number_of_bit == 0 ) && ( (tmp & 0x00000001) != 0x00000001 ) ) return false;
    
    if( ( number_of_bit == 1 ) && ( (tmp & 0x00000010) == 0x00000010 ) ) return true;
    if( ( number_of_bit == 1 ) && ( (tmp & 0x00000010) != 0x00000010 ) ) return false;
    
    if( ( number_of_bit == 2 ) && ( (tmp & 0x00000100) == 0x00000100 ) ) return true;
    if( ( number_of_bit == 2 ) && ( (tmp & 0x00000100) != 0x00000100 ) ) return false;
    
    if( ( number_of_bit == 3 ) && ( (tmp & 0x00001000) == 0x00001000 ) ) return true;
    if( ( number_of_bit == 3 ) && ( (tmp & 0x00001000) != 0x00001000 ) ) return false;
    
    if( ( number_of_bit == 4 ) && ( (tmp & 0x00010000) == 0x00010000 ) ) return true;
    if( ( number_of_bit == 4 ) && ( (tmp & 0x00010000) != 0x00010000 ) ) return false;
    
    if( ( number_of_bit == 5 ) && ( (tmp & 0x00100000) == 0x00100000 ) ) return true;
    if( ( number_of_bit == 5 ) && ( (tmp & 0x00100000) != 0x00100000 ) ) return false;
    
    if( ( number_of_bit == 6 ) && ( (tmp & 0x01000000) == 0x01000000 ) ) return true;
    if( ( number_of_bit == 6 ) && ( (tmp & 0x01000000) != 0x01000000 ) ) return false;
    
    if( ( number_of_bit == 7 ) && ( (tmp & 0x10000000) == 0x10000000 ) ) return true;
    if( ( number_of_bit == 7 ) && ( (tmp & 0x10000000) != 0x10000000 ) ) return false;
    
    return false;
}

bool app_modbus_rtu::setResponse(char* buff, int size)
{
     //0x01 0x01 0x01 0x00 0x51 0x88   
    memset(this->response,0,size);
    memcpy(this->response, buff,size);
 
    this->response_size = size;
    
    if(size < 5) return false;
    
    this->slaveAddress    = this->response[0];
    this->function        = this->response[1];
    this->number_of_byte  = (int)(this->response[2]);
    
    
   // std::cout<<"\n\nAdr "<<(int)this->slaveAddress<<"\n\n";
    
   // this->resData = new char[this->number_of_byte]; //tu moze byc problem, ciagle jest rezerwowane jakiejs miejsce..
    
    for(int i=0; i<this->number_of_byte; i++)
    {
        this->resData[i] = this->response[i+3];
    }
    
   
    char crc16HI = this->response[this->number_of_byte +3];
    char crc16LO = this->response[this->number_of_byte +4];
    
    if(this->function  == 16)
    {
       crc16HI = this->response[6];
       crc16LO = this->response[7];      
    }
    
    
    
    unsigned short crc  = this->CRC16(this->response, size-2);
    convert um;
    um.znaki [0] = crc16HI;
    um.znaki [1] = crc16LO;
    
   // if(this->function == 16)
  //  {
     //   std::cout<<"\n\nCRC HI "<<(int)crc16HI;
       // std::cout<<"\n\nCRC LP "<<(int)crc16LO;
        
      //  convert umT;
      //  umT.us = crc;
        
      //  std::cout<<"\n\nCRC HIAAA "<<(int)umT.znaki[0];
      //  std::cout<<"\n\nCRC LPAAA "<<(int)umT.znaki[1];
    //}
    
    if(um.us == crc)
    {
        return true;
    }
    
   
    
    
    return false;
}


int app_modbus_rtu::getNumberOfBytes()
{
    return this->number_of_byte;
}

void app_modbus_rtu::setCoilStatus(bool status)
{
    this->statusCoilToSave = status;
}


void app_modbus_rtu::setRegisterValue(unsigned short value)
{
    this->toWriteData = value;
}

void app_modbus_rtu::setRegisterValue1(unsigned short value)
{
    this->toWriteData1 = value;
}


int app_modbus_rtu::get(char* buff) 
{
    
    //--------------------------------------------------------------------------
    if(this->function == 1 ) 
    {      
          char buffer[8];             
          buffer[0] = this->slaveAddress;
          buffer[1] = this->function;
          
          convert um0;;
          um0.us = this->dataAddress;
          buffer[2] = um0.znaki[1];    //adres pierwszego bitu, HI
          buffer[3] = um0.znaki[0];    //adres pierwszego bitu LO
    
          convert um1;
          um1.us = this->totalCoils;
          buffer[4] = um1.znaki[1];    //ilosc bitow // Hi
          buffer[5] = um1.znaki[0];    //ilosc bitow // Lo
              
     
          unsigned short us = this->CRC16( (const char*)buffer,6);                  
          convert um;
          um.us = us;    
          
          buffer[6] = um.znaki[0];
          buffer[7] = um.znaki[1];
          
          for(int w=0; w<8; w++) buff[w] = buffer[w];                     
          
          return 8;
    }
    
    
    
    if(this->function == 2 ) 
    {      
          char buffer[8];             
          buffer[0] = this->slaveAddress;
          buffer[1] = this->function;
          
          convert um0;;
          um0.us = this->dataAddress;
          buffer[2] = um0.znaki[1];    //adres pierwszego bitu, HI
          buffer[3] = um0.znaki[0];    //adres pierwszego bitu LO
    
          convert um1;
          um1.us = this->totalCoils;
          buffer[4] = um1.znaki[1];    //ilosc bitow // Hi
          buffer[5] = um1.znaki[0];    //ilosc bitow // Lo
              
     
          unsigned short us = this->CRC16( (const char*)buffer,6);                  
          convert um;
          um.us = us;    
          
          buffer[6] = um.znaki[0];
          buffer[7] = um.znaki[1];
          
          for(int w=0; w<8; w++) buff[w] = buffer[w];                     
          
          return 8;
    }
    
    
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    
    if(this->function == 3 ) 
    {      
          char buffer[8];             
          buffer[0] = this->slaveAddress;
          buffer[1] = this->function;
          
          convert um0;;
          um0.us = this->dataAddress;
          buffer[2] = um0.znaki[1];    //adres pierwszego bitu, HI
          buffer[3] = um0.znaki[0];    //adres pierwszego bitu LO
    
          convert um1;
          um1.us = this->totalCoils;
          buffer[4] = um1.znaki[1];    //ilosc bitow // Hi
          buffer[5] = um1.znaki[0];    //ilosc bitow // Lo
              
     
          unsigned short us = this->CRC16( (const char*)buffer,6);                  
          convert um;
          um.us = us;    
          
          buffer[6] = um.znaki[0];
          buffer[7] = um.znaki[1];
          
          for(int w=0; w<8; w++) buff[w] = buffer[w];                     
          
          return 8;
    }
    
    //--------------------------------------------------------------------------
    if(this->function == 4 ) 
    {      
          char buffer[8];             
          buffer[0] = this->slaveAddress;
          buffer[1] = this->function;
          
          convert um0;;
          um0.us = this->dataAddress;
          buffer[2] = um0.znaki[1];    //adres pierwszego bitu, HI
          buffer[3] = um0.znaki[0];    //adres pierwszego bitu LO
    
          convert um1;
          um1.us = this->totalCoils;
          buffer[4] = um1.znaki[1];    //ilosc bitow // Hi
          buffer[5] = um1.znaki[0];    //ilosc bitow // Lo
              
     
          unsigned short us = this->CRC16( (const char*)buffer,6);                  
          convert um;
          um.us = us;    
          
          buffer[6] = um.znaki[0];
          buffer[7] = um.znaki[1];
          
          for(int w=0; w<8; w++) buff[w] = buffer[w];                     
          
          return 8;
    }
    
    
    if(this->function == 5 ) 
    {      
          char buffer[8];             
          buffer[0] = this->slaveAddress;
          buffer[1] = this->function;
          
          convert um0;;
          um0.us = this->dataAddress;
          buffer[2] = um0.znaki[1];    //adres pierwszego bitu, HI
          buffer[3] = um0.znaki[0];    //adres pierwszego bitu LO
    
          if(this->statusCoilToSave)
          {
              buffer[4] = 255;
              buffer[5] = 0;
          }else
          {
              buffer[4] = 0;     
              buffer[5] = 0;
          }
              
     
          unsigned short us = this->CRC16( (const char*)buffer,6);                  
          convert um;
          um.us = us;    
          
          buffer[6] = um.znaki[0];
          buffer[7] = um.znaki[1];
          
          for(int w=0; w<8; w++) buff[w] = buffer[w];                     
          
          return 8;
    }
    
    
    
    
    if(this->function == 6 ) 
    {      
          char buffer[8];             
          buffer[0] = this->slaveAddress;
          buffer[1] = this->function;
          
          convert um0;;
          um0.us = this->dataAddress;
          buffer[2] = um0.znaki[1];    //adres pierwszego bitu, HI
          buffer[3] = um0.znaki[0];    //adres pierwszego bitu LO
    
          convert um1;
          um1.us = this->toWriteData;
          buffer[4] = um1.znaki[1];    
          buffer[5] = um1.znaki[0];    
              
     
          unsigned short us = this->CRC16( (const char*)buffer,6);                  
          convert um;
          um.us = us;    
          
          buffer[6] = um.znaki[0];
          buffer[7] = um.znaki[1];
          
          for(int w=0; w<8; w++) buff[w] = buffer[w];                     
          
          return 8;
    }
    
    
    if(this->function == 16 ) //Preset Multiple Registers (FC=16)
    {      
          char buffer[8];             
          buffer[0] = this->slaveAddress;
          buffer[1] = this->function;
          
          convert um0;;
          um0.us = this->dataAddress;
          buffer[2] = um0.znaki[1];    //adres pierwszego bitu, HI //adres pierwszej tablicy...
          buffer[3] = um0.znaki[0];    //adres pierwszego bitu LO
    
          //The number of registers to write  //offset według tablicy..
          convert um1z;
          um1z.us = this->numRegisterToWrite;
          buffer[4] = um1z.znaki[1];    
          buffer[5] = um1z.znaki[0];    
        
        
          //The number of data bytes to follow (2 registers x 2 bytes each = 4 bytes)
          convert um1bc;
          um1bc.us = (unsigned short)(this->numRegisterToWrite * 2); //jeden rejestr zawiera 2 bajty
          buffer[6] = um1bc.znaki[0];     
                   
          //
          int nextIndex = 7;
          
          //he value to write to register ......
          if(this->numRegisterToWrite>0)
          {
              convert um1a;
              um1a.us = this->toWriteData; //jeden rejestr zawiera 2 bajty
              buffer[nextIndex] = um1a.znaki[1];     nextIndex++;
              buffer[nextIndex] = um1a.znaki[0];     nextIndex++;
          }
          
          if(this->numRegisterToWrite>1)
          {
              convert um1b;
              um1b.us = this->toWriteData1; //jeden rejestr zawiera 2 bajty
              buffer[nextIndex] = um1b.znaki[1];     nextIndex++;
              buffer[nextIndex] = um1b.znaki[0];     nextIndex++;
          }
     
          unsigned short us = this->CRC16( (const char*)buffer,nextIndex);                  
          convert um;
          um.us = us;    
          
          buffer[nextIndex] = um.znaki[0];    nextIndex++;
          buffer[nextIndex] = um.znaki[1];    nextIndex++;
          
          buffer[nextIndex] = NULL;
                   
          
          for(int w=0; w<nextIndex; w++) buff[w] = buffer[w];                     
          
          return nextIndex;
    }
    
    
    return 0;
}