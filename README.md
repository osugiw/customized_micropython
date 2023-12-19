# Micropython

Created by Sugiarto Wibowo - R&D Embedded Software Engineer at PT. Hartono Istana Teknologi

![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled.png)

# Requirements

1. Ubuntu Environment or WSL2 in Windows
2. CMAKE version 3.16 or newer
3. Python3
4. USB Library

```bash
sudo apt install linux-tools-5.4.0-77-generic hwdata
```

1. Serial Emulator/Terminal

```bash
sudo apt-get install picocom
```

1. ESP-IDF version 5.0.2
    
    Install ESP-IDF dependencies:
    
    ```bash
    sudo apt-get install git wget flex bison gperf python3 python3-pip python3-venv cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0
    ```
    
    Create a directory, namely ‘esp’, then clone the repository inside the created folder.
    
    ```bash
    mkdir -p ~/esp
    cd ~/esp
    git clone -b v5.0.2 --recursive https://github.com/espressif/esp-idf.git
    ```
    
    You might notice that we use version v.5.0.2 instead of the latest version, v.5.2.0. We use the older version because the micropython framework is currently supported on ESP-IDF version 5.0.2. Some ESP-IDF components are deprecated and have different names in v.5.2.0; therefore, we will use v.5.0.2 instead for simplicity. Moreover, some functions inside the ESP-IDF components also changed on its parameters, name, etc.
    
    Next, we must set up the tools, including compiler, debugger, python packages, etc. For example, you must specify the ESP chip type to compile for your ESP32, ESP32S3.
    
    ```bash
    cd ~/esp/esp-idf
    ./install.sh esp32,esp32s3
    ```
    
    or
    
    ```bash
    cd ~/esp/esp-idf
    ./install.sh all
    ```
    
    You already installed the ESP-IDF framework at this point, but it couldn’t be used yet because we haven’t added it to the PATH environment variable. To make it usable, we need to export the path of our ESP-IDF, and for future usage, we could make an alias in our shell’s profile (.profile).
    
    ```bash
    nano ~/.profile
    ```
    
    Add the following commands to your shell’s profile:
    
    ```bash
    alias get_idf=’. $HOME/esp/esp-idf/export.sh’
    ```
    
    After that, save the configuration and refresh the terminal session, then call the get_idf by entering this command:
    
    ```bash
    source ~/.profile
    get_idf
    ```
    
    Every time you want to compile or use ESP-IDF, call **get_idf** from your terminal to activate the ESP-IDF.
    
2. Micropython repository
    
    Installing micropython is a piece of cake; we just need to clone the repository and compile the cross-compiler. First, download the repository and save it to a directory, ‘micropython’.
    
    ```bash
    git clone --recursive https://github.com/osugiw/micropython.git micropython
    ```
    
    Next, we must compile a cross-compiler because most devices require a Micropython cross-compiler or mpy-cross.
    
    ```bash
    cd micropython/mpy-cross
    make
    ```
    
    At this point, we could compile our customized firmware for devices located in **micropython/ports.** Before we delve deeper into creating and compiling C modules and their dependencies, we need to understand the micropython root folder’s structure first.
    
    ![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%201.png)
    
    Let’s take a closer look inside our target device folder, in this case, is **esp32**:
    
    ![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%202.png)
    
    For the first time, please do not edit the Makefile or CMakeLists.txt; leave all configurations by default and compile the firmware. Next, head toward the **micropython/ports/esp32/** and enter the following commands to compile.
    
    ```bash
    get_idf
    make BOARD=GENERIC all
    ```
    
    If you successfully compile, you will have no error message, and there will be a folder, namely **build-ESP32_GENERIC,** and inside that folder, you will find **firmware.bin,** which is the result of the compiling process that could be flashed to our device. Next, you can deploy the image using **Make** command, but you must ensure that our device is already attached to the WSL. To attach our device to the WSL, open Windows Powershell with administrator privilege first, then list the COMs attached to our computer and pay attention to the **bus-id**. After you find the **bus-id** you can attach the device to our WSL. Below is the command to list the attached device and how to attach the device:
    
    ```bash
    usbipd wsl list
    usbipd wsl attach –busid <busid of your device>
    ```
    
    ![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%203.png)
    
    Our device is already attached to the WSL, and we can continue to deploy our compiled firmware to our device using **Make.** In this case, because my target device is ESP32, the system will detect it as ttyUSB0, but if your device is ESP32S3, the system will detect it as ttyACM0. Enter the following command to deploy our firmware to the device and connect to the Python REPL:
    
    ```bash
    make deploy
    picocom -b 115200 /dev/ttyUSB0
    ```
    
    Congratulations, you already built your micropython firmware for ESP32. The next step is to create a frozen module and extend the C module.
    

![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%204.png)

# **Frozen Module**

Micropython has a feature to freeze the Python code into the firmware. This feature will compile Python code (.py) into bytecode (.mpy), which can then be imported as a library. Some key benefits of this method include:

- The Python code will be compiled into a bytecode, which makes the MCU computation lighter because it doesn’t need to be compiled at load time.
- The bytecode is directly compiled from the ROM (i.e., flash memory) rather than being copied into the RAM, which makes more memory available for the application.
- Enabling devices that don’t have a filesystem to load a Python code.

To compile a frozen module, we need to define our module in the [manifest.py](http://manifest.py/), which, by default, is located inside the board folder as follows:

![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%205.png)

A manifest file is a Python file containing a series of function calls. Some of the available functions for creating a custom manifest file include:

•  High-Level Functions

![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%206.png)

![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%207.png)

•  Low-Level Functions

![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%208.png)

```python
freeze("$(PORT_DIR)/modules")
include("$(MPY_DIR)/extmod/asyncio")

# Useful networking-related packages.
require("bundle-networking")

# Require some micropython-lib modules.
# require("aioespnow")
require("dht")
require("ds18x20")
require("neopixel")
require("onewire")
require("umqtt.robust")
require("umqtt.simple")
require("upysh")
```

By default, the manifest file is shown in above codes. In line 1, it has written our /modules folder; for trying purposes, please locate our code inside the modules folder. At this point, we need to compile using the make command. However, there might be circumstances when you need to make your own manifest file. Your customized manifest file should be located outside the micropython root folder. To include your customized manifest file, add the path of the file when compiling by adding the argument of FROZEN_MANIFEST. For example:

```bash
make BOARD=ESP32 FROZEN_MANIFEST=/path/to/manifest.py
```

In this example, we will create a Python code to create a JSON file if the file doesn’t exist or open the file and print the content inside. Create a Python file inside /modules with the code, as shown in codes below:

```python
import ujson

def myFile():
    """Ini adalah Fungsi operasi myFile dengan try except error handling"""
    try:
        """Ini adalah operasi untuk mencari file dan membaca file"""
        with open("data.txt", "rt") as file:
            data = file.read()
            print(data)
    except Exception:
        """Jika File tidak di temukan maka akan membuat file "data.json" dengan kata 'Hello World'"""
        with open("data.txt", "wt") as file:
            file.write("Hello World\n")
            
def myFileJson():
    """Ini adalah Fungsi operasi myFile dengan try except error handling"""
    try:
        """Ini adalah operasi untuk mencari file dan membaca file"""
        with open("data.json", "rt") as file:
            data = file.read()
            dataJson = ujson.loads(data)
            print(dataJson)
            print(dataJson["voltage"])
            
    except Exception:
        """Jika File tidak di temukan maka akan membuat file "data.json" dengan json object (dataJson) """
        with open("data.json", "wt") as file:
            dataJson = ujson.loads("""{"voltage":12.05}""")
            file.write(str(ujson.dumps(dataJson)))
```

Then, compile the firmware using the make command. Upon finishing compiling the firmware, you can test out the frozen module by calling it through the Python shell or Thonny IDE.

![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%209.png)

# Extending Micropython in C using External C modules

There are two methods to create C files as a module in micropython. The first method is by **using external C modules**. This method requires us to bind the code with Micropython and compile it with the firmware. The module must contain the source code and the Makefile.

![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%2010.png)

It is mandatory to **bind your code with micropython**; otherwise, Micropython won’t recognize the module. Look at the following snippet codes and notice that in the beginning, we include files of “py/runtime.h” and “py/obj.h”. **This file contains APIs for binding with micropython**. For example, on line 5, we create a function called “subtract” using mp_obj_t; inside the function, we declare an integer variable using mp_obj_get_int. For the Micropython to recognize our module, we need to define **our function as the function object**. In this example, the function object accepts two arguments, which we could use up to four arguments by changing the number (MP_DEFINE_FUN_OBJ_x). If we have a function that receives more than four arguments, we could use MP_DEFINE_CONST_FUN_OBJ_VAR(obj_name, n_args_min, fun_name)

```c
// Include MicroPython API.
#include "py/runtime.h"
#include "py/obj.h"

/************************************************************************************************/
/*                                      Function                                                */
/************************************************************************************************/
// This is the function which will be called from Python as math_test.subtract(a, b).
STATIC mp_obj_t subtract(mp_obj_t a_obj, mp_obj_t b_obj) {
    // Extract the ints from the micropython input objects.
    int a = mp_obj_get_int(a_obj);
    int b = mp_obj_get_int(b_obj);
    
    return mp_obj_new_int(a - b);
}
// Define a Python reference to the function above.
STATIC MP_DEFINE_CONST_FUN_OBJ_2(subtract_obj, subtract);

