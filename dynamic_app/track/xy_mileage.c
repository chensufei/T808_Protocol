
/********************************************************************************
**
** 文件名:     xy_mileage.c
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

#ifdef __XY_MILEAGE_SUPPORT__

#define MILEAGE_COMPENSATE         1.13   //补偿过隧道的误差

typedef struct MyPoint
{
	double x;
	double y;
}GPSPoint;

typedef struct GprmcMassage
{
	double Longitude;
	double Latitude;  //纬度
	kal_uint32 CurSpeed;
	kal_int8 flag;
	kal_int8 year;
	kal_int8 month;
	kal_int8 day;
	kal_int8 hour;
	kal_int8 min;
	kal_int8 sec;
	kal_int8 update;
	kal_int8 CurDir_ns;
	kal_int8 CurDir_we;
}GPRMC_MSG;

typedef struct _MIL_COMPUTE_
{
    kal_uint32 cur_m; // 单位米
}MIL_COMPUTE_T;

static MIL_COMPUTE_T s_cur_mileage = 0;

float string_to_float(kal_int8 *str_p)//参数为str_p
{
	kal_uint8 i,j,k,negative=0;
	#define s_temp str_p
	float result=0,result_1=0;
	for(i=0;i<10;i++)
	{
		j=str_p[i];
		if(j==0||((j<'0'||j>'9')&&(j!='.')&&(j!='-')))break;
	}
	k=j=i;//数值的个数
	for(i=0;i<j;i++)//查找小数点的位置，结束后小数点位于第i+1位
	{
		if(s_temp[i]=='.')break;
	}
	
	for(j=0;j<i;j++)
	{
		if(s_temp[j]=='-')     {negative=1;continue;}
		result=result*10+(s_temp[j]-'0');
	}
	j++;//加1后j=i+1，即小数点的位置
	i=j;//第一个小数的位置
	for(;j<k;j++)
	{
		if(s_temp[j]<'0'||s_temp[j]>'9')break;//非法字符，返回
		result_1=result_1*10+(s_temp[j]-'0');
	}
	for(j=0;j<(k-i);j++)result_1*=0.1;
	result+=result_1;

if(negative)result=-result;
return result;
}

//N,--------------N=北，S=南
//E,--------------W=西，E=东
/*
#define    NORTH            1   //北
#define    EAST_NORTH       2   //东北
#define    EAST             3   //东
#define    EAST_SOUTH       4   //东南
#define    SOUTH            5   //南
#define    WEST_SOUTH       6   //西南
#define    WEST             7   //西
#define    WEST_NORTH       8   //西北
*/
////  由经纬度反算成高斯投影坐标
GPSPoint GaussToBLToGauss(double longitude, double latitude)	//GPS坐标转平面坐标
{
	GPSPoint point;

	long ProjNo=0; long ZoneWide; ////带宽
	double longitude1,latitude1, longitude0,latitude0, X0,Y0, xval,yval;
	double a,f, e2,ee, NN, T,C,A, M, iPI;
	iPI = 0.0174532925199433; ////3.1415926535898/180.0;
	ZoneWide = 6; ////6度带宽
	a=6378245.0; f=1.0/298.3; //54年北京坐标系参数
	////a=6378140.0; f=1/298.257; //80年西安坐标系参数
	ProjNo = (long)(longitude / ZoneWide) ;
	longitude0 = ProjNo * ZoneWide + ZoneWide / 2;
	longitude0 = longitude0 * iPI ;
	latitude0 = 0;
	//System.out.println(latitude0);
	longitude1 = longitude * iPI ; //经度转换为弧度
	latitude1 = latitude * iPI ; //纬度转换为弧度
	e2=2*f-f*f;
	ee=e2*(1.0-e2);
	NN=a/sqrt(1.0-e2*sin(latitude1)*sin(latitude1));
	T=tan(latitude1)*tan(latitude1);
	C=ee*cos(latitude1)*cos(latitude1);
	A=(longitude1-longitude0)*cos(latitude1);
	M=a*((1-e2/4-3*e2*e2/64-5*e2*e2*e2/256)*latitude1-(3*e2/8+3*e2*e2/32+45*e2*e2
	*e2/1024)*sin(2*latitude1)
	+(15*e2*e2/256+45*e2*e2*e2/1024)*sin(4*latitude1)-(35*e2*e2*e2/3072)*sin(6*latitude1));
	xval = NN*(A+(1-T+C)*A*A*A/6+(5-18*T+T*T+72*C-58*ee)*A*A*A*A*A/120);
	yval = M+NN*tan(latitude1)*(A*A/2+(5-T+9*C+4*C*C)*A*A*A*A/24
	+(61-58*T+T*T+600*C-330*ee)*A*A*A*A*A*A/720);
	X0 = 1000000L*(ProjNo+1)+500000L;
	Y0 = 0;
	xval = xval+X0; yval = yval+Y0;
	point.x=xval;
	point.y=yval;
	return point;
}

double CalLength(double latitude,double longitude)
{
	double len=0.0;
	double d1,d2,d3,d4;
	static kal_uint8 CalFlag=0;    //是否第一次计算长度
	static double firstLatitude=0;     //保存第一次的经度
	static double firstLongitude=0;    //保存第一次纬度
	static double lasttLatitude=0;     //保存前一次的经度
	static double lasttLongitude=0;    //保存前一次的纬度
	if(CalFlag==0)
	{
		CalFlag=1;
		
		firstLatitude=latitude;
		firstLongitude=longitude;

		lasttLatitude=latitude;
		lasttLongitude=longitude;
		return 0.0;
	}

	d1=lasttLatitude-firstLatitude;
	d2=lasttLongitude-firstLongitude;

	d3=latitude-firstLatitude;
	d4=longitude-firstLongitude;

	lasttLatitude=latitude;
	lasttLongitude=longitude;
	len=sqrt((d3 - d1) * (d3 - d1) + (d4 - d2) * (d4 - d2));
	return len;
}

