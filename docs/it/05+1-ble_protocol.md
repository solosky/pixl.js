# Parametri Bluetooth

Attualmente, si utilizza l'SDK NRF52 per fornire il servizio UART Nordic, che fornisce una porta seriale virtuale BLE per comunicare con l'host.

Nome del dispositivo: Pixl.js

Lista dei servizi:

1. Servizio UART Nordic

NUS_SERVICE_UUID: 6e400001-b5a3-f393-e0a9-e50e24dcca9e

Sono fornite due caratteristiche:

* NUS_CHAR_TX_UUID: 6e400002-b5a3-f393-e0a9-e50e24dcca9e => Fornisce la trasmissione dei dati, supporta la scrittura e la scrittura senza ritorno.
* NUS_CHAR_RX_UUID: 6e400003-b5a3-f393-e0a9-e50e24dcca9e => Fornisce la lettura dei dati e supporta solo le notifiche.

(La versione ufficiale dei descrittori sopra può essere modificata.)

# Formato del protocollo

## Definizione del protocollo

Considerando l'efficienza, il protocollo BLE è un protocollo binario.
Questo protocollo è un protocollo di richiesta risposta, e tutte le richieste devono essere iniziate dal client.
Per semplificare l'implementazione del protocollo, ci sarà solo un pacchetto di risposta per ogni richiesta.

### Definizione della struttura del pacchetto

Per semplificare il protocollo, sia le richieste che le risposte utilizzano la stessa struttura del pacchetto.

La struttura generale del pacchetto è la seguente

| Nome del campo | Tipo | Lunghezza (byte) | Descrizione |
| ---- | ----- |---- | ---- |
| cmd | uint8 | 1 | tipo di comando |
| status | uint8 | 1 | Codice di stato, vedi la definizione del codice di stato sotto, il pacchetto di richiesta è fisso a 0 |
| chunk | uint16 | 2 | numero di sequenza del frammento |
| data | byte | N | Dati, relativi alla definizione specifica del pacchetto |

### Modalità di trasferimento del chunk

L'MTU attuale definito dal dispositivo pixl.js è 250. In una trasmissione di dati ATT, possono essere trasmessi fino a 247 byte di dati.

Dimensione massima del pacchetto:
L'intestazione del pacchetto fisso è di 4 byte, quindi ogni pacchetto può trasmettere fino a 243 byte. Se i dati trasmessi superano i 243 byte, è necessario trasmetterli a blocchi.

Quando la richiesta trasmessa o il risultato di ritorno supera l'MTU, è necessario trasmetterlo a chunk, e il valore del chunk aumenta di 1 per ogni trasmissione.
Quando il bit più alto del chunk è 1, significa che i dati sono ancora in fase di invio. Quando è 0, significa che i dati sono stati letti.

La scrittura e la lettura dei file richiedono l'abilitazione del trasferimento a chunk.
Il campo chunk è di 2 byte, e il flusso di trasporto a chunk può trasmettere fino a 7,8M byte.
Se supera questi 7,8M byte, è necessario aprire un nuovo flusso di trasmissione a chunk.

Ad esempio, processo di scrittura su un file:

```
c -> s: 0x12, modalità: wc, apri file
s -> c: 0x12, stato 0, id file 0xa1, il file è stato creato e può essere scritto
c -> s: 0x15, chunk 0 | 0x8000, id file 0xa1, 246 byte di dati
c -> s: 0x15, chunk 1 | 0x8000, id file 0xa1, 246 byte di dati
...
c -> s: 0x15, chunk N, id file 0xa1, 56 byte di dati
s -> c: 0x15, stato 0, scrittura completata
c -> s: 0x13, id file 0xa1, chiudi il file,
s -> c: 0x13, stato 0,
```

Processo di lettura del file

```
c -> s: 0x12, modalità: r, apri file
s -> c: 0x12, stato 0, id file 0xa1, il file è aperto e può essere letto
c -> s: 0x14, chunk 0, id file 0xa1
s -> c: 0x14, stato 0, chunk 1 | 0x8000, id file 0xa1, 246 byte di dati
...
s -> c: 0x14, stato 0, chunk N, id file 0xa1, 56 byte di dati
c -> s: 0x13, id file 0xa1, chiudi il file,
s -> c: 0x13, stato 0,
```