// Define all attributes of the module (the MicroPython object reference.)
// All identifiers and strings are written as MP_QSTR_xxx and will be
// optimized to word-sized integers by the build system (interned strings).
STATIC const mp_rom_map_elem_t example_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_math_test) },
    { MP_ROM_QSTR(MP_QSTR_subtract), MP_ROM_PTR(&subtract_obj) },
};
STATIC MP_DEFINE_CONST_DICT(example_module_globals, example_module_globals_table);

// Define module object.
const mp_obj_module_t math_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&example_module_globals,
};

// Register the module to make it available in Python.
MP_REGISTER_MODULE(MP_QSTR_math_test, math_module);
```

After defining it as a function object, we need to **register the function object to the global module** using mp_rom_map_elem_t. At the end of the code, you **need to register the module name as the module object to the micropython.** Otherwise, you couldn’t call your module from micropython.

Code below shows the snippet code of the module cmake file, which contains the module’s configuration. In line 2, we need to create an interface (a.k.a. library); in this example, I named it “math_test,” and then we need to add the path of our C file to the interface we already made. Next, in line 10, we define the path of our folder to include other files, such as a header (.h) file. Finally, in line 15, we register our interface to the usermod. Usermod is a cmake file that will be called when we are compiling for firmware, **usermod is a kind of tool that help us to add user-defined module** or library easily. It was located inside micropython/py/usermod.cmake.

```c
# Create an INTERFACE library for our C module.
add_library(math_test INTERFACE)

# Add our source files to the lib from current directory
target_sources(math_test INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/math_test.c
)

