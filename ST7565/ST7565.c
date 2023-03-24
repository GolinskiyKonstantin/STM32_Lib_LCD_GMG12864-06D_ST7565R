/*

  ******************************************************************************
  * @file 			( фаил ):   ST7565.c
  * @brief 		( описание ):  	
  ******************************************************************************
  * @attention 	( внимание ):	author: Golinskiy Konstantin	e-mail: golinskiy.konstantin@gmail.com
  ******************************************************************************
  
*/

/* Includes ----------------------------------------------------------*/
#include "ST7565.h"


uint8_t ST7565_buffer[SCREEN_WIDTH * SCREEN_HEIGHT / 8];


//==============================================================================
// Процедура управления SPI
//==============================================================================
inline static void ST7565_Select(void) {
	
    #ifdef CS_PORT
			//-- если захотим переделать под HAL ------------------	
			#ifdef ST7565_SPI_HAL
				HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
			#endif
			//-----------------------------------------------------
			
			//-- если захотим переделать под CMSIS  ---------------
			#ifdef ST7565_SPI_CMSIS
				CS_GPIO_Port->BSRR = ( CS_Pin << 16 );
			#endif
			//-----------------------------------------------------
	#endif
	
}
//==============================================================================


//==============================================================================
// Процедура управления SPI
//==============================================================================
inline static void ST7565_Unselect(void) {
	
    #ifdef CS_PORT
			//-- если захотим переделать под HAL ------------------	
			#ifdef ST7565_SPI_HAL
				HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
			#endif
			//-----------------------------------------------------
			
			//-- если захотим переделать под CMSIS  ---------------
			#ifdef ST7565_SPI_CMSIS
					 CS_GPIO_Port->BSRR = CS_Pin;
			#endif
			//-----------------------------------------------------
	#endif
	
}
//==============================================================================


/*
	******************************************************************************
	* @brief	 ( описание ):  аппарвтный сброс дисплея
	* @param	( параметры ):	
	* @return  ( возвращает ):	

	******************************************************************************
*/
static void ST7565_Reset( void ){  
	
 HAL_GPIO_WritePin( RES_GPIO_Port, RES_Pin, GPIO_PIN_RESET );
 HAL_Delay( 10 ); 
	
 HAL_GPIO_WritePin( RES_GPIO_Port, RES_Pin, GPIO_PIN_SET );
 HAL_Delay( 10 );  
}
//--------------------------------------------------------------------------------



