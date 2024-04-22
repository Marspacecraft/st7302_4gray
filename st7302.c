#include <string.h>
#include "st7302.h"
#include "spi.h"
#include "tracelog.h"

#ifdef TEST_LCD
void testcase_lcd(void);
#endif


#define LCD_MODE_INVERSION_OFF 		0//正显
#define LCD_MODE_INVERSION_ON 		1//反显

#define LCD_MODE_GRAY_0 		0x00//0阶灰度 
#define LCD_MODE_GRAY_1 		0x08//1阶灰度
#define LCD_MODE_GRAY_2 		0x06//2阶灰度
#define LCD_MODE_GRAY_3 		0x07//3阶灰度 
#define LCD_MODE_GRAY_4 		0x0f//4阶灰度




#define LCD_RST_PIN 	LCD_RES_GPIO_Port, LCD_RES_Pin
#define LCD_DC_PIN      LCD_DC_GPIO_Port, LCD_DC_Pin
#define LCD_CS_PIN      LCD_CS_GPIO_Port, LCD_CS_Pin

#define LCD_PIN_Write(_pin, _value) HAL_GPIO_WritePin(_pin, _value == 0? GPIO_PIN_RESET:GPIO_PIN_SET)
static uint8_t sg_lcd_cache[CACHE_SIZE_WIDTH][CACHE_SIZE_HEIGHT] = {0};

void LCD_SPI_WriteByte(uint8_t value)
{
    SPI2_Transmit(SPI2_MODE_LCD, &value, 1);
}


void LCD_SPI_Init(void)
{
    //LCD_PIN_Write(LCD_DC_PIN, 0);
    LCD_PIN_Write(LCD_CS_PIN, 0);
    //LCD_PIN_Write(LCD_RST_PIN, 1);
}

void LCD_SPI_Exit(void)
{
   //LCD_PIN_Write(LCD_DC_PIN, 0);
    LCD_PIN_Write(LCD_CS_PIN, 1);
    //LCD_PIN_Write(LCD_RST_PIN, 0);
}


/******************************************************************************
function :	send command
parameter:
     Reg : Command register
******************************************************************************/
static void st7302_writeCommand(uint8_t Reg)
{
    LCD_PIN_Write(LCD_DC_PIN, 0);
    LCD_PIN_Write(LCD_CS_PIN, 0);
    LCD_SPI_WriteByte(Reg);
    LCD_PIN_Write(LCD_CS_PIN, 1);

}

/******************************************************************************
function :	send data
parameter:
    Data : Write data
******************************************************************************/
static void st7302_writeData8(uint8_t Data)
{
    LCD_PIN_Write(LCD_DC_PIN, 1);
    LCD_PIN_Write(LCD_CS_PIN, 0);
    LCD_SPI_WriteByte(Data);
    LCD_PIN_Write(LCD_CS_PIN, 1);
}

static void st7302_writeData(uint8_t* data ,uint16_t len)
{
    LCD_PIN_Write(LCD_DC_PIN, 1);
    LCD_PIN_Write(LCD_CS_PIN, 0);
    SPI2_Transmit(SPI2_MODE_LCD, data, len);
    //HAL_SPI_Transmit_DMA(&LCD_SPI_HANDLER, data, len);while(HAL_SPI_GetState(&LCD_SPI_HANDLER) == HAL_SPI_STATE_BUSY_RX);
    LCD_PIN_Write(LCD_CS_PIN, 1);
}


void st7302_begin()
{

	SPI2_Start(SPI2_MODE_LCD);
	
	PLKB_INFO("Init lcd ...\r\n");
	LCD_PIN_Write(LCD_RST_PIN, 0);
	HAL_Delay(100);
	LCD_PIN_Write(LCD_RST_PIN, 1);
	HAL_Delay(500);

	
	st7302_writeCommand(0x38);                                              
	st7302_writeCommand(0xEB);//Enable OTP    
	st7302_writeData8(0x02); 
	st7302_writeCommand(0xD7);//OTP Load Control    
	st7302_writeData8(0x68);       
	st7302_writeCommand(0xD1);//Auto Power Control       
	st7302_writeData8(0x01);      
	st7302_writeCommand(0xC0);//Gate Voltage Setting VGH=12V ; VGL=-5V       
	st7302_writeData8(0x80);       
	st7302_writeCommand(0xC1);//VSH Setting        
	st7302_writeData8(0x28);//       
	st7302_writeData8(0x28);            
	st7302_writeData8(0x28);       
	st7302_writeData8(0x28);      
	st7302_writeData8(0x14);      
	st7302_writeData8(0x00);     
	st7302_writeCommand(0xC2);//VSL Setting VSL=0     
	st7302_writeData8(0x00);      
	st7302_writeData8(0x00);           
	st7302_writeData8(0x00);       
	st7302_writeData8(0x00);    
	st7302_writeCommand(0xCB);//VCOMH Setting       
	st7302_writeData8(0x14);//14  0C   7    
	st7302_writeCommand(0xB4);//Gate EQ Setting HPM EQ LPM EQ      
	st7302_writeData8(0xE5);      
	st7302_writeData8(0x77);           
	st7302_writeData8(0xF1);      
	st7302_writeData8(0xFF);       
	st7302_writeData8(0xFF);       
	st7302_writeData8(0x4F);      
	st7302_writeData8(0xF1);       
	st7302_writeData8(0xFF);       
	st7302_writeData8(0xFF);      
	st7302_writeData8(0x4F);     
	st7302_writeCommand(0x11);//Sleep out      
	HAL_Delay(100);  // delay_ms 100ms      
	st7302_writeCommand(0xC7);//OSC Setting       
	st7302_writeData8(0xA6);       
	st7302_writeData8(0xE9);    
	st7302_writeCommand(0xB0);   //Duty Setting       
	st7302_writeData8(0x64);  //250duty/4=63 
	
	st7302_writeCommand(0x36);//Memory Data Access Control       
	#if 0
	st7302_writeData8(0x4C);
	#else
	st7302_writeData8(0x00);
	#endif
	st7302_writeCommand(0x3A);//Data Format Select 4 write for 24 bit       
	st7302_writeData8(0x11);     
	st7302_writeCommand(0xB9);//Source Setting       
	st7302_writeData8(0x23);      
	st7302_writeCommand(0xB8);//Panel Setting Frame inversion       
	st7302_writeData8(0x09);      
	st7302_writeCommand(0x2A);////Column Address Setting S61~S182       
	st7302_writeData8(0x05);       
	st7302_writeData8(0x36);         
	st7302_writeCommand(0x2B);////Row Address Setting G1~G250       
	st7302_writeData8(0x00);       
	st7302_writeData8(0xC7);        
	st7302_writeCommand(0xD0);       
	st7302_writeData8(0x1F); 
	st7302_writeCommand(0x29);//Display on  
	st7302_writeCommand(0xB9);//enable CLR RAM       
	st7302_writeData8(0xE3);      
	HAL_Delay(100);       
	st7302_writeCommand(0xB9);//enable CLR RAM     
	st7302_writeData8(0x23);  
	st7302_writeCommand(0x72);     
	st7302_writeData8(0x00);         //Destress OFF
	st7302_writeCommand(0x39);//LPM
	st7302_writeCommand(0x2A);   //Column Address Setting                          
	st7302_writeData8(0x19);                                                   
	st7302_writeData8(0x23);  //35                                               
	st7302_writeCommand(0x2B);   //Row Address Setting                             
	st7302_writeData8(0); 
	st7302_writeData8(0x7C);  
	st7302_writeData8(0x2C);   //write image data
	HAL_Delay(120);

	PLKB_INFO("LCD init end\r\n");

	#ifdef TEST_LCD
		testcase_lcd();
	#endif
    
}


