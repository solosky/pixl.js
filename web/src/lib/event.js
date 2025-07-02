import { EventEmitter } from "events";

var eventDispatcher = new EventEmitter();

export function sharedEventDispatcher() {
  return eventDispatcher;
}
