EESchema Schematic File Version 2  date 2013/11/01 6:11:49
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:special
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:dot_matrix_led-cache
EELAYER 27 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "ATMega88 LED DotMatrix 16x16"
Date "31 oct 2013"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L ATMEGA88-P IC?
U 1 1 517BD653
P 2350 2850
F 0 "IC?" H 1500 4150 40  0000 L BNN
F 1 "ATMEGA88-P" H 2700 1500 40  0000 L BNN
F 2 "DIL28" H 2350 2850 30  0000 C CIN
F 3 "" H 2350 2850 60  0000 C CNN
	1    2350 2850
	1    0    0    -1  
$EndComp
$Comp
L 74LS138 U?
U 1 1 517BD7AB
P 5050 1650
F 0 "U?" H 4800 2150 60  0000 C CNN
F 1 "74LS138" H 4950 1100 60  0000 C CNN
F 2 "~" H 5050 1650 60  0000 C CNN
F 3 "~" H 5050 1650 60  0000 C CNN
	1    5050 1650
	1    0    0    -1  
$EndComp
$Comp
L 74LS138 U?
U 1 1 517BD7BA
P 5050 2900
F 0 "U?" H 4800 3400 60  0000 C CNN
F 1 "74LS138" H 4950 2350 60  0000 C CNN
F 2 "~" H 5050 2900 60  0000 C CNN
F 3 "~" H 5050 2900 60  0000 C CNN
	1    5050 2900
	1    0    0    -1  
$EndComp
$Comp
L NJU3711D U?
U 1 1 517C02D3
P 4700 5000
F 0 "U?" H 4550 5500 60  0000 C CNN
F 1 "NJU3711D" H 4700 4500 60  0000 C CNN
F 2 "" H 4550 5350 60  0000 C CNN
F 3 "" H 4550 5350 60  0000 C CNN
	1    4700 5000
	1    0    0    -1  
$EndComp
$Comp
L NJU3711D U?
U 1 1 517C02E2
P 4700 6250
F 0 "U?" H 4550 6750 60  0000 C CNN
F 1 "NJU3711D" H 4700 5750 60  0000 C CNN
F 2 "" H 4550 6600 60  0000 C CNN
F 3 "" H 4550 6600 60  0000 C CNN
	1    4700 6250
	1    0    0    -1  
$EndComp
Wire Wire Line
	4450 1300 4150 1300
Wire Wire Line
	4150 1300 4150 2550
Wire Wire Line
	3250 2550 4450 2550
Wire Wire Line
	4450 1400 4050 1400
Wire Wire Line
	4050 1400 4050 2650
Wire Wire Line
	3250 2650 4450 2650
Wire Wire Line
	4450 1500 3950 1500
Wire Wire Line
	3950 1500 3950 2750
Wire Wire Line
	3250 2750 4450 2750
$Comp
L FDS4935A U?
U 1 1 517C643C
P 7300 1300
F 0 "U?" H 7050 1800 60  0000 C CNN
F 1 "FDS4935A" H 7300 850 60  0000 C CNN
F 2 "" H 7350 1500 60  0000 C CNN
F 3 "" H 7350 1500 60  0000 C CNN
	1    7300 1300
	1    0    0    -1  
$EndComp
$Comp
L FDS4935A U?
U 1 1 517C644B
P 7300 2400
F 0 "U?" H 7050 2900 60  0000 C CNN
F 1 "FDS4935A" H 7300 1950 60  0000 C CNN
F 2 "" H 7350 2600 60  0000 C CNN
F 3 "" H 7350 2600 60  0000 C CNN
	1    7300 2400
	1    0    0    -1  
$EndComp
$Comp
L FDS4935A U?
U 1 1 517C645A
P 7300 3500
F 0 "U?" H 7050 4000 60  0000 C CNN
F 1 "FDS4935A" H 7300 3050 60  0000 C CNN
F 2 "" H 7350 3700 60  0000 C CNN
F 3 "" H 7350 3700 60  0000 C CNN
	1    7300 3500
	1    0    0    -1  
$EndComp
$Comp
L FDS4935A U?
U 1 1 517C647D
P 7300 4600
F 0 "U?" H 7050 5100 60  0000 C CNN
F 1 "FDS4935A" H 7300 4150 60  0000 C CNN
F 2 "" H 7350 4800 60  0000 C CNN
F 3 "" H 7350 4800 60  0000 C CNN
	1    7300 4600
	1    0    0    -1  
