# 소스 코드

#include <mega128.h>
#include <delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <lcd.h>

#define mapsize 16
#define Q0 PORTC.4
#define Q1 PORTC.5
#define Q2 PORTC.6
#define Q3 PORTC.7
#define FND PORTF
#define DCM_IN1 PORTC.2
#define DCM_IN2 PORTC.3
#define STEP_MOTOR PORTE
#define STEP_LED PORTD

#asm
    .equ __lcd_port=0x1B
#endasm


int i=0, Score=0;
char obs; //장애물
int fnd1000,fnd100,fnd10,fnd1;
int remainder;
unsigned fnd[17]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xd8,0x80,0x98,0x88,0x83,0xc6,0xa1,0x86,0x8e,0x7f,};

void init(void)
{
    // PORTA LCD
    PORTA=0x00;
    DDRA=0xFF;
    // PORTB LED
    PORTB=0x00;
    DDRB=0xff;
      
    // PORTC 0,1 : 스위치 2, 3 : DC모터 4,5,6,7 : 7-segment Q
    PORTC=0x00;
    DDRC=0xF0;
    PINC=0x00;
    // PORTD STEP_MOTOR LED
    PORTD=0x00;
    DDRD=0xFF;
    // PORTF STEP_MOTOR Data
    PORTF=0X00;
    DDRF=0XFF;
    // PORTE 7-segment Data
    PORTE=0x00;
    DDRE=0xFF;
    
    FND = fnd[0];
    Q0 = 1;
    Q1 = 1;
    Q2 = 1;
    Q3 = 1;
}
void display(int); // 7-segment
void DC_M_CW();  //  DC MOTOR 작동
void DC_M_STOP(); //  DC STOP 
void Addmap(char [] , char); // 맵 업데이트
void STEP_M_CW();   // STEP MOTOR 
void STEP_M_STOP();   // STEP STOP
void STEP_M_CCW();

void main(){
    
    unsigned char pName[] = {0B11101111}; // 캐릭터
    /* 기본 맵 작성 */
    char map[100] ="__________________________";
    map[7]=0b11111111;
    map[11]=0b11111111;
    map[3]=0b11111111;
   
    init();

    lcd_init(16);
    lcd_gotoxy(0,1);
    lcd_puts(pName);   
    
    lcd_gotoxy(1,1);
    lcd_puts(map);
    delay_ms(1000);
    
    while(1)
    {   i = rand()%10;    // 랜덤으로 장애물 생성 
        if ( (i%2)==0)
        {       
            obs = 0b11111111;
        }
        else if( (i%2)!=0 )
        {
            obs = '_';
        }
        // LCD 핸들링
        PORTB = 0b11111111;
        lcd_gotoxy(1,0);
        lcd_puts(" ");
        lcd_gotoxy(0,1);
        lcd_puts(pName); 
        
        lcd_gotoxy(1,1);
        Addmap(map,obs);
        
        lcd_puts(map);
        delay_ms(1000);
        
        if (PINC.0 == 0)
        {   
            lcd_gotoxy(0,1);
            lcd_puts(" ");
            lcd_gotoxy(1,0);
            lcd_puts(pName);
            if (map[0]==0b11111111)
            {
                PORTB = 0b00001111;
                display(Score);
                STEP_M_CW();
            }
            delay_ms(1000);
        }
        else
        {
            if (map[0]==0b11111111)
            {   
                
                PORTB = 0b11110000;
                DC_M_CW();
                DC_M_STOP();
                STEP_M_CCW();
                 if(Score >=100)
                 Score -= 100;               
            }
        }                                          
    }          
}
void Addmap(char cur[], char randC)
{        
    for (i = 0; i<mapsize - 2; i++)
    {
        cur[i] = cur[i + 1];

    }
    cur[mapsize-2] = randC;
}
void display(int Scor){
    Score += 50;
    fnd1000 = Scor/1000;
    remainder = Scor%1000;
    fnd100 = remainder/100;
    remainder = Scor%100;
    fnd10 = remainder/10;
    remainder = Scor%10;
    fnd1 = remainder;
    for (i=0;i<50;i++)
    {
        if (Scor >= 1000)
        {
            FND = fnd[fnd1000];
            Q3 = 0;
            Q2 = 1;
            Q1 = 1;
            Q0 = 1;
            delay_ms(10);
        }
        if (Scor >= 100)
        {
            FND = fnd[fnd100];
            Q3 = 1;
            Q2 = 0;
            Q1 = 1;
            Q0 = 1;
            delay_ms(10);
        }
        FND = fnd[fnd10];
        Q3 = 1;
        Q2 = 1;
        Q1 = 0;
        Q0 = 1;
        delay_ms(10);
        FND = fnd[fnd1];
        Q3 = 1;
        Q2 = 1;
        Q1 = 1;
        Q0 = 0;
        delay_ms(10);
    }        
    Q0 = 1;
    Q1 = 1;
    Q2 = 1;
    Q3 = 1;    
}
void DC_M_CW()
{
    DCM_IN1 = 1;
    DCM_IN2 = 0;
    delay_ms(60);
}

void DC_M_STOP()
{
    DCM_IN1 = 0;
    DCM_IN2 = 0;
}

void STEP_M_CW()
{

    STEP_MOTOR = 0x01;
    STEP_LED = 0x01;
    delay_ms(100);
    
    STEP_MOTOR = 0x02;
    STEP_LED = 0x02;
    delay_ms(100);
    
    STEP_MOTOR = 0x04;
    STEP_LED = 0x04;
    delay_ms(100);
    
    STEP_MOTOR = 0x08;
    STEP_LED = 0x08;
    delay_ms(100);
    
}
void STEP_M_CCW()
{

    STEP_MOTOR = 0x08;
    STEP_LED = 0x08;
    delay_ms(100);
    
    STEP_MOTOR = 0x04;
    STEP_LED = 0x04;
    delay_ms(100);
    
    STEP_MOTOR = 0x02;
    STEP_LED = 0x02;
    delay_ms(100);
    
    STEP_MOTOR = 0x01;
    STEP_LED = 0x01;
    delay_ms(100);
    
}
void STEP_M_STOP()
{
    STEP_MOTOR = 0x00;
    STEP_LED = 0x00;
}
