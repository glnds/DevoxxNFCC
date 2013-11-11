#!/bin/bash  

# bootscript to launch scanners

# load module first

sudo modprobe spi_bcm2708

# then start all tagscanners in a screen ( following is after boot if all are inserted from left to right on order in hub, pi as last)
#Bus 001 Device 002: ID 0424:9512 Standard Microsystems Corp. 
#Bus 001 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub
#Bus 001 Device 003: ID 0424:ec00 Standard Microsystems Corp. 
#Bus 001 Device 004: ID 1a40:0101 Terminus Technology Inc. 4-Port HUB
#Bus 001 Device 005: ID 072f:90cc Advanced Card Systems, Ltd ACR38 SmartCard Reader
#Bus 001 Device 006: ID 072f:90cc Advanced Card Systems, Ltd ACR38 SmartCard Reader
#Bus 001 Device 007: ID 072f:90cc Advanced Card Systems, Ltd ACR38 SmartCard Reader

sudo screen -d -m ./poll-d acr122_usb:001:007 likes.txt http://172.19.0.40:3000/rooms/3/votes 8 like=1
sudo screen -d -m ./poll-d acr122_usb:001:006 favourites.txt http://172.19.0.40:3000/rooms/3/favourites 4 favorite=1
sudo screen -d -m ./poll-d acr122_usb:001:005 dislikes.txt http://172.19.0.40:3000/rooms/3/votes 0 like=0