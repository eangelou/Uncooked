################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../skull/skull_clean.c \
../skull/skull_init.c 

OBJS += \
./skull/skull_clean.o \
./skull/skull_init.o 

C_DEPS += \
./skull/skull_clean.d \
./skull/skull_init.d 


# Each subdirectory must supply rules for building sources it contributes
skull/%.o: ../skull/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr/src/linux-headers-3.0.0-13/include/ -I/usr/src/linux-headers-3.0.0-13/arch/x86/include/ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


