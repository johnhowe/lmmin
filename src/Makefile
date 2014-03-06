RTOS_SOURCE_DIR=../kernel
DEMO_COMMON_DIR=../Minimal
DEMO_INCLUDE_DIR=../include
LUMINARY_DRIVER_DIR=../driverlib

PROJECT=nadir

CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy
SIZE=arm-none-eabi-size

LDSCRIPT=standalone.ld

# should use --gc-sections but the debugger does not seem to be able to cope with the option.
LFLAGS=	-nostartfiles -Xlinker -onadir.axf -Xlinker -M -Xlinker -Map=$(PROJECT).map -Xlinker --no-gc-sections

DEBUG=	-g
OPTIM=	-0

INC=	-I$(RTOS_SOURCE_DIR)/include \
		-I$(RTOS_SOURCE_DIR)/portable/GCC/ARM_CM3 \
		-I$(DEMO_INCLUDE_DIR) \
		-I$(LUMINARY_DRIVER_DIR) \
		-I$(LUMINARY_DRIVER_DIR)/inc \
		-I.. \
		-I. 

CFLAGS= $(DEBUG) $(INC) -std=gnu99 -D GCC_ARMCM3 -mthumb -mcpu=cortex-m3 -D inline= -o $(OPTIM) -T$(LDSCRIPT) -D PACK_STRUCT_END=__attribute\(\(packed\)\) -D ALIGN_STRUCT_END=__attribute\(\(aligned\(4\)\)\) -D sprintf=usprintf -D snprintf=usnprintf -D printf=uipprintf -ffunction-sections -fdata-sections -Wall

SOURCE=	main.c \
		utils/rit128x96x4.c \
		utils/ustdlib.c \
		timertest.c \
		$(RTOS_SOURCE_DIR)/list.c \
		$(RTOS_SOURCE_DIR)/queue.c \
		$(RTOS_SOURCE_DIR)/tasks.c \
		$(RTOS_SOURCE_DIR)/portable/GCC/ARM_CM3/port.c \
		$(RTOS_SOURCE_DIR)/portable/MemMang/heap_2.c

LIBS=	$(LUMINARY_DRIVER_DIR)/gcc/libdriver.a

OBJS=	$(SOURCE:.c=.o)


all: $(PROJECT).bin
	 
$(PROJECT).bin : $(PROJECT).axf
	@echo "  CP    ${@}"; \
	$(OBJCOPY) $(PROJECT).axf -O binary $(PROJECT).bin
	@cp $(PROJECT).bin $(PROJECT).axf ../

$(PROJECT).axf : $(OBJS) startup.o Makefile
	@echo "  LD    ${@} ${LIBS}"; \
	$(CC) $(CFLAGS) $(OBJS) startup.o $(LIBS) $(LFLAGS)
#	@$(SIZE) $@

$(OBJS) : %.o : %.c Makefile FreeRTOSConfig.h
	@echo "  CC    ${<}"; \
	$(CC) -c $(CFLAGS) $< -o $@

startup.o : startup.c Makefile
	@echo "  CC    ${<}"; \
	$(CC) -c $(CFLAGS) -O1 startup.c -o startup.o
		
clean :
	rm -f $(OBJS) startup.o $(PROJECT).bin $(PROJECT).axf $(PROJECT).map
	
install : all
	openocd -f ../openocd/lm3s1968-flash.cfg