$EndComp
Wire Wire Line
	6700 1150 6500 1150
Wire Wire Line
	6700 1550 6500 1550
Wire Wire Line
	6700 2250 6500 2250
Wire Wire Line
	6700 2650 6500 2650
Wire Wire Line
	6700 3350 6500 3350
Wire Wire Line
	6700 3750 6500 3750
Wire Wire Line
	6700 4450 6500 4450
Wire Wire Line
	6700 4850 6500 4850
Entry Wire Line
	6400 1250 6500 1150
Entry Wire Line
	6400 1650 6500 1550
Entry Wire Line
	6400 2350 6500 2250
Entry Wire Line
	6400 2750 6500 2650
Entry Wire Line
	6400 3450 6500 3350
Entry Wire Line
	6400 3850 6500 3750
Entry Wire Line
	6400 4550 6500 4450
Entry Wire Line
	6400 4950 6500 4850
Wire Wire Line
	5650 1300 5850 1300
Wire Wire Line
	5650 1400 5850 1400
Wire Wire Line
	5650 1500 5850 1500
Wire Wire Line
	5650 1600 5850 1600
Wire Wire Line
	5650 1700 5850 1700
Wire Wire Line
	5650 1800 5850 1800
Wire Wire Line
	5650 1900 5850 1900
Wire Wire Line
	5650 2000 5850 2000
Wire Wire Line
	5650 2550 5850 2550
Wire Wire Line
	5650 2650 5850 2650
Wire Wire Line
	5650 2750 5850 2750
Wire Wire Line
	5650 2850 5850 2850
Wire Wire Line
	5650 2950 5850 2950
Wire Wire Line
	5650 3050 5850 3050
Wire Wire Line
	5650 3150 5850 3150
Wire Wire Line
	5650 3250 5850 3250
Entry Wire Line
	5850 1300 5950 1400
Entry Wire Line
	5850 1400 5950 1500
Entry Wire Line
	5850 1500 5950 1600
Entry Wire Line
	5850 1600 5950 1700
Entry Wire Line
	5850 1700 5950 1800
Entry Wire Line
	5850 1800 5950 1900
Entry Wire Line
	5850 1900 5950 2000
Entry Wire Line
	5850 2000 5950 2100
Entry Wire Line
	5850 2550 5950 2650
Entry Wire Line
	5850 2650 5950 2750
Entry Wire Line
	5850 2750 5950 2850
Entry Wire Line
	5850 2850 5950 2950
Entry Wire Line
	5850 2950 5950 3050
Entry Wire Line
	5850 3050 5950 3150
Entry Wire Line
	5850 3150 5950 3250
Entry Wire Line
	5850 3250 5950 3350
Text Label 5700 1300 0    60   ~ 0
Y0
Text Label 5700 1400 0    60   ~ 0
Y1
Text Label 5700 1500 0    60   ~ 0
Y2
Text Label 5700 1600 0    60   ~ 0
Y3
Text Label 5700 1700 0    60   ~ 0
Y4
Text Label 5700 1800 0    60   ~ 0
Y5
Text Label 5700 1900 0    60   ~ 0
Y6
Text Label 5700 2000 0    60   ~ 0
Y7
Text Label 5700 2550 0    60   ~ 0
Y8
Text Label 5700 2650 0    60   ~ 0
Y9
Text Label 5700 2750 0    60   ~ 0
Y10
Text Label 5700 2850 0    60   ~ 0
Y11
Text Label 5700 2950 0    60   ~ 0
Y12
Text Label 5700 3050 0    60   ~ 0
Y13
Text Label 5700 3150 0    60   ~ 0
Y14
Text Label 5700 3250 0    60   ~ 0
Y15
Text Label 6500 1150 0    60   ~ 0
Y0
Text Label 6500 1550 0    60   ~ 0
Y1
Text Label 6500 2250 0    60   ~ 0
Y2
Text Label 6500 2650 0    60   ~ 0
Y3
Text Label 6500 3350 0    60   ~ 0
Y4
Text Label 6500 3750 0    60   ~ 0
Y5
Text Label 6500 4450 0    60   ~ 0
Y6
Text Label 6500 4850 0    60   ~ 0
Y7
$Comp
L FDS4935A U?
U 1 1 517C67D2
P 9600 1300
F 0 "U?" H 9350 1800 60  0000 C CNN
F 1 "FDS4935A" H 9600 850 60  0000 C CNN
F 2 "" H 9650 1500 60  0000 C CNN
F 3 "" H 9650 1500 60  0000 C CNN
	1    9600 1300
	1    0    0    -1  
