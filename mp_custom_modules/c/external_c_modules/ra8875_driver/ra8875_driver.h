// Include MicroPython API.
#include "py/runtime.h"
#include "py/obj.h"

// ESP-IDF APIs
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <stdint.h>

/*********************************************************************************************************/
/*********************************************************************************************************/
/*********************************************************************************************************/
/******************************************MACRO FOR REGISTERS********************************************/
/*********************************************************************************************************/
/*********************************************************************************************************/
// Panel Settings (Choose only one by uncomment the selected size)
// #define RA8875_480x80
// #define RA8875_480x128
// #define RA8875_480x272
#define RA8875_800x480

// For BMP Drawing
#define BUFFPIXEL 20

// Command SPI
#define RA8875_DATAWRITE                  0x00
#define RA8875_DATAREAD                   0x40
#define RA8875_CMDWRITE                   0x80
#define RA8875_CMDREAD                    0xC0   

// Colors (RGB565) - Color Depth 64K
#define RA8875_BLACK                    0x0000   
#define RA8875_BLUE                     0x001F   
#define RA8875_RED                      0xF800   
#define RA8875_GREEN                    0x07E0   
#define RA8875_CYAN                     0x07FF   
#define RA8875_MAGENTA                  0xF81F   
#define RA8875_YELLOW                   0xFFE0   
#define RA8875_WHITE                    0xFFFF   
#define RA8875_TOSCA                    0x6DF1

/****************************************************************************
                          System & Configuration Registers
*****************************************************************************/
// Power & Display Register - bit -> 7: Display ON/Off, 6-2: NA, 1: Sleep Mode, 0: Software Reset
#define RA8875_PWRR_ADDR                0x01
#define RA8875_PWRR_DISPLAY_ON          0x80
#define RA8875_PWRR_DISPLAY_OFF         0x00    
#define RA8875_PWRR_DISPLAY_SLEEP       0x02
#define RA8875_PWRR_DISPLAY_NORMAL      0x00
#define RA8875_PWRR_DISPLAY_RESET       0x01

// Memory Read/Write - bit -> 3-2: Data to write/read
#define RA8875_MRWC_ADDR                0x02    

// Pixel Clock Setting - bit -> 7: PCLK Inversion (Rising/Falling Edge), 6-2: NA, 1-0: Period Setting  
#define RA8875_PCSR_ADDR                0x04    
#define RA8875_PCSR_CLK                 0x00
#define RA8875_PCSR_2CLK                0x01
#define RA8875_PCSR_4CLK                0X02
#define RA8875_PCSR_8CLK                0x03
#define RA8875_PCSR_PDAT_RISING         0x00
#define RA8875_PCSR_PDAT_FALLING        0x80

// Serial flash/ROM Config - bit 7: Flash Select, 6: Flash Address, 5: Flash Waveform mode, 4-3: Flash Read Cycle, 2: Acess Mode, 1-0: Flash Data Latch Mode Select
#define RA8875_SROC_ADDR                0x05
#define RA8875_SROC_DMA_MODE            0x04
#define RA8875_SRIC_4_BUS               0x00
#define RA8875_SROC_5_BUS               0x08
#define RA8875_DUAL_MODE_0              0x02
#define RA8875_DUAL_MODE_1              0x03
#define RA8875_SFCLR_ADDR               0x06

// System Configuration - bit -> 7-4: NA, 3-2: Color Depth, 1-0: MUCIF Selection
#define RA8875_SYSR_ADDR                0x10
#define RA8875_SYSR_COLOR_8BPP          0x00
#define RA8875_SYSR_COLOR_16BPP         0x0C
#define RA8875_SYSR_MCU_8BIT            0x00
#define RA8875_SYSR_MCU_16BIT           0x03

// LCD Horizontal Display Width Register - bit -> 7: NA, 6-0: Horizontal Width Setting (max. 0x64 = 800 )
#define RA8875_HDWR_ADDR                0x14

// Horizontal Non-Display Period Fine Tuning Option
#define RA8875_HNDFTR_ADDR              0x15
#define RA8875_HNDFTR_DE_HIGH           0x00
#define RA8875_HNDFTR_DE_LOW            0x80

#define RA8875_HNDR_ADDR                0x16    // LCD Horizontal Non-Display Period - bit -> 7-5: NA, 4-0: Horizontal Non-Display Period
#define RA8875_HSTR_ADDR                0x17    // HSYNC Start Position Register - bit -> 7-5: NA, 4-0: HSYNC Start Position

// Hysnc Pulse Width - bit -> 7: HSYNC Polarity, 6-5: NA, 4-0: HSYNC Pulse Width
#define RA8875_HPWR_ADDR                0x18     
#define RA8875_HPWR_POLARITY_HIGH       0x80
#define RA8875_HPWR_POLARITY_LOW        0X00

#define RA8875_VDHR0_ADDR               0X19   // LCD Vertical Display Height Register - bit -> 7-0: Vertical Display Height
#define RA8875_VDHR1_ADDR               0x1A   // LCD Vertical Display Height Register0 - bit -> 7-1: NA, 0: Vertical Display Height (max. 0x1E = 480)
#define RA8875_VNDR0_ADDR               0X1B   // LCD Vertical Non-Display Period Register - bit -> 7-0: Vertical Non-Display Periode
#define RA8875_VNDR1_ADDR               0x1C   // LCD Vertical Non-Display Period Register - bit -> 7-1: NA, 0: Vertical Non-Display Bit
#define RA8875_VSTR0_ADDR               0x1D   // VSYNC Start Position - bit -> 7-0: VSYNC Start Position
#define RA8875_VSTR1_ADDR               0x1E   // VSYNC Start Position - bit -> 7-1: NA, 0: VSYNC Start Position

