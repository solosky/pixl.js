#Pixl.js

Este proyecto es una bifurcación del proyecto original [Pixl.js](http://www.espruino.com/Pixl.js). Su principal funcion es simular un Amiibo.

## picture

![image](https://github.com/solosky/pixl.js/blob/main/assets/pixljs-3.jpg)
![image](https://github.com/solosky/pixl.js/blob/main/assets/pixljs-4.jpg)

![image](https://github.com/solosky/pixl.js/blob/main/assets/pixljs-5.jpg)


## Demostración de uso

[La más reciente versión del simulador de Amiibo permite ilimitadas lecturas por parte de los juegos!, lo cual se realiza mediante generación aleatoría del UUID en cada lectura](https://www.bilibili.com/video/BV1TD4y1t76A/)

## Carcaza

El señor Baicheng creó una carcaza para éste proyecto, puede ser descargada [aquí](https://www.thingiverse.com/thing:5877482)
![image](https://github.com/solosky/pixl.js/blob/main/assets/pixjs-case1.png)

##PCB

La versión hw/RevC es la más reciente versión del PCB, por favor utilizar [Kicad 6](https://www.kicad.org/download/) para abrila y editarla.

![image](https://github.com/solosky/pixl.js/blob/main/assets/pixljs-pcb-revc.png)

## Lista de Materiales

* [Lista de Materiales interactiva HTML para RevC](docs/RevC-ibom.html)
* [Lista de Materiales y guía de compra de la RevC](docs/RevC-bom.md)

## Versiones alternas

[docs/boards-thirdparty.md](docs/boards-thirdparty.md)

## Versión OLED

Netizen @xiaohail ha contribuído con la versión OLED del dispositivo. El PCB puede ser descargado [aquí](https://gitlab.com/xiaohai/pixl.js).

![image](https://github.com/solosky/pixl.js/blob/main/assets/pixljs-oled1.png)


## Compilar el firmware

El proyecto tiene una implementación de integración continua (CI), por lo cual la compilación de la versión «Nightly» puede ser descargada en la sección [Acciones](https://github.com/solosky/pixl.js/actions/workflows/pixl.js-fw.yml) evitándo tener que ser compilada por usted.

Sin embargo, si usted requiere compilarlar el código por usted mismo, se recomienda utilizar la imágen docker provista para tal fin.
```
docker run -it --rm solosky/nrf52-sdk:latest
root@b10d54636088:/builds# git clone https://github.com/solosky/pixl.js
root@b10d54636088:/builds# cd pixl.js
root@b10d54636088:/builds/pixl.js# git submodule update --init --recursive
root@b10d54636088:/builds/pixl.js# cd fw && make all
root@b10d54636088:/builds/pixl.js/fw# cd application && make full ota
```
El firmware compilado se encuentra en la ruta  fw/_build/pixjs_all.hex, y la versión OTA (Paquete de actualización vía bluetooth) se encuentra en la ruta fw/_build/pixjs_ota_vXXXX.zip

## Programación del firmware en el dispositivo

Descarge la versión más reciente del archivo zip con el firmware, utilice un dispositivo JLink o DAPLink para programar el archivo «pixjs_all.hex» en el dispositivo.
Luego de haber programado el dispositivo por primera vez, las siguientes programaciones de firmware pueden ser realizadas vía actualización inalambríca (OTA).
También se puede utilizar el programa OpenOCD para programar el firmware:
```
openocd -f interface/cmsis-dap.cfg -c "transport select swd" -f target/nrf52.cfg -d2 -c init -c "reset init" -c halt -c "nrf5 mass_erase" -c "program pixjs_all. hex verify" -c "program nrf52832_xxaa.hex verify" -c exit
```

## Actualizaciones inalámbricas (OTA)
Descarge la versión más reciente del archivo zip con el firmware y extraígalo a una carpeta.
Para realizar una actualización OTA, en la interface de usuario del dispositivo seleccione la opción «Actualizar Firmware» del menú de Configuraciones, luego visite el sitio web [actualización firmware](https://thegecko.github.io/web-bluetooth-dfu/examples/web.html). Conéctese al dispositivo Pixl.js, selecciones el archivo «pixljs_ota_xxx» de la carpeta donde extrajo el archivo .zip.


## Carga de Archivos

Actualmente, se soporta subir archivos al dispositivo, utilizando la página web o el aplicativo.
Para la transferencia de archivos utilizando la página web, puede utilizar alguno de los siguientes sitios:

* Sitio principal [https://pixl.amiibo.xyz/](https://pixl.amiibo.xyz/)
* Sitio espejo [https://solosky.github.io/pixl.js/](https://solosky.github.io/pixl.js/)

Se soporta la aplicación de la tienda iOS llamada iNFC que ha sido desarrollada por un tercero.
Puedes buscar el aplicativo iNFC en la tienda App Store, o utilizar el siguiente código QR para descargarla:

![image](https://github.com/solosky/pixl.js/blob/main/assets/iNFC.jpg)

Actualmente, el aplicativo iNFC para MAC y Andriod se está desarrollando, ¡así que estáte atento!

[Vídeo tutorial de como subir un Amiibo al dispositivo Pixl.js utilizando iNFC](https://www.bilibili.com/video/BV1RV4y1f7bn/)

¡Mil gracias al autor del aplicativo iNFC por dar soporte al dispositivo Pixl.js!


## Protocolo Bluetooth

[fw/docs/ble_protocol.md](fw/docs/ble_protocol.md)

## Creación de Animaciones

[fw/docs/video_player.md](fw/docs/video_player.md)


## Creditos

* [FlipperZero Firmware](https://github.com/flipperdevices/flipperzero-firmware)
* [mlib](https://github.com/P-p-H-d/mlib)
* [TLSF](https://github.com/mattconte/tlsf)
* [cwalk](https://github.com/likle/cwalk)
* [SPIFFS](https://github.com/pellepl/spiffs)

## Contribuciones

* Especial agradecimiento a @Caleeeeeeeeeeeee por su perfecto Bootloader.
* Especial agradecimiento a @白成 (Mr. Baicheng) por crear al carcaza.


## Descargo de responsabilidad

Este proyecto es de código abierto (Open Source) y fue concebido para propósitos de investigación y aprendizaje.  Por favor no lo utilice para fines comerciales.
Amiibo es una marca registrada de Nintento.
NTAG21X es una marca registrada de NXP.

La base de datos de Amiibo incluída ha sido extraída de:
* [amiiloop](https://download.amiloop.app/)
* [AmiiboAPI](https://www.amiiboapi.com/)

El código fuente no contiene material o recursos protegidos por derechos de autor de Nintento, como por ejemplo, llaves de encriptación, datos originales de Amiibos, etc.

# Licencia

El presente proyecto se ha liberado bajo la licencia GPL 2.0.  Por favor cumpla con todos los términos de la licencia si utiliza el código.

* Si se realizan modificaciones al proyecto, el código modificado tiene que ser publicado.
* El código modificado debe ser publicado utilizando la misma licencia.


## Consejo

La característica que permite ilimitadas lecturas por parte de los juegos, requiere que el archivo «key_retail.bin» sea cargado a la carpeta raíz del dispositivo antes de poder se activada.  Se requiere que usted provea una copia legalmente adquirida del archivo, puede utilizar alguna herramienta para extraerla de su consola 3DS o Switch

>**sumas de verificación del archivo key_retail.bin:**
>MD5:	45fd53569f5765eef9c337bd5172f937
>
>SHA1:	bbdbb49a917d14f7a997d327ba40d40c39e606ce