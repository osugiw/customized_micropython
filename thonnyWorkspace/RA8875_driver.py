import machine, os
from machine import SPI, Pin
from time import sleep_ms as delay

CLK_SPI_INIT  = 125000
CLK_SPI_WRITE = 2000000

## Command SPI
RA8875_DATAWRITE  = const(0x00)
RA8875_DATAREAD   = const(0x40)
RA8875_CMDWRITE   = const(0x80)
RA8875_CMDREAD    = const(0xC0)   

## Colors (RGB565) - Color Depth 64K
RA8875_BLACK      = const(0x0000)   
RA8875_BLUE       = const(0x001F)   
RA8875_RED        = const(0xF800)   
RA8875_GREEN      = const(0x07E0)   
RA8875_CYAN       = const(0x07FF)   
RA8875_MAGENTA    = const(0xF81F)   
RA8875_YELLOW     = const(0xFFE0)   
RA8875_WHITE      = const(0xFFFF)   
RA8875_TOSCA      = const(0x6DF1)

##############################################################################
##                        System & Configuration Registers
##############################################################################
## Power & Display Register - bit -> 7: Display ON/Off, 6-2: NA, 1: Sleep Mode, 0: Software Reset
RA8875_PWRR_ADDR            = const(0x01)
RA8875_PWRR_DISPLAY_ON      = const(0x80)
RA8875_PWRR_DISPLAY_OFF     = const(0x00)    
RA8875_PWRR_DISPLAY_SLEEP   = const(0x02)
RA8875_PWRR_DISPLAY_NORMAL  = const(0x00)
RA8875_PWRR_DISPLAY_RESET   = const(0x01)

## Memory Read/Write - bit -> 3-2: Data to write/read
RA8875_MRWC_ADDR            = const(0x02)    

## Pixel Clock Setting - bit -> 7: PCLK Inversion (Rising/Falling Edge), 6-2: NA, 1-0: Period Setting  
RA8875_PCSR_ADDR            = const(0x04)    
RA8875_PCSR_CLK             = const(0x00)
RA8875_PCSR_2CLK            = const(0x01)
RA8875_PCSR_4CLK            = const(0X02)
RA8875_PCSR_8CLK            = const(0x03)
RA8875_PCSR_PDAT_RISING     = const(0x00)
RA8875_PCSR_PDAT_FALLING    = const(0x80)

## Serial flash/ROM Config - bit 7: Flash Select, 6: Flash Address, 5: Flash Waveform mode, 4-3: Flash Read Cycle, 2: Acess Mode, 1-0: Flash Data Latch Mode Select
RA8875_SROC_ADDR            = const(0x05)
RA8875_SROC_DMA_MODE        = const(0x04)
RA8875_SRIC_4_BUS           = const(0x00)
RA8875_SROC_5_BUS           = const(0x08)
RA8875_DUAL_MODE_0          = const(0x02)
RA8875_DUAL_MODE_1          = const(0x03)
RA8875_SFCLR_ADDR           = const(0x06)

## System Configuration - bit -> 7-4: NA, 3-2: Color Depth, 1-0: MUCIF Selection
RA8875_SYSR_ADDR            = const(0x10)
RA8875_SYSR_COLOR_8BPP      = const(0x00)
RA8875_SYSR_COLOR_16BPP     = const(0x0C)
RA8875_SYSR_MCU_8BIT        = const(0x00)
RA8875_SYSR_MCU_16BIT       = const(0x03)

## LCD Horizontal Display Width Register - bit -> 7: NA, 6-0: Horizontal Width Setting (max. 0x64 = 800 )
RA8875_HDWR_ADDR            = const(0x14)

## Horizontal Non-Display Period Fine Tuning Option
RA8875_HNDFTR_ADDR          = const(0x15)
RA8875_HNDFTR_DE_HIGH       = const(0x00)
RA8875_HNDFTR_DE_LOW        = const(0x80)

RA8875_HNDR_ADDR            = const(0x16)    ## LCD Horizontal Non-Display Period - bit -> 7-5: NA, 4-0: Horizontal Non-Display Period
RA8875_HSTR_ADDR            = const(0x17)    ## HSYNC Start Position Register - bit -> 7-5: NA, 4-0: HSYNC Start Position

## Hysnc Pulse Width - bit -> 7: HSYNC Polarity, 6-5: NA, 4-0: HSYNC Pulse Width
RA8875_HPWR_ADDR            = const(0x18)     
RA8875_HPWR_POLARITY_HIGH   = const(0x80)
RA8875_HPWR_POLARITY_LOW    = const(0X00)

RA8875_VDHR0_ADDR           = const(0x19)   ## LCD Vertical Display Height Register - bit -> 7-0: Vertical Display Height
RA8875_VDHR1_ADDR           = const(0x1A)   ## LCD Vertical Display Height Register0 - bit -> 7-1: NA, 0: Vertical Display Height (max. 0x1E = 480)
RA8875_VNDR0_ADDR           = const(0X1B)   ## LCD Vertical Non-Display Period Register - bit -> 7-0: Vertical Non-Display Periode
RA8875_VNDR1_ADDR           = const(0x1C)   ## LCD Vertical Non-Display Period Register - bit -> 7-1: NA, 0: Vertical Non-Display Bit
RA8875_VSTR0_ADDR           = const(0x1D)   ## VSYNC Start Position - bit -> 7-0: VSYNC Start Position
RA8875_VSTR1_ADDR           = const(0x1E)   ## VSYNC Start Position - bit -> 7-1: NA, 0: VSYNC Start Position

## VSYNC Pulse Width - bit -> 7: VSYNC Polarity, 6-0: VSYNC Pulse Width
RA8875_VPWR_ADDR            = const(0x1F)   
RA8875_VPWR_POLARITY_HIGH   = const(0x80)
RA8875_VPWR_POLARITY_LOW    = const(0X00)

##############################################################################
##                        LCD Display Control Register
##############################################################################
## Display Configuration Register - bit -> 7: Layer Setting Control, 6-4: NA, 3: Horizontal Scan Direction, 2: Vertical Scan Direction, 1-0: NA
RA8875_DPCR_LAYER1          = const(0x00)
RA8875_DPCR_LAYER2          = const(0x80)
RA8875_DPCR_HDIR_INC        = const(0x00)
RA8875_DPCR_HDIR_DEC        = const(0x08)
RA8875_DPCR_VDIR_INC        = const(0x00)
RA8875_DPCR_VDIR_DEC        = const(0X04)

