#include <reg51.h>
#include<intrins.h>
 
/*  数据端接口定义 */
sbit  LSH = P2^0;     //列数时钟
sbit  LDS = P2^1;     //输入
sbit  LST = P2^2;     //列寄存器
 
sbit  HSH = P2^3;     //行数时钟
sbit  HDS = P2^4;     //输入
sbit  HST = P2^5;     //行寄存器时钟
 
int hang_flag=1;
int lie_flag=0;

unsigned int sel[17]={0x7fff,0xbfff,0xdfff,0xefff,0xf7ff,0xfbff,0xfdff,0xfeff,
					0xff7f,0xffbf,0xffdf,0xffef,0xfff7,0xfffb,0xfffd,0xfffe,0xffff};
 
char code hanzi_hang[272] =
{
0x01,0x00,0x01,0x00,0x01,0x00,0x3F,0xF8,0x21,0x08,0x21,0x08,0x21,0x08,0x3F,0xF8,
0x21,0x08,0x21,0x08,0x21,0x08,0x3F,0xF8,0x21,0x0A,0x01,0x02,0x01,0x02,0x00,0xFE,//鐢�
0x00,0x00,
0x00,0x00,0x7F,0xF8,0x00,0x10,0x00,0x20,0x00,0x40,0x01,0x80,0x01,0x00,0xFF,0xFE,
0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x05,0x00,0x02,0x00,//瀛�
0x00,0x00,
0x01,0x00,0x01,0x00,0x7F,0xFC,0x01,0x00,0x1F,0xF0,0x00,0x00,0x7F,0xFE,0x40,0x02,
0x9F,0xF4,0x00,0x00,0x1F,0xF0,0x10,0x10,0x1F,0xF0,0x08,0x20,0x04,0x40,0xFF,0xFE,//澹�
0x00,0x00,
0x20,0x80,0x10,0x80,0x00,0x9C,0x47,0xE0,0x20,0x80,0x08,0x84,0x10,0x84,0x60,0x7C,
0x21,0x00,0x01,0x00,0xFF,0xFE,0x05,0x40,0x09,0x20,0x31,0x18,0xC1,0x06,0x01,0x00,//鏌�
0x00,0x00,
0x01,0x00,0x00,0x80,0x3F,0xFE,0x20,0x80,0x2F,0xF8,0x20,0x88,0x3F,0xFE,0x20,0x88,
0x2F,0xF8,0x28,0x80,0x24,0xC4,0x22,0xA8,0x44,0x90,0x48,0x88,0x92,0x86,0x01,0x00,//搴�
0x00,0x00,	

};
 
char code hanzi_lie[272] =
{
0x01,0x00,0x01,0x00,0x01,0x00,0x3F,0xF8,0x21,0x08,0x21,0x08,0x21,0x08,0x3F,0xF8,
0x21,0x08,0x21,0x08,0x21,0x08,0x3F,0xF8,0x21,0x0A,0x01,0x02,0x01,0x02,0x00,0xFE,//鐢�
0x00,0x00,
0x00,0x00,0x7F,0xF8,0x00,0x10,0x00,0x20,0x00,0x40,0x01,0x80,0x01,0x00,0xFF,0xFE,
0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x05,0x00,0x02,0x00,//瀛�
0x00,0x00,
0x01,0x00,0x01,0x00,0x7F,0xFC,0x01,0x00,0x1F,0xF0,0x00,0x00,0x7F,0xFE,0x40,0x02,
0x9F,0xF4,0x00,0x00,0x1F,0xF0,0x10,0x10,0x1F,0xF0,0x08,0x20,0x04,0x40,0xFF,0xFE,//澹�
0x00,0x00,
0x20,0x80,0x10,0x80,0x00,0x9C,0x47,0xE0,0x20,0x80,0x08,0x84,0x10,0x84,0x60,0x7C,
0x21,0x00,0x01,0x00,0xFF,0xFE,0x05,0x40,0x09,0x20,0x31,0x18,0xC1,0x06,0x01,0x00,//鏌�
0x00,0x00,
0x01,0x00,0x00,0x80,0x3F,0xFE,0x20,0x80,0x2F,0xF8,0x20,0x88,0x3F,0xFE,0x20,0x88,
0x2F,0xF8,0x28,0x80,0x24,0xC4,0x22,0xA8,0x44,0x90,0x48,0x88,0x92,0x86,0x01,0x00,//搴�
0x00,0x00,

};  