// VSYNC Pulse Width - bit -> 7: VSYNC Polarity, 6-0: VSYNC Pulse Width
#define RA8875_VPWR_ADDR                0x1F   
#define RA8875_VPWR_POLARITY_HIGH       0x80
#define RA8875_VPWR_POLARITY_LOW        0X00

/****************************************************************************
                        LCD Display Control Register
*****************************************************************************/
// Display Configuration Register - bit -> 7: Layer Setting Control, 6-4: NA, 3: Horizontal Scan Direction, 2: Vertical Scan Direction, 1-0: NA
#define RA8875_DPCR_ADDR                0x20
#define RA8875_DPCR_LAYER1              0x00
#define RA8875_DPCR_LAYER2              0x80
#define RA8875_DPCR_HDIR_INC            0x00
#define RA8875_DPCR_HDIR_DEC            0x08
#define RA8875_DPCR_VDIR_INC            0x00
#define RA8875_DPCR_VDIR_DEC            0X04

// Font Control Register1 - bit -> 7: Full Alignment Selection Bit, 6: Font Transparency, 5: NA, 4: Font Rotation, 3-2: Horizontal Font Enlargement, 1-0: Vertical Font Enlargement
#define RA8875_FNCR1_ADDR               0x22
#define RA8875_FNCR1_EN_ALIGNMENT       0x80
#define RA8875_FNCR1_DIS_ALIGNMENT      0x00
#define RA8875_FNCR1_BG_COLOR           0x00
#define RA8875_FNCR1_BG_TRANSPARENT     0x40 
#define RA8875_FNCR1_EN_ROTATION        0x10
#define RA8875_FNCR1_DIS_ROTATION       0x00
#define RA8875_FNCR1_HOR_FONT_ENLARGED1 0x00
#define RA8875_FNCR1_HOR_FONT_ENLARGED2 0x04
#define RA8875_FNCR1_HOR_FONT_ENLARGED3 0x08
#define RA8875_FNCR1_HOR_FONT_ENLARGED4 0x0C
#define RA8875_FNCR1_VER_FONT_ENLARGED1 0x00
#define RA8875_FNCR1_VER_FONT_ENLARGED2 0x01
#define RA8875_FNCR1_VER_FONT_ENLARGED3 0x02
#define RA8875_FNCR1_VER_FONT_ENLARGED4 0x03

#define RA8875_DPCR_ADDR                0x20  // Display configuration - bit 7: Layer setting control, 6-4: NA, 3: Horizontal Scan Direction, 2: Vertical Scan Direction, 1-0: NA
#define RA8875_FNCR0_ADDR               0x21  // Font control register 0 - bit 7: CGRAM/CGROM font selection, 6: NA, 5: External/Internal CGROM selection, 4-2: NA, 1-0: Font Selection 
#define RA8875_FNCR0_INTERNAL_CGROM     0x00  // Bit 5: 0, 7:0 for selecting internal CGROM
#define RA8875_FNCR0_SELECT_CGRAM       0x80  // Bit 7: 1 for selecting CGRAM Font
#define RA8875_FNCR1_ADDR               0x22  // Font control register 1 - bit 7: Full allignemtn, 6: font transparency, 5: NA, 4: Font rotation, 3-2: Horizontal enlargement, 1-0: Vertical englargement 
#define RA8875_CGSR_ADDR                0x23  // CGRAM Select register - bit 7-0: Setting number of the character in CGRAM, for user-defined character bitmap
#define RA8875_HOFS0_ADDR               0x24  // Horizontal Display Scroll Offset Register 0- bit -> 7-0: Horizontal Display Scroll
#define RA8875_HOFS1_ADDR               0x25  // Horizontal Scroll Offset Register 1 - bit -> 7-3: NA, 2-0: Horizontal Display Scroll Offset
#define RA8875_VOFS0_ADDR               0x26  // Vertical Scroll Offset Register 0 - bit -> 7-0: Vertical Display Scroll
#define RA8875_VOFS1_ADDR               0x27  // Vertical Scroll Offset Register 1 - bit -> 7-2: NA, 1-0: Vertical Display Scroll Offset
#define RA8875_FLDR_ADDR                0x29  // Font Line Distance Setting Register - bit -> 7-5: NA, 4-0: Font Line Distance Setting 
#define RA8875_F_CURXL_ADDR             0x2A  // Font Write Cursor Horizontal Position Register 0 - bit -> 7-0: Font Write Cursor Horizontal Position
#define RA8875_F_CURXH_ADDR             0x2B  // Font Write Cursor Horizontal Position Register 1 - bit -> 7-2: NA, 1-0: Font Write Cursor Horizontal Position
#define RA8875_F_CURYL_ADDR             0x2C  // Font Write Cursor Vertical Position Register 0 - bit -> 7-0: Font Write Cursor Vertical Position
#define RA8875_F_CURYH_ADDR             0x2D  // Font Write Cursor Vertical Position Register 1 - bit -> 7-1: NA, 0: Font Write Cursor Vertical Position

