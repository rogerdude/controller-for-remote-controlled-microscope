# controller-for-remote-controlled-microscope
Project with Nucleo-F429ZI board.<br>

This repository contains the programs I developed to run on a Nucleo-F429ZI board in CSSE3010 (Embedded Systems Design and Interfacing).<br>

The mylib folder contains all the drivers I developed for hardware components such as a multifunctional shield which had buttons, LEDs, trimpot, and a seven segment display. I also used a LTA-1000G LED display, 140C05 RGB LED, and a nRF24L01P radio transceiver.<br>
The files in s1-s3 were used to help develop and test the drivers in the mylib folder.<br>

The project folder contains a program for using the board to control a remote-controlled microscope. The board gets input from a computer terminal and sends it wirelessly to the microscope to control its position, angle, and zoom.<br>
Additionally, FreeRTOS was used to implement the project.