## Font Control Register1 - bit -> 7: Full Alignment Selection Bit, 6: Font Transparency, 5: NA, 4: Font Rotation, 3-2: Horizontal Font Enlargement, 1-0: Vertical Font Enlargement
RA8875_FNCR1_EN_ALIGNMENT           = const(0x80)
RA8875_FNCR1_DIS_ALIGNMENT          = const(0x00)
RA8875_FNCR1_BG_COLOR               = const(0x00)
RA8875_FNCR1_BG_TRANSPARENT         = const(0x40) 
RA8875_FNCR1_EN_ROTATION            = const(0x10)
RA8875_FNCR1_DIS_ROTATION           = const(0x00)
RA8875_FNCR1_HOR_FONT_ENLARGED1     = const(0x00)
RA8875_FNCR1_HOR_FONT_ENLARGED2     = const(0x04)
RA8875_FNCR1_HOR_FONT_ENLARGED3     = const(0x08)
RA8875_FNCR1_HOR_FONT_ENLARGED4     = const(0x0C)
RA8875_FNCR1_VER_FONT_ENLARGED1     = const(0x00)
RA8875_FNCR1_VER_FONT_ENLARGED2     = const(0x01)
RA8875_FNCR1_VER_FONT_ENLARGED3     = const(0x02)
RA8875_FNCR1_VER_FONT_ENLARGED4     = const(0x03)

RA8875_DPCR_ADDR                    = const(0x20)  ## Display configuration - bit 7: Layer setting control, 6-4: NA, 3: Horizontal Scan Direction, 2: Vertical Scan Direction, 1-0: NA
RA8875_FNCR0_ADDR                   = const(0x21)  ## Font control register 0 - bit 7: CGRAM/CGROM font selection, 6: NA, 5: External/Internal CGROM selection, 4-2: NA, 1-0: Font Selection 
RA8875_FNCR0_INTERNAL_CGROM         = const(0x00)  ## Bit 5: 0, 7:0 for selecting internal CGROM
RA8875_FNCR0_SELECT_CGRAM           = const(0x80)  ## Bit 7: 1 for selecting CGRAM Font
RA8875_FNCR1_ADDR                   = const(0x22)  ## Font control register 1 - bit 7: Full allignemtn, 6: font transparency, 5: NA, 4: Font rotation, 3-2: Horizontal enlargement, 1-0: Vertical englargement 
RA8875_CGSR_ADDR                    = const(0x23)  ## CGRAM Select register - bit 7-0: Setting number of the character in CGRAM, for user-defined character bitmap
RA8875_HOFS0_ADDR                   = const(0x24)  ## Horizontal Display Scroll Offset Register 0- bit -> 7-0: Horizontal Display Scroll
RA8875_HOFS1_ADDR                   = const(0x25)  ## Horizontal Scroll Offset Register 1 - bit -> 7-3: NA, 2-0: Horizontal Display Scroll Offset
RA8875_VOFS0_ADDR                   = const(0x26)  ## Vertical Scroll Offset Register 0 - bit -> 7-0: Vertical Display Scroll
RA8875_VOFS1_ADDR                   = const(0x27)  ## Vertical Scroll Offset Register 1 - bit -> 7-2: NA, 1-0: Vertical Display Scroll Offset
RA8875_FLDR_ADDR                    = const(0x29)  ## Font Line Distance Setting Register - bit -> 7-5: NA, 4-0: Font Line Distance Setting 
RA8875_F_CURXL_ADDR                 = const(0x2A)  ## Font Write Cursor Horizontal Position Register 0 - bit -> 7-0: Font Write Cursor Horizontal Position
RA8875_F_CURXH_ADDR                 = const(0x2B)  ## Font Write Cursor Horizontal Position Register 1 - bit -> 7-2: NA, 1-0: Font Write Cursor Horizontal Position
RA8875_F_CURYL_ADDR                 = const(0x2C)  ## Font Write Cursor Vertical Position Register 0 - bit -> 7-0: Font Write Cursor Vertical Position
RA8875_F_CURYH_ADDR                 = const(0x2D)  ## Font Write Cursor Vertical Position Register 1 - bit -> 7-1: NA, 0: Font Write Cursor Vertical Position

## Font Write Type Setting Register - bit -> 7-6: Font Size Setting, 5-0: Font to font width setting
RA8875_FONT_TYPE_ADDR               = const(0x2E)
RA8875_FONT_TYPE_SIZE_16X16         = const(0x00)
RA8875_FONT_TYPE_SIZE_24X24         = const(0x40)
RA8875_FONT_TYPE_SIZE_32X32         = const(0xC0)
RA8875_FONT_TYPE_WIDTH_0            = const(0x00)
RA8875_FONT_TYPE_WIDTH_5            = const(0x05)
RA8875_FONT_TYPE_WIDTH_10           = const(0x0A)

##############################################################################
##                    Active Window & Scroll Window Setting
##############################################################################
RA8875_HSAW0_ADDR                   = const(0x30)  ## Horizontal Start Point 0 of Active Window - bit 7-0
RA8875_HSAW1_ADDR                   = const(0x31)  ## Horizontal Start Point 1 of Active Window - bit -> 7-2: NA, 1-0: Horizontal Start Point of Active Window
RA8875_VSAW0_ADDR                   = const(0x32)  ## Vertical Start Point 0 of Active Window - bit 7-0
RA8875_VSAW1_ADDR                   = const(0x33)  ## Vertical Start Point 1 of Active Window - bit -> 7-1: NA, 0: Vertical Start Point of Active Window
RA8875_HEAW0_ADDR                   = const(0x34)  ## Horizontal End Point 0 of Active Window - bit 7-0
RA8875_HEAW1_ADDR                   = const(0x35)  ## Horizontal End Point 1 of Active Window - bit 7-2: NA, 1-0: Horizontal End point of Active Window
RA8875_VEAW0_ADDR                   = const(0x36)  ## VERTICAL End Point 0 of Active Window - bit 7-0
RA8875_VEAW1_ADDR                   = const(0x37)  ## VERTICAL End Point 1 of Active Window - bit 7-1: NA, 0: Vertical End point of Active Window

RA8875_HSSW0_ADDR                   = const(0x38)  ## Horizontal Start Point 0 of Scroll Window - bit 7-0
RA8875_HSSW1_ADDR                   = const(0x39)  ## Horizontal Start Point 1 of Scroll Window - bit -> 7-2: NA, 1-0: Horizontal Start Point of Scroll Window
RA8875_VSSW0_ADDR                   = const(0x3A)  ## Vertical Start Point 0 of Scroll Window - bit 7-0
RA8875_VSSW1_ADDR                   = const(0x3B)  ## Vertical Start Point 1 of Scroll Window - bit -> 7-1: NA, 0: Vertical Start Point of Scroll Window
RA8875_HESW0_ADDR                   = const(0x3C)  ## Horizontal End Point 0 of Scroll Window - bit 7-0
RA8875_HESW1_ADDR                   = const(0x3D)  ## Horizontal End Point 1 of Scroll Window - bit -> 7-2: NA, 1-0: Horizontal End Point of Scroll Window
RA8875_VESW0_ADDR                   = const(0x3E)  ## Vertical End Point 0 of Scroll Window - bit 7-0
RA8875_VESW1_ADDR                   = const(0x3F)  ## Vertical End Point 1 of Scroll Window - bit -> 7-1: NA, 0: Vertical End Point of Scroll Window

