/********************************************************************************
**
** 文件名:     dynamic_common.c
** 版权所有:   
** 文件描述: 
**
*********************************************************************************
**             修改历史记录
**===============================================================================
**| 日期       | 作者   |  修改记录
**===============================================================================
**|  |  | 
*********************************************************************************/
#include "dynamic_init.h"


#include <string.h>
#include <ctype.h>
#include <stdio.h>


/*******************************************************************
** 函数名:     dynamic_malloc_malloc
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void*  dynamic_malloc_malloc (kal_uint32 size)
{
    void* dtr = dynamic_malloc(size);
    if (dtr == NULL)
    {
        dynamic_error("内存申请失败:%d",size);
    }

    return dtr;
}

/*******************************************************************
** 函数名:     dynamic_big_endian_pack_short
** 函数描述:   按大端把数据流组成short
** 参数:       
** 返回:       
********************************************************************/
kal_int16 dynamic_big_endian_pack_short(kal_uint8*data)
{
    kal_uint16 shortdata;
    
    shortdata = data[0];
    shortdata = shortdata << 8;
    shortdata += data[1];

    return shortdata;
}

/*******************************************************************
** 函数名:     dynamic_big_endian_pack_int
** 函数描述:   按大端把数据流组成int
** 参数:       
** 返回:       
********************************************************************/
kal_int32 dynamic_big_endian_pack_int(kal_uint8*data)
{
    kal_uint32 intdata;
    
    intdata = data[0];
    intdata = intdata << 8;
    intdata += data[1];
    intdata = intdata << 8;
    intdata += data[2];
    intdata = intdata << 8;
    intdata += data[3];
    
    return intdata;
}

/*******************************************************************
** 函数名:     dynamic_big_endian_transm_short
** 函数描述:   把双字节按大端模式传输
** 参数:       
** 返回:       
********************************************************************/
kal_uint16 dynamic_big_endian_transm_short(kal_uint8* dtr,kal_uint16 data)
{
    dtr[0] = (data >> 8) & 0xff;
    dtr[1] = data & 0xff;

    return 2;
}

/*******************************************************************
** 函数名:     dynamic_checksum_xor
** 函数描述:   异或校验码 单字节
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 dynamic_checksum_xor(kal_uint8 *Ptr,kal_int32 len)
{
   kal_uint8 sum;

   sum = *Ptr++;
   while(--len) sum ^= *Ptr++;
   return sum;
}
/*******************************************************************
** 函数名:     dynamic_find_byte_pos
** 函数描述:   查找单个字节的位置
** 参数:       
** 返回:       
********************************************************************/
kal_uint8* dynamic_find_byte_pos(kal_uint8 *sptr,kal_uint8 data,kal_uint32 maxlen)
{
    kal_uint32 i;

    for (i=0;i<maxlen;i++)
    {
        if (sptr[i] == data)
        {
            return &sptr[i];
        }
    }
    return NULL;
}