/*
	******************************************************************************
	* @brief	 ( описание ):  отправка данных в SPI
	* @param	( параметры ):	1 байт информации
	* @return  ( возвращает ):	

	******************************************************************************
*/
inline static void ST7565_w_dat( uint8_t Dat ){  
	
	//-- если захотим переделать под HAL ------------------	
	#ifdef ST7565_SPI_HAL
	
		 HAL_SPI_Transmit(&ST7565_SPI_HAL, &Dat, 1, HAL_MAX_DELAY);
		 while(HAL_SPI_GetState(&ST7565_SPI_HAL) != HAL_SPI_STATE_READY){};
		 
	#endif
	//-----------------------------------------------------
	
	//-- если захотим переделать под CMSIS  ---------------------------------------------
	#ifdef ST7565_SPI_CMSIS
		
		//======  FOR F-SERIES ===========================================================
			
			// Disable SPI	
			//CLEAR_BIT(ST7565_SPI_CMSIS->CR1, SPI_CR1_SPE);	// ST7565_SPI_CMSIS->CR1 &= ~SPI_CR1_SPE;
			// Enable SPI
			if((ST7565_SPI_CMSIS->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE){
				// If disabled, I enable it
				SET_BIT(ST7565_SPI_CMSIS->CR1, SPI_CR1_SPE);	// ST7565_SPI_CMSIS->CR1 |= SPI_CR1_SPE;
			}
			
			// Ждем, пока не освободится буфер передатчика
			// TXE(Transmit buffer empty) – устанавливается когда буфер передачи(регистр SPI_DR) пуст, очищается при загрузке данных
			while( (ST7565_SPI_CMSIS->SR & SPI_SR_TXE) == RESET ){};
		
			// передаем 1 байт информации--------------
			*((__IO uint8_t *)&ST7565_SPI_CMSIS->DR) = Dat;
			
			// TXE(Transmit buffer empty) – устанавливается когда буфер передачи(регистр SPI_DR) пуст, очищается при загрузке данных
			while( (ST7565_SPI_CMSIS->SR & (SPI_SR_TXE | SPI_SR_BSY)) != SPI_SR_TXE ){};
			
			// Ждем, пока не освободится буфер передатчика
			// while((ST7565_SPI_CMSIS->SR&SPI_SR_BSY)){};
			
			// Disable SPI	
			//CLEAR_BIT(ST7565_SPI_CMSIS->CR1, SPI_CR1_SPE);
			
		//================================================================================
		
/*		//======  FOR H-SERIES ===========================================================

			// Disable SPI	
			//CLEAR_BIT(ST7565_SPI_CMSIS->CR1, SPI_CR1_SPE);	// ST7565_SPI_CMSIS->CR1 &= ~SPI_CR1_SPE;
			// Enable SPI
			if((ST7565_SPI_CMSIS->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE){
				// If disabled, I enable it
				SET_BIT(ST7565_SPI_CMSIS->CR1, SPI_CR1_SPE);	// ST7565_SPI_CMSIS->CR1 |= SPI_CR1_SPE;
			}
			
			SET_BIT(ST7565_SPI_CMSIS->CR1, SPI_CR1_CSTART);	// ST7565_SPI_CMSIS->CR1 |= SPI_CR1_CSTART;
			
			// ждем пока SPI будет свободна------------
			//while (!(ST7565_SPI_CMSIS->SR & SPI_SR_TXP)){};		
		
			// передаем 1 байт информации--------------
			*((__IO uint8_t *)&ST7565_SPI_CMSIS->TXDR )  = Dat;
				
			// Ждать завершения передачи---------------
			while (!( ST7565_SPI_CMSIS -> SR & SPI_SR_TXC )){};
			
			// Disable SPI	
			//CLEAR_BIT(ST7565_SPI_CMSIS->CR1, SPI_CR1_SPE);
			
*/		//================================================================================
		
	#endif
	//-----------------------------------------------------------------------------------
	
}  
//--------------------------------------------------------------------------------

  

/*
	******************************************************************************
	* @brief	 ( описание ):  отправка команды в SPI
	* @param	( параметры ):	1 байт информации
	* @return  ( возвращает ):	

	******************************************************************************
*/
inline static void ST7565_w_cmd( uint8_t Command ){  
	
	//-- если захотим переделать под HAL ------------------	
	#ifdef ST7565_SPI_HAL
		
		// pin DC LOW
		 HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_RESET);
		 
		 HAL_SPI_Transmit(&ST7565_SPI_HAL, &Command, 1, HAL_MAX_DELAY);
		 while(HAL_SPI_GetState(&ST7565_SPI_HAL) != HAL_SPI_STATE_READY){};
		 
		 // pin DC HIGH
		 HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);
		 
	#endif
	//-----------------------------------------------------
	
	//-- если захотим переделать под CMSIS  ---------------------------------------------
	#ifdef ST7565_SPI_CMSIS
		
		// pin DC LOW
		DC_GPIO_Port->BSRR = ( DC_Pin << 16 );
		
		//======  FOR F-SERIES ===========================================================
			
			// Disable SPI	
			//CLEAR_BIT(ST7565_SPI_CMSIS->CR1, SPI_CR1_SPE);	// ST7565_SPI_CMSIS->CR1 &= ~SPI_CR1_SPE;
			// Enable SPI
			if((ST7565_SPI_CMSIS->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE){
				// If disabled, I enable it
				SET_BIT(ST7565_SPI_CMSIS->CR1, SPI_CR1_SPE);	// ST7565_SPI_CMSIS->CR1 |= SPI_CR1_SPE;
			}
			
			// Ждем, пока не освободится буфер передатчика
			// TXE(Transmit buffer empty) – устанавливается когда буфер передачи(регистр SPI_DR) пуст, очищается при загрузке данных
			while( (ST7565_SPI_CMSIS->SR & SPI_SR_TXE) == RESET ){};
		
			// передаем 1 байт информации--------------
			*((__IO uint8_t *)&ST7565_SPI_CMSIS->DR) = Command;
			
			// TXE(Transmit buffer empty) – устанавливается когда буфер передачи(регистр SPI_DR) пуст, очищается при загрузке данных
			while( (ST7565_SPI_CMSIS->SR & (SPI_SR_TXE | SPI_SR_BSY)) != SPI_SR_TXE ){};
			
			// Ждем, пока не освободится буфер передатчика
			// while((ST7565_SPI_CMSIS->SR&SPI_SR_BSY)){};
			
			// Disable SPI	
			//CLEAR_BIT(ST7565_SPI_CMSIS->CR1, SPI_CR1_SPE);
			
		//================================================================================
		
/*		//======  FOR H-SERIES ===========================================================

			// Disable SPI	
			//CLEAR_BIT(ST7565_SPI_CMSIS->CR1, SPI_CR1_SPE);	// ST7565_SPI_CMSIS->CR1 &= ~SPI_CR1_SPE;
			// Enable SPI
			if((ST7565_SPI_CMSIS->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE){
				// If disabled, I enable it
				SET_BIT(ST7565_SPI_CMSIS->CR1, SPI_CR1_SPE);	// ST7565_SPI_CMSIS->CR1 |= SPI_CR1_SPE;
			}
			
			SET_BIT(ST7565_SPI_CMSIS->CR1, SPI_CR1_CSTART);	// ST7565_SPI_CMSIS->CR1 |= SPI_CR1_CSTART;
			
			// ждем пока SPI будет свободна------------
			//while (!(ST7565_SPI_CMSIS->SR & SPI_SR_TXP)){};		
		
			// передаем 1 байт информации--------------
			*((__IO uint8_t *)&ST7565_SPI_CMSIS->TXDR )  = Command;
				
			// Ждать завершения передачи---------------
			while (!( ST7565_SPI_CMSIS -> SR & SPI_SR_TXC )){};
			
			// Disable SPI	
			//CLEAR_BIT(ST7565_SPI_CMSIS->CR1, SPI_CR1_SPE);
			
*/		//================================================================================
		
		// pin DC HIGH
		DC_GPIO_Port->BSRR = DC_Pin;
		
	#endif
	//-----------------------------------------------------------------------------------
	
} 
//--------------------------------------------------------------------------------