void st7302_inversion_on(void)
{
    st7302_writeCommand(0x21);
}

void st7302_inversion_off(void)
{
    st7302_writeCommand(0x20);
}

void st7302_display_on(void)
{
    st7302_writeCommand(0x29);
}

void st7302_display_off(void)
{
    st7302_writeCommand(0x28);
}

void st7302_clearbuffer()
{
	memset(sg_lcd_cache[0],0,sizeof(sg_lcd_cache));
}

void st7302_clear(void)
{
    st7302_writeCommand(0xB9);
    st7302_writeData8(0xE3);
    HAL_Delay(200);
    st7302_writeCommand(0xB9);
    st7302_writeData8(0x23);

	st7302_clearbuffer();
}

/*************************************************************************************/
/*************************************************************************************/
/********************************************
*
*	x:水平像素坐标。左为0，[0,2*CACHE_SIZE_WIDTH-1]
*	y:垂直像素坐标。顶为0，[0,4*CACHE_SIZE_HEIGHT-1]
*	mode：0正显，1反显
*
********************************************/
static void st7302_1b2cache_2gray(uint16_t x,uint16_t y,uint8_t mode)
{//y=1 x=2
	uint16_t xs,ys;
	ys = y/4;//0
	if(ys>= CACHE_SIZE_HEIGHT)
		return;
	xs = x/2;//1
	if(xs>= CACHE_SIZE_WIDTH)
		return;

	if(mode)
	{
		sg_lcd_cache[xs][ys] &= ~(0x80 >> ((y%4)*2 + (x%2)));	// 0x80>>(3)
	}
	else
	{
		sg_lcd_cache[xs][ys] |= 0x80 >> ((y%4)*2 + (x%2));
	}
}

/********************************************
*
*	clear 1byte内[xs,ys]到[xe,ye]的值
*	mode：0正显，1反显
*
********************************************/
static void st7302_rectbyte2cache_2gray(uint16_t xs,uint16_t ys,uint16_t xe,uint16_t ye,uint8_t mode)
{
	uint16_t i,j;
	for(i=xs;i<xe;i++)
	{
		for(j=ys;j<ye;j++)
			st7302_1b2cache_2gray(i,j,!mode);
	}

}

/********************************************
*
*	[x,y]到[x+8,y]处写入1byte，高位先入
*	mode：0正显，1反显
*
********************************************/
static void st7302_8b2cache_2gray(uint16_t x,uint16_t y,uint8_t data,uint8_t mode)
{
	uint8_t i,v;
	for(i=0;i<8;i++)
	{
		v = data & (0x80>>i);
		if(v)
		{
			st7302_1b2cache_2gray(x+i,y,mode);
		}
	}
}

/********************************************
*
*	clear [xs,ys]到[xe,ye]的值，用于默认背景值
*	mode：0正显，1反显
*
********************************************/
void st7302_clearcache_2gray(uint16_t xs,uint16_t ys,uint16_t xe,uint16_t ye,uint8_t mode)
{
	uint16_t x_start,x_end,y_start,y_end,i;

	x_start = xs >> 1;
	y_start = ys >> 2;
	x_start++;
	y_start++;	
	
	x_end = xe >> 1;
	y_end = ye >> 2;

	st7302_rectbyte2cache_2gray(xs ,ys ,x_start<<1 ,ye ,mode);
	st7302_rectbyte2cache_2gray(xs ,ys ,xe ,y_start<<2 ,mode);
	st7302_rectbyte2cache_2gray(x_end<<1   ,ys ,xe ,ye ,mode);
	st7302_rectbyte2cache_2gray(xs ,y_end<<2 ,xe ,ye ,mode);
	
	
	for(i=x_start;(i<x_end)&&(i<CACHE_SIZE_WIDTH);i++)
	{
		if(y_end>=CACHE_SIZE_HEIGHT)
			y_end = CACHE_SIZE_HEIGHT-1;
		if(y_end<=y_start)
			return;
		
		if(mode)
			memset(&sg_lcd_cache[i][y_start],0xff,y_end-y_start);
		else
			memset(&sg_lcd_cache[i][y_start],0,y_end-y_start);
	}
}


/********************************************
*
*	x:水平像素坐标。左为0，[0,CACHE_SIZE_WIDTH-1]
*	y:垂直像素坐标。顶为0，[0,2*CACHE_SIZE_HEIGHT-1]
*	mode：0正显，1反显
*
********************************************/
static void st7302_1b2cache_4gray(uint16_t x,uint16_t y,uint8_t mode)
{//y=1 x=2
	uint16_t xs,ys;
	ys = y/2;//0
	if(ys>= CACHE_SIZE_HEIGHT)
		return;
	xs = x;//1
	if(xs>= CACHE_SIZE_WIDTH)
		return;

	if(y%2)
	{
		mode &= 0x0f ;
		sg_lcd_cache[xs][ys] &= 0xf0;
		sg_lcd_cache[xs][ys] |= mode;
	}
	else
	{
		mode = mode<<4 ;
		sg_lcd_cache[xs][ys] &= 0x0f;
		sg_lcd_cache[xs][ys] |= mode;
	}
}