##############################################################################
##                          Cursor Setting Register
##############################################################################

## Memory Write Control Register 0 - bit -> 7: Text/Graphic Mode, 6: Font Write Cursor, 5: Font Write Cursor Blink, 4: NA, 3-2: Memory Write Direction for Graphic, 
#                                           1: Write Cursor Auto-Increase Disable, 0: Read Cursor Auto-Increase Disable
RA8875_MWCR0_ADDR                   = const(0x40)
RA8875_MWCR0_GRAPHIC_MODE           = const(0x00) 
RA8875_MWCR0_TEXT_MODE              = const(0x80)
RA8875_MWCR0_CURSOR_VISIBLE         = const(0x40)
RA8875_MWCR0_CURSOR_INVISIBLE       = const(0x00)
RA8875_MWCR0_CURSOR_EN_BLINK        = const(0x20)
RA8875_MWCR0_CURSOR_DIS_BLINK       = const(0x00)    
RA8875_MWCR0_DIRMASK                = const(0x0C)    ## Bitmask for Write Direction
RA8875_MWCR0_LRTD                   = const(0x00)    ## Left->Right then Top->Down
RA8875_MWCR0_RLTD                   = const(0x04)    ## Right->Left then Top->Down
RA8875_MWCR0_TDLR                   = const(0x08)    ## Top->Down then Left->Right
RA8875_MWCR0_DTLR                   = const(0x0C)    ## Down->Top then Left->Right    

##  Memory Write Control Register 1 - bit -> 7: Graphic Cursor Enable, 6-4: Graphic Cursor Selection Bit, 3-2: Write Destination Selection, 1: NA, 0: Layer Selection
RA8875_MWCR1_ADDR                   = const(0x41)
RA8875_MWCR1_GRAPHIC_CURSOR_EN      = const(0x80)
RA8875_MWCR1_GRAPHIC_CURSOR_DIS     = const(0x00)
RA8875_MWCR1_GRAPHIC_CURSOR_1       = const(0x00)
RA8875_MWCR1_GRAPHIC_CURSOR_3       = const(0x20)
RA8875_MWCR1_GRAPHIC_CURSOR_5       = const(0x40)
RA8875_MWCR1_GRAPHIC_CURSOR_7       = const(0x60)
RA8875_MWCR1_DEST_LAYER_1_2         = const(0x00)
RA8875_MWCR1_DEST_CGRAM             = const(0x04)
RA8875_MWCR1_DEST_GRAPHIC_CUR       = const(0x08)
RA8875_MWCR1_DEST_PATTERN           = const(0x0C)
RA8875_MWCR1_SELECT_LAYER_1         = const(0x00)
RA8875_MWCR1_SELECT_LAYER_2         = const(0x01)

RA8875_BTCR_ADDR                    = const(0x44)    ## Blink Time Control Register - bit -> 7-0: Text Blink Time Setting (00-FF = 0-256 Frames)
RA8875_MRCD_ADDR                    = const(0x45)    ## Memory Read Cursor DIrection - bit -> 7-2: NA, 1-0: Memory Read Direction for Graphic Mode
RA8875_CURH0_ADDR                   = const(0x46)    ## Memory Write Cursor Horizontal Position Register 0 - bit 7-0: Cursor Horizontal location
RA8875_CURH1_ADDR                   = const(0x47)    ## Memory Write Cursor Horizontal Position Register 1 - bit 7-2: NA, 1-0: Cursor Horizontal location
RA8875_CURV0_ADDR                   = const(0x48)    ## Memory Write Cursor Vertical Position Register 0 - bit 7-0: Cursor Vertical location
RA8875_CURV1_ADDR                   = const(0x49)    ## Memory Write Cursor Vertical Position Register 1 - bit 7-1: NA, 0: Cursor Vertical location

##############################################################################
##                    Block Transfer Engine Control Register
##############################################################################

## BTE Function Control Register 0  - bit 7: BTE Function Enable/Status, 6: BTE Source Data, 5: BTE Destination Data Type, 4-0: NA
RA8875_BECR0_ADDR                 = const(0x50)
RA8875_BECR0_ENABLE               = const(0x80)
RA8875_BECR0_SOURCE_BLOCK         = const(0x00)
RA8875_BECR0_SOURCE_LINEAR        = const(0x40)
RA8875_BECR0_DESTINATION_BLOCK    = const(0x00)
RA8875_BECR0_DESTINATION_LINEAR   = const(0x20)

## BTE Function Control Register 1 - bit 7-4: BTE ROP Code Bit[3:0], 3-0: BTE Operation Code Bit[3:0]
RA8875_BECR1_ADDR                     = const(0x51)
RA8875_OPERATION_WRITE_ROP            = const(0x00)
RA8875_OPERATION_WRTIE_TRANSPARENT    = const(0x04)
RA8875_OPERATION_SOLID_FILL           = const(0x0C)
RA8875_ROP_S_EQUAL_D                  = const(0xC0)
RA8875_ROP_S_OR_D                     = const(0xE0)
RA8875_ROP_INVS_MULTI_D               = const(0x20)
RA8875_ROP_D_EQUAL_D                  = const(0xA0)

## Layer Transparency Register 0 - bit -> 7-6: Layer 1/2 Scroll Mode, 5: Floating Windows Transparency, 4-3: NA, 2-0: Layer 1/2 Display Mode
RA8875_LTPR0_ADDR                 = const(0x52)
RA8875_LTPR0_SCROLL_SIMUL         = const(0x00)
RA8875_LTPR0_SCROLL_LAYER1        = const(0x40)
RA8875_LTPR0_SCROLL_LAYER2        = const(0x80)
RA8875_LTPR0_SCROLL_BUFFER        = const(0xC0)
RA8875_LTPR0_EN_FLOATING          = const(0x20)
RA8875_LTPR0_DIS_FLOATING         = const(0x00)
RA8875_LTPR0_ONLY_LAYER1          = const(0x00)
RA8875_LTPR0_ONLY_LAYER2          = const(0x01)
RA8875_LTPR0_LIGHTEN_OVERLAY      = const(0x02)
RA8875_LTPR0_TRANSPARENT          = const(0x03)

## Layer Transparency Register 1 - bit -> 7-4: Layer Transparency for Layer 2, 3-0: Layer Transparency for Layer 1
RA8875_LTPR1_ADDR                 = const(0x53)
RA8875_LTPR1_L2_TOTAL_DISP        = const(0x00)
RA8875_LTPR1_L2_HALF_DISP         = const(0x40)
RA8875_LTPR1_L2_DIS_DISP          = const(0x80)
RA8875_LTPR1_L1_TOTAL_DISP        = const(0x00)
RA8875_LTPR1_L1_HALF_DISP         = const(0x04)
RA8875_LTPR1_L1_DIS_DISP          = const(0x08)