// 上下滚动
void send_data_H_hang(unsigned int dat);  /*  发送行数据端数据函数  */
void send_data_L_hang(unsigned int dat);  /*  发送列选通端数据函数  */
void display_hang(int a);                 //每一次显示什么东西

// 左右滚动
void send_data_H_lie(unsigned int dat);  /*  发送行数据端数据函数  */
void send_data_L_lie(unsigned int dat);  /*  发送列选通端数据函数  */
void display_lie(int a);                 //每一次显示什么东西

void delay(unsigned int m);
 
 
void main()
{
  unsigned int j=0,num;
  EA = 1;     //开总中断
  EX0 = 1;    //开外部中断0
  EX1 = 1;
  IT0 = 1;    //选择电平负跳变触发方式，
  IT1 = 1; 
	while(1)
	{
		num=9;
		while(num--)
		{
		    if (lie_flag == 0)
			{
			    display_hang(j);
		    }
			else if(hang_flag == 0)
			{
			    display_lie(j);
			}
			delay(10);

	    }
		if ( (hang_flag==1&&lie_flag==0) || (lie_flag == 1 && hang_flag == 0) )
		{
		    j=j+2;
	    }
		else if ( (hang_flag == -1 && lie_flag == 0) || (lie_flag == -1 && hang_flag == 0) )
		{
		    j = j - 2;
			if(j>270)
			    j=270;
		}
		
		
	}
}
	  
void send_data_L_hang(unsigned int dat)
{
    unsigned char i;
    
    for(i=0;i<8;i++)
    {
        LSH = 0;
        LDS = dat&0x80;       //temp<<=1;
        dat = dat<<1;               //ds = CY;
        LSH = 1;
    }
		LST = 0;
    _nop_();
    _nop_();
    LST = 1;
}
 
void send_data_H_hang(unsigned int dat)
{
    int i; 
    for(i=0;i<16;i++)
    {
			  
        HSH = 0;
        HDS = dat&0x8000;
        dat =dat<<1;
        HSH = 1;
    }	
		HST = 0;
    _nop_();
    HST = 1;
}
void delay(unsigned int m)
{
    unsigned char i;
    for(;m>0;m--)
    {
        for(i=0;i<124;i++)
			{}
    }
}
 
void display_hang( int a)
{
    unsigned char i;
	unsigned int x;
    for(i=0;i<16;i++)
    {
        send_data_H_hang(sel[i]);
		x=a+2*i;//展示第几个字符
		if(a+2*i>270)
			x=(a+2*i)%272;
		send_data_L_hang(hanzi_hang[x]);
		send_data_L_hang(hanzi_hang[x+1]);
		delay(1);	  
    }
}


void send_data_L_lie(unsigned int dat)
{
    unsigned char i;
    
    for(i=0;i<16;i++)
    {
        LSH = 0;
        LDS = dat&0x8000;       //temp<<=1;
        dat = dat<<1;               //ds = CY;
        LSH = 1;
    }
		LST = 0;
    _nop_();
    _nop_();
    LST = 1;
}

void send_data_H_lie(unsigned int dat)
{
    int i; 
    for(i=0;i<8;i++)
    {
			  
        HSH = 0;
        HDS = dat&0x80;
        dat =dat<<1;
        HSH = 1;
    }	
		HST = 0;
    _nop_();
    HST = 1;
}

void display_lie( int a)
{
    unsigned char i;
	unsigned int x;
    for(i=0;i<16;i++)
    {
        send_data_L_lie(~sel[i]);
	    x=a+2*i;//展示第几个字符
		if(a+2*i>270)
			x=(a+2*i)%272;
		send_data_H_lie(~hanzi_lie[x]);
		send_data_H_lie(~hanzi_lie[x+1]);
		delay(1);	  
    }
}


void External_Interrupt_0() interrupt 0
//中断函数：INT0，外部中断
{
    if (hang_flag == 0)
	    hang_flag = -1;
    lie_flag = 0;
    hang_flag = -hang_flag;//滚动反向
}

void External_Interrupt_1() interrupt 2
//中断函数：INT1，外部中断
{
    if (lie_flag == 0)
	    lie_flag = -1;
    hang_flag = 0;
    lie_flag = -lie_flag;//滚动反向
}