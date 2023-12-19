# This top-level micropython.cmake is responsible for listing
# the individual modules we want to include.
# Paths are absolute, and ${CMAKE_CURRENT_LIST_DIR} can be
# used to prefix subdirectories.

# # Add the C example.
# include(${CMAKE_CURRENT_LIST_DIR}/cexample/micropython.cmake)

# # Add the CPP example.
# include(${CMAKE_CURRENT_LIST_DIR}/cppexample/micropython.cmake)

# # Add the Test.
# include(${CMAKE_CURRENT_LIST_DIR}/math_test/micropython.cmake)

# Add I2C Driver
include(${CMAKE_CURRENT_LIST_DIR}/i2c_module/micropython.cmake)

# Add I2C Driver
include(${CMAKE_CURRENT_LIST_DIR}/i2c_ssd1306/micropython.cmake)

# Add ADC Driver
if(NOT MICROPY_BOARD STREQUAL "ESP32S3_TRGB")
    include(${CMAKE_CURRENT_LIST_DIR}/ntc_module/micropython.cmake)
endif()

# Simpleclass
include(${CMAKE_CURRENT_LIST_DIR}/math_module/micropython.cmake)

# SPI Module
include(${CMAKE_CURRENT_LIST_DIR}/spi_module/micropython.cmake)

# Digital GPIO Module
include(${CMAKE_CURRENT_LIST_DIR}/digitalGPIO/micropython.cmake)

# TRGB Module
include(${CMAKE_CURRENT_LIST_DIR}/trgb_module/micropython.cmake)