# Add the current directory as an include directory.
target_include_directories(math_test INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Link our INTERFACE library to the usermod target.
target_link_libraries(usermod INTERFACE math_test)
```

The next step is to add the source file to SRC_USERMOD. Lastly, you need to add the path of your module’s Makefile to the main module’s Makefile.

```c
MATH_TEST_MOD_DIR := $(USERMOD_DIR)

# Add all C files to SRC_USERMOD.
SRC_USERMOD += $(MATH_TEST_MOD_DIR)/math_test.c

# We can add our module folder to include paths if needed
# This is not actually needed in this example.
CFLAGS_USERMOD += -I$(MATH_TEST_MOD_DIR)
```

Suppose you have already done with your module code and configuration. In that case, you can continue to **compile the program on your target device and deploy it**. Before compiling, you need to be inside the folder of a specific device; for instance, if we want to deploy our firmware on ESP32, we need to change the directory to ..**/micropython/ports/esp32/.** Notice that the compiling process uses Makefile. Moreover, when compiling, we need to add **USER_C_MODULES** parameter; this tells the Makefile if we want to add our custom library to the firmware. There is another parameter that we could add, which is **BOARD,** this will tell the Makefile our board type of ESP32, such as ESP32_GENERIC_S3 (ESP32S3), ESP32_GENERIC_C3, (ESP32C3), etc., and by default, it is ESP32_GENERIC. In this example, I will deploy my firmware on ESP32S3; here is the command that I will run:

```bash
Make BOARD=ESP32_GENERIC_S3 USER_C_MODULES=/home/osugiw/micropython/examples/usercmodule/micropython.cmake
```

![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%2011.png)

If compiled successfully, then you could deploy it to your board using the command below:

```bash
esptool.py --chip esp32s3 --port /dev/ttyACM0 --baud 460800 write_flash -z 0x0 build-GENERIC_S3/firmware.bin
```

To simplify the command, create an alias inside the makefile “deploy_S3” which calls the flashing command. When flashing using esptool we need to tell which chip we will use, the port name, baud rate, the address of the chip ID, and the path of the compiled firmware. The compiled program will create a folder that contains folders and files, including firmware.bin that could be used while flashing in IDE.

![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%2012.png)

After completing the compile and flash process, you could test your C library in Micropython. I use picocom to connect to the Python REPL; after you enter the REPL, test our module by importing it and calling the function.

![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%2013.png)

# **Extending Micropython in C using Native Machine Code**

The second method for including C in our micropython uses **Native Machine Code.** It is a bit different from the first method because this method **doesn’t require us to include our module while compiling the firmware**. In other words, we could compile it separately and add it to the device’s filesystem; it is **more dynamic and flexible than External C Module (first method).** Before we continue to delve deeper, we need some requirements to install. Some of the requirements include the following:

```bash
pip install Cython
pip install pyelftools
```

Next, we will continue to create a native module. The module was built inside the ports/esp32/for testing purposes since our target is the ESP32 board. We will create a native module called **multiplication**, which functions to multiply two input numbers from the user. We need **at least a source file** and **a Makefile inside the module folder**. The source file contains our code for multiplying between two numbers, while the Makefile contains configurations for compiling.

![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%2014.png)

We need to configure some parameters such **as the root path of micropython,** **our native module name, our native module source file name**, and **device target architecture** and include micropython rules for compiling and linking the module.

```c
# Location of top-level MicroPython directory
MPY_DIR = /home/osugiw/micropython

# Name of module
MOD = multiplication

# Source files (.c or .py)
SRC = main.c

# Architecture to build for (x86, x64, armv6m, armv7m, xtensa, xtensawin)
ARCH = xtensawin

# Include to get the rules for compiling and linking the module
include $(MPY_DIR)/py/dynruntime.mk
```

After configuring the Makefile, we can continue to code. Initially, we must **include “py/dyruntime.h” and “py/obj.h” for Micropython APIs access.** In line 5, we make a function of integer to **return a multiplication value between x and y variables**. Next, we need to **register our integer function as an object.** Finally, in line 26, we need to add our function object to the mpy to use it as a module in micropython.

```c
// Include the header file to get access to the MicroPython API
#include "py/dynruntime.h"
#include "py/obj.h"

// Helper function to compute multiplication
STATIC mp_int_t multiplication_helper(mp_int_t x, mp_int_t y) {
    if (x == 0 || y ==0) {
        return 1;
    }
    return x * y;
}

// This is the function which will be called from Python, as multiplication(x)
STATIC mp_obj_t multiplication(mp_obj_t x_obj, mp_obj_t y_obj) {
    // Extract the integer from the MicroPython input object
    mp_int_t x = mp_obj_get_int(x_obj);
    mp_int_t y = mp_obj_get_int(y_obj);
    // Calculate the multiplications
    mp_int_t result = multiplication_helper(x, y);
    // Convert the result to a MicroPython integer object and return it
    return mp_obj_new_int(result);
}
// Define a Python reference to the function above
STATIC MP_DEFINE_CONST_FUN_OBJ_2(multiplication_obj, multiplication);

// This is the entry point and is called when the module is imported
mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    // This must be first, it sets up the globals dict and other things
    MP_DYNRUNTIME_INIT_ENTRY

    // Make the function available in the module's namespace
    mp_store_global(MP_QSTR_multiplication, MP_OBJ_FROM_PTR(&multiplication_obj));

    // This must be last, it restores the globals dict
    MP_DYNRUNTIME_INIT_EXIT
}
```

After we create our code inside the source file, we can **compile it by specifying our target device architecture**. Because we use ESP32, then we will use **ARCH=xtensawin**. Currently, the supported architectures are as follows:

![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%2015.png)

Then, compile it by entering this command:

```bash
make ARCH=xtensawin
```

If you successfully compiled the module, **you will have a build folder and .mpy fil**e. The .mpy file contains our compiled module that could be inserted into micropython system files.

![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%2016.png)

Finally, we could use our native module by uploading .mpy file to the device file system. Then try to import our module in the shell and call the function for multiplying two numbers.

![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%2017.png)

# GPIO Module in C Code

You might be wondering how to create a module that could control the **device’s GPIOs**. Indeed, it is possible by using the first method, **External C Modules.** When we used the first method, the ESP-IDF was compiled with Micropython framework; therefore, we could access the ESP-IDF APIs. Let’s walk through how to control GPIO on ESP32 using our module. First, we need to **include files from the ESP-IDF APIs for controlling GPIOs**. Then, we could use the APIs to interface the input or output pin.

The first function to control an LED, namely **digitalWrite**. The digitalWrite function accepts two parameters: the GPIO Pin Number and the value (High or Low). We need to initialize the output pin and set the output level inside the function. While the **digitalRead function** requires a PIN parameter, inside the function, we need to initialize the pin as an input peripheral and then return the read value to the console. Lastly, we must define our function as an object and register it to the module.

```c
// Include MicroPython API.
#include "py/runtime.h"
#include "py/obj.h"
#include "driver/gpio.h"

/************************************************************************************************/
/*                                      Digital GPIO                                            */
/************************************************************************************************/
STATIC mp_obj_t digitalWrite(mp_obj_t outputPin_obj, mp_obj_t _value_obj) {
    mp_int_t _pin = mp_obj_get_int(outputPin_obj);
    mp_int_t _value = mp_obj_get_int(_value_obj);
    
    gpio_config_t io_conf     = {};
    io_conf.intr_type       = GPIO_INTR_DISABLE;        // Disable interrupt
    io_conf.mode            = GPIO_MODE_OUTPUT;         // Set as output mode
    io_conf.pin_bit_mask    = (1ULL<<_pin);
    io_conf.pull_down_en    = 0;                        // Disable pull-down mode
    io_conf.pull_up_en      = 0;                        // Disable pull-up mode
    gpio_config(&io_conf);                              // Configure GPIO with the given settings

    gpio_set_level(_pin, _value);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(digitalWrite_obj, digitalWrite);

STATIC mp_obj_t digitalRead(mp_obj_t inputPin_obj) {
    mp_int_t _pin = mp_obj_get_int(inputPin_obj);
    
    gpio_config_t io_conf     = {};
    io_conf.intr_type       = GPIO_INTR_DISABLE;       // Disable interrupt
    io_conf.mode            = GPIO_MODE_INPUT;         // Set as output mode
    io_conf.pin_bit_mask    = (1ULL<<_pin);
    io_conf.pull_down_en    = 0;                        // Disable pull-down mode
    io_conf.pull_up_en      = 1;                        // Enable pull-up mode
    gpio_config(&io_conf);                              // Configure GPIO with the given settings

    bool readValue = gpio_get_level(_pin);
    return mp_obj_new_bool(readValue);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(digitalRead_obj, digitalRead);

// Define all attributes of the module (the MicroPython object reference.)
// All identifiers and strings are written as MP_QSTR_xxx and will be
// optimized to word-sized integers by the build system (interned strings).
STATIC const mp_rom_map_elem_t example_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_math_test) },
    { MP_ROM_QSTR(MP_QSTR_digitalWrite), MP_ROM_PTR(&digitalWrite_obj) },
    { MP_ROM_QSTR(MP_QSTR_digitalRead), MP_ROM_PTR(&digitalRead_obj) },
};
STATIC MP_DEFINE_CONST_DICT(example_module_globals, example_module_globals_table);

// Define module object.
const mp_obj_module_t gpio__module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&example_module_globals,
};

// Register the module to make it available in Python.
MP_REGISTER_MODULE(MP_QSTR_math_test, math_module);
```

Then you need to include the source files and directory to be compiled in the cmake and makefile of the module.

```c
# Create an INTERFACE library for our C module.
add_library(gpio_module INTERFACE)

# Add our source files to the lib from current directory
target_sources(gpio_module INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/main.c
)

