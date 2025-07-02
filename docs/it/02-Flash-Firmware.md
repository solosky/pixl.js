Dopo aver costruito o acquistato l'hardware, devi flashare il firmware per la prima volta, che può essere ottenuto solo tramite il metodo cablato.

Il firmware può essere flashato o aggiornato utilizzando uno dei seguenti metodi:

## Metodo 1: Cablato

Questo metodo richiede un flasher compatibile con CMASS-DAP JLink o DAPLink. Raccomandiamo il programmatore STM32 PWLINK2 Lite Emulator, puoi comprarne uno per circa 9.9 yuan su [Taobao](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.4b942e8deXyaQO&id=675067753017&_u=d2p75qfn774a "Taobao")

Scarica l'ultima versione del pacchetto zip del firmware. Dovrebbe contenere i seguenti file:

- fw_update.bat
- bootloader.hex
- pixljs.hex
- pixljs_all.hex
- fw_readme.txt
- pixjs_ota_v237.zip

Devi collegare i cavi da 3.3 Volt, GND, SWDIO e SWDCLK. Puoi utilizzare il file `fw_update.bat` per flashare il firmware `pixjs_all.hex`.

Puoi anche utilizzare OpenOCD per flashare il dispositivo, questo è un esempio del comando da eseguire:

```
openocd -f interface/cmsis-dap.cfg -c "transport select swd" -f target/nrf52.cfg -d2 -c init -c "reset init" -c halt -c "nrf5 mass_erase" -c "program pixjs_all.hex verify" -c "program nrf52832_xxaa.hex verify" -c exit
```

Dopo che il primo flash è completato, gli aggiornamenti del firmware successivi possono essere effettuati tramite OTA.

## Metodo 2: Aggiornamento OTA

Questo metodo è applicabile solo ai dispositivi Pixl.js che sono stati programmati con successo tramite il metodo cablato.

### App nRF Connect

Installa l'applicazione nRF Connect (la puoi trovare sia su iOS che su Android app stores).

Nella lista dei dispositivi, seleziona pixl.js (o pixl dfu) e tocca il pulsante `CONNECT`

Metti il tuo dispositivo pixl.js in modalità "Aggiornamento Firmware", poi il dispositivo entrerà in modalità DFU. Per farlo, seleziona l'app `Settings` e seleziona la voce `Firmware Update`.

Apri l'app "nRF Connect" sul tuo telefono e connettiti al dispositivo chiamato `pixl dfu` per aggiornare il firmware.

Su iOS, il firmware è `pixjs_ota_vxxx.zip`

### Metodo della pagina web

Scarica l'ultima versione del pacchetto zip del firmware corrispondente alla versione del tuo dispositivo ed estrailo in una directory.

Il progetto fornisce due modi per ottenere un aggiornamento DFU:

#### Pagina web di trasferimento file.

Innanzitutto, puoi collegare il dispositivo alla [pagina web ufficiale](https://pixl.amiibo.xyz/ "pagina web ufficiale"), poi sulla pagina web, dopo che il dispositivo è connesso, premi il pulsante grigio `DFU`, il dispositivo entrerà in modalità DFU e la pagina ti chiederà "Vuoi aprire la pagina di aggiornamento DFU?" se accetti, la Pagina di Aggiornamento Firmware sarà aperta.

#### Direttamente alla Pagina di Aggiornamento Firmware.

Puoi anche andare direttamente alla Pagina di Aggiornamento Firmware.

Prima di tutto, devi mettere il tuo dispositivo pixl.js in modalità "Aggiornamento Firmware". Per farlo, seleziona l'app `Impostazioni` e seleziona la voce `Aggiornamento Firmware`.

Apri la [pagina di aggiornamento firmware](https://thegecko.github.io/web-bluetooth-dfu). Trascina e rilascia o seleziona il file `pixljs_ota_xxx.zip` dalla cartella dove hai estratto il pacchetto firmware.

Poi premi il pulsante `SELEZIONA DISPOSITIVO` sulla pagina dovresti vedere un dispositivo chiamato `pixl dfu` connetti per iniziare il processo di aggiornamento del firmware.

# Riparazione versione firmware errata.

Se per errore programmi la versione sbagliata (LCD/OLED) sul tuo dispositivo, il dispositivo funzionerà ma non mostrerà informazioni sullo schermo, la retroilluminazione della versione LCD potrebbe accendersi.

Puoi utilizzare i seguenti metodi per recuperare o flashare la versione firmware corretta.

## Opzione 1: Flasha il firmware tramite connessione cablata

Se hai a disposizione un programmatore JLink o DAPLink compatibile con CMASS-DAP, puoi utilizzare il [Metodo Cablato](Flash-Firmware#Method 1: Wired "Metodo Cablato") per flashare manualmente la versione firmware corretta.
Se per errore programmi la versione sbagliata (LCD/OLED) sul tuo dispositivo, il dispositivo funzionerà ma non mostrerà informazioni sullo schermo, la retroilluminazione della versione LCD potrebbe accendersi.

## Opzione 2: Segui le sequenze di tasti speciali per entrare nuovamente nella modalità DFU e installare la versione corretta del firmware.

Prima assicurati che il tuo dispositivo sia spento, poi premi le seguenti sequenze di tasti per entrare nella `Modalità DFU`

- Un qualsiasi tasto per risvegliare il dispositivo
- SINISTRA
- CENTRO
- SINISTRA X 4
- CENTRO

Ora il tuo dispositivo è in modalità DFU, usa l'app [nRF Connect APP](#nRF-Connect-APP) o [Direttamente dalla pagina di aggiornamento del firmware](#directly-to-the-firmware-update-page) per aggiornare il firmware.
