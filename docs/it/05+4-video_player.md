Certo, ecco la traduzione in italiano:

# Lettore di animazioni

Il lettore di animazioni può riprodurre file di animazione convertiti in formati personalizzati. Richiede la versione del firmware 2.3.0 o superiore.

## Formato del file di animazione

Il formato di base del file è intestazione + frame1 + frame2 +... + frameN

     # magic + frame_count + fps + width + height + reserved
     # 4b + 2b + 2b + 2b + 2b + 4b = 16 byte

Il formato dell'intestazione è il seguente

| Nome del campo | Tipo | Lunghezza (byte) | Descrizione |
| ---- | ----- |---- | ---- |
| magic | bytes | 4 | Prefisso fisso, utilizzato per determinare rapidamente se è un file di animazione, fisso a 0x56 0x58 0x49 0x50 |
| conteggio frame | uint16 | 2 | Quanti frame di animazione ci sono in totale |
| fps | uint16 | 2 | Quanti frame di animazione al secondo |
| larghezza | uint16 | 2 | larghezza dell'animazione |
| altezza | uint16 | 2 | altezza dell'animazione |
| riservato | uint32 | 4 | byte riservati |

Il formato dei dati del frame N è i dati di animazione di ogni frame in formato xbm, e la dimensione è larghezza * altezza / 8.
Se la lunghezza della larghezza non è un multiplo di 8, arrotonda per eccesso.

## Come generare file di animazione

Prima prepara il file video che deve essere convertito, di solito termina in mp4, ma sono accettabili anche altri formati.

Le seguenti operazioni devono essere eseguite sotto Linux.
Inoltre, puoi utilizzare WSL2 per creare rapidamente un ambiente Linux Ubuntu sotto Windows.

Esegui prima il seguente comando per installare gli strumenti necessari

```
sudo apt install ffmpeg python3-pip
sudo pip3 install imageio -i https://pypi.tuna.tsinghua.edu.cn/simple
```

Per Ubuntu avrai bisogno di installare i pacchetti eseguendo il prossimo comando:

```
sudo apt install ffmpeg python3 python3-imageio
```

Se preferisci l'ambiente MSYS MINGW64 puoi installare i pacchetti utilizzando questi comandi:

```
pacman -S mingw-w64-x86_64-ffmpeg mingw-w64-x86_64-python mingw-w64-x86_64-python-imageio
```

Quindi esegui il comando ffmpeg per convertire l'animazione in immagini di frame indipendenti.
```
mkdir frames
ffmpeg -y -i badapple.mp4 -ss 0:0 -t 60 -vf "scale=128:64:decrease:flags=lanczos,hue=s=0" -r 10 "frames/%04d.bmp"
```
La descrizione del parametro è la seguente:

* -i badapple.mp4: Questo è il file video di input
* -ss 0:0: intercetta da 0:0 del video. Se non vuoi intercettare parte del video, non hai bisogno di fornire questo parametro.
* -t 60: intercetta 60 secondi di video. Se converti l'intero video, non hai bisogno di fornire questo parametro.
* -vf "scale=128:64:decrease:flags=lanczos,hue=s=0" : Converti il video in un'immagine 128x64, mantenendo l'aspect ratio del video
* -r 10: L'fps convertito è 10
* "frames/%04d.bmp": I file video convertiti sono memorizzati nella directory frames e il formato è bmp

Se il video è più grande, puoi ridurre le dimensioni del video come segue:

* Dato i parametri -ss -t, intercetta parte del video
* Riduci la risoluzione di output, come 80x64, ecc.

Infine, esegui il seguente comando per convertirlo in un file di animazione personalizzato

```
python3 fw/scripts/video_clip_gen.py 10 frames badapple.bin
```
La descrizione del parametro è la seguente:

* 10: L'fps convertito è 10, ad esempio, è lo stesso dell'fps convertito da ffmpeg, altrimenti la velocità di riproduzione sarà errata.
* frames: directory delle immagini convertite da ffmpeg
* badapple.bin: file di animazione convertito

## Riproduci file di animazione

Trasferisci i file video convertiti nella directory del lettore dello storage Flash in Pixl.js tramite la pagina web. La directory del lettore non può essere creata manualmente.

Apri Pixl.js, seleziona [Animation Player], elenca tutti i file nella directory del lettore, quindi seleziona il file video che vuoi riprodurre e premi il pulsante centrale per riprodurre.

Avviso:

* Sebbene lo storage Flash sia uno spazio di 1,8 MB, a causa delle caratteristiche del file system SPIFFS, quando lo spazio disponibile è inferiore al 30%, la scrittura sarà molto lenta o addirittura fallirà.
* Se incontri una scrittura lenta, puoi ripristinare la velocità di caricamento formattando il disco.
* La cancellazione di file di grandi dimensioni è lenta, per favore sii paziente.