# Create an INTERFACE library for our C module.
add_library(spi_module INTERFACE)

# Add our source files to the lib from current directory
target_sources(spi_module INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/main.c
)

# Add the current directory as an include directory.
target_include_directories(spi_module INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Link our INTERFACE library to the usermod target.
target_link_libraries(usermod INTERFACE spi_module)