# Add the current directory as an include directory.
target_include_directories(gpio_module INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Link our INTERFACE library to the usermod target.
target_link_libraries(usermod INTERFACE gpio_module)
```

```c
GPIO_MODULE_MOD_DIR := $(USERMOD_DIR)

# Add all C files to SRC_USERMOD.
SRC_USERMOD += $(GPIO_MODULE_MOD_DIR)/main.c

# We can add our module folder to include paths if needed
# This is not actually needed in this example.
CFLAGS_USERMOD += -I$(GPIO_MODULE_MOD_DIR)
```

Then you can compile, like in the second method, change your directory to micropython/ports/esp32/ and compile using make with **USER_C_MODULE** parameter. For example:

```bash
make BOARD=GENERIC USER_C_MODULES=/home/osugiw/custom-micropython-modules/c/external_c_modules/micropython.cmake clean all
```

Then, deploy to the board using **make deploy** and connect to the device through REPL. Try to import the module to turn the LED On/Off and read the push button value.

![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%2018.png)

![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%2019.png)

# Interfacing I2C as External C Module

Sometimes, we need to communicate with sensors or other devices through the I2C Communication. **I2C is a serial communication between the master and slaves; when the master wants to send data to the slave, the master needs the slave’s address.** In Micropython, we can also implement I2C as a module, and we will walk through the steps by using two ESP32 devices. Initially, create the module folder with its files. In this example, I divided the source codes into several files, including one for the master, one for the slave, and one for configuring the master and slave APIs as a module.

![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%2020.png)

The **header file contains the library and APIs for the I2C**. Also, it defines some macros that are used for I2C configurations. We must also define **the function as the extern** since we will use it across files.

```cpp
// Include MicroPython API.
#include "py/runtime.h"
#include "py/obj.h"

// ESP-IDF APIs
#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"

// Macros
#define DATA_LENGTH 512                                         /*!< Data buffer length of test buffer */
#define RW_TEST_LENGTH 128                                      /*!< Data length for r/w test, [0,DATA_LENGTH] */
#define DELAY_TIME_BETWEEN_ITEMS_MS 1000                        /*!< delay time between different test items */

#define I2C_MASTER_TX_BUF_DISABLE 0                             /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0                             /*!< I2C master doesn't need buffer */
#define I2C_SLAVE_TX_BUF_LEN (2 * DATA_LENGTH)                  /*!< I2C slave tx buffer size */
#define I2C_SLAVE_RX_BUF_LEN (2 * DATA_LENGTH)                  /*!< I2C slave rx buffer size */

#define WRITE_BIT I2C_MASTER_WRITE                              /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ                                /*!< I2C master read */
#define ACK_CHECK_EN 0x1                                        /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0                                       /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                                             /*!< I2C ack value */
#define NACK_VAL 0x1                                            /*!< I2C nack value */

// Function Declaration
/************************************I2C MASTER****************************************/
extern mp_obj_t master_init(mp_obj_t _sda_obj, mp_obj_t _scl_obj, mp_obj_t _freq_obj);
extern mp_obj_t master_write(mp_obj_t _slave_addr_obj, mp_obj_t _data_obj);

/************************************I2C SLAVE****************************************/
extern mp_obj_t slave_init(mp_obj_t _sda_obj, mp_obj_t _scl_obj, mp_obj_t _slaveAddr_obj);
extern mp_obj_t slave_read();
```

We will **initialize the master peripherals, which** receive **three parameters: SDA, SCL pins, and clock frequency**. The received parameters are used for configuring the **I2C controller**, as shown in lines 19 to 26. The function **for the master to write data to the slave is in line 44**. To write data to the slave, the user must **specify the slave’s address** and **the data as a list object**.

```c
// Include Header
#include <i2c_module.h>

/**
*   @brief Iniialize I2C Master
*   @param  _sda_obj     Master SDA Pin
*   @param  _scl_obj     Master SCL Pin 
*   @param  _freq_obj    I2C Frequency 
**/
mp_obj_t master_init(mp_obj_t _sda_obj, mp_obj_t _scl_obj, mp_obj_t _freq_obj) {
    mp_int_t _sda = mp_obj_get_int(_sda_obj);
    mp_int_t _scl = mp_obj_get_int(_scl_obj);
    mp_int_t _freq = mp_obj_get_int(_freq_obj);

    // I2C Port Number
    int i2c_master_port = 1;    //  0(ESP32C3, ESP32C2, ESP32H2) , 1(ESP32/ESP32S2/ESP32S3)

    // I2C Configuration
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = _sda,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = _scl,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = _freq,
        // .clk_flags = 0,          /*!< Optional, you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here. */
    };

    // Error checking on the I2C Configuration
    esp_err_t err = i2c_param_config(i2c_master_port, &conf);
    if(err != ESP_OK){
        printf("Error master configuration - Error Code %d\n", err);
        return mp_obj_new_int(err);
    }
    return mp_obj_new_int(i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0));
}

/**
*   @brief I2C Master Write
*   @param  _i2c_port_num_obj        I2C Port Number
*   @param  _data_obj                Data to write 
*   @param  _size_obj                Data Length 
**/
mp_obj_t master_write(mp_obj_t _slave_addr_obj, mp_obj_t _data_obj){
    mp_int_t _slave_addr = mp_obj_get_int(_slave_addr_obj);
    mp_obj_t *_data = NULL;
    size_t _data_len = 0;
    mp_obj_get_array(_data_obj, &_data_len, &_data);

    // Save the received data into the buffer
    uint8_t* data_wr = (uint8_t *) malloc(DATA_LENGTH);
    for(int i=0; i<_data_len; i++){
        data_wr[i] = mp_obj_get_int(_data[i]);
    }

    // Your code here!
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();                               // Create and initialize an I2C commands list with a given buffer.
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (_slave_addr << 1) | WRITE_BIT, ACK_CHECK_EN);   // Send a byte to slave
    i2c_master_write(cmd, data_wr, _data_len, ACK_CHECK_EN);                    // Queue data
    i2c_master_stop(cmd);

    esp_err_t err = i2c_master_cmd_begin(1, cmd, 1000 / portTICK_PERIOD_MS);    // Send All queued commands
    i2c_cmd_link_delete(cmd);                                                   // Finished I2C Transaction

    return mp_obj_new_int(err);
}
```

While the **slave initialization function** **accepts parameters of SDA, SCL pins, and the slave address**. The parameters are then used for configuring **the slave peripherals** and **then installed on the I2C controller.** We also create a function to listen to data from the master and print the data to the console.

```c
// Include Header
#include <i2c_module.h>

/**
*   @brief Iniialize I2C Slave
*   @param _sda_obj     Slave SDA Pin
*   @param _scl_obj     Slave SCL Pin 
*   @param _freq_obj    I2C Frequency 
**/
mp_obj_t slave_init(mp_obj_t _sda_obj, mp_obj_t _scl_obj, mp_obj_t _slaveAddr_obj) {
    mp_int_t _sda = mp_obj_get_int(_sda_obj);
    mp_int_t _scl = mp_obj_get_int(_scl_obj);
    mp_int_t _slaveAddr = mp_obj_get_int(_slaveAddr_obj);

    // I2C Port Number
    int i2c_slave_port = 0;   

    // I2C Configuration
    i2c_config_t conf = {
        .mode = I2C_MODE_SLAVE,
        .sda_io_num = _sda,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = _scl,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .slave.addr_10bit_en = 0,
        .slave.slave_addr = _slaveAddr,
    };

    // Error checking on the I2C Configuration
    esp_err_t err = i2c_param_config(i2c_slave_port, &conf);
    if(err != ESP_OK){
        printf("Error slave configuration - Error Code %d\n", err);
        return mp_obj_new_int(err);
    }

    return mp_obj_new_int(i2c_driver_install(i2c_slave_port, conf.mode, I2C_SLAVE_RX_BUF_LEN, I2C_SLAVE_TX_BUF_LEN, 0));
}

