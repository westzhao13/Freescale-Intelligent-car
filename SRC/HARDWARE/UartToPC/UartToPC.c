/**
*                   === Threshold Studio ===
* @name   : Freescale Intelligent Car Racing
* @author : ZhaoXiang @杭州电子科技大学 信息工程学院CCD光电组
* @Tips   : Based on MK60DN212 200Mhz Core Speed and 100Mhz Bus Speed
* @Lib    : CH_Lib v2.4
* @Date   : Found date 2014.9.7
*/
#include "UartToPC.h"

/**
*  @name   : Uart_Init
*  @author : ZX
*  @tips   : 
*/
void Uart_Init(void)
{
	UART_DebugPortInit(UART4_RX_E24_TX_E25,115200);
}

/**
*  @name   : SendHex
*  @author : ZX
*  @tips   : 
*/
void SendHex(unsigned char hex) 
{
  unsigned char temp;
  temp = hex >> 4;
  if(temp < 10) {
    UART_SendData(UART4,temp + '0');
  } else {
    UART_SendData(UART4,temp - 10 + 'A');
  }
  temp = hex & 0x0F;
  if(temp < 10) {
    UART_SendData(UART4,temp + '0');
  } else {
   UART_SendData(UART4,temp - 10 + 'A');
  }
}

/**
*  @name   : SendImageData
*  @author : ZX
*  @tips   : 
*/
void SendImageData(unsigned char * ImageData) 
{

    unsigned char i;
    unsigned char crc = 0;

    /* Send Data */
    UART_SendData(UART4,'*');
    UART_SendData(UART4,'L');
    UART_SendData(UART4,'D');

    SendHex(0);
    SendHex(0);
    SendHex(0);
    SendHex(0);

    for(i=0; i<128; i++) {
      SendHex(*ImageData++);
    }

    SendHex(crc);
    UART_SendData(UART4,'#');
}


//SerialChart
uint8_t sendbuf[4][15];      // 4个通道，每个通道数据长度最长20个ASCII字符

/**
*  @name   : Send_dat
*  @author : ZX
*  @tips   : 
*/
void Send_dat(uint8_t dat[],uint8_t ledgth);  // 发送一个通道的数据,数组的长度 length
/**
*  @name   : Serial_Send
*  @author : ZX
*  @tips   : 
*/
void Serial_Send(float dat0,float dat1,float dat2,float dat3)
{
  uint8_t CH0Len=0,CH1Len=0,CH2Len=0,CH3Len=0; // 各个通道所要发送数据的实际长度
 // 浮点数转换成ASCII数组
  CH0Len=ftostr(dat0,  sendbuf[0],AFTERDOT) ;
  CH1Len=ftostr(dat1,  sendbuf[1],AFTERDOT) ;
  CH2Len=ftostr(dat2,  sendbuf[2],AFTERDOT) ;
  CH3Len=ftostr(dat3,  sendbuf[3],AFTERDOT) ;
  
  Send_dat(sendbuf[0],CH0Len);          // serialchart 数据格式 ： ASCII + , + ASCII + ,+.....+\n
  Send_dat(sendbuf[1],CH1Len);
  Send_dat(sendbuf[2],CH2Len);
  Send_dat(sendbuf[3],CH3Len);
  UART_printf("\n");
}
/**
*  @name   : Send_dat
*  @author : ZX
*  @tips   : 
*/
void Send_dat(uint8_t dat[],uint8_t length )
{
  uint8_t i=0;
  for(i=0;i<length;i++)
  {
    UART_printf("%c",dat[i]);
  }
  UART_printf(",");             
}

