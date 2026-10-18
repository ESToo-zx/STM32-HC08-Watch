# STM32 + HC-08 Bluetooth Watch

**English** | [简体中文](README.zh-CN.md)

**Personal learning project | Jul–Aug 2026**

A project based on [BQT0411/STM32-WATCH](https://github.com/BQT0411/STM32-WATCH), built around the STM32F103C8T6. The work covers code refactoring, feature additions, an STM32 HAL port, schematic design, PCB layout, and board assembly.

## Contributions and Project Status

- **FSM refactoring**: Reorganized the reference code, rebuilt the main menu as a finite state machine, and organized subsequent feature interactions around explicit states. Redesigned the stopwatch and countdown UIs.
- **Feature additions**: Added a countdown timer, HC-08 Bluetooth time synchronization, idle timeout into Standby, and wake-up through PA0.
- **Two firmware versions**: Completed the Standard Peripheral Library version first, then used it as the basis for the HAL port.
- **Hardware development**: Completed schematic design, PCB layout, assembly of two boards, and power-on testing.

The added, refactored, and ported firmware code was handwritten for practice, without AI-generated or AI-modified code. Reference projects and third-party code are credited below.

| Version / Hardware | Current status |
| --- | --- |
| **Standard Peripheral Library firmware + breadboard** | All implemented features work correctly, including RTC timekeeping, local and Bluetooth time setting, and time retention across resets. |
| **HAL port** | An unresolved RTC bug causes the device to hang and the time to reset to zero after a reset. |
| **Assembled PCBs** | Both boards can be programmed and run the main application logic. Damaged MPU6050 chips and unreliable button detection prevented completion of a fully functional device. |

The Standard Peripheral Library version implements **time and date display, local time setting, Bluetooth time synchronization, a stopwatch, a countdown timer, a spirit level, the dinosaur game, battery indication, and Standby/wake-up**. `Alarm` is a menu placeholder and is not implemented.

<details>
<summary>HAL RTC and PCB issue notes</summary>

**HAL RTC**: The port uses custom `My_RTC_WriteTimeCounter` and `My_RTC_ReadTimeCounter` functions, based on internal HAL implementations, to access the RTC CNT counter directly. The root cause of the hang and time reset has not been identified; the implementation remains unchanged. See the [RTC module](firmware/HAL/Core/System/Src/MyRTC.c) and [initialization code](firmware/HAL/Core/Src/main.c).

**PCB**: A soldering short on the first board damaged the ST-Link's 3.3 V regulator during power-on. Subsequently powering the board directly from 5 V damaged the MPU6050. The supply error was discovered only after testing the second board; by then, both boards had been exposed to 5 V and neither MPU6050 worked. Button presses were also frequently missed. Interference related to PCB routing is suspected, but further measurements have not established the specific cause. Functional validation was completed on the breadboard, while the PCBs reached assembly and partial testing only.

</details>

## Project Images

<img src="assets/images/breadboard-running.png" alt="Clock screen running on the breadboard prototype" width="680">

*Clock screen on the breadboard prototype.*

<details>
<summary>Breadboard wiring, HC-08, and Bluetooth time synchronization</summary>

<img src="assets/images/breadboard-wiring.png" alt="Breadboard wiring" width="480">

<img src="assets/images/hc08-module.png" alt="HC-08 Bluetooth module" width="420">

<img src="assets/images/bluetooth-time-setting.png" alt="Bluetooth time synchronization test on the breadboard" width="480">

</details>

<details>
<summary>Schematic and PCB layout</summary>

<img src="assets/images/schematic.png" alt="Project schematic" width="960">

<img src="assets/images/pcb-layout.png" alt="PCB layout and routing" width="640">

</details>

<details>
<summary>PCB assembly and partial testing (not a fully functional device)</summary>

<img src="assets/images/pcb-assembled.png" alt="Assembled PCB" width="480">

<img src="assets/images/pcb-display-test.png" alt="Partial display test with the PCB connected to an OLED" width="560">

</details>

Hardware documentation currently consists of images only; editable EDA projects and Gerber files are not included.

## Technical Overview and Source Code

The hardware includes an STM32F103C8T6, an I²C OLED, an MPU6050, an HC-08, buttons, a battery connector, a TP4056 charging circuit, an ME6211 3.3 V regulator circuit, and an SWD interface.

The main loop handles page interactions, while TIM2 provides an approximately 1 ms tick for button scanning, timing, and game updates. Menu handlers are dispatched by the current state; the countdown timer manages its running state and settings page separately. The device enters Standby after approximately 120 seconds of inactivity. Battery indication uses ADC sampling; its accuracy and battery life have not been measured.

Bluetooth time synchronization receives an ASCII Unix timestamp in seconds through the HC-08 and USART1, assembles the message, converts the time, and updates the RTC. Date and time are displayed in UTC+8.

### Project Files

| Version | Development tools | Project files |
| --- | --- | --- |
| Standard Peripheral Library | Keil5 | [App_Watch.uvprojx](firmware/StdPeriph/App_Watch.uvprojx) |
| HAL | STM32CubeMX + Keil5 | [Keil project](firmware/HAL/MDK-ARM/STM32Watch_HAL.uvprojx), [CubeMX configuration](firmware/HAL/STM32Watch_HAL.ioc) |

## References and Acknowledgments

- [BQT0411/STM32-WATCH](https://github.com/BQT0411/STM32-WATCH): The main reference project; the dinosaur game also comes from this project.
- Jiangxie Technology OLED driver and other third-party code: Original attribution and copyright notices are preserved.
- STMicroelectronics Standard Peripheral Library, HAL, and ARM CMSIS: Original files and license notices are preserved. See the [HAL driver license](firmware/HAL/Drivers/STM32F1xx_HAL_Driver/LICENSE.txt) and [CMSIS license](firmware/HAL/Drivers/CMSIS/LICENSE.txt).

No single repository-wide license has been added to cover all components. Refer to each component's original source and applicable license.
