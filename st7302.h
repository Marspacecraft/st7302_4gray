#ifndef __ST7302_H__
#define __ST7302_H__

#include "main.h"
#include "keyconfig.h"
#include "fonts.h"

void st7302_begin(void);
void st7302_clear(void);
void st7302_inversion_on(void);
void st7302_inversion_off(void);
void st7302_display_on(void);
void st7302_display_off(void);
void st7302_flushBuffer(void);
void st7302_writecache_2gray(uint16_t xs,uint16_t ys,uint16_t xlen,uint16_t ylen,uint8_t* data,uint8_t mode);
void st7302_writecache_4gray(uint16_t xs,uint16_t ys,uint16_t xlen,uint16_t ylen,uint8_t* data,uint8_t mode,uint8_t bkup);
void st7302_drawline(uint16_t xs,uint16_t ys,uint16_t len,uint16_t width,uint8_t mode);
void st7302_drawline_4gray(uint16_t xs,uint16_t ys,uint16_t len,uint16_t width,uint8_t mode);
void st7302_drawrect(uint16_t xs,uint16_t ys,uint16_t xe,uint16_t ye,uint16_t width,uint8_t mode);
void st7302_drawrect_4gray(uint16_t xs,uint16_t ys,uint16_t xe,uint16_t ye,uint16_t width,uint8_t mode);



#define SCREEN_WIDTH    250
#define SCREEN_HEIGHT   122

#define REG_HEIGHT	33
#define REG_WIDTH 125

#define BUFFER_SIZE (REG_WIDTH*REG_HEIGHT)

#define CACHE_SIZE_WIDTH 	REG_WIDTH
#define CACHE_SIZE_HEIGHT 	REG_HEIGHT


/*********************************************************************************

	字体16x16 显示宽度 18x16
	框线宽：2
	框宽度：20
	行：6行，行间距2 顶间距3 低间距1 6*18+5*2+2+2=122 横屏顶部2行无法显示
	列：15列，首间距8 尾间距2            15*16+8+2=250

	屏幕分辨率 250x122byte           寄存器分辨率：250x132bit        125x33byte 可见：125x30byte

	寄存器排列方式:
		R[0][0]	R[0][1]
		D7 D6	D7 D6
		D5 D4	D5 D4
		D3 D2	D3 D2
		D1 D0	D1 D0
		
		R[1][0]	R[1][1]
		D7 D6	D7 D6
		D5 D4	D5 D4
		D3 D2	D3 D2
		D1 D0	D1 D0

	模式：
		gray2，两阶灰度	分辨率：250x132
		gray4，四节灰度 分辨率：125x66

	字体排列： 阴码，逐行式，高位在前


*********************************************************************************/

#define LCD_SHOW_MODE_GRAY_2				0x01//2阶灰度 正显
#define LCD_SHOW_MODE_GRAY_2_INVERSION		0x02//2阶灰度 反显

#define LCD_SHOW_MODE_GRAY_4_GRAY_0			0x00//4阶灰度 0阶显示
#define LCD_SHOW_MODE_GRAY_4_GRAY_1			0x08//4阶灰度 1阶显示
#define LCD_SHOW_MODE_GRAY_4_GRAY_2			0x06//4阶灰度 2阶显示
#define LCD_SHOW_MODE_GRAY_4_GRAY_3			0x07//4阶灰度 3阶显示
#define LCD_SHOW_MODE_GRAY_4_GRAY_4			0x0f//4阶灰度 4阶显示

#define LCD_SHOW_MODE_GRAY_4_BKG_GRAY_0			0x00//4阶灰度 0阶背景
#define LCD_SHOW_MODE_GRAY_4_BKG_GRAY_1			0x80//4阶灰度 1阶背景
#define LCD_SHOW_MODE_GRAY_4_BKG_GRAY_2			0x60//4阶灰度 2阶背景
#define LCD_SHOW_MODE_GRAY_4_BKG_GRAY_3			0x70//4阶灰度 3阶背景
#define LCD_SHOW_MODE_GRAY_4_BKG_GRAY_4			0xf0//4阶灰度 4阶背景




#define LCD_Init() 				st7302_begin()
#define LCD_Clear() 			st7302_clear()
#define LCD_Inversion_ON() 		st7302_inversion_on()
#define LCD_Inversion_OFF() 	st7302_inversion_off()

#define LCD_Display_ON() 		st7302_display_on()
#define LCD_Display_OFF() 		st7302_display_off()

#define LCD_Flush()				st7302_flushBuffer()


