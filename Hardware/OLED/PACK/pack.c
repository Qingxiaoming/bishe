#include "pack.h"
// #include "log.h"

//..\HAREWARE\PACK\pack.c(79): warning:  #1256-D: "char" would have been promoted to "int" when passed through the ellipsis parameter; use the latter type instead temp_char = a_arg(arg, char);
#pragma diag_suppress 1256 
//使用这个宏屏蔽掉上面的一个默认为int，格式转换的警告

typedef union _bytes2float
{
    char bytes[4];
    float num;
} bytes2float;

typedef union _bytes2uint32
{
    char bytes[4];
    uint32_t num;
} bytes2uint32;

typedef union _bytes2int32
{
    char bytes[4];
    uint32_t num;
} bytes2int32;


/**
* @brief : void struct_unpack(char* buff,uint32_t buff_size, char*fmt, ...)
* @param : [in]buff 带解析的buff, [in]buff_size 待解析的buff的大小(单位，字节)，
           [in] fmt 解析格式字符串，目前只支持 c/i/I，支持大小端><,默认大端
            [out]... 可变参数,将对应解包出的变量放在这些参数里面，因此需要取地址传入，以便得到他们的值。
    返回解析的参数的数目
* @attention : None
* @author : Sheep
* @date : 23/07/21
*/

uint32_t struct_unpack(char* buff,uint32_t buff_size, char*fmt, ...)
{
    va_list arg;
    char * temp_char; //1
    int32_t *temp_int32; //4
    uint32_t * temp_uint32; //4
    bytes2uint32 b2ui;
    bytes2int32 b2i;
    bytes2float b2f;
    float * temp_float; 
    uint8_t dir = 0;//默认小端
    uint32_t i = 1,index = 0;//i->fmt的下标，index->buff的下标
    if( fmt[0] == '>' )dir = 0; 
    else if( fmt[0]  == '<' )dir = 1; 
    va_start(arg,fmt);
    while(fmt[i] != '\0')
    {
        if(fmt[i] == 'c' || fmt[i] == 'C')
        {
            temp_char = va_arg(arg, char*);
            *temp_char = buff[index]; 
            index+=1;
        }
        else if( fmt[i] == 'I' )
        {
            temp_uint32 = va_arg(arg, uint32_t*);
            if(dir == 0) //大端数据高位在前;
            {
                b2ui.bytes[0] = buff[index+3];
                b2ui.bytes[1] = buff[index+2];
                b2ui.bytes[2] = buff[index+1];
                b2ui.bytes[3] = buff[index];
            }
            else if(dir == 1)//小端数据低位在前;
            {
                b2ui.bytes[0] = buff[index];
                b2ui.bytes[1] = buff[index+1];
                b2ui.bytes[2] = buff[index+2];
                b2ui.bytes[3] = buff[index+3];
            }
            //logError("buff %x,%x,%x,%x",buff[index],buff[index+1],buff[index+2],buff[index+3]);
            *temp_uint32  = b2ui.num; 
            index+=4;
        }
        else if( fmt[i] == 'i' )
        {
            temp_int32 = va_arg(arg, int32_t*);
            if(dir == 0) //大端数据高位在前;
            {
                b2i.bytes[0] = buff[index+3];
                b2i.bytes[1] = buff[index+2];
                b2i.bytes[2] = buff[index+1];
                b2i.bytes[3] = buff[index];
            }
            else if(dir == 1)//小端数据低位在前;
            {
                b2i.bytes[0] = buff[index];
                b2i.bytes[1] = buff[index+1];
                b2i.bytes[2] = buff[index+2];
                b2i.bytes[3] = buff[index+3];
            }
            //logError("buff %x,%x,%x,%x",buff[index],buff[index+1],buff[index+2],buff[index+3]);
            *temp_int32  = b2i.num; 
            index+=4;
        }
        else if ( fmt[i] == 'f' )
        {
            temp_float = va_arg(arg, float*);
            if(dir == 0)
            {
                b2f.bytes[0] = buff[index+3];
                b2f.bytes[1] = buff[index+2];
                b2f.bytes[2] = buff[index+1];
                b2f.bytes[3] = buff[index];
            } 
            else if(dir == 1)
            {
                b2f.bytes[0] = buff[index];
                b2f.bytes[1] = buff[index+1];
                b2f.bytes[2] = buff[index+2];
                b2f.bytes[3] = buff[index+3];
            }
            *temp_float = b2f.num;
            index+=4;
        }
        if(index > buff_size)break;//格式参数过多，不匹配
        i++;
    }
    va_end(arg);
    return i;
}