// float 类型数据转换成ASCII类型string的函数
// 函数名：ftostr
// 输入参数：待转换数字，存放字符串，浮点小数后边位数
// 描述：将浮点型数据转换为字符串
// 返回值:字符串大小
//============================================================================
/**
*  @name   : ftostr
*  @author : ZX
*  @tips   : 
*/
uint8_t ftostr(float fNum,  uint8_t str[],uint8_t dotsize)
{
	// 定义变量
	 uint8_t iSize=0;//记录字符串长度的数
	 uint8_t n=0;//用作循环的临时变量
	 uint8_t *p=str;//做换向时用的指针
	 uint8_t *s=str;//做换向时用的指针
	 uint8_t isnegative=0;//负数标志
	 uint32_t i_predot;//小数点前的数
	 uint32_t i_afterdot;//小数点后的数
	 float f_afterdot;//实数型的小数部分
               
	//判断是否为负数
	if(fNum<0)
	{
		isnegative=1;//设置负数标志
		fNum=-fNum;//将负数变为正数
	}

	i_predot=(unsigned long int)fNum;//将小数点之前的数变为整数
	f_afterdot=fNum-i_predot;//单独取出小数点之后的数
	//根据设定的要保存的小数点后的位数，将小数点后相应的位数变到小数点之前
	for(n=dotsize;n>0;n--)
	{
		f_afterdot=f_afterdot*10;
	}
	i_afterdot=(unsigned long int)f_afterdot;//将小数点后相应位数的数字变为整数

	//先将小数点后的数转换为字符串
	n=dotsize;
	while(i_afterdot>0|n>0)
	{
		n--;
	 	str[iSize++]=i_afterdot%10+'0';//对10取余并变为ASCII码
	 	i_afterdot=i_afterdot/10;//对10取商
	}
	str[iSize++] ='.';//加上小数点
    //处理小数点前为0的情况。
	if(i_predot==0)
		str[iSize++]='0';
	//再将小数点前的数转换为字符串
	while(i_predot>0)
	{
	 	str[iSize++]=i_predot%10+'0';//对10取余并变为ASCII码
	 	i_predot=i_predot/10;//对10取商
	}

	if(isnegative==1)
		str[iSize++]='-';//如果是负数，则在最后加上负号
       	

	p=str+iSize-1;//将P指针指向字符串结束标志之前

	for(;p-s>0;p--,s++)//将字符串中存储的数调头
	{
		*s^=*p;
		*p^=*s;
		*s^=*p;
	}
      //  *s='\0';     // 字符串结束标志
	// 返回指针字符串大小
	return iSize;
}
/**
*  @name   : strtofloat
*  @author : ZX
*  @tips   : 
*/
float strtofloat(uint8_t str[],uint8_t len) //将带符号数字的浮点字符串转换成浮点数据
{
  int8_t i=0,dot=0;
  uint8_t isnegative=0;//负数标志
  float tempint=0,tempf=0.0,value=0.0;
  
  for(i=0;i<len;i++)       // 检索小数点和数字结束标志
  {
    if(str[i]=='.')
    {
      dot=i;
      break;
    }
  }
  if(dot==0) // 没有小数点
  {
    dot=len;
  }
  
  if(str[0]=='-')        // 负数
  {
    isnegative=1;
  }
  for(i=isnegative;i<dot;i++)        // 整数部分
  {
      str[i]-='0';;   // 字符转整数
      tempint=tempint*10+str[i];
  }
  for(i=len;i>dot;i--)         // 小数部分
  {
    if(str[i]>=0x30&&str[i]<=0x39)
    {
      str[i]-=0x30;
      tempf=tempf*0.1+str[i]*0.1;
    }
  }
  value=tempint+tempf;
  if(isnegative)
  {
    value=-value;
  }
  return(value);
  
}