/********************************************
*
*	clear 1byte内[xs,ys]到[xe,ye]的值
*	mode：0正显，1反显
*
********************************************/
static void st7302_rectbyte2cache_4gray(uint16_t xs,uint16_t ys,uint16_t xe,uint16_t ye,uint8_t bkup)
{
	uint16_t i,j;
	for(i=xs;i<xe;i++)
	{
		for(j=ys;j<ye;j++)
			st7302_1b2cache_4gray(i,j,bkup);
	}

}

/********************************************
*
*	[x,y]到[x+8,y]处写入1byte，高位先入
*	mode：0正显，1反显
*
********************************************/
static void st7302_8b2cache_4gray(uint16_t x,uint16_t y,uint8_t data,uint8_t mode)
{
	uint8_t i,v;
	for(i=0;i<8;i++)
	{
		v = data & (0x80>>i);
		if(v)
		{
			st7302_1b2cache_4gray(x+i,y,mode);
		}
	}
}


/********************************************
*
*	clear [xs,ys]到[xe,ye]的值，用于默认背景值
*	mode：0正显，1反显
*
********************************************/
void st7302_clearcache_4gray(uint16_t xs,uint16_t ys,uint16_t xe,uint16_t ye,uint8_t bkgd)
{
	uint16_t x_start,x_end,y_start,y_end,i;

	x_start = xs;
	y_start = ys >> 1;
	y_start++;	
	
	x_end = xe;
	y_end = ye >> 1;

	//st7302_rectbyte2cache_4gray(xs ,ys ,x_start ,ye ,bkgd);
	st7302_rectbyte2cache_4gray(xs ,ys ,xe ,y_start<<1 ,bkgd);
	//st7302_rectbyte2cache_4gray(x_end   ,ys ,xe ,ye ,bkgd);
	st7302_rectbyte2cache_4gray(xs ,y_end<<1 ,xe ,ye ,bkgd);
	
	
	for(i=x_start;(i<x_end)&&(i<CACHE_SIZE_WIDTH);i++)
	{
		if(y_end>=CACHE_SIZE_HEIGHT)
			y_end = CACHE_SIZE_HEIGHT-1;
		if(y_end<=y_start)
			return;
		bkgd |= (bkgd<<4);
		memset(&sg_lcd_cache[i][y_start],bkgd,(y_end-y_start));
	}
}


/*************************************************************************************/
/*************************************************************************************/

/********************************************
*
*	[xs,ys]到[xs+xlen,ys+ylen]处写入字体，字体只支持列行式
*	mode：0正显，1反显
*
********************************************/
void st7302_writecache_2gray(uint16_t xs,uint16_t ys,uint16_t xlen,uint16_t ylen,uint8_t* data,uint8_t mode)
{
	uint16_t x,y,k;
	uint8_t bitdata;
	k = 0;
	//设置背景值
	st7302_clearcache_2gray(xs,ys,xs+xlen,ys+ylen,mode);
	for(y=ys;y<ys+ylen;y++)
	{
		for(x=xs;x<xs+xlen;x+=8)
		{
			if(data[k])
			{
				bitdata = data[k];
				st7302_8b2cache_2gray(x,y,bitdata,mode);
			}
			k++;
		}
	}
}


/********************************************
*
*	[xs,ys]到[xs+xlen,ys+ylen]处写入字体，字体只支持列行式
*	mode：0正显，1反显
*
********************************************/
void st7302_writecache_4gray(uint16_t xs,uint16_t ys,uint16_t xlen,uint16_t ylen,uint8_t* data,uint8_t mode,uint8_t bkup)
{
	uint16_t x,y,k;
	uint8_t bitdata;
	k = 0;
	//设置背景值
	st7302_clearcache_4gray(xs,ys,xs+xlen,ys+ylen,bkup);
	for(y=ys;y<ys+ylen;y++)
	{
		for(x=xs;x<xs+xlen;x+=8)
		{
			if(data[k])
			{
				bitdata = data[k];
				st7302_8b2cache_4gray(x,y,bitdata,mode);
			}
			k++;
		}
	}
}

/*************************************************************************************/
/*************************************************************************************/

void st7302_drawline(uint16_t xs,uint16_t ys,uint16_t xlen,uint16_t ylen,uint8_t mode)
{
	st7302_rectbyte2cache_2gray(xs,ys,xs+xlen,ys+ylen,!mode);
}

void st7302_drawline_4gray(uint16_t xs,uint16_t ys,uint16_t xlen,uint16_t ylen,uint8_t mode)
{
	st7302_rectbyte2cache_4gray(xs,ys,xs+xlen,ys+ylen,mode);
}

void st7302_drawrect(uint16_t xs,uint16_t ys,uint16_t xe,uint16_t ye,uint16_t width,uint8_t mode)
{
	st7302_drawline(xs,ys,xe-xs,width,mode);
	st7302_drawline(xe,ys,width,ye-ys,mode);
	st7302_drawline(xs,ye,xe-xs+width,width,mode);
	st7302_drawline(xs,ys,width,ye-ys,mode);
}

void st7302_drawrect_4gray(uint16_t xs,uint16_t ys,uint16_t xe,uint16_t ye,uint16_t width,uint8_t mode)
{
	st7302_drawline_4gray(xs,ys,xe-xs,width,mode);
	st7302_drawline_4gray(xe,ys,width,ye-ys,mode);
	st7302_drawline_4gray(xs,ye,xe-xs+width,width,mode);
	st7302_drawline_4gray(xs,ys,width,ye-ys,mode);
}

void st7302_drawpicture_full(st7302_pic_t* pic)
{
	memcpy(sg_lcd_cache[0],pic->buffer,REG_HEIGHT*REG_WIDTH);
}

