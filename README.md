# LED driver for Raspberry 3 B+

This driver abstracts a LED, and initially it does so by providing a procfs
entry. Then, the user is able to toggle a GPIO pin and blink a LED connected to
it. The procfs entry is a file which the user can write to, using a simple
format as input: a pair of integers separated by a comma, as "21,1". This input
tells the driver to turn the pin 21 on. Otherwise, "21,0" would turn the same
pin off.

This approach comes with a series of problems that will be addressed in the
course of the driver's development:

- We created a particular interface to the user (the procfs entry with the pair
  of integers), and what we need is actually a standardized way to read and
  write from this driver.
- One single module is mapping the whole GPIO bank of registers, so I am able
  to break UART communication simply by toggling the TX or RX pins.
- The driver contains information about the hardware, so it won't be usable if
  we change the hardware
