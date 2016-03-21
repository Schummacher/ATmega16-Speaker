# ATmega16-Speaker
My first usefule programing in my life of university.
//包含音量控制 LCD音量显示 以调节好IO口位置 整机使用第一版，但LCD与PWM方向相反
#include<iom16Av.h>
#include <macros.h>

#define PIN_A (PINB&1)
#define PIN_B ((PINB>>1)&1)

#define uchar unsigned char
#define uint  unsigned int
#define RS_0  PORTA &= ~(1 << PA2) 	//RS为0
#define RS_1  PORTA |= (1 << PA2)		//RS为1
#define RW_0  PORTA &= ~(1 << PA3)	//RW为0
#define RW_1  PORTA |= (1 << PA3)	//RW为1
#define EN_0  PORTA &= ~(1 << PA4)	//EN为0
#define EN_1  PORTA |= (1 << PA4)		//EN为1
/********以下是函数声明********/
void delay_us(unsigned int n);
void Delay_ms(uint xms)	;
uchar lcd_busy(void);
void lcd_wcmd(uchar cmd);
void lcd_wdat(uchar dat) ;
void lcd_clr(void)  ;
void lcd_init(void) ;
void LocateXY(uchar x,uchar y) ;
/********us延时函数********/
void delay_us(unsigned  int n)
{
if (n == 0)
return ;
  while (--n);
 }
/********ms延时函数********/
void Delay_ms(uint xms)
{
 	int i,j;
  for(i=0;i<xms;i++)
	{ for(j=0;j<1140;j++) ; }
}
/********以下是LCD忙碌检查函数********/
uchar lcd_busy()
{
  uchar result;
  DDRC=0X00;//端口A设为输入
	RS_0;
  RW_1;
  EN_1;
  delay_us(40);
	result = PINC&0x80;    //读入PB7口，为高电平则忙
  EN_0;
	return result;
}
/********以下是写指令寄存器IR函数********/
void lcd_wcmd(uchar cmd)
{
DDRC=0xff;		//PA设为输出
  // while(lcd_busy());
  RS_0;
  RW_0;
  EN_0;
 	PORTC=cmd;//向PA口输出命令
delay_us(40);
	EN_1;
  delay_us(40);
  EN_0;
}
/********以下是写寄存器DR函数********/
void lcd_wdat(uchar dat)
{
 	DDRC=0xff;		//PA设为输出
  // while(lcd_busy());
  RS_1;
  RW_0;
  EN_0;
  PORTC= dat;//向PA口写入数据
  delay_us(40);
	EN_1;
  delay_us(40);
	EN_0;
}
/********以下是LCD清屏函数********/
void lcd_clr()
{
lcd_wcmd(0x01);          //清除LCD的显示内容
Delay_ms(5);
}
/********以下是LCD初始化函数********/
void lcd_init()
{
Delay_ms(15);             //等待LCD电源稳定
  while(lcd_busy());
	lcd_wcmd(0x38);          //16*2显示，5*7点阵，8位数据
  Delay_ms(10);
  while(lcd_busy());
	lcd_wcmd(0x38);
  Delay_ms(10);
  while(lcd_busy());
	lcd_wcmd(0x38);
  Delay_ms(10);
  while(lcd_busy());
  lcd_wcmd(0x0c);          //显示开，关光标
  Delay_ms(10);
  while(lcd_busy());
	lcd_wcmd(0x06);          //移动光标
  Delay_ms(10);
  while(lcd_busy());
	lcd_wcmd(0x01);          //清除LCD的显示内容
  Delay_ms(10);
}
/********光标定位函数,x为显示列，y为显示行********/
 void LocateXY(uchar x,uchar y)
{
if (y == 0) //第0行显示
 	{
 		lcd_wcmd(0x80 + x);
 	 }
 	 else //第1行显示
 	 {
 	 	lcd_wcmd(0xC0 + x);
 	}
}

unsigned char dir = 0;
unsigned char voice=0;
unsigned char *p=&voice;
unsigned char stand_by=0;
unsigned char a=0x30;
unsigned char b=0x30;

void port_init()
{
    DDRB=0xF8;
    PORTB=0xff;
    DDRA=0xFF;
    PORTA=0xff;
    PORTC=0xff;
    DDRC=0xff;
    DDRD=0x00;
    PORTD=0xff;
}

void timer0_init()
{
    TCCR0=0x69;
    OCR0=voice;
    SREG=0x80;
}

void Turn_left()
{
    if(*p>0)
    *p=*p-5;
    if(a>0x30)
    a--;
    else if(b==0x30)
    ;
    else a=0x39;
    if(a==0x39)
    {
        if(a>0x30)
        b--;
    }
}

void Turn_right()
{
    if(*p<255)
    *p=*p+5;
    if(a<0x39)
    {
        if(b<0x35)
        a++;
    }
    else a=0x30;
    if(a==0x30)
    {
        if(b<0x35)
        b++;
    }
}

void Dir()
    {
    if(dir == 0)
    {
        if(PIN_A & (!PIN_B))
        {
            dir = 2;
        }
    else if(PIN_B & (!PIN_A))
    {
        dir = 4;
    }else
    {
        dir = 0;
    }
    }
    else if(dir == 2)
    {
        if(PIN_A & (!PIN_B))
        {
            dir = 2;
        }
        else if((!PIN_A) & (!PIN_B))
        {
            Turn_right();
            dir = 0;
         }
        else
        {
            dir = 0;
        }
    }
    else if(dir == 4)
    {
        if(PIN_B & (!PIN_A))
        {
            dir = 4;
        }
        else if((!PIN_A) & (!PIN_B))
        {
            Turn_left();
            dir = 0;
        }
        else
        {
            dir = 0;
        }
    }
    else if(PIN_B & PIN_A)
    {
        dir = 0;
    }
}

void WUYONG()
{
    LocateXY(0,1);          //输入: HLJU EEC IC 2016
    lcd_wdat(0b01001000);   //H
    lcd_wdat(0b01001100);   //L
    lcd_wdat(0b01001010);   //J
    lcd_wdat(0b01010101);   //U
    lcd_wdat(0b00100000);   //
    lcd_wdat(0b01000101);   //E
    lcd_wdat(0b01000101);   //E
    lcd_wdat(0b01000011);   //C
    lcd_wdat(0b00100000);   //
    lcd_wdat(0b01001001);   //I
    lcd_wdat(0b01000011);   //C
    lcd_wdat(0b00100000);   //
    lcd_wdat(0b00110010);   //2
    lcd_wdat(0b00110000);   //0
    lcd_wdat(0b00110001);   //1
    lcd_wdat(0b00110110);   //6
}

void main()
{
    port_init();
    timer0_init();
    lcd_init();
    WUYONG();
    while(1)
    {
        Dir();
        OCR0=voice;
        LocateXY(5,0);
        lcd_wdat(0b01010110);
        lcd_wdat(0b01001111);
        lcd_wdat(0b01001100);
        lcd_wdat(0b00100000);
        lcd_wdat(b);
        lcd_wdat(a);
    }
}
