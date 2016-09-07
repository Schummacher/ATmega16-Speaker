#include <iom16v.h>
#include <macros.h>

/********全局变量********/
unsigned int ask[3];
unsigned char ash = 0;
unsigned char ass = 0;
unsigned int ii, jj;
unsigned char kk;
/********宏定义********/
#define RS_0  PORTD &= ~(1 << PD4)	//RS为0
#define RS_1  PORTD |=  (1 << PD4)	//RS为1
#define RW_0  PORTD &= ~(1 << PD5)	//RW为0
#define RW_1  PORTD |=  (1 << PD5)	//RW为1
#define EN_0  PORTD &= ~(1 << PD6)	//EN为0
#define EN_1  PORTD |=  (1 << PD6)	//EN为1

/********函数声明********/
void Delay_us(unsigned int n); 
void Delay_ms(unsigned int xms) ;
void LCD_wcmd(unsigned char cmd);     
void LCD_wdat(unsigned char dat) ;     
void LCD_clr(void)  ;
void LCD_init(void) ;
void LocateXY(unsigned char x,unsigned char y) ;
void LCD_write_char(unsigned char x,unsigned char y,unsigned char data) ;
void LCD_write_str(unsigned char x,unsigned char y, unsigned char *s);
void Port_int();
void INT_int();
void Timer_init();
void ADC0_date();
void ADC1_date();
void ADC2_date();
float ADC_change(float i);
void LED_display_ADC0(unsigned int i);
void LED_display_ADC1(unsigned int i);
void LED_display_ADC1(unsigned int i);
void Display_first();
void ADC_run();
void Display_PWM1(unsigned char i);

/********us延时函数********/
void Delay_us(unsigned  int n){
	if (n == 0) 
		return;
	while (--n);
 }

/********ms延时函数********/
void Delay_ms(unsigned int xms){
	int i,j;
	for(i = 0; i < xms; i++)
		for(j = 0;j < 1140; j++);
}   

/********以下是写指令寄存器IR函数********/
void LCD_wcmd(unsigned char cmd){
	DDRC = 0xff;					//PA设为输出
	RS_0;
	RW_0;
	EN_0;
	PORTC = cmd;					//向PA口输出命令
	Delay_us(40);
	EN_1;
	Delay_us(40);
	EN_0;
} 

/********以下是写寄存器DR函数********/
void LCD_wdat(unsigned char dat){
	DDRC = 0xff;					//PA设为输出
	RS_1;
	RW_0;
	EN_0;
	PORTC = dat;					//向PA口写入数据
	Delay_us(40);
	EN_1;
	Delay_us(40);
	EN_0;                                                            
}

/********以下是LCD清屏函数********/
void LCD_clr(){
	LCD_wcmd(0x01);					//清除LCD的显示内容
	Delay_ms(5);
}

/********以下是LCD初始化函数********/
void LCD_init(){
	Delay_ms(15);					//等待LCD电源稳定
	LCD_wcmd(0x38);					//16*2显示，5*7点阵，8位数据
	Delay_ms(10);
	LCD_wcmd(0x38);
	Delay_ms(10);
	LCD_wcmd(0x38);
	Delay_ms(10);
	LCD_wcmd(0x0c);					//显示开，关光标
	Delay_ms(10);
	LCD_wcmd(0x06);					//移动光标
	Delay_ms(10);
	LCD_wcmd(0x01);					//清除LCD的显示内容
	Delay_ms(10);
}  

/********光标定位函数,x为显示列，y为显示行********/
 void LocateXY(unsigned char x,unsigned char y){
	 if (y == 0)					//第0行显示
		 LCD_wcmd(0x80 + x);
	 else							//第1行显示
		 LCD_wcmd(0xC0 + x);
}

/********显示屏字符串写入函数,x为显示列，y为显示行********/
 void LCD_write_str(unsigned char x,unsigned char y,unsigned char *s){
	 LocateXY(x,y);					//定位光标位置
	 while (*s){					//如果没结束
		 LCD_wdat( *s);				//写字符
		 s ++;						//指向下一字符
	 }
 }

/********显示屏单字符写入函数********/
 void LCD_write_char(unsigned char x,unsigned char y,unsigned char data){
	 LocateXY(x,y);					//定位光标位置
	 LCD_wdat(data);
 }


/********端口初始化********/
void Port_int(){
	DDRA  = 0x00;
	PORTA = 0x00;
	DDRB  = 0xF8;
	PORTB = 0xFF;
	DDRC  = 0xFF;
	PORTC = 0xFF;
	DDRD  = 0xF0;
	PORTD = 0xFF;
}

/********扫描PA(ADC)电压值********/
void ADC0_date(){
	//ADMUX = 0x00;
	ADMUX = 0x40;					//ADC参考电压为AVcc，ADC结果右对齐，选择通道ADC0
	ADCSRA = 0x86;					//使能AD转换，ADC时钟64分频
	ADCSRA |= (1 << ADSC);			//开始AD转换
	while(!(ADCSRA & (1 << ADIF)));	//等待转换完成
	ADCSRA |= (1 << ADIF);			//清零ADC中断标志位
}

void ADC1_date(){
	//ADMUX = 0x00;
	ADMUX = 0x41;					//ADC参考电压为AVcc，ADC结果右对齐，选择通道ADC0
	ADCSRA = 0x86;					//使能AD转换，ADC时钟64分频
	ADCSRA |= (1 << ADSC);			//开始AD转换
	while(!(ADCSRA & (1 << ADIF)));	//等待转换完成
	ADCSRA |= (1 << ADIF);			//清零ADC中断标志位
}

