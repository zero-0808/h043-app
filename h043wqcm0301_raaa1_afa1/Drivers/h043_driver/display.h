#ifndef __DISPLAY_H__
#define __DISPLAY_H__


//#include "lpc_types.h"
#include "gui.h"
#include "hmi_config.h"
#include "stdint.h"

#if   (TFT_TYPE_SIZE == TFT_TYPE_4_3)

/* LCD Config */
#define LCD_W_SIZE           480
#define LCD_W_PULSE          41
#define LCD_W_FRONT_PORCH    2
#define LCD_W_BACK_PORCH     2
#define LCD_H_SIZE           272
#define LCD_H_PULSE          10
#define LCD_H_FRONT_PORCH    2
#define LCD_H_BACK_PORCH     2
#define LCD_PIX_CLK          (9000000)
#define LCD_PER_PIX          16

#elif (TFT_TYPE_SIZE == TFT_TYPE_5_0)


#define LCD_W_SIZE           800
#define LCD_W_PULSE          10
#define LCD_W_FRONT_PORCH    210
#define LCD_W_BACK_PORCH     46
#define LCD_H_SIZE           480
#define LCD_H_PULSE          10
#define LCD_H_FRONT_PORCH    22
#define LCD_H_BACK_PORCH     23
#define LCD_PIX_CLK          (30000000)
#define LCD_PER_PIX          16



#elif (TFT_TYPE_SIZE == TFT_TYPE_7_0)


#define LCD_W_SIZE           800
#define LCD_W_PULSE          2
#define LCD_W_FRONT_PORCH    17
#define LCD_W_BACK_PORCH     45
#define LCD_H_SIZE           480
#define LCD_H_PULSE          2
#define LCD_H_FRONT_PORCH    22
#define LCD_H_BACK_PORCH     22
#define LCD_PIX_CLK          (16000000)
#define LCD_PER_PIX          16

#endif



