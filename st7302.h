#ifndef __ST7302_H__
#define __ST7302_H__

#include "main.h"
#include "keyconfig.h"

/*********************************************************************************

	��Ļ�ֱ��� 250x122   

	�Ĵ�����
		ÿ���Ĵ�������24�����أ�0x2A��0x2B��ѡ��ѡ�Ĵ�����Χ
			0x2A�������мĴ�����ʼ��Χ [BLOCK_START_Y,BLOCK_END_X]
			0x2B�������мĴ�����ʼ��Χ [BLOCK_START_X,BLOCK_END_Y]

		�Ĵ����ֱ��ʣ�250x120bit        125x30byte ��������2���޷���ʾ
	
		�Ĵ������з�ʽ:
			R1		R2
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

			R[2][0]	R[2][1]
			D7 D6	D7 D6
			D5 D4	D5 D4
			D3 D2	D3 D2
			D1 D0	D1 D0
			
		�Ĵ���ɨ��˳��
			��ɨ���У���ɨ����
		

	ģʽ��
		gray2�����׻Ҷ�	�ֱ��ʣ�250x120
		gray4���ĽڻҶ� �ֱ��ʣ�125x60

	�������У� ���룬����ʽ����λ��ǰ


*********************************************************************************/

#define BLOCK_START_X	0x00
#define BLOCK_START_Y	0x19

#define BLOCK_END_X		0x7C
#define BLOCK_END_Y		0x23

#define BLOCK_NUM_X		(BLOCK_END_X-BLOCK_START_X+1)//125
#define BLOCK_NUM_Y		(BLOCK_END_Y-BLOCK_START_Y+1)//11

#define REG_WIDTH 		BLOCK_NUM_X//125
#define REG_HEIGHT		(BLOCK_NUM_Y*3)//33

#define SCREEN_WIDTH    (REG_WIDTH*2)//250
#define SCREEN_HEIGHT   (REG_HEIGHT*4)//132

#define BUFFER_SIZE (REG_WIDTH*REG_HEIGHT)

#define CACHE_SIZE_WIDTH 	REG_WIDTH
#define CACHE_SIZE_HEIGHT 	REG_HEIGHT


#define LCD_SHOW_MODE_GRAY_2				0x00//2�׻Ҷ� ����
#define LCD_SHOW_MODE_GRAY_2_INVERSION		0x01//2�׻Ҷ� ����

#define LCD_SHOW_MODE_GRAY_4_GRAY_0			0x00//4�׻Ҷ� 0����ʾ
#define LCD_SHOW_MODE_GRAY_4_GRAY_1			0x08//4�׻Ҷ� 1����ʾ
#define LCD_SHOW_MODE_GRAY_4_GRAY_2			0x06//4�׻Ҷ� 2����ʾ
#define LCD_SHOW_MODE_GRAY_4_GRAY_3			0x07//4�׻Ҷ� 3����ʾ
#define LCD_SHOW_MODE_GRAY_4_GRAY_4			0x0f//4�׻Ҷ� 4����ʾ

#define LCD_SHOW_MODE_GRAY_4_BKG_GRAY_0			0x00//4�׻Ҷ� 0�ױ���
#define LCD_SHOW_MODE_GRAY_4_BKG_GRAY_1			0x80//4�׻Ҷ� 1�ױ���
#define LCD_SHOW_MODE_GRAY_4_BKG_GRAY_2			0x60//4�׻Ҷ� 2�ױ���
#define LCD_SHOW_MODE_GRAY_4_BKG_GRAY_3			0x70//4�׻Ҷ� 3�ױ���
#define LCD_SHOW_MODE_GRAY_4_BKG_GRAY_4			0xf0//4�׻Ҷ� 4�ױ���


typedef struct
{
	#define ST7302_TYPE_GRAY2 0
	#define ST7302_TYPE_GRAY4 1
	
	uint16_t xs;
	uint16_t ys;
	uint16_t xlen;
	uint16_t ylen;
	uint8_t* buffer;
	
	uint8_t pictype;
	uint8_t showtype;
	uint8_t showmode;
}st7302_pic_t;

