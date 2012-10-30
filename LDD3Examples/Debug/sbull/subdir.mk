################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../sbull/sbull.c 

OBJS += \
./sbull/sbull.o 

C_DEPS += \
./sbull/sbull.d 


# Each subdirectory must supply rules for building sources it contributes
sbull/%.o: ../sbull/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr/src/linux-headers-3.0.0-13/include/ -I/usr/src/linux-headers-3.0.0-13/arch/x86/include/ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