### Definizione del percorso

Le operazioni sui file richiedono il trasferimento del percorso completo del file. Espressione del percorso standard di Windows con lettera di unità.
Ad esempio, E:/amiibo/mifa.bin.

### tipo 

I tipi numerici sono tutti interi senza segno, modalità little endian.
La codifica della stringa è lunga 2 byte + array di stringhe, e le stringhe sono codificate uniformemente utilizzando utf8.

### Informazioni meta del file
Le informazioni meta del file vengono utilizzate per memorizzare alcuni dati relativi al file, con una lunghezza massima di 128 byte. Si distingue in base al campo tipo. I dati sono di lunghezza variabile e i dati vengono analizzati in base al tipo.

Il formato di base è il seguente:

| Nome del campo | Tipo | Lunghezza (byte) | Descrizione |
| ---- | ----- |---- | ---- |
| dimensione meta | uint8 | 1 | La dimensione di tutte le informazioni meta, 0xFF o 0 senza metadati |
| tipo meta N | uint8 | 1 | Tipo di informazione meta, definito nella sezione seguente |
| dati meta N | byte | N | La lunghezza varia in base al tipo |

I tipi attualmente definiti possono assumere i seguenti valori:

1: annotazioni amiibo
| Nome del campo | Tipo | Lunghezza (byte) | Descrizione |
| ---- | ----- |---- | ---- |
| lunghezza nota | uint8 | 1 | lunghezza nota |
| dati nota | byte | N | Stringa di note, massimo 90 byte, codifica UTF8. |

2: Proprietà del file
| Nome del campo | Tipo | Lunghezza (byte) | Descrizione |
| ---- | ----- |---- | ---- |
| flag | uint8 | N | Attributi del file, definiti a bit |

* 0b0000_0001: Il file è impostato come attributo nascosto e non verrà visualizzato sulla pagina amiibo, ma può essere visualizzato sulla pagina web. settings.bin imposterà questo flag per impostazione predefinita.
* 0bxxxx_xxxx: Altri bit sono temporaneamente non definiti.

3: attributi amiibo
| Nome del campo | Tipo | Lunghezza (byte) | Descrizione |
| ---- | ----- |---- | ---- |
| testa | uint32 | N | id testa amiibo |
| coda | uint32 | N | id coda amiibo |