/**
* @brief : uint32_t struct_pack(char* buff,uint32_t buff_size, char*fmt, ...)
* @param : [out]要封成包的数据缓冲区，[in]buff_size 缓冲区大小,
            [in] fmt，格式化的字符串，目前支持cIf三种，[in]...对应的参数值，不用取地址，只需要参数值
* @attention : None
* @author : Sheep
* @date : 23/07/29
*/
uint32_t struct_pack(char* buff,uint32_t buff_size, char*fmt, ...)
{
    va_list arg;
    char temp_char; //1
    int32_t temp_int32; //4
    uint32_t temp_uint32; //4
    bytes2int32 b2i;
    bytes2uint32 b2ui;
    bytes2float b2f;
    float temp_float;
    uint8_t dir = 0;//默认小端
    if( fmt[0] == '>' )dir = 0; 
    else if( fmt[0]  == '<' )dir = 1; 
    va_start(arg,fmt);
    uint32_t i = 1,index = 0;//i->fmt的下标，index->buff的下标
    while(fmt[i] != '\0')
    {
        if(fmt[i] == 'c' || fmt[i] == 'C')
        {
            temp_char = va_arg(arg, char);
            buff[index] = temp_char; 
            index+=1;
        }
        else if( fmt[i] == 'I' )
        {
            temp_uint32 = va_arg(arg, uint32_t);
            b2ui.num = temp_uint32;
            if(dir == 0) 
            {
                buff[index+3] = b2ui.bytes[0];
                buff[index+2] = b2ui.bytes[1];
                buff[index+1] = b2ui.bytes[2];
                buff[index] = b2ui.bytes[3];
            }
            else if(dir == 1)
            {
                buff[index] = b2ui.bytes[0];
                buff[index+1] = b2ui.bytes[1];
                buff[index+2] = b2ui.bytes[2];
                buff[index+3] = b2ui.bytes[3];
            }
            index+=4;
        }
        else if( fmt[i] == 'i' )
        {
            temp_int32 = va_arg(arg, int32_t);
            b2i.num = temp_int32;
            if(dir == 0) 
            {
                buff[index+3] = b2i.bytes[0];
                buff[index+2] = b2i.bytes[1];
                buff[index+1] = b2i.bytes[2];
                buff[index] = b2i.bytes[3];
            }
            else if(dir == 1)
            {
                buff[index] = b2i.bytes[0];
                buff[index+1] = b2i.bytes[1];
                buff[index+2] = b2i.bytes[2];
                buff[index+3] = b2i.bytes[3];
            }
            index+=4;
        }
        else if ( fmt[i] == 'f' )
        {
            temp_float = va_arg(arg, float);
            b2f.num = temp_float;
            if(dir == 0) 
            {
                buff[index+3] = b2f.bytes[0];
                buff[index+2] = b2f.bytes[1];
                buff[index+1] = b2f.bytes[2];
                buff[index] = b2f.bytes[3];
            }
            else if(dir == 1)
            {
                buff[index] = b2f.bytes[0];
                buff[index+1] = b2f.bytes[1];
                buff[index+2] = b2f.bytes[2];
                buff[index+3] = b2f.bytes[3];
            }
            index+=4;
        }
        if(index > buff_size)break;//格式参数过多，不匹配
        i++;
    }
    va_end(arg);
    return i;
}



/**
// pack 的简单使用,解包
uint32_t inter[5] = {0};
struct_unpack((char*)USART2_RX_BUF,20,"<IIIII",inter,&inter[1],&inter[2],&inter[3],&inter[4]);
printf("usart_buff:%d,%d,%d,%d,%d\r\n",inter[0],inter[1],inter[2],inter[3],inter[4]);
//压包
static uint32_t pack_test = 0;
uint8_t send_buff[12] = {0};
pack_test++;
struct_pack((char*)send_buff,12,"<III",pack_test,pack_test+1,pack_test+2);
USART2_sendbuff(send_buff,12);
*/
