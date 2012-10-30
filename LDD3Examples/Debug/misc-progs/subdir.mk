################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../misc-progs/asynctest.c \
../misc-progs/dataalign.c \
../misc-progs/datasize.c \
../misc-progs/inp.c \
../misc-progs/load50.c \
../misc-progs/mapcmp.c \
../misc-progs/mapper.c \
../misc-progs/nbtest.c \
../misc-progs/netifdebug.c \
../misc-progs/outp.c \
../misc-progs/polltest.c \
../misc-progs/setconsole.c \
../misc-progs/setlevel.c 

OBJS += \
./misc-progs/asynctest.o \
./misc-progs/dataalign.o \
./misc-progs/datasize.o \
./misc-progs/inp.o \
./misc-progs/load50.o \
./misc-progs/mapcmp.o \
./misc-progs/mapper.o \
./misc-progs/nbtest.o \
./misc-progs/netifdebug.o \
./misc-progs/outp.o \
./misc-progs/polltest.o \
./misc-progs/setconsole.o \
./misc-progs/setlevel.o 

C_DEPS += \
./misc-progs/asynctest.d \
./misc-progs/dataalign.d \
./misc-progs/datasize.d \
./misc-progs/inp.d \
./misc-progs/load50.d \
./misc-progs/mapcmp.d \
./misc-progs/mapper.d \
./misc-progs/nbtest.d \
./misc-progs/netifdebug.d \
./misc-progs/outp.d \
./misc-progs/polltest.d \
./misc-progs/setconsole.d \
./misc-progs/setlevel.d 


# Each subdirectory must supply rules for building sources it contributes
misc-progs/%.o: ../misc-progs/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr/src/linux-headers-3.0.0-13/include/ -I/usr/src/linux-headers-3.0.0-13/arch/x86/include/ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


