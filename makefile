#!/bin/make

DIR = /usr/local/bin/wifi-menu.d

make:
	make compile
	chmod +x server.js
	chmod +x wifi-menu-config.js
	mkdir $(DIR)

compile:
	g++ main.cpp -o wifi-menu -lncurses -liw -pthread -ljsoncpp
#-ljson
#-lboost_system

update:
	cp wifi-menu-config.json $(DIR)/wifi-menu-config.json
	cp wifi-menu-config.js /usr/local/bin/wifi-menu-config
	cp server.js $(DIR)/server
	cp wifi-menu /usr/local/bin/wifi-menu

install:
	make update
	cd $(DIR)
	npm i node-wifi
	npm i net
	cat README.md
