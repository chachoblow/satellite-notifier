# satellite-notifier

UART

To read serial data on Mac:
- Open terminal.
- Find the wanted serial port. 
  - It is probably something like "/dev/tty.usb...". 
  - You can run "ls /dev/*" with the device unplugged and then plugged to see which is the correct port.
- Run "screen \<serial port\> \<baud rate\>" to see serial output.
- To exit, press "CRTL-A" and then "CTRL-\". 
