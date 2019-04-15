#include "stm32f10x.h"
#include "open.h"
#include "adc.h"

// ADC1转换的电压值通过MDA方式传到SRAM
extern __IO uint16_t ADC_ConvertedValue;

// 局部变量，用于保存转换计算后的电压值 	 
float ADC_ConvertedValueLocal;  

/********************************************
*函数名：int sensor(void)
*函数功能: 通过比较3中颜色下，电压最大的值来暂定此时识别的相应颜色
*函数参数说明：void
*函数返回值：1-blue 2-red 3-green
*******************************************/
int sensor(void)
{
	float bl;
	float re;
	float gr;
	int val;
	float max;
	//测蓝光返回的电压值
	GPIO_ResetBits(GPIOB,GPIO_Pin_10);
	delay_nms(10);
	bl=(float) ADC_ConvertedValue/4096.0*3.3;
//	printf("\r\n blue The current AD value = %f V \r\n",bl); 
	GPIO_SetBits(GPIOB,GPIO_Pin_10);
	val=1;
	max=bl;
	
	//测红光返回的电压值
	GPIO_ResetBits(GPIOB,GPIO_Pin_11);
	delay_nms(10);
	re=(float) ADC_ConvertedValue/4096.0*3.3;
//	printf("\r\n red The current AD value = %f V \r\n",re); 
	GPIO_SetBits(GPIOB,GPIO_Pin_11);
	if(re>max)
	{
		val=2;
		max=re;
	}
	//测绿光返回的电压值
	GPIO_ResetBits(GPIOD,GPIO_Pin_10);
	delay_nms(10);
	gr =(float) ADC_ConvertedValue/4096.0*3.3;
//	printf("\r\n green The current AD value = %f V \r\n",gr); 
	GPIO_SetBits(GPIOD,GPIO_Pin_10);
	if(gr>max)
	{
		val=3;
	}
//	printf("\r\n the current return %d \r\n ",val);
	return val;
}

/********************************************
*函数名：int judge(void)
*函数功能：用于通过读入sensor函数返回的值来，通过某个颜色出现的次数来判断识别的颜色
*函数参数说明：void
*函数返回值：1-blue 2-red 3-green 0-不能判断
*******************************************/

int judge(void)
{
	int cur; //保存当前获取的颜色 1-blue 2-red 3-green
	int blue=0;
	int red=0;
	int green=0;
	int i;
	for(i=0;i<10;i++)
	{
		cur=sensor();
		switch(cur)
		{
			case 1 :
			blue++;
			break;
			case 2 :
			red++;
			break;
			case 3 :
			green++;
			break  ;			
			default :
			printf("\r\n wrong return \r\n");
			break;
		}
	}
	if(blue>4) 
	{
		return 1;
	}
	if(red>5)  
	{
		return 2;
	}
	if(green>5)
	{		
		return 3;
	}
	else
	{
		return 0;
	}
}


int main(void)
{
	int val;
	Open_Init();
// ADC 初始化
	ADC1_Init();
	printf("\r\n ----这是一个ADC实验(DMA传输)-颜色识别---\r\n");
	while (1)
	{
		val=judge();
		switch(val)
		{
			case 1 :
			printf("\r\n blue \r\n");
			break;
			case 2 :
			printf(" \r\n red \r\n");
			break;
			case 3 :
			printf("\r\n green \r\n");	
			break;			
			default :
			printf("\r\n can't judge color \r\n");
			break;
		}
	}
}



/*
int main(void)
{
	Open_Init();
// ADC 初始化
	ADC1_Init();
	printf("\r\n ----这是一个ADC实验(DMA传输)-颜色识别---\r\n");
	while(1)
	{
		
		ADC_ConvertedValueLocal =(float) ADC_ConvertedValue/4096.0*3.3;
		printf("\r\n The current AD value = %f V \r\n",ADC_ConvertedValueLocal); 
		delay_nms(1000);
	}
}

*/
	
	