//匿名上位机
/**
*  @name   : Data_Send_Begins
*  @author : ZX
*  @tips   : 
*/
uint16_t data_to_send[128];
//发送四个ccd的扫描起始点值
void Data_Send_Begins(uint8_t x1,uint8_t x2,uint8_t x3,uint8_t x4)
{
	uint8_t _cnt=0;
	uint16_t sum = 0;
	uint8_t i;
	
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xF1;
	data_to_send[_cnt++]=0;

	data_to_send[_cnt++] = x1;
	data_to_send[_cnt++] = x2;
	data_to_send[_cnt++] = x3;
	data_to_send[_cnt++] = x4;
	
	data_to_send[3] = _cnt-4;
	
	for(i=0;i<_cnt;i++)
		sum += data_to_send[i];
	
	data_to_send[_cnt++]=sum;

	for(i=0;i<_cnt;i++)
		UART_SendData(UART4,data_to_send[i]);
}
//send endlong data
/**
*  @name   : Data_Send_Senser
*  @author : ZX
*  @tips   : 
*/
void Data_Send_Senser(int16_t x,int16_t y,int16_t z,int16_t a,int16_t b,int16_t c,int16_t d)
{
	uint8_t _cnt=0;
	uint32_t sum = 0;
	uint8_t i;
	
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xF2;
	data_to_send[_cnt++]=0;
	
	data_to_send[_cnt++]=BYTE1(x);  //?8?
	data_to_send[_cnt++]=BYTE0(x);  //?8?
	data_to_send[_cnt++]=BYTE1(y);
	data_to_send[_cnt++]=BYTE0(y);
	data_to_send[_cnt++]=BYTE1(z);
	data_to_send[_cnt++]=BYTE0(z);
  data_to_send[_cnt++]=BYTE1(a);  //?8?
	data_to_send[_cnt++]=BYTE0(a);  //?8?
	data_to_send[_cnt++]=BYTE1(b);
	data_to_send[_cnt++]=BYTE0(b);
	data_to_send[_cnt++]=BYTE1(c);
	data_to_send[_cnt++]=BYTE0(c);
	data_to_send[_cnt++]=BYTE1(d);
	data_to_send[_cnt++]=BYTE0(d);
	
	data_to_send[3] = _cnt-4;
	
	for(i=0;i<_cnt;i++)
		sum += data_to_send[i];
	
	data_to_send[_cnt++]=sum;

	for(i=0;i<_cnt;i++)
		UART_SendData(UART4,data_to_send[i]);
}