/*
	******************************************************************************
	* @brief	 ( описание ):  зарисовка всего дисплея указаными битами
	* @param	( параметры ):	1 байт информации, при 0х00 дисплей закраситься белым, при 0хFF черным, 
								при 0хF0 будут линии ( значение можно менять будет разное отображение )
								( данные отправляются сразу на дисплей и записываются в буфер кадра )
								Функцию ST7565_Update() вызывать не нужно
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7565_Display_fill( uint8_t fill ){  
	
	uint8_t page,column;  
	
	memset( ST7565_buffer, (uint8_t)fill, ( SCREEN_WIDTH * SCREEN_HEIGHT / 8 ) );	// отчистка памяти заполняем массив значением 0x00
	
	ST7565_Select();
	
	for( page=0xB7; page>=0xB0; page-- ){
		
		ST7565_w_cmd(page);  //set page address   
		ST7565_w_cmd(0x10);  //set Column address MSB   
		ST7565_w_cmd(0x00);  //set column address LSB   
		for( column=0; column<131; column++ ){  
			ST7565_w_dat(fill);  
        }  
    }
		
	ST7565_Unselect();
}
//--------------------------------------------------------------------------------


/*
	******************************************************************************
	* @brief	 ( описание ):  инициализация дисплея ( при другой инициализации можно поменять параметры внутри функции )
	* @param	( параметры ):	
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7565_Init( void ){  
	
	ST7565_Select();
	
	ST7565_Reset();

	// Установите рабочий цикл ( 1/7 или 1/9 ) в зависимости от физического ЖК-дисплея	
	ST7565_w_cmd( CMD_SET_BIAS_9 );    // CMD_SET_BIAS_7   1/7    CMD_SET_BIAS_9  1/9
	
	HAL_Delay(1);
	
	//---------------------------------------------------------------------------------------------
	// Установите горизонтальную и вертикальную ориентацию в известное состояние
	#if defined (SCREEN_ORIENTATION_180)
		ST7565_w_cmd( CMD_SET_ADC_REVERSE );    	//ADC selection(SEG0->SEG128)  CMD_SET_ADC_NORMAL горизонтально «нормальная» (не перевернутая)
		ST7565_w_cmd( CMD_SET_COM_NORMAL );    		//SHL selection(COM0->COM64)   CMD_SET_COM_REVERSE вертикально «перевернутая» (дополняет команду выше)
	#else
		ST7565_w_cmd( CMD_SET_ADC_NORMAL );    		//ADC selection(SEG0->SEG128)  CMD_SET_ADC_NORMAL горизонтально «нормальная» (не перевернутая)
		ST7565_w_cmd( CMD_SET_COM_REVERSE );    	//SHL selection(COM0->COM64)   CMD_SET_COM_REVERSE вертикально «перевернутая» (дополняет команду выше)
	#endif		
	//---------------------------------------------------------------------------------------------	
	
	HAL_Delay(1);
	
	// делитель внутреннего резистора установлен на 7 (от 0..7)	
	ST7565_w_cmd( CMD_SET_RESISTOR_RATIO | 0x7 );    //Regulator Resistor Selection   
	
	HAL_Delay(1);
	
	// управление питанием, все внутренние блоки включены	(от 0..7)
	ST7565_w_cmd( CMD_SET_POWER_CONTROL | 0x7 );
		
	HAL_Delay(1);

	// войти в режим динамического контраста	
	ST7565_w_cmd( CMD_SET_VOLUME_FIRST );    //Electronic Volume   
	ST7565_w_cmd( 19 );	// устанавливает контраст от 0..63.
	
	HAL_Delay(1);
	
	ST7565_w_cmd( CMD_SET_DISP_START_LINE );
 
	HAL_Delay(1);

	// CMD_DISPLAY_ON  CMD_DISPLAY_OFF
	ST7565_w_cmd( CMD_DISPLAY_ON );    		//Display on   
 
	// Инвертирование экрана CMD_SET_DISP_NORMAL - OFF    CMD_SET_DISP_REVERSE - ON
	ST7565_w_cmd( CMD_SET_DISP_NORMAL );
	
	HAL_Delay(1);
	
	ST7565_Display_fill( 0x00 );
	
	ST7565_Unselect();
   
}
//--------------------------------------------------------------------------------



/*
	******************************************************************************
	* @brief	 ( описание ):  очистка только буфера кадра, дисплей не отчищает
	* @param	( параметры ):	
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7565_Clear_buffer(void){
	
  memset( ST7565_buffer, 0, ( SCREEN_WIDTH * SCREEN_HEIGHT / 8 ) );	// отчистка памяти заполняем массив значением 0x00
}
//--------------------------------------------------------------------------------



/*
	******************************************************************************
	* @brief	 ( описание ):  очистка дисплея и буфер кадра( заполняет весь дисплей белым 0x00)
	* @param	( параметры ):	
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7565_Clear(void){
	
  ST7565_Clear_buffer();
	
  uint8_t i, j;
		
  ST7565_Select();
	
  for(i = 0; i < 8; i++) {

     ST7565_w_cmd(0xB0 | i); 			//sets page(row) from 0 to 7
	 ST7565_w_cmd(0x10);				//sets column address(MSB) to 0 for each page(row)
     ST7565_w_cmd(0x00);				//sets Column address(LSB)to 0 for each page(row)

    for(j = 0; j < 131; j++) {
		ST7565_w_dat( 0x00 );
    }

  }
  
  ST7565_Unselect();
}
//--------------------------------------------------------------------------------



/*
	******************************************************************************
	* @brief	 ( описание ):  записываем пиксель в буфер кадра
	* @param	( параметры ):	координата X(0 - 127) и координата Y(0 - 63)  color 1 or 0
	* @return  ( возвращает ):	

	******************************************************************************
*/
// X(0 - 127)  Y(0 - 63)
void ST7565_Draw_pixel(int16_t x, int16_t y, uint8_t color){
	
    if (x >= SCREEN_WIDTH || x < 0 || y >= SCREEN_HEIGHT || y < 0) return;

    uint16_t array_pos = x + ((y / 8) * SCREEN_WIDTH );


    if ( color ) {
        ST7565_buffer[array_pos] |= 1 << (y % 8);
    } 
	else {
        ST7565_buffer[array_pos] &= 0xFF ^ 1 << (y % 8);
    }
}
//--------------------------------------------------------------------------------