void st7302_drawpicture_partial(st7302_pic_t* pic)
{
	if(ST7302_TYPE_GRAY2 == pic->pictype)
	{
		if(ST7302_TYPE_GRAY2 == pic->showtype)
			st7302_writecache_2gray(pic->xs,pic->ys,pic->xlen,pic->ylen,pic->buffer,pic->showmode);
		else
			st7302_writecache_4gray(pic->xs,pic->ys,pic->xlen,pic->ylen,pic->buffer,pic->showmode&0x0f,pic->showmode>>4);
	}
	else
	{
		uint16_t i,j;
		for(i=pic->xs;i<pic->xlen;i++)
		{
			for(j=pic->ys;j<pic->ylen;j++)
			{
				if(ST7302_TYPE_GRAY2 == pic->showtype)
					st7302_1b2cache_2gray(i,j,pic->showmode);
				else
					st7302_1b2cache_4gray(i,j,pic->showmode&0x0f);
			}
		}
	}
}



/*************************************************************************************/
/*************************************************************************************/

void st7302_flushPartial(uint16_t xs,uint16_t ys,uint16_t xlen,uint16_t ylen)
{
	uint8_t RX_s,RX_e,RY_s,RY_e,i;

	RX_s = xs/2;
	RX_e = (xs+xlen)/2;
	RY_s = ys/12;
	RY_e = (ys+ylen)/12;

	st7302_writeCommand(0x2A);
	st7302_writeData8(BLOCK_START_Y+RY_s);
	st7302_writeData8(BLOCK_START_Y+RY_e);
	st7302_writeCommand(0x2B);
	st7302_writeData8(BLOCK_START_X+RX_s);
	st7302_writeData8(BLOCK_START_X+RX_e);
	st7302_writeCommand(0x2C);	

	for(i=RX_s;i<=RX_e;i++)
	{
		st7302_writeData(&sg_lcd_cache[i][RY_s*3],(RY_e-RY_s+1)*3);
	}

}

void st7302_flushBuffer(void) 
{
  // address set

	st7302_writeCommand(0x2A);
	st7302_writeData8(BLOCK_START_Y);
	st7302_writeData8(BLOCK_END_Y);
	st7302_writeCommand(0x2B);
	st7302_writeData8(BLOCK_START_X);
	st7302_writeData8(BLOCK_END_X);
	st7302_writeCommand(0x2C);	
	st7302_writeData(sg_lcd_cache[0],REG_HEIGHT*REG_WIDTH);

}



#ifdef TEST_LCD


#include "tracelog.h"
uint8_t love[4][32] = {
		{0x00,0x08,0x01,0xFC,0x7E,0x10,0x22,0x10,0x11,0x20,0x7F,0xFE,0x42,0x02,0x82,0x04,
			0x7F,0xF8,0x04,0x00,0x07,0xF0,0x0A,0x10,0x11,0x20,0x20,0xC0,0x43,0x30,0x1C,0x0E/*"爱",0*/},
		{0x10,0x00,0x10,0x00,0x10,0x00,0x1F,0xFC,0x10,0x00,0x20,0x00,0x20,0x00,0x3F,0xF8,
			0x00,0x08,0x00,0x08,0x00,0x08,0xFF,0xC8,0x00,0x08,0x00,0x08,0x00,0x50,0x00,0x20/*"与",1*/},
		{0x04,0x00,0x0E,0x00,0x78,0x00,0x08,0x7C,0x08,0x44,0xFF,0x44,0x08,0x44,0x18,0x44,
			0x1C,0x44,0x2A,0x44,0x2A,0x44,0x48,0x44,0x88,0x7C,0x08,0x44,0x08,0x00,0x08,0x00/*"和",2*/},
		{0x00,0x00,0x7F,0xFC,0x01,0x00,0x01,0x00,0x11,0x10,0x09,0x10,0x09,0x20,0x01,0x00,
			0xFF,0xFE,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00/*"平",3*/		}};
uint8_t biglove[] = {0x00,0x00,0x00,0x00,0x00,0x30,0x00,0x07,0xF8,0x0F,0xF8,0x20,0x02,0x10,0x60,0x01,
		0x08,0x40,0x01,0x8C,0x80,0x00,0x88,0x80,0x1F,0xFF,0xFC,0x10,0x00,0x08,0x30,0x30,
		0x10,0x20,0x60,0x20,0x1F,0xFF,0xF0,0x00,0x40,0x00,0x00,0x7F,0xC0,0x00,0xE0,0xC0,
		0x00,0xA0,0x80,0x01,0x11,0x00,0x03,0x0E,0x00,0x06,0x06,0x00,0x08,0x1B,0xC0,0x10,
		0xE0,0x7C,0x27,0x00,0x08,0x00,0x00,0x00/*"爱",0*/};

