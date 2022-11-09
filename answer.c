/* ����ͷ�ļ� */
#include "ioCC2530.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
/* �ĸ�LED�ƵĶ˿ڶ��� */
#define D3 P1_0
#define D4 P1_1
#define D5 P1_3
#define D6 P1_4
/* �����Ķ˿ڶ��� */
#define KEY1 P1_2


/* �������� */
unsigned char workMode=0;     //ϵͳ����ģʽ:0����ˮ��ģʽ  1���Զ�ģʽ
unsigned int  counter=0;      //��ʱ���жϼ�����
unsigned int  sensorValue=0;  //���洫������ֵ
unsigned char temp;//�Ӵ��ڽ���һ���ַ�
char uart_rx_buf[20];//���ڽ��ջ�����
char uart_rx_len=0;//���ڽ����ַ�����

/**********LED�˿ڳ�ʼ��************************/
void InitLED(void)
{
   
   /*.......������1��ʼ��LED��IO�˿ڳ�ʼ�� 4��LEDĬ��ΪϨ��״̬....................*/
    P1SEL &=~0x1B;      //����D3 D4 D5 D6Ϊ��ͨIO�� 
    P1DIR |= 0x1B;     //����D3 D4 D5 D6Ϊ�������
    D3=0;
    D4=0;
    D5=0;
    D6=0;
    
  /*.......������ ����.......................................*/
}

/************��ˮ��ģʽ**************/
void warterLedMode()
{
    //�ɿ�����д ʵ����ˮ��Ч������ʱ����ʱ10ms����һ���жϣ�ÿ����ʱ��ԼΪ250ms
    /*.......������2 ��ʼ��....................*/
    if(counter<25){
        D3 = 1;
        D4 = 0;
        D5 = 0;
        D6 = 0;
    }
    else if(counter<50){
        D3 = 0;
        D4 = 1;
        D5 = 0;
        D6 = 0;
    }
    else if(counter<75){
        D3 = 0;
        D4 = 0;
        D5 = 1;
        D6 = 0;
    }
    else if(counter<100){
        D3 = 0;
        D4 = 0;
        D5 = 0;
        D6 = 1;
    }
    else
        counter=0;
   /*.......������2 ����.......................................*/
}




void InitKey(void)
{

   /*.......������3 ��ʼ������KEY��Ӧ����ģʽ����ͨIO�ڣ����룬������ģʽ....................*/
    P1SEL&=~0X04;       //����KEY1Ϊ��ͨIO��
    P1DIR&=~0X04;       //����KEY1Ϊ��������
    P1INP&=~0X04;       //����KEY1Ϊ������ģʽ
   /*.......������3 ����.......................................*/
}
/***********��ʱ����ʼ��************************/
void InittTimer1(void)
{
    T1IF=0;                  //���timer1�жϱ�־
    T1STAT &= ~0x01;         //���ͨ��0�жϱ�־
    
    T1CTL = 0x0A;            //����32��Ƶ��ģ�Ƚϼ�������ģʽ 
    
    T1CCTL0 |= 0x04;         //�趨timer1ͨ��0�Ƚ�ģʽ
    T1CC0L = 10000&0xFF;     //��10000�ĵ�8λд��T1CC0L
    T1CC0H = (10000>>8)&0xFF;//��10000�ĸ�8λд��T1CC0H
    
    T1IE = 1;                //ʹ�ܶ�ʱ��1���жϣ�����дΪIEN1 |= 0x02;
   }

