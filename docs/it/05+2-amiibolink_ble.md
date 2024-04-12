Certo, ecco la traduzione in italiano:

# Protocollo di trasferimento Amiibolink

## Parametri Bluetooth

L'attuale protocollo utilizza ancora il servizio NUS Nordic come protocollo di porta seriale Bluetooth.

Nome del dispositivo di trasmissione: amiibolink

Lista dei servizi:

1. Servizio UART Nordic

NUS_SERVICE_UUID: 6e400001-b5a3-f393-e0a9-e50e24dcca9e

Sono fornite due caratteristiche:

* NUS_CHAR_TX_UUID: 6e400002-b5a3-f393-e0a9-e50e24dcca9e => Fornisce la trasmissione dei dati, supporta la scrittura e la scrittura senza ritorno.
* NUS_CHAR_RX_UUID: 6e400003-b5a3-f393-e0a9-e50e24dcca9e => Fornisce la lettura dei dati e supporta solo le notifiche.

## Protocollo di trasferimento amiibo

## Descrizione del protocollo


| Funzione | Invio | Risposta | Descrizione |
|---------|--------------|------|--------------------------|
| Imposta modalità dispositivo | a1 b1 01 | b1 a1 | 01: Modalità casuale 02: Modalità sequenziale 03: Modalità di lettura e scrittura |
| Preparazione scrittura carta | a0 b0 | b0 a0 | Invia prima di inviare dati |
| ?? | ac ac 00 04 00 00 02 1c | ca ca | Invia 540 byte di dati |
| ?? | ab ab 02 1c | ba ba | Non so perché è stato inviato un pacchetto con lo stesso significato |
| Invia dati | dd aa 00 96 ... 00 01 | aa dd | 0x96 lunghezza dati, seguita da dati, gli ultimi due byte sono il numero di sequenza del pacchetto |
| ?? | bc bc | cb cb | fine trasferimento |
| ?? | cc dd | dd cc | Non so perché sto inviando questo pacchetto |


## Processo di interazione

### Processo di interazione del mini programma ufficiale

```
c->s: a0 b0
s->c: b0 a0
c->s: ac ac 00 04 00 00 02 1c //540
s->c: ca ca
c->s: ab ab 02 1c
s->c: ba ba
c->s:

0000 dd aa 00 96 (0x96 byte di dati) 00 00

dd aa 00
96 //150
04 51 91 4c e2 ..//150 byte
00 00 //seq, inizia da 0


c->s: aadd
s->c:
0000 dd aa 00 96 (0x96 byte di dati) 00 01
c->s: aadd
s->c:
0000 dd aa 00 96 (0x96 byte di dati) 00 02
c->s: aadd
s->c: dd aa 00 5a (0x5a byte dati) 00 03
s->c: aadd
c->s: bcbc
s->c: cbcb
s->c:ccdd
c->s: ddcc
```

### Processo di interazione iNFC
Il processo è sostanzialmente lo stesso di sopra, ma c'è un comando in più
```
c->s: a1 b1 01
s->c: b1 a1
c->s: a0b0
s->c: b0a0
...
```