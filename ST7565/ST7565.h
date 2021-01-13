/*

  ******************************************************************************
  * @file 			( фаил ):   ST7565.h
  * @brief 		( описание ):  	
  ******************************************************************************
  * @attention 	( внимание ):
  ******************************************************************************
  
*/

#ifndef _ST7565_H
#define _ST7565_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ----------------------------------------------------------*/

// Обязательно нужен #include "main.h" 
// чтоб отдельно не подключать файлы связанные с МК и стандартными библиотеками
#include "main.h"

#include "fonts.h"

#include "stdlib.h"
#include "string.h"

/*
	подключение LCD_128_64_ST7565 ( GMG12864-06D ):

	в CubeMX включаем SPI только на отправку скорость максимум
	( пины желательно называем как в коде если нет то в коде ставим нужнве )

	CS   ->   CS ( chip select )
	RSE  ->   RES ( Reset )
	RS   ->   DC ( date / command )
	SCL	 ->   SCL
	SI	 ->   MOSI
	VDD  ->   +3.3V
	VSS	 ->   GND
	A	 ->   anode led ( +3.3V or PWM )
	K	 ->   katode led ( GND )

*/

//##########  SETUP  ######################################################################################################################################

		//==== выбераем через что будем отправлять через HAL или CMSIS(быстрее) ==================
		//-- нужное оставляем другое коментируем ( важно должно быть только один выбран )---------
		
			// указываем порт SPI для CMSIS ( быстро )-------
			// так как у разных МК разные регистры то в функциях корректируем под свой МК
			// на данный момент есть реализация на серию F1 F4 H7 для выбора серии в функциях
			//	void ST7565_SendCmd(uint8_t Cmd);
			//	void ST7565_SendData(uint8_t Data );
			// комментируем и раскомментируем то что нам нужно, также там же редактируем под свой МК если не работает
			#define 	ST7565_SPI_CMSIS 	SPI1
			//-----------------------------------------------
			
			// указываем порт SPI для HAL ( медлено )--------
			//#define 	ST7565_SPI_HAL 		hspi1
			//-----------------------------------------------
			
		//============================================================================
		
		//=== указываем порты ( если в кубе назвали их DC RES CS то тогда нечего указывать не нужно )
		#if defined (DC_GPIO_Port)
		#else
			#define DC_GPIO_Port		GPIOA
			#define DC_Pin				GPIO_PIN_11
		#endif
		
		#if defined (RES_GPIO_Port)
		#else
			#define RES_GPIO_Port   	GPIOA
			#define RES_Pin			GPIO_PIN_12
		#endif
		
		//--  Cесли используем порт CS для выбора устройства тогда раскомментировать ------------
		// если у нас одно устройство лучше пин CS притянуть к земле( или на порту подать GND )
		
		#define CS_PORT
		
		//----------------------------------------------------------------------------------------
		#ifdef CS_PORT
			#if defined (CS_GPIO_Port)
			#else
				#define CS_GPIO_Port    GPIOA
				#define CS_Pin			GPIO_PIN_14
			#endif
		#endif
		
		//=============================================================================
		
		// выбираем ориентацию экрана ( если нужно повернуть на 180 градусов )
		// если не нужно то комментируем и получим 0 градусов
		//#define SCREEN_ORIENTATION_180
		
//##########################################################################################################################

enum PixelStatus{ PIX_OFF, PIX_ON };

#ifdef ST7565_SPI_HAL
	extern SPI_HandleTypeDef ST7565_SPI_HAL;
#endif


#if defined (SCREEN_ORIENTATION_180)
	#define ST7565_SetX(x) 		ST7565_w_cmd(((x+4) & 0xf0) >> 4 | 0x10); ST7565_w_cmd((x+4) & 0x0f)
#else
	#define ST7565_SetX(x) 		ST7565_w_cmd(((x) & 0xf0) >> 4 | 0x10); ST7565_w_cmd((x) & 0x0f)
#endif

#define ST7565_SetY(y) 		ST7565_w_cmd( ( (y) & 0x07 ) | 0xB0 )



