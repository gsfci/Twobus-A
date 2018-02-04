# Twobus-A

Twobus-A is a decentralized data transfer protocol for AVR microcontrollers.
## Features
 1. Only four wires (VCC, GND, CLK, DAT)
 2. Independent on clock source (can work on internal unstable 1/4/8MHz oscillator)
 3. Up to 255 devices on one bus
 4. Decentralized (no master)
 5. Low flash memory usage
 6. High speed
 7. Builtin XOR encryption
## Limitations
 1. No ACKNOWLEDGE signal. One incorrect packet may damage all system's timings, reboot needed (will be implemented in Twobus-B)
 2. No intereference protetction (will be implemented in Twobus-D)
 3. Only one-direction sending. For example, if device A wants to read data from device B, then A will send packet with request to B, and then B will send packet with data to A, but between these packets other devices can send data to each other, which can cause delays. (will be implemented in Twobus-B)
 ## Typical connection schematic with 4x ATmega8
 ![Typical connection schematic with 4x ATmega8](https://github.com/gsfci/Twobus-A/blob/master/Twobus_A_mega8.png "Typical connection schematic with 4x ATmega8")
