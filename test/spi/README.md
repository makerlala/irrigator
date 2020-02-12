
Compile and run spidev_test on a Linux host:

``$ gcc -Wall spidev_test.c -o spidev_test``


Open spidev_test.ino with Arduino Studio and upload it to Arduino.

Then run the test, e.g.:

``$ ./spidev_test -v -D /dev/spidev0.0 -s 1000 -p 1234``
