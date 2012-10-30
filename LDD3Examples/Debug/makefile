################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include usb/subdir.mk
-include tty/subdir.mk
-include snull/subdir.mk
-include skull/subdir.mk
-include simple/subdir.mk
-include shortprint/subdir.mk
-include short/subdir.mk
-include scullv/subdir.mk
-include scullp/subdir.mk
-include sculld/subdir.mk
-include scullc/subdir.mk
-include scull/subdir.mk
-include sbull/subdir.mk
-include pci/subdir.mk
-include misc-progs/subdir.mk
-include misc-modules/subdir.mk
-include lddbus/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: LDD3Examples

# Tool invocations
LDD3Examples: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++  -o"LDD3Examples" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C++_DEPS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) LDD3Examples
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