/**
*   @brief I2C Slave Read
**/
mp_obj_t slave_read() {
    uint8_t *data = (uint8_t *)malloc(DATA_LENGTH);
    int size = i2c_slave_read_buffer(0, data, RW_TEST_LENGTH, 1000 / portTICK_PERIOD_MS);

    // Print recieved data
    for(int i=0; i<size; i++){
        printf("%02x ", data[i]);
        if((i+1) % 16 == 0){
            printf("\n");
        }
    }
    printf("\n");
    return mp_const_none;
}
```

After all, **we must register our functions as function objects and create QSTR referencing the corresponding function objects inside a dictionary**.

```c
 // Include Header
#include <i2c_module.h>

/************************************************************************************************/
/*                                      I2C SLAVE                                               */
/************************************************************************************************/
MP_DEFINE_CONST_FUN_OBJ_3(slave_init_obj, slave_init);
MP_DEFINE_CONST_FUN_OBJ_0(slave_read_obj, slave_read);

/************************************************************************************************/
/*                                      I2C MASTER                                              */
/************************************************************************************************/
MP_DEFINE_CONST_FUN_OBJ_3(master_init_obj, master_init);
MP_DEFINE_CONST_FUN_OBJ_2(master_write_obj, master_write);

// Define all attributes of the module (the MicroPython object reference.)
// All identifiers and strings are written as MP_QSTR_xxx and will be
// optimized to word-sized integers by the build system (interned strings).
STATIC const mp_rom_map_elem_t module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_i2c_driver) },
    { MP_ROM_QSTR(MP_QSTR_master_init), MP_ROM_PTR(&master_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_master_write), MP_ROM_PTR(&master_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_slave_init), MP_ROM_PTR(&slave_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_slave_read), MP_ROM_PTR(&slave_read_obj) },
};
STATIC MP_DEFINE_CONST_DICT(module_globals, module_globals_table);

// Define module object.
const mp_obj_module_t i2c_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&module_globals,
};

// Register the module to make it available in Python.
MP_REGISTER_MODULE(MP_QSTR_i2c_module, i2c_module);
```

For example, the **MP_QSTR_master_init will be referenced to master_init function**. Finally, we need to register the dictionary to the Micropython module using macros of MP_REGISTER_MODULE. We also must define our source codes path and the module name to the micropython.cmake and micropython.mk.

```c
# Create an INTERFACE library for our C module.
add_library(i2c_module INTERFACE)

# Add our source files to the lib from current directory
target_sources(i2c_module INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/i2c_module.c
    ${CMAKE_CURRENT_LIST_DIR}/i2c_master.c
    ${CMAKE_CURRENT_LIST_DIR}/i2c_slave.c
)

# Add the current directory as an include directory.
target_include_directories(i2c_module INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Link our INTERFACE library to the usermod target.
target_link_libraries(usermod INTERFACE i2c_module)
```

```c
I2C_MODULE_MOD_DIR := $(USERMOD_DIR)

# Add all C files to SRC_USERMOD.
SRC_USERMOD += $(I2C_MODULE_MOD_DIR)/i2c_module.c
SRC_USERMOD += $(I2C_MODULE_MOD_DIR)/i2c_master.c
SRC_USERMOD += $(I2C_MODULE_MOD_DIR)/i2c_slave.c

# We can add our module folder to include paths if needed
# This is not actually needed in this example.
CFLAGS_USERMOD += -I$(I2C_MODULE_MOD_DIR)
```

Ultimately, you can test the I2C module by deploying it to two ESP32s. **One acts as the master and the other as the slave.** On the master side, you **must initialize the peripheral with the required SDA, SCL pin, and clock frequency parameters.** On the slave side, **the slave also needs to initialize the SDA, SCL pin, and address**. After being initialized, the slave can continue to listen to the I2C line. After both side is initialized, the **master can write to the slave by specifying the slave’s address and list of data, and the slave can read the data from the master**.

![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%2021.png)

# Using ADC in the External C Module

One of the most common peripherals in embedded devices is ADC. We will experiment using ADC to **read the temperature sensor’s analog value using our External C Module**. To configure ADC peripherals, besides **the channel number, we also need to determine the ADC unit, resolution, attenuation level, and conversion modes**. In ESP32**, the available conversion modes are single-shot and continuous**. In this example, we use the **single-shot conversion mode** to read the analog data from the NTC sensor with the specification of 5K at room temperature. Like before, we need to create the module folder containing source codes and makefile configurations.

![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%2022.png)

Before writing the code, we need to **comment out the default ADC library by Micropython**, since **they use a deprecated ADC library or ESP32. Instead of the deprecated one,** we will use the **newer library of ESP32 ADC**. Moreover, **both libraries can’t be called together** because it **will cause a conflict**. To turn off the default library, you can comment **lines 313-314 in ../ports/esp32/modmachine.c**. 

![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%2023.png)

Also, **comment out lines 69 to 70 in ../ports/esp32/esp32_common.cmake**.

![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%2024.png)

After turning off the default ADC code, you can continue to **work on the NTC module code.** Initially, include all the required APIs of the newer ESP ADC library and others. We also declare enum data for the data threshold that will be used for **converting analog data to temperature**. The ‘**init_adc_calibration**’ is used to **initialize and calibrate the ADC peripherals**. This calibration **aims to minimize the effect of difference reference voltages and increase the accuracy.**

There are two types of calibrations**, curve, and line fitting**; all the **ESP32 support line fitting but not curve fitting.** We use **curve fitting in this example since I use ESP32S3, which supports this scheme.** We will create a function to detach the calibration. Besides that, we also **create a function to convert the analog value to temperature in Celsius**. Then, we create **a function object that receives the channel number parameter**, which is used to configure the ADC peripheral channel in the ESP32. Lines 126 to 136 configure the **ADC one-shot conversion, channel configuration resolution, and attenuation (determines the voltage level).** In line 140, we call the calibration function to **calibrate the voltage level** and then **continue to read analog data from the selected channel**. The analog value is then **converted into temperature in Celsius**. Lastly, we register the function object to the Micropython module.

```c
// Include MicroPython API.
#include "py/runtime.h"
#include "py/obj.h"

// ESP-IDF APIs
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

// Other library
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define ADC_DIVIDER                  8
#define EXAMPLE_ADC_ATTEN           ADC_ATTEN_DB_11

static int adc_raw[2][10];
static int voltage[2][10];

// NTC Threshold after Divided
enum{
    ADC_MIN40		= 483,
    ADC_MIN20       = 424,
	ADC_0			= 316,
	ADC_PLUS20		= 195,
	ADC_PLUS40		= 107,
	ADC_PLUS60		= 56,
	ADC_PLUS80		= 30,
	ADC_PLUS100		= 17,
	ADC_PLUS120		= 10,
	ADC_PLUS150		= 5,
};

/**
*   @brief Initialize calibration on ADC pin
*   @param adc_unit        ADC Channel Number
*   @param atten           Level of attenuation on target adc_unit
*   @param out_handle      ADC Calibration handle 
**/
static bool init_adc_calibration(adc_unit_t unit, adc_atten_t atten, adc_cali_handle_t *out_handle)
{
    adc_cali_handle_t handle = NULL;
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

    if (!calibrated) {
        printf( "calibration scheme version is %s", "Curve Fitting");
        adc_cali_curve_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_12,
        };
        ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }

    *out_handle = handle;
    if (ret == ESP_OK) {
        printf( " - Calibration Success\n");
    } else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated) {
        printf("- eFuse not burnt, skip software calibration\n");
    } else {
        printf("- Invalid arg or no memory\n");
    }
    return calibrated;
}

