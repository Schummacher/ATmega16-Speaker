#include <iom16v.h>
#include <macros.h>

/********ȫ�ֱ���********/
unsigned int ask[3];
unsigned char ash = 0;
unsigned char ass = 0;
unsigned int ii, jj;
unsigned char kk;
/********�궨��********/
#define RS_0  PORTD &= ~(1 << PD4)	//RSΪ0
#define RS_1  PORTD |=  (1 << PD4)	//RSΪ1
#define RW_0  PORTD &= ~(1 << PD5)	//RWΪ0
#define RW_1  PORTD |=  (1 << PD5)	//RWΪ1
#define EN_0  PORTD &= ~(1 << PD6)	//ENΪ0
#define EN_1  PORTD |=  (1 << PD6)	//ENΪ1

/********��������********/
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

/********us��ʱ����********/
void Delay_us(unsigned  int n){
	if (n == 0) 
		return;
	while (--n);
 }

/********ms��ʱ����********/
void Delay_ms(unsigned int xms){
	int i,j;
	for(i = 0; i < xms; i++)
		for(j = 0;j < 1140; j++);
}   

/********������дָ��Ĵ���IR����********/
void LCD_wcmd(unsigned char cmd){
	DDRC = 0xff;					//PA��Ϊ���
	RS_0;
	RW_0;
	EN_0;
	PORTC = cmd;					//��PA���������
	Delay_us(40);
	EN_1;
	Delay_us(40);
	EN_0;
} 

/********������д�Ĵ���DR����********/
void LCD_wdat(unsigned char dat){
	DDRC = 0xff;					//PA��Ϊ���
	RS_1;
	RW_0;
	EN_0;
	PORTC = dat;					//��PA��д������
	Delay_us(40);
	EN_1;
	Delay_us(40);
	EN_0;                                                            
}

/********������LCD��������********/
void LCD_clr(){
	LCD_wcmd(0x01);					//���LCD����ʾ����
	Delay_ms(5);
}

/********������LCD��ʼ������********/
void LCD_init(){
	Delay_ms(15);					//�ȴ�LCD��Դ�ȶ�
	LCD_wcmd(0x38);					//16*2��ʾ��5*7����8λ����
	Delay_ms(10);
	LCD_wcmd(0x38);
	Delay_ms(10);
	LCD_wcmd(0x38);
	Delay_ms(10);
	LCD_wcmd(0x0c);					//��ʾ�����ع��
	Delay_ms(10);
	LCD_wcmd(0x06);					//�ƶ����
	Delay_ms(10);
	LCD_wcmd(0x01);					//���LCD����ʾ����
	Delay_ms(10);
}  

/********��궨λ����,xΪ��ʾ�У�yΪ��ʾ��********/
 void LocateXY(unsigned char x,unsigned char y){
	 if (y == 0)					//��0����ʾ
		 LCD_wcmd(0x80 + x);
	 else							//��1����ʾ
		 LCD_wcmd(0xC0 + x);
}

/********��ʾ���ַ���д�뺯��,xΪ��ʾ�У�yΪ��ʾ��********/
 void LCD_write_str(unsigned char x,unsigned char y,unsigned char *s){
	 LocateXY(x,y);					//��λ���λ��
	 while (*s){					//���û����
		 LCD_wdat( *s);				//д�ַ�
		 s ++;						//ָ����һ�ַ�
	 }
 }

/********��ʾ�����ַ�д�뺯��********/
 void LCD_write_char(unsigned char x,unsigned char y,unsigned char data){
	 LocateXY(x,y);					//��λ���λ��
	 LCD_wdat(data);
 }


/********�˿ڳ�ʼ��********/
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

/********ɨ��PA(ADC)��ѹֵ********/
void ADC0_date(){
	//ADMUX = 0x00;
	ADMUX = 0x40;					//ADC�ο���ѹΪAVcc��ADC����Ҷ��룬ѡ��ͨ��ADC0
	ADCSRA = 0x86;					//ʹ��ADת����ADCʱ��64��Ƶ
	ADCSRA |= (1 << ADSC);			//��ʼADת��
	while(!(ADCSRA & (1 << ADIF)));	//�ȴ�ת�����
	ADCSRA |= (1 << ADIF);			//����ADC�жϱ�־λ
}

void ADC1_date(){
	//ADMUX = 0x00;
	ADMUX = 0x41;					//ADC�ο���ѹΪAVcc��ADC����Ҷ��룬ѡ��ͨ��ADC0
	ADCSRA = 0x86;					//ʹ��ADת����ADCʱ��64��Ƶ
	ADCSRA |= (1 << ADSC);			//��ʼADת��
	while(!(ADCSRA & (1 << ADIF)));	//�ȴ�ת�����
	ADCSRA |= (1 << ADIF);			//����ADC�жϱ�־λ
}

