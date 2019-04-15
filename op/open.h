#ifndef __OPEN_H
#define __OPEN_H

#include "stdio.h"
#include "stm32f10x.h"
#include "stm32f10x_it.h"

GPIO_InitTypeDef GPIO_InitStructure;
ErrorStatus HSEStartUpStatus;             /* 枚举变量，定义高速时钟的起动状态 */
NVIC_InitTypeDef NVIC_InitStruct;
EXTI_InitTypeDef EXTI_InitStructure;  
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;
TIM_BDTRInitTypeDef TIM_BDTRInitStructure;

/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures System Clocks
*******************************************************************************/

void RCC_Configuration(void)
{
	/* 将外设RCC寄存器重设为默认值，即有关寄存器复位，但该函数不改禦CC_CR的HSITRIM[4:0]位，也不重置寄存器RCC_BDCR和寄存器RCC_CSR */
	RCC_DeInit();
	/* 使能外部HSE高速晶振 */
	RCC_HSEConfig(RCC_HSE_ON);
	/* 等待HSE高速晶振稳定，或者在超时的情况下退出 */
	HSEStartUpStatus = RCC_WaitForHSEStartUp();
	/* SUCCESS:HSE晶振稳定且就绪，ERROR：HSE晶振未就绪 */
	if(HSEStartUpStatus == SUCCESS)
	{
		/* 使能flash预取指令缓冲区。这两句跟RCC没直接关系 */
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
		/* 设置FLASH存储器延时时钟周期数，2是针对高频时钟的，
		FLASH_Latency_0：0延时周期，FLASH_Latency_1：1延时周期 
		FLASH_Latency_2：2延时周期 */
		FLASH_SetLatency(FLASH_Latency_2);
		/* HCLK=SYSCLK 设置高速总线时钟=系统时钟 */
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		/* PCLK1=HCLK/2 设置低速总线1时钟=高速时钟的二分频*/
		RCC_PCLK1Config(RCC_HCLK_Div2);
		/* PCLK2=HCLK 设置低速总线2时钟=高速总线时钟 */
		RCC_PCLK2Config(RCC_HCLK_Div1);
		/* Set PLL clock output to 72MHz using HSE (8MHz) as entry clock */
		/* 利用锁相环将HSE外部8MHz晶振9倍频到72MHz */ 
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
		/* Enable PLL：使能PLL锁相环 */
		RCC_PLLCmd(ENABLE);
		
		 /* Wait till PLL is ready：等待锁相环输出稳定 */
     /* RCC_FLAG_HSIRDY：HSI晶振就绪，RCC_FLAG_HSERDY：HSE晶振就绪
        RCC_FLAG_PLLRDY：PLL就绪，RCC_FLAG_LSERDY：LSE晶振就绪
        RCC_FLAG_LSIRDY：LSI晶振就绪，RCC_FLAG_PINRST：引脚复位
        RCC_FLAG_PORRST：POR/PDR复位，RCC_FLAG_SFTRST：软件复位
        RCC_FLAG_IWDGRST：IWDG复位，RCC_FLAG_WWDGRST：WWDG复位
        RCC_FLAG_LPWRRST：低功耗复位 */
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)	;
		
		 /* Select PLL as system clock source：将锁相环输出设置为系统时钟 */
     /* RCC_SYSCLKSource_HSI：选择HSI作为系统时钟
        RCC_SYSCLKSource_HSE：选择HSE作为系统时钟
        RCC_SYSCLKSource_PLLCLK：选择PLL作为系统时钟*/
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		 /* 等待PLL作为系统时钟标志位置位 */
     /* 0x00：HSI作为系统时钟；0x04：HSE作为系统时钟
        0x08：PLL作为系统时钟 */
		while(RCC_GetSYSCLKSource() != 0x08);	
	}
	
	 /* Enable GPIOA~E and AFIO clocks：使能外围端口总线时钟。注意各外设的隶属情况，不同芯片和开发板的分配不同*/
 	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_AFIO, ENABLE);
   /* USART1 clock enable：USART1时钟使能 */
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	 /* TIM1 clock enable：TIM1时钟使能 */
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
   /* TIM2 clock enable：TIM2时钟使能*/
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	 /* ADC1 clock enable：ADC1时钟使能*/
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
}


/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures GPIO Modes
*******************************************************************************/

