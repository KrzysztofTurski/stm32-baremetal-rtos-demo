# STM32F411RE Bare-Metal RTOS Platform 

An educational bare-metal RTOS platform for the **STM32F411RE (Cortex-M4)**, written from scratch in **C + ARM assembly (PendSV)**.  
No HAL. No FreeRTOS.

This repository focuses on learning and demonstrating how a minimal **preemptive RTOS kernel**, **IPC primitives**, and **real peripheral drivers**
work together in a clean project structure and run on real hardware.

> **Status:** actively developed (WIP).  
> The RTOS core is stable (SysTick + PendSV, sleep, mutex, mailbox) and the platform already runs on real hardware with live UART logs and sensor sampling.

---

## Demo (real hardware)

**UART logging output (RealTerm):**  
![UART log screenshot](docs/images/uart_log.png)

**Hardware setup (STM32 + sensors):**  
![Hardware setup](docs/images/hardware_setup.png)

---

## Highlights

### Kernel / RTOS
- **Preemptive Round-Robin scheduler** driven by **SysTick (1 ms)** and **PendSV**
- **Context switching in ASM** (`core/rtos/osPort.s`)
- Thread states:
  - `READY`
  - `SLEEPING`
  - `BLOCKED_MUTEX`
  - `BLOCKED_QUEUE_RX`
  - `BLOCKED_QUEUE_TX`
- `IdleTask` uses **`__WFI()`** to sleep the CPU when no thread is runnable

### IPC
- **Mutex** with atomic block/unblock (lost-wakeup bug fixed)
- **Mailbox “latest sample”** (overwrite semantics, no queue buildup)
- Queue + semaphore stubs available for extension:
  - `osQueue.*`
  - `osSemaphore.*`

### Drivers / Peripherals
- **USART2 TX** logging (single-writer principle via Logger task)
- **I2C1** driver (register-level)
- Integrated sensors:
  - **LPS25HB** over I2C1 (temperature + pressure)
  - **HC-SR04** ultrasonic distance measurement using **TIM2 input capture**

---

## High-level architecture

### Scheduling flow
1. **SysTick (1 ms)** decrements `sleepTime` for sleeping threads and wakes them when it reaches 0.
2. SysTick sets **PendSV** to request a context switch.
3. **PendSV handler** saves the current context, selects the next runnable thread (RR), restores its context, and returns.

### Data flow (pipeline)

Two producer tasks sample sensors and publish data to “latest” mailboxes.  
A single Logger task consumes those samples, converts/formats them, and outputs via UART.

```text
[TaskLPS]   ┐
            ├──> mailbox (latest) ──> [TaskLogger] ──> USART2 TX
[TaskSonar] ┘
```

## Tasks (current)

- **TaskLPS (producer)**
  - reads raw temperature/pressure from LPS25HB over I2C1
  - publishes `(temp_raw, press_raw)` to the mailbox
  - sleeps ~1000 ms

- **TaskSonar (producer)**
  - triggers HC-SR04, measures echo pulse width via TIM2 input capture
  - performs `median3()` filtering (noise reduction)
  - publishes `distance_cm` to the mailbox

- **TaskLogger (consumer)**
  - receives latest samples from mailboxes
  - converts and formats values
  - prints log lines on USART2 (mutex-protected single writer)


Example log lines:
```text
LPS: 24.40 C | 1014.00 hPa
HC:  D=15 cm
```

---

## Hardware

### Target board
- **STM32F411RE** (e.g., NUCLEO-F411RE)

### Sensor wiring (current setup)

**LPS25HB (I2C1)**
- SCL: PB8
- SDA: PB9
- Speed: 100 kHz

**HC-SR04 (TIM2 Input Capture)**
- TRIG: PA0 (GPIO)
- ECHO: PA1 (TIM2_CH2, AF1)
- TIM2 configured for **1 µs tick** to measure pulse width

---

## Project structure

```text
stm32-baremetal-rtos-demo/
├── README.md
├── docs/
│   └── images/
│       ├── uart_log.png
│       └── hardware_setup.png
├── core/
│   ├── rtos/
│   │   ├── osKernel.c/.h
│   │   ├── osPort.s
│   │   └── ipc/
│   │       ├── osMutex.c/.h
│   │       ├── osQueue.c/.h
│   │       ├── osSemaphore.c/.h
│   │       └── osMailbox.c/.h
│   ├── drivers/
│   │   ├── bus/
│   │   │   ├── usart/usart2.c/.h
│   │   │   └── i2c/i2c.c/.h
│   │   └── sensors/
│   │       ├── hc_sr04/hc_sr04.c/.h
│   │       └── lps25hb/lps25hb.c/.h
│   └── app/
│       ├── app.c/.h
│       └── log/log.c/.h
├── platform/
│   └── stm32f411/cmsis/
└── ide/
    └── stm32cubeide/



---

## Build & Run

This repository contains a ready-to-import STM32CubeIDE project:

1. Open `ide/stm32cubeide/` in **STM32CubeIDE**
2. Build and flash to the board
3. Open a serial terminal for **USART2**

Recommended serial settings:
- Baudrate: **115200**
- 8N1

> Note: the project intentionally avoids HAL. All peripheral setup is register-level.

---

## Roadmap (planned)

- Architecture documentation and diagrams  
- CMSIS / platform cleanup and consistent naming  
- FIFO queue IPC with optional timeouts  
- Additional peripherals (SPI, DMA, selected sensors)  


---
## Background & Inspiration

This project is inspired by advanced embedded systems courses by  
**Israel Gbati** (author of *Bare-Metal Embedded C Programming*).

The implementation is independent and extended beyond the course material,  
serving as a long-term learning and experimentation platform focused on:

- ARM Cortex-M internals and exception handling  
- RTOS kernel design and scheduling  
- Low-level peripheral drivers  
- Practical inter-task communication and synchronization  

This repository is not a course reproduction.  
It represents an original implementation built to deepen understanding of bare-metal embedded systems and RTOS architecture through real hardware experimentation.



## License
Add a license when you are ready (MIT / Apache-2.0 / etc.).




