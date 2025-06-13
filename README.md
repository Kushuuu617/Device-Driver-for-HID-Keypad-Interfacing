# 🔢 Keypad Device Driver on Raspberry Pi

## 👨‍💻 Team Contributions

| Name                        | ID                | Contribution |
|-----------------------------|-------------------|--------------|
| **Atul M**                  | 2022AAPS1362G     | 40%          |
| **Kushagra Tanwar**         | 2022A3PS0349G     | 30%          |
| **Aditya Singh Jamwal**     | 2022AAPS0473G     | 30%          |

---

## 📌 Project Overview

This project implements a **custom Linux device driver** for a **4x4 matrix keypad** interfaced with a **Raspberry Pi**. The driver detects physical key presses and releases, translates them into Linux input events, and feeds them into the Linux input subsystem — simulating a standard keyboard.

> ✅ **Goal:** Interface a matrix keypad with Raspberry Pi using a kernel-level driver and generate standard keyboard events in Linux.

---

## 🔧 Hardware Design

### 🔲 Keypad Configuration

- **Matrix Size**: 4 rows × 4 columns = **16 keys**
- **Wiring**:
  - **Rows (Outputs):** GPIO 17, 5, 6, 13
  - **Columns (Inputs):** GPIO 23, 24, 12, 16

### 🧰 Working Principle

- Each key press connects a specific row-column pair.
- The scanning algorithm sets one **row HIGH** at a time and reads the state of the **column inputs**.
- If a column reads HIGH, a key press is detected at the row-column intersection.
- **Pull-down resistors** are used on the column lines to avoid floating inputs and erroneous readings.

---

## 🧠 Software Architecture

### 📂 Kernel Module Highlights

- **Kernel Structures Used**:
  - `input_dev`: Registers the keypad as a virtual input device.
  - `timer_list`: For periodic scanning using kernel timers.
  - `work_struct`: For deferring the scan task using workqueues.
  - `mutex`: Ensures thread-safe access to shared resources during scan and state change detection.

### 🔄 Key Scanning Logic

1. **Row cycling** using GPIO output.
2. **Column reading** to detect pressed keys.
3. **State management** to distinguish between press/release.
4. **Reporting** events via:
   ```c
   input_report_key(input_dev, keycode, 1 or 0);
   input_sync(input_dev);
   ```

### 🧰 Key Features

- **Event Debouncing**: Handled through periodic scan intervals.
- **Thread Safety**: `mutex` locks around shared state.
- **Input Subsystem Integration**: Allows keypresses to be recognized as standard keyboard inputs.

---

## ⚙️ Setup & Installation

### 🖥️ Prerequisites

- Raspberry Pi (any model with GPIO support)
- 4x4 Matrix Keypad
- Linux kernel headers installed
- `make`, `gcc`, and kernel module tools (`insmod`, `rmmod`, etc.)

### 🔌 Wiring Reference

| Keypad Row/Column | Raspberry Pi GPIO |
|-------------------|-------------------|
| Row 1             | GPIO 17           |
| Row 2             | GPIO 5            |
| Row 3             | GPIO 6            |
| Row 4             | GPIO 13           |
| Column 1          | GPIO 23           |
| Column 2          | GPIO 24           |
| Column 3          | GPIO 12           |
| Column 4          | GPIO 16           |

### 🚀 Installation Steps

1. **Clone repository**
   ```bash
   git clone https://github.com/<your-repo-url>
   cd keypad-driver
   ```

2. **Build the driver**
   ```bash
   make
   ```

3. **Insert the driver**
   ```bash
   sudo insmod keypad_driver.ko
   ```

4. **Check logs**
   ```bash
   dmesg | tail
   ```

5. **Remove the driver**
   ```bash
   sudo rmmod keypad_driver
   ```

---

## 🎯 Key Learning Outcomes

- Writing a basic GPIO-interfacing kernel module
- Integrating Linux input subsystem for input event generation
- Understanding Linux kernel concepts like:
  - Workqueues
  - Kernel timers
  - Mutual exclusion (`mutex`)
- Real-time hardware-software interaction on embedded Linux systems

---

## 📚 References

- 📖 [Embetronicx GPIO Driver Tutorial](https://embetronicx.com/tutorials/linux/device-drivers/gpio-driver-basic-using-raspberry-pi/)
- 📘 [Linux Kernel Input Subsystem Docs](https://www.kernel.org/doc/html/v4.15/input/input-programming.html)
- 🧵 [Deferred Work in Linux Kernel](https://linux-kernel-labs.github.io/refs/heads/master/labs/deferred_work.html#workqueues)
- 📗 [Linux Device Driver (Chapter 10)](https://litux.nl/mirror/kerneldevelopment/0672327201/ch10lev1sec7.html)

---

## ⚠️ Disclaimer

This project is intended for **educational purposes only**. Direct hardware manipulation via kernel modules can damage components or cause system instability if not handled properly. Proceed with caution and use on test systems.

---

## 📷 Project Image

![Keypad Hardware Connection](https://github.com/user-attachments/assets/d6bc2a4c-f348-4a55-9578-1e8c72d93518)