/** Screen width in pixels (tested with 128) */
#define SCREEN_WIDTH 	128
/** Screen height in pixels (tested with 64) */
#define SCREEN_HEIGHT 	64


/* Absolute value */
#define ABS(x)   ((x) > 0 ? (x) : -(x))


#define LCD_DATA 					0x00

#define CMD_DISPLAY_OFF   			0xAE
#define CMD_DISPLAY_ON    			0xAF

#define CMD_SET_DISP_START_LINE  	0x40
#define CMD_SET_PAGE  				0xB0

#define CMD_SET_COLUMN_UPPER  		0x10
#define CMD_SET_COLUMN_LOWER  		0x00

#define CMD_SET_ADC_NORMAL  		0xA0
#define CMD_SET_ADC_REVERSE 		0xA1

#define CMD_SET_DISP_NORMAL 		0xA6
#define CMD_SET_DISP_REVERSE 		0xA7

#define CMD_SET_ALLPTS_NORMAL 		0xA4
#define CMD_SET_ALLPTS_ON  			0xA5
#define CMD_SET_BIAS_9 				0xA2
#define CMD_SET_BIAS_7 				0xA3

#define CMD_RMW  					0xE0
#define CMD_RMW_CLEAR 				0xEE
#define CMD_INTERNAL_RESET  		0xE2
#define CMD_SET_COM_NORMAL  		0xC0
#define CMD_SET_COM_REVERSE  		0xC8
#define CMD_SET_POWER_CONTROL  		0x28
#define CMD_SET_RESISTOR_RATIO  	0x20
#define CMD_SET_VOLUME_FIRST  		0x81
#define CMD_SET_VOLUME_SECOND  		0
#define CMD_SET_STATIC_OFF  		0xAC
#define CMD_SET_STATIC_ON  			0xAD
#define CMD_SET_STATIC_REG  		0x0
#define CMD_SET_BOOSTER_FIRST  		0xF8
#define CMD_SET_BOOSTER_234  		0
#define CMD_SET_BOOSTER_5  			1
#define CMD_SET_BOOSTER_6  			3
#define CMD_NOP  					0xE3
#define CMD_TEST  					0xF0

	

/* Functions prototypes ----------------------------------------------*/


/*
	******************************************************************************
	* @brief	 ( описание ):  включение CS
	* @param	( параметры ):	
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7565_Select(void);
//--------------------------------------------------------------------------------


/*
	******************************************************************************
	* @brief	 ( описание ):  выключение CS
	* @param	( параметры ):	
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7565_Unselect(void);
//--------------------------------------------------------------------------------


/*
	******************************************************************************
	* @brief	 ( описание ):  аппарвтный сброс дисплея
	* @param	( параметры ):	
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7565_Reset( void );
//--------------------------------------------------------------------------------



/*
	******************************************************************************
	* @brief	 ( описание ):  отправка данных в SPI
	* @param	( параметры ):	1 байт информации
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7565_w_dat( uint8_t Dat );  
//--------------------------------------------------------------------------------

  

/*
	******************************************************************************
	* @brief	 ( описание ):  отправка команды в SPI
	* @param	( параметры ):	1 байт информации
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7565_w_cmd( uint8_t Command ); 
//--------------------------------------------------------------------------------



/*
	******************************************************************************
	* @brief	 ( описание ):  зарисовка всего дисплея указаными битами
	* @param	( параметры ):	1 байт информации, при 0х00 дисплей закраситься белым, при 0хFF черным, 
								при 0хF0 будут линии ( значение можно менять будет разное отображение )
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7565_Display_fill( uint8_t fill );
//--------------------------------------------------------------------------------


/*
	******************************************************************************
	* @brief	 ( описание ):  инициализация дисплея ( при другой инициализации можно поменять параметры внутри функции )
	* @param	( параметры ):	
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7565_Init( void );
//--------------------------------------------------------------------------------


/*
	******************************************************************************
	* @brief	 ( описание ):  очистка дисплея ( заполняет весь дисплей белым 0x00)
	* @param	( параметры ):	
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7565_Clear(void);
//--------------------------------------------------------------------------------


/*
	******************************************************************************
	* @brief	 ( описание ):  рисуем пиксель
	* @param	( параметры ):	координата X(1 - 128) и координата Y(1 - 64) color 1 or 0
	* @return  ( возвращает ):	

	******************************************************************************
*/
// X(1 - 128)  Y(1 - 64)
void ST7565_Draw_pixel(int16_t x, int16_t y, uint8_t color);
//--------------------------------------------------------------------------------


