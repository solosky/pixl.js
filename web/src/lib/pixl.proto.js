import { sharedEventDispatcher } from "./event"
import { pixlBleTxData } from "./pixl.ble"



export function pixlProtoInit() {
    sharedEventDispatcher().addListener("ble_rx_data", pixlOnRxData);
}



function pixlOnRxData() {

}