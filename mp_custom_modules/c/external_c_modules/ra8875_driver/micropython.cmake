# Create an INTERFACE library for our C module.
add_library(RA8875 INTERFACE)

# Add our source files to the lib from current directory
target_sources(RA8875 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/ra8875_driver.c
    ${CMAKE_CURRENT_LIST_DIR}/displaySPI.c
)

# Add the current directory as an include directory.
target_include_directories(RA8875 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Link our INTERFACE library to the usermod target.
target_link_libraries(usermod INTERFACE RA8875)
