file(GLOB_RECURSE UI_SOURCES ${CMAKE_CURRENT_LIST_DIR}/ui/src/*.c)

# Create an INTERFACE library for our C module.
add_library(TRGB_MODULE INTERFACE)

# Add our source files to the lib from current directory
target_sources(TRGB_MODULE INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/main.c
    ${CMAKE_CURRENT_LIST_DIR}/XL9535_driver.c
    ${CMAKE_CURRENT_LIST_DIR}/ft3267.c
    ${UI_SOURCES}
)

# Add the current directory as an include directory.
target_include_directories(TRGB_MODULE INTERFACE 
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/ui/src
)

# Link our INTERFACE library to the usermod target.
target_link_libraries(usermod INTERFACE TRGB_MODULE)
target_compile_options(usermod INTERFACE 
    -Wno-unused-function
    -Wunused-variable
)