/**********************************************************************

	XS:[0,CACHE_SIZE_WIDTH*2)
	YS:[0,CACHE_SIZE_HEIGHT*4)
	XLEN:X bit width
	YLEN:Y bit height
	MODE:[LCD_MODE_INVERSION_OFF,LCD_MODE_INVERSION_ON]
	
**********************************************************************/
#define LCD_WriteFront_2Gray(XS,YS,XLEN,YLEN,DATA,MODE) 			st7302_writecache_2gray((XS),(YS),(XLEN),(YLEN),(DATA),(MODE))

/**********************************************************************

	XS:[0,CACHE_SIZE_WIDTH)
	YS:[0,CACHE_SIZE_HEIGHT*2)
	XLEN:X bit width(4 gray)
	YLEN:Y bit height(4 gray)
	MODE:[LCD_MODE_GRAY_0,LCD_MODE_GRAY_4] bit gray
	BKGD:[LCD_MODE_GRAY_0,LCD_MODE_GRAY_4] background gray
	
**********************************************************************/
#define LCD_WriteFront_4Gray(XS,YS,XLEN,YLEN,DATA,MODE,BKGD) 	st7302_writecache_4gray((XS),(YS),(XLEN),(YLEN),(DATA),(MODE),(BKGD))

/**********************************************************************

	XS:[0,CACHE_SIZE_WIDTH*2)
	YS:[0,CACHE_SIZE_HEIGHT*4)
	LEN: line length
	WIDTH:line width
	MODE:[LCD_MODE_INVERSION_OFF,LCD_MODE_INVERSION_ON]
	
**********************************************************************/
#define LCD_DrawLine(XS,YS,LEN,WIDTH,MODE) 			st7302_drawline((XS),(YS),(LEN),(WIDTH),(MODE));
/**********************************************************************

	XS:[0,CACHE_SIZE_WIDTH)
	YS:[0,CACHE_SIZE_HEIGHT*2)
	LEN: line length(4 gray)
	WIDTH:line width(4 gray)
	MODE:[LCD_MODE_GRAY_0,LCD_MODE_GRAY_4] line gray
	
**********************************************************************/
#define LCD_DrawLine_4Gray(XS,YS,LEN,WIDTH,MODE) 	st7302_drawline_4gray((XS),(YS),(LEN),(WIDTH),(MODE))

/**********************************************************************

	XS:[0,CACHE_SIZE_WIDTH*2)
	YS:[0,CACHE_SIZE_HEIGHT*4)
	XE:[0,CACHE_SIZE_WIDTH*2)
	YE:[0,CACHE_SIZE_HEIGHT*4)
	WIDTH:line width
	MODE:[LCD_MODE_INVERSION_OFF,LCD_MODE_INVERSION_ON]
	
**********************************************************************/
#define LCD_DrawRect(XS,YS,XE,YE,WIDTH,MODE) 			st7302_drawrect((XS),(YS),(XE),(YE),(WIDTH),(MODE));
/**********************************************************************

	XS:[0,CACHE_SIZE_WIDTH)
	YS:[0,CACHE_SIZE_HEIGHT*2)
	XE:[0,CACHE_SIZE_WIDTH)
	YE:[0,CACHE_SIZE_HEIGHT*2)
	WIDTH:line width(4 gray)
	MODE:[LCD_MODE_GRAY_0,LCD_MODE_GRAY_4] line gray
	
**********************************************************************/
#define LCD_DrawRect_4Gray(XS,YS,XE,YE,WIDTH,MODE) 	st7302_drawrect_4gray((XS),(YS),(XE),(YE),(WIDTH),(MODE))

void LCD_ShowString(uint16_t xs,uint16_t ys,unsigned char *string,fonts_e font,uint8_t mode);


typedef struct
{
	fonts_e tilte_font;	
	fonts_e table_font;
	
	uint8_t tablehand_width;//行头字符个数
	uint8_t table_width;//行字符数
	uint8_t table_num;//行个数
	uint8_t table_roll;//使能行滚动
	

}LCD_Menu;

void LCD_Menu_Init(LCD_Menu* menu);
void LCD_Menu_Exit(void);

void LCD_Menu_SetSelectTable(uint8_t table);
void LCD_Menu_SelectTableUp(void);
void LCD_Menu_SelectTableDown(void);
uint8_t LCD_Menu_GetSelectTable(void);

void LCD_Menu_SetTitleHead(uint8_t* heand);
void LCD_Menu_SetTableHead(uint8_t table,uint8_t* heand);
void LCD_Menu_SetTable(uint8_t table,uint8_t* heand);






#ifdef TEST_LCD
extern void testcase_lcd(void);
#endif

#endif


