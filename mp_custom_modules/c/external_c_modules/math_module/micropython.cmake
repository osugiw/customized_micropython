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