// Font Write Type Setting Register - bit -> 7-6: Font Size Setting, 5-0: Font to font width setting
#define RA8875_FONT_TYPE_ADDR           0x2E
#define RA8875_FONT_TYPE_SIZE_16X16     0x00
#define RA8875_FONT_TYPE_SIZE_24X24     0x40
#define RA8875_FONT_TYPE_SIZE_32X32     0xC0
#define RA8875_FONT_TYPE_WIDTH_0        0x00
#define RA8875_FONT_TYPE_WIDTH_5        0x05
#define RA8875_FONT_TYPE_WIDTH_10       0x0A

/****************************************************************************
                          Active Window & Scroll Window Setting
*****************************************************************************/
#define RA8875_HSAW0_ADDR               0x30  // Horizontal Start Point 0 of Active Window - bit 7-0
#define RA8875_HSAW1_ADDR               0x31  // Horizontal Start Point 1 of Active Window - bit -> 7-2: NA, 1-0: Horizontal Start Point of Active Window
#define RA8875_VSAW0_ADDR               0x32  // Vertical Start Point 0 of Active Window - bit 7-0
#define RA8875_VSAW1_ADDR               0x33  // Vertical Start Point 1 of Active Window - bit -> 7-1: NA, 0: Vertical Start Point of Active Window
#define RA8875_HEAW0_ADDR               0x34  // Horizontal End Point 0 of Active Window - bit 7-0
#define RA8875_HEAW1_ADDR               0x35  // Horizontal End Point 1 of Active Window - bit 7-2: NA, 1-0: Horizontal End point of Active Window
#define RA8875_VEAW0_ADDR               0x36  // VERTICAL End Point 0 of Active Window - bit 7-0
#define RA8875_VEAW1_ADDR               0x37  // VERTICAL End Point 1 of Active Window - bit 7-1: NA, 0: Vertical End point of Active Window

#define RA8875_HSSW0_ADDR               0x38  // Horizontal Start Point 0 of Scroll Window - bit 7-0
#define RA8875_HSSW1_ADDR               0x39  // Horizontal Start Point 1 of Scroll Window - bit -> 7-2: NA, 1-0: Horizontal Start Point of Scroll Window
#define RA8875_VSSW0_ADDR               0x3A  // Vertical Start Point 0 of Scroll Window - bit 7-0
#define RA8875_VSSW1_ADDR               0x3B  // Vertical Start Point 1 of Scroll Window - bit -> 7-1: NA, 0: Vertical Start Point of Scroll Window
#define RA8875_HESW0_ADDR               0x3C  // Horizontal End Point 0 of Scroll Window - bit 7-0
#define RA8875_HESW1_ADDR               0x3D  // Horizontal End Point 1 of Scroll Window - bit -> 7-2: NA, 1-0: Horizontal End Point of Scroll Window
#define RA8875_VESW0_ADDR               0x3E  // Vertical End Point 0 of Scroll Window - bit 7-0
#define RA8875_VESW1_ADDR               0x3F  // Vertical End Point 1 of Scroll Window - bit -> 7-1: NA, 0: Vertical End Point of Scroll Window

/****************************************************************************
                          Cursor Setting Register
*****************************************************************************/

/* Memory Write Control Register 0 - bit -> 7: Text/Graphic Mode, 6: Font Write Cursor, 5: Font Write Cursor Blink, 4: NA, 3-2: Memory Write Direction for Graphic, 
                                            1: Write Cursor Auto-Increase Disable, 0: Read Cursor Auto-Increase Disable*/
#define RA8875_MWCR0_ADDR               0x40
#define RA8875_MWCR0_GRAPHIC_MODE       0x00 
#define RA8875_MWCR0_TEXT_MODE          0x80
#define RA8875_MWCR0_CURSOR_VISIBLE     0x40
#define RA8875_MWCR0_CURSOR_INVISIBLE   0x00
#define RA8875_MWCR0_CURSOR_EN_BLINK    0x20
#define RA8875_MWCR0_CURSOR_DIS_BLINK   0x00    
#define RA8875_MWCR0_DIRMASK            0x0C    ///< Bitmask for Write Direction
#define RA8875_MWCR0_LRTD               0x00    ///< Left->Right then Top->Down
#define RA8875_MWCR0_RLTD               0x04    ///< Right->Left then Top->Down
#define RA8875_MWCR0_TDLR               0x08    ///< Top->Down then Left->Right
#define RA8875_MWCR0_DTLR               0x0C    ///< Down->Top then Left->Right    

/*  Memory Write Control Register 1 - bit -> 7: Graphic Cursor Enable, 6-4: Graphic Cursor Selection Bit, 3-2: Write Destination Selection, 1: NA, 0: Layer Selection*/
#define RA8875_MWCR1_ADDR               0x41
#define RA8875_MWCR1_GRAPHIC_CURSOR_EN  0x80
#define RA8875_MWCR1_GRAPHIC_CURSOR_DIS 0x00
#define RA8875_MWCR1_GRAPHIC_CURSOR_1   0x00
#define RA8875_MWCR1_GRAPHIC_CURSOR_3   0x20
#define RA8875_MWCR1_GRAPHIC_CURSOR_5   0x40
#define RA8875_MWCR1_GRAPHIC_CURSOR_7   0x60
#define RA8875_MWCR1_DEST_LAYER_1_2     0x00
#define RA8875_MWCR1_DEST_CGRAM         0x04
#define RA8875_MWCR1_DEST_GRAPHIC_CUR   0x08
#define RA8875_MWCR1_DEST_PATTERN       0x0C
#define RA8875_MWCR1_SELECT_LAYER_1     0x00
#define RA8875_MWCR1_SELECT_LAYER_2     0x01

