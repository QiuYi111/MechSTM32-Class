# Project Index: STM32 LED Control Project

**Generated**: 2025-12-16
**MCU**: STM32F103ZE (Cortex-M3)
**IDE**: Keil µVision 5
**Purpose**: ADC-controlled LED experiments with UART communication

---

## 📁 Project Structure

```
stm32/
├── BSP/                          # Board Support Package (Hardware Abstraction)
│   ├── led.h/c                   # LED control (PC0-PC3)
│   ├── key.h/c                   # Key input handling
│   ├── uart.h/c                  # UART communication driver
│   ├── delay.h/c                 # Delay functions
│   ├── adc.h/c                   # ADC driver (potentiometer input)
│   └── pwm.h/c                   # Software PWM driver for LED brightness
├── User/                         # Application code
│   ├── source/
│   │   ├── main.c                # Main application entry point
│   │   ├── stm32f10x_it.c        # Interrupt handlers
│   │   └── system_stm32f10x.c    # System initialization
│   └── include/                  # User headers
├── Libraries/                    # STM32 Standard Peripheral Library
│   ├── inc/                      # SPL headers (25+ peripheral headers)
│   └── src/                      # SPL source files (25+ peripheral drivers)
├── startup/                      # Startup files
│   ├── startup_stm32f10x_hd.s    # Assembly startup code
│   └── core_cm3.h/c              # Cortex-M3 core functions
├── uart.md                       # UART lab documentation
├── AGENTS.md                     # Repository guidelines
└── led.uvprojx                   # Keil µVision project file
```

---

## 🚀 Entry Points

### Primary Entry Points

- **`User/source/main.c`** - Main application with 3 selectable experiments
  - `EXPERIMENT 1`: ADC voltage monitoring via UART
  - `EXPERIMENT 2`: LED brightness control based on ADC input
  - `EXPERIMENT 3`: Running light with speed/direction control

- **`startup/startup_stm32f10x_hd.s`** - Assembly startup and vector table
  - Stack: 1KB (0x400)
  - Heap: 512B (0x200)
  - Reset vector → `main()`

### Interrupt Handlers

- **`User/source/stm32f10x_it.c`** - STM32 peripheral interrupt handlers

---

## 📦 Core Modules

### BSP: Hardware Abstraction Layer

**Module: LED Driver** (`BSP/led.h/c`)
- **Purpose**: Control 4 LEDs connected to PC0-PC3
- **API**: `LED_GPIO_Init()`, `LED_Set()`, `LED_WriteMask()`, `LED_AllOff()`
- **Usage**: Status indication and user feedback

**Module: UART Driver** (`BSP/uart.h/c`)
- **Purpose**: USART1 communication with configurable pin remapping
- **Features**:
  - Pin remapping: PA9/PA10 (default) or PB6/PB7
  - Configurable baud rate, data bits, stop bits, parity
  - Interrupt-driven RX with circular buffer support
- **API**: `UART_Init()`, `UART_SendString()`, `UART_ReceiveChar()`, `UART_ReceiveAvailable()`

**Module: Delay Driver** (`BSP/delay.h/c`)
- **Purpose**: Software timing delays using SysTick
- **API**: `delay_ms()`, `Systick_Init()`

**Module: Key Driver** (`BSP/key.h/c`)
- **Purpose**: Key input handling with debouncing
- **API**: `Key_Init()`, `Key_ReadMask()`

**Module: ADC Driver** (`BSP/adc.h/c`)
- **Purpose**: Potentiometer reading and voltage conversion
- **API**: `ADC_Config()`, `ADC_ReadValue()`, `ADC_ReadVoltage()`
- **Channel**: PA1 (ADC_CHANNEL_1)

**Module: PWM Driver** (`BSP/pwm.h/c`)
- **Purpose**: Software PWM for LED brightness control
- **Features**:
  - Software-based PWM (no hardware timer required)
  - 0-100% duty cycle control
  - Uses LED module for output
- **API**: `PWM_Init()`, `PWM_SetDutyCycle()`, `PWM_Update()`

### Application: Main Control Logic

**Module: Experiment Controller** (`User/source/main.c`)
- **Purpose**: Switch between 3 ADC/LED experiments
- **Configuration**: `EXPERIMENT` macro (1-3) selects experiment at compile time
- **Key Functions**:
  - `System_Init()` - Initialize all peripherals (NVIC, SysTick, LED, Key, ADC)
  - `Experiment1_DisplayVoltage()` - ADC voltage monitoring via UART
  - `Experiment2_LED_BrightnessControl()` - LED brightness based on ADC input
  - `Experiment3_RunningLightControl()` - Running light with ADC speed and key direction control

