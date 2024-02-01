#include <string.h>
#include "st7302.h"
#include "spi.h"

#define LCD_MODE_INVERSION_OFF 		0//正显
#define LCD_MODE_INVERSION_ON 		1//反显

#define LCD_MODE_GRAY_0 		0x00//0阶灰度 
#define LCD_MODE_GRAY_1 		0x08//1阶灰度
#define LCD_MODE_GRAY_2 		0x06//2阶灰度
#define LCD_MODE_GRAY_3 		0x07//3阶灰度 
#define LCD_MODE_GRAY_4 		0x0f//4阶灰度




#define LCD_RST_PIN 	EPD_RST_GPIO_Port, EPD_RST_Pin
#define LCD_DC_PIN      EPD_DC_GPIO_Port, EPD_DC_Pin
#define LCD_CS_PIN      EPD_CS_GPIO_Port, EPD_CS_Pin

#define LCD_PIN_Write(_pin, _value) HAL_GPIO_WritePin(_pin, _value == 0? GPIO_PIN_RESET:GPIO_PIN_SET)
static uint8_t sg_lcd_cache[CACHE_SIZE_WIDTH][CACHE_SIZE_HEIGHT] = {0};


void LCD_SPI_WriteByte(uint8_t value)
{
    HAL_SPI_Transmit(&EXT_SPI_HANDLER, &value, 1, 1000);
}


int LCD_SPI_Init(void)
{
    LCD_PIN_Write(LCD_DC_PIN, 0);
    LCD_PIN_Write(LCD_CS_PIN, 0);
    LCD_PIN_Write(LCD_RST_PIN, 1);
	return 0;
}

void LCD_SPI_Exit(void)
{
    LCD_PIN_Write(LCD_DC_PIN, 0);
    LCD_PIN_Write(LCD_CS_PIN, 0);
    LCD_PIN_Write(LCD_RST_PIN, 0);
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
    HAL_SPI_Transmit(&EXT_SPI_HANDLER, data, len, 1000);
    //HAL_SPI_Transmit_DMA(&EXT_SPI_HANDLER, data, len);
    LCD_PIN_Write(LCD_CS_PIN, 1);
}

/******************************************************************************
      函数说明：设置起始和结束地址
      入口数据：x1,x2 设置列的起始和结束地址
                y1,y2 设置行的起始和结束地址
      返回值：  无
******************************************************************************/


void st7302_addrSet(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
		st7302_writeCommand(0x2a); //列地址设置
		st7302_writeData8(x1+0x19);
		st7302_writeData8(x2+0x19);
		st7302_writeCommand(0x2b); //行地址设置
		st7302_writeData8(y1);
		st7302_writeData8(y2);
		st7302_writeCommand(0x2c); //储存器写
}