#define RA8875_BTCR_ADDR                0x44    // Blink Time Control Register - bit -> 7-0: Text Blink Time Setting (00-FF = 0-256 Frames)
#define RA8875_MRCD_ADDR                0x45    // Memory Read Cursor DIrection - bit -> 7-2: NA, 1-0: Memory Read Direction for Graphic Mode
#define RA8875_CURH0_ADDR               0x46    // Memory Write Cursor Horizontal Position Register 0 - bit 7-0: Cursor Horizontal location
#define RA8875_CURH1_ADDR               0x47    // Memory Write Cursor Horizontal Position Register 1 - bit 7-2: NA, 1-0: Cursor Horizontal location
#define RA8875_CURV0_ADDR               0x48    // Memory Write Cursor Vertical Position Register 0 - bit 7-0: Cursor Vertical location
#define RA8875_CURV1_ADDR               0x49    // Memory Write Cursor Vertical Position Register 1 - bit 7-1: NA, 0: Cursor Vertical location

/****************************************************************************
                    Block Transfer Engine Control Register
*****************************************************************************/

// BTE Function Control Register 0  - bit 7: BTE Function Enable/Status, 6: BTE Source Data, 5: BTE Destination Data Type, 4-0: NA
#define RA8875_BECR0_ADDR               0x50
#define RA8875_BECR0_ENABLE             0x80
#define RA8875_BECR0_SOURCE_BLOCK       0x00
#define RA8875_BECR0_SOURCE_LINEAR      0x40
#define RA8875_BECR0_DESTINATION_BLOCK  0x00
#define RA8875_BECR0_DESTINATION_LINEAR 0x20

// BTE Function Control Register 1 - bit 7-4: BTE ROP Code Bit[3:0], 3-0: BTE Operation Code Bit[3:0]
#define RA8875_BECR1_ADDR                   0x51
#define RA8875_OPERATION_WRITE_ROP          0x00
#define RA8875_OPERATION_WRTIE_TRANSPARENT  0x04
#define RA8875_OPERATION_SOLID_FILL         0x0C
#define RA8875_ROP_S_EQUAL_D                0xC0
#define RA8875_ROP_S_OR_D                   0xE0
#define RA8875_ROP_INVS_MULTI_D             0x20
#define RA8875_ROP_D_EQUAL_D                0xA0

// Layer Transparency Register 0 - bit -> 7-6: Layer 1/2 Scroll Mode, 5: Floating Windows Transparency, 4-3: NA, 2-0: Layer 1/2 Display Mode
#define RA8875_LTPR0_ADDR               0x52
#define RA8875_LTPR0_SCROLL_SIMUL       0x00
#define RA8875_LTPR0_SCROLL_LAYER1      0x40
#define RA8875_LTPR0_SCROLL_LAYER2      0x80
#define RA8875_LTPR0_SCROLL_BUFFER      0xC0
#define RA8875_LTPR0_EN_FLOATING        0x20
#define RA8875_LTPR0_DIS_FLOATING       0x00
#define RA8875_LTPR0_ONLY_LAYER1        0x00
#define RA8875_LTPR0_ONLY_LAYER2        0x01
#define RA8875_LTPR0_LIGHTEN_OVERLAY    0x02
#define RA8875_LTPR0_TRANSPARENT        0x03

// Layer Transparency Register 1 - bit -> 7-4: Layer Transparency for Layer 2, 3-0: Layer Transparency for Layer 1
#define RA8875_LTPR1_ADDR               0x53
#define RA8875_LTPR1_L2_TOTAL_DISP      0x00
#define RA8875_LTPR1_L2_HALF_DISP       0x40
#define RA8875_LTPR1_L2_DIS_DISP        0x80
#define RA8875_LTPR1_L1_TOTAL_DISP      0x00
#define RA8875_LTPR1_L1_HALF_DISP       0x04
#define RA8875_LTPR1_L1_DIS_DISP        0x08

// BTE Source Point
#define RA8875_HSBE0_ADDR               0x54  // Horizontal Source Point 0 of BTE - Bit 7-0: Horizontal Source Point of BTE[7:0]
#define RA8875_HSBE1_ADDR               0x55  // Horizontal Source Point 1 of BTE - Bit 7-2: NA, 1-0: Horizontal Source Point of BTE[9:8]
#define RA8875_VSBE0_ADDR               0x56  // Vertical Source Point 0 of BTE - Bit 7-0: Vertical Source Point of BTE[7:0]
#define RA8875_VSBE1_ADDR               0x57  // Vertical Source Point 1 of BTE - Bit 7: BTE Source Layer Selection, 6-1: NA, 0: Vertical Source Point of BTE[8]

// BTE Destination Point
#define RA8875_HDBE0_ADDR               0x58  // Horizontal Destination Point 0 of BTE - Bit 7-0: Horizontal Destination Point of BTE[7:0]
#define RA8875_HDBE1_ADDR               0x59  // Horizontal Destination Point 1 of BTE - Bit 7-2: NA, 1-0: Horizontal Destination Point of BTE[9:8]
#define RA8875_VDBE0_ADDR               0x5A  // Vertical Destination Point 0 of BTE - Bit 7-0: Vertical Destination Point of BTE[7:0]
#define RA8875_VDBE1_ADDR               0x5B  // Vertical Destination Point 1 of BTE - Bit 7: BTE Destination Layer Selection, 6-1: NA, 0: Vertical Destination Point of BTE[8]