/**
*   @brief DeInitialize calibration
*   @param handle      ADC Calibration handle 
**/
static void uninit_adc_calibration(adc_cali_handle_t handle)
{
    printf( "deregister %s calibration scheme", "Curve Fitting");
    ESP_ERROR_CHECK(adc_cali_delete_scheme_curve_fitting(handle));
}

/**
*   @brief Convert raw value to temperature (Celsius)
*   @param raw_data     ADC Raw data
**/
int8_t temperature_conversion_ntc_5k(uint16_t raw_data)
{
    uint16_t divided = raw_data / ADC_DIVIDER;
    uint8_t _celsius = 0;

    if(divided <= ADC_MIN40 && divided >= ADC_MIN20)
		_celsius =  ((-0.3336 * divided) + 122.61);
	else if(divided < ADC_MIN20 && divided >= ADC_0)
		_celsius = ((-0.1825 * divided) + 58.187);
	else if(divided < ADC_0 && divided >= ADC_PLUS20)
		_celsius = ((-0.1644 * divided) + 51.868);
	else if(divided < ADC_PLUS20 && divided >= ADC_PLUS40)
		_celsius = ((-0.2274 * divided) + 63.628);
    else if(divided < ADC_PLUS40 && divided >= ADC_PLUS60)
		_celsius = ((-0.3992 * divided) + 81.552);
    else if(divided < ADC_PLUS60 && divided >= ADC_PLUS80)
		_celsius = ((-0.7771 * divided) + 102.49);
    else if(divided < ADC_PLUS80 && divided >= ADC_PLUS100)
		_celsius = ((-1.5101 * divided) + 124.3);
    else if(divided < ADC_PLUS100 && divided >= ADC_PLUS120)
		_celsius = ((-2.9107 * divided) + 147.44);
	else if(divided < ADC_PLUS120 && divided >= ADC_PLUS150)
		_celsius = ((0.5588 * pow(divided, 2)) - (14.089 * divided) + 204.67);
	return _celsius;
}

/**
*   @brief Read and inialize data from ADC Pin
*   @param channel_number        ADC Channel Number
**/
STATIC mp_obj_t ntc_5k(mp_obj_t _channel_number_obj) {
    mp_int_t _channel_number = mp_obj_get_int(_channel_number_obj);
    uint8_t channel_number = (uint8_t)_channel_number;

    //-------------ADC1 Init---------------//
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));
    
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = EXAMPLE_ADC_ATTEN,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, channel_number, &config));

    //-------------ADC1 Calibration Init---------------//
    adc_cali_handle_t adc1_cali_handle = NULL;
    bool do_calibration1 = init_adc_calibration(ADC_UNIT_1, EXAMPLE_ADC_ATTEN, &adc1_cali_handle);
    uint8_t readTemperature;

    while (1) {
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, channel_number, &adc_raw[0][0]));
        readTemperature = temperature_conversion_ntc_5k(adc_raw[0][0]);
        printf("ADC%d Channel[%d] - Temperature: %d, Raw Data: %d, ", ADC_UNIT_1 + 1, channel_number, readTemperature, adc_raw[0][0]);
        if (do_calibration1) {
            ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc1_cali_handle, adc_raw[0][0], &voltage[0][0]));
            printf(", Cali Voltage: %d mV\n", voltage[0][0]);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    //Tear Down
    ESP_ERROR_CHECK(adc_oneshot_del_unit(adc1_handle));
    if (do_calibration1) {
        uninit_adc_calibration(adc1_cali_handle);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(ntc_5k_obj, ntc_5k);

// Copy the uncommented line into your map table
STATIC const mp_rom_map_elem_t ntc_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_adc_driver) },
    { MP_ROM_QSTR(MP_QSTR_ntc_5k), MP_ROM_PTR(&ntc_5k_obj) },
};
STATIC MP_DEFINE_CONST_DICT(ntc_globals, ntc_globals_table);

// Define module object
const mp_obj_module_t ntc_module = {
    .base       = {&mp_type_module},
    .globals    = (mp_obj_dict_t *)&ntc_globals,
};

// Register the module to make available in Python
MP_REGISTER_MODULE(MP_QSTR_ntc_module, ntc_module);
```

```c
# Create an INTERFACE library for our C module.
add_library(NTC_MODULE INTERFACE)

# Add our source files to the lib from current directory
target_sources(NTC_MODULE INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/main.c
)

# Add the current directory as an include directory.
target_include_directories(NTC_MODULE INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Link our INTERFACE library to the usermod target.
target_link_libraries(usermod INTERFACE NTC_MODULE)
```

```c
NTC_MODULE_MOD_DIR := $(USERMOD_DIR)

# Add all C files to SRC_USERMOD.
SRC_USERMOD += $(NTC_MODULE_MOD_DIR)/main.c

# We can add our module folder to include paths if needed
# This is not actually needed in this example.
CFLAGS_USERMOD += -I$(NTC_MODULE_MOD_DIR)
```

Finally, try to call the ADC module from the Micropython to read analog data from the NTC and convert the data to the temperature of Celsius.

![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%2025.png)

# Create a Class in the External C Module

Python is object-oriented programming (OOP), which **uses objects and classes in programming. Creating a class is like creating a new object type and its methods and instances**. Micropython also adopts the OOP and treats a class as a new object. This example will teach us **to create a micropython class inside the external C modules**. For simplicity, we will create **a class of simple calculations,** including the **operation of addition, subtraction, multiplication, and division**.

![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%2026.png)

Then, we create the main.h **contains the structs used to construct a class and the class methods**. 

```c
// Include MicroPython API.
#include "py/runtime.h"
#include "py/obj.h"

// Math Class
typedef struct _mp_mathclass_obj_t {
    mp_obj_base_t base;
    int32_t currentValue;
} mp_mathclass_obj_t;
extern const mp_obj_type_t mp_mathclass_type_obj;

// Class methods
extern mp_obj_t mathclass_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);
extern void mathclass_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
extern mp_obj_t mathclass_add(mp_obj_t self_in, mp_obj_t input_value);
extern mp_obj_t mathclass_subtract(mp_obj_t self_in, mp_obj_t input_value);
extern mp_obj_t mathclass_multiply(mp_obj_t self_in, mp_obj_t input_value);
extern mp_obj_t mathclass_divide(mp_obj_t self_in, mp_obj_t input_value);
```

The **class methods code placed in simple_math.c**. Notice that **we have a function, namely “mathclass_make_new”** in line 5, which is **used for initializing the class instances**; it is similar to __init__. The **“mathclass_print” method is also similar to __repr__**, used **for printing the initialized class**. The rest of the methods have functions just like their names and accept the input parameter of a list; the value of the list will then be **used to modify the currentValue** instance.

```c
#include "main.h"

