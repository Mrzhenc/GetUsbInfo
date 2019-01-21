all:
	gcc -g libusb.c -o genUsbInfoFile -I/usr/include/libusb-1.0 -L. -lusb-1.0
clean:
	rm genUsbInfoFile