unsigned char gImage_h[2760] = { //0X01,0X01,0XB4,0X00,0X78,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X07,0XC0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0XFF,0XFF,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X7F,0XFF,0XFF,0XFE,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0XFF,0XFF,0XFF,0XFF,0X80,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X1F,0XFF,0XFF,0XFF,0XFF,0XF8,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X3F,0XFF,0XFF,0XFF,0XFF,0XFC,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X03,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XC0,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XF8,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X1F,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFC,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X3F,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X7F,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X80,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XC0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X07,0XFF,0XFF,0XFF,0XFC,0X00,0X03,0XFF,0XFF,0XF0,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X07,0XFF,0XFF,0XFF,
0XC0,0X00,0X00,0X1F,0XFF,0XF8,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X0F,0XFF,0XFF,0XFF,0X00,0X00,0X00,0X00,0XFF,0XFC,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X1F,0XFF,0XFF,0X80,0X00,0X00,
0X00,0X00,0X07,0XFE,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X3F,0XFF,0XFC,0X00,0X00,0X00,0X00,0X00,0X00,0XFF,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X7F,0XFF,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X07,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X7F,
0XFC,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X03,0X80,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X7F,0XC0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X40,0X00,0X00,0X00,0XFE,0X00,0X2C,0X00,0X00,0X00,0X00,0X00,0X00,0XFF,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X60,0X07,0XFF,0XFF,0XFE,0X00,0XEC,0X00,0X00,
0X00,0X00,0X00,0X01,0XF8,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X20,0X07,
0XFF,0XFF,0XF0,0X0F,0XEC,0X0F,0XF0,0X00,0X00,0X00,0X01,0XF8,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X10,0X00,0X60,0XC0,0X30,0X0F,0X0F,0XF0,0X70,0X00,0X00,
0X00,0X01,0XE0,0X00,0X00,0X00,0X3F,0XFF,0XFF,0XE0,0X00,0X00,0X18,0X00,0X70,0XE0,
0X70,0X0C,0X0F,0XF0,0X30,0X00,0X00,0X00,0X03,0XC0,0X00,0X00,0X03,0XFF,0XFF,0XFF,
0XFE,0X00,0X00,0X08,0X00,0X70,0X70,0X70,0X0C,0X0C,0X00,0X30,0X00,0X00,0X00,0X07,
0X80,0X00,0X0F,0XFF,0XFF,0XFF,0XFF,0XFF,0XFC,0X00,0X00,0X00,0X30,0X20,0X60,0X0C,
0X0C,0X00,0X30,0X00,0X00,0X00,0X07,0X00,0X00,0X3F,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0X00,0X00,0X07,0XFF,0XFF,0XFE,0X0F,0XFC,0X07,0XF0,0X00,0X00,0X00,0X06,0X00,0X07,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFC,0X04,0X07,0XFF,0XFF,0XFE,0X0F,0XFF,0XF7,
0XF0,0X00,0X00,0X00,0X00,0X00,0X3F,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X04,
0X06,0X00,0X00,0X06,0X0C,0X0F,0XF0,0X30,0X00,0X00,0X00,0X00,0X00,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XC6,0X06,0X0C,0X60,0X06,0X0C,0X0F,0XF0,0X30,0X00,
0X00,0X00,0X08,0X07,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0X06,0XCC,
0X31,0X9E,0X0C,0X00,0X30,0X30,0X00,0X00,0X00,0X08,0X1F,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0X7F,0XFF,0XFE,0X00,0XCC,0X31,0XD8,0X0C,0X0C,0X30,0X30,0X00,0X00,0X00,
0X10,0X7F,0XFF,0XFF,0XFF,0XFE,0XFF,0XFF,0XFF,0X7F,0XFF,0XFE,0X01,0XCC,0X01,0X9C,
0X0F,0XFC,0X37,0XF0,0X00,0X00,0X00,0X10,0XFF,0XFF,0XFF,0XFF,0XFC,0XFF,0XFF,0XFF,
0X7F,0XFF,0XFE,0X03,0XCE,0X03,0X8E,0X0F,0XFC,0X37,0XF0,0X00,0X00,0X00,0X03,0XFF,
0XFF,0XFF,0XFF,0XFC,0X7F,0XF7,0XFF,0X7F,0XFF,0XFE,0X07,0X0F,0XFF,0X06,0X0C,0X0C,
0X30,0X30,0X00,0X00,0X00,0X27,0XFF,0XFF,0XFF,0XFF,0XFC,0X7F,0XF7,0XFF,0X5F,0XFF,
0XFF,0X02,0X07,0XFC,0X06,0X0C,0X0C,0X30,0X30,0X00,0X00,0X00,0X3F,0XFF,0XFF,0XFF,
0XFF,0XFD,0X7F,0XF7,0XFF,0X5F,0XFF,0XFF,0X00,0X1C,0X00,0X00,0X0C,0X0C,0X30,0X30,
0X00,0X00,0X00,0X3F,0XFF,0XFF,0XFF,0XFF,0XFD,0XFF,0XF7,0XFF,0X5F,0XFF,0XFF,0X00,
0X3F,0XFF,0XF8,0X1F,0XEE,0X30,0X78,0X00,0X00,0X00,0X7F,0XFF,0XFF,0XFF,0XFF,0XF9,
0XFF,0XA7,0XFE,0X5F,0XFF,0XFF,0X00,0X7F,0XFF,0XF8,0X3F,0XFF,0XFF,0XFC,0X00,0X00,
0X00,0X7F,0XFF,0XFF,0XFF,0XFF,0XF8,0XFF,0XA7,0XFE,0X5F,0XFF,0XFF,0X00,0XFC,0X00,
0XF0,0X3F,0XFF,0XFF,0XFC,0X00,0X00,0X00,0X7F,0XFF,0XFF,0XFF,0XFF,0XF8,0XFF,0XAF,
0XFE,0X1F,0XFF,0XFF,0X87,0XCF,0X03,0XE0,0X00,0X10,0X00,0X00,0X00,0X00,0X00,0X7F,
0XFF,0XFF,0XFF,0XFF,0XF8,0XFF,0X8F,0XFE,0X3F,0XFB,0XFF,0X87,0X87,0X87,0X80,0X00,
0X38,0X1C,0X00,0X00,0X00,0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XE0,0XFE,0X0F,0XFE,0X3F,
0XFB,0XFF,0X86,0X00,0XFE,0X00,0X01,0XF8,0X1F,0X80,0X00,0X00,0X00,0XFF,0XFF,0XFF,
0XFF,0XFF,0XC0,0X0F,0X01,0XFE,0X3F,0XFB,0XFF,0X80,0X01,0XFF,0X00,0X07,0XF0,0X0F,
0XE0,0X00,0X00,0X01,0XFF,0XFF,0XFF,0XFF,0XFF,0X80,0X03,0XC0,0X1E,0X3F,0XF3,0XFF,
0X80,0X0F,0XFF,0XF0,0X0F,0XC0,0X03,0XF0,0X00,0X00,0X01,0XFF,0XFF,0XFF,0XFF,0XFE,
0X00,0X00,0X20,0X00,0X0F,0XF1,0XFF,0X8F,0XFE,0X01,0XFF,0XBF,0X00,0X00,0X7C,0X00,
0X00,0X03,0XFF,0XFF,0XFF,0XFF,0XFE,0X00,0X00,0X00,0X00,0X01,0XF1,0XFF,0X87,0XF0,
0X00,0X3F,0X1C,0X00,0X00,0X38,0X00,0X00,0X03,0XFF,0XFF,0XFF,0XFF,0XF8,0X00,0X00,
0X00,0X00,0X00,0X11,0XFF,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X03,
0XFF,0XFF,0XFF,0XFF,0XF8,0X00,0X00,0X00,0X00,0X00,0XC3,0XFF,0X80,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X07,0XFF,0XFF,0XFF,0XFF,0XE0,0X3C,0X00,0X00,0X00,
0X03,0XFB,0XFF,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X07,0XFF,0XFF,
0XFF,0XFF,0XE1,0XFF,0X00,0X00,0X00,0X00,0X0F,0XFF,0X80,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X0F,0XFF,0XFF,0XFF,0XFF,0X9F,0XE7,0XC0,0X00,0X00,0X00,0X01,
0XFF,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X0E,0XFF,0XFF,0XFF,0XFF,
0X9F,0X80,0XE0,0X00,0X00,0X00,0X01,0XFE,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X0D,0XFF,0XFF,0XFF,0XFF,0X0C,0X00,0X30,0X00,0X00,0X00,0X00,0XFE,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X13,0XFF,0XC1,0XFF,0XFE,0X08,0X00,
0X38,0X00,0X00,0X00,0X00,0XFE,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X03,0XFF,0X98,0X7F,0XFE,0X08,0X00,0X1C,0X00,0X00,0X00,0X00,0XFE,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X07,0XFF,0XA6,0X7F,0XFE,0X08,0X00,0X1C,0X00,
0X00,0X00,0X00,0XFE,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X0C,0X7F,
0X41,0X7F,0XFE,0X08,0X00,0X12,0X00,0X00,0X01,0XF8,0XFE,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X08,0XFE,0X00,0X7F,0XFE,0X08,0X00,0X12,0X00,0X00,0X07,
0XFC,0XFE,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XFE,0X01,0XFF,
0XFE,0X08,0X00,0X10,0X00,0X00,0X0C,0X4C,0XFC,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X01,0XFE,0X06,0X7F,0XFE,0X08,0X00,0X10,0X00,0X00,0X70,0X4D,0XF8,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X7E,0X04,0X7F,0XFE,0X08,
0X00,0X10,0X00,0X00,0X40,0X49,0XF8,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X7E,0X00,0X7F,0XFE,0X08,0X00,0X10,0X00,0X00,0X00,0X43,0XE0,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X3E,0X00,0X7F,0XFE,0X08,0X00,0X30,
0X00,0X00,0X00,0X43,0XE0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X1E,0X08,0X3F,0XFE,0X08,0X00,0X20,0X00,0X00,0X80,0X43,0XC0,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X1E,0X18,0X7F,0XFC,0X08,0X00,0X20,0X00,0X00,
0X80,0X43,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X06,0X10,
0XBF,0XFC,0X06,0X00,0X20,0X00,0X00,0X80,0X87,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X06,0X10,0XBF,0XFC,0X03,0X00,0X20,0X00,0X00,0X80,0X87,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X03,0X00,0XBF,0XFC,
0X01,0XF0,0X40,0X00,0X00,0X80,0X0F,0X00,0X00,0X03,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X03,0X90,0XBF,0XFC,0X00,0X7F,0XC0,0X00,0X00,0X80,0X0F,0X00,0X00,
0X1F,0X00,0X00,0X1F,0XFF,0XFF,0XF8,0X00,0X00,0X00,0X01,0X88,0X7F,0XFC,0X00,0X00,
0X00,0X00,0X00,0X81,0X0F,0X00,0X0F,0XFF,0X00,0X00,0X1F,0XFF,0XFF,0XF8,0X00,0X00,
0X00,0X01,0XC4,0X3F,0XFC,0X00,0X00,0X00,0X00,0X00,0X01,0X0F,0X00,0X0F,0XF8,0X3F,
0XFE,0X0F,0XFF,0XFF,0XF8,0X00,0X00,0X00,0X00,0X60,0XDF,0XFC,0X00,0X00,0X00,0X00,
0X00,0X62,0X1F,0X00,0X00,0X30,0X3F,0XFE,0X00,0X03,0X80,0X00,0X00,0X00,0X00,0X00,
0X70,0X1F,0XFC,0X00,0X00,0X00,0X00,0X00,0X32,0X1F,0X00,0X00,0X30,0X3F,0XFE,0X02,
0X03,0X80,0X40,0X00,0X00,0X00,0X00,0X78,0X0F,0XFC,0X00,0X00,0X00,0X00,0X00,0X0F,
0X1F,0X00,0X00,0X30,0X38,0X06,0X07,0X03,0X80,0X60,0X00,0X00,0X00,0X00,0X1C,0X0F,
0XFC,0X00,0X00,0X00,0X00,0X00,0X00,0X1F,0X00,0X00,0X30,0X38,0X06,0X07,0X83,0X81,
0XE0,0X00,0X00,0X00,0X00,0X1F,0X0F,0XF8,0X00,0X00,0X00,0X00,0X00,0X00,0X1F,0X00,
0X00,0X70,0X38,0X06,0X03,0X83,0X81,0XE0,0X00,0X00,0X00,0X00,0X07,0XFF,0XF8,0X00,
0X00,0X00,0X00,0X01,0X00,0X1F,0X00,0X1F,0XFF,0XB8,0X06,0X01,0XC3,0X83,0X80,0X00,
0X00,0X00,0X00,0X07,0XFF,0XF8,0X00,0X00,0X00,0X00,0X01,0X00,0X3E,0X00,0X1F,0XFF,
0XB8,0X06,0X00,0XE3,0X87,0X80,0X00,0X00,0X00,0X00,0X03,0XFF,0XF8,0X00,0X00,0X00,
0X00,0X01,0X00,0X3E,0X00,0X00,0XF0,0X38,0X06,0X00,0XF3,0X8F,0X00,0X00,0X00,0X00,
0X00,0X01,0XFF,0XF8,0X00,0X00,0X00,0X00,0X11,0X00,0X3E,0X00,0X00,0XF0,0X38,0X06,
0X00,0X73,0X8E,0X00,0X00,0X00,0X00,0X00,0X00,0XFF,0XF8,0X00,0X00,0X00,0X00,0X02,
0X00,0X3E,0X00,0X01,0XF8,0X38,0X06,0X00,0X23,0X8C,0X00,0X00,0X00,0X00,0X00,0X00,
0X7F,0XF8,0X00,0X00,0X00,0X00,0X02,0X00,0X3E,0X00,0X01,0XFC,0X38,0X06,0X00,0X03,
0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X3F,0XF8,0X00,0X00,0X00,0X00,0X00,0X00,0X3C,
0X00,0X03,0XFF,0X38,0X06,0X3F,0XFF,0XFF,0XFC,0X00,0X00,0X00,0X00,0X00,0X3F,0XF8,
0X00,0X00,0X00,0X00,0X00,0X00,0X3C,0X00,0X03,0XB7,0X38,0X06,0X7F,0XFF,0XFF,0XFE,
0X00,0X00,0X00,0X00,0X00,0X0F,0XF8,0X00,0X00,0X00,0X00,0X00,0X00,0X7C,0X00,0X07,
0X33,0XB8,0X06,0X3F,0XFF,0XFF,0XFC,0X00,0X00,0X00,0X00,0X00,0X0F,0XFC,0X00,0X00,
0X00,0X00,0X00,0X00,0XFC,0X00,0X06,0X33,0X38,0X06,0X00,0X03,0X80,0X00,0X00,0X00,
0X00,0X00,0X00,0X07,0XFC,0X00,0X00,0X00,0X00,0X00,0X00,0XFC,0X00,0X0E,0X30,0X38,
0X06,0X00,0X03,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X07,0XFC,0X00,0X00,0X00,0X00,
0X00,0X01,0XFC,0X00,0X1E,0X30,0X38,0X06,0X00,0X03,0X80,0X00,0X00,0X00,0X00,0X00,
0X00,0X03,0XFC,0X00,0X00,0X00,0X18,0X00,0X03,0XFC,0X00,0X0C,0X30,0X38,0X06,0X00,
0X03,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X03,0XFE,0X00,0X00,0X00,0X48,0X00,0X07,
0XF8,0X00,0X00,0X30,0X38,0X06,0X00,0X03,0X80,0X00,0X00,0X00,0X00,0X00,0X03,0XC3,
0XBD,0X80,0X00,0X00,0X48,0X00,0X07,0XF8,0X00,0X00,0X30,0X3F,0XFE,0X00,0X03,0X80,
0X00,0X00,0X00,0X00,0X00,0X02,0X0F,0X3C,0X60,0X00,0X00,0X00,0X00,0X09,0XF0,0X00,
0X00,0X30,0X3F,0XFE,0X00,0X03,0X80,0X00,0X00,0X00,0X00,0X00,0X06,0X01,0XBC,0X20,
0X00,0X00,0X00,0X00,0X11,0XF0,0X00,0X00,0X30,0X38,0X06,0X00,0X03,0X80,0X00,0X00,
0X00,0X00,0X00,0X04,0X00,0X3C,0X18,0X00,0X00,0X00,0X00,0X71,0XF0,0X00,0X00,0X30,
0X38,0X06,0X00,0X03,0X80,0X00,0X00,0X00,0X00,0X00,0X04,0X00,0X1C,0X06,0X00,0X00,
0X00,0X00,0XC1,0XF0,0X00,0X00,0X30,0X38,0X06,0X00,0X03,0X80,0X00,0X00,0X00,0X00,
0X00,0X04,0X00,0X0F,0X01,0X00,0X00,0X00,0X06,0X00,0XE0,0X00,0X00,0X30,0X00,0X00,
0X00,0X00,0X80,0X00,0X00,0X00,0X00,0X00,0X04,0X00,0X0F,0X81,0X80,0X00,0X00,0X04,
0X00,0XE0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X04,
0X00,0X06,0X60,0X3C,0X00,0X00,0X70,0X00,0XC0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X04,0X00,0X06,0X20,0X0F,0X00,0X01,0XC0,0X00,0XC0,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X02,
0X08,0X00,0X1F,0XE0,0X00,0X00,0XC0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X04,0X00,0X08,0X00,0X00,0X00,0XC0,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X02,0X00,
0X10,0X00,0X00,0X00,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X01,0X00,0X30,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0X80,0X20,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X60,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X20,0X40,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X08,0X00,
0X00,0X00,0X18,0X40,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X08,0X00,0X00,0X00,0X0C,0X40,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X18,0X00,0X00,0X00,
0X00,0XC0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X10,0X00,0X00,0X00,0X02,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0X80,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X20,0X00,0X00,0X00,0X00,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X60,0X00,0X00,0X00,0X00,0X40,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X40,
0X00,0X00,0X00,0X00,0X40,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X30,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X10,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X80,0X00,0X00,0X00,0X00,0X08,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,};



