/********************************************************************************
**
** �ļ���:     dynamic_file.c
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
 
/*******************************************************************
** ������:     dynamic_fs_delete
** ��������:   ɾ���ļ�
** ����:       
** ����:       
********************************************************************/
kal_int32 dynamic_file_dele(char* file_name)
{	
    return dynamic_fs_delete(file_name);
}

/*******************************************************************
** ������:     dynamic_file_read
** ��������:   ��ȡ�ļ�
** ����:       
** ����:       
********************************************************************/
kal_int32 dynamic_file_read(char* file_name, void *buffer,kal_int32 len)
{	
    int file_p;
    kal_int32 file_size;
    kal_uint32 read_size;

    file_size = dynamic_fs_get_size(file_name);

    if (len > file_size)
    {
        len = file_size;
    }
    
    file_p = dynamic_fs_open(file_name,FS_READ_ONLY);

    if (file_p)
    {
        dynamic_fs_seek(file_p,0,FS_FILE_BEGIN);
        dynamic_fs_read(file_p,buffer,len, &read_size);
        dynamic_fs_close(file_p);
        return read_size;
    }
    return 0;
}

/*******************************************************************
** ������:     dynamic_file_write
** ��������:   д�ļ����ļ����������½��ļ���д
** ����:       
** ����:       
********************************************************************/
kal_int32 dynamic_file_write(char* file_name, void *buffer,kal_int32 len)
{	
	int file_p;
	kal_uint32 written = 0;
    
    file_p = dynamic_fs_open(file_name,FS_READ_WRITE);
    if (file_p < 0)
    {
        file_p = dynamic_fs_open(file_name,FS_READ_WRITE | FS_CREATE);
    }
    
    if (file_p)
    {
        dynamic_fs_truncate(file_p);
        dynamic_fs_seek(file_p,0,FS_FILE_BEGIN);
        dynamic_fs_write(file_p,buffer,len, &written);
        dynamic_fs_flush(file_p);
        dynamic_fs_close(file_p);
        return written;
    }
    return 0;
}