// BTE Width & Height Register
#define RA8875_BEWR0_ADDR               0x5C  // BTE Width Register 0 - Bit 7-0: BTE Width Setting[7:0]
#define RA8875_BEWR1_ADDR               0x5D  // BTE Width Register 1 - Bit 7-2: NA, 1-0: BTE Width Setting[9:8]
#define RA8875_BEHR0_ADDR               0x5E  // BTE Height Resigter 0 - Bit 7-0: BTE Height Setting[7:0]
#define RA8875_BEHR1_ADDR               0x5E  // BTE Height Resigter 1 - Bit 7-2: NA, 1-0: BTE Height Setting[9:8]

// BTE Color Register
#define RA8875_BGCR0_ADDR               0x60 // Background Color Register 0 - bit -> 7-5: NA, 4-0: Background Color Red
#define RA8875_BGCR1_ADDR               0x61 // Background Color Register 1 - bit -> 7-6: NA, 5-0: Background Color Green
#define RA8875_BGCR2_ADDR               0x62 // Background Color Register 2 - bit -> 7-5: NA, 4-0: Background Color Blue
#define RA8875_FGCR0_ADDR               0x63 // Foreground Color Register 0 - bit -> 7-5: NA, 4-0: Foreground Color Red
#define RA8875_FGCR1_ADDR               0x64 // Foreground Color Register 1 - bit -> 7-6: NA, 5-0: Foreground Color Green
#define RA8875_FGCR2_ADDR               0x65 // Foreground Color Register 2 - bit -> 7-5: NA, 4-0: Foreground Color Blue

// Pattern Set No for BTE - bit -> 7: Pattern Format, 6-4: NA, 3-0: Pattern Set No
#define RA8875_PTNO_ADDR                0x66 
#define RA8875_PTNO_8x8                 0x00
#define RA8875_PTN0_16X16               0x80

// BTE Background Color Register for Transparent
#define RA8875_BGTR0_ADDR               0x67  // Background color register for transparent 0 - Bit 7-5: NA, 4-0: Background color register for transparent Red[4:0]
#define RA8875_BGTR1_ADDR               0x68  // Background color register for transparent 1 - Bit 7-6: NA, 5-0: Background color register for transparent Green[5:0]
#define RA8875_BGTR2_ADDR               0x69  // Background color register for transparent 2 - Bit 7-5: NA, 4-0: Background color register for transparent Blue[4:0]

/****************************************************************************
                    Touch Panel Control Register
*****************************************************************************/

// Touch Panel Control Register 0 - bit -> 7: Touch Panel Enable, 6-4: TP Sample Time, 3: Wakeup Enable, 2-0: ADC Clock Setting
#define RA8875_TPCR0_ADDR               0x70               
#define RA8875_TPCR0_ENABLE             0x80        
#define RA8875_TPCR0_DISABLE            0x00       
#define RA8875_TPCR0_WAIT_512CLK        0x00   
#define RA8875_TPCR0_WAIT_1024CLK       0x10  
#define RA8875_TPCR0_WAIT_2048CLK       0x20  
#define RA8875_TPCR0_WAIT_4096CLK       0x30  
#define RA8875_TPCR0_WAIT_8192CLK       0x40  
#define RA8875_TPCR0_WAIT_16384CLK      0x50 
#define RA8875_TPCR0_WAIT_32768CLK      0x60 
#define RA8875_TPCR0_WAIT_65536CLK      0x70 
#define RA8875_TPCR0_WAKEENABLE         0x08    
#define RA8875_TPCR0_WAKEDISABLE        0x00   
#define RA8875_TPCR0_ADCCLK_DIV1        0x00   
#define RA8875_TPCR0_ADCCLK_DIV2        0x01   
#define RA8875_TPCR0_ADCCLK_DIV4        0x02   
#define RA8875_TPCR0_ADCCLK_DIV8        0x03   
#define RA8875_TPCR0_ADCCLK_DIV16       0x04  
#define RA8875_TPCR0_ADCCLK_DIV32       0x05  
#define RA8875_TPCR0_ADCCLK_DIV64       0x06  
#define RA8875_TPCR0_ADCCLK_DIV128      0x07 

// Touch Panel Control Register 1 - bit -> 7: NA, 6: TP Manual Enable, 5: TP ADC Reference Voltage, 4-3: NA, 2: De-bounce Circuit Enable, 1-0: Model Selection for TP Manual Mode
#define RA8875_TPCR1_ADDR               0x71 
#define RA8875_TPCR1_AUTO               0x00 
#define RA8875_TPCR1_MANUAL             0x40 
#define RA8875_TPCR1_VREFINT            0x00 
#define RA8875_TPCR1_VREFEXT            0x20 
#define RA8875_TPCR1_DEBOUNCE           0x04 
#define RA8875_TPCR1_NODEBOUNCE         0x00 
#define RA8875_TPCR1_IDLE               0x00 
#define RA8875_TPCR1_WAIT               0x01 
#define RA8875_TPCR1_LATCHX             0x02 
#define RA8875_TPCR1_LATCHY             0x03 