$EndComp
$Comp
L FDS4935A U?
U 1 1 517C67E1
P 9600 2400
F 0 "U?" H 9350 2900 60  0000 C CNN
F 1 "FDS4935A" H 9600 1950 60  0000 C CNN
F 2 "" H 9650 2600 60  0000 C CNN
F 3 "" H 9650 2600 60  0000 C CNN
	1    9600 2400
	1    0    0    -1  
$EndComp
$Comp
L FDS4935A U?
U 1 1 517C67F0
P 9600 3500
F 0 "U?" H 9350 4000 60  0000 C CNN
F 1 "FDS4935A" H 9600 3050 60  0000 C CNN
F 2 "" H 9650 3700 60  0000 C CNN
F 3 "" H 9650 3700 60  0000 C CNN
	1    9600 3500
	1    0    0    -1  
$EndComp
$Comp
L FDS4935A U?
U 1 1 517C67FF
P 9600 4600
F 0 "U?" H 9350 5100 60  0000 C CNN
F 1 "FDS4935A" H 9600 4150 60  0000 C CNN
F 2 "" H 9650 4800 60  0000 C CNN
F 3 "" H 9650 4800 60  0000 C CNN
	1    9600 4600
	1    0    0    -1  
$EndComp
Wire Wire Line
	9000 1150 8800 1150
Wire Wire Line
	9000 1550 8800 1550
Wire Wire Line
	9000 2250 8800 2250
Wire Wire Line
	9000 2650 8800 2650
Wire Wire Line
	9000 3350 8800 3350
Wire Wire Line
	9000 3750 8800 3750
Wire Wire Line
	9000 4450 8800 4450
Wire Wire Line
	9000 4850 8800 4850
Entry Wire Line
	8700 2350 8800 2250
Entry Wire Line
	8700 2750 8800 2650
Entry Wire Line
	8700 3450 8800 3350
Entry Wire Line
	8700 3850 8800 3750
Entry Wire Line
	8700 4550 8800 4450
Entry Wire Line
	8700 4950 8800 4850
Entry Wire Line
	8700 1250 8800 1150
Entry Wire Line
	8700 1650 8800 1550
$Comp
L C C?
U 1 1 517C6AED
P 950 4900
F 0 "C?" H 950 5000 40  0000 L CNN
F 1 "C" H 956 4815 40  0000 L CNN
F 2 "~" H 988 4750 30  0000 C CNN
F 3 "~" H 950 4900 60  0000 C CNN
	1    950  4900
	1    0    0    -1  
$EndComp
$Comp
L C C?
U 1 1 517C6AFC
P 950 5600
F 0 "C?" H 950 5700 40  0000 L CNN
F 1 "C" H 956 5515 40  0000 L CNN
F 2 "~" H 988 5450 30  0000 C CNN
F 3 "~" H 950 5600 60  0000 C CNN
	1    950  5600
	1    0    0    -1  
$EndComp
$Comp
L C C?
U 1 1 517C6B0B
P 1100 7250
F 0 "C?" H 1100 7350 40  0000 L CNN
F 1 "C" H 1106 7165 40  0000 L CNN
F 2 "~" H 1138 7100 30  0000 C CNN
F 3 "~" H 1100 7250 60  0000 C CNN
	1    1100 7250
	1    0    0    -1  
$EndComp
$Comp
L C C?
U 1 1 517C6B1A
P 1500 7250
F 0 "C?" H 1500 7350 40  0000 L CNN
F 1 "C" H 1506 7165 40  0000 L CNN
F 2 "~" H 1538 7100 30  0000 C CNN
F 3 "~" H 1500 7250 60  0000 C CNN
	1    1500 7250
	1    0    0    -1  
