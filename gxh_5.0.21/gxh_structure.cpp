
//Convertion
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#include "gxh_structure.h"


std::string gxh_IntToString(int value)
{
     std::string Result;         
     std::ostringstream convert;   
     convert << value; 
     Result = convert.str(); 
    
     char * data = new char[Result.size() + 1];
     std::copy(Result.begin(), Result.end(), data);
     data[Result.size()] = '\0'; 
     
     std::string ret = std::string(data);
     delete[] data;
     
     return ret;
};

//------------------------------------------------------------------------------

 std::string gxh_DoubleToString(double value)
 {
     std::string Result;         
     std::ostringstream convert;   
     convert << value;      
     Result = convert.str();    
  
     char * data = new char[Result.size() + 1];
     std::copy(Result.begin(), Result.end(), data);
     data[Result.size()] = '\0'; 
     
     std::string ret = std::string(data);
     
     delete[] data;
     
     return ret;
 };

//------------------------------------------------------------------------------
 
 std::string gxh_UnsignedLongToString(unsigned long value)
 {
    std::string Result;         
    std::ostringstream convert;   
    convert << value;      
    Result = convert.str();
    
    char * data = new char[Result.size() + 1];
    std::copy(Result.begin(), Result.end(), data);
    data[Result.size()] = '\0'; 
    
    std::string ret = std::string(data);
    
    delete[] data;
    
    return ret;
 } ;
 
 
//------------------------------------------------------------------------------

 std::string gxh_CharToString(char c)
 {
    char* z = new char[2];
    z[0] = c;
    z[1] = 0;
    
    std::string ret = std::string(z);
    
    delete[] z;
    
    return ret;
 }; 
 
//------------------------------------------------------------------------------
 
char gxh_StringToChar(std::string value)
 {
    const char* c = value.c_str(); 
    return c[0];    
 }; 
 
//------------------------------------------------------------------------------

 
 int gxh_StringToInt(std::string value)
 {    
     if(value.length() == 0 ) return 0; //tu zmiana..
    
     if (value.find(  std::string("NULL") ) != std::string::npos) {
         return 0;
      }
     
      if (value.find(  std::string("null") ) != std::string::npos) {
         return 0;
      }
     
     
    int i;
    std::stringstream stream1;
    stream1.clear();
    stream1.str(value);
    stream1 >> i;
    return i;
 };  

//------------------------------------------------------------------------------
 
 
 
 
//------------------------------------------------------------------------------

 double gxh_StringToDouble(std::string value)
 {    
        
     if(value.length() == 0 ) return 0; //tu zmiana..
    
     if (value.find(  std::string("NULL") ) != std::string::npos) {
         return 0;
      }
     
      if (value.find(  std::string("null") ) != std::string::npos) {
         return 0;
      }
     
     
    double i;
    std::stringstream stream1;
    stream1.clear();
    stream1.str(value);
    stream1 >> i;
    return i;
 }; 
 

//------------------------------------------------------------------------------
 
  unsigned long gxh_StringToUnsignedLong(std::string value)
 {    
    unsigned long i;
    std::stringstream stream1;
    stream1.clear();
    stream1.str(value);
    stream1 >> i;
    return i;
 }; 
 
//------------------------------------------------------------------------------

 bool gxh_replace(std::string& str, const std::string& from, const std::string& to)
 {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
    return false;
    str.replace(start_pos, from.length(), to);
    return true;
 };
 
//------------------------------------------------------------------------------

bool gxh_replaceAll(std::string& str, const std::string& from, const std::string& to)
 {
    if(from.empty())
        return false;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();  
    }
    
    return true;
 }; 
 
//------------------------------------------------------------------------------


 
//------------------------------------------------------------------------------

int gxh_compare(const char* string, const char* needed)
{
    int result = 0;
    
    unsigned int s1 = strlen(string);
    unsigned int s2 = strlen(needed);
    
    if(s1 < s2) return -1;
    
    
    if(s1 >4 && s2 > 4)
    {
        if(string[0] != needed[0]) return -1;
        if(string[2] != needed[2]) return -1;
        if(string[4] != needed[4]) return -1;
    }
        
    for(unsigned int i=0; i<s2; i++)
    {
        if(string[i] != needed[i]) result = -1;
    }
    
    
    return result;
}; 

//------------------------------------------------------------------------------

long int gxh_getTimestamp() {
    
    struct timeval tp;
    gettimeofday(&tp, NULL);
    long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    return ms;
};
//------------------------------------------------------------------------------

unsigned long gxh_getSecondsFrom1900( ) {  //Jan 1, 1970 UTC
    
   unsigned long int sec= time(NULL);
   
   return sec;
};

//------------------------------------------------------------------------------

std::string gxh_currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return std::string(buf);
}

//------------------------------------------------------------------------------

//-----------------------------------------------------------------------

char* gxh_base64_encode(const unsigned char *data, int input_length, int *output_length)
{
    
        //base 64
    static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'}; 
    static int mod_table[] = {0, 2, 1};
    
    *output_length = (4 * ((input_length + 2) / 3)) + 1; //+1 po ostatni znak bedzie nulem....

    
    
    
    char *encoded_data =   (char*)malloc(*output_length);
    if (encoded_data == NULL) return NULL;

    for (int i = 0, j = 0; i < input_length;) 
    {

      int octet_a = i < input_length ? (unsigned char)data[i++] : 0;
      int octet_b = i < input_length ? (unsigned char)data[i++] : 0;
      int octet_c = i < input_length ? (unsigned char)data[i++] : 0;

      int triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

      encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
      encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
      encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
      encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
    }

    for (int i = 0; i < mod_table[input_length % 3]; i++)
       encoded_data[*output_length - 1 - i] = '=';
    
       encoded_data[ (*output_length)-1 ] = '\0';
    

    return encoded_data;
}

//------------------------------------------------------------------------------
