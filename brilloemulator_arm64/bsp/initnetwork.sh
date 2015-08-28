#!/system/bin/sh

# Setup networking when boot starts
ifconfig eth0 10.0.2.15 netmask 255.255.255.0 up
# Open up port 5555 for adb
iptables -I INPUT -p tcp --dport 5555 -j ACCEPT -w