$EndComp
$Comp
L C C?
U 1 1 517C6B29
P 1900 7250
F 0 "C?" H 1900 7350 40  0000 L CNN
F 1 "C" H 1906 7165 40  0000 L CNN
F 2 "~" H 1938 7100 30  0000 C CNN
F 3 "~" H 1900 7250 60  0000 C CNN
	1    1900 7250
	1    0    0    -1  
$EndComp
$Comp
L C C?
U 1 1 517C6B38
P 2300 7250
F 0 "C?" H 2300 7350 40  0000 L CNN
F 1 "C" H 2306 7165 40  0000 L CNN
F 2 "~" H 2338 7100 30  0000 C CNN
F 3 "~" H 2300 7250 60  0000 C CNN
	1    2300 7250
	1    0    0    -1  
$EndComp
$Comp
L CP1 C?
U 1 1 517C6B47
P 950 6300
F 0 "C?" H 1000 6400 50  0000 L CNN
F 1 "CP1" H 1000 6200 50  0000 L CNN
F 2 "~" H 950 6300 60  0000 C CNN
F 3 "~" H 950 6300 60  0000 C CNN
	1    950  6300
	1    0    0    -1  
$EndComp
Text Label 8800 1150 0    60   ~ 0
Y8
Text Label 8800 1550 0    60   ~ 0
Y9
Text Label 8800 2250 0    60   ~ 0
Y10
Text Label 8800 2650 0    60   ~ 0
Y11
Text Label 8800 3350 0    60   ~ 0
Y12
Text Label 8800 3750 0    60   ~ 0
Y13
Text Label 8800 4450 0    60   ~ 0
Y14
Text Label 8800 4850 0    60   ~ 0
Y15
Connection ~ 4150 2550
Connection ~ 4050 2650
Connection ~ 3950 2750
Wire Wire Line
	3250 2850 4250 2850
Wire Wire Line
	4250 2000 4250 3050
Wire Wire Line
	4250 2000 4450 2000
Wire Wire Line
	4250 3050 4450 3050
Connection ~ 4250 2850
Wire Wire Line
	4450 1900 4350 1900
Wire Wire Line
	4350 1100 4350 3500
Wire Wire Line
	4350 3150 4450 3150
Wire Wire Line
	4350 3500 3250 3500
Connection ~ 4350 3150
$Comp
L R R?
U 1 1 517D442E
P 3950 1100
F 0 "R?" V 4030 1100 40  0000 C CNN
F 1 "470K" V 3957 1101 40  0000 C CNN
F 2 "~" V 3880 1100 30  0000 C CNN
F 3 "~" H 3950 1100 30  0000 C CNN
	1    3950 1100
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4200 1100 4350 1100
Connection ~ 4350 1900
$Comp
L +5V #PWR?
U 1 1 517D44AE
P 3500 950
F 0 "#PWR?" H 3500 1040 20  0001 C CNN
F 1 "+5V" H 3500 1040 30  0000 C CNN
F 2 "" H 3500 950 60  0000 C CNN
F 3 "" H 3500 950 60  0000 C CNN
	1    3500 950 
	1    0    0    -1  
$EndComp
Wire Wire Line
	3500 950  3500 1250
Wire Wire Line
	3500 1100 3700 1100
Wire Wire Line
	4450 1800 3850 1800
Wire Wire Line
	3850 1800 3850 1250
Wire Wire Line
	3850 1250 3500 1250
Connection ~ 3500 1100
$Comp
L GND #PWR?
U 1 1 517D454B
P 4250 4150
F 0 "#PWR?" H 4250 4150 30  0001 C CNN
F 1 "GND" H 4250 4080 30  0001 C CNN
F 2 "" H 4250 4150 60  0000 C CNN
F 3 "" H 4250 4150 60  0000 C CNN
	1    4250 4150
	1    0    0    -1  
$EndComp
Wire Wire Line
	4450 3250 4250 3250
Wire Wire Line
	4250 3250 4250 4150
Wire Bus Line
	5950 1400 5950 5150
Wire Bus Line
	5950 5150 6000 5200
Wire Bus Line
	6350 5200 6400 5150
Wire Bus Line
	6400 5150 6400 1250
Wire Bus Line
	8650 5200 8700 5150
Wire Bus Line
	8700 5150 8700 1250
Wire Wire Line
	7900 950  8000 950 
Wire Wire Line
	8000 950  8000 1150
Wire Wire Line
	8000 1150 7900 1150
Wire Wire Line
	7900 1350 8000 1350
