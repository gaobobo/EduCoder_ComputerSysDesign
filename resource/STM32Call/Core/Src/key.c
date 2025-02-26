#include "key.h"

 
 
//void key_Config(void)			//配置 主控按键
//{
//	GPIO_InitTypeDef KEY_Pin;
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
//	
//	KEY_Pin.GPIO_Mode = GPIO_Mode_IN_FLOATING;		//浮空 输入
//	KEY_Pin.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
//	KEY_Pin.GPIO_Speed = GPIO_Speed_50MHz; 
//	GPIO_Init(GPIOA,&KEY_Pin);
//}

void key_Config(void)			//配置 主控按键 开启时钟
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
	
	
	 HAL_GPIO_WritePin(EXT_KEY_GND_GPIO_Port, EXT_KEY_GND_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
//  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin_1, GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin_2, GPIO_PIN_RESET);

  /*Configure GPIO pins : KEY1_Pin KEY2_Pin EXT_KEY1_Pin EXT_KEY2_Pin
                           EXT_KEY3_Pin */
  GPIO_InitStruct.Pin = KEY1_Pin|KEY2_Pin|EXT_KEY1_Pin|EXT_KEY2_Pin
                          |EXT_KEY3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : EXT_KEY_GND_Pin */
  GPIO_InitStruct.Pin = EXT_KEY_GND_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(EXT_KEY_GND_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : EXT_KEY4_Pin */
  GPIO_InitStruct.Pin = EXT_KEY4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(EXT_KEY4_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LED_Pin */
//  GPIO_InitStruct.Pin = LED_Pin_1|LED_Pin_2;
	  GPIO_InitStruct.Pin = LED_Pin_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);
	
	
}
 



//typedef struct {
//  uint32_t u32time1;            //第一次按下后开始计时，主要用于判断长按，放在定时器中自加
//  uint32_t u32time2;            //第一次松手后开始计时，用于判断双击或单击，放在定时器中自加
//  uint8_t u8key_flag;           //第一次按下标志  按下为1  松开为0
//  uint8_t u8key_double_flag;    //第二次标志
//}Key_Tag;

//Key_Tag skey;   // 短按 长按 双击标签 结构体 



//uint8_t KEY_Scan()    // 以一个键为例
//{
//    static uint8_t press = 0;
//        
////    if(KEY_1==0)//按键按下
//			if(KEY_1==GPIO_PIN_RESET)//按键按下
//    {
//        HAL_Delay(10);//消抖
////        if(KEY_1==0)
//			if(KEY_1==GPIO_PIN_RESET)
//        { 
//            if(skey.u8key_flag==0)
//            {
//                skey.u8key_flag=1;        //第一次按下，标志位置1。同时计数值归零
//                skey.u32time1=0;
//            }
//            else if(skey.u8key_flag==1)
//            {
//                if(!press && skey.u32time1 > 3000)//如果第一次按下且时间超过3S，视为长按
//                {
//                    press = 1;
//                    return LONG_PRES;//返回长按键值
//                }
//            }
//        }
//    }
//    
////    else if(KEY_1==1)//按键松开
//			else if(KEY_1==GPIO_PIN_SET)//按键松开
//    {
//        if(skey.u8key_flag==1)    //第一次按键松开
//        {
//            skey.u8key_flag=0;
//            if(skey.u32time1>3000)//按下后超过3秒才松开，已经返回了键值，松手后把标志位都归零
//            {
//                press = 0;
//                skey.u32time1 = 0;
//                skey.u32time2 = 0;
//                skey.u8key_flag=0;
//                skey.u8key_double_flag=0;
//            }
//            else if(skey.u8key_double_flag==0)
//            {
//                skey.u8key_double_flag=1;    //第一次松开之后标志位置1开始等待第二次按键松开
//                skey.u32time2=0;            
//            }
//            else if(skey.u8key_double_flag==1)
//            {
//                if(skey.u32time2<500)            //如果第二次松开时间间隔小于0.5S，视为双击
//                {
//                    skey.u8key_double_flag=0;
// 
//                    return DOUBLE_PRES;          //返回双击键值
//                }
//            }
//        }
//        else if(skey.u8key_double_flag==1)
//        {
//            if(skey.u32time2>=500)            
//            {
//                skey.u8key_double_flag=0;
// 
//                return KEY1_PRES;//如果第一次松开之后0.5s没有第二次按键操作，视为短按
//            }
//        }
//    }
//    return 0;//没有按键按下返回0
//}






 
/*
*@ Description: 按键 扫描
* @param 1 – mode：0（单击模式）/ 1（连按模式）
* @return – 返回值：按键 反馈数值
*/   
/*使用方法为 KEY_Scanf(0)  */


uint8_t KEY_Scanf(uint8_t mode)    
{	 
	
	static uint8_t key_up=1;//按键按松开标志
	if(mode)
	{
		key_up=1;  //支持连按		  
	}
//	if(key_up&&(KEY_1==0||KEY_2==0||KEY_3==0||KEY_4==0))
	if(key_up&&(KEY_1==GPIO_PIN_RESET||KEY_2==GPIO_PIN_RESET||KEY_3==GPIO_PIN_RESET||KEY_4==GPIO_PIN_RESET))
	{
//		delay_ms(10);//去抖动 
		HAL_Delay(10); //去抖动 
		key_up=0;  //设置按键松开
		if(KEY_1==0) return KEY1_PRES;
		else if(KEY_2==0) return KEY2_PRES;
		else if(KEY_3==0) return KEY3_PRES;
		else if(KEY_4==0) return KEY4_PRES;
	}
	else if(KEY_1==GPIO_PIN_SET&&KEY_2== GPIO_PIN_SET&&KEY_3== GPIO_PIN_SET&&KEY_4== GPIO_PIN_SET)
	{
		key_up=1; 	    
	}
 	return 0;// 无按键按下
}
 
 
void key_function(void)				//按键 功能
{
	static uint8_t key = 0;
	key = KEY_Scanf(0);
	switch(key){
		case 1:            //LED2：亮
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin_1, GPIO_PIN_SET);    // 这里reset 变set?  但是前面 set 和reset 没变 不然 KEY_Scanf() 函数输出全部为0 无按键按下
			HAL_Delay(1000);
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin_1, GPIO_PIN_RESET);
			break;
		case 2:            //LED2：灭
			 HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin_1, GPIO_PIN_SET);    // 这里reset 变set?  但是前面 set 和reset 没变 不然 KEY_Scanf() 函数输出全部为0 无按键按下
			HAL_Delay(1000);
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin_1, GPIO_PIN_RESET);
			break;        
		case 3:            //LED2：当前状态取反
//			 HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin_1|LED_Pin_2, GPIO_PIN_SET);
//				 HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin_1, GPIO_PIN_SET);
//				 printf("key1\r\n");
			break;
		case 4:            //LED2：当前状态取反
//			 HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin_1|LED_Pin_2, GPIO_PIN_RESET);
			break;
	}
}