typedef  struct disp_device
{
	void (*init)(void);
	uint8_t * (*get_addr)(uint8_t);
	void (*put_pixel)(uint32_t,uint32_t,uint32_t);
	void (*put_pixel_buffer)(uint32_t,uint32_t,uint32_t,uint8_t * );
	uint16_t (*read_pixel_buffer)(uint32_t,uint32_t,uint8_t *);
	void (*fill_rect)(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
	void (*fill_screen)(uint32_t);
	void (*draw_cross)(uint16_t,uint16_t,uint16_t,uint32_t);
	void (*switch_base_addr)(void);
	void (*set_light)(uint8_t);
	void (*open_backlight)(void);
	void (*close_backlight)(void);
	dc disp[4];
	uint32_t width0;
	uint32_t height0;
	uint32_t width;
	uint32_t height;
	uint8_t dir;
	uint8_t bpp;
	uint8_t flag;
}disp_device;


extern disp_device disp_dev;


#define LCD_PWR_ENA_DIS_DLY  10000
#define LCD_FUNC_OK  0
#define LCD_FUNC_ERR -1

#define GET_CURSOR_IMG_SIZE(size)  ((size == 32) ? 64:256)

/**
 * @}
 */

/** @defgroup LCD_Public_Types LCD Public Types
 * @{
 */

typedef enum __LCD_TYPES
{
  LCD_STN_MONOCHROME,
  LCD_STN_COLOR,
  LCD_TFT,
  LCD_TYPE_UNKNOWN,
} LCD_TYPES;

typedef enum __LCD_BPP
{
  LCD_BPP_1 = 0x00,
  LCD_BPP_2 = 0x01,
  LCD_BPP_4 = 0x02,
  LCD_BPP_8 = 0x03,
  LCD_BPP_16 = 0x04,
  LCD_BPP_24 = 0x05,
  LCD_BPP_16_565Mode = 0x06,
  LCD_BPP_12_444Mode = 0x07,
} LCD_BPP;

typedef enum __LCD_PANEL
{
  LCD_PANEL_UPPER,
  LCD_PANEL_LOWER,
}LCD_PANEL;

/**
 * @brief A struct for Bitmap on LCD screen
 */
typedef struct _Bmp_t
{
  uint32_t  H_Size;
  uint32_t  V_Size;
  uint32_t  BitsPP;
  uint32_t  BytesPP;
  uint8_t *pPalette;
  uint8_t *pPicStream;
  uint8_t *pPicDesc;
} Bmp_t, *pBmp_t;

/**
 * @brief A struct for Font Type on LCD screen
 */

typedef struct _FontType_t
{
  uint32_t H_Size;
  uint32_t V_Size;
  uint32_t CharacterOffset;
  uint32_t CharactersNuber;
  uint8_t *pFontStream;
  uint8_t *pFontDesc;
} FontType_t, *pFontType_t;

/**
 * @brief A struct for LCD Palette
 */

typedef struct __LCD_PALETTE_Type
{
    uint8_t Red;
    uint8_t Green;
    uint8_t Blue;
}LCD_PALETTE_Type, *pLCD_PALETTE_Type;

/**
 * @brief A struct for Horizontal configuration
 */

typedef struct __LCD_HConfig_Type
{
  uint8_t hfp;  // Horizontal front porch
  uint8_t hbp;  // Horizontal back porch
  uint8_t hsw;  // Horizontal synchronization pulse width
  uint16_t ppl;  // Number of pixels per line
}LCD_HConfig_Type;

/**
 * @brief A struct for Vertical configuration
 */

typedef struct __LCD_VConfig_Type
{
  uint8_t vfp;  // Vertical front and back porch
  uint8_t vbp;  // Vertical back porch
  uint8_t vsw;  // Vertical synchronization pulse width
  uint16_t lpp; // Number of lines per panel
}LCD_VConfig_Type;

/**
 * @brief A struct for Polarity configuration
 */

typedef struct __LCD_POLARITY_Type
{
	uint16_t cpl; // Number of pixel clocks per line
	uint8_t  active_high; // Signal polarity, active HIGH or LOW
	uint8_t acb;  // AC bias pin frequency
    uint8_t  invert_panel_clock;   // Invert Panel clock;
    uint8_t  invert_hsync;          // Invert HSYNC
    uint8_t  invert_vsync;          // Invert VSYSNC
}LCD_POLARITY_Type;

/**
 * @brief A struct for LCD Configuration
 */

typedef struct __LCD_Config_Type
{
  LCD_HConfig_Type hConfig; // Horizontal config
  LCD_VConfig_Type vConfig; // Vertical config
  LCD_POLARITY_Type polarity; // Polarity config
  uint32_t panel_clk; // Panel clock frequency
  LCD_BPP lcd_bpp;   // Bits-per-pixel
  LCD_TYPES lcd_type; // Display type: STN monochrome, STN color, or TFT
  uint8_t   lcd_mono8; // STN 4 or 8-bit interface mode
  uint8_t   lcd_dual;  // STN dual or single panel mode
  uint8_t   big_endian_byte; // byte ordering in memory
  uint8_t   big_endian_pixel; // pixel ordering within a byte
  uint32_t  lcd_panel_upper;  // base address of frame buffer
  uint32_t  lcd_panel_lower;  // base address of frame buffer
  uint8_t*  lcd_palette;      // point to palette buffer
  uint8_t		lcd_bgr;		  // False: RGB , TRUE: BGR
} LCD_Config_Type;

/**
 * @brief A struct for Cursor configuration
 */

typedef struct __LCD_Cursor_Config_Type
{
   uint8_t size32;              // 32x32 or 64x64
   uint8_t framesync;
   LCD_PALETTE_Type palette[2];
   uint32_t   baseaddress;
} LCD_Cursor_Config_Type;

typedef uint32_t LcdPixel_t, *pLcdPixel_t;
typedef int32_t  LCD_RET_CODE;

/**
 * @}
 */
extern void open_backlight(void);
extern void close_backlight(void);

/** @defgroup LCD_Public_Functions LCD Public Functions
 * @{
 */




#endif









