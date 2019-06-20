/********************************************************************************
**
** 文件名:     dynamic_file.c
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
 
/*******************************************************************
** 函数名:     dynamic_fs_delete
** 函数描述:   删除文件
** 参数:       
** 返回:       
********************************************************************/
kal_int32 dynamic_file_dele(char* file_name)
{	
    return dynamic_fs_delete(file_name);
}

/*******************************************************************
** 函数名:     dynamic_file_read
** 函数描述:   读取文件
** 参数:       
** 返回:       
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
** 函数名:     dynamic_file_write
** 函数描述:   写文件，文件不存在则新建文件再写
** 参数:       
** 返回:       
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