#define RA8875_TPXH_ADDR                0x72  // Touch Panel X High Byte Data Register  - bit -> 7-0: Touch Panel X Data Bit[9:2]
#define RA8875_TPYH_ADDR                0x73  // Touch Panel Y High Byte Data Register  - bit -> 7-0: Touch Panel Y Data Bit[9:2]
#define RA8875_TPXYL_ADDR               0x74  // Touch Panel X/Y Low Byte Data Register - BIT -> 7: Touch Event Detector, 6-4: NA, 3-2: Touch Panel Y, 1-0: Touch Panel X

/****************************************************************************
                    Graphic Cursor Setting Register
*****************************************************************************/
#define RA8875_GCHP0_ADDR               0x80  // Graphic Cursor Horizontal Position Register 0 - bit -> 7-0: Graphic Cursor Horizontal Location
#define RA8875_GCHP1_ADDR               0x81  // Graphic Cursor Horizontal Position Register 1 - bit -> 7-2: NA, 1-0: Graphic Cursor Horizoontal Location
#define RA8875_GCVP0_ADDR               0x82  // Graphic Cursor Vertical Position Register 0   - bit -> 7-0: Graphic Cursor Vertical Location
#define RA8875_GCVP1_ADDR               0x83  // Graphic Cursor Vertial Position Register 1    - bit -> 7-1: NA, 0: Graphic Cursor Vertical Location
#define RA8875_GCC0_ADDR                0x84  // Graphic Cursor Color 0                        - bit -> 7-0: Graphic Cursor COlor 0 with 256 Colors (RGB)
#define RA8875_GCC1_ADDR                0x85  // Graphic Cursor Color 1

/****************************************************************************
                    Graphic Cursor Setting Register
*****************************************************************************/

// PLL Control Register 1 - bit -> 7: PLLDIVM, 6-5: NA, 4-0: PLLDIVN
#define RA8875_PLLC1_ADDR               0x88  
#define RA8875_PLLC1_DIV_1              0x00
#define RA8875_PLLC1_DIV_2              0x80

// PLL Control Register 2 - bit -> 7-3: NA, 2-0: PLL Output divider
#define RA8875_PLLC2_ADDR               0x89       
#define RA8875_PLLC2_DIV1               0x00  
#define RA8875_PLLC2_DIV2               0x01  
#define RA8875_PLLC2_DIV4               0x02  
#define RA8875_PLLC2_DIV8               0x03  
#define RA8875_PLLC2_DIV16              0x04 
#define RA8875_PLLC2_DIV32              0x05 
#define RA8875_PLLC2_DIV64              0x06 
#define RA8875_PLLC2_DIV128             0x07

// PWM1 Control Register - bit -> 7: PWM1 Enable, 6: PWM1 Disable Level, 5: Reserved, 4: PWM1 Function Selection, 3-0: PWM1 Clock Source Divide Ratio
#define RA8875_P1CR_ADDR                0x8A        
#define RA8875_P1CR_ENABLE              0x80 
#define RA8875_P1CR_DISABLE             0x00
#define RA8875_P1CR_CLKOUT              0x10 
#define RA8875_P1CR_PWMOUT              0x00 
#define RA8875_P1DCR_ADDR               0x8B  // PWM1 Duty Cycle Register - bit 7-0: PWM Cycle Duty Selection Bit (00-FF)

// PWM2 Control Register - bit -> 7: PWM2 Enable, 6L PWM2 Disable Level, 5: Reserved, 4: PWM2 Function Selection, 3-0: PWM2 Clock Source Divide Ratio
#define RA8875_P2CR_ADDR                0x8C         
#define RA8875_P2CR_ENABLE              0x80  
#define RA8875_P2CR_DISABLE             0x00 
#define RA8875_P2CR_CLKOUT              0x10  
#define RA8875_P2CR_PWMOUT              0x00  
#define RA8875_P2DCR_ADDR               0x8D  // PWM1 Duty Cycle Register - bit 7-0: PWM Cycle Duty Selection Bit (00-FF)

// Memory Clear Control Register - bit -> 7: Memory Clear Function, 6: Memory Clear Area Setting, 5-0: NA
#define RA8875_MCLR_ADDR                0x8E            
#define RA8875_MCLR_START               0x80      
#define RA8875_MCLR_STOP                0x00       
#define RA8875_MCLR_READSTATUS          0x80 
#define RA8875_MCLR_FULL                0x00       
#define RA8875_MCLR_ACTIVE              0x40     

#define RA8875_PWM_CLK_DIV1             0x00     
#define RA8875_PWM_CLK_DIV2             0x01     
#define RA8875_PWM_CLK_DIV4             0x02     
#define RA8875_PWM_CLK_DIV8             0x03     
#define RA8875_PWM_CLK_DIV16            0x04    
#define RA8875_PWM_CLK_DIV32            0x05    
#define RA8875_PWM_CLK_DIV64            0x06    
#define RA8875_PWM_CLK_DIV128           0x07   
#define RA8875_PWM_CLK_DIV256           0x08   
#define RA8875_PWM_CLK_DIV512           0x09   
#define RA8875_PWM_CLK_DIV1024          0x0A  
#define RA8875_PWM_CLK_DIV2048          0x0B  
#define RA8875_PWM_CLK_DIV4096          0x0C  
#define RA8875_PWM_CLK_DIV8192          0x0D  
#define RA8875_PWM_CLK_DIV16384         0x0E 
#define RA8875_PWM_CLK_DIV32768         0x0F 