/*
	******************************************************************************
	* @brief	 ( описание ):  вывод буффера кадра на дисплей
	* @param	( параметры ):	
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7565_Update (void){
	
	uint8_t x, y;
		
	ST7565_Select();

  	for(y = 0; y < 8; y++) {

		ST7565_SetX(0);	
		ST7565_SetY(y);	

		for(x = 0; x < 128; x++) {
			ST7565_w_dat( ST7565_buffer[x + 128 * y] );
		}
	}
	
	ST7565_Unselect();
}
//--------------------------------------------------------------------------------


/*
	******************************************************************************
	* @brief	 ( описание ):  инвертирование дисплея
	* @param	( параметры ):	1 - инвертировать, 0- не инвертировать
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7565_InvertDisplay (uint8_t i){

	ST7565_Select();
	
	// Инвертирование экрана CMD_SET_DISP_NORMAL - OFF    CMD_SET_DISP_REVERSE - ON
	if( i ){
		ST7565_w_cmd( CMD_SET_DISP_REVERSE );
	}
	else{
		ST7565_w_cmd( CMD_SET_DISP_NORMAL );
	}
	
	ST7565_Unselect();
}
//--------------------------------------------------------------------------------


/*
	******************************************************************************
	* @brief	 ( описание ):  включение и отключение дисплея ( по умолчанию включен )
	* @param	( параметры ):	1 - включить, 0- выключить
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7565_DisplayOnOff (uint8_t i){

	ST7565_Select();
	
	if( i ){
		ST7565_w_cmd( CMD_DISPLAY_ON );
	}
	else{
		ST7565_w_cmd( CMD_DISPLAY_OFF );
	}
	
	ST7565_Unselect();
}
//--------------------------------------------------------------------------------


/*
	******************************************************************************
	* @brief	 ( описание ):  функция записывает монохромныую картинку в буфер кадра
	* @param	( параметры ):	начальные координаты X(0 - 127)  Y(0 - 63) ширина и высота картинки и цвет
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7565_DrawBitmap(int16_t x, int16_t y, const unsigned char* bitmap, int16_t w, int16_t h, uint8_t color){

    int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
    uint8_t byte = 0;

    for(int16_t j=0; j<h; j++, y++){
		
        for(int16_t i=0; i<w; i++){
			
            if(i & 7){
				
               byte <<= 1;
            }
            else{
				
               byte = (*(const unsigned char *)(&bitmap[j * byteWidth + i / 8]));
            }
			
            if(byte & 0x80){ ST7565_Draw_pixel(x+i, y, color); }
        }
    }	
}
//----------------------------------------------------------------------------------


/*
	******************************************************************************
	* @brief	 ( описание ):  функция записывает 1 символ в буфер кадра
	* @param	( параметры ):	координаты X(0 - 127)  Y(0 - 63), символ, шрифт, множитель, цвет 1 или 0
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7565_DrawChar(int16_t x, int16_t y, unsigned char ch, FontDef_t* Font, uint8_t multiplier, uint8_t color) {
	
	uint16_t i, j;
	
	uint16_t b;
	
	int16_t X = x, Y = y;
	
	int16_t xx, yy;
	
	if( multiplier < 1 ){
		multiplier = 1;
	}
	
	/* Check available space in LCD */
	if ( SCREEN_WIDTH >= ( x + Font->FontWidth) || SCREEN_HEIGHT >= ( y + Font->FontHeight)){
	
		/* Go through font */
		for (i = 0; i < Font->FontHeight; i++) {
			
			if( ch < 127 ){			
				b = Font->data[(ch - 32) * Font->FontHeight + i];
			}
			
			else if( (uint8_t) ch > 191 ){
				// +96 это так как латинские символы и знаки в шрифтах занимают 96 позиций
				// и если в шрифте который содержит сперва латиницу и спец символы и потом 
				// только кирилицу то нужно добавлять 95 если шрифт 
				// содержит только кирилицу то +96 не нужно
				b = Font->data[((ch - 192) + 96) * Font->FontHeight + i];
			}
			
			else if( (uint8_t) ch == 168 ){	// 168 символ по ASCII - Ё
				// 160 эллемент ( символ Ё ) 
				b = Font->data[( 160 ) * Font->FontHeight + i];
			}
			
			else if( (uint8_t) ch == 184 ){	// 184 символ по ASCII - ё
				// 161 эллемент  ( символ ё ) 
				b = Font->data[( 161 ) * Font->FontHeight + i];
			}
			//-------------------------------------------------------------------------------
			//----  Украинская раскладка ----------------------------------------------------
			else if( (uint8_t) ch == 170 ){	// 168 символ по ASCII - Є
				// 162 эллемент ( символ Є )
				b = Font->data[( 162 ) * Font->FontHeight + i];
			}
			else if( (uint8_t) ch == 175 ){	// 184 символ по ASCII - Ї
				// 163 эллемент  ( символ Ї )
				b = Font->data[( 163 ) * Font->FontHeight + i];
			}
			else if( (uint8_t) ch == 178 ){	// 168 символ по ASCII - І
				// 164 эллемент ( символ І )
				b = Font->data[( 164 ) * Font->FontHeight + i];
			}
			else if( (uint8_t) ch == 179 ){	// 184 символ по ASCII - і
				// 165 эллемент  ( символ і )
				b = Font->data[( 165 ) * Font->FontHeight + i];
			}
			else if( (uint8_t) ch == 186 ){	// 184 символ по ASCII - є
				// 166 эллемент  ( символ є )
				b = Font->data[( 166 ) * Font->FontHeight + i];
			}
			else if( (uint8_t) ch == 191 ){	// 168 символ по ASCII - ї
				// 167 эллемент ( символ ї )
				b = Font->data[( 167 ) * Font->FontHeight + i];
			}
			//-----------------------------------------------------------------------------
			
			for (j = 0; j < Font->FontWidth; j++) {
				
				if ((b << j) & 0x8000) {
					
					for (yy = 0; yy < multiplier; yy++){
						for (xx = 0; xx < multiplier; xx++){
								ST7565_Draw_pixel(X+xx, Y+yy, color);
						}
					}
					
				}
				// если фон очищать то оставляем если чтоб фон оставался старый то коментируем эту часть --------------------------------------------
				//-----------------------------------------------------------------------------------------------------------------------------------
				else{
					
					for (yy = 0; yy < multiplier; yy++){
						for (xx = 0; xx < multiplier; xx++){
								ST7565_Draw_pixel(X+xx, Y+yy, !color);
						}
					}
				}
				//-----------------------------------------------------------------------------------------------------------------------------------
				//-----------------------------------------------------------------------------------------------------------------------------------
				
				X = X + multiplier;
			}
			
			X = x;
			Y = Y + multiplier;
		}
		
	}
}
//----------------------------------------------------------------------------------


