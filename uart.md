1. **Enable Clock**
2. **Initialize GPIO**
3. **Initialize USART (BaudRate = 115200, wordlength = 8b, **

   stopbits = 1, no parity, no hardware control, Rx/Tx)
4. **Process control.**

#### Pins

UART 1 TX remap_0 PA9

UART 1 TX remap_1 PB6


UART 1 RX remap_0 PA10

UART 1 RX remap_1 PB7



#### Functions

+ Sending Function
+ Recieve Function
+ Intertupt Function



### Main


1. **Basic experiment 1: **Using serial communication method, use microcontroller to send the names (English) of group members to PC;
2. **Basic experiment 2: **Configure the serial port to enable the PC to send data to the microcontroller, which in turn sends that data back to the PC;** **
3. **Basic experiment 3: **Using serial port and interrupt, send 1, 2, 3, 4 respectively in PC to change the state of LED1~4.
4. **Improvement experiment: **Using serial port to make a simple calculator. For example, when PC sends “2+2” to the microcontroller, it will return “4” to the PC.
