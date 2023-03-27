cmake_minimum_required(VERSION 3.0)

project(my_project)

# Set source and include directories
set(SRC_DIR src)
set(INC_DIR include)

# Add the source and include directories to the include path
include_directories(${INC_DIR})

# Add the source files to the executable
add_executable(my_project ${SRC_DIR}/main.c ${SRC_DIR}/cpu.c)

# Specify the location of the header files
target_include_directories(my_project PUBLIC ${INC_DIR})

# Link the required libraries to the executable
target_link_libraries(my_project mingw32 SDL2main SDL2)