/*
	******************************************************************************
	* @brief	 ( описание ):  функция записывает строку в буфер кадра ( поддерживает кириллицу ( в том числе и Укр. язык) и латиницу
	* @param	( параметры ):	координаты X(0 - 127)  Y(0 - 63), символ, шрифт, множитель, цвет 1 или 0
	* @return  ( возвращает ):	
	******************************************************************************
*/
void ST7565_Print(int16_t x, int16_t y, char* str, FontDef_t* Font, uint8_t multiplier, uint8_t color) {
	
	if( multiplier < 1 ){
		multiplier = 1;
	}
	
	unsigned char buff_char;
	
	uint16_t len = strlen(str);
	
	while (len--) {
		
		//---------------------------------------------------------------------
		// проверка на кириллицу UTF-8, если латиница то пропускаем if
		// Расширенные символы ASCII Win-1251 кириллица (код символа 128-255)
		// проверяем первый байт из двух ( так как UTF-8 ето два байта )
		// если он больше либо равен 0xC0 ( первый байт в кириллеце будет равен 0xD0 либо 0xD1 именно в алфавите )
		if ( (uint8_t)*str >= 0xC0 ){	// код 0xC0 соответствует символу кириллица 'A' по ASCII Win-1251
			
			// проверяем какой именно байт первый 0xD0 либо 0xD1
			switch ((uint8_t)*str) {
				case 0xD0: {
					// увеличиваем массив так как нам нужен второй байт
					str++;
					// проверяем второй байт там сам символ
					if ((uint8_t)*str == 0x81) { buff_char = 0xA8; break; }		// байт символа Ё ( если нужнф еще символы добавляем тут и в функции DrawChar() )
					else if ((uint8_t)*str >= 0x90 && (uint8_t)*str <= 0xBF){ buff_char = (*str) + 0x30; }	// байт символов А...Я а...п  делаем здвиг на +48
					else if ((uint8_t)*str == 0x84) { buff_char = 0xAA; break; }		// байт символа Є ( если нужнф еще символы добавляем тут и в функции DrawChar() )
					else if ((uint8_t)*str == 0x86) { buff_char = 0xB2; break; }		// байт символа І ( если нужнф еще символы добавляем тут и в функции DrawChar() )
					else if ((uint8_t)*str == 0x87) { buff_char = 0xAF; break; }		// байт символа Ї ( если нужнф еще символы добавляем тут и в функции DrawChar() )
					break;
				}
				case 0xD1: {
					// увеличиваем массив так как нам нужен второй байт
					str++;
					// проверяем второй байт там сам символ
					if ((uint8_t)*str == 0x91) { buff_char = 0xB8; break; }		// байт символа ё ( если нужнф еще символы добавляем тут и в функции DrawChar() )
					else if ((uint8_t)*str >= 0x80 && (uint8_t)*str <= 0x8F){ buff_char = (*str) + 0x70; }	// байт символов п...я	елаем здвиг на +112
					else if ((uint8_t)*str == 0x94) { buff_char = 0xBA; break; }		// байт символа є ( если нужнф еще символы добавляем тут и в функции DrawChar() )
					else if ((uint8_t)*str == 0x96) { buff_char = 0xB3; break; }		// байт символа і ( если нужнф еще символы добавляем тут и в функции DrawChar() )
					else if ((uint8_t)*str == 0x97) { buff_char = 0xBF; break; }		// байт символа ї ( если нужнф еще символы добавляем тут и в функции DrawChar() )
					break;
				}
			}
			// уменьшаем еще переменную так как израсходывали 2 байта для кириллицы
			len--;
			
			ST7565_DrawChar( x, y, buff_char, Font, multiplier, color);
		}
		//---------------------------------------------------------------------
		else{			
			ST7565_DrawChar( x, y, *str, Font, multiplier, color);
		}
		
		x = x + (Font->FontWidth * multiplier);
		/* Increase string pointer */
		str++;
	}
}
//----------------------------------------------------------------------------------


