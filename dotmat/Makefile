#
#	AVR data logger Makefile
#
TARGET		=	dotmat

CSOURCES	=
PSOURCES	=	mainloop.cpp \
				monograph.cpp \
				dm_draw.cpp \
				psound.cpp \
				music.cpp \
				menu.cpp \
				tetris.cpp \
				ktimer.cpp \
				timer.cpp

ASOURCES	=

MCU_TARGET	=	atmega168p

# OPTIMIZE	= -Os -mcall-prologues
OPTIMIZE	= -O2 -mcall-prologues
DEFS		=	$(addprefix -I, $(INC_ROOT))
DEFS		+=	-DF_CPU=20000000UL
LIBS		=

# You should not have to change anything below here.

CC		= avr-gcc

ASFLAGS        = -Wa,-adhlns=$(<:.S=.lst),-gstabs 
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

write_fuse_low:
	hidspx -ph -d4 -FL11100110

write_fuse_high:
	hidspx -ph -d4 -FH11011100

ram_size:
	grep heap_start $(TARGET).map

run:
	hidspx -ph -d4 $(TARGET).hex
#	avrsp -pb3 $(TARGET).hex
#	avrsp -pb3 -d40 $(TARGET).hex

create_bitmap:
	bmc -text -offset  0*8,0 -size 3,5 images/matrix_font.png font3x5.h
	bmc -text -offset  1*8,0 -size 3,5 images/matrix_font.png -append font3x5.h
	bmc -text -offset  2*8,0 -size 3,5 images/matrix_font.png -append font3x5.h
	bmc -text -offset  3*8,0 -size 3,5 images/matrix_font.png -append font3x5.h
	bmc -text -offset  4*8,0 -size 3,5 images/matrix_font.png -append font3x5.h
	bmc -text -offset  5*8,0 -size 3,5 images/matrix_font.png -append font3x5.h
	bmc -text -offset  6*8,0 -size 3,5 images/matrix_font.png -append font3x5.h
	bmc -text -offset  7*8,0 -size 3,5 images/matrix_font.png -append font3x5.h
	bmc -text -offset  8*8,0 -size 3,5 images/matrix_font.png -append font3x5.h
	bmc -text -offset  9*8,0 -size 3,5 images/matrix_font.png -append font3x5.h
	bmc -text -offset 10*8,0 -size 3,5 images/matrix_font.png -append font3x5.h

	bmc -text -offset  0*8,8 -size 7,10 images/matrix_font.png font7x10.h
	bmc -text -offset  1*8,8 -size 7,10 images/matrix_font.png -append font7x10.h
	bmc -text -offset  2*8,8 -size 7,10 images/matrix_font.png -append font7x10.h
	bmc -text -offset  3*8,8 -size 7,10 images/matrix_font.png -append font7x10.h
	bmc -text -offset  4*8,8 -size 7,10 images/matrix_font.png -append font7x10.h
	bmc -text -offset  5*8,8 -size 7,10 images/matrix_font.png -append font7x10.h
	bmc -text -offset  6*8,8 -size 7,10 images/matrix_font.png -append font7x10.h
	bmc -text -offset  7*8,8 -size 7,10 images/matrix_font.png -append font7x10.h
	bmc -text -offset  8*8,8 -size 7,10 images/matrix_font.png -append font7x10.h
	bmc -text -offset  9*8,8 -size 7,10 images/matrix_font.png -append font7x10.h
	bmc -text -offset 10*8,8 -size 7,10 images/matrix_font.png -append font7x10.h
	bmc -text -offset 11*8,8 -size 7,10 images/matrix_font.png -append font7x10.h
	bmc -text -offset 12*8,8 -size 7,10 images/matrix_font.png -append font7x10.h
	bmc -text -offset 13*8,8 -size 7,10 images/matrix_font.png -append font7x10.h

	bmc -text -offset  0*16,48 -size 16,16 images/matrix_font.png icon.h
	bmc -text -offset  1*16,48 -size 16,16 images/matrix_font.png -append icon.h
	bmc -text -offset  2*16,48 -size 16,16 images/matrix_font.png -append icon.h

depend:
	makedepend -Y -- $(CFLAGS) $(INC_A) -- $(ASOURCES) $(CSOURCES) $(PSOURCES)

tarball:
	chmod -x *.[hc] *.[hc]pp
	chmod -x Makefile
	chmod -x KiCAD/*
	rm -f $(TARGET)_src.tgz
	tar cfvz $(TARGET)_src.tgz *.[hc] *.[hc]pp KiCAD
	chmod 444 $(TARGET)_src.tgz

# DO NOT DELETE

mainloop.o: mainloop.hpp ledout.hpp monograph.hpp switch.hpp psound.hpp
mainloop.o: menu.hpp task.hpp i_task.hpp dm_draw.hpp music.hpp twi.hpp
monograph.o: monograph.hpp
dm_draw.o: dm_draw.hpp monograph.hpp font3x5.h font7x10.h icon.h
psound.o: psound.hpp
music.o: music.hpp psound.hpp
menu.o: menu.hpp task.hpp switch.hpp monograph.hpp psound.hpp i_task.hpp
menu.o: dm_draw.hpp music.hpp ktimer.hpp tetris.hpp timer.hpp twi.hpp
tetris.o: tetris.hpp task.hpp switch.hpp monograph.hpp psound.hpp i_task.hpp
tetris.o: dm_draw.hpp music.hpp menu.hpp
ktimer.o: ktimer.hpp task.hpp switch.hpp monograph.hpp psound.hpp i_task.hpp
ktimer.o: dm_draw.hpp music.hpp menu.hpp
timer.o: timer.hpp task.hpp switch.hpp monograph.hpp psound.hpp i_task.hpp
timer.o: dm_draw.hpp music.hpp twi.hpp menu.hpp