/*
	******************************************************************************
	* @brief	 ( описание ):  инвертирование дисплея
	* @param	( параметры ):	1 - инвертировать, 0- не инвертировать
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7565_InvertDisplay (uint8_t i);
//--------------------------------------------------------------------------------


/*
	******************************************************************************
	* @brief	 ( описание ):  включение и отключение дисплея ( по умолчанию включен )
	* @param	( параметры ):	1 - включить, 0- выключить
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7565_DisplayOnOff (uint8_t i);
//--------------------------------------------------------------------------------
/*
	******************************************************************************
	* @brief	 ( описание ):  функция рисует монохромныую картинку
	* @param	( параметры ):	начальные координаты X Y ширина и высота картинки и цвет
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7565_DrawBitmap(int16_t x, int16_t y, const unsigned char* bitmap, int16_t w, int16_t h, uint8_t color);
//----------------------------------------------------------------------------------
/*
	******************************************************************************
	* @brief	 ( описание ):  функция выводит 1 символ
	* @param	( параметры ):	координаты X Y, символ, шрифт, множитель, цвет 1 или 0
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7565_DrawChar(int16_t x, int16_t y, unsigned char ch, FontDef_t* Font, uint8_t multiplier, uint8_t color);
//----------------------------------------------------------------------------------
/*
	******************************************************************************
	* @brief	 ( описание ):  функция пишет строку
	* @param	( параметры ):	координаты X Y, символ, шрифт, множитель, цвет 1 или 0
	* @return  ( возвращает ):	
	******************************************************************************
*/
void ST7565_Print(int16_t x, int16_t y, char* str, FontDef_t* Font, uint8_t multiplier, uint8_t color);
//----------------------------------------------------------------------------------
/*
	******************************************************************************
	* @brief	 ( описание ):  рисуем линию
	* @param	( параметры ):	координаты начальной точки и конечной и цвет 0 или 1
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7565_DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t c);
//----------------------------------------------------------------------------------
/*
	******************************************************************************
	* @brief	 ( описание ):  рисуем прямоугольник пустотелый
	* @param	( параметры ):	координаты верхнего левого и ширина и высота и цвет
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7565_DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t c);
//----------------------------------------------------------------------------------
/*
	******************************************************************************
	* @brief	 ( описание ):  рисуем прямоугольник закрашеный
	* @param	( параметры ):	координаты верхнего левого и ширина и высота и цвет
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7565_DrawFilledRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t c);	
//----------------------------------------------------------------------------------
/*
	******************************************************************************
	* @brief	 ( описание ):  рисуем триугольник пустотелый
	* @param	( параметры ):	координаты вершин и цвет
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7565_DrawTriangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint8_t color);
//----------------------------------------------------------------------------------
/*
	******************************************************************************
	* @brief	 ( описание ):  рисуем триугольник заполненый
	* @param	( параметры ):	координаты вершин и цвет
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7565_DrawFilledTriangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint8_t color);
//----------------------------------------------------------------------------------
/*
	******************************************************************************
	* @brief	 ( описание ):  рисуем круг пустотелый
	* @param	( параметры ):	координаты центра круга, радиус и цвет
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7565_DrawCircle(int16_t x0, int16_t y0, int16_t r, uint8_t c);
//----------------------------------------------------------------------------------
/*
	******************************************************************************
	* @brief	 ( описание ):  рисуем круг заполненый
	* @param	( параметры ):	координаты центра круга, радиус и цвет
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7565_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, uint8_t c);
//----------------------------------------------------------------------------------


	
	












#ifdef __cplusplus
}
#endif

#endif	/*	_ST7565_H */

/************************ (C) COPYRIGHT GKP *****END OF FILE****/