void test_bit()
{
	uint8_t i,j;
	for(i=0;i<CACHE_SIZE_WIDTH;i++)
	{
		for(j=0;j<CACHE_SIZE_HEIGHT;j++)
		{
			memset(sg_lcd_cache,0,sizeof(sg_lcd_cache));
			sg_lcd_cache[i][j] = 0xff;
			st7302_flushBuffer();	
			HAL_Delay(100);
		}
	}
}

static void test_chinese()
{
	PLKB_DEBUG("\ttest_chinese...\r\n");
	

	st7302_writecache_2gray(0,0,16,16,love[0],0);
	st7302_writecache_2gray(0,16,16,16,love[0],1);
	
	st7302_writecache_2gray(16,0,16,16,love[1],0);
	st7302_writecache_2gray(16,16,16,16,love[1],1);
	
	st7302_writecache_2gray(32,0,16,16,love[2],0);
	st7302_writecache_2gray(32,16,16,16,love[2],1);
	
	st7302_writecache_2gray(48,0,16,16,love[3],0);
	st7302_writecache_2gray(48,16,16,16,love[3],1);
	st7302_flushBuffer();
	PLKB_DEBUG("\ttest_chinese end...\r\n");

}

void test_chinese_4gray()
{
	PLKB_DEBUG("\ttest_chinese...\r\n");

	st7302_writecache_4gray(0,0,16,16,love[0],LCD_MODE_GRAY_4,LCD_MODE_GRAY_0);
	st7302_writecache_4gray(0,16,16,16,love[0],LCD_MODE_GRAY_0,LCD_MODE_GRAY_4);
	
	st7302_writecache_4gray(16,0,16,16,love[1],LCD_MODE_GRAY_4,LCD_MODE_GRAY_0);
	st7302_writecache_4gray(16,16,16,16,love[1],LCD_MODE_GRAY_0,LCD_MODE_GRAY_4);
	
	st7302_writecache_4gray(32,0,16,16,love[2],LCD_MODE_GRAY_4,LCD_MODE_GRAY_0);
	st7302_writecache_4gray(32,16,16,16,love[2],LCD_MODE_GRAY_0,LCD_MODE_GRAY_4);
	
	st7302_writecache_4gray(48,0,16,16,love[3],LCD_MODE_GRAY_4,LCD_MODE_GRAY_0);
	st7302_writecache_4gray(48,16,16,16,love[3],LCD_MODE_GRAY_0,LCD_MODE_GRAY_4);
	st7302_flushBuffer();
	PLKB_DEBUG("\ttest_chinese end...\r\n");
}