void ScanKey(void)
{
  
  /*.......������4 ��ʼ��Ҫ��ʵ�ְ���ɨ��
  ʵ��Ĭ���ϵ�����ˮ��ģʽ��
  ��1�� �����Զ�����ģʽ
  �ٰ�1�Σ�������ˮ��ģʽ  
  ....................*/
    static char key1down=0;
    
    if(KEY1==0){//����а���
        if(key1down==0) 
          key1down=1;
        else if(key1down==1) 
        {
          workMode=(workMode+1)%2;
          if(workMode==1)    
          {
            counter=0;
          }
        }
        
    }else{
        key1down=0;
       
    }
    
   
     /*.......������4 ����.......................................*/
}
/**********����ͨ�ų�ʼ��************************/
void InitUART0(void)
{
    PERCFG = 0x00; //usart0 ʹ�ñ���λ��1 TX-P0.3 RX-P0.2  
    P0SEL |= 0x0c; //  P0.2 P0.3 �������蹦��
    U0CSR |= 0xC0; //uartģʽ �������
    P2DIR &= ~0xC0;   //P0������ΪUART��ʽ  
   
   /*.......������5 ��ʼ�� ������115200bps....................*/
    U0BAUD = 216;       //����������Ϊ115200bps
    U0GCR = 11;
     /*.......������5 ����.......................................*/
    
    U0UCR |= 0x80;//������ ����żУ�� 8λ����λ  1λֹͣλ
    URX0IF = 0;         //����UART0 RX�жϱ�־
    UTX0IF = 0;         //����UART0 TX�жϱ�־
    URX0IE = 1;         //ʹ�ܽ����ж�
}


/************��ʱ��T1�жϷ����ӳ���**************/
#pragma vector = T1_VECTOR //�жϷ����ӳ���
__interrupt void T1_ISR(void)
{   
    counter++;
    ScanKey();
}



/*************** �����ڷ���ָ�����ȵ�����  ***************/
void uart_tx_string(char *data_tx,int len)  
{   
  unsigned int j;  
  for(j=0;j<len;j++)  
  {   
    U0DBUF = *data_tx++;  // ��Ҫ���͵�1�ֽ�����д��U0DBUF
    while(UTX0IF == 0);  // �ȴ�TX�жϱ�־����U0DBUF����
    UTX0IF = 0;   // ����TX�жϱ�־
  }
} 



/************UART0 �����жϷ����ӳ���**************/
#pragma vector = URX0_VECTOR //�жϷ����ӳ���
__interrupt void UART0_RX_ISR(void)
{
   
    URX0IF=0;
    temp = U0DBUF;
  
    if(temp !='#')
    {
      /*.......������6 ��ʼ��..
      �ѴӴ��ڽ��յ������ݴ�ŵ�uart_rx_buf�У���uart_rx_len����ס�±�..................*/
      uart_rx_buf[uart_rx_len++] = temp;
       /*.......������6 ����.......................................*/
    }
    else
    {
      U0CSR &= ~0x40;//��ֹ����
      
      uart_tx_string("����ADCֵ=",sizeof("����ADCֵ="));
      /*.......������8 ��ʼ��..�ѽ��յ���uart_rx_buf���ݷ��ͻش���..................*/
      uart_tx_string(uart_rx_buf,uart_rx_len);
       /*.......������8 ����.....................*/
   
      sensorValue=atoi(uart_rx_buf);//�����洫������ת������������ŵ����洫��������sensorValue��
      memset(uart_rx_buf,0,sizeof(uart_rx_buf));
      uart_rx_len=0;      
      
      U0CSR |= 0x40;//�������
    }
}



/***************�Զ�ģʽ*****************/
void autoControl()
{
    //�ɿ�����д �Զ�ģʽ�µ�״̬����D3�� D4����
     ���ݻ��洫��������ֵ�Զ�����D5 D6�����״̬
    //sensorValue>=200ʱ��ʵ��D5 D6����������D5 D6����
    /*.......������7 ��ʼ��....................*/
    D3=0;
    D4=1;
    if(sensorValue<200)
    {
        D5=0;D6=0;  
    }
    else
    {
        D5=1;D6=1;  
    }
   /*.......������7 ����.......................................*/
}

/************main�������**************************/
void main(void)
{
    //ʱ�ӳ�ʼ��,�ٶ�����Ϊ32MHz
    CLKCONCMD &= 0X80;
    while(CLKCONSTA&0X40);
    
    InitLED();
    InitKey();
    InittTimer1();      //��ʼ��Timer1
    InitUART0();        //UART0��ʼ��

    EA = 1;             //ʹ��ȫ���ж�
    
    while(1)
    {
        if(workMode==0)
            warterLedMode();//��ˮ��ģʽ
        else if(workMode==1)
             autoControl();//�Զ�����ģʽ
       
    }
}