/****************************************************************************
                        Drawing Control Register
*****************************************************************************/

/* Draw Line/Circle/Square Control Register - bit -> 7: Draw Line/Square/Triangle Start Signal, 6: Draw Circle Start Signal, 5: Fill the Circle/Square/Triangle Signal, 
  4: Draw Line/Square Select Signal, 3-1: NA, 0: Draw Triangle or Line/Square Signal */
#define RA8875_DCR_ADDR               0x90                   
#define RA8875_DCR_LINESQUTRI_START   0x80  
#define RA8875_DCR_LINESQUTRI_STOP    0x00   
#define RA8875_DCR_LINESQUTRI_STATUS  0x80 
#define RA8875_DCR_CIRCLE_START       0x40      
#define RA8875_DCR_CIRCLE_STATUS      0x40     
#define RA8875_DCR_CIRCLE_STOP        0x00       
#define RA8875_DCR_FILL               0x20              
#define RA8875_DCR_NOFILL             0x00            
#define RA8875_DCR_DRAWLINE           0x00          
#define RA8875_DCR_DRAWTRIANGLE       0x01      
#define RA8875_DCR_DRAWSQUARE         0x10        

#define RA8875_DLHSR0_ADDR            0x91  // Draw Line/Square Horizontal Start Address Register0 - bit 7-0: Draw Line/Square Horizontal Start Address
#define RA8875_DLHSR1_ADDR            0x92  // Draw Line/Square Horizontal Start Address Register1 - bit 7-2: NA, 1-0: Draw Line/Square Horizontal Start Address
#define RA8875_DLVSR0_ADDR            0x93  // Draw Line/Square Vertical Start Address Register0   - bit 7-0: Draw Line/Square Vertical Start Address
#define RA8875_DLVSR1_ADDR            0x94  // Draw Line/Square Vertical Start Address Register1   - bit 7-1: NA, 0: Draw Line/Square Vertical Start Address
#define RA8875_DLHER0_ADDR            0x95  // Draw Line/Square Horizontal End Address Register0   - bit 7-0: Draw Line/Square Horizontal End Address
#define RA8875_DLHER1_ADDR            0x96  // Draw Line/Square Horizontal End Address Register1   - bit 7-2: NA, 1-0: Draw Line/Square Horizontal End Address
#define RA8875_DLVER0_ADDR            0x97  // Draw Line/Square Vertical End Address Register0     - bit 7-0: Draw Line/Square Vertical End Address
#define RA8875_DLVER1_ADDR            0x98  // Draw Line/Square Vertical End Address Register1     - bit 7-1: NA, 0: Draw Line/Square Vertical End Address
#define RA8875_DCHR0_ADDR             0x99  // Draw Circle Center Horizontal Address Register0     - bit 7-0: Draw Circle Center Horizontal Address
#define RA8875_DCHR1_ADDR             0x9A  // Draw Circle Center Horizontal Address Register1     - bit 7-2: NA, 1-0: Draw Circle Center Horizontal Address
#define RA8875_DCVR0_ADDR             0x9B  // Draw Circle Center Vertical Address Register0       - bit 7-0: Draw Circle Center Vertical Address
#define RA8875_DCVR1_ADDR             0X9C  // Draw Circle Center Vertical Address Register1       - bit 7-1: NA. 0: Draw Circle Center Vertical Address
#define RA8875_DCRR_ADDR              0x9D  // Draw Circle Radius Register - bit 7-0: Draw Circle Radius

/*Draw Ellipse/Ellipse Curve/Circle Square Control Register - bit 7: Draw Ellipse/Circle Square Start Signal, 6:Fill the Ellipse/Circle Square Signal, 
  5: Draw Ellipse/ Ellipse Curve or Circle Square Select Signal, 4: Draw Ellipse or Ellipse Curve Select Signal, 3-2: NA, 1-0: Draw Ellipse Curve Part Select(DECP)*/
#define RA8875_ELLIPSE_ADDR           0xA0
#define RA8875_ELLIPSE_START          0x80
#define RA8875_ELLIPSE_STOP           0x00
#define RA8875_ELLIPSE_STATUS         0x80
#define RA8875_ELLIPSE_FILL           0x40
#define RA8875_ELLIPSE_NOFILL         0x00
#define RA8875_CIRCLESQUARE_DRAW      0x20
#define RA8875_ELLIPSE_CURVE_DRAW     0x10
#define RA8875_ELLIPSE_DRAW           0x00      