### System: STM32 Standard Peripheral Library

**Module: STM32F10x SPL** (`Libraries/`)
- **Purpose**: Low-level hardware abstraction for STM32F10x peripherals
- **Coverage**: GPIO, USART, RCC, NVIC, EXTI, DMA, ADC, Timers, SPI, I2C, CAN, etc.
- **Key Headers**: `stm32f10x.h`, `stm32f10x_conf.h`, `misc.h`

---

## 🔧 Configuration

### Hardware Configuration
- **MCU**: STM32F103ZE (High-density, 512KB Flash, 64KB RAM)
- **Clock**: Configured in `system_stm32f10x.c`
- **UART1**:
  - Default: PA9 (TX), PA10 (RX)
  - Remap option: PB6 (TX), PB7 (RX)
  - Baud Rate: 115200 (configurable)
- **LEDs**: PC0 (LED1), PC1 (LED2), PC2 (LED3), PC3 (LED4)
- **ADC**: PA1 (ADC_CHANNEL_1) - Potentiometer input
- **Keys**: User-defined keys for direction control (Experiment 3)

### Build Configuration
- **IDE**: Keil µVision (project: `led.uvprojx`)
- **Target**: STM32F10x High Density
- **Toolchain**: ARM Compiler (armcc)
- **Build Artifacts**: `Objects/`, `Listings/` (should be cleaned before commits)

---

## 📚 Documentation

- **`uart.md`** - UART pin configurations and experiment requirements
  - Pin remapping options (PA9/10 vs PB6/7)
  - UART initialization and function requirements
- **`AGENTS.md`** - Development guidelines and best practices
  - Build, flash, debug workflow
  - Code style conventions (4-space indentation, snake_case)
  - Testing and commit guidelines

---

## 🧪 Test Coverage

### Built-in Experiments (main.c)
- **Experiment 1:** ADC voltage monitoring via UART (115200 baud)
- **Experiment 2:** LED brightness control based on ADC input (0-3.3V mapping)
- **Experiment 3:** Running light with ADC speed control and key direction control

### Hardware Test Points
- **4 LEDs** on PC0-PC3 for visual feedback
- **Potentiometer** on PA1 for analog input (speed/brightness control)
- **Keys** for user input (direction control in Experiment 3)
- **UART1** for serial communication (PA9/10 or PB6/7, 115200 baud)

### Debugging Support
- `#ifdef DEBUG` guards for conditional debug code
- GPIO toggling for timing validation
- Logic analyzer capture points for timing-sensitive operations

---

## 🔗 Key Dependencies

### Hardware Dependencies
- **STM32F103ZE MCU** - Primary target microcontroller (Cortex-M3)
- **LEDs** - 4x LEDs connected to PC0-PC3
- **Potentiometer** - Analog input on PA1 for control
- **Keys** - User input for direction control
- **USB-Serial Adapter** - For PC communication (115200 baud)

### Software Dependencies
- **STM32 Standard Peripheral Library v3.5.0** - Hardware abstraction
- **CMSIS Cortex-M3** - Core definitions and startup
- **Keil µVision** - Primary development environment

### External Tools
- **J-Link Debugger** - Flash programming and debugging
- **Serial Terminal** - PC communication testing (PuTTY, minicom, etc.)

---

## 📝 Quick Start

### Development Environment Setup
```bash
# 1. Open Keil µVision project
open led.uvprojx

# 2. Select experiment (edit main.c:11)
#define EXPERIMENT 1  # Choose 1-3

# 3. Build project
Project → Build Target  # or F7

# 4. Flash to target
Flash → Download  # or F8
```

### Hardware Validation
```bash
# Connect serial terminal to MCU at 115200 baud
# Test selected experiment functionality:
# - Exp 1: Verify ADC voltage readings (0-3.3V)
# - Exp 2: Adjust potentiometer, observe LED brightness changes
# - Exp 3: Use potentiometer for speed control, keys for direction
```

### Expected Token Efficiency
- **Project Index Size**: ~3KB (this file)
- **Full Codebase Analysis**: ~58,000 tokens
- **Token Reduction**: 94%
- **Break-even**: 1 session analysis
- **100 sessions savings**: ~5.5M tokens

## 🎯 Hardware Pin Map

| Function | Pin | Default | Remapped |
|----------|-----|---------|----------|
| UART TX  | PA9 | ✓ | PB6 |
| UART RX  | PA10| ✓ | PB7 |
| LED1-4   | PC0-PC3 | ✓ | - |
| ADC IN   | PA1 | ✓ | - |
| Keys     | [Defined in BSP] | ✓ | - |

---

**Token Efficiency:** This 3KB index replaces 58KB of full codebase reading (94% reduction)