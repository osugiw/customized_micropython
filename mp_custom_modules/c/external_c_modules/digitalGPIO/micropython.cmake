# Create an INTERFACE library for our C module.
add_library(digitalGPIO INTERFACE)

# Add our source files to the lib
target_sources(digitalGPIO INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/main.c
)

# Add the current directory as an include directory.
target_include_directories(digitalGPIO INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Link our INTERFACE library to the usermod target.
target_link_libraries(usermod INTERFACE digitalGPIO)
