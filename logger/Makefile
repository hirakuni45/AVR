
TARGET			=	avrlogger

CSOURCES	=	mainloop.c \
				monograph.c \
				mobj.c \
				uart.c \
				lcdio.c \
				rtc4543.c \
				mcp3208.c \
				puls.c \
				time.c \
				fileio.c \
				shell.c \
				utils.c \
				menu.c \
				config.c \
				datalogger.c \
				laptimer.c \
				speed.c \
				tacho.c \
				monitor.c \
				filer.c \
				gpsio.c \
				avr_mmc/ff.c \
				avr_mmc/mmc.c \
				avr_mmc/cc932_avr.c \
				psound.c \
				eeprom_man.c \
				title.c

ASOURCES	=	avr_mmc/xitoa.s

INC_ROOT	=	avr_mmc

MCU_TARGET	=	atmega1284p

# OPTIMIZE       = -Os -mcall-prologues
OPTIMIZE       = -O2 -mcall-prologues
DEFS		=	$(addprefix -I, $(INC_ROOT))
DEFS		+=	-DF_CPU=19660800UL
LIBS		=

# You should not have to change anything below here.

CC             = avr-gcc

ASFLAGS        = -Wa,-adhlns=$(<:.s=.lst),-gstabs 
ALL_ASFLAGS    = -mmcu=$(MCU_TARGET) -I. -x assembler-with-cpp $(ASFLAGS) $(DEFS)

# Override is only needed by avr-lib build system.

# override CFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)
override CFLAGS        = -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)
override LDFLAGS       = -mmcu=$(MCU_TARGET) -Wl,-Map,$(TARGET).map

OBJECTS		=	$(CSOURCES:.c=.o) $(ASOURCES:.s=.o)

OBJCOPY		=	avr-objcopy
OBJDUMP		=	avr-objdump

# all: $(TARGET).elf lst text eeprom
all: $(TARGET).elf lst text

$(TARGET).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.s
	$(CC) -c $(ALL_ASFLAGS) $< -o $@

clean:
	rm -rf *.o $(TARGET).elf *.eps *.bak 
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

run:
	avrsp -pb8 -d0 $(TARGET).hex
#	avrsp -pb8 -d5 $(TARGET).hex

read_fuse:
	avrsp -pb8 -d40 -Rf

prog_fuse_lo:
	avrsp -pb8 -d40 -FL11100111

prog_fuse_hi:
	avrsp -pb8 -d40 -FH1010001

depend:
	makedepend -Y -- $(CFLAGS) $(INC_A) -- $(CSOURCES) $(ASOURCES)

