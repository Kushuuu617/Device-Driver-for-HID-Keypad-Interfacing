# Keypad Device Driver on Raspberry Pi

## A group project with contributions as : 
### Atul M(2022AAPS1362G)- 40%
### Aditya Singh Jamwal(2022AAPS0473G)-30%
### Kushagra Tanwar(2022A3PS0349G)-30%

## Introduction
This project involves designing and implementing a Linux device driver to interface a keypad with a Raspberry Pi. The goal is to detect key presses and translate them into Linux input events using the Linux input subsystem.
![image](https://github.com/user-attachments/assets/d6bc2a4c-f348-4a55-9578-1e8c72d93518)

## Hardware Implementation
On the hardware side, the keypad consists of 16 buttons arranged in a grid of 4 rows and four columns. Each button connects a unique row-column pair. The rows are configured as outputs in the Raspberry Pi (here in GPIO 17, 5, 6, and 13), while the columns are configured as inputs in the Raspberry Pi (GPIO 23, 24, 12, and 16). When a button on the keypad is pressed, it completes the circuit, which the driver can detect during the scan function. In scanning, we set one row high at a time, keep the other rows low, and check the column inputs to see if a signal is present, indicating a key press at that specific row-column intersection.
There are pull-down resistors for the column row to prevent it from reading floating values (might cause error otherwise).

## Software Implementation
On the software side, the Linux device driver handles all the logic needed to detect key presses and translate them into standard keyboard events. The input_dev structure functioned to link our virtual input device with Linux input subsystem functionalities, including keycodes configuration for keypad buttons. For key scanning, the system needs scheduled loops enabled by kernel timers and workqueue execution. The periodic workqueue execution of the timer functionality enabled the scanning function to verify key statuses for detecting press events and release events. The events were passed to the input subsystem through input_report_key() combined with input_sync() functions to simulate keyboard events properly. During scanning operations, we used a mutex to guarantee thread safety of shared data access. The Raspberry Pi could accept matrix keypad inputs, which it interpreted as if they originated from a standard USB keyboard.

## References
-  https://embetronicx.com/tutorials/linux/device-drivers/gpio-driver-basic-using-raspberry-pi/
-  https://www.kernel.org/doc/html/v4.15/input/input-programming.html
-  https://linux-kernel-labs.github.io/refs/heads/master/labs/deferred_work.html#workqueues
-  https://litux.nl/mirror/kerneldevelopment/0672327201/ch10lev1sec7.html
