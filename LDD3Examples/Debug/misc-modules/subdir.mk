################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../misc-modules/complete.c \
../misc-modules/faulty.c \
../misc-modules/hello.c \
../misc-modules/hellop.c \
../misc-modules/jiq.c \
../misc-modules/jit.c \
../misc-modules/kdataalign.c \
../misc-modules/kdatasize.c \
../misc-modules/seq.c \
../misc-modules/silly.c \
../misc-modules/sleepy.c 

OBJS += \
./misc-modules/complete.o \
./misc-modules/faulty.o \
./misc-modules/hello.o \
./misc-modules/hellop.o \
./misc-modules/jiq.o \
./misc-modules/jit.o \
./misc-modules/kdataalign.o \
./misc-modules/kdatasize.o \
./misc-modules/seq.o \
./misc-modules/silly.o \
./misc-modules/sleepy.o 

C_DEPS += \
./misc-modules/complete.d \
./misc-modules/faulty.d \
./misc-modules/hello.d \
./misc-modules/hellop.d \
./misc-modules/jiq.d \
./misc-modules/jit.d \
./misc-modules/kdataalign.d \
./misc-modules/kdatasize.d \
./misc-modules/seq.d \
./misc-modules/silly.d \
./misc-modules/sleepy.d 


# Each subdirectory must supply rules for building sources it contributes
misc-modules/%.o: ../misc-modules/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr/src/linux-headers-3.0.0-13/include/ -I/usr/src/linux-headers-3.0.0-13/arch/x86/include/ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


