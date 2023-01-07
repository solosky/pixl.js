import { sharedEventDispatcher } from "./event";

const NUS_SERVICE_UUID = "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
const NUS_CHAR_TX_UUID = "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
const NUS_CHAR_RX_UUID = "6e400003-b5a3-f393-e0a9-e50e24dcca9e"


var nus_service;
var nus_char_rx;
var nus_char_tx;

export function pixlBleConnect() {
    return navigator.bluetooth.requestDevice({
        filters: [
            { services: [NUS_SERVICE_UUID] }
        ],
        optionalServices: [NUS_SERVICE_UUID]
    })
        .then(device => {
            console.log('Connecting to GATT Server...');
            device.addEventListener('gattserverdisconnected', onDeviceDisconnected);
            return device.gatt.connect();
        })
        .then(server => {
            console.log('Getting Services...');
            return server.getPrimaryServices();
        })
        .then(services => {
            console.log('Getting Characteristics...');
            services.forEach(service => {
                if (service.uuid == NUS_SERVICE_UUID) {
                    nus_service = service;
                    console.log('> found nus Service: ' + service.uuid);
                }
            });
            return nus_service.getCharacteristics();
        })
        .then(characteristics => {
            characteristics.forEach(characteristic => {
                console.log('>> Characteristic: ' + characteristic.uuid + ' ' +
                    getSupportedProperties(characteristic));
                if (characteristic.uuid == NUS_CHAR_TX_UUID) {
                    nus_char_tx = characteristic;

                } else if (characteristic.uuid == NUS_CHAR_RX_UUID) {
                    nus_char_rx = characteristic;
                    characteristic.addEventListener('characteristicvaluechanged',
                        onRxDataReceived);
                    characteristic.startNotifications();
                }
            });
            sharedEventDispatcher().emit("ble_connected");
        })
        .catch(error => {
            sharedEventDispatcher().emit("ble_connect_error");
        });
}


export function pixlBleTxData(buff){
    return nus_char_tx.writeValue(buff);
}

/* Utils */

function onRxDataReceived(event) {
    console.log(event);
    sharedEventDispatcher().emit("ble_rx_data", event.target.value);
}

function onDeviceDisconnected(event) {
    sharedEventDispatcher().emit("ble_disconnected");
}

function getSupportedProperties(characteristic) {
    let supportedProperties = [];
    for (const p in characteristic.properties) {
        if (characteristic.properties[p] === true) {
            supportedProperties.push(p.toUpperCase());
        }
    }
    return '[' + supportedProperties.join(', ') + ']';
}