/*
	******************************************************************************
	* @brief	 ( описание ):  записываем линию в буфер кадра
	* @param	( параметры ):	координаты начальной точки и конечной и цвет 0 или 1
								X(0 - 127)  Y(0 - 63)
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7565_DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t c) {
	
	int16_t dx, dy, sx, sy, err, e2, i, tmp; 
	
	/* Check for overflow */
	if (x0 >= SCREEN_WIDTH) {
		x0 = SCREEN_WIDTH - 1;
	}
	if (x1 >= SCREEN_WIDTH) {
		x1 = SCREEN_WIDTH - 1;
	}
	if (y0 >= SCREEN_HEIGHT) {
		y0 = SCREEN_HEIGHT - 1;
	}
	if (y1 >= SCREEN_HEIGHT) {
		y1 = SCREEN_HEIGHT - 1;
	}
	
	dx = (x0 < x1) ? (x1 - x0) : (x0 - x1); 
	dy = (y0 < y1) ? (y1 - y0) : (y0 - y1); 
	sx = (x0 < x1) ? 1 : -1; 
	sy = (y0 < y1) ? 1 : -1; 
	err = ((dx > dy) ? dx : -dy) / 2; 

	if (dx == 0) {
		if (y1 < y0) {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}
		
		if (x1 < x0) {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}
		
		/* Vertical line */
		for (i = y0; i <= y1; i++) {
			ST7565_Draw_pixel(x0, i, c);
		}
		
		/* Return from function */
		return;
	}
	
	if (dy == 0) {
		if (y1 < y0) {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}
		
		if (x1 < x0) {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}
		
		/* Horizontal line */
		for (i = x0; i <= x1; i++) {
			ST7565_Draw_pixel(i, y0, c);
		}
		
		/* Return from function */
		return;
	}
	
	while (1) {
		ST7565_Draw_pixel(x0, y0, c);
		if (x0 == x1 && y0 == y1) {
			break;
		}
		e2 = err; 
		if (e2 > -dx) {
			err -= dy;
			x0 += sx;
		} 
		if (e2 < dy) {
			err += dx;
			y0 += sy;
		} 
	}
}
//----------------------------------------------------------------------------------


/*
	******************************************************************************
	* @brief	 ( описание ):  записываем прямоугольник пустотелый в буфер кадра
	* @param	( параметры ):	координаты верхнего левого и ширина и высота и цвет
								X(0 - 127)  Y(0 - 63)
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7565_DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t c) {
	/* Check input parameters */
	if (
		x >= SCREEN_WIDTH ||
		y >= SCREEN_HEIGHT
	) {
		/* Return error */
		return;
	}
	
	/* Check width and height */
	if ((x + w) >= SCREEN_WIDTH) {
		w = SCREEN_WIDTH - x;
	}
	if ((y + h) >= SCREEN_HEIGHT) {
		h = SCREEN_HEIGHT - y;
	}
	
	/* Draw 4 lines */
	ST7565_DrawLine(x, y, x + w, y, c);         /* Top line */
	ST7565_DrawLine(x, y + h, x + w, y + h, c); /* Bottom line */
	ST7565_DrawLine(x, y, x, y + h, c);         /* Left line */
	ST7565_DrawLine(x + w, y, x + w, y + h, c); /* Right line */
}
//----------------------------------------------------------------------------------