Wire Wire Line
	8000 1350 8000 1550
Wire Wire Line
	8000 1550 7900 1550
Wire Wire Line
	7900 2050 8000 2050
Wire Wire Line
	8000 2050 8000 2250
Wire Wire Line
	8000 2250 7900 2250
Wire Wire Line
	7900 2450 8000 2450
Wire Wire Line
	8000 2450 8000 2650
Wire Wire Line
	8000 2650 7900 2650
Wire Wire Line
	7900 3150 8000 3150
Wire Wire Line
	8000 3150 8000 3350
Wire Wire Line
	8000 3350 7900 3350
Wire Wire Line
	7900 3550 8000 3550
Wire Wire Line
	8000 3550 8000 3750
Wire Wire Line
	8000 3750 7900 3750
Wire Wire Line
	7900 4250 8000 4250
Wire Wire Line
	8000 4250 8000 4450
Wire Wire Line
	8000 4450 7900 4450
Wire Wire Line
	7900 4650 8000 4650
Wire Wire Line
	8000 4650 8000 4850
Wire Wire Line
	8000 4850 7900 4850
Wire Wire Line
	10200 950  10300 950 
Wire Wire Line
	10300 950  10300 1150
Wire Wire Line
	10300 1150 10200 1150
Wire Wire Line
	10200 1350 10300 1350
Wire Wire Line
	10300 1350 10300 1550
Wire Wire Line
	10300 1550 10200 1550
Wire Wire Line
	10200 2050 10300 2050
Wire Wire Line
	10300 2050 10300 2250
Wire Wire Line
	10300 2250 10200 2250
Wire Wire Line
	10200 2450 10300 2450
Wire Wire Line
	10300 2450 10300 2650
Wire Wire Line
	10300 2650 10200 2650
Wire Wire Line
	10200 3150 10300 3150
Wire Wire Line
	10300 3150 10300 3350
Wire Wire Line
	10300 3350 10200 3350
Wire Wire Line
	10200 3550 10300 3550
Wire Wire Line
	10300 3550 10300 3750
Wire Wire Line
	10300 3750 10200 3750
Wire Wire Line
	10200 4250 10300 4250
Wire Wire Line
	10300 4250 10300 4450
Wire Wire Line
	10300 4450 10200 4450
Wire Wire Line
	10200 4650 10300 4650
Wire Wire Line
	10300 4650 10300 4850
Wire Wire Line
	10300 4850 10200 4850
$Comp
L +5V #PWR?
U 1 1 517D4C96
P 6200 750
F 0 "#PWR?" H 6200 840 20  0001 C CNN
F 1 "+5V" H 6200 840 30  0000 C CNN
F 2 "" H 6200 750 60  0000 C CNN
F 3 "" H 6200 750 60  0000 C CNN
	1    6200 750 
	1    0    0    -1  
$EndComp
Wire Wire Line
	6200 750  6200 4650
Wire Wire Line
	6200 950  6700 950 
Wire Wire Line
	6200 1350 6700 1350
Connection ~ 6200 950 
Wire Wire Line
	6200 2050 6700 2050
Connection ~ 6200 1350
Wire Wire Line
	6200 2450 6700 2450
Connection ~ 6200 2050
Wire Wire Line
	6200 3150 6700 3150
Connection ~ 6200 2450
Wire Wire Line
	6200 3550 6700 3550
Connection ~ 6200 3150
$Comp
L +5V #PWR?
U 1 1 517D5197
P 8500 750
F 0 "#PWR?" H 8500 840 20  0001 C CNN
F 1 "+5V" H 8500 840 30  0000 C CNN
F 2 "" H 8500 750 60  0000 C CNN
F 3 "" H 8500 750 60  0000 C CNN
	1    8500 750 
	1    0    0    -1  
$EndComp
Wire Wire Line
	8500 750  8500 4650
Wire Wire Line
	8500 950  9000 950 
Wire Wire Line
	8500 1350 9000 1350
Connection ~ 8500 950 
Wire Wire Line
	8500 2050 9000 2050
Connection ~ 8500 1350
Wire Wire Line
	8500 2450 9000 2450
Connection ~ 8500 2050
Wire Wire Line
	8500 3150 9000 3150
Connection ~ 8500 2450
Wire Wire Line
	8500 3550 9000 3550
