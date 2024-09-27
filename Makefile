# Makefile for STM32 operations using OpenOCD

# OpenOCD configuration file
OPENOCD_CFG = ./openocd/openocd.cfg

# Target to flash the STM32
flash:
	openocd -f $(OPENOCD_CFG) -c "program program.bin 0x08000000 verify reset exit"

# Target to reset the STM32
reset:
	openocd -f $(OPENOCD_CFG) -c "init" -c "reset" -c "exit"

# Target to erase the STM32
erase:
	openocd -f $(OPENOCD_CFG) -c "init" -c "halt" -c "stm32f4x mass_erase 0" -c "exit"

.PHONY: flash reset erase