/*******************************************************************
** 函数名:     xy_mil_LatLongPro
** 函数描述:   经纬度字符串处理，加点
               例 11356.1129 处理后为113.56.1129  度，分，分的小数部分
** 参数:       
** 返回:       
********************************************************************/
void xy_mil_LatLongPro(kal_int8 *buf)
{
	kal_int8 i;
	kal_int8 len=strlen((char*)buf);
	kal_int8 tempBuf[16]={'\0'};
	for(i=0;i<len;i++)
	{
		if(buf[i]=='.')
		{
			memcpy(tempBuf,buf,i-2);
			tempBuf[i-2]='.';
			memcpy(tempBuf+i-1,buf+i-2,len-i+2);
			i=len;
			tempBuf[11]='\0';
			memcpy(buf,tempBuf,11);
		}
	}
}

/*******************************************************************
** 函数名:     xy_mil_LatLongToDouble
** 函数描述:   经纬度字符串转成DOUBLE
               例 113.56.1129  度，分，分的小数部分 变为113.935215度
               113+56.1129/60
** 参数:       
** 返回:       
********************************************************************/
double xy_mil_LatLongToDouble(kal_int8 *buf)
{
    kal_int8 i;
	double dat=0.000000;
	kal_int8 len=strlen((char*)buf);
	kal_int8 tempBuf[16]={'\0'};
	for(i=0;i<len;i++)
	{
		if(buf[i]=='.')
		{
			//处理度
			memcpy(tempBuf,buf,i);  //取出度数
			tempBuf[i]='\0';  //加入结束符
			dat=(double)(atoi((char*)tempBuf));

			//处理分
			memcpy(tempBuf,buf+i+1,len-i);
			dat+=((string_to_float(tempBuf))/60.0);
			i=len;
		}
	}
	return dat;
}

/*******************************************************************
** 函数名:     xy_mil_compute
** 函数描述:   里程计算
** 参数:       
** 返回:       
********************************************************************/
double xy_mil_compute(kal_int8 *latitudeStr,kal_int8 *longitudeStr)
{
    double SumLength=0.0; 
    GPSPoint point = {0};
    GPRMC_MSG GprmcMsg = {0};
    xy_mil_LatLongPro(latitudeStr);
    xy_mil_LatLongPro(longitudeStr);
    GprmcMsg.Latitude=xy_mil_LatLongToDouble(latitudeStr);
    GprmcMsg.Longitude=xy_mil_LatLongToDouble(longitudeStr);
    point=GaussToBLToGauss(GprmcMsg.Longitude,GprmcMsg.Latitude);
    SumLength=CalLength(point.x,point.y);
    return SumLength;
}

/*******************************************************************
** 函数名:     xy_mil_sum
** 函数描述:   里程统计
** 参数:       
** 返回:       
********************************************************************/
kal_uint32 xy_mil_sum(void)
{
    static kal_uint32 log_cnt=0;
    static kal_uint32 rmcMsgTime=0;
    dynamic_gps_info_t gpsdata;
    double SumLength=0.0; // 里程数，计算结果
    XY_INFO_T * xy_info = xy_get_info();
    
	memcpy(&gpsdata,dynamic_gps_get_info(),sizeof(dynamic_gps_info_t));
	rmcMsgTime++;
	if((gpsdata.valid == 1))
	{
		SumLength = xy_mil_compute((kal_int8*)gpsdata.lat_str,(kal_int8*)gpsdata.lng_str);
		if(rmcMsgTime>5)  //不正常，过隧道了
		{
			rmcMsgTime=0;
			SumLength *= MILEAGE_COMPENSATE; //补偿过隧道的误差
			if(SumLength<50000)
			{
				s_cur_mileage.cur_m += SumLength;   //总里程
				//dynamic_debug("SumLength<50000 ok");
			}
			else
			{
				//dynamic_debug("SumLength>50000 err");	
			}
		}
		else  //正常每秒钟信号
		{
			rmcMsgTime=0; 
			if(SumLength<200)
			{
				s_cur_mileage.cur_m += SumLength;   //总里程
				//dynamic_debug("SumLength<200 ok");	
			}
			else
			{
				//dynamic_debug("SumLength>200 err");	
			}
		}
	}

    xy_info->mileage = s_cur_mileage.cur_m;

	if (++log_cnt >= 60)
	{
	    log_cnt = 0;
    	dynamic_debug("s_cur_mileage.cur_m =%d",s_cur_mileage.cur_m);
    	xy_info_save();
	}

	return (kal_uint32)SumLength;
}

/*******************************************************************
** 函数名:     xy_mil_reset
** 函数描述:   里程清空
** 参数:       
** 返回:       
********************************************************************/
kal_int32 xy_mil_reset(void)
{
	memset(&s_cur_mileage,0,sizeof(MIL_COMPUTE_T));
	return 1;
}

/*******************************************************************
** 函数名:     xy_mil_get_total_m
** 函数描述:   获取当前总里程 单位米
** 参数:       
** 返回:       
********************************************************************/
kal_uint32 xy_mil_get_total_m(void)
{
    kal_uint32 total_m = 0;

    total_m = s_cur_mileage.cur_m;
    return total_m;
}

/*******************************************************************
** 函数名:     xy_mil_init
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_mil_init(kal_uint32 mil)
{
    s_cur_mileage.cur_m = mil;
}

#endif

