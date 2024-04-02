# Database Amiibo
Questa applicazione permette l'emulazione di amiibo dalla lista di quelli ben noti, utilizzando informazioni legalmente disponibili come le informazioni sul modello, poi quando ne selezioni uno, viene creato un nuovo amiibo virtuale in memoria utilizzando un UUID casuale.  Per utilizzare questa applicazione devi fornire il tuo file «key_retail.bin».

_Ogni volta che si usa un amiibo l'UUID iniziale viene generato casualmente._

Una volta aperta l'applicazione il menu principale ha le opzioni

|   |
| ------------ |
| Browser…<BR>Ricerca…<BR>I miei preferiti…<BR>I miei Amiibo…<BR>Impostazioni… |
|   |

## Browser… 
Ti permette di sfogliare il database, prima per categoria, poi tutti gli amiibo di quella categoria.
Dopo aver selezionato un amiibo, vengono mostrati i dettagli, la prima riga contiene l'UUID attuale e la seconda riga il nome dell'amiibo.

Nella pagina dei dettagli dell'amiibo puoi usare i pulsanti laterali per navigare tra tutti gli amiibo della categoria, se premi il pulsante centrale puoi cambiare il comportamento dell'amiibo corrente, con il sottomenu:

|   |
| ------------ |
| Rand. Tag<BR>Auto Rand.<BR>Preferito…<BR>Salva come…<BR>[Torna ai dettagli]<BR>[Torna alla lista]<BR>[Esci] |
|   |

### Rand. Tag
Randomizza Tag, cambia l'UUID corrente presentato al gioco per l'amiibo corrente. Il nuovo UUID rimane invariato fino a quando non cambi l'amiibo corrente o selezioni nuovamente questa opzione.
### Auto Rand.
Random automatico, Attivando questa funzione, verrà generato un nuovo UUID casuale per l'amiibo corrente ogni volta che un gioco lo legge.  Consentendo di utilizzare lo stesso amiibo più volte su giochi con restrizioni.
### Preferito…
Apri un nuovo sottomenu per gestire le cartelle dei preferiti.  Hai le opzioni:

```
Nuovo…
Cartella1
Cartella2
[Indietro]
```

**Nuovo…** ti permette di creare una nuova cartella dei preferiti, **Indietro** torna ai dettagli dell'amiibo corrente, se selezioni una cartella esistente, quella definizione di amiibo verrà aggiunta alla cartella selezionata.

### Salva come…
Ti permette di assegnare l'amiibo corrente a uno degli slot «I miei Amiibo».
### [Torna ai dettagli]
Ritorna ai dettagli dell'amiibo corrente.
### [Torna alla lista]
Ritorna alla lista degli amiibo della categoria corrente.
### [Esci]
Esci dall'applicazione Database Amiibo.


## Ricerca…
Permette di cercare nel database usando un nome parziale, il risultato della ricerca è come una categoria, puoi selezionare qualsiasi amiibo su di essa e impostarlo come un altro.

## I miei preferiti…
Mostra le cartelle dei preferiti esistenti, puoi sfogliarle con i pulsanti laterali e selezionarne una con il pulsante centrale.  Se premi e tieni premuto il pulsante centrale si apre un sottomenu:

|   |
| ------------ |
| Nuovo…<BR>Svuota…<BR>Elimina…<BR>[Indietro] |
|   |

### Nuovo…
Ti permette di creare una nuova cartella dei preferiti.
### Svuota…
Dopo la conferma, rimuove tutti gli amiibo associati alla cartella selezionata.
### Elimina…
Dopo la conferma, rimuove la cartella dei preferiti.
### [Indietro]
Ritorna alla lista delle cartelle.

Una volta selezionata una cartella viene mostrata una lista degli amiibo associati a quella cartella dei preferiti, puoi sfogliare usando i pulsanti laterali e selezionare un amiibo con il pulsante centrale, l'amiibo selezionato diventa l'attuale e si comporta come un amiibo del browser.