## BTE Source Point
RA8875_HSBE0_ADDR                 = const(0x54)  ## Horizontal Source Point 0 of BTE - Bit 7-0: Horizontal Source Point of BTE[7:0]
RA8875_HSBE1_ADDR                 = const(0x55)  ## Horizontal Source Point 1 of BTE - Bit 7-2: NA, 1-0: Horizontal Source Point of BTE[9:8]
RA8875_VSBE0_ADDR                 = const(0x56)  ## Vertical Source Point 0 of BTE - Bit 7-0: Vertical Source Point of BTE[7:0]
RA8875_VSBE1_ADDR                 = const(0x57)  ## Vertical Source Point 1 of BTE - Bit 7: BTE Source Layer Selection, 6-1: NA, 0: Vertical Source Point of BTE[8]

## BTE Destination Point
RA8875_HDBE0_ADDR                 = const(0x58)  ## Horizontal Destination Point 0 of BTE - Bit 7-0: Horizontal Destination Point of BTE[7:0]
RA8875_HDBE1_ADDR                 = const(0x59)  ## Horizontal Destination Point 1 of BTE - Bit 7-2: NA, 1-0: Horizontal Destination Point of BTE[9:8]
RA8875_VDBE0_ADDR                 = const(0x5A)  ## Vertical Destination Point 0 of BTE - Bit 7-0: Vertical Destination Point of BTE[7:0]
RA8875_VDBE1_ADDR                 = const(0x5B)  ## Vertical Destination Point 1 of BTE - Bit 7: BTE Destination Layer Selection, 6-1: NA, 0: Vertical Destination Point of BTE[8]

## BTE Width & Height Register
RA8875_BEWR0_ADDR                 = const(0x5C)  ## BTE Width Register 0 - Bit 7-0: BTE Width Setting[7:0]
RA8875_BEWR1_ADDR                 = const(0x5D)  ## BTE Width Register 1 - Bit 7-2: NA, 1-0: BTE Width Setting[9:8]
RA8875_BEHR0_ADDR                 = const(0x5E)  ## BTE Height Resigter 0 - Bit 7-0: BTE Height Setting[7:0]
RA8875_BEHR1_ADDR                 = const(0x5E)  ## BTE Height Resigter 1 - Bit 7-2: NA, 1-0: BTE Height Setting[9:8]

## BTE Color Register
RA8875_BGCR0_ADDR                 = const(0x60) ## Background Color Register 0 - bit -> 7-5: NA, 4-0: Background Color Red
RA8875_BGCR1_ADDR                 = const(0x61) ## Background Color Register 1 - bit -> 7-6: NA, 5-0: Background Color Green
RA8875_BGCR2_ADDR                 = const(0x62) ## Background Color Register 2 - bit -> 7-5: NA, 4-0: Background Color Blue
RA8875_FGCR0_ADDR                 = const(0x63) ## Foreground Color Register 0 - bit -> 7-5: NA, 4-0: Foreground Color Red
RA8875_FGCR1_ADDR                 = const(0x64) ## Foreground Color Register 1 - bit -> 7-6: NA, 5-0: Foreground Color Green
RA8875_FGCR2_ADDR                 = const(0x65) ## Foreground Color Register 2 - bit -> 7-5: NA, 4-0: Foreground Color Blue

## Pattern Set No for BTE - bit -> 7: Pattern Format, 6-4: NA, 3-0: Pattern Set No
RA8875_PTNO_ADDR                  = const(0x66) 
RA8875_PTNO_8x8                   = const(0x00)
RA8875_PTN0_16X16                 = const(0x80)

## BTE Background Color Register for Transparent
RA8875_BGTR0_ADDR                 = const(0x67)  ## Background color register for transparent 0 - Bit 7-5: NA, 4-0: Background color register for transparent Red[4:0]
RA8875_BGTR1_ADDR                 = const(0x68)  ## Background color register for transparent 1 - Bit 7-6: NA, 5-0: Background color register for transparent Green[5:0]
RA8875_BGTR2_ADDR                 = const(0x69)  ## Background color register for transparent 2 - Bit 7-5: NA, 4-0: Background color register for transparent Blue[4:0]

##############################################################################
##                    	Touch Panel Control Register
##############################################################################

## Touch Panel Control Register 0 - bit -> 7: Touch Panel Enable, 6-4: TP Sample Time, 3: Wakeup Enable, 2-0: ADC Clock Setting
RA8875_TPCR0_ADDR               = const(0x70)               
RA8875_TPCR0_ENABLE             = const(0x80)        
RA8875_TPCR0_DISABLE            = const(0x00)       
RA8875_TPCR0_WAIT_512CLK        = const(0x00)   
RA8875_TPCR0_WAIT_1024CLK       = const(0x10)  
RA8875_TPCR0_WAIT_2048CLK       = const(0x20)  
RA8875_TPCR0_WAIT_4096CLK       = const(0x30)  
RA8875_TPCR0_WAIT_8192CLK       = const(0x40)  
RA8875_TPCR0_WAIT_16384CLK      = const(0x50) 
RA8875_TPCR0_WAIT_32768CLK      = const(0x60) 
RA8875_TPCR0_WAIT_65536CLK      = const(0x70) 
RA8875_TPCR0_WAKEENABLE         = const(0x08)    
RA8875_TPCR0_WAKEDISABLE        = const(0x00)   
RA8875_TPCR0_ADCCLK_DIV1        = const(0x00)   
RA8875_TPCR0_ADCCLK_DIV2        = const(0x01)   
RA8875_TPCR0_ADCCLK_DIV4        = const(0x02)   
RA8875_TPCR0_ADCCLK_DIV8        = const(0x03)   
RA8875_TPCR0_ADCCLK_DIV16       = const(0x04)  
RA8875_TPCR0_ADCCLK_DIV32       = const(0x05)  
RA8875_TPCR0_ADCCLK_DIV64       = const(0x06)  
RA8875_TPCR0_ADCCLK_DIV128      = const(0x07) 

## Touch Panel Control Register 1 - bit -> 7: NA, 6: TP Manual Enable, 5: TP ADC Reference Voltage, 4-3: NA, 2: De-bounce Circuit Enable, 1-0: Model Selection for TP Manual Mode
RA8875_TPCR1_ADDR               = const(0x71) 
RA8875_TPCR1_AUTO               = const(0x00) 
RA8875_TPCR1_MANUAL             = const(0x40) 
RA8875_TPCR1_VREFINT            = const(0x00) 
RA8875_TPCR1_VREFEXT            = const(0x20) 
RA8875_TPCR1_DEBOUNCE           = const(0x04) 
RA8875_TPCR1_NODEBOUNCE         = const(0x00) 
RA8875_TPCR1_IDLE               = const(0x00) 
RA8875_TPCR1_WAIT               = const(0x01) 
RA8875_TPCR1_LATCHX             = const(0x02) 
RA8875_TPCR1_LATCHY             = const(0x03) 