/*
void Data_Send_Status(void)
{
	uint8_t _cnt=0;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0x01;
	data_to_send[_cnt++]=0;
	uint16_t _temp;
	_temp = (int)(Att_Angle.rol*100);
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = (int)(Att_Angle.pit*100);
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = (int)(Att_Angle.yaw*100);
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	vs32 _temp2 = Alt;
	data_to_send[_cnt++]=BYTE3(_temp2);
	data_to_send[_cnt++]=BYTE2(_temp2);
	data_to_send[_cnt++]=BYTE1(_temp2);
	data_to_send[_cnt++]=BYTE0(_temp2);
		
	if(Rc_C.ARMED==0)		data_to_send[_cnt++]=0xA0;	
	else if(Rc_C.ARMED==1)		data_to_send[_cnt++]=0xA1;
	
	data_to_send[3] = _cnt-4;
	
	u8 sum = 0;
	for(u8 i=0;i<_cnt;i++)
		sum += data_to_send[i];
	data_to_send[_cnt++]=sum;

	Uart1_Put_Buf(data_to_send,_cnt);
}

void Data_Send_Senser(void)
{
	u8 _cnt=0;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0x02;
	data_to_send[_cnt++]=0;
	data_to_send[_cnt++]=BYTE1(Acc.X);  //?8?
	data_to_send[_cnt++]=BYTE0(Acc.X);  //?8?
	data_to_send[_cnt++]=BYTE1(Acc.Y);
	data_to_send[_cnt++]=BYTE0(Acc.Y);
	data_to_send[_cnt++]=BYTE1(Acc.Z);
	data_to_send[_cnt++]=BYTE0(Acc.Z);
	data_to_send[_cnt++]=BYTE1(Gyr.X);
	data_to_send[_cnt++]=BYTE0(Gyr.X);
	data_to_send[_cnt++]=BYTE1(Gyr.Y);
	data_to_send[_cnt++]=BYTE0(Gyr.Y);
	data_to_send[_cnt++]=BYTE1(Gyr.Z);
	data_to_send[_cnt++]=BYTE0(Gyr.Z);
	data_to_send[_cnt++]=0;
	data_to_send[_cnt++]=0;
	data_to_send[_cnt++]=0;
	data_to_send[_cnt++]=0;
	data_to_send[_cnt++]=0;
	data_to_send[_cnt++]=0;
	
	data_to_send[3] = _cnt-4;
	
	u8 sum = 0;
	for(u8 i=0;i<_cnt;i++)
		sum += data_to_send[i];
	data_to_send[_cnt++] = sum;

	Uart1_Put_Buf(data_to_send,_cnt);
}

�3:??????
void Data_Send_RCData(void)
{
	u8 _cnt=0;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0x03;
	data_to_send[_cnt++]=0;
	data_to_send[_cnt++]=BYTE1(Rc_D.THROTTLE);
	data_to_send[_cnt++]=BYTE0(Rc_D.THROTTLE);
	data_to_send[_cnt++]=BYTE1(Rc_D.YAW);
	data_to_send[_cnt++]=BYTE0(Rc_D.YAW);
	data_to_send[_cnt++]=BYTE1(Rc_D.ROLL);
	data_to_send[_cnt++]=BYTE0(Rc_D.ROLL);
	data_to_send[_cnt++]=BYTE1(Rc_D.PITCH);
	data_to_send[_cnt++]=BYTE0(Rc_D.PITCH);
	data_to_send[_cnt++]=BYTE1(Rc_D.AUX1);
	data_to_send[_cnt++]=BYTE0(Rc_D.AUX1);
	data_to_send[_cnt++]=BYTE1(Rc_D.AUX2);
	data_to_send[_cnt++]=BYTE0(Rc_D.AUX2);
	data_to_send[_cnt++]=BYTE1(Rc_D.AUX3);
	data_to_send[_cnt++]=BYTE0(Rc_D.AUX3);
	data_to_send[_cnt++]=BYTE1(Rc_D.AUX4);
	data_to_send[_cnt++]=BYTE0(Rc_D.AUX4);
	data_to_send[_cnt++]=BYTE1(Rc_D.AUX5);
	data_to_send[_cnt++]=BYTE0(Rc_D.AUX5);
	data_to_send[_cnt++]=BYTE1(Rc_D.AUX6);
	data_to_send[_cnt++]=BYTE0(Rc_D.AUX6);
	
	data_to_send[3] = _cnt-4;
	
	u8 sum = 0;
	for(u8 i=0;i<_cnt;i++)
		sum += data_to_send[i];
	
	data_to_send[_cnt++]=sum;

	Uart1_Put_Buf(data_to_send,_cnt);
}

�4:??PWM?
void Data_Send_MotoPWM(void)
{
	u8 _cnt=0;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0x06;
	data_to_send[_cnt++]=0;
	data_to_send[_cnt++]=BYTE1(Moto_PWM_1);
	data_to_send[_cnt++]=BYTE0(Moto_PWM_1);
	data_to_send[_cnt++]=BYTE1(Moto_PWM_2);
	data_to_send[_cnt++]=BYTE0(Moto_PWM_2);
	data_to_send[_cnt++]=BYTE1(Moto_PWM_3);
	data_to_send[_cnt++]=BYTE0(Moto_PWM_3);
	data_to_send[_cnt++]=BYTE1(Moto_PWM_4);
	data_to_send[_cnt++]=BYTE0(Moto_PWM_4);
	data_to_send[_cnt++]=BYTE1(Moto_PWM_5);
	data_to_send[_cnt++]=BYTE0(Moto_PWM_5);
	data_to_send[_cnt++]=BYTE1(Moto_PWM_6);
	data_to_send[_cnt++]=BYTE0(Moto_PWM_6);
	data_to_send[_cnt++]=BYTE1(Moto_PWM_7);
	data_to_send[_cnt++]=BYTE0(Moto_PWM_7);
	data_to_send[_cnt++]=BYTE1(Moto_PWM_8);
	data_to_send[_cnt++]=BYTE0(Moto_PWM_8);
	
	data_to_send[3] = _cnt-4;
	
	u8 sum = 0;
	for(u8 i=0;i<_cnt;i++)
		sum += data_to_send[i];
	
	data_to_send[_cnt++]=sum;
	
	Uart1_Put_Buf(data_to_send,_cnt);
}

�5:???????
void Data_Send_OFFSET(void)
{
	u8 _cnt=0;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0x16;
	data_to_send[_cnt++]=0;
	vs16 _temp = AngleOffset_Rol*1000;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = AngleOffset_Pit*1000;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	
	data_to_send[3] = _cnt-4;
	
	u8 sum = 0;
	for(u8 i=0;i<_cnt;i++)
		sum += data_to_send[i];
	
	data_to_send[_cnt++]=sum;
	
	Uart1_Put_Buf(data_to_send,_cnt);
}

�6:??PID1
void Data_Send_PID1(void)
{
	u8 _cnt=0;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0x10;
	data_to_send[_cnt++]=0;
	
	vs16 _temp;
	_temp = PID_ROL.P * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_ROL.I * 1000;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_ROL.D * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PIT.P * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PIT.I * 1000;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PIT.D * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_YAW.P * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_YAW.I * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_YAW.D * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	
	data_to_send[3] = _cnt-4;
	
	u8 sum = 0;
	for(u8 i=0;i<_cnt;i++)
		sum += data_to_send[i];
	
	data_to_send[_cnt++]=sum;
	
	Uart1_Put_Buf(data_to_send,_cnt);
}

�7:??PID2
void Data_Send_PID2(void)
{
	u8 _cnt=0;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0x11;
	data_to_send[_cnt++]=0;
	
	vs16 _temp;
	_temp = PID_ALT.P * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_ALT.I * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_ALT.D * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_POS.P * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_POS.I * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_POS.D * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PID_1.P * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PID_1.I * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PID_1.D * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	
	data_to_send[3] = _cnt-4;
	
	u8 sum = 0;
	for(u8 i=0;i<_cnt;i++)
		sum += data_to_send[i];
	
	data_to_send[_cnt++]=sum;
	
	Uart1_Put_Buf(data_to_send,_cnt);
}

�8:??PID3
void Data_Send_PID3(void)
{
	u8 _cnt=0;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0x12;
	data_to_send[_cnt++]=0;
	
	vs16 _temp;
	_temp = PID_PID_2.P * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PID_2.I * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PID_2.D * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PID_3.P * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PID_3.I * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PID_3.D * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PID_4.P * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PID_4.I * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PID_4.D * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	
	data_to_send[3] = _cnt-4;
	
	u8 sum = 0;
	for(u8 i=0;i<_cnt;i++)
		sum += data_to_send[i];
	
	data_to_send[_cnt++]=sum;
	
	Uart1_Put_Buf(data_to_send,_cnt);
}

�8:??PID4
void Data_Send_PID4(void)
{
	u8 _cnt=0;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0x13;
	data_to_send[_cnt++]=0;
	
	vs16 _temp;
	_temp = PID_PID_5.P * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PID_5.I * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PID_5.D * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PID_6.P * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PID_6.I * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PID_6.D * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PID_7.P * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PID_7.I * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PID_7.D * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	
	data_to_send[3] = _cnt-4;
	
	u8 sum = 0;
	for(u8 i=0;i<_cnt;i++)
		sum += data_to_send[i];
	
	data_to_send[_cnt++]=sum;
	
	Uart1_Put_Buf(data_to_send,_cnt);
}

�8:??PID5
void Data_Send_PID5(void)
{
	u8 _cnt=0;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0x14;
	data_to_send[_cnt++]=0;
	
	vs16 _temp;
	_temp = PID_PID_8.P * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PID_8.I * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PID_8.D * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PID_9.P * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PID_9.I * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PID_9.D * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PID_10.P * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PID_10.I * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PID_10.D * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	
	data_to_send[3] = _cnt-4;
	
	u8 sum = 0;
	for(u8 i=0;i<_cnt;i++)
		sum += data_to_send[i];
	
	data_to_send[_cnt++]=sum;
	
	Uart1_Put_Buf(data_to_send,_cnt);
}

�8:??PID6
void Data_Send_PID6(void)
{
	u8 _cnt=0;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0x15;
	data_to_send[_cnt++]=0;
	
	vs16 _temp;
	_temp = PID_PID_11.P * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PID_11.I * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PID_11.D * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PID_12.P * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PID_12.I * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = PID_PID_12.D * 100;
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);

	data_to_send[3] = _cnt-4;
	
	u8 sum = 0;
	for(u8 i=0;i<_cnt;i++)
		sum += data_to_send[i];
	
	data_to_send[_cnt++]=sum;
	
	Uart1_Put_Buf(data_to_send,_cnt);
}

�9:??????
void Data_Send_Check(u16 check)
{
	data_to_send[0]=0xAA;
	data_to_send[1]=0xAA;
	data_to_send[2]=0xF0;
	data_to_send[3]=3;
	data_to_send[4]=0xBA;
	
	data_to_send[5]=BYTE1(check);
	data_to_send[6]=BYTE0(check);
	
	u8 sum = 0;
	for(u8 i=0;i<7;i++)
		sum += data_to_send[i];
	
	data_to_send[7]=sum;

	Uart1_Put_Buf(data_to_send,8);
}

ΖΖΖΖΖΖΖΖΖΖΖΖΖΖΖΖ
Ζ??????????????????

oid Data_Receive_Anl(u8 *data_buf,u8 num)
{
	vs16 rc_value_temp;
	u8 sum = 0;
	
	for(u8 i=0;i<(num-1);i++)
		sum += *(data_buf+i);
	if(!(sum==*(data_buf+num-1)))		return;		//??sum
	if(!(*(data_buf)==0xAA && *(data_buf+1)==0xAF))		return;		//????
/////////////////////////////////////////////////////////////////////////////////////
	if(*(data_buf+2)==0X01)
	{
		if(*(data_buf+4)==0X01)
			MPU6050_CalOff_Acc();
		if(*(data_buf+4)==0X02)
			MPU6050_CalOff_Gyr();
		if(*(data_buf+4)==0X03)
		{MPU6050_CalOff_Acc();MPU6050_CalOff_Gyr();}
//		if(*(data_buf+4)==0X04)
//			Cal_Compass();
//		if(*(data_buf+4)==0X05)
//			MS5611_CalOffset();
	}
	if(*(data_buf+2)==0X02)
	{
		if(*(data_buf+4)==0X01)
		{
			Send_PID1 = 1;
			Send_PID2 = 1;
			Send_PID3 = 1;
			Send_PID4 = 1;
			Send_PID5 = 1;
			Send_PID6 = 1;
		}
		if(*(data_buf+4)==0X02)
			Send_Offset = 1;
	}
#ifndef CONTROL_USE_RC
	if(*(data_buf+2)==0X03)
	{
		Rc_D.THROTTLE = (vs16)(*(data_buf+4)<<8)|*(data_buf+5);
		Rc_D.YAW = (vs16)(*(data_buf+6)<<8)|*(data_buf+7);
		Rc_D.ROLL = (vs16)(*(data_buf+8)<<8)|*(data_buf+9);
		Rc_D.PITCH = (vs16)(*(data_buf+10)<<8)|*(data_buf+11);
		Rc_D.AUX1 = (vs16)(*(data_buf+12)<<8)|*(data_buf+13);
		Rc_D.AUX2 = (vs16)(*(data_buf+14)<<8)|*(data_buf+15);
		Rc_D.AUX3 = (vs16)(*(data_buf+16)<<8)|*(data_buf+17);
		Rc_D.AUX4 = (vs16)(*(data_buf+18)<<8)|*(data_buf+19);
		Rc_D.AUX5 = (vs16)(*(data_buf+20)<<8)|*(data_buf+21);
		Rc_D.AUX6 = (vs16)(*(data_buf+21)<<8)|*(data_buf+22);
		Rc_Fun(&Rc_D,&Rc_C);
	}
#endif
	if(*(data_buf+2)==0X10)								//PID1
	{
			PID_ROL.P = (float)((vs16)(*(data_buf+4)<<8)|*(data_buf+5))/100;
			PID_ROL.I = (float)((vs16)(*(data_buf+6)<<8)|*(data_buf+7))/1000;
			PID_ROL.D = (float)((vs16)(*(data_buf+8)<<8)|*(data_buf+9))/100;
			PID_PIT.P = (float)((vs16)(*(data_buf+10)<<8)|*(data_buf+11))/100;
			PID_PIT.I = (float)((vs16)(*(data_buf+12)<<8)|*(data_buf+13))/1000;
			PID_PIT.D = (float)((vs16)(*(data_buf+14)<<8)|*(data_buf+15))/100;
			PID_YAW.P = (float)((vs16)(*(data_buf+16)<<8)|*(data_buf+17))/100;
			PID_YAW.I = (float)((vs16)(*(data_buf+18)<<8)|*(data_buf+19))/100;
			PID_YAW.D = (float)((vs16)(*(data_buf+20)<<8)|*(data_buf+21))/100;
			Data_Send_Check(sum);
	}
	if(*(data_buf+2)==0X11)								//PID2
	{
			PID_ALT.P = (float)((vs16)(*(data_buf+4)<<8)|*(data_buf+5))/100;
			PID_ALT.I = (float)((vs16)(*(data_buf+6)<<8)|*(data_buf+7))/100;
			PID_ALT.D = (float)((vs16)(*(data_buf+8)<<8)|*(data_buf+9))/100;
			PID_POS.P = (float)((vs16)(*(data_buf+10)<<8)|*(data_buf+11))/100;
			PID_POS.I = (float)((vs16)(*(data_buf+12)<<8)|*(data_buf+13))/100;
			PID_POS.D = (float)((vs16)(*(data_buf+14)<<8)|*(data_buf+15))/100;
			PID_PID_1.P = (float)((vs16)(*(data_buf+16)<<8)|*(data_buf+17))/100;
			PID_PID_1.I = (float)((vs16)(*(data_buf+18)<<8)|*(data_buf+19))/100;
			PID_PID_1.D = (float)((vs16)(*(data_buf+20)<<8)|*(data_buf+21))/100;
			Data_Send_Check(sum);
	}
	if(*(data_buf+2)==0X12)								//PID3
	{
			PID_PID_2.P = (float)((vs16)(*(data_buf+4)<<8)|*(data_buf+5))/100;
			PID_PID_2.I = (float)((vs16)(*(data_buf+6)<<8)|*(data_buf+7))/100;
			PID_PID_2.D = (float)((vs16)(*(data_buf+8)<<8)|*(data_buf+9))/100;
			PID_PID_3.P = (float)((vs16)(*(data_buf+10)<<8)|*(data_buf+11))/100;
			PID_PID_3.I = (float)((vs16)(*(data_buf+12)<<8)|*(data_buf+13))/100;
			PID_PID_3.D = (float)((vs16)(*(data_buf+14)<<8)|*(data_buf+15))/100;
			PID_PID_4.P = (float)((vs16)(*(data_buf+16)<<8)|*(data_buf+17))/100;
			PID_PID_4.I = (float)((vs16)(*(data_buf+18)<<8)|*(data_buf+19))/100;
			PID_PID_4.D = (float)((vs16)(*(data_buf+20)<<8)|*(data_buf+21))/100;
			Data_Send_Check(sum);
	}
	if(*(data_buf+2)==0X13)								//PID4
	{
			PID_PID_5.P = (float)((vs16)(*(data_buf+4)<<8)|*(data_buf+5))/100;
			PID_PID_5.I = (float)((vs16)(*(data_buf+6)<<8)|*(data_buf+7))/100;
			PID_PID_5.D = (float)((vs16)(*(data_buf+8)<<8)|*(data_buf+9))/100;
			PID_PID_6.P = (float)((vs16)(*(data_buf+10)<<8)|*(data_buf+11))/100;
			PID_PID_6.I = (float)((vs16)(*(data_buf+12)<<8)|*(data_buf+13))/100;
			PID_PID_6.D = (float)((vs16)(*(data_buf+14)<<8)|*(data_buf+15))/100;
			PID_PID_7.P = (float)((vs16)(*(data_buf+16)<<8)|*(data_buf+17))/100;
			PID_PID_7.I = (float)((vs16)(*(data_buf+18)<<8)|*(data_buf+19))/100;
			PID_PID_7.D = (float)((vs16)(*(data_buf+20)<<8)|*(data_buf+21))/100;
			Data_Send_Check(sum);
	}
	if(*(data_buf+2)==0X14)								//PID5
	{
			PID_PID_8.P = (float)((vs16)(*(data_buf+4)<<8)|*(data_buf+5))/100;
			PID_PID_8.I = (float)((vs16)(*(data_buf+6)<<8)|*(data_buf+7))/100;
			PID_PID_8.D = (float)((vs16)(*(data_buf+8)<<8)|*(data_buf+9))/100;
			PID_PID_9.P = (float)((vs16)(*(data_buf+10)<<8)|*(data_buf+11))/100;
			PID_PID_9.I = (float)((vs16)(*(data_buf+12)<<8)|*(data_buf+13))/100;
			PID_PID_9.D = (float)((vs16)(*(data_buf+14)<<8)|*(data_buf+15))/100;
			PID_PID_10.P = (float)((vs16)(*(data_buf+16)<<8)|*(data_buf+17))/100;
			PID_PID_10.I = (float)((vs16)(*(data_buf+18)<<8)|*(data_buf+19))/100;
			PID_PID_10.D = (float)((vs16)(*(data_buf+20)<<8)|*(data_buf+21))/100;
			Data_Send_Check(sum);
	}
	if(*(data_buf+2)==0X15)								//PID6
	{
			PID_PID_11.P = (float)((vs16)(*(data_buf+4)<<8)|*(data_buf+5))/100;
			PID_PID_11.I = (float)((vs16)(*(data_buf+6)<<8)|*(data_buf+7))/100;
			PID_PID_11.D = (float)((vs16)(*(data_buf+8)<<8)|*(data_buf+9))/100;
			PID_PID_12.P = (float)((vs16)(*(data_buf+10)<<8)|*(data_buf+11))/100;
			PID_PID_12.I = (float)((vs16)(*(data_buf+12)<<8)|*(data_buf+13))/100;
			PID_PID_12.D = (float)((vs16)(*(data_buf+14)<<8)|*(data_buf+15))/100;
			Data_Send_Check(sum);
			EE_SAVE_PID();
	}
	if(*(data_buf+2)==0X16)								//OFFSET
	{
			AngleOffset_Rol = (float)((vs16)(*(data_buf+4)<<8)|*(data_buf+5))/1000;
			AngleOffset_Pit = (float)((vs16)(*(data_buf+6)<<8)|*(data_buf+7))/1000;
	}
}
*/


