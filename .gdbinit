# Tell GDB that we are using 32-bit x86 architecture
set arch i386

# Tell GDB to load symbols from main.exe
#
# Notice here that we are using the .exe file, not the .xbe file, as
# GDB does not understand the .xbe format.
#
add-symbol-file main.exe

# Use a layout which shows source code
layout src

# Connect to the XQEMU GDB server
target remote 127.0.0.1:1234

# Stop execution at the beginning of the `main` function
b main

# Let XQEMU run until the CPU tries to execute from the address
# we have placed our breakpoint(s)
c