void test_4gray()
{
	PLKB_DEBUG("\ttest_4gray...\r\n");

	st7302_writecache_4gray(0,0,16,16,love[0],LCD_MODE_GRAY_4,LCD_MODE_GRAY_0);
	st7302_flushBuffer();
	HAL_Delay(2000);

	st7302_writecache_4gray(0,0,16,16,love[0],LCD_MODE_GRAY_3,LCD_MODE_GRAY_0);
	st7302_flushBuffer();
	HAL_Delay(2000);

	st7302_writecache_4gray(0,0,16,16,love[0],LCD_MODE_GRAY_2,LCD_MODE_GRAY_0);
	st7302_flushBuffer();
	HAL_Delay(2000);

	st7302_writecache_4gray(0,0,16,16,love[0],LCD_MODE_GRAY_1,LCD_MODE_GRAY_0);
	st7302_flushBuffer();
	HAL_Delay(2000);

	st7302_writecache_4gray(0,0,16,16,love[0],LCD_MODE_GRAY_0,LCD_MODE_GRAY_0);
	st7302_flushBuffer();
	HAL_Delay(2000);

	st7302_writecache_4gray(0,0,16,16,love[0],LCD_MODE_GRAY_0,LCD_MODE_GRAY_1);
	st7302_flushBuffer();
	HAL_Delay(2000);

	st7302_writecache_4gray(0,0,16,16,love[0],LCD_MODE_GRAY_0,LCD_MODE_GRAY_2);
	st7302_flushBuffer();
	HAL_Delay(2000);

	st7302_writecache_4gray(0,0,16,16,love[0],LCD_MODE_GRAY_0,LCD_MODE_GRAY_3);
	st7302_flushBuffer();
	HAL_Delay(2000);

	st7302_writecache_4gray(0,0,16,16,love[0],LCD_MODE_GRAY_0,LCD_MODE_GRAY_4);
	st7302_flushBuffer();
	HAL_Delay(2000);

	st7302_writecache_4gray(0,0,16,16,love[0],LCD_MODE_GRAY_1,LCD_MODE_GRAY_3);
	st7302_flushBuffer();
	HAL_Delay(2000);

	st7302_writecache_4gray(0,0,16,16,love[0],LCD_MODE_GRAY_2,LCD_MODE_GRAY_2);
	st7302_flushBuffer();
	HAL_Delay(2000);

	st7302_writecache_4gray(0,0,16,16,love[0],LCD_MODE_GRAY_3,LCD_MODE_GRAY_1);
	st7302_flushBuffer();
	HAL_Delay(2000);


	st7302_writecache_4gray(0,0,24,24,biglove,LCD_MODE_GRAY_0,LCD_MODE_GRAY_4);
		st7302_flushBuffer();

	

	PLKB_DEBUG("\ttest_4gray end...\r\n");
}