void ADC2_date(){
	//ADMUX = 0x00;
	ADMUX = 0x42;					//ADC参考电压为AVcc，ADC结果右对齐，选择通道ADC0
	ADCSRA = 0x86;					//使能AD转换，ADC时钟64分频
	ADCSRA |= (1 << ADSC);			//开始AD转换
	while(!(ADCSRA & (1 << ADIF)));	//等待转换完成
	ADCSRA |= (1 << ADIF);			//清零ADC中断标志位
}

void ADC4_date(){
	//ADMUX = 0x00;
	ADMUX = 0x44;					//ADC参考电压为AVcc，ADC结果右对齐，选择通道ADC0
	ADCSRA = 0x86;					//使能AD转换，ADC时钟64分频
	ADCSRA |= (1 << ADSC);			//开始AD转换
	while(!(ADCSRA & (1 << ADIF)));	//等待转换完成
	ADCSRA |= (1 << ADIF);			//清零ADC中断标志位
}

void ADC5_date(){
	//ADMUX = 0x00;
	ADMUX = 0x45;					//ADC参考电压为AVcc，ADC结果右对齐，选择通道ADC0
	ADCSRA = 0x86;					//使能AD转换，ADC时钟64分频
	ADCSRA |= (1 << ADSC);			//开始AD转换
	while(!(ADCSRA & (1 << ADIF)));	//等待转换完成
	ADCSRA |= (1 << ADIF);			//清零ADC中断标志位
}

/********0~1023转制0~5000********/
float ADC_change(float i){
	i /= 1023.0;
	i *= 5;
	i *= 100;
	return i;
}

/********1602用四位数********/
void LED_display_ADC0(unsigned int i){		//BAT电流
	int a, b, c;
	a = i / 100;				//百位
	b = i % 100 /10;			//十位
	c = i % 10;					//个位
	ask[0] = a + 48;			//百位还换成ASCII码
	ask[1] = b + 48;			//十位还换成ASCII码
	ask[2] = c + 48;			//个位还换成ASCII码
	LocateXY(3,1);				//在第二行第二列输入
	LCD_wdat(ask[0]);			//百位
	LocateXY(5,1);
	LCD_wdat(ask[1]);			//十位
	LCD_wdat(ask[2]);			//个位
}

void LED_display_ADC1(unsigned int i){		//VCC电压
	int a, b, c;
	a = i / 100;				//百位
	b = i % 100 /10;			//十位
	c = i % 10;					//个位
	ask[0] = a + 48;			//百位还换成ASCII码
	ask[1] = b + 48;			//十位还换成ASCII码
	ask[2] = c + 48;			//个位还换成ASCII码
	LocateXY(11,0);				//在第二行第二列输入
	LCD_wdat(ask[0]);			//百位
	LCD_wdat(ask[1]);			//十位
	LocateXY(14,0);
	LCD_wdat(ask[2]);			//个位
}

void LED_display_ADC2(unsigned int i){		//BAT电压
	int a, b, c;
	a = i / 100;				//百位
	b = i % 100 /10;			//十位
	c = i % 10;					//个位
	ask[0] = a + 48;			//百位还换成ASCII码
	ask[1] = b + 48;			//十位还换成ASCII码
	ask[2] = c + 48;			//个位还换成ASCII码
	LocateXY(11,1);				//在第二行第二列输入
	LCD_wdat(ask[0]);			//百位
	LCD_wdat(ask[1]);			//十位
	LocateXY(14,1);
	LCD_wdat(ask[2]);			//个位
}

/********LCD预显示********/
void Display_first(){
	LCD_clr();
	Delay_ms(100);
	LCD_write_str(0,0,"################");
	LCD_write_str(0,1,"################");
	Delay_ms(100);
	LCD_write_str(0,0,"DC TO DC IN00.0V");
	LCD_write_str(0,1,"OUT0.00A   00.0V");
}

unsigned char R_test(unsigned int v1, unsigned int v2)
{
	float i;
	i=(v2*10)/(v1-v2)*16;
	return i;
}

/********LCD实时显示********/
void ADC_run(){
	unsigned char i;
	Delay_ms(800);
	ADC0_date();
	LED_display_ADC0(ADC_change(ADC));
	ADC1_date();
	LED_display_ADC1(ADC_change(ADC));
	ADC2_date();
	LED_display_ADC2(ADC_change(ADC));
	ADC4_date();
	ii = ADC;
	Delay_ms(100);
	ADC5_date();
	jj = ADC;
	i= R_test(ii, jj);
	if (i > 200)
		i = 79;
	OCR2 = i;
	
}

void Timer_init(){
	TCCR0=(1<<WGM01)|(1<<WGM00)|(1<<COM01)|(0<<COM00)|(1<<CS00)|(0<<CS02);// T/C0工作于快速PWM模式，不分频。比较匹配时，OC0置位，记数到0XFF时，清零OC0  
	OCR0=204;                  	 //比较匹配寄存器初值	
	TCCR2=(1<<WGM21)|(1<<WGM20)|(1<<COM21)|(0<<COM20)|(1<<CS20)|(0<<CS22);// T/C0工作于快速PWM模式，不分频。比较匹配时，OC0置位，记数到0XFF时，清零OC0  
	OCR2=0;                  	 //比较匹配寄存器初值
	//SREG=0x80;
	asm("sei");//使能全局中断
}

void main(){
	Port_int();
	LCD_init();
	Timer_init();
	Display_first();
	PORTB |= 0x20;
	PORTB &= 0xEF;
	Delay_ms(1000);
	while(1){
			ADC_run();
	}
}