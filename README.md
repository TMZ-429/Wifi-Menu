wifi-menu, by SD

Install:
```
sudo apt install libiw-dev
sudo apt install libgcc-dev
sudo apt install libboost-all-dev
sudo apt install libjson0
sudo apt install libjson0-dev
sudo apt install libncurses-dev
# replace 'apt install' with your OS' package manager & installation argument, same goes with 'sudo' if you do not use it.
```

Getting possible wifi-devices to scan with:
```
iw dev | awk '$1=="Interface"{print $2}'
```
This command should print out possible wifi devices you can connect with.
(http://unix.stackexchange.com/questions/286721/ddg#286732)

From there, replace the "wirelessDevice" value in the wifi-men-config.json file with your desired wireless device.

Compilation:
```
sudo make 
sudo make install
```
# Replace 'sudo' with any other Super User program your OS might use.

Usage:
```
$ wifi-menu <SSID> <password>

#Using " " or any other password in the password argument when connecting to a network without a password will work*

$ wifi-menu <SSID>

# The program will then open a menu to ask for the password

# or

$ wifi-menu

(The rest will be handled by the Terminal User Interface)
```