RA8875_TPXH_ADDR                = const(0x72)  ## Touch Panel X High Byte Data Register  - bit -> 7-0: Touch Panel X Data Bit[9:2]
RA8875_TPYH_ADDR                = const(0x73)  ## Touch Panel Y High Byte Data Register  - bit -> 7-0: Touch Panel Y Data Bit[9:2]
RA8875_TPXYL_ADDR               = const(0x74)  ## Touch Panel X/Y Low Byte Data Register - BIT -> 7: Touch Event Detector, 6-4: NA, 3-2: Touch Panel Y, 1-0: Touch Panel X

##############################################################################
##                    Graphic Cursor Setting Register
##############################################################################
RA8875_GCHP0_ADDR               = const(0x80)  ## Graphic Cursor Horizontal Position Register 0 - bit -> 7-0: Graphic Cursor Horizontal Location
RA8875_GCHP1_ADDR               = const(0x81)  ## Graphic Cursor Horizontal Position Register 1 - bit -> 7-2: NA, 1-0: Graphic Cursor Horizoontal Location
RA8875_GCVP0_ADDR               = const(0x82)  ## Graphic Cursor Vertical Position Register 0   - bit -> 7-0: Graphic Cursor Vertical Location
RA8875_GCVP1_ADDR               = const(0x83)  ## Graphic Cursor Vertial Position Register 1    - bit -> 7-1: NA, 0: Graphic Cursor Vertical Location
RA8875_GCC0_ADDR                = const(0x84)  ## Graphic Cursor Color 0                        - bit -> 7-0: Graphic Cursor COlor 0 with 256 Colors (RGB)
RA8875_GCC1_ADDR                = const(0x85)  ## Graphic Cursor Color 1

##############################################################################
##                    Graphic Cursor Setting Register
##############################################################################

## PLL Control Register 1 - bit -> 7: PLLDIVM, 6-5: NA, 4-0: PLLDIVN
RA8875_PLLC1_ADDR               = const(0x88)  
RA8875_PLLC1_DIV_1              = const(0x00)
RA8875_PLLC1_DIV_2              = const(0x80)

## PLL Control Register 2 - bit -> 7-3: NA, 2-0: PLL Output divider
RA8875_PLLC2_ADDR               = const(0x89)       
RA8875_PLLC2_DIV1               = const(0x00)  
RA8875_PLLC2_DIV2               = const(0x01)  
RA8875_PLLC2_DIV4               = const(0x02)  
RA8875_PLLC2_DIV8               = const(0x03)  
RA8875_PLLC2_DIV16              = const(0x04) 
RA8875_PLLC2_DIV32              = const(0x05) 
RA8875_PLLC2_DIV64              = const(0x06) 
RA8875_PLLC2_DIV128             = const(0x07)

## PWM1 Control Register - bit -> 7: PWM1 Enable, 6: PWM1 Disable Level, 5: Reserved, 4: PWM1 Function Selection, 3-0: PWM1 Clock Source Divide Ratio
RA8875_P1CR_ADDR                = const(0x8A)        
RA8875_P1CR_ENABLE              = const(0x80) 
RA8875_P1CR_DISABLE             = const(0x00)
RA8875_P1CR_CLKOUT              = const(0x10) 
RA8875_P1CR_PWMOUT              = const(0x00) 
RA8875_P1DCR_ADDR               = const(0x8B)  ## PWM1 Duty Cycle Register - bit 7-0: PWM Cycle Duty Selection Bit (00-FF)

## PWM2 Control Register - bit -> 7: PWM2 Enable, 6L PWM2 Disable Level, 5: Reserved, 4: PWM2 Function Selection, 3-0: PWM2 Clock Source Divide Ratio
RA8875_P2CR_ADDR                = const(0x8C)         
RA8875_P2CR_ENABLE              = const(0x80)  
RA8875_P2CR_DISABLE             = const(0x00) 
RA8875_P2CR_CLKOUT              = const(0x10)  
RA8875_P2CR_PWMOUT              = const(0x00)  
RA8875_P2DCR_ADDR               = const(0x8D)  ## PWM1 Duty Cycle Register - bit 7-0: PWM Cycle Duty Selection Bit (00-FF)

## Memory Clear Control Register - bit -> 7: Memory Clear Function, 6: Memory Clear Area Setting, 5-0: NA
RA8875_MCLR_ADDR                = const(0x8E)            
RA8875_MCLR_START               = const(0x80)      
RA8875_MCLR_STOP                = const(0x00)       
RA8875_MCLR_READSTATUS          = const(0x80) 
RA8875_MCLR_FULL                = const(0x00)       
RA8875_MCLR_ACTIVE              = const(0x40)     

RA8875_PWM_CLK_DIV1             = const(0x00)     
RA8875_PWM_CLK_DIV2             = const(0x01)     
RA8875_PWM_CLK_DIV4             = const(0x02)     
RA8875_PWM_CLK_DIV8             = const(0x03)     
RA8875_PWM_CLK_DIV16            = const(0x04)    
RA8875_PWM_CLK_DIV32            = const(0x05)    
RA8875_PWM_CLK_DIV64            = const(0x06)    
RA8875_PWM_CLK_DIV128           = const(0x07)   
RA8875_PWM_CLK_DIV256           = const(0x08)   
RA8875_PWM_CLK_DIV512           = const(0x09)   
RA8875_PWM_CLK_DIV1024          = const(0x0A)  
RA8875_PWM_CLK_DIV2048          = const(0x0B)  
RA8875_PWM_CLK_DIV4096          = const(0x0C)  
RA8875_PWM_CLK_DIV8192          = const(0x0D)  
RA8875_PWM_CLK_DIV16384         = const(0x0E) 
RA8875_PWM_CLK_DIV32768         = const(0x0F) 

##############################################################################
##                        Drawing Control Register
##############################################################################

## Draw Line/Circle/Square Control Register - bit -> 7: Draw Line/Square/Triangle Start Signal, 6: Draw Circle Start Signal, 5: Fill the Circle/Square/Triangle Signal, 
##  												 4: Draw Line/Square Select Signal, 3-1: NA, 0: Draw Triangle or Line/Square Signal
RA8875_DCR_ADDR                 = const(0x90)                   
RA8875_DCR_LINESQUTRI_START     = const(0x80)  
RA8875_DCR_LINESQUTRI_STOP      = const(0x00)   
RA8875_DCR_LINESQUTRI_STATUS    = const(0x80) 
RA8875_DCR_CIRCLE_START         = const(0x40)      
RA8875_DCR_CIRCLE_STATUS        = const(0x40)     
RA8875_DCR_CIRCLE_STOP          = const(0x00)       
RA8875_DCR_FILL                 = const(0x20)              
RA8875_DCR_NOFILL               = const(0x00)            
RA8875_DCR_DRAWLINE             = const(0x00)          
RA8875_DCR_DRAWTRIANGLE         = const(0x01)      
RA8875_DCR_DRAWSQUARE           = const(0x10)        

