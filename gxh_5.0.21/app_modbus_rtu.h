
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   app_modbus_rtu.h
 * Author: Blazej Kita
 *
 * Created on 27 lutego 2017, 08:18
 */

#include "gxh_scope.h"


#ifndef APP_MODBUS_RTU_H
#define APP_MODBUS_RTU_H

class app_modbus_rtu {
public:
    app_modbus_rtu();    
    virtual ~app_modbus_rtu();
       
    
   
    /**
     * Set the total number of coils requested
     * @param number
     */
    void setNumberOfCoils(unsigned short number);
    /**
     * Set the total number of registers requested.
     * @param number
     */
    void setNumberOfRegister(unsigned short number);
    
   /**
    * Set the Data Address of the first coil to read.
    * @param 
    */
    void setDataAddress(unsigned short addr);
    
    /**
     * Set slave address
     * @param addr
     */
    void setSlaveAddress(char addr);    
    
    /**
     * 
     */
    void setFunctionReadCoil();
    void setFunctionReadInput();
    void setFunctionInputHoldingRegister();
    void setFunctionInputRegister();
    void setFunctionForceSingleCoil();
    void setFunctionForceSingleRegister();
    void setFunctionMultipleRegister();
    
    /**
     * Get data for send.
     * @param buffer
     * @return size of buffer
     */
    int get(char* buff);
    
   
    
    /**
     * Convert buffer to ascii 
     * @param buff
     * @param size
     */
    void toASCII(char* buff, int size, char* buffOut);
    
    /**
     * Set data received from port
     * @param buff
     * @param size
     * @return true if crc16 sum is corect
     */
    bool setResponse(char* buff, int size);
    
    int getNumberOfBytes();
    
    /**
     * Return bit status from result array(byte 1 (0x00000000), byte 2  (0x00000000), byte 3  (0x00000000) ....)
     * @param number_of_byte, from 0 
     * @param number_of_bit, from 0
     * @return true if bit is on 1
     */
    bool getCoilStatus(int number_of_byte,int number_of_bit);
    
    /**
     * 
     * @param number_of_register  from 0 value
     * @return 
     */
    unsigned short getRegister(int number_of_register);
    
    
    void setCoilStatus(bool status);
    
    
    void setRegisterValue(unsigned short value);
    void setRegisterValue1(unsigned short value);
    
    void setNumRegisterToWrite(unsigned long int value);
    
private:
    
    
    
    std::string IntToHex(int number);
    
    union convert{
            unsigned short us;
            char znaki[2];
    };
    
    char slaveAddress;
    char function;
    unsigned short dataAddress;
    unsigned short totalCoils;     
    
    
    unsigned long int numRegisterToWrite;
    
    bool statusCoilToSave;
    
    unsigned short CRC16 (const char *nData, int wLength);    
    
  //  char ascii[1000];
    
    unsigned short toWriteData;
    unsigned short toWriteData1;  
    
    char response[1024];
    int response_size;
    int number_of_byte;
    char resData[1024];
    
    char resSlaveAddress;
    char resFunction;
    
   
};

#endif /* APP_MODBUS_RTU_H */

