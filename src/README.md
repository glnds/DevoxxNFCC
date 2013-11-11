# NFC voting setup written in C

## Dependencies

coming soon

## Compile code

Make sure to have the lpd8806led.o file on the path and libfnc and curl installed

    gcc -o poll-d poll-d.c -lnfc -lcurl lpd8806led.o -lm

## running

Requires root to run, use bootscript and double check that module for leds is loaded (spi).
Other common issues are a broken URL and missing dependencies or arguments.

Check the available devices with

    lsusb

Example to run from bootscript.sh

    sudo screen -d -m ./poll-d acr122_usb:001:007 likes.txt http://172.19.0.40:3000/rooms/3/votes 8 like=1

# prepare sd card

    sudo diskutil unmountDisk /dev/disk1
    sudo dd bs=1m of=/dev/rdisk1 if=pi-image.img

# update date

shouldn't be needed anymore. Just need to * 1000

    apt-get install ntpdate

# find ip address

    brew install nmap
    ./nmap 172.19.0.0-255 -p22 --open



