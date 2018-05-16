DEVICE=attiny85
F_CPU=16000000

AVRDUDE = micronucleus --run

COMPILE = avr-gcc -Wall -Os -Iusbdrv -I. -mmcu=$(DEVICE) -DF_CPU=$(F_CPU) -DDEBUG_LEVEL=0

OBJECTS = usbdrv/osccal.o usbdrv/usbdrv.o usbdrv/usbdrvasm.o main.o

# symbolic targets:
all:	main.hex

hidtool:
	gcc hidtool.c -o hidtool

.c.o:
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@

.c.s:
	$(COMPILE) -S $< -o $@

flash:  all
	$(AVRDUDE) main.hex

clean:
	rm -f main.hex main.lst main.obj main.cof main.list main.map main.eep.hex main.bin *.o usbdrv/*.o main.s usbdrv/oddebug.s usbdrv/usbdrv.s libs-device/osccal.o

main.bin:   $(OBJECTS)
	$(COMPILE) -o main.bin $(OBJECTS)

main.hex:   main.bin
	rm -f main.hex main.eep.hex
	avr-objcopy -j .text -j .data -O ihex main.bin main.hex
	avr-size main.bin

disasm: main.bin
	avr-objdump -d main.bin

cpp:
	$(COMPILE) -E main.c
