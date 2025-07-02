# Compilazione del firmware

## Compilazione con Github Actions

Puoi scaricare l'ultima build di sviluppo da Github Actions

https://github.com/solosky/pixl.js/actions

## Compilazione con immagine Docker personalizzata

Puoi compilare il firmware utilizzando un'immagine Docker personalizzata.

```bash
# creare i contenitori
docker run -it --rm solosky/nrf52-sdk:latest

# inizializzare il repository
root@b10d54636088:/builds# git clone https://github.com/solosky/pixl.js
root@b10d54636088:/builds# cd pixl.js
root@b10d54636088:/builds/pixl.js# git submodule update --init --recursive

# compilare la versione LCD
root@b10d54636088:/builds/pixl.js# cd fw && make all BOARD=LCD RELEASE=1

# compilare la versione OLED
root@b10d54636088:/builds/pixl.js# cd fw && make all BOARD=OLED RELEASE=1
```

Il firmware si trova in fw/\_build/pixjs_all.hex, il pacchetto ota si trova in fw/\_build/pixjs_ota_vXXXX.zip
