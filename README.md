pyhidlib
========

Easy access to USB Hid Devices with Python. 
Running for most Linux-Systems.

Tested with:
- Python 2.7
- Intel-PC 32Bit Xubuntu 14
- RaspberriPi with Rasbian (Debian Wheezy)
- Intel-PC 64Bit Xubuntu 16.04 LTS (Xenial Xerus)

unfortunately with actual Linux versions there is a 64-bit problem, because ctypes in python 2.7 does handle a pointer as 32-bit integer. Workaround: use python3. Or try compiling the lib on a 32-bit system.