void GPIO_Configuration(void)
{			
	  /* Configure USART1 Tx (PA.09) as alternate function push-pull */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//配置以下两个
	  /* Configure USART1 Rx (PA.10) as input floating */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
//	  /* Configure LEDs IO */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//设置为输入
   GPIO_Init(GPIOB,&GPIO_InitStructure);
	//配置PE8/PE10/PE12
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_10|GPIO_Pin_12;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//设置为输出
    GPIO_Init(GPIOE,&GPIO_InitStructure);
	//配置运动
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
  /* Configure USART1 Rx (PA.10) as input floating */
	//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
//	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
 //   GPIO_Init(GPIOA, &GPIO_InitStructure);
	
//	  /* Configure LEDs IO */
	//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
//	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 //  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 //   GPIO_Init(GPIOB, &GPIO_InitStructure);
		
	
//	  /* Configure Motors I/O */
//	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(GPIOC, &GPIO_InitStructure);	

//	  /* Configure KEY I/O PC8 to PC11 */
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_Init(GPIOC, &GPIO_InitStructure);

//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_Init(GPIOC, &GPIO_InitStructure);

//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_Init(GPIOC, &GPIO_InitStructure);

//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_Init(GPIOC, &GPIO_InitStructure);
//			
//		/* Configure infrared IO*/
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_Init(GPIOE, &GPIO_InitStructure);

//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_Init(GPIOE, &GPIO_InitStructure);

//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
//		GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN_FLOATING;
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_Init(GPIOE, &GPIO_InitStructure);

//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
//		GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN_FLOATING;
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_Init(GPIOE, &GPIO_InitStructure);
		
//		/* GPIOA Configuration: Channel 1, 2, 3 and 4 Output */
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_Init(GPIOA, &GPIO_InitStructure);
//		GPIO_PinRemapConfig(GPIO_FullRemap_TIM1, ENABLE);
		

//		/* GPIOB Configuration: Channel 1N, 2N and 3N Output */
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_Init(GPIOB, &GPIO_InitStructure); 
//  
//		/* GPIOB Configuration: BKIN pin */   
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_Init(GPIOB, &GPIO_InitStructure);

}


 /*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures NVIC Modes for T1
*******************************************************************************/

void NVIC_Configuration(void)
{

#ifdef  VECT_TAB_RAM  
  /* Set the Vector Table base location at 0x20000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif

  /* Configure the NVIC Preemption Priority Bits[配置优先级组] */  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

  /* Configure INT IO  PC9 enable exti9_5*/
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource9);  
  EXTI_InitStructure.EXTI_Line=EXTI_Line9;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority =0;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelCmd =ENABLE;
  NVIC_Init(&NVIC_InitStruct);

  /* Configure INT IO  PE4 enable exti4*/
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource4);  
  EXTI_InitStructure.EXTI_Line=EXTI_Line4;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  NVIC_InitStruct.NVIC_IRQChannel = EXTI4_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority =0;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelCmd =ENABLE;
  NVIC_Init(&NVIC_InitStruct);

  /* Configure INT IO  PE5 enable exti9_5*/
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource5);  
  EXTI_InitStructure.EXTI_Line=EXTI_Line5;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority =0;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelCmd =ENABLE;
  NVIC_Init(&NVIC_InitStruct);
	
	/* Enable the TIM2 gloabal Interrupt [允许TIM1全局中断] */
	NVIC_InitStruct.NVIC_IRQChannel = TIM1_UP_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;  
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	/* Enable the TIM2 gloabal Interrupt [允许TIM2全局中断] */
	NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;  
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
}

/*******************************************************************************
* Function Name  : USART_Configuration
* Description    : Configures the USART1.
*******************************************************************************/

void USART_Configuration(void)
{
	USART_InitTypeDef USART_InitStructure;
  USART_ClockInitTypeDef USART_ClockInitStructure;

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =	USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USART1, &USART_InitStructure);

	USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;
	USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
	USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;
	USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;
	USART_ClockInit(USART1, &USART_ClockInitStructure);

	/* Enables the USART1 transmit interrupt */
	USART_ITConfig(USART1, USART_IT_RXNE , ENABLE);
	USART_ClearFlag(USART1, USART_FLAG_TC);  
	/* Enable the USART1 */
	USART_Cmd(USART1, ENABLE);
}


