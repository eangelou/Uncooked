project(cryptodev_demos)

add_executable(cryptodev_demos main.cpp)

# Make sure the compiler can find include files from our Hello library.
include_directories (/home/vagos/projects/cryptodev-linux) 

# Make sure the linker can find the Hello library once it is built.
link_directories (/home/vagos/projects/cryptodev-linux) 