#define RA8875_ELL_A0_ADDR            0xA1  // Draw Ellipse/Circle Square Long axis Setting Register - bit 7-0: Draw Ellipse/Circle Square Long axis
#define RA8875_ELL_A1_ADDR            0xA2  // Draw Ellipse/Circle Square Long axis Setting Register - bit 7-2: NA, 1-0: Draw Ellipse/Circle Square Long axis
#define RA8875_ELL_B0_ADDR            0xA3  // Draw Ellipse/Circle Square Short axis Setting Register - bit 7-0: Draw Ellipse/Circle Square Short axis
#define RA8875_ELL_B1_ADDR            0xA4  // Draw Ellipse/Circle Square Short axis Setting Register - bit 7-1: NA, 0: Draw Ellipse/Circle Square Short axis
#define RA8875_DEHR0_ADDR             0xA5  // Draw Ellipse/Circle Square Center Horizontal Address Register0 - bit 7-0: Draw Ellipse/Circle Square Center Horizontal Address
#define RA8875_DEHR1_ADDR             0xA6  // Draw Ellipse/Circle Square Center Horizontal Address Register1 - bit 7-2: NA, 1-0: Draw Ellipse/Circle Square Center Horizontal Address
#define RA8875_DEVR0_ADDR             0xA7  // Draw Ellipse/Circle Square Center Vertical Address Register0 - bit 7-0: Draw Ellipse/Circle Square Center Vertical Address
#define RA8875_DEVR1_ADDR             0xA8  // Draw Ellipse/Circle Square Center Vertical Address Register1 - bit 7-1: NA, 0: Draw Ellipse/Circle Square Center Vertical Address
#define RA8875_DTPH0_ADDR             0xA9  // Draw Triangle Point 2 Horizontal Address Register0 - bit 7-0: Draw Triangle Point 2 Horizontal Address
#define RA8875_DTPH1_ADDR             0xAA  // Draw Triangle Point 2 Horizontal Address Register1 - bit 7-2: NA, 1-0: Draw Triangle Point 2 Horizontal Address
#define RA8875_DTPV0_ADDR             0xAB  // Draw Triangle Point 2 Vertical Address Register0 - bit 7-0: Draw Triangle Point 2 Vertical Address
#define RA8875_DTPV1_ADDR             0xAC  // Draw Triangle Point 2 Vertical Address Register1 - bit 7-1: NA, 0: Draw Triangle Point 2 Vertical Address

/****************************************************************************
                              DMA Registers
*****************************************************************************/
#define RA8875_SSAR0_ADDR             0xB0  // Source Starting Address REG0 - bit 7-0: DMA Source START ADDRESS [7:0]
#define RA8875_SSAR1_ADDR             0xB1  // Source Starting Address REG1 - bit 7-0: DMA Source START ADDRESS [15:8]
#define RA8875_SSAR2_ADDR             0xB2  // Source Starting Address REG2 - bit 7-0: DMA Source START ADDRESS [23:16]
#define RA8875_BWR0_ADDR              0xB4  // Block width REG0 - bit 7-0: DMA Transfer number[0:7] / DMA Block width[7:0]
#define RA8875_BWR1_ADDR              0xB5  // Block width REG1 - bit 7-2: NA, 1-0: DMA Block Width [9:8]
#define RA8875_BHR0_ADDR              0xB6  // Block height REG0 - bit 7-0:  DMA Transfer number [15:8] or DMA Block Height[7:0]
#define RA8875_BHR1_ADDR              0xB7  // Block Height REG1 - bit 7-2: NA, 1-0: DMA Block Height [9:8]
#define RA8875_SPWR0_ADDR             0xB8  // Source Picture Width Reg 0 - 7-3: DMA Source Picture Width [7:3], 2-0: DMA Transfer Number[18:16] or DMA Source Picture Width[2:0]
#define RA8875_SPWR1_ADDR             0xB9  // Source Picture Width Reg 1 - 7-2: NA, 1-0: DMA Source Picture Width[9:8]
#define RA8875_DMACR_ADDR             0xBF  // DMA Configuration Reg - 7-2: NA, 1: Continuous/Block Mode, 0: Write Start Bit or Read DMA Busy Check Bit

/****************************************************************************
                          IO & Key Control Registers
*****************************************************************************/
#define RA8875_GPIOX_ADDR             0xC7  // Extra General Purpose IO Register - bit 7-1: NA, 0: GPIX/GPOX Data Bit

/****************************************************************************
                        Serial Flash Control Registers
*****************************************************************************/
#define RA8875_SACS_MODE_ADDR         0xE0  // Serial Flash/ROM Direct Access Mode
#define RA8875_SACS_MODE_DMA          0x00  // FONT/DMA Select
#define RA8875_SACS_MODE_DIRECT       0x01  // Direct Acess Mode selected
#define RA8875_SACS_ADDR              0xE1  // Serial Flash/ROM Direct Access Mode Address
#define RA8875_SACS_DATA              0xE2  // Serial Flash/ROM Direct Acess Data Read

/****************************************************************************
                          Interrupt Control Registers
*****************************************************************************/
/*Interrupt Control Register1 - bit 7-5: NA, 4: KEYSCAN Interrupt Enable Bit, 3: DMA Interrupt Enable Bit, 2: Touch Panel Interrupt Enable Bit, 
  1: BTE Process Complete Interrupt Enable Bit, 0: Enable the BTE Interrupt for MCU R/W or Enable the Interrupt of Font Write Function*/
#define RA8875_INTC1_ADDR             0xF0    
#define RA8875_INTC1_KEY              0x10
#define RA8875_INTC1_DMA              0x08
#define RA8875_INTC1_TP               0x04 
#define RA8875_INTC1_BTE              0x02

/*Interrupt Control Register2 - bit 7-5: NA, 4: KEYSCAN Interrupt Enable Bit, 3: DMA Interrupt Enable Bit, 2: Touch Panel Interrupt Enable Bit, 
  1: BTE Process Complete Interrupt Enable Bit, 0: Enable the BTE Interrupt for MCU R/W or Enable the Interrupt of Font Write Function*/
#define RA8875_INTC2_ADDR             0xF1     
#define RA8875_INTC2_KEY              0x10 
#define RA8875_INTC2_DMA              0x08 
#define RA8875_INTC2_TP               0x04  
#define RA8875_INTC2_BTE              0x02 

/****************************************************************************/