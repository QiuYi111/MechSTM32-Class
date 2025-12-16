Final Project – LCD & AC Motor (Experiment Details)

Note: The title mentions "AC motor", but the experiment details specify controlling a "DC motor".

1. Liquid Crystal Display (LCD) Fundamentals

Basic Parameters

Pixel: The fundamental unit of the display.

Resolution: Dimensions in pixels (e.g., 800 x 480).

Size: Physical screen size.

Color Depth: Bits per pixel (e.g., 16-bit for RGB565).

Video Memory (SRAM/SDRAM)

Role: Acts as a frame buffer to cache pixel data before transmission to the screen.

Hardware: External controllers usually have built-in memory; Integrated controllers (like in STM32F429) use internal SRAM or external SDRAM.

Storage Calculation:

Goal: Must store at least one full frame (one page).

Example: For an 800x480 resolution using RGB565 format:


$$2 \text{ bytes/pixel} \times 800 \times 480 = 768,000 \text{ bytes}$$

2. Hardware Interface: FSMC & SSD1963

FSMC (Flexible Static Memory Controller) Configuration

The LCD is interfaced as a memory-mapped device using the FSMC peripheral.

Control Line (A10): Address Line 10 is used to select between Command and Data modes.

A10 = 0: Register (Command) write.

A10 = 1: Data write/read.

Memory Mapping:

LCD Address: Base address for the command register.

LCD Parameters: Base address for the data register.

SSD1963 Controller Commands

The controller uses 8-bit commands.

Command

Function

Details

0xD3

Read LCD ID

Reads the device identification code.

0x36

Scanning Direction

Sets the scan direction (orientation).

0x2A

Set Column Address

Sets the horizontal (X) start/end positions for frame memory access.

0x2B

Set Page Address

Sets the vertical (Y) start/end positions for frame memory access.

0x2C

Write GRAM

Start writing pixel data to Graphics RAM.

0x2E

Read GRAM

Start reading pixel data from Graphics RAM.

0x29

Display ON

Turn the display panel on.

0x28

Display OFF

Turn the display panel off.

3. Software Drivers & Logic

Low-Level Register Access

void LCD_WR_REG(u16 regval); — Write Command (A10=0).

void LCD_WR_DATA(u16 data); — Write Data (A10=1).

u16 LCD_RD_DATA(void); — Read Data.

High-Level API

void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue); — Write value to specific register.

u16 LCD_ReadReg(u16 LCD_Reg); — Read value from specific register.

void LCD_WriteRAM_Prepare(void); — Issue 0x2C command to prepare for pixel data stream.

void LCD_WriteRAM(u16 RGB_Code); — Write actual color value.

Structure-Based Access

The code likely uses a structure pointer to access the LCD (inferred from LCD->LCD_RAM), mapping memory addresses to struct members for cleaner syntax.

Initialization Sequence

Enable Clock: Peripheral clocks.

Initialize GPIO: Control pins (CS, RS, WR, RD, Data lines).

Initialize FSMC: Timing and bank configuration.

Read LCD ID: Validate hardware.

Initialize LCD: Send specific init sequence based on ID.

Set Parameters: e.g., display_dir(0) for screen orientation.

Backlight: LCD_LED=1 (Turn on background light).

Clear LCD: Erase video memory to black/white.

4. Graphics Algorithms

Drawing a Point

Set Cursor: Send 0x2A and 0x2B to define the X,Y position.

Prepare Write: Send 0x2C (Write GRAM command).

Write Data: Send the 16-bit Color Data.

Flow: LCD -> LCD_RAM

Drawing a Character

Font Data Sources:

asc2_1206 (Likely 12x6 pixels)

asc2_1608 (Likely 16x8 pixels)

asc2_2412 (Likely 24x12 pixels)

Note: The text also mentions "sizes of font 1212, 1616, and 24*24". Be aware of this potential discrepancy between variable names and description.

Rendering Algorithm:

Call the corresponding font array based on size.

Calculate the number of bytes per character.

Loop through each byte of the character model.

Loop through each bit in the byte.

Logic: If the bit == 1, fill the pixel with the foreground color.

5. Experiment Requirements

Experiment 1: Geometric Shapes

Use the LCD to draw the following shapes and their corresponding filled patterns:

Isosceles Triangle

Right Triangle

Arbitrary Triangle

Filled Patterns (for the above shapes)

Experiment 2: Display & Motor Control

Combine LCD display with external hardware control.

Static Display:

Show the names of group members.

Dynamic Display (Timer):

Implement a timer counting from 00:00 to 59:59.

Motor Control System:

Input: Use AD (Analog-to-Digital) conversion (likely from a potentiometer) to control the speed.

Output: Control the rotation speed of a DC Motor.

Status Display: Show the following real-time states on the LCD:

Forward (Direction)

Reverse (Direction)

Speed (Value)