## I miei Amiibo…
Elenca gli slot configurati, puoi sfogliare gli slot usando i pulsanti laterali, selezionarne uno con il pulsante centrale e l'amiibo associato a quello slot diventa l'attuale.

Se premi e tieni premuto il pulsante centrale si apre un sottomenu con l'opzione di reimpostare lo slot permettendo di svuotare lo slot.

## Impostazioni…
Il menu delle impostazioni ti mostra lo stato delle chiavi e il numero di slot impostato.

|   |
| ------------ |
| Chiavi []<BR>Num. Slot []<BR>[Indietro] |
|   |

### Chiavi []
Mostra lo stato del file «key_retail.bin», può essere «Caricato» o «NON caricato»
### Num. Slot []
Permette di selezionare quanti slot sono disponibili per la sezione «I miei Amiibo», valori da 10 a 50 con incrementi di decine.
### [Indietro]
Esci dal Database Amiibo e torna al Menu Principale.

----
# AmiiboLink
Questa applicazione emula un dispositivo AmiiboLink (AmiLoop o omllbolink).

Questi dispositivi sono schede espruino generiche vendute in diverse forme, normalmente sono portachiavi a forma di goccia, ma esistono anche alcuni Puck o Sheika Slate.

Quasi tutti hanno un pulsante e alcuni LED come indicatori e funzionano con batterie a cella CR2032 o celle lipo.

Tutti sono controllati da applicazioni per telefoni tramite Bluetooth, inoltre puoi caricare uno o più file .BIN amiibo utilizzando il telefono sul dispositivo.

Avviando questa applicazione vengono mostrati i dettagli per l'amiibo corrente.  Se sei in modalità sequenziale puoi usare i pulsanti laterali per scorrere tutti gli amiibo caricati, in tutte le modalità, puoi premere il pulsante centrale per aprire il sottomenu di configurazione dell'APP AmiiboLink, il menu ha queste voci:

|   |
| ------------ |
| Modalità []<BR>Auto Rand. []<BR>Modalità Compati. []<BR>Dettagli Tag<BR>[Menu Principale] |
|   |

## Modalità []
Mostra la modalità di funzionamento attuale, quando selezionata puoi scegliere la modalità di funzionamento dell'amiibo, le modalità disponibili sono:
- ### Randomizza (Manuale)
Sul dispositivo AmiiboLink attuale l'amiibo corrente cambia l'UUID quando premi il pulsante del dispositivo
- ### Randomizza (Auto)
Un nuovo UUID casuale per l'amiibo corrente viene generato ogni volta che un gioco lo legge.  Consentendo di utilizzare lo stesso amiibo più volte su giochi con restrizioni. Non puoi cambiare l'amiibo attivo in questa modalità.
- ### Modalità sequenziale
In questa modalità puoi cambiare l'amiibo attuale usando i pulsanti laterali, scorrendo tra tutti gli amiibo caricati con l'app del telefono. Tutti i file .BIN caricati sono memorizzati nei file 01.bin a 26.bin nella cartella /amiibolink/.
- ### Modalità lettura-scrittura
Il dispositivo utilizza uno slot amiibo "speciale" dove può essere utilizzato solo un amiibo, l'amiibo può essere sostituito da un altro file .BIN dall'app del telefono. Nella memoria locale questo amiibo viene salvato con il file /amiibolink/99.bin.

Cambiando la modalità si conserva l'amiibo selezionato corrente, con l'eccezione della modalità «Lettura-scrittura».

## Auto Rand. []
Quando questa opzione è attiva, viene generato un nuovo UUID casuale per l'amiibo corrente ogni volta che un gioco lo legge.  Consentendo di utilizzare lo stesso amiibo più volte su giochi con restrizioni. Non puoi cambiare l'amiibo attivo in questa modalità.  Non può essere disattivato per la modalità «Randomizza (Auto)», non può essere attivato per la modalità «Randomizza (Manuale)»; 
## Modalità Compati. []
Mostra la modalità di emulazione del firmware, i valori supportati sono V1, V2 e AmiLoop, ci sono diverse APP, ognuna progettata per una versione del firmware AmiiboLink o per il protocollo AmiLoop.