//NRF24L01
//Exmaple:


//CH Kinetis固件库 V2.50 版本
// 修改主频 请修改 CMSIS标准文件 system_MKxxxx.c 中的 CLOCK_SETUP 宏 
 

/*     实验名称：NRF2401实验
     实验平台：渡鸦开发板
     板载芯片：MK60DN512ZVQ10
 实验效果：通过串口实现无线数据的收发功能，
      无线通信支持的是NRF24L01模块
    注意：底层驱动的编写针对2.4G的usb无线模块编写，需要配合USB-2.4G模块使用 
      TX_ADDRESS[5]={0x34,0x43,0x10,0x10,0x01}; //发送地址
      RX_ADDRESS[5]={0x34,0x43,0x10,0x10,0x01}; //接收地址    
	*/		


/*extern int kinetis_spi_bus_init(struct spi_bus* bus, uint32_t instance);
static uint8_t NRF2401RXBuffer[32] = "HelloWorld\r\n";//无线接收数据
static uint8_t* gpRevChar;

// 串口接收中断 
void UART_ISR(uint16_t ch)
{
    static uint8_t rev_ch;
    rev_ch = ch;
    gpRevChar = (uint8_t*)&rev_ch;
}

int main(void)
{
    uint32_t i;
    uint32_t len;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    UART_CallbackRxInstall(HW_UART0, UART_ISR);
    UART_ITDMAConfig(HW_UART0, kUART_IT_Rx, true);
    
    printf("NRF24L01 test\r\n");
    // 初始化 NRF2401模块 的SPI接口及片选 
    PORT_PinMuxConfig(HW_GPIOE, 1, kPinAlt2); 
    PORT_PinMuxConfig(HW_GPIOE, 2, kPinAlt2); 
    PORT_PinMuxConfig(HW_GPIOE, 3, kPinAlt2); 
    PORT_PinMuxConfig(HW_GPIOE, 4, kPinAlt2);
    //初始化2401所需的CE引脚 
    GPIO_QuickInit(HW_GPIOE, 0 , kGPIO_Mode_OPP);
    //初始化2401模块
    static struct spi_bus bus;
    kinetis_spi_bus_init(&bus, HW_SPI1);
    nrf24l01_init(&bus, 0);
    //检测是否存在无线设备，并配置接收和发送地址
    if(nrf24l01_probe())
    {
        printf("no nrf24l01 device found!\r\n");
    }
    // 进入Rx模式 
    nrf24l01_set_rx_mode();
    while(1)
    {
        // 如果收到串口数据则发送 
        if(gpRevChar != NULL)
        {
            nrf24l01_set_tx_mode();
            nrf24l01_write_packet(gpRevChar, 1);
            nrf24l01_set_rx_mode();
            gpRevChar = NULL;
        }
        // 如果收到2401 的数据 则传输到串口 
        if(!nrf24l01_read_packet(NRF2401RXBuffer, &len))
        {
            i = 0;
            while(len--)
            {
                UART_WriteByte(HW_UART0, NRF2401RXBuffer[i++]);
            }
        }
    }
}
*/