RA8875_DLHSR0_ADDR              = const(0x91)  ## Draw Line/Square Horizontal Start Address Register0 - bit 7-0: Draw Line/Square Horizontal Start Address
RA8875_DLHSR1_ADDR              = const(0x92)  ## Draw Line/Square Horizontal Start Address Register1 - bit 7-2: NA, 1-0: Draw Line/Square Horizontal Start Address
RA8875_DLVSR0_ADDR              = const(0x93)  ## Draw Line/Square Vertical Start Address Register0   - bit 7-0: Draw Line/Square Vertical Start Address
RA8875_DLVSR1_ADDR              = const(0x94)  ## Draw Line/Square Vertical Start Address Register1   - bit 7-1: NA, 0: Draw Line/Square Vertical Start Address
RA8875_DLHER0_ADDR              = const(0x95)  ## Draw Line/Square Horizontal End Address Register0   - bit 7-0: Draw Line/Square Horizontal End Address
RA8875_DLHER1_ADDR              = const(0x96)  ## Draw Line/Square Horizontal End Address Register1   - bit 7-2: NA, 1-0: Draw Line/Square Horizontal End Address
RA8875_DLVER0_ADDR              = const(0x97)  ## Draw Line/Square Vertical End Address Register0     - bit 7-0: Draw Line/Square Vertical End Address
RA8875_DLVER1_ADDR              = const(0x98)  ## Draw Line/Square Vertical End Address Register1     - bit 7-1: NA, 0: Draw Line/Square Vertical End Address
RA8875_DCHR0_ADDR               = const(0x99)  ## Draw Circle Center Horizontal Address Register0     - bit 7-0: Draw Circle Center Horizontal Address
RA8875_DCHR1_ADDR               = const(0x9A)  ## Draw Circle Center Horizontal Address Register1     - bit 7-2: NA, 1-0: Draw Circle Center Horizontal Address
RA8875_DCVR0_ADDR               = const(0x9B)  ## Draw Circle Center Vertical Address Register0       - bit 7-0: Draw Circle Center Vertical Address
RA8875_DCVR1_ADDR               = const(0X9C)  ## Draw Circle Center Vertical Address Register1       - bit 7-1: NA. 0: Draw Circle Center Vertical Address
RA8875_DCRR_ADDR                = const(0x9D)  ## Draw Circle Radius Register - bit 7-0: Draw Circle Radius

## Draw Ellipse/Ellipse Curve/Circle Square Control Register - bit 7: Draw Ellipse/Circle Square Start Signal, 6:Fill the Ellipse/Circle Square Signal, 
##   5: Draw Ellipse/ Ellipse Curve or Circle Square Select Signal, 4: Draw Ellipse or Ellipse Curve Select Signal, 3-2: NA, 1-0: Draw Ellipse Curve Part Select(DECP)*/
RA8875_ELLIPSE_ADDR             = const(0xA0)
RA8875_ELLIPSE_START            = const(0x80)
RA8875_ELLIPSE_STOP             = const(0x00)
RA8875_ELLIPSE_STATUS           = const(0x80)
RA8875_ELLIPSE_FILL             = const(0x40)
RA8875_ELLIPSE_NOFILL           = const(0x00)
RA8875_CIRCLESQUARE_DRAW        = const(0x20)
RA8875_ELLIPSE_CURVE_DRAW       = const(0x10)
RA8875_ELLIPSE_DRAW             = const(0x00)      

RA8875_ELL_A0_ADDR              = const(0xA1)  ## Draw Ellipse/Circle Square Long axis Setting Register - bit 7-0: Draw Ellipse/Circle Square Long axis
RA8875_ELL_A1_ADDR              = const(0xA2)  ## Draw Ellipse/Circle Square Long axis Setting Register - bit 7-2: NA, 1-0: Draw Ellipse/Circle Square Long axis
RA8875_ELL_B0_ADDR              = const(0xA3)  ## Draw Ellipse/Circle Square Short axis Setting Register - bit 7-0: Draw Ellipse/Circle Square Short axis
RA8875_ELL_B1_ADDR              = const(0xA4)  ## Draw Ellipse/Circle Square Short axis Setting Register - bit 7-1: NA, 0: Draw Ellipse/Circle Square Short axis
RA8875_DEHR0_ADDR               = const(0xA5)  ## Draw Ellipse/Circle Square Center Horizontal Address Register0 - bit 7-0: Draw Ellipse/Circle Square Center Horizontal Address
RA8875_DEHR1_ADDR               = const(0xA6)  ## Draw Ellipse/Circle Square Center Horizontal Address Register1 - bit 7-2: NA, 1-0: Draw Ellipse/Circle Square Center Horizontal Address
RA8875_DEVR0_ADDR               = const(0xA7)  ## Draw Ellipse/Circle Square Center Vertical Address Register0 - bit 7-0: Draw Ellipse/Circle Square Center Vertical Address
RA8875_DEVR1_ADDR               = const(0xA8)  ## Draw Ellipse/Circle Square Center Vertical Address Register1 - bit 7-1: NA, 0: Draw Ellipse/Circle Square Center Vertical Address
RA8875_DTPH0_ADDR               = const(0xA9)  ## Draw Triangle Point 2 Horizontal Address Register0 - bit 7-0: Draw Triangle Point 2 Horizontal Address
RA8875_DTPH1_ADDR               = const(0xAA)  ## Draw Triangle Point 2 Horizontal Address Register1 - bit 7-2: NA, 1-0: Draw Triangle Point 2 Horizontal Address
RA8875_DTPV0_ADDR               = const(0xAB)  ## Draw Triangle Point 2 Vertical Address Register0 - bit 7-0: Draw Triangle Point 2 Vertical Address
RA8875_DTPV1_ADDR               = const(0xAC)  ## Draw Triangle Point 2 Vertical Address Register1 - bit 7-1: NA, 0: Draw Triangle Point 2 Vertical Address