Nota:
* Questo attributo è attualmente utilizzato solo in amiibo/data/*.bin ed è utilizzato per identificare rapidamente se si tratta di un file amiibo.



### Definizione del codice di stato globale

| Codice di stato | Descrizione |
| ---- | ---- |
| 0 | La richiesta è stata completata con successo |
| 0xFF | Il firmware non supporta l'attuale messaggio |

TODO Integrare il codice di errore in dettaglio. 



#Definizione del protocollo

## 0x01: Ottieni informazioni sulla versione

1. Il client invia una richiesta

| Nome del campo | Tipo | Lunghezza (byte) | Descrizione |
| ---- | ----- |---- | ---- |
| cmd | uint8 | 1 | 0x01 |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 | 0 |


2. Il server risponde alla richiesta

| Nome del campo | Tipo | Lunghezza (byte) | Descrizione |
| ---- | ----- |---- | ---- |
| cmd | uint8 | 1 | 0x01 |
| status | uint8 | 1 | Codice di stato, vedi descrizione del codice di stato |
| chunk | uint16 | 2 | 0 |
| lunghezza versione | uint16 | 2 | lunghezza stringa versione |
| versione | byte | N | versione |

## 0x02: Entra in modalità DFU

1. Il client invia una richiesta

| Nome del campo | Tipo | Lunghezza (byte) | Descrizione |
| ---- | ----- |---- | ---- |
| cmd | uint8 | 1 | 0x02 |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 | 0 |

2. Il server risponde alla richiesta

| Nome del campo | Tipo | Lunghezza (byte) | Descrizione |
| ---- | ----- |---- | ---- |
| cmd | uint8 | 1 | 0x102 |
| status | uint8 | 1 | Codice di stato, vedi descrizione del codice di stato |
| chunk | uint16 | 2 | 0 |

## 0x10: Ottieni lista dischi

1. Il client invia una richiesta

| Nome del campo | Tipo | Lunghezza (byte) | Descrizione |
| ---- | ----- |---- | ---- |
| cmd | uint8 | 1 | 0x10 |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 | 0 |

2. Il server risponde alla richiesta

| Nome del campo | Tipo | Lunghezza (byte) | Descrizione |
| ---- | ----- |---- | ---- |
| cmd | uint8 | 1 | 0x10 |
| status | uint8 | 1 | Codice di stato, vedi descrizione del codice di stato |
| chunk | uint16 | 2 | 0 |
| conteggio unità | uint8 | 1 | Numero di dischi |
| codice di stato unità N | uint8 | 1 | Codice di stato disco N 0: Il disco è disponibile, non-0 disco non è disponibile |
| etichetta unità N |char | 1| lettera di unità disco |
| lunghezza nome unità N | uint16 | 2 | lunghezza nome disco |
| nome unità N | byte | N | stringa nome disco |
| dimensione totale unità N |uint32 | 4 | Spazio totale su disco |
| dimensione utilizzata unità N | uint32 | 4 | spazio libero su disco |

## 0x11: Formatta disco

1. Il client invia una richiesta

| Nome del campo | Tipo | Lunghezza (byte) | Descrizione |
| ---- | ----- |---- | ---- |
| cmd | uint8 | 1 | 0x11 |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 | 0 |
| etichetta unità | char | 1 | Lettera di unità disco: E/I |

## 0x12: Apri file

1. Il client invia una richiesta

| Nome del campo | Tipo | Lunghezza (byte) | Descrizione |
| ---- | ----- |---- | ---- |
| cmd | uint8 | 1 | 0x12 |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 | 0 |
| lunghezza percorso | uint16 | 2 | lunghezza percorso |
| percorso | byte | N | stringa percorso |
| modalità | uint8 | 1 | modalità di apertura |

2. Il server risponde alla richiesta

| Nome del campo | Tipo | Lunghezza (byte) | Descrizione |
| ---- | ----- |---- | ---- |
| cmd | uint8 | 1 | fisso 0x12 |
| status | uint8 | 1 | Codice di stato, vedi descrizione del codice di stato |
| chunk | uint16 | 2 | 0 |
| id file | uint8 | 1 | id file, utilizzato per le successive richieste di lettura e scrittura |

## 0x13: Chiudi file

1. Il client invia una richiesta

| Nome del campo | Tipo | Lunghezza (byte) | Descrizione |
| ---- | ----- |---- | ---- |
| cmd | uint8 | 1 | 0x13 |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 | 0 | 
| id file | uint8 | 1 | id file |

2. Risposta del servizio

| Nome del campo | Tipo | Lunghezza (byte) | Descrizione |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x13  |
| status | uint8 | 1 | Codice di stato, vedi descrizione del codice di stato |
| chunk | uint16 | 2 |  0 | 
| id file | uint8 | 1 | id file, utilizzato per le successive richieste di lettura e scrittura |

## 0x14: Leggi file

1. Il client invia una richiesta

Supporta solo la lettura sequenziale.

| Nome del campo | Tipo | Lunghezza (byte) | Descrizione |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x14  |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 |  0 | 
| id file | uint8 | 1| id file |

2. Il server risponde alla richiesta

| Nome del campo | Tipo | Lunghezza (byte) | Descrizione |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x14  |
| status | uint8 | 1 |Codice di stato, vedi descrizione del codice di stato |
| chunk | uint16 | 2 |  Abilita trasferimento a chunk | 
| data  | byte | N | Dati del file |

## 0x15 Scrivi su file

1. Il client invia una richiesta

| Nome del campo | Tipo | Lunghezza (byte) | Descrizione |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x15  |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 | Abilita trasferimento a chunk | 
| id file | uint8 | 1 | id file | 
| data | byte | N | Dati del file, lunghezza massima = MTU - 4 |

2. Il server risponde alla richiesta

| Nome del campo | Tipo | Lunghezza (byte) | Descrizione |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x15  |
| status | uint8 | 1 |Codice di stato, vedi descrizione del codice di stato |
| chunk | uint16 | 2 |  Stesso della richiesta | 

## 0x16：Leggi cartella

1. Il client invia una richiesta

| Nome del campo | Tipo | Lunghezza (byte) | Descrizione |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x16  |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 |  0 | 
| lunghezza percorso | uint16 | 2 | Lunghezza percorso in byte |
| percorso  | byte | N | Stringa percorso |

2. Il server risponde alla richiesta

| Nome del campo | Tipo | Lunghezza (byte) | Descrizione |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x16  |
| status | uint8 | 1 |Codice di stato, vedi descrizione del codice di stato |
| chunk | uint16 | 2 |  Abilita trasferimento a chunk | 
| lunghezza nome file N | uint16 | 2 | Lunghezza nome file in byte|
| nome file N | byte | N| Stringa nome file |
| dimensione file N | uint32 | 4 |Dimensione file |
| tipo file N | uint8 | 1 | Tipo file： 0 => File, 1 => Cartella |
| lunghezza meta file N | uint8 | 1 | Lunghezza dati meta file, 64 max|
| meta file N | byte | N | Dati meta file |

## 0x17 Crea cartella

1. Il client invia una richiesta

| Nome del campo | Tipo | Lunghezza (byte) | Descrizione |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x17  |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 | 0 | 
| lunghezza percorso | uint16 | 2 | Lunghezza percorso in byte |
| percorso  | byte | N | Stringa percorso |

2. Il server risponde alla richiesta

| Nome del campo | Tipo | Lunghezza (byte) | Descrizione |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x17  |
| status | uint8 | 1 |Codice di stato, vedi descrizione del codice di stato |
| chunk | uint16 | 2 |  0 | 

## 0x18 Elimina file o cartella

1. Il client invia una richiesta

| Nome del campo | Tipo | Lunghezza (byte) | Descrizione |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x18  |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 | 0 | 
| lunghezza percorso | uint16 | 2 | Lunghezza percorso in byte |
| percorso  | byte | N | Stringa percorso |

2. Il server risponde alla richiesta

| Nome del campo | Tipo | Lunghezza (byte) | Descrizione |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x18  |
| status | uint8 | 1 |Codice di stato, vedi descrizione del codice di stato |
| chunk | uint16 | 2 |  0 | 

## 0x19 Rinomina file o cartella

1. Il client invia una richiesta

| Nome del campo | Tipo | Lunghezza (byte) | Descrizione |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x18  |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 | Abilita trasferimento a chunk | 
| vecchia lunghezza percorso | uint16 | 2 | Lunghezza vecchio percorso in byte |
| vecchio percorso  | byte | N | Vecchia stringa percorso |
| nuova lunghezza percorso | uint16 | 2 | Lunghezza nuovo percorso in byte |
| nuovo percorso  | byte | N | Nuova stringa percorso |

Se la lunghezza dei dati è maggiore dell'MTU, utilizza il trasferimento a chunk.

2. Il server risponde alla richiesta

| Nome del campo | Tipo | Lunghezza (byte) | Descrizione |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x18  |
| status | uint8 | 1 |Codice di stato, vedi descrizione del codice di stato |
| chunk | uint16 | 2 |  0 | 

## 0x1A Aggiorna meta file

1. Il client invia una richiesta

| Nome del campo | Tipo | Lunghezza (byte) | Descrizione |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x1a  |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 | 0 | 
| lunghezza percorso | uint16 | 2 | Lunghezza percorso in byte |
| percorso  | byte | N | Stringa percorso |
| dati meta | byte | N | Dati Meta File | 

2. Il server risponde alla richiesta

| Nome del campo | Tipo | Lunghezza (byte) | Descrizione |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x1a  |
| status | uint8 | 1 |Codice di stato, vedi descrizione del codice di stato |
| chunk | uint16 | 2 |  0 |