void test_draw()
{
	PLKB_DEBUG("\ttest_drawy end...\r\n");

	LCD_DrawLine(0,16,16,3,LCD_MODE_INVERSION_OFF);
	LCD_DrawLine_4Gray(0,10,16,1,LCD_MODE_GRAY_4);
	LCD_DrawLine_4Gray(0,12,16,2,LCD_MODE_GRAY_3);
	LCD_DrawLine_4Gray(0,15,16,3,LCD_MODE_GRAY_2);
	LCD_DrawLine_4Gray(0,19,16,4,LCD_MODE_GRAY_1);
	
	LCD_DrawRect(50,0,80,10,2,LCD_MODE_INVERSION_OFF);
	LCD_DrawRect_4Gray(28,10,40,20,4,LCD_MODE_GRAY_4);
	LCD_DrawRect_4Gray(24,15,40,25,3,LCD_MODE_GRAY_3);
	LCD_DrawRect_4Gray(20,20,40,30,2,LCD_MODE_GRAY_2);
	LCD_DrawRect_4Gray(16,23,40,33,1,LCD_MODE_GRAY_1);

	
	LCD_Flush();
	HAL_Delay(2000);
	
	
	
	PLKB_DEBUG("\ttest_draw end...\r\n");
}

void test_drawpic()
{
	st7302_pic_t pic;
	pic.xs = 0;
	pic.ys = 0;

	pic.xlen = 184;
	pic.ylen = 120;

	pic.buffer = (uint8_t*)gImage_h;
	pic.pictype = ST7302_TYPE_GRAY2;
	pic.showtype = ST7302_TYPE_GRAY2;
	pic.showmode = LCD_SHOW_MODE_GRAY_2;
	
	st7302_drawpicture_partial(&pic);

	LCD_Flush();
	HAL_Delay(2000);
}
void test_partial()
{
	PLKB_DEBUG("\ttest_partial end...\r\n");

	memset(sg_lcd_cache[0],0xff,sizeof(sg_lcd_cache));
	LCD_Flush();
	HAL_Delay(2000);

	st7302_writecache_2gray(0,16,16,16,love[0],1);
	LCD_FlushPartial(0,16,16,16);
	HAL_Delay(2000);


	st7302_writecache_2gray(0,0,16,16,love[0],1);
	st7302_writecache_4gray(0,16,16,16,love[0],LCD_MODE_GRAY_0,LCD_MODE_GRAY_4);
	LCD_FlushPartial_4gray(0,16,16,16);

	HAL_Delay(2000);



	PLKB_DEBUG("\ttest_partial end...\r\n");
}

void test_all()
{
	test_chinese();
	HAL_Delay(2000);
	st7302_clear();
	
	test_chinese_4gray();
	HAL_Delay(2000);
	st7302_clear();
	
	test_4gray();
	HAL_Delay(2000);
	st7302_clear();
	
	test_draw();
	HAL_Delay(2000);

	test_partial();
	HAL_Delay(2000);
	
	LCD_Inversion_ON();
	HAL_Delay(2000);

	LCD_Inversion_OFF();
	HAL_Delay(2000);
	
	LCD_Display_OFF();
	HAL_Delay(2000);

	LCD_Display_ON();
	HAL_Delay(2000);

}

void testcase_lcd()
{
	PLKB_DEBUG("Runnign testcase_lcd...\r\n");	
	//test_bit();
	//test_chinese();
	//test_chinese_4gray();
	//test_4gray();
	//test_draw();
	//test_partial();
	//test_drawpic();
	test_all();

	LCD_Display_OFF();
	PLKB_DEBUG("testcase_lcd...\r\n");
}

#endif


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////