void st7302_begin()
{
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

void st7302_clear(void)
{
    st7302_writeCommand(0xB9);
    st7302_writeData8(0xE3);
    HAL_Delay(200);
    st7302_writeCommand(0xB9);
    st7302_writeData8(0x23);

	memset(sg_lcd_cache,0,sizeof(sg_lcd_cache));
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
static void st7302_rect2cache_2gray(uint16_t xs,uint16_t ys,uint16_t xe,uint16_t ye,uint8_t mode)
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
static void st7302_rect2cache_4gray(uint16_t xs,uint16_t ys,uint16_t xe,uint16_t ye,uint8_t bkgd)
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
	st7302_rect2cache_2gray(xs,ys,xs+xlen,ys+ylen,mode);
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
	st7302_rect2cache_4gray(xs,ys,xs+xlen,ys+ylen,bkup);
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




void st7302_flushBuffer(void) 
{
  // address set
	st7302_writeCommand(0x2A);
	st7302_writeData8(0x19);
	st7302_writeData8(0x23);
	st7302_writeCommand(0x2B);
	#if 0
	st7302_writeData8(0x4B);
	st7302_writeData8(0xC7);
	#else
	st7302_writeData8(0x00);
	st7302_writeData8(0x7C);
	#endif
	st7302_writeCommand(0x2C);	
	st7302_writeData(sg_lcd_cache[0],REG_HEIGHT*REG_WIDTH);
}


void LCD_ShowString(uint16_t xs,uint16_t ys,unsigned char *string,fonts_e font,uint8_t mode)
{
	fonthandler handler;
	font_t * fonts;

	handler.startpos = 0;
	handler.string = string;

	while(1)
	{
		fonts = Fonts_Get(font, &handler);
		if(NULL == fonts)
			break;

		if(mode == 1 || mode == 2)
		{
			st7302_writecache_2gray(xs,ys,fonts->Width,fonts->Height,fonts->Font,mode-1);		
		}
		else
		{			
			st7302_writecache_4gray(xs,ys,fonts->Width,fonts->Height,fonts->Font,mode&0x0f,(mode&0xf0)>>4);
		}
		xs += fonts->FontWidth;
	}
	LCD_Flush();

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
	TRACELOG_DEBUG("\ttest_chinese...");
	

	st7302_writecache_2gray(0,0,16,16,love[0],0);
	st7302_writecache_2gray(0,16,16,16,love[0],1);
	
	st7302_writecache_2gray(16,0,16,16,love[1],0);
	st7302_writecache_2gray(16,16,16,16,love[1],1);
	
	st7302_writecache_2gray(32,0,16,16,love[2],0);
	st7302_writecache_2gray(32,16,16,16,love[2],1);
	
	st7302_writecache_2gray(48,0,16,16,love[3],0);
	st7302_writecache_2gray(48,16,16,16,love[3],1);
	st7302_flushBuffer();
	TRACELOG_DEBUG("\ttest_chinese end...");

}

void test_chinese_4gray()
{
	TRACELOG_DEBUG("\ttest_chinese...");

	st7302_writecache_4gray(0,0,16,16,love[0],LCD_MODE_GRAY_4,LCD_MODE_GRAY_0);
	st7302_writecache_4gray(0,16,16,16,love[0],LCD_MODE_GRAY_0,LCD_MODE_GRAY_4);
	
	st7302_writecache_4gray(16,0,16,16,love[1],LCD_MODE_GRAY_4,LCD_MODE_GRAY_0);
	st7302_writecache_4gray(16,16,16,16,love[1],LCD_MODE_GRAY_0,LCD_MODE_GRAY_4);
	
	st7302_writecache_4gray(32,0,16,16,love[2],LCD_MODE_GRAY_4,LCD_MODE_GRAY_0);
	st7302_writecache_4gray(32,16,16,16,love[2],LCD_MODE_GRAY_0,LCD_MODE_GRAY_4);
	
	st7302_writecache_4gray(48,0,16,16,love[3],LCD_MODE_GRAY_4,LCD_MODE_GRAY_0);
	st7302_writecache_4gray(48,16,16,16,love[3],LCD_MODE_GRAY_0,LCD_MODE_GRAY_4);
	st7302_flushBuffer();
	TRACELOG_DEBUG("\ttest_chinese end...");
}

void test_4gray()
{
	TRACELOG_DEBUG("\ttest_4gray...");

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

	

	TRACELOG_DEBUG("\ttest_4gray end...");
}

void test_draw()
{
	TRACELOG_DEBUG("\ttest_drawy end...");

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
	TRACELOG_DEBUG("\ttest_draw end...");
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
	
	LCD_Inversion_ON();
	HAL_Delay(2000);

	LCD_Inversion_OFF();
	HAL_Delay(2000);
	
	LCD_Display_OFF();
	HAL_Delay(2000);

	LCD_Display_ON();
	HAL_Delay(2000);
}

void test_font()
{

	//LCD_ShowString(0,0,"Pace and Love Keyboard",FONT_ASCII_5x8,LCD_SHOW_MODE_GRAY_4_GRAY_4|LCD_SHOW_MODE_GRAY_4_BKG_GRAY_0);
	//LCD_ShowString(0,8,"abcdefghijklmnopqrstuvwxyz",FONT_ASCII_5x8,LCD_SHOW_MODE_GRAY_4_GRAY_4|LCD_SHOW_MODE_GRAY_4_BKG_GRAY_0);
	//LCD_ShowString(0,16,"nopqrstuvwxyz",FONT_ASCII_5x8,LCD_SHOW_MODE_GRAY_4_GRAY_4|LCD_SHOW_MODE_GRAY_4_BKG_GRAY_0);
	//LCD_ShowString(0,24,"ABCDEFGHIJKLM",FONT_ASCII_5x8,LCD_SHOW_MODE_GRAY_4_GRAY_4|LCD_SHOW_MODE_GRAY_4_BKG_GRAY_0);
	//LCD_ShowString(0,32,"NOPQRSTUVWXYZ",FONT_ASCII_5x8,LCD_SHOW_MODE_GRAY_4_GRAY_4|LCD_SHOW_MODE_GRAY_4_BKG_GRAY_0);
	//LCD_ShowString(0,40,"0123456789",FONT_ASCII_5x8,LCD_SHOW_MODE_GRAY_4_GRAY_4|LCD_SHOW_MODE_GRAY_4_BKG_GRAY_0);
	//LCD_ShowString(0,48,"[]{}<>|/\\@^!`=+-_%$#*",FONT_ASCII_5x8,LCD_SHOW_MODE_GRAY_4_GRAY_4|LCD_SHOW_MODE_GRAY_4_BKG_GRAY_0);
	//LCD_ShowString(0,0,"Pace and Love Keyboard",FONT_ASCII_5x8,LCD_SHOW_MODE_GRAY_2);
	LCD_ShowString(0,0,"abcdefghijklmnopqrstuvwxyz",FONT_ASCII_5x8,LCD_SHOW_MODE_GRAY_2);
	//LCD_ShowString(0,16,"nopqrstuvwxyz",FONT_ASCII_5x8,LCD_SHOW_MODE_GRAY_2);
	//LCD_ShowString(0,24,"ABCDEFGHIJKLM",FONT_ASCII_5x8,LCD_SHOW_MODE_GRAY_2);
	//LCD_ShowString(0,32,"NOPQRSTUVWXYZ",FONT_ASCII_5x8,LCD_SHOW_MODE_GRAY_2);
	//LCD_ShowString(0,40,"0123456789",FONT_ASCII_5x8,LCD_SHOW_MODE_GRAY_2);
	LCD_ShowString(0,8,"[]{}<>|/\\@^!`=+-_%$#*",FONT_ASCII_5x8,LCD_SHOW_MODE_GRAY_2);

	LCD_ShowString(0,16,"abcdefghijklmnopqrstuvwxyz",FONT_ASCII_7x12,LCD_SHOW_MODE_GRAY_2);
	LCD_ShowString(0,28,"abcdefghijklmnopqrstuvwxyz",FONT_ASCII_11x16,LCD_SHOW_MODE_GRAY_2);
	LCD_ShowString(0,44,"abcdefghijklmnopqrstuvwxyz",FONT_ASCII_14x20,LCD_SHOW_MODE_GRAY_2);
	LCD_ShowString(0,64,"abcdefghijklmnopqrstuvwxyz",FONT_ASCII_17x24,LCD_SHOW_MODE_GRAY_2);
	LCD_ShowString(0,88,"abcdefghijklmnopqrstuvwxyz",FONT_ASCII_17x24,LCD_SHOW_MODE_GRAY_2_INVERSION);


	
	//LCD_ShowString(0,8,"abcdefghijklmnopqrstuvwxyz",FONT_ASCII_7x12,LCD_SHOW_MODE_GRAY_4_GRAY_4|LCD_SHOW_MODE_GRAY_4_BKG_GRAY_3);
	//LCD_ShowString(0,20,"abcdefghijklmnopqrstuvwxyz",FONT_ASCII_11x16,LCD_SHOW_MODE_GRAY_4_GRAY_4|LCD_SHOW_MODE_GRAY_4_BKG_GRAY_1);
	//LCD_ShowString(0,36,"abcdefghijklmnopqrstuvwxyz",FONT_ASCII_14x20,LCD_SHOW_MODE_GRAY_4_GRAY_4|LCD_SHOW_MODE_GRAY_4_BKG_GRAY_3);
	//LCD_ShowString(0,36,"abcdefghijklmnopqrstuvwxyz",FONT_ASCII_17x24,LCD_SHOW_MODE_GRAY_4_GRAY_0|LCD_SHOW_MODE_GRAY_4_BKG_GRAY_4);
}

void testcase_lcd()
{
	TRACELOG_DEBUG("Runnign testcase_lcd...");
	st7302_begin();	
	//test_bit();
	//test_chinese();
	//test_chinese_4gray();
	//test_4gray();
	//test_draw();
	//test_all();
	test_font();
	TRACELOG_DEBUG("testcase_lcd...");
}

#endif


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////