/*************************************Class methods*****************************************/
// __init__
mp_obj_t mathclass_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 256, true);
    mp_mathclass_obj_t *self = m_new_obj(mp_mathclass_obj_t);
    self->base.type     = &mp_mathclass_type_obj;
    self->currentValue  = mp_obj_get_int(args[0]);
    return MP_OBJ_FROM_PTR(self);
}

// __repr__
void mathclass_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_mathclass_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_print_str(print, "mathclass(");
    mp_obj_print_helper(print, mp_obj_new_int(self->currentValue), PRINT_REPR);
    mp_print_str(print, ")");
}

// Sum method
mp_obj_t mathclass_add(mp_obj_t self_in, mp_obj_t input_value) {
    mp_mathclass_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t *arg = NULL;
    size_t dataLength = 0;
    mp_obj_get_array(input_value, &dataLength, &arg);

    for(uint8_t i=0; i<dataLength; i++){
        self->currentValue += mp_obj_get_int(arg[i]);
    }
    return mp_obj_new_int(self->currentValue); 
}

// Subtract method
mp_obj_t mathclass_subtract(mp_obj_t self_in, mp_obj_t input_value) {
    mp_mathclass_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t *arg = NULL;
    size_t dataLength = 0;
    mp_obj_get_array(input_value, &dataLength, &arg);

    for(uint8_t i=0; i<dataLength; i++){
        self->currentValue -= mp_obj_get_int(arg[i]);
    }
    return mp_obj_new_int(self->currentValue);
}

// Multiplication method
mp_obj_t mathclass_multiply(mp_obj_t self_in, mp_obj_t input_value){
    mp_mathclass_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t *arg = NULL;
    size_t dataLength = 0;
    mp_obj_get_array(input_value, &dataLength, &arg);

    for(uint8_t i=0; i<dataLength; i++){
        self->currentValue *= mp_obj_get_int(arg[i]);
    }
    return mp_obj_new_int(self->currentValue);
}

// Division method
mp_obj_t mathclass_divide(mp_obj_t self_in, mp_obj_t input_value){
    mp_mathclass_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t *arg = NULL;
    size_t dataLength = 0;
    mp_obj_get_array(input_value, &dataLength, &arg);

    for(uint8_t i=0; i<dataLength; i++){
        self->currentValue /= mp_obj_get_int(arg[i]);
    }
    return mp_obj_new_int(self->currentValue);
}
/**********************************EOF Class methods*****************************************/
```

After the class methods are created, **we need to register each class method to the class by referencing QSTR to each method**. Then, the class needs to be registered as the object using **MP_DEFINE_CONST_OBJ_TYPE**. Lastly, the class object is registered to the module. 

```c
#include "main.h"

// Simple Math Class methods
MP_DEFINE_CONST_FUN_OBJ_2(mathclass_add_obj, mathclass_add);
MP_DEFINE_CONST_FUN_OBJ_2(mathclass_subtract_obj, mathclass_subtract);
MP_DEFINE_CONST_FUN_OBJ_2(mathclass_multiply_obj, mathclass_multiply);
MP_DEFINE_CONST_FUN_OBJ_2(mathclass_divide_obj, mathclass_divide);

// Define class methods
STATIC const mp_rom_map_elem_t mathclass_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_add), MP_ROM_PTR(&mathclass_add_obj) },
    { MP_ROM_QSTR(MP_QSTR_subtract), MP_ROM_PTR(&mathclass_subtract_obj) },
    { MP_ROM_QSTR(MP_QSTR_multiply), MP_ROM_PTR(&mathclass_multiply_obj) },
    { MP_ROM_QSTR(MP_QSTR_divide), MP_ROM_PTR(&mathclass_divide_obj) },
};
STATIC MP_DEFINE_CONST_DICT(mathclass_locals_dict, mathclass_locals_dict_table);

// Define Class as the object
MP_DEFINE_CONST_OBJ_TYPE(
    mp_mathclass_type_obj,                  // Name of the object
    MP_QSTR_mathclass,                      // Name of the class
    MP_TYPE_FLAG_NONE,                  
    make_new, mathclass_make_new,           // __init__
    print, mathclass_print,                 // __print__
    locals_dict, &mathclass_locals_dict     // methods
);

// Map the class reference
STATIC const mp_map_elem_t mp_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_math_module) },
    { MP_ROM_QSTR(MP_QSTR_mathclass), (mp_obj_t)&mp_mathclass_type_obj },
};
STATIC MP_DEFINE_CONST_DICT (
    mp_module_mp_globals,
    mp_globals_table
);
const mp_obj_module_t mp_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_mp_globals,
};
// Register the module to make it available in Python.
MP_REGISTER_MODULE(MP_QSTR_math_module, mp_user_cmodule);
```

You can see **the same class but written in Python to give a clear picture.**

![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%2027.png)

![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%2028.png)

```c
# Create an INTERFACE library for our C module.
add_library(math_module INTERFACE)

# Add our source files to the lib from current directory
target_sources(math_module INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/main.c
    ${CMAKE_CURRENT_LIST_DIR}/simple_math.c
)