void st7302_begin(void);
void st7302_clear(void);
void st7302_clearbuffer(void);
void st7302_inversion_on(void);
void st7302_inversion_off(void);
void st7302_display_on(void);
void st7302_display_off(void);
void st7302_flushBuffer(void);
void st7302_flushPartial(uint16_t xs,uint16_t ys,uint16_t xlen,uint16_t ylen);
void st7302_writecache_2gray(uint16_t xs,uint16_t ys,uint16_t xlen,uint16_t ylen,uint8_t* data,uint8_t mode);
void st7302_writecache_4gray(uint16_t xs,uint16_t ys,uint16_t xlen,uint16_t ylen,uint8_t* data,uint8_t mode,uint8_t bkup);
void st7302_clearcache_2gray(uint16_t xs,uint16_t ys,uint16_t xe,uint16_t ye,uint8_t mode);
void st7302_clearcache_4gray(uint16_t xs,uint16_t ys,uint16_t xe,uint16_t ye,uint8_t bkgd);
void st7302_drawline(uint16_t xs,uint16_t ys,uint16_t len,uint16_t width,uint8_t mode);
void st7302_drawline_4gray(uint16_t xs,uint16_t ys,uint16_t len,uint16_t width,uint8_t mode);
void st7302_drawrect(uint16_t xs,uint16_t ys,uint16_t xe,uint16_t ye,uint16_t width,uint8_t mode);
void st7302_drawrect_4gray(uint16_t xs,uint16_t ys,uint16_t xe,uint16_t ye,uint16_t width,uint8_t mode);
void st7302_drawpicture_full(st7302_pic_t* pic);
void st7302_drawpicture_partial(st7302_pic_t* pic);


void LCD_SPI_Init(void);
void LCD_SPI_Exit(void);


#define LCD_Clear() 			st7302_clear()
#define LCD_Inversion_ON() 		st7302_inversion_on()
#define LCD_Inversion_OFF() 	st7302_inversion_off()

#define LCD_Display_ON() 		st7302_display_on()
#define LCD_Display_OFF() 		st7302_display_off()

#define LCD_DrawPicture_Full 	st7302_drawpicture_full
#define LCD_DrawPicture_Partial 	st7302_drawpicture_partial


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
#define LCD_DrawRect(XS,YS,XE,YE,WIDTH,MODE) 			st7302_drawrect((XS),(YS),(XE),(YE),(WIDTH),(MODE))

/**********************************************************************

	XS:[0,CACHE_SIZE_WIDTH)
	YS:[0,CACHE_SIZE_HEIGHT*2)
	XE:[0,CACHE_SIZE_WIDTH)
	YE:[0,CACHE_SIZE_HEIGHT*2)
	WIDTH:line width(4 gray)
	MODE:[LCD_MODE_GRAY_0,LCD_MODE_GRAY_4] line gray
	
**********************************************************************/
#define LCD_DrawRect_4Gray(XS,YS,XE,YE,WIDTH,MODE) 	st7302_drawrect_4gray((XS),(YS),(XE),(YE),(WIDTH),(MODE))

/**********************************************************************

	Flash all data to lcd
	
**********************************************************************/
#define LCD_Flush()										st7302_flushBuffer()

/**********************************************************************

	XS:[0,CACHE_SIZE_WIDTH*2)
	YS:[0,CACHE_SIZE_HEIGHT*4)
	XLEN:X bit width
	YLEN:Y bit height
	
**********************************************************************/
#define LCD_FlushPartial(xs,ys,xlen,ylen)		st7302_flushPartial(xs,ys,xlen,ylen)

/**********************************************************************

	XS:[0,CACHE_SIZE_WIDTH)
	YS:[0,CACHE_SIZE_HEIGHT*2)
	XLEN:X bit width(4 gray)
	YLEN:Y bit height(4 gray)
	
**********************************************************************/
#define LCD_FlushPartial_4gray(xs,ys,xlen,ylen)		st7302_flushPartial((xs)*2,(ys)*2,(xlen)*2,(ylen)*2)


#endif


