#!/usr/bin/env node
const prompt = require('prompt-sync')({sigint: false});
const configPath = '/root/wifi-menu-config.json';
const config = require(configPath);
const fs = require('fs');

prompt("Warning! You are about to change important settings to the program 'Wifi-Menu', press enter to continue or ^c to quit_");
config.wirelessDevice = prompt("New wireless device -> ");
config.serverPort = prompt("New server port -> ");
fs.rm(configPath, function (err) {
    if (err) {
        console.log(`[ERROR]: '${configPath}' does not exist or insufficient permissions to edit`);
    }
});
fs.appendFileSync(configPath, JSON.stringify(config));