/*
	******************************************************************************
	* @brief	 ( описание ):  записываем прямоугольник закрашеный в буфер кадра
	* @param	( параметры ):	координаты верхнего левого и ширина и высота и цвет
								X(0 - 127)  Y(0 - 63)
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7565_DrawFilledRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t c) {
	
	uint8_t i;
	
	/* Check input parameters */
	if (
		x >= SCREEN_WIDTH ||
		y >= SCREEN_HEIGHT
	) {
		/* Return error */
		return;
	}
	
	/* Check width and height */
	if ((x + w) >= SCREEN_WIDTH) {
		w = SCREEN_WIDTH - x;
	}
	if ((y + h) >= SCREEN_HEIGHT) {
		h = SCREEN_HEIGHT - y;
	}
	
	/* Draw lines */
	for (i = 0; i <= h; i++) {
		/* Draw lines */
		ST7565_DrawLine(x, y + i, x + w, y + i, c);
	}
}
//----------------------------------------------------------------------------------


/*
	******************************************************************************
	* @brief	 ( описание ):  записываем триугольник пустотелый в буфер кадра
	* @param	( параметры ):	координаты вершин и цвет
								X(0 - 127)  Y(0 - 63)
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7565_DrawTriangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint8_t color) {
	/* Draw lines */
	ST7565_DrawLine(x1, y1, x2, y2, color);
	ST7565_DrawLine(x2, y2, x3, y3, color);
	ST7565_DrawLine(x3, y3, x1, y1, color);
}
//----------------------------------------------------------------------------------


/*
	******************************************************************************
	* @brief	 ( описание ):  записываем триугольник заполненый в буфер кадра
	* @param	( параметры ):	координаты вершин и цвет
								X(0 - 127)  Y(0 - 63)
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7565_DrawFilledTriangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint8_t color) {
	
	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
	yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0, 
	curpixel = 0;
	
	deltax = ABS(x2 - x1);
	deltay = ABS(y2 - y1);
	x = x1;
	y = y1;

	if (x2 >= x1) {
		xinc1 = 1;
		xinc2 = 1;
	} else {
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1) {
		yinc1 = 1;
		yinc2 = 1;
	} else {
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay){
		xinc1 = 0;
		yinc2 = 0;
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;
	} else {
		xinc2 = 0;
		yinc1 = 0;
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++) {
		ST7565_DrawLine(x, y, x3, y3, color);

		num += numadd;
		if (num >= den) {
			num -= den;
			x += xinc1;
			y += yinc1;
		}
		x += xinc2;
		y += yinc2;
	}
}
//----------------------------------------------------------------------------------


/*
	******************************************************************************
	* @brief	 ( описание ):  записываем круг пустотелый в буфер кадра
	* @param	( параметры ):	координаты центра круга, радиус и цвет
								X(0 - 127)  Y(0 - 63)
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7565_DrawCircle(int16_t x0, int16_t y0, int16_t r, uint8_t c) {
	
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

    ST7565_Draw_pixel(x0, y0 + r, c);
    ST7565_Draw_pixel(x0, y0 - r, c);
    ST7565_Draw_pixel(x0 + r, y0, c);
    ST7565_Draw_pixel(x0 - r, y0, c);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        ST7565_Draw_pixel(x0 + x, y0 + y, c);
        ST7565_Draw_pixel(x0 - x, y0 + y, c);
        ST7565_Draw_pixel(x0 + x, y0 - y, c);
        ST7565_Draw_pixel(x0 - x, y0 - y, c);

        ST7565_Draw_pixel(x0 + y, y0 + x, c);
        ST7565_Draw_pixel(x0 - y, y0 + x, c);
        ST7565_Draw_pixel(x0 + y, y0 - x, c);
        ST7565_Draw_pixel(x0 - y, y0 - x, c);
    }
}
//----------------------------------------------------------------------------------


/*
	******************************************************************************
	* @brief	 ( описание ):  записываем круг заполненый в буфер кадра
	* @param	( параметры ):	координаты центра круга, радиус и цвет
								X(0 - 127)  Y(0 - 63)
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7565_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, uint8_t c) {
	
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

    ST7565_Draw_pixel(x0, y0 + r, c);
    ST7565_Draw_pixel(x0, y0 - r, c);
    ST7565_Draw_pixel(x0 + r, y0, c);
    ST7565_Draw_pixel(x0 - r, y0, c);
    ST7565_DrawLine(x0 - r, y0, x0 + r, y0, c);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        ST7565_DrawLine(x0 - x, y0 + y, x0 + x, y0 + y, c);
        ST7565_DrawLine(x0 + x, y0 - y, x0 - x, y0 - y, c);

        ST7565_DrawLine(x0 + y, y0 + x, x0 - y, y0 + x, c);
        ST7565_DrawLine(x0 + y, y0 - x, x0 - y, y0 - x, c);
    }
}
//----------------------------------------------------------------------------------


/*
	******************************************************************************
	* @brief	 ( описание ):  функция подсчета длины строки с учетом кириллицы ( 2 байта на символ )
	* @param	( параметры ):	
	* @return  ( возвращает ):	

	******************************************************************************
*/
uint16_t strlenUTF8( char* str ){
	uint16_t i = 0;
	while( *str != '\0' ){
		if( (uint8_t)*str >= 0xC0 ){	// код 0xC0 соответствует символу кириллица 'A' по ASCII Win-1251
			str++;
		}
		
		i++;
		str++;
	}
	return i;
}
//----------------------------------------------------------------------------------