tarball:
	chmod -x *.[hcs] Makefile readme.txt avr_mmc/*.[hcs] bitmap/*.png bitmap/*.bin
	rm -f $(TARGET)_$(shell date +%Y%m%d%H)_src.tgz
	tar cfvz $(TARGET)_$(shell date +%Y%m%d%H)_src.tgz *.[hc] avr_mmc/*.[hcs] bitmap/*.png bitmap/*.bin Makefile readme.txt
	chmod 444 $(TARGET)_$(shell date +%Y%m%d%H)_src.tgz

title:
	bmc -png ./bitmap/title.png ./bitmap/title.bin
	ccsrc -header 2 bitmap/title.bin > title_bmp.h

fontsys:
	bmc -png -no-header -clip-x  0*16 6 -clip-y 0*16 12 ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  1*16 6 -clip-y 0*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  2*16 6 -clip-y 0*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  3*16 6 -clip-y 0*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  4*16 6 -clip-y 0*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  5*16 6 -clip-y 0*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  6*16 6 -clip-y 0*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  7*16 6 -clip-y 0*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  8*16 6 -clip-y 0*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  9*16 6 -clip-y 0*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 10*16 6 -clip-y 0*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 11*16 6 -clip-y 0*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 12*16 6 -clip-y 0*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 13*16 6 -clip-y 0*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 14*16 6 -clip-y 0*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 15*16 6 -clip-y 0*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  0*16 6 -clip-y 1*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  1*16 6 -clip-y 1*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  2*16 6 -clip-y 1*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  3*16 6 -clip-y 1*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  4*16 6 -clip-y 1*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  5*16 6 -clip-y 1*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  6*16 6 -clip-y 1*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  7*16 6 -clip-y 1*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  8*16 6 -clip-y 1*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  9*16 6 -clip-y 1*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 10*16 6 -clip-y 1*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 11*16 6 -clip-y 1*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 12*16 6 -clip-y 1*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 13*16 6 -clip-y 1*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 14*16 6 -clip-y 1*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 15*16 6 -clip-y 1*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  0*16 6 -clip-y 2*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  1*16 6 -clip-y 2*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  2*16 6 -clip-y 2*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  3*16 6 -clip-y 2*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  4*16 6 -clip-y 2*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  5*16 6 -clip-y 2*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  6*16 6 -clip-y 2*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  7*16 6 -clip-y 2*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  8*16 6 -clip-y 2*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  9*16 6 -clip-y 2*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 10*16 6 -clip-y 2*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 11*16 6 -clip-y 2*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 12*16 6 -clip-y 2*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 13*16 6 -clip-y 2*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 14*16 6 -clip-y 2*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 15*16 6 -clip-y 2*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  0*16 6 -clip-y 3*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  1*16 6 -clip-y 3*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  2*16 6 -clip-y 3*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  3*16 6 -clip-y 3*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  4*16 6 -clip-y 3*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  5*16 6 -clip-y 3*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  6*16 6 -clip-y 3*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  7*16 6 -clip-y 3*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  8*16 6 -clip-y 3*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  9*16 6 -clip-y 3*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 10*16 6 -clip-y 3*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 11*16 6 -clip-y 3*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 12*16 6 -clip-y 3*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 13*16 6 -clip-y 3*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 14*16 6 -clip-y 3*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 15*16 6 -clip-y 3*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  0*16 6 -clip-y 4*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  1*16 6 -clip-y 4*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  2*16 6 -clip-y 4*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  3*16 6 -clip-y 4*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  4*16 6 -clip-y 4*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  5*16 6 -clip-y 4*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  6*16 6 -clip-y 4*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  7*16 6 -clip-y 4*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  8*16 6 -clip-y 4*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  9*16 6 -clip-y 4*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 10*16 6 -clip-y 4*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 11*16 6 -clip-y 4*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 12*16 6 -clip-y 4*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 13*16 6 -clip-y 4*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 14*16 6 -clip-y 4*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 15*16 6 -clip-y 4*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  0*16 6 -clip-y 5*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  1*16 6 -clip-y 5*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  2*16 6 -clip-y 5*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  3*16 6 -clip-y 5*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  4*16 6 -clip-y 5*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  5*16 6 -clip-y 5*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  6*16 6 -clip-y 5*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  7*16 6 -clip-y 5*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  8*16 6 -clip-y 5*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  9*16 6 -clip-y 5*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 10*16 6 -clip-y 5*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 11*16 6 -clip-y 5*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 12*16 6 -clip-y 5*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 13*16 6 -clip-y 5*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 14*16 6 -clip-y 5*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 15*16 6 -clip-y 5*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  0*16 6 -clip-y 6*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  1*16 6 -clip-y 6*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  2*16 6 -clip-y 6*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  3*16 6 -clip-y 6*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  4*16 6 -clip-y 6*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  5*16 6 -clip-y 6*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  6*16 6 -clip-y 6*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  7*16 6 -clip-y 6*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  8*16 6 -clip-y 6*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  9*16 6 -clip-y 6*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 10*16 6 -clip-y 6*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 11*16 6 -clip-y 6*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 12*16 6 -clip-y 6*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 13*16 6 -clip-y 6*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 14*16 6 -clip-y 6*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 15*16 6 -clip-y 6*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  0*16 6 -clip-y 7*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  1*16 6 -clip-y 7*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  2*16 6 -clip-y 7*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  3*16 6 -clip-y 7*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  4*16 6 -clip-y 7*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  5*16 6 -clip-y 7*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  6*16 6 -clip-y 7*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  7*16 6 -clip-y 7*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  8*16 6 -clip-y 7*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x  9*16 6 -clip-y 7*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 10*16 6 -clip-y 7*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 11*16 6 -clip-y 7*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 12*16 6 -clip-y 7*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 13*16 6 -clip-y 7*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 14*16 6 -clip-y 7*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	bmc -png -no-header -clip-x 15*16 6 -clip-y 7*16 12 -append ./bitmap/ascii6x12.png ./bitmap/system.bin
	ccsrc -length 9 bitmap/system.bin > font6x12.h

font12:
	bmc -png -clip-x  0*16 8 -clip-y 0*16 12 ./bitmap/font12.png ./bitmap/nmb12_0.bin
	bmc -png -clip-x  1*16 8 -clip-y 0*16 12 ./bitmap/font12.png ./bitmap/nmb12_1.bin
	bmc -png -clip-x  2*16 8 -clip-y 0*16 12 ./bitmap/font12.png ./bitmap/nmb12_2.bin
	bmc -png -clip-x  3*16 8 -clip-y 0*16 12 ./bitmap/font12.png ./bitmap/nmb12_3.bin
	bmc -png -clip-x  4*16 8 -clip-y 0*16 12 ./bitmap/font12.png ./bitmap/nmb12_4.bin
	bmc -png -clip-x  5*16 8 -clip-y 0*16 12 ./bitmap/font12.png ./bitmap/nmb12_5.bin
	bmc -png -clip-x  6*16 8 -clip-y 0*16 12 ./bitmap/font12.png ./bitmap/nmb12_6.bin
	bmc -png -clip-x  7*16 8 -clip-y 0*16 12 ./bitmap/font12.png ./bitmap/nmb12_7.bin
	bmc -png -clip-x  8*16 8 -clip-y 0*16 12 ./bitmap/font12.png ./bitmap/nmb12_8.bin
	bmc -png -clip-x  9*16 8 -clip-y 0*16 12 ./bitmap/font12.png ./bitmap/nmb12_9.bin
	bmc -png -clip-x 10*16 2 -clip-y 0*16 12 ./bitmap/font12.png ./bitmap/nmb12_co.bin
	bmc -png -clip-x 11*16 2 -clip-y 0*16 12 ./bitmap/font12.png ./bitmap/nmb12_po.bin
	bmc -png -clip-x 12*16 7 -clip-y 0*16 12 ./bitmap/font12.png ./bitmap/nmb12_sl.bin
	bmc -png -clip-x 13*16 8 -clip-y 0*16 12 ./bitmap/font12.png ./bitmap/nmb12_x.bin
	bmc -png -clip-x 14*16 8 -clip-y 0*16 12 ./bitmap/font12.png ./bitmap/nmb12_a.bin
	bmc -png -clip-x 15*16 8 -clip-y 0*16 12 ./bitmap/font12.png ./bitmap/nmb12_b.bin
	bmc -png -clip-x  0*16 8 -clip-y 1*16 12 ./bitmap/font12.png ./bitmap/nmb12_v.bin
	bmc -png -clip-x  1*16 8 -clip-y 1*16 12 ./bitmap/font12.png ./bitmap/nmb12_c.bin
	bmc -png -clip-x  2*16 5 -clip-y 1*16 5  ./bitmap/font12.png ./bitmap/nmb12_do.bin
	ccsrc -header 2 bitmap/nmb12_0.bin > nmb12_0.h
	ccsrc -header 2 bitmap/nmb12_1.bin > nmb12_1.h
	ccsrc -header 2 bitmap/nmb12_2.bin > nmb12_2.h
	ccsrc -header 2 bitmap/nmb12_3.bin > nmb12_3.h
	ccsrc -header 2 bitmap/nmb12_4.bin > nmb12_4.h
	ccsrc -header 2 bitmap/nmb12_5.bin > nmb12_5.h
	ccsrc -header 2 bitmap/nmb12_6.bin > nmb12_6.h
	ccsrc -header 2 bitmap/nmb12_7.bin > nmb12_7.h
	ccsrc -header 2 bitmap/nmb12_8.bin > nmb12_8.h
	ccsrc -header 2 bitmap/nmb12_9.bin > nmb12_9.h
	ccsrc -header 2 bitmap/nmb12_co.bin > nmb12_co.h
	ccsrc -header 2 bitmap/nmb12_po.bin > nmb12_po.h
	ccsrc -header 2 bitmap/nmb12_sl.bin > nmb12_sl.h
	ccsrc -header 2 bitmap/nmb12_x.bin > nmb12_x.h
	ccsrc -header 2 bitmap/nmb12_a.bin > nmb12_a.h
	ccsrc -header 2 bitmap/nmb12_b.bin > nmb12_b.h
	ccsrc -header 2 bitmap/nmb12_v.bin > nmb12_v.h
	ccsrc -header 2 bitmap/nmb12_c.bin > nmb12_c.h
	ccsrc -header 2 bitmap/nmb12_do.bin > nmb12_do.h

font32:
	bmc -png -clip-x  0*20 18 -clip-y 0*32 32 ./bitmap/font32.png ./bitmap/nmb32_0.bin
	bmc -png -clip-x  1*20 18 -clip-y 0*32 32 ./bitmap/font32.png ./bitmap/nmb32_1.bin
	bmc -png -clip-x  2*20 18 -clip-y 0*32 32 ./bitmap/font32.png ./bitmap/nmb32_2.bin
	bmc -png -clip-x  3*20 18 -clip-y 0*32 32 ./bitmap/font32.png ./bitmap/nmb32_3.bin
	bmc -png -clip-x  4*20 18 -clip-y 0*32 32 ./bitmap/font32.png ./bitmap/nmb32_4.bin
	bmc -png -clip-x  5*20 18 -clip-y 0*32 32 ./bitmap/font32.png ./bitmap/nmb32_5.bin
	bmc -png -clip-x  6*20 18 -clip-y 0*32 32 ./bitmap/font32.png ./bitmap/nmb32_6.bin
	bmc -png -clip-x  7*20 18 -clip-y 0*32 32 ./bitmap/font32.png ./bitmap/nmb32_7.bin
	bmc -png -clip-x  8*20 18 -clip-y 0*32 32 ./bitmap/font32.png ./bitmap/nmb32_8.bin
	bmc -png -clip-x  9*20 18 -clip-y 0*32 32 ./bitmap/font32.png ./bitmap/nmb32_9.bin
	bmc -png -clip-x 10*20 33 -clip-y 20   12 ./bitmap/font32.png ./bitmap/nmb32_kmh.bin
	bmc -png -clip-x 12*20  4 -clip-y 0*32 32 ./bitmap/font32.png ./bitmap/nmb32_c.bin
	bmc -png -clip-x 12*20+4 4 -clip-y 0*32 32 ./bitmap/font32.png ./bitmap/nmb32_p.bin
	bmc -png -clip-x  0*20 18  -clip-y 1*32 32 ./bitmap/font32.png ./bitmap/nmb32_x.bin
	bmc -png -clip-x  1*20 26  -clip-y 1*32+18 14 ./bitmap/font32.png ./bitmap/nmb32_rpm.bin
	ccsrc -header 2 bitmap/nmb32_0.bin > nmb32_0.h
	ccsrc -header 2 bitmap/nmb32_1.bin > nmb32_1.h
	ccsrc -header 2 bitmap/nmb32_2.bin > nmb32_2.h
	ccsrc -header 2 bitmap/nmb32_3.bin > nmb32_3.h
	ccsrc -header 2 bitmap/nmb32_4.bin > nmb32_4.h
	ccsrc -header 2 bitmap/nmb32_5.bin > nmb32_5.h
	ccsrc -header 2 bitmap/nmb32_6.bin > nmb32_6.h
	ccsrc -header 2 bitmap/nmb32_7.bin > nmb32_7.h
	ccsrc -header 2 bitmap/nmb32_8.bin > nmb32_8.h
	ccsrc -header 2 bitmap/nmb32_9.bin > nmb32_9.h
	ccsrc -header 2 bitmap/nmb32_kmh.bin > nmb32_kmh.h
	ccsrc -header 2 bitmap/nmb32_c.bin > nmb32_c.h
	ccsrc -header 2 bitmap/nmb32_p.bin > nmb32_p.h
	ccsrc -header 2 bitmap/nmb32_x.bin > nmb32_x.h
	ccsrc -header 2 bitmap/nmb32_rpm.bin > nmb32_rpm.h

icon:
	bmc -png -clip-x 3*16 21 -clip-y 1*16 10 ./bitmap/font12.png ./bitmap/batt_icon.bin
	ccsrc -header 2 bitmap/batt_icon.bin > batt_icon.h
	bmc -png -clip-x 5*16 21 -clip-y 1*16 10 ./bitmap/font12.png ./bitmap/batt_ext.bin
	ccsrc -header 2 bitmap/batt_ext.bin > batt_ext.h

# DO NOT DELETE

mainloop.o: mainloop.h time.h avr_mmc/diskio.h avr_mmc/integer.h uart.h
mainloop.o: share_port.h rtc4543.h lcdio.h mcp3208.h avr_mmc/ff.h
mainloop.o: avr_mmc/ffconf.h monograph.h mobj.h fileio.h title.h menu.h
mainloop.o: laptimer.h lowbattery.h gpsio.h avr_mmc/xitoa.h datalogger.h
mainloop.o: shell.h eeprom_man.h speed.h psound.h config.h puls.h
monograph.o: monograph.h time.h config.h avr_mmc/ff.h avr_mmc/integer.h
monograph.o: avr_mmc/ffconf.h font6x12.h
mobj.o: monograph.h time.h mobj.h nmb12_0.h nmb12_1.h nmb12_2.h nmb12_3.h
mobj.o: nmb12_4.h nmb12_5.h nmb12_6.h nmb12_7.h nmb12_8.h nmb12_9.h
mobj.o: nmb12_co.h nmb12_po.h nmb12_sl.h nmb12_x.h nmb12_a.h nmb12_b.h
mobj.o: nmb12_v.h nmb12_c.h nmb12_do.h nmb32_0.h nmb32_1.h nmb32_2.h
mobj.o: nmb32_3.h nmb32_4.h nmb32_5.h nmb32_6.h nmb32_7.h nmb32_8.h nmb32_9.h
mobj.o: nmb32_kmh.h nmb32_c.h nmb32_p.h nmb32_x.h nmb32_rpm.h title_bmp.h
mobj.o: batt_icon.h batt_ext.h
uart.o: uart.h avr_mmc/xitoa.h
lcdio.o: share_port.h lcdio.h
rtc4543.o: rtc4543.h time.h share_port.h avr_mmc/xitoa.h
mcp3208.o: mcp3208.h avr_mmc/ff.h avr_mmc/integer.h avr_mmc/ffconf.h
mcp3208.o: mainloop.h time.h avr_mmc/diskio.h share_port.h fileio.h
puls.o: puls.h config.h
time.o: time.h avr_mmc/xitoa.h
fileio.o: avr_mmc/xitoa.h fileio.h time.h avr_mmc/ff.h avr_mmc/integer.h
fileio.o: avr_mmc/ffconf.h avr_mmc/diskio.h mainloop.h
shell.o: avr_mmc/xitoa.h uart.h fileio.h time.h avr_mmc/ff.h
shell.o: avr_mmc/integer.h avr_mmc/ffconf.h mainloop.h avr_mmc/diskio.h
utils.o: utils.h monograph.h time.h mainloop.h avr_mmc/diskio.h
utils.o: avr_mmc/integer.h mcp3208.h avr_mmc/ff.h avr_mmc/ffconf.h psound.h
utils.o: mobj.h
menu.o: menu.h mainloop.h time.h avr_mmc/diskio.h avr_mmc/integer.h
menu.o: monograph.h mobj.h utils.h laptimer.h monitor.h config.h fileio.h
menu.o: avr_mmc/ff.h avr_mmc/ffconf.h gpsio.h speed.h tacho.h
config.o: utils.h monograph.h time.h monitor.h mobj.h mainloop.h
config.o: avr_mmc/diskio.h avr_mmc/integer.h config.h rtc4543.h speed.h
config.o: avr_mmc/xitoa.h menu.h eeprom_man.h psound.h
datalogger.o: avr_mmc/xitoa.h monograph.h time.h mobj.h mainloop.h
datalogger.o: avr_mmc/diskio.h avr_mmc/integer.h menu.h mcp3208.h
datalogger.o: avr_mmc/ff.h avr_mmc/ffconf.h datalogger.h config.h
datalogger.o: eeprom_man.h fileio.h
laptimer.o: monograph.h time.h laptimer.h mobj.h mainloop.h avr_mmc/diskio.h
laptimer.o: avr_mmc/integer.h menu.h utils.h datalogger.h avr_mmc/ff.h
laptimer.o: avr_mmc/ffconf.h psound.h
speed.o: monograph.h time.h speed.h mobj.h mainloop.h avr_mmc/diskio.h
speed.o: avr_mmc/integer.h menu.h mcp3208.h avr_mmc/ff.h avr_mmc/ffconf.h
speed.o: eeprom_man.h config.h psound.h puls.h
tacho.o: tacho.h monograph.h time.h speed.h mobj.h mainloop.h
tacho.o: avr_mmc/diskio.h avr_mmc/integer.h menu.h puls.h psound.h
monitor.o: monograph.h time.h mobj.h mainloop.h avr_mmc/diskio.h
monitor.o: avr_mmc/integer.h utils.h menu.h monitor.h mcp3208.h avr_mmc/ff.h
monitor.o: avr_mmc/ffconf.h gpsio.h fileio.h filer.h psound.h
filer.o: utils.h monitor.h mainloop.h time.h avr_mmc/diskio.h
filer.o: avr_mmc/integer.h monograph.h fileio.h avr_mmc/ff.h avr_mmc/ffconf.h
filer.o: filer.h psound.h
gpsio.o: avr_mmc/xitoa.h uart.h fileio.h time.h avr_mmc/ff.h
gpsio.o: avr_mmc/integer.h avr_mmc/ffconf.h gpsio.h rtc4543.h config.h
avr_mmc/ff.o: avr_mmc/ff.h avr_mmc/integer.h avr_mmc/ffconf.h
avr_mmc/ff.o: avr_mmc/diskio.h
avr_mmc/mmc.o: avr_mmc/diskio.h avr_mmc/integer.h
avr_mmc/cc932_avr.o: avr_mmc/ff.h avr_mmc/integer.h avr_mmc/ffconf.h
psound.o: psound.h
eeprom_man.o: eeprom_man.h time.h
title.o: monograph.h time.h title.h mainloop.h avr_mmc/diskio.h
title.o: avr_mmc/integer.h mobj.h fileio.h avr_mmc/ff.h avr_mmc/ffconf.h
title.o: menu.h speed.h tacho.h laptimer.h config.h
