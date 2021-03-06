#
#	AVR Makefile
#
TARGET		=	gaiger

CSOURCES	=
PSOURCES	=	mainloop.cpp \
				monograph.cpp \
				lcdio.cpp \
				psound.cpp

ASOURCES	=

MCU_TARGET	=	atmega328p

# OPTIMIZE	= -Os -mcall-prologues
OPTIMIZE	= -O2 -mcall-prologues
DEFS		=	$(addprefix -I, $(INC_ROOT))
DEFS		+=	-DF_CPU=12000000UL
LIBS		=

# You should not have to change anything below here.

CC		= avr-gcc

ASFLAGS        = -Wa,-adhlns=$(<:.s=.lst),-gstabs 
ALL_ASFLAGS    = -mmcu=$(MCU_TARGET) -I. -x assembler-with-cpp $(ASFLAGS) $(DEFS)

# Override is only needed by avr-lib build system.

override CFLAGS		= -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)
override CFLAGS	   += -Wno-deprecated-declarations -D__PROG_TYPES_COMPAT__
override PFLAGS		= -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)
override PFLAGS    += -Wno-deprecated-declarations -D__PROG_TYPES_COMPAT__
override LDFLAGS	= -mmcu=$(MCU_TARGET) -Wl,-Map,$(TARGET).map

OBJECTS		=	$(CSOURCES:.c=.o) $(PSOURCES:.cpp=.o) $(ASOURCES:.s=.o)

OBJCOPY		=	avr-objcopy
OBJDUMP		=	avr-objdump

# all: $(TARGET).elf lst text eeprom
all: $(TARGET).elf lst text

$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBS)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

%.o: %.cpp
	$(CC) -c $(PFLAGS) $< -o $@

%.o: %.s
	$(CC) -c $(ALL_ASFLAGS) $< -o $@

clean:
	rm -rf *.o $(TARGET).elf *.eps *.png *.pdf *.bak 
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)

lst:  $(TARGET).lst

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

# Rules for building the .text rom images

text: hex bin

hex:  $(TARGET).hex
bin:  $(TARGET).bin
#srec: $(TARGET).srec

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@

# Rules for building the .eeprom rom images

eeprom: ehex ebin

ehex:  $(TARGET)_eeprom.hex
ebin:  $(TARGET)_eeprom.bin

%_eeprom.hex: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@ \
	|| { echo empty $@ not generated; exit 0; }

%_eeprom.bin: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O binary $< $@ \
	|| { echo empty $@ not generated; exit 0; }

# Every thing below here is used by avr-libc's build system and can be ignored
# by the casual user.

FIG2DEV                 = fig2dev
EXTRA_CLEAN_FILES       = *.hex *.bin

dox: eps png pdf

eps: $(TARGET).eps
png: $(TARGET).png
pdf: $(TARGET).pdf

%.eps: %.fig
	$(FIG2DEV) -L eps $< $@

%.pdf: %.fig
	$(FIG2DEV) -L pdf $< $@

%.png: %.fig
	$(FIG2DEV) -L png $< $@

read_fuse:
	hidspx -ph -d4 -Rf

atmega88_write_fuse_low:
	avrsp -pb3 -d40 -FL11100110

atmega88_write_fuse_high:
	avrsp -pb3 -d40 -FH11011001

ram_length:
	grep heap_start $(TARGET).map

run:
	hidspx -ph $(TARGET).hex
#	avrsp -pb3 -d40 $(TARGET).hex

depend:
	makedepend -Y -- $(CFLAGS) $(INC_A) -- $(ASOURCES) $(CSOURCES) $(PSOURCES)

tarball:
	chmod -x *.[hcs] *.[hc]pp
	chmod -x Makefile
	chmod -x KiCAD/*
	rm -f $(TARGET)_src.tgz
	tar cfvz $(TARGET)_src.tgz *.[hcs] *.[hc]pp KiCAD
	chmod 444 $(TARGET)_src.tgz

# DO NOT DELETE

mainloop.o: mainloop.hpp monograph.hpp lcdio.hpp psound.hpp
monograph.o: monograph.hpp font6x12.h
lcdio.o: lcdio.hpp
psound.o: psound.hpp
