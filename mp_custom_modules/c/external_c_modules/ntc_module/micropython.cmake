# Create an INTERFACE library for our C module.
add_library(ntc_module INTERFACE)

# Add our source files to the lib from current directory
if(MICROPY_BOARD STREQUAL "ESP32_GENERIC_S3")
    target_sources(ntc_module INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/adc_ESP32S3.c
        ${CMAKE_CURRENT_LIST_DIR}/main.c
    )
endif()

if(MICROPY_BOARD STREQUAL "ESP32_LILYGO")
    target_sources(ntc_module INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/adc_ESP32.c
        ${CMAKE_CURRENT_LIST_DIR}/main.c
    )
endif()

# Add the current directory as an include directory.
target_include_directories(ntc_module INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Link our INTERFACE library to the usermod target.
target_link_libraries(usermod INTERFACE ntc_module)