# Add the current directory as an include directory.
target_include_directories(math_module INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Link our INTERFACE library to the usermod target.
target_link_libraries(usermod INTERFACE math_module)
```

```c
MATH_MODULE := $(USERMOD_DIR)

# Add all C files to SRC_USERMOD.
SRC_USERMOD += $(MATH_MODULE)/main.c
SRC_USERMOD += $(MATH_MODULE)/simple_math.c

# We can add our module folder to include paths if needed
# This is not actually needed in this example.
CFLAGS_USERMOD += -I$(MATH_MODULE)
```

Lastly, we need to test the class by simply instantiating the class and then calling each method.

![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%2029.png)

# Adding LVGL to Existing Micropython Firmware

> Disclaimer: Not yet functional, however, LVGL successfully recognized by the Micropython
To-Do: Make it fully functional to draw from frame buffer.
> 

**LVGL (Light and Versatile Graphics Library)** is one of the most powerful open-source **embedded graphics libraries** to create UIs. It supports various targets from Linux, Windows, and microcontrollers such as ESP32, STM32, NXP, and many more. LVGL is written in C language and can be **ported to Micropython framework**. Hence, we will go through the steps for porting LVGL into Micropython.

Initially, you must clone the LVGL repository and place it under esp-idf/components. Navigate to /esp-idf/components/ and clone the repository on branch v8.0:

```bash
cd ~/esp/esp-idf/components/
git clone https://github.com/lvgl/lvgl.git --branch release/v8.0
```

Copy the lv_conf_template.h in the same directory and rename it to lv_conf.h. In lv_conf.h you need to enable the LVGL API. It should look like this:

```bash
Esp-idf
---components
------…..
------lvgl
------lv_conf.h
---examples
---tools
---…..
```

Upon successful cloning the LVGL, create a folder containing the **Python converter code with pycparser dependency**, a CMake file, and a header file. You can place the folder anywhere, but I place it under micropython/lib in this example. If you want to know the details, I encourage you to observe the repository I also used as a reference: [https://github.com/lvgl/lv_binding_micropython.git](https://github.com/lvgl/lv_binding_micropython.git).

![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%2030.png)

Copy necessary files from the repo, including **mkrules.cmake**, **gen folder containing the LVGL converter from C to micropython**, **pycparser dependency**, and create a header file containing codes to include Micropython APIs.

```c
/*********************
 *      INCLUDES
 *********************/
#include <py/mpconfig.h>
#include <py/misc.h>
#include <py/gc.h>
#include <py/mpstate.h>
#include "lvgl/lvgl.h"

typedef struct lvgl_root_pointers_t {
  LV_ROOTS
} lvgl_root_pointers_t;
```

Open gen_mpy.py and edit the line code. The left pictures are the original code, and the right pictures are the updated code. We need to change these lines because we use LVGL version 8.xx, a stable version.

![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%2031.png)

![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%2032.png)

![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%2033.png)

![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%2034.png)

After that, open mkrules.cmake and change the directory of the LVGL and lv_conf.h. Also, remove lines containing **if(ESP_PLATFORM)** since we won’t need**.**

```c
find_package(Python3 REQUIRED COMPONENTS Interpreter)
find_program(AWK awk mawk gawk)

set(LV_BINDINGS_DIR ${CMAKE_CURRENT_LIST_DIR})

# Common function for creating LV bindings

function(lv_bindings)
    set(_options)
    set(_one_value_args OUTPUT)
    set(_multi_value_args INPUT DEPENDS COMPILE_OPTIONS PP_OPTIONS GEN_OPTIONS FILTER)
    cmake_parse_arguments(
        PARSE_ARGV 0 LV
        "${_options}"
        "${_one_value_args}"
        "${_multi_value_args}"
    )

    set(LV_PP ${LV_OUTPUT}.pp)
    set(LV_MPY_METADATA ${LV_OUTPUT}.json)

    add_custom_command(
        OUTPUT 
            ${LV_PP}
        COMMAND
        ${CMAKE_C_COMPILER} -E -DPYCPARSER ${LV_COMPILE_OPTIONS} ${LV_PP_OPTIONS} "${LV_CFLAGS}" -I ${LV_BINDINGS_DIR}/pycparser/utils/fake_libc_include ${MICROPY_CPP_FLAGS} ${LV_INPUT} > ${LV_PP}
        DEPENDS
            ${LV_INPUT}
            ${LV_DEPENDS}
            ${LV_BINDINGS_DIR}/pycparser/utils/fake_libc_include
        IMPLICIT_DEPENDS
            C ${LV_INPUT}
        VERBATIM
        COMMAND_EXPAND_LISTS
    )

    if(ESP_PLATFORM)
        target_compile_options(${COMPONENT_LIB} PRIVATE ${LV_COMPILE_OPTIONS})
    else()
        target_compile_options(usermod_lv_bindings INTERFACE ${LV_COMPILE_OPTIONS})
    endif()

    if (DEFINED LV_FILTER)

        set(LV_PP_FILTERED ${LV_PP}.filtered)
        set(LV_AWK_CONDITION)
        foreach(_f ${LV_FILTER})
            string(APPEND LV_AWK_CONDITION "\$3!~\"${_f}\" && ")
        endforeach()
        string(APPEND LV_AWK_COMMAND "\$1==\"#\"{p=(${LV_AWK_CONDITION} 1)} p{print}")

        # message("AWK COMMAND: ${LV_AWK_COMMAND}")

        add_custom_command(
            OUTPUT
                ${LV_PP_FILTERED}
            COMMAND
                ${AWK} ${LV_AWK_COMMAND} ${LV_PP} > ${LV_PP_FILTERED}
            DEPENDS
                ${LV_PP}
            VERBATIM
            COMMAND_EXPAND_LISTS
        )
    else()
        set(LV_PP_FILTERED ${LV_PP})
    endif()

    add_custom_command(
        OUTPUT
            ${LV_OUTPUT}
        COMMAND
            ${Python3_EXECUTABLE} ${LV_BINDINGS_DIR}/gen/gen_mpy.py ${LV_GEN_OPTIONS} -MD ${LV_MPY_METADATA} -E ${LV_PP_FILTERED} ${LV_INPUT} > ${LV_OUTPUT} || (rm -f ${LV_OUTPUT} && /bin/false)
        DEPENDS
            ${LV_BINDINGS_DIR}/gen/gen_mpy.py
            ${LV_PP_FILTERED}
        COMMAND_EXPAND_LISTS
    )

endfunction()

# Definitions for specific bindings

set(LVGL_DIR ${LV_BINDINGS_DIR}/../../../esp/esp-idf/components/lvgl)

set(LV_MP ${CMAKE_BINARY_DIR}/lv_mp.c)

# Function for creating all specific bindings

function(all_lv_bindings)

    # LVGL bindings

    file(GLOB_RECURSE LVGL_HEADERS ${LVGL_DIR}/src/*.h ${LV_BINDINGS_DIR}/../../../esp/esp-idf/component/lv_conf.h)
    lv_bindings(
        OUTPUT
            ${LV_MP}
        INPUT
            ${LVGL_DIR}/lvgl.h
        DEPENDS
            ${LVGL_HEADERS}
        GEN_OPTIONS
            -M lvgl -MP lv
    )

endfunction()

# Add includes to CMake component

set(LV_INCLUDE
    ${LV_BINDINGS_DIR}
)

# Add sources to CMake component

set(LV_SRC
    ${LV_MP}
)
```

Next, open esp32_common.cmake under /ports/esp32/. In this file, we need to add lines of code for calling the **lv_mpy** module and its source codes. Also, add compile option ‘LV_KCONFIG_IGNORE’ to ignore the KCONFIG of ESP-IDF configuration since we could easily configure the LVGL parameter through **lv_conf.h**. Lastly, call the **all_lv_bindings()** to include the LVGL APIs and its configurations to our firmware

![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%2035.png)

![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%2036.png)

![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%2037.png)

![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%2038.png)

Finally, compile the firmware as usual and try to import LVGL:

![Untitled](Micropython%207e488ec988d042ac9445af946dabf001/Untitled%2039.png)

# Tips

You might be curious whether any tools could help us to generate the code for binding C code with Micropython, and the answer is YES! Below are tools for generating the boilerplate using the Online Tool Generator [3]. However, I suggest you not to fully believe the generated code because sometimes it doesn’t generate the correct syntax.

For a deeper understanding, I encourage you to read these books and the Micropython docs:

1. Micropython for the Internet of Things a Beginners Guide to Programming with Python on Microcontrollers [4]
2. Programming with MicroPython Embedded Programming With Microcontrollers & Python [5]

# References

- [https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html)
- [https://docs.micropython.org/en/latest/](https://docs.micropython.org/en/latest/)
- [https://mpy-c-gen.oliverrobson.tech/](https://mpy-c-gen.oliverrobson.tech/)
- https://github.com/lvgl/lv_binding_micropython
- C. Bell, *MicroPython for the Internet of Things*. Apress, 2017. doi: 10.1007/978-1-4842-3123-4
- N. H. Tollervey, “Programming with MicroPython EMBEDDED PROGRAMMING WITH MICROCONTROLLERS & PYTHON.”