//==============================================================================
// Процедура рисования прямоугольник с закругленніми краями ( заполненый )
//==============================================================================
void ST7565_DrawFillRoundRect(int16_t x, int16_t y, uint16_t width, uint16_t height, int16_t cornerRadius, uint8_t color) {
	
	int16_t max_radius = ((width < height) ? width : height) / 2; // 1/2 minor axis
  if (cornerRadius > max_radius){
    cornerRadius = max_radius;
	}
	
  ST7565_DrawRectangleFilled(x + cornerRadius, y, x + cornerRadius + width - 2 * cornerRadius, y + height, color);
  // draw four corners
  ST7565_DrawFillCircleHelper(x + width - cornerRadius - 1, y + cornerRadius, cornerRadius, 1, height - 2 * cornerRadius - 1, color);
  ST7565_DrawFillCircleHelper(x + cornerRadius, y + cornerRadius, cornerRadius, 2, height - 2 * cornerRadius - 1, color);
}
//==============================================================================

//==============================================================================
// Процедура рисования половины окружности ( правая или левая ) ( заполненый )
//==============================================================================
void ST7565_DrawFillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t corners, int16_t delta, uint8_t color) {

  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;
  int16_t px = x;
  int16_t py = y;

  delta++; // Avoid some +1's in the loop

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    if (x < (y + 1)) {
      if (corners & 1){
        ST7565_DrawLine(x0 + x, y0 - y, x0 + x, y0 - y - 1 + 2 * y + delta, color);
			}
      if (corners & 2){
        ST7565_DrawLine(x0 - x, y0 - y, x0 - x, y0 - y - 1 + 2 * y + delta, color);
			}
    }
    if (y != py) {
      if (corners & 1){
        ST7565_DrawLine(x0 + py, y0 - px, x0 + py, y0 - px - 1 + 2 * px + delta, color);
			}
      if (corners & 2){
        ST7565_DrawLine(x0 - py, y0 - px, x0 - py, y0 - px - 1 + 2 * px + delta, color);
			}
			py = y;
    }
    px = x;
  }
}
//==============================================================================																		

//==============================================================================
// Процедура рисования четверти окружности (закругление, дуга) ( ширина 1 пиксель)
//==============================================================================
void ST7565_DrawCircleHelper(int16_t x0, int16_t y0, int16_t radius, int8_t quadrantMask, uint8_t color)
{
    int16_t f = 1 - radius ;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * radius;
    int16_t x = 0;
    int16_t y = radius;

    while (x <= y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
				
        x++;
        ddF_x += 2;
        f += ddF_x;

        if (quadrantMask & 0x4) {
            ST7565_DrawPixel(x0 + x, y0 + y, color);
            ST7565_DrawPixel(x0 + y, y0 + x, color);;
        }
        if (quadrantMask & 0x2) {
						ST7565_DrawPixel(x0 + x, y0 - y, color);
            ST7565_DrawPixel(x0 + y, y0 - x, color);
        }
        if (quadrantMask & 0x8) {
						ST7565_DrawPixel(x0 - y, y0 + x, color);
            ST7565_DrawPixel(x0 - x, y0 + y, color);
        }
        if (quadrantMask & 0x1) {
            ST7565_DrawPixel(x0 - y, y0 - x, color);
            ST7565_DrawPixel(x0 - x, y0 - y, color);
        }
    }
}
//==============================================================================		

//==============================================================================
// Процедура рисования прямоугольник с закругленніми краями ( пустотелый )
//==============================================================================
void ST7565_DrawRoundRect(int16_t x, int16_t y, uint16_t width, uint16_t height, int16_t cornerRadius, uint8_t color) {
	
	int16_t max_radius = ((width < height) ? width : height) / 2; // 1/2 minor axis
  if (cornerRadius > max_radius){
    cornerRadius = max_radius;
	}
	
  ST7565_DrawLine(x + cornerRadius, y, x + cornerRadius + width -1 - 2 * cornerRadius, y, color);         // Top
  ST7565_DrawLine(x + cornerRadius, y + height - 1, x + cornerRadius + width - 1 - 2 * cornerRadius, y + height - 1, color); // Bottom
  ST7565_DrawLine(x, y + cornerRadius, x, y + cornerRadius + height - 1 - 2 * cornerRadius, color);         // Left
  ST7565_DrawLine(x + width - 1, y + cornerRadius, x + width - 1, y + cornerRadius + height - 1 - 2 * cornerRadius, color); // Right
	
  // draw four corners
	ST7565_DrawCircleHelper(x + cornerRadius, y + cornerRadius, cornerRadius, 1, color);
  ST7565_DrawCircleHelper(x + width - cornerRadius - 1, y + cornerRadius, cornerRadius, 2, color);
	ST7565_DrawCircleHelper(x + width - cornerRadius - 1, y + height - cornerRadius - 1, cornerRadius, 4, color);
  ST7565_DrawCircleHelper(x + cornerRadius, y + height - cornerRadius - 1, cornerRadius, 8, color);
}
//==============================================================================





/************************ (C) COPYRIGHT GKP *****END OF FILE****/
