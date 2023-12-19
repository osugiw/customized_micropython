# Create an INTERFACE library for our C module.
add_library(i2c_ssd1306 INTERFACE)

# Add our source files to the lib from current directory
target_sources(i2c_ssd1306 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/main.c
)

# Add the current directory as an include directory.
target_include_directories(i2c_ssd1306 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Link our INTERFACE library to the usermod target.
target_link_libraries(usermod INTERFACE i2c_ssd1306)