##############################################################################
##                            DMA Registers
##############################################################################
RA8875_SSAR0_ADDR               = const(0xB0)  ## Source Starting Address REG0 - bit 7-0: DMA Source START ADDRESS [7:0]
RA8875_SSAR1_ADDR               = const(0xB1)  ## Source Starting Address REG1 - bit 7-0: DMA Source START ADDRESS [15:8]
RA8875_SSAR2_ADDR               = const(0xB2)  ## Source Starting Address REG2 - bit 7-0: DMA Source START ADDRESS [23:16]
RA8875_BWR0_ADDR                = const(0xB4)  ## Block width REG0 - bit 7-0: DMA Transfer number[0:7] / DMA Block width[7:0]
RA8875_BWR1_ADDR                = const(0xB5)  ## Block width REG1 - bit 7-2: NA, 1-0: DMA Block Width [9:8]
RA8875_BHR0_ADDR                = const(0xB6)  ## Block height REG0 - bit 7-0:  DMA Transfer number [15:8] or DMA Block Height[7:0]
RA8875_BHR1_ADDR                = const(0xB7)  ## Block Height REG1 - bit 7-2: NA, 1-0: DMA Block Height [9:8]
RA8875_SPWR0_ADDR               = const(0xB8)  ## Source Picture Width Reg 0 - 7-3: DMA Source Picture Width [7:3], 2-0: DMA Transfer Number[18:16] or DMA Source Picture Width[2:0]
RA8875_SPWR1_ADDR               = const(0xB9)  ## Source Picture Width Reg 1 - 7-2: NA, 1-0: DMA Source Picture Width[9:8]
RA8875_DMACR_ADDR               = const(0xBF)  ## DMA Configuration Reg - 7-2: NA, 1: Continuous/Block Mode, 0: Write Start Bit or Read DMA Busy Check Bit

##############################################################################
##						IO & Key Control Registers
##############################################################################
RA8875_GPIOX_ADDR               = const(0xC7)  ## Extra General Purpose IO Register - bit 7-1: NA, 0: GPIX/GPOX Data Bit

##############################################################################
##                      Serial Flash Control Registers
##############################################################################
RA8875_SACS_MODE_ADDR           = const(0xE0)  ## Serial Flash/ROM Direct Access Mode
RA8875_SACS_MODE_DMA            = const(0x00)  ## FONT/DMA Select
RA8875_SACS_MODE_DIRECT         = const(0x01)  ## Direct Acess Mode selected
RA8875_SACS_ADDR                = const(0xE1)  ## Serial Flash/ROM Direct Access Mode Address
RA8875_SACS_DATA                = const(0xE2)  ## Serial Flash/ROM Direct Acess Data Read

##############################################################################
##                        Interrupt Control Registers
##############################################################################
##	Interrupt Control Register1 - bit 7-5: NA, 4: KEYSCAN Interrupt Enable Bit, 3: DMA Interrupt Enable Bit, 2: Touch Panel Interrupt Enable Bit, 
##  1: BTE Process Complete Interrupt Enable Bit, 0: Enable the BTE Interrupt for MCU R/W or Enable the Interrupt of Font Write Function
RA8875_INTC1_ADDR               = const(0xF0)    
RA8875_INTC1_KEY                = const(0x10)
RA8875_INTC1_DMA                = const(0x08)
RA8875_INTC1_TP                 = const(0x04) 
RA8875_INTC1_BTE                = const(0x02)

## Interrupt Control Register2 - bit 7-5: NA, 4: KEYSCAN Interrupt Enable Bit, 3: DMA Interrupt Enable Bit, 2: Touch Panel Interrupt Enable Bit, 
##  1: BTE Process Complete Interrupt Enable Bit, 0: Enable the BTE Interrupt for MCU R/W or Enable the Interrupt of Font Write Function
RA8875_INTC2_ADDR               = const(0xF1)     
RA8875_INTC2_KEY                = const(0x10) 
RA8875_INTC2_DMA                = const(0x08) 
RA8875_INTC2_TP                 = const(0x04)  
RA8875_INTC2_BTE                = const(0x02) 
######################################################################################

