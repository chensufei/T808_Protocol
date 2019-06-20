/********************************************************************************
**
** �ļ���:     dynamic_common.c
** ��Ȩ����:   
** �ļ�����: 
**
*********************************************************************************
**             �޸���ʷ��¼
**===============================================================================
**| ����       | ����   |  �޸ļ�¼
**===============================================================================
**|  |  | 
*********************************************************************************/
#include "dynamic_init.h"


#include <string.h>
#include <ctype.h>
#include <stdio.h>


/*******************************************************************
** ������:     dynamic_malloc_malloc
** ��������:   
** ����:       
** ����:       
********************************************************************/
void*  dynamic_malloc_malloc (kal_uint32 size)
{
    void* dtr = dynamic_malloc(size);
    if (dtr == NULL)
    {
        dynamic_error("�ڴ�����ʧ��:%d",size);
    }

    return dtr;
}

/*******************************************************************
** ������:     dynamic_big_endian_pack_short
** ��������:   ����˰����������short
** ����:       
** ����:       
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
** ������:     dynamic_big_endian_pack_int
** ��������:   ����˰����������int
** ����:       
** ����:       
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
** ������:     dynamic_big_endian_transm_short
** ��������:   ��˫�ֽڰ����ģʽ����
** ����:       
** ����:       
********************************************************************/
kal_uint16 dynamic_big_endian_transm_short(kal_uint8* dtr,kal_uint16 data)
{
    dtr[0] = (data >> 8) & 0xff;
    dtr[1] = data & 0xff;

    return 2;
}

/*******************************************************************
** ������:     dynamic_checksum_xor
** ��������:   ���У���� ���ֽ�
** ����:       
** ����:       
********************************************************************/
kal_uint8 dynamic_checksum_xor(kal_uint8 *Ptr,kal_int32 len)
{
   kal_uint8 sum;

   sum = *Ptr++;
   while(--len) sum ^= *Ptr++;
   return sum;
}
/*******************************************************************
** ������:     dynamic_find_byte_pos
** ��������:   ���ҵ����ֽڵ�λ��
** ����:       
** ����:       
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


