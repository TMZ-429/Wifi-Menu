#!/usr/bin/env node
const configPath = './wifi-menu-config.json';
const config = require(configPath);
const wifi = require('node-wifi');
const net = require('net');

wifi.init({ iface: config.wirelessDevice });

const server = net.createServer((socket) => {
    socket.on('data', (data) => {
        let message = data.toString('utf8'), msg = JSON.parse(message);
        wifi.connect({ssid: msg.ssid, password: (msg.password || "")}, () => {
            wifi.getCurrentConnections((error, currentConnections) => {
            if (currentConnections.find(item => item.ssid === msg.ssid)) {
                socket.write('true');
                socket.destroy();
                process.exit(0);
            }
            //If the connection works then the process is ended and therefore anything here does not happen
             socket.write('false');
             socket.destroy();
            })
        })
    })
})

server.listen(config.serverPort);
server.maxConnections = 1;