"""Python class for RA8875 Driver"""
class RA8875:
    def __init__(self, _cs=10, _clk=12, _mosi=11, _miso=13, _intr=6, _rst=5, _baudrate=CLK_SPI_INIT):
        # Pin Assigments
        self.cs		= Pin(_cs, Pin.OUT, value=1)
        self.rst	= Pin(_rst, Pin.OUT, value=1)
        self.intr	= Pin(_intr, Pin.IN)
        self.mosi	= Pin(_mosi)
        self.miso	= Pin(_miso)
        self.clk	= Pin(_clk)
        self.spi 	= None
        self.baudrate = _baudrate
    
    def begin(self):
        # Set CS to High
        self.cs.value(1)
        # Set Reset Pin to Low
        self.rst.value(0)
        delay(1)
        # Set Reset Pin to High
        self.rst.value(1)
        delay(10)
        
        # Set SPI Baudrate to Init Speed for entire initialization
        self.baudrate = CLK_SPI_INIT
        
        # SPI Initialization - Similiar to SPI_MODE0 (Polarity=0, Phase=0) in Arduino
        self.spi = SPI(2, baudrate=self.baudrate, miso=self.miso, mosi=self.mosi, sck=self.clk, polarity=0, phase=0)
        
        check_SPI_bus = self.readReg(0)
        if check_SPI_bus != 0x75:
            print(check_SPI_bus)
            return False
        
        
        # Initalize Clock Frequency	- System Clock: 60MHz (20*(11+1)/2^2)
        self.writeReg(RA8875_PLLC1_ADDR, RA8875_PLLC1_DIV_1 + 11)
        delay(1)
        # 16bpp Color depth & 8 bit MCU Interface
        self.writeReg(RA8875_SYSR_ADDR, RA8875_SYSR_COLOR_16BPP | RA8875_SYSR_MCU_8BIT)
        
        # Windows Configuration
        screen_width = 800 
        screen_height = 480
        pixclk = RA8875_PCSR_PDAT_FALLING | RA8875_PCSR_2CLK   # 30 MHz = (System Clock / 2) = 60/2
        hsync_nondisp = 26
        hsync_start = 32
        hsync_pw = 96
        hsync_finetune = 3 
        # hsync_finetune = 0
        vsync_nondisp = 32
        vsync_start = 23
        vsync_pw = 2
        _voffset = 0
        
        # Pixel Clock Settings
        self.writeReg(RA8875_PCSR_ADDR, pixclk) 
        delay(1)
        
        # Horizontal Settings Registers
        self.writeReg(RA8875_HDWR_ADDR, (screen_width // 8) - 1)                                           # LCD Horizontal Display Width (HDWR = (Width/8)-1)
        self.writeReg(RA8875_HNDFTR_ADDR, RA8875_HNDFTR_DE_HIGH + hsync_finetune)                         # Period Fine Tuning for Horizontal Non-Display
        self.writeReg(RA8875_HNDR_ADDR, (hsync_nondisp - hsync_finetune - 2) // 7)                         # LCD Horizontal Non-Display Period (HNDR = (HSYNC_Nondisp-HSYNC_Finetune-2)/8)
        # self.writeReg(RA8875_HNDR_ADDR, (hsync_nondisp - hsync_finetune - 2) / 8)                       # LCD Horizontal Non-Display Period (HNDR = (HSYNC_Nondisp-HSYNC_Finetune-2)/8)
        self.writeReg(RA8875_HSTR_ADDR, (hsync_start // 8) - 2)                                            # HSYNC Start Position (HSTR = (HSYNC_Start/8)-1)
        # self.writeReg(RA8875_HSTR_ADDR, (hsync_start / 8) - 1)                                          # HSYNC Start Position (HSTR = (HSYNC_Start/8)-1)
        self.writeReg(RA8875_HPWR_ADDR, RA8875_HPWR_POLARITY_LOW + (hsync_pw // 8 - 12))                   # HSYNC Pulse Width (HPW = (HSYNC_PW/8)-1)
        # self.writeReg(RA8875_HPWR_ADDR, RA8875_HPWR_POLARITY_LOW + (hsync_pw / 8 - 1))                  # HSYNC Pulse Width (HPW = (HSYNC_PW/8)-1)

        # Vertical Setting Registers
        self.writeReg(RA8875_VDHR0_ADDR, (screen_height - 1 + _voffset) & 0xFF)                 # LCD Vertical Height Register 0 (VDHR = Height-1+V_Offset)
        self.writeReg(RA8875_VDHR1_ADDR, (screen_height - 1 + _voffset) >> 8)                   # LCD Vertical Height Register 1 (VDHR = Height-1+V_Offset)
        self.writeReg(RA8875_VNDR0_ADDR, vsync_nondisp - 12)                                              # LCD Vertical Non-Display Period (VNDR = VSYNC_NonDisplay - 1)
        # self.writeReg(RA8875_VNDR0_ADDR, vsync_nondisp - 1)                                             # LCD Vertical Non-Display Period (VNDR = VSYNC_NonDisplay - 1)
        self.writeReg(RA8875_VNDR1_ADDR, (vsync_nondisp - 1)  >> 8)                                       # LCD Vertical Non-Display Period (VNDR = VSYNC_NonDisplay - 1) >> 8
        self.writeReg(RA8875_VSTR0_ADDR, vsync_start - 17)                                                # VSYNC Start Position Register (VSTR = VSYNC_Start - 1)
        # self.writeReg(RA8875_VSTR0_ADDR, vsync_start - 1)                                               # VSYNC Start Position Register (VSTR = VSYNC_Start - 1)
        self.writeReg(RA8875_VSTR1_ADDR, (vsync_start - 1) >> 8)                                          # VSYNC Start Position Register (VSTR = VSYNC_Start - 1) >> 8
        self.writeReg(RA8875_VPWR_ADDR, (RA8875_VPWR_POLARITY_LOW + vsync_pw - 1))                        # VSYNC Pulse Width (VPWR = VSYNC_PW - 1)
        
        # Setting active area 
        self.writeReg(RA8875_HSAW0_ADDR, 0)                                                               # Horizontal Start point is at 0
        self.writeReg(RA8875_HSAW1_ADDR, 0 >> 8)
        self.writeReg(RA8875_HEAW0_ADDR, (screen_width - 1 ) & 0xFF)                            # Horizontal End point is at x
        self.writeReg(RA8875_HEAW1_ADDR, (screen_width - 1 ) >> 8)

        self.writeReg(RA8875_VSAW0_ADDR, 0 + _voffset)                                                    # Vertical Start point is at 0
        self.writeReg(RA8875_VSAW1_ADDR, (0 + _voffset) >> 8)
        self.writeReg(RA8875_VEAW0_ADDR, (screen_height - 1 + _voffset) & 0xFF)                 # Vertical End point is at y
        self.writeReg(RA8875_VEAW1_ADDR, (screen_height - 1 + _voffset) >> 8)

        # Clear Window
        self.writeReg(RA8875_MCLR_ADDR, RA8875_MCLR_START | RA8875_MCLR_FULL)
        delay(500)
        
        # Set SPI Baudrate to Write Speed
        self.baudrate = CLK_SPI_WRITE
        return True
    
    """Enable PWM of Layer 1"""
    def enablePWM1(self, clock):
        self.writeReg(RA8875_P1CR_ADDR, RA8875_P1CR_ENABLE | (clock & 0xF))
    
    """Set duty cycle of PWM"""
    def setPWM1DutyCycle(self, dc):
        self.writeReg(RA8875_P1DCR_ADDR, dc)
    
    """Enable GPIO"""
    def enableGPIOX(self):
        self.writeReg(RA8875_GPIOX_ADDR, True)
    
    """Disable GPIO"""
    def disableGPIOX(self):
        self.writeReg(RA8875_GPIOX_ADDR, False)

    """Turn display on"""
    def displayOn(self):
        self.writeReg(RA8875_PWRR_ADDR, RA8875_PWRR_DISPLAY_ON | RA8875_PWRR_DISPLAY_NORMAL)
    
    """Turn display off"""
    def displayOff(self):
        self.writeReg(RA8875_PWRR_ADDR, RA8875_PWRR_DISPLAY_OFF | RA8875_PWRR_DISPLAY_NORMAL)
    
    """Write Data to Register"""
    def writeReg(self, addr, val):
        self.writeCMD(addr)
        self.writeData(val)
        
    """Read Data from Register"""
    def readReg(self, addr):
        self.writeCMD(addr)
        return self.readData()
    
    """Write Data"""
    def writeData(self, _data):
        self.cs.value(0)
        #self.spi = SPI(2, baudrate=self.baudrate, miso=self.miso, mosi=self.mosi, sck=self.clk, polarity=0, phase=0)
        self.spi.write(bytearray([RA8875_DATAWRITE]))
        self.spi.write(bytearray([_data]))
        #self.spi.deinit()
        self.cs.value(1)
    
    """Read Data"""
    def readData(self):
        self.cs.value(0)
        #self.spi = SPI(2, baudrate=self.baudrate, miso=self.miso, mosi=self.mosi, sck=self.clk, polarity=0, phase=0)
        self.spi.write(bytearray([RA8875_DATAREAD]))
        spi_data = self.spi.read(1, 0x00)	# Write 1 byte data with the value of 0x00 and receieve the feedback
        #self.spi.deinit()
        self.cs.value(1)
        return spi_data
    
    """Write Command"""    
    def writeCMD(self, _cmd):
        self.cs.value(0)
        #self.spi = SPI(2, baudrate=self.baudrate, miso=self.miso, mosi=self.mosi, sck=self.clk, polarity=0, phase=0)
        self.spi.write(bytearray([RA8875_CMDWRITE]))
        self.spi.write(bytearray([_cmd]))
        #self.spi.deinit()
        self.cs.value(1)
    
    """Read Status"""
    def readStatus(self):
        self.cs.value(0)
        #self.spi = SPI(2, baudrate=self.baudrate, miso=self.miso, mosi=self.mosi, sck=self.clk, polarity=0, phase=0)
        self.spi.write(bytearray([RA8875_CMDREAD]))
        spi_data = self.spi.read(1, 0x00)	# Write 1 byte data with the value of 0x00 and receieve the feedback
        #self.spi.deinit()
        self.cs.value(1)
        return spi_data
    