Connection ~ 8500 3150
Wire Wire Line
	8500 4250 9000 4250
Connection ~ 8500 3550
Wire Wire Line
	8500 4650 9000 4650
Connection ~ 8500 4250
Wire Bus Line
	6000 5200 8650 5200
Wire Wire Line
	6200 4250 6700 4250
Connection ~ 6200 3550
Wire Wire Line
	6200 4650 6700 4650
Connection ~ 6200 4250
Wire Wire Line
	5250 4650 5450 4650
Wire Wire Line
	5250 4750 5450 4750
Wire Wire Line
	5250 4850 5450 4850
Wire Wire Line
	5250 4950 5450 4950
Wire Wire Line
	5250 5050 5450 5050
Wire Wire Line
	5250 5150 5450 5150
Wire Wire Line
	5250 5250 5450 5250
Wire Wire Line
	5250 5350 5450 5350
Wire Wire Line
	5250 5900 5450 5900
Wire Wire Line
	5250 6000 5450 6000
Wire Wire Line
	5250 6100 5450 6100
Wire Wire Line
	5250 6200 5450 6200
Wire Wire Line
	5250 6300 5450 6300
Wire Wire Line
	5250 6400 5450 6400
Wire Wire Line
	5250 6500 5450 6500
Wire Wire Line
	5250 6600 5450 6600
Text Label 5250 4650 0    60   ~ 0
X0
Text Label 5250 4750 0    60   ~ 0
X1
Text Label 5250 4850 0    60   ~ 0
X2
Text Label 5250 4950 0    60   ~ 0
X3
Text Label 5250 5050 0    60   ~ 0
X4
Text Label 5250 5150 0    60   ~ 0
X5
Text Label 5250 5250 0    60   ~ 0
X6
Text Label 5250 5350 0    60   ~ 0
X7
Text Label 5250 5900 0    60   ~ 0
X8
Text Label 5250 6000 0    60   ~ 0
X9
Text Label 5250 6100 0    60   ~ 0
X10
Text Label 5250 6200 0    60   ~ 0
X11
Text Label 5250 6300 0    60   ~ 0
X12
Text Label 5250 6400 0    60   ~ 0
X13
Text Label 5250 6500 0    60   ~ 0
X14
Text Label 5250 6600 0    60   ~ 0
X15
Entry Wire Line
	5450 4650 5550 4750
Entry Wire Line
	5450 4750 5550 4850
Entry Wire Line
	5450 4850 5550 4950
Entry Wire Line
	5450 4950 5550 5050
Entry Wire Line
	5450 5050 5550 5150
Entry Wire Line
	5450 5150 5550 5250
Entry Wire Line
	5450 5250 5550 5350
Entry Wire Line
	5450 5350 5550 5450
Entry Wire Line
	5450 5900 5550 6000
Entry Wire Line
	5450 6000 5550 6100
Entry Wire Line
	5450 6100 5550 6200
Entry Wire Line
	5450 6200 5550 6300
Entry Wire Line
	5450 6300 5550 6400
Entry Wire Line
	5450 6400 5550 6500
Entry Wire Line
	5450 6500 5550 6600
Entry Wire Line
	5450 6600 5550 6700
Wire Wire Line
	3250 1700 3750 1700
Wire Wire Line
	3750 1700 3750 5150
Wire Wire Line
	3750 5150 4150 5150
Wire Wire Line
	3250 1800 3650 1800
Wire Wire Line
	3650 1800 3650 6400
Wire Wire Line
	3650 6400 4150 6400
Wire Wire Line
	3250 1900 3550 1900
Wire Wire Line
	3550 1900 3550 6300
Wire Wire Line
	3550 5050 4150 5050
Wire Wire Line
	3550 6300 4150 6300
Connection ~ 3550 5050
Wire Wire Line
	3250 2000 3450 2000
Wire Wire Line
	3450 2000 3450 6200
Wire Wire Line
	3450 4950 4150 4950
Wire Wire Line
	3450 6200 4150 6200
Connection ~ 3450 4950
Wire Wire Line
	4150 4850 4000 4850
Wire Wire Line
	4000 4650 4000 6100
Wire Wire Line
	4000 6100 4150 6100
Wire Wire Line
	4000 4650 4150 4650
Connection ~ 4000 4850
$EndSCHEMATC
