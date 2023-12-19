#include "displaySPI.h"
#include "ra8875_driver.h"

#define TAG "RA8875"
#define CLK_SPI_INIT  125000
#define CLK_SPI_WRITE 40000000      // (System Clock / 3)
#define CLK_SPI_READ  20000000      // (System Clock / 6)

/**********************
 *  STATIC PROTOTYPES
 **********************/

/************************************Display TFT**********************************************/

uint8_t readCMD(uint8_t cmd)
{
    uint8_t buf[4] = {RA8875_CMDWRITE, cmd, RA8875_DATAREAD, 0x00};
    disp_spi_transaction(buf, sizeof(buf), (disp_spi_send_flag_t)(DISP_SPI_RECEIVE | DISP_SPI_SEND_POLLING), buf, 0, 0);
    return buf[3];
}

void writeCMD(uint8_t cmd, uint8_t data)
{
    uint8_t buf[4] = {RA8875_CMDWRITE, cmd, RA8875_DATAWRITE, data};
    disp_spi_send_data(buf, sizeof(buf));
}

STATIC mp_obj_t init(size_t n_args, const mp_obj_t *args) {
    mp_int_t _miso = mp_obj_get_int(args[0]);
    mp_int_t _mosi = mp_obj_get_int(args[1]);
    mp_int_t _sck = mp_obj_get_int(args[2]);
    mp_int_t _cs = mp_obj_get_int(args[3]);
    mp_int_t _rst = mp_obj_get_int(args[4]);
    bool _int = mp_obj_is_true(args[5]);
    bool ret_val = true;

    // Your code here!
    unsigned int i = 0;

    struct {
        uint8_t cmd;                                   // Register address of command
        uint8_t data;                                  // Value to write to register
    } init_cmds[] = {
        {RA8875_PLLC1_ADDR,     (RA8875_PLLC1_DIV_1 + 11)},                         // System Clock: 60MHz (20*(11+1)/2^2)
        {RA8875_PLLC2_ADDR,     RA8875_PLLC2_DIV2},                                 // Div1: 240 MHz, Div2: 120 MHz, Div4: 60 MHz
        {RA8875_SYSR_ADDR,      (RA8875_SYSR_COLOR_16BPP | RA8875_SYSR_MCU_8BIT)},  // System Configuration Register (SYSR)
        {RA8875_PCSR_ADDR,      (RA8875_PCSR_PDAT_FALLING | RA8875_PCSR_2CLK)},     // 30 MHz = (System Clock / 2) = 60/2
        {RA8875_HDWR_ADDR,      ((800 / 8) - 1)},                                    // LCD Horizontal Display Width Register (HDWR)
        {RA8875_HNDFTR_ADDR,    (RA8875_HNDFTR_DE_HIGH + 3)},                       // Horizontal Non-Display Period Fine Tuning Option Register (HNDFTR)
        {RA8875_HNDR_ADDR,      ((26 - 3 - 2) / 7)},                                // Horizontal Non-Display Period Register (HNDR)
        {RA8875_HSTR_ADDR,      ((32 / 8) - 2)},                                    // HSYNC Start Position Register (HSTR)
        {RA8875_HPWR_ADDR,      (RA8875_HPWR_POLARITY_LOW + (96 / 8 - 12))},        // HSYNC Pulse Width Register (HPWR)
        {RA8875_VDHR0_ADDR,     ((uint16_t)(480 - 1 + 0) & 0xFF)},                 // LCD Vertical Display Height Register (VDHR0)
        {RA8875_VDHR1_ADDR,     ((uint16_t)(480 - 1 + 0) >> 8)},                   // LCD Vertical Display Height Register0 (VDHR1)
        {RA8875_VNDR0_ADDR,     (32 - 12)},                                         // LCD Vertical Non-Display Period Register (VNDR0)
        {RA8875_VNDR1_ADDR,     ((32 - 1) >> 8)},                                   // LCD Vertical Non-Display Period Register (VNDR1)
        {RA8875_VSTR0_ADDR,     (23 - 17)},                                         // VSYNC Start Position Register (VSTR0)
        {RA8875_VSTR1_ADDR,     ((23 - 1) >> 8)},                                   // VSYNC Start Position Register (VSTR1)
        {RA8875_VPWR_ADDR,      (RA8875_VPWR_POLARITY_LOW + 2 - 1)},                // VSYNC Pulse Width Register (VPWR)
        {RA8875_HSAW0_ADDR,     0},                                                 // Horizontal Start point is at 0
        {RA8875_HSAW1_ADDR,     (0 >>8)},
        {RA8875_HEAW0_ADDR,     ((uint16_t)(800 - 1) & 0xFF)},                      // Horizontal End point is at x
        {RA8875_HEAW1_ADDR,     ((uint16_t)(800 - 1) >> 8)},
        {RA8875_VSAW0_ADDR,     (0 + 0)},                                           // Vertical Start point is at 0
        {RA8875_VSAW1_ADDR,     ((0 + 0) >> 8)},
        {RA8875_VEAW0_ADDR,     ((uint16_t)(480 - 1 + 0) & 0xFF)},                  // Vertical End point is at y
        {RA8875_VEAW1_ADDR,     ((uint16_t)(480 - 1 + 0) >> 8)},
        {RA8875_MWCR0_ADDR,     0x00},                                              // Memory Write Control Register 0 (MWCR0)
        {RA8875_MWCR1_ADDR,     0x00},                                              // Memory Write Control Register 1 (MWCR1)
        {RA8875_LTPR0_ADDR,     0x00},                                              // Layer Transparency Register0 (LTPR0)
        {RA8875_LTPR1_ADDR,     0x00},                                              // Layer Transparency Register1 (LTPR1)
        {RA8875_MCLR_ADDR,      (RA8875_MCLR_START | RA8875_MCLR_FULL)}            // Clear Window
    };
    #define INIT_CMDS_SIZE (sizeof(init_cmds)/sizeof(init_cmds[0]))
    ESP_LOGI(TAG, "Initializing RA8875...");

    // Register Reset Pin
    gpio_config_t io_conf = {};    
    io_conf.intr_type = GPIO_INTR_DISABLE;          //disable interrupt
    io_conf.mode = GPIO_MODE_OUTPUT;                //set as output mode
    io_conf.pin_bit_mask = (1ULL<<_rst);
    io_conf.pull_down_en = 0;                       //disable pull-down mode
    io_conf.pull_up_en = 0;                         //disable pull-up mode
    gpio_config(&io_conf);                          //configure GPIO with the given settings

    // Register CS Pin
    io_conf.intr_type = GPIO_INTR_DISABLE;          //disable interrupt
    io_conf.mode = GPIO_MODE_OUTPUT;                //set as output mode
    io_conf.pin_bit_mask = (1ULL<<_cs);
    io_conf.pull_down_en = 0;                       //disable pull-down mode
    io_conf.pull_up_en = 0;                         //disable pull-up mode
    gpio_config(&io_conf);                          //configure GPIO with the given settings

    // Register Interrupt Pin
    io_conf.intr_type = GPIO_INTR_DISABLE;          //disable interrupt
    io_conf.mode = GPIO_MODE_INPUT;                 //set as input mode
    io_conf.pin_bit_mask = (1ULL<<_int);
    io_conf.pull_down_en = 0;                       //disable pull-down mode
    io_conf.pull_up_en = 1;                         //enable pull-up mode
    gpio_config(&io_conf);                          //configure GPIO with the given settings

    // Reset RA8875
    gpio_set_level(_rst, 0);
    vTaskDelay(1);
    gpio_set_level(_rst, 1);
    vTaskDelay(1);

    // SPI Bus configuration
    spi_bus_config_t buscfg={
        .miso_io_num=_miso,
        .mosi_io_num=_mosi,
        .sclk_io_num=_sck,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1
    };
    spi_device_interface_config_t *devcfg={
        .clock_speed_hz=CLK_SPI_INIT,
        .mode=0,
        .spics_io_num=_cs
    };
    disp_spi_add_device_config(VSPI_HOST, devcfg, buscfg);
    
    // Send all the commands
    for (i = 0; i < INIT_CMDS_SIZE; i++) {
        writeCMD(init_cmds[i].cmd, init_cmds[i].data);
    }

    // Turn display On
    writeCMD(RA8875_PWRR_ADDR, (RA8875_PWRR_DISPLAY_ON | RA8875_PWRR_DISPLAY_NORMAL));
    writeCMD(RA8875_P1CR_ADDR, RA8875_P1CR_ENABLE | (RA8875_PWM_CLK_DIV4096 & 0xF));

    return mp_obj_new_bool(ret_val);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(init_obj, 6, 6, init);
/*********************************************************************************************/

// Define all attributes of the module (the MicroPython object reference.)
// All identifiers and strings are written as MP_QSTR_xxx and will be
// optimized to word-sized integers by the build system (interned strings).
STATIC const mp_rom_map_elem_t module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_RA8875) },
    { MP_ROM_QSTR(MP_QSTR_INIT), MP_ROM_PTR(&init_obj) }
};
STATIC MP_DEFINE_CONST_DICT(module_globals, module_globals_table);

// Define module object.
const mp_obj_module_t RA8875_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&module_globals,
};

// Register the module to make it available in Python.
MP_REGISTER_MODULE(MP_QSTR_RA8875, RA8875_module);
