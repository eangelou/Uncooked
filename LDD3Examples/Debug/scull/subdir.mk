################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../scull/access.c \
../scull/main.c \
../scull/pipe.c 

OBJS += \
./scull/access.o \
./scull/main.o \
./scull/pipe.o 

C_DEPS += \
./scull/access.d \
./scull/main.d \
./scull/pipe.d 


# Each subdirectory must supply rules for building sources it contributes
scull/%.o: ../scull/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr/src/linux-headers-3.0.0-13/include/ -I/usr/src/linux-headers-3.0.0-13/arch/x86/include/ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