void ADC2_date(){
	//ADMUX = 0x00;
	ADMUX = 0x42;					//ADC�ο���ѹΪAVcc��ADC����Ҷ��룬ѡ��ͨ��ADC0
	ADCSRA = 0x86;					//ʹ��ADת����ADCʱ��64��Ƶ
	ADCSRA |= (1 << ADSC);			//��ʼADת��
	while(!(ADCSRA & (1 << ADIF)));	//�ȴ�ת�����
	ADCSRA |= (1 << ADIF);			//����ADC�жϱ�־λ
}

void ADC4_date(){
	//ADMUX = 0x00;
	ADMUX = 0x44;					//ADC�ο���ѹΪAVcc��ADC����Ҷ��룬ѡ��ͨ��ADC0
	ADCSRA = 0x86;					//ʹ��ADת����ADCʱ��64��Ƶ
	ADCSRA |= (1 << ADSC);			//��ʼADת��
	while(!(ADCSRA & (1 << ADIF)));	//�ȴ�ת�����
	ADCSRA |= (1 << ADIF);			//����ADC�жϱ�־λ
}

void ADC5_date(){
	//ADMUX = 0x00;
	ADMUX = 0x45;					//ADC�ο���ѹΪAVcc��ADC����Ҷ��룬ѡ��ͨ��ADC0
	ADCSRA = 0x86;					//ʹ��ADת����ADCʱ��64��Ƶ
	ADCSRA |= (1 << ADSC);			//��ʼADת��
	while(!(ADCSRA & (1 << ADIF)));	//�ȴ�ת�����
	ADCSRA |= (1 << ADIF);			//����ADC�жϱ�־λ
}

/********0~1023ת��0~5000********/
float ADC_change(float i){
	i /= 1023.0;
	i *= 5;
	i *= 100;
	return i;
}

/********1602����λ��********/
void LED_display_ADC0(unsigned int i){		//BAT����
	int a, b, c;
	a = i / 100;				//��λ
	b = i % 100 /10;			//ʮλ
	c = i % 10;					//��λ
	ask[0] = a + 48;			//��λ������ASCII��
	ask[1] = b + 48;			//ʮλ������ASCII��
	ask[2] = c + 48;			//��λ������ASCII��
	LocateXY(3,1);				//�ڵڶ��еڶ�������
	LCD_wdat(ask[0]);			//��λ
	LocateXY(5,1);
	LCD_wdat(ask[1]);			//ʮλ
	LCD_wdat(ask[2]);			//��λ
}

void LED_display_ADC1(unsigned int i){		//VCC��ѹ
	int a, b, c;
	a = i / 100;				//��λ
	b = i % 100 /10;			//ʮλ
	c = i % 10;					//��λ
	ask[0] = a + 48;			//��λ������ASCII��
	ask[1] = b + 48;			//ʮλ������ASCII��
	ask[2] = c + 48;			//��λ������ASCII��
	LocateXY(11,0);				//�ڵڶ��еڶ�������
	LCD_wdat(ask[0]);			//��λ
	LCD_wdat(ask[1]);			//ʮλ
	LocateXY(14,0);
	LCD_wdat(ask[2]);			//��λ
}

void LED_display_ADC2(unsigned int i){		//BAT��ѹ
	int a, b, c;
	a = i / 100;				//��λ
	b = i % 100 /10;			//ʮλ
	c = i % 10;					//��λ
	ask[0] = a + 48;			//��λ������ASCII��
	ask[1] = b + 48;			//ʮλ������ASCII��
	ask[2] = c + 48;			//��λ������ASCII��
	LocateXY(11,1);				//�ڵڶ��еڶ�������
	LCD_wdat(ask[0]);			//��λ
	LCD_wdat(ask[1]);			//ʮλ
	LocateXY(14,1);
	LCD_wdat(ask[2]);			//��λ
}

/********LCDԤ��ʾ********/
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

/********LCDʵʱ��ʾ********/
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
	TCCR0=(1<<WGM01)|(1<<WGM00)|(1<<COM01)|(0<<COM00)|(1<<CS00)|(0<<CS02);// T/C0�����ڿ���PWMģʽ������Ƶ���Ƚ�ƥ��ʱ��OC0��λ��������0XFFʱ������OC0  
	OCR0=204;                  	 //�Ƚ�ƥ��Ĵ�����ֵ	
	TCCR2=(1<<WGM21)|(1<<WGM20)|(1<<COM21)|(0<<COM20)|(1<<CS20)|(0<<CS22);// T/C0�����ڿ���PWMģʽ������Ƶ���Ƚ�ƥ��ʱ��OC0��λ��������0XFFʱ������OC0  
	OCR2=0;                  	 //�Ƚ�ƥ��Ĵ�����ֵ
	//SREG=0x80;
	asm("sei");//ʹ��ȫ���ж�
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