/*******************************************************************************
* Function Name  : TIM_Configuration
* Description    : Configures the TIMx.
*******************************************************************************/
 void TIM_Configuration()
{ 
	/* TIM1定时器配置 */
  u16 CCR1_Val = 1800;       // 设置TIM1通道1输出占空比:50%
  u16 CCR2_Val = 900;        // 设置TIM1通道2输出占空比:25%
  u16 CCR3_Val = 450;        // 设置TIM1通道3输出占空比:12.5%
	/*复位TIM1定时器*/
	TIM_DeInit(TIM1);  
	/* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 3600-1;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);
	
	/* Channel 1,2,3 and 4 Configuration in PWM mode */
	/* 选择定时器输出比较为PWM模式2。在向上计数时，当TIM1_CNT<TIM1_CCR1时，
	   通道1为无效电平，否则为有效电平；在向下计数时，当TIM1_CNT>TIM1_CCR1时，
	   通道1为有效电平，否则为无效电平。如果选择PWM模式1，则相反。 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	// 选择输出比较状态，以及互补输出比较状态
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;                  

// 设置通道1捕获比较寄存器的脉冲值——占空比为50%
  TIM_OCInitStructure.TIM_Pulse = CCR1_Val;

// 输出极性和互补输出极性的有效电平为低
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;

// 选择空闲状态下的非工作状态（MOE=0时，设置TIM1输出比较空闲状态）
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;          
  
  TIM_OC1Init(TIM1,&TIM_OCInitStructure); 

// 设置通道2捕获比较寄存器的脉冲值——占空比为25%
  TIM_OCInitStructure.TIM_Pulse = CCR2_Val;
  TIM_OC2Init(TIM1,&TIM_OCInitStructure);

// 设置通道3捕获比较寄存器的脉冲值——占空比为12.5%
  TIM_OCInitStructure.TIM_Pulse = CCR3_Val;
  TIM_OC3Init(TIM1,&TIM_OCInitStructure);

  /* Automatic Output enable, Break, dead time and lock configuration */
// 设置在运行模式下非工作状态
  TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
  TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;

// 写保护：锁定级别为1，不能写入TIM1_BDTR寄存器的DTG/BKE/BKP/AOE位、TIM1_CR2寄存器的OISx/OISxN位。系统复位后，只能写入一次LOCK位，其内容冻结直至复位。
  TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
  TIM_BDTRInitStructure.TIM_DeadTime = 0x75;  //互补输出的死区时间1.625us
  TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable;  // 刹车输入使能

  // 配置刹车（中止）输入信号特性：高电平有效，允许自动输出
  TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
  TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
  TIM_BDTRConfig(TIM1,&TIM_BDTRInitStructure);
	
	/* Clear TIM1 update pending flag，清除TIM1溢出中断标志 */
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);
	/* Enable TIM1 Update interrupt，TIM1溢出中断允许 */
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);
	/* TIM1 enable counter，启动TIM1计数 */
  TIM_Cmd(TIM1,ENABLE);
	/* TIM1 Main Output Enable，输出PWM */
  TIM_CtrlPWMOutputs(TIM1,ENABLE);

  /* TIM2定时器配置 */
	/*复位TIM2定时器*/
  TIM_DeInit(TIM2);         
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 35999;
  TIM_TimeBaseStructure.TIM_Prescaler = 1999;   //（1999+1）除以72MHz后为
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	/* Clear TIM2 update pending flag，清除TIM2溢出中断标志 */
  TIM_ClearFlag(TIM2, TIM_FLAG_Update);
  /* Enable TIM2 Update interrupt，TIM2溢出中断允许 */
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  /* TIM2 enable counter，启动TIM2计数 */
  TIM_Cmd(TIM2,ENABLE);	
	
}


/*******************************************************************************
* Function Name  : fputc
* Description    : Retargets the C library printf function to the USART.
*******************************************************************************/
int fputc(int ch, FILE *f)
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	USART_SendData(USART1, (unsigned char) ch);
	/* Loop until the end of transmission */
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC)!=SET) ;  // waiting here
	
	return ch;
}


/*******************************************************************************
* Function Name  : Open_Init
* Description    : Configures RCC,GPIO,NVIC
*******************************************************************************/

void Open_Init(void)
{
 	RCC_Configuration();
	GPIO_Configuration();
  USART_Configuration();
//	NVIC_Configuration();
//  TIM_Configuration();
}


/*******************************************************************************
* Function Name  : delay_nus
* Description    : delay n us
*******************************************************************************/
void delay_nus(unsigned long n)  //??n us: n>=6,??????6us
{ 
  unsigned long j;
  while(n--)              // ????:8M;PLL:9;8M*9=72MHz
  {
    j=8;				  // ????,???????
	while(j--);
  }
}

/*******************************************************************************
* Function Name  : delay_nms
* Description    : delay n ms
*******************************************************************************/
void delay_nms(unsigned long n)  //??n ms
{
  while(n--)		   // ????:8M;PLL:9;8M*9=72MHz
    delay_nus(1100);   // 1ms????
}

#endif
