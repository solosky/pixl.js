# Concetti del Firmware
## Amiibo
È un dispositivo venduto da Nintendo, normalmente sotto forma di statuetta, ma esistono anche carte, cinturini da polso e altri.

Ogni amiibo ha un tag NFC incorporato nella base della statuetta. Il tag NFC ha alcuni dati pre-registrati in fabbrica che identificano il tipo di statuetta e rendono ogni amiibo unico rispetto agli altri. I principali dati dell'amiibo sono:

Come affermato da Nintendo, Amiibo è singolare e plurale (https://twitter.com/NintendoAmerica/status/535485751635763200)

## Informazioni sul modello
È un numero che descrive la serie amiibo, la serie di giochi, il personaggio e la variazione del personaggio, queste informazioni non possono essere modificate dopo la prima programmazione.

## UID
È il numero di serie della statuetta, anch'esso preimpostato in fabbrica, rendendo ogni amiibo unico.

## Dati Amiibo
Questa sezione dei dati sul tag memorizza le informazioni modificabili dall'utente come la registrazione del proprietario e del soprannome o i dati del salvataggio del gioco.

## UUID
Il termine UUID viene utilizzato nella scena mod per fare riferimento all'UID inviato al gioco da un emulatore amiibo, poiché il gioco usa l'UID per sapere se un amiibo è già stato utilizzato; se l'emulatore cambia l'UID, l'amiibo emulato sembra un amiibo completamente diverso per il gioco, permettendo di utilizzare nuovamente quell'amiibo.

## key_retail.bin
È un file binario che contiene le chiavi private di Nintendo per criptare amiibo, è un materiale protetto da copyright quindi devi fornire una copia legalmente acquisita, puoi scaricarla da una console 3DS o Switch.

## File .BIN
I dati di un amiibo possono essere scaricati su un file, quel dump binario del contenuto originale di un amiibo viene memorizzato su un file con estensione `.BIN`, quei dump vengono caricati sul dispositivo per l'applicazione Amiibo Emulator o l'applicazione AmiiboLink Emulator. Devi fornire un file di dump binario legalmente acquisito.

## Interruttore a cursore Thumbwheel
È il microinterruttore in cima al dispositivo, è un dispositivo che ha tre pulsanti in uno, uno centrale e due laterali. Oscillando il microinterruttore ai lati si premono i pulsanti "**laterali**". Premendo il microinterruttore si preme il pulsante "**centrale**".

In questo documento, premere i lati del Thumbwheel viene chiamato "**pulsanti laterali**", premere il Thumbwheel viene chiamato "**pulsante centrale**"; per navigare tra gli elementi di un menu si usano i pulsanti laterali, per selezionare un elemento si preme il pulsante centrale.

# Utilizzo del Firmware
La schermata principale del firmware Pixl.js è divisa in diverse applicazioni, ognuna delle quali può essere eseguita per eseguire alcune azioni, le applicazioni nella schermata principale sono:


|   |
| ------------ |
|  Emulatore Amiibo<br> Database Amiibo<br>֍ AmiiboLink<br>ᛡᛒ  Trasferimento file BLE<br> Impostazioni |
|   |

----
# Emulatore Amiibo
Questa applicazione ti permette di sfogliare la memoria locale per i file .BIN e usarli come amiibo corrente.

Puoi sfogliare cartelle e file usando l'interruttore a cursore Thumbwheel, spingendo ai lati cambi gli elementi, premendo il pulsante centrale selezioni l'elemento evidenziato.

Una volta selezionato un file .BIN, i suoi dati vengono utilizzati come amiibo corrente, lo schermo mostra i dettagli dell'amiibo corrente come UUID corrente dell'amiibo, nome del file e nome dell'amiibo.

## Schermata dettagli amiibo
Puoi usare i pulsanti laterali per cambiare l'amiibo corrente con il successivo o il precedente nella cartella corrente. Premendo il pulsante centrale puoi cambiare il comportamento dell'amiibo corrente, nel sottomenu:

|   |
| ------------ |
| Rand. Tag<BR>Auto Rand.<BR>Elimina Tag<BR>Torna ai dettagli del Tag<BR>Torna alla lista dei file<BR>Torna al menu principale<BR>Rand. Tag |
|   |

### Rand. Tag
Randomizza Tag, cambia l'UUID corrente presentato al gioco per l'amiibo corrente. Il nuovo UUID rimane invariato fino a quando non cambi l'amiibo corrente o selezioni nuovamente questa opzione.
### Auto Rand. (Random automatico)
Attivando questa funzione, verrà generato un nuovo UUID casuale per l'amiibo corrente ogni volta che un gioco lo legge.  Consentendo di utilizzare lo stesso amiibo più volte su giochi con restrizioni.
### Elimina Tag
Elimina il file associato all'amiibo corrente.
### Torna ai dettagli del Tag
Ritorna ai dettagli dell'amiibo corrente.
### Torna alla lista dei file
Ritorna alla lista dei file della cartella corrente.
### Torna al menu principale
Esci dall'applicazione emulatore amiibo.

_Se abiliti la generazione di UUID casuali, manualmente o automaticamente l'UUID originale del file .BIN viene utilizzato la prossima volta che quel file viene caricato._

## Esploratore di file.
Se hai **evidenziato una cartella o un file** e tieni premuto il pulsante centrale per 1 secondo si apre un sottomenu delle proprietà:

|   |
| ------------ |
| Crea nuova cartella…<BR>Crea nuovo Tag…<BR>Rinomina…<BR>Elimina...<BR>Torna alla lista dei file<BR>Torna al menu principale |
|   |

### Crea nuova cartella…
Dialogo per creare una nuova sottocartella nella corrente.
### Crea nuovo Tag…
Crea un file .BIN amiibo vuoto nella cartella corrente.  Quel file .BIN è ora un simile di un New Ntag215, pronto per essere programmato, puoi assegnare quel file .BIN come amiibo corrente e usando un programmatore NFC puoi scrivere i dati dell'amiibo, ad esempio puoi usare iNFC o Tagmo per i telefoni, Thenaya per il 3DS.  Se l'amiibo programmato è nel database il file .BIN viene anche rinominato con il nome dell'amiibo.
### Rinomina…
Dialogo per rinominare il file .BIN o la cartella evidenziata
### Elimina…
Rimuove l'oggetto evidenziato, chiedendo conferma.  Se è una cartella, anche tutte le sottocartelle e i file vengono eliminati.
### Torna alla lista dei file
Ritorna alla lista dei file della cartella corrente.
### Torna al menu principale
Esci dall'applicazione emulatore amiibo.

Se hai **evidenziato una memoria** il sottomenu delle proprietà è diverso:

|   |
| ------------ |
| Stato della memoria<BR>Spazio totale<BR>Spazio libero<BR>Formatta…<BR>Torna alla lista<BR>Torna al menu principale |
|   |

### Stato della memoria
Mostra lo stato corrente e il tipo della memoria, ad esempio:
```
=====Non montato=====
===Montato[LFS]===
===Montato[FFS]===
```

### Spazio totale
Mostra lo spazio totale della memoria montata in KB.
### Spazio libero
Mostra lo spazio libero attuale della memoria montata in KB.
### Formatta…
Chiede conferma per formattare la memoria corrente, ATTENZIONE TUTTI I DATI VERRANNO PERSI!
### Torna alla lista
Ritorna alla lista dei file della cartella corrente.
### Torna al menu principale
Esci dall'applicazione emulatore amiibo.

----
# Database Amiibo
Questa applicazione permette l'emulazione di amiibo dalla lista di quelli ben noti, utilizzando informazioni legalmente disponibili come le informazioni sul modello, poi quando ne selezioni uno, viene creato un nuovo amiibo virtuale in memoria utilizzando un UUID casuale.  Per utilizzare questa applicazione devi fornire il tuo file «key_retail.bin».

Ogni volta che si usa un amiibo, l'UUID iniziale viene generato casualmente._

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
### Auto Rand. (Random automatico)
Attivando questa funzione, verrà generato un nuovo UUID casuale per l'amiibo corrente ogni volta che un gioco lo legge.  Consentendo di utilizzare lo stesso amiibo più volte su giochi con restrizioni.
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
Quando questa opzione è attiva, viene generato un nuovo UUID casuale per l'amiibo corrente ogni volta che un gioco lo legge, 
consentendo di utilizzare lo stesso amiibo più volte su giochi con restrizioni. Non puoi cambiare l'amiibo attivo in questa modalità.


### Modalità Compati. []
Mostra la modalità di emulazione del firmware, i valori supportati sono V1, V2 e AmiLoop, ci sono diverse APP, ognuna progettata per una versione del firmware AmiiboLink o per il protocollo AmiLoop.  Utilizzando questa impostazione puoi far corrispondere l'emulazione con il protocollo necessario per l'APP che usi sul tuo telefono.
## Dettagli Tag
Ritorna alla pagina dei dettagli del tag
## [Menu Principale]
Esci dall'applicazione Emulatore AmiiboLink.

## Applicazioni compatibili con AmiiboLink
Finora le uniche due App di gestione della modalità AmiiboLink/AmiLoop tradotte in inglese sono "Umiibo" e "AmiLoop", queste applicazioni sono compatibili solo con la Modalità AmiLoop.

Un'altra particolarità è che le modalità sulle applicazioni non corrispondono con le modalità impostate sul dispositivo.

| App Umiibo | Modalità di emulazione Pixl.js |
| ------------ |------------ |
| Modalità Manuale | Random automatico |
| Modalità File | Sequenziale |
| Modalità Auto | Lettura/Scrittura |

| App AmiiLoop | Modalità di emulazione Pixl.js |
| ------------ |------------ |
| Modalità Manuale | Random automatico |
| Modalità File | Sequenziale |
| Modalità Auto | Lettura/Scrittura |


# Emulatore di Carte

Questa applicazione può essere utilizzata per emulare carte Mifare e carte della serie NTAG.

Le carte Mifare sono comunemente utilizzate per le carte di controllo degli accessi, e il dispositivo può emulare completamente i tipi di carte Mifare. I tipi di carte Mifare supportati includono:

* Mifare Mini
* Mifare 1K
* Mifare 2K
* Mifare 4K

Le carte della serie NTAG sono comunemente utilizzate per l'identificazione del dispositivo. I tipi di carte NTAG supportati includono:

* NTAG 213
* NTAG 215
* NTAG 216

Attualmente, un totale di 8 carte sono supportate, con l'opzione per personalizzare il numero di carte in futuro.

> Nota:<br/>
> La funzione di emulazione della serie NTAG è ancora in fase di test, e la funzionalità non è ancora completa. Non emula completamente > le caratteristiche di NTAG125. <br/>
> Continueranno ulteriori miglioramenti a questa parte della funzionalità.

L'emulazione può essere attivata solo in questa interfaccia, e sarà disattivata se si lascia questa interfaccia.

Dopo la formattazione del disco o il primo ingresso nell'emulatore di carte, i dati verranno inizializzati. Questo processo può richiedere alcuni secondi, quindi si prega di essere pazienti.


## Emulazione di Carte

L'interfaccia della pagina di emulazione delle carte è la seguente:

|   |
| ------------ |
| [01] de:ad:be:ef|
|  |
| <    Slot 01      >|
|  |
| MF 1K <08/04 00>  |
|   |

L'interfaccia è spiegata come segue:

* La prima riga: `01` è il numero della carta, e `de:ad:be:ef` è l'ID della carta.
* La seconda riga: Slot 01 è il nome della carta corrente, che può essere liberamente impostato.
* La terza riga: `MF 1K` visualizza il tipo di carta. Le abbreviazioni del tipo sono mostrate nella tabella sottostante. `08` è il SAK della carta, `04 00` è l'ATQA della carta. L'ultimo simbolo indica la modalità di scrittura. Se è contrassegnato come un disco, la scrittura è consentita; altrimenti, la scrittura non è consentita.
Abbreviazioni per i tipi di carte:

| Nome visualizzato | Tipo di carta | Dimensione del file di dati |
| ---- | --- | --- |
| MF mini | Mifare Mini | 320 |
| MF 1k | Mifare 1K | 1024 |
| MF 2k | Mifare 2K | 2048 |
| MF 4k | Mifare 4K | 4096 |
| N213 | NTAG 213 | 180 |
| N215 | NTAG 215 | 540 |
| N216 | NTAG 216 | 924 |

# Menu Principale

Premendo il pulsante centrale si può entrare nel menu principale. Come segue:

|   |
| ------------ |
|  Slot  [01]|
|  Nick [Slot 01]|
|  ID [de:ad:be:ef]|
|  Tipo [MiFare 1K] |
|  Dati.. |
|  Avanzate.. |
|  Impostazioni Slot.. |
|  [Dettagli Tag] |
|  [Menu Principale] |
|   |

* Slot: Visualizza lo slot della carta selezionato corrente. Puoi selezionare uno slot della carta premendo il pulsante centrale.
* Nick: Il nome corrente della carta. Premendo il pulsante centrale si può entrare nell'interfaccia di impostazione del nome della carta.
* ID: Visualizza l'ID corrente della carta.
* Tipo: Visualizza il tipo corrente della carta.
* Dati: Premendo il pulsante centrale si possono gestire i dati della carta.
* Avanzate: Premendo il pulsante centrale si possono impostare le impostazioni avanzate della carta.
* Impostazioni Slot: Premendo il pulsante centrale si entra nell'interfaccia di gestione dello slot, dove si possono abilitare o disabilitare gli slot delle carte.
* [Dettagli Tag]: Ritorna alla pagina dei dettagli del tag.
* [Menu Principale]: Esce da questa applicazione e entra nel menu principale.

> **Nota speciale:**：<br/>
> Alcune modifiche devono essere salvate nella memoria quando si esce dai dettagli del tag. Se hai modificato alcune configurazioni, assicurati di entrare nella pagina dei dettagli del tag per salvarle.

## Aggiornamento Nick

In questa interfaccia, è possibile modificare il nome della carta.

A causa dello spazio di visualizzazione limitato, è possibile inserire solo caratteri inglesi.

In particolare, se si utilizzano caratteri cinesi, a causa della codifica UTF-8, l'eliminazione richiede almeno 2 pressioni per eliminare completamente.

Se si desidera aggiungere note cinesi alla carta, è possibile modificare le note di `/chameleon/slots/00.bin` attraverso la pagina web per ottenere note cinesi.<br />
00.bin è il file per il primo slot della carta, 01.bin è il file per il secondo slot della carta, e così via.

> A causa delle limitazioni di dimensione del firmware, attualmente solo circa 1000 caratteri cinesi possono essere visualizzati correttamente. Se i caratteri cinesi non vengono visualizzati, si prega di segnalare un problema con i caratteri che è necessario visualizzare. I caratteri specifici possono essere aggiunti nella prossima versione.

## Dati della Carta

In questa interfaccia, è possibile eseguire l'importazione e l'esportazione dei dati della carta.

|   |
| ------------ |
|  Carica.. |
|  Salva.. |
|  Factory.. |
|   |

I file di importazione ed esportazione sono memorizzati nella cartella `/chameleon/dump/`.<br />
Se è necessario importare dati, è necessario scrivere in anticipo il file di dati che si desidera importare nella cartella sopra tramite la pagina web.

* Carica: Premendo il pulsante centrale si può entrare nell'interfaccia di caricamento. L'interfaccia leggerà tutti i file sotto `/chameleon/dump/`, e premendo il pulsante centrale si può eseguire l'importazione.
* Salva: Premendo il pulsante centrale si può esportare la carta corrente nella cartella `/chameleon/dump/`.
* Factory: Premendo il pulsante centrale si può reimpostare i dati della carta corrente ai dati della carta vuota predefiniti incorporati.

## Avanzate

Questa interfaccia mostra contenuti diversi in base alle carte della serie MiFare e alle carte della serie NTAG.

> **Nota speciale:**<br/>
> Questa interfaccia è le impostazioni avanzate per l'emulazione della carta, e non è consigliato per gli utenti ordinari modificare. Se intendi apportare modifiche, assicurati di capire cosa stai facendo prima di modificare!<br/>
> Se la modifica di questa parte porta a un fallimento dell'emulazione della carta, prova a ripristinare le impostazioni predefinite prima.

### Serie MiFare
|   |
| ------------ |
|  Modalità personalizzata [OFF] |
|  ID [de:ad:be:ef]|
|  SAK [08] |
|  ATQA: [00 40]|
|  Gen1A Abilitato [OFF] |
|  Gen2 Abilitato [OFF] |
|  Modalità di scrittura [Normale] |
|  [RITORNO] |
|   |


* Modalità personalizzata: Di default è disattivata. Disattivata: Durante la fase di riconoscimento della carta ID, ID/SAK/ATQA vengono letti dal settore 0 dei dati della carta. Attivata: Puoi impostare risorse indipendenti nel menu ID/SAK/ATQA qui sotto.
* ID: Mostra l'ID corrente della carta. Se la modalità personalizzata è attiva, premendo il pulsante centrale ti permette di entrare nell'interfaccia di input per la modifica. Se la modalità personalizzata è disattivata, premendo il pulsante centrale non ha effetto.
* SAK: Mostra l'SAK corrente della carta. Se la modalità personalizzata è attiva, premendo il pulsante centrale ti permette di entrare nell'interfaccia di input per la modifica. Se la modalità personalizzata è disattivata, premendo il pulsante centrale non ha effetto.
* ATQA: Mostra l'ATQA corrente della carta. Se la modalità personalizzata è attiva, premendo il pulsante centrale ti permette di entrare nell'interfaccia di input per la modifica. Se la modalità personalizzata è disattivata, premendo il pulsante centrale non ha effetto.
* Gen1A Abilitato: Di default è disattivato. Quando abilitato, permette all'emulatore di rispondere ai comandi backdoor avanzati domestici. Questi comandi backdoor possono sbloccare direttamente la carta senza la necessità di una chiave.
* Gen2 Abilitato: Di default è disattivato. Quando abilitato, permette la scrittura nel settore 0. (Il settore 0 memorizza principalmente informazioni come ID/SAK/ATQA, e permettendo la scrittura può modificare l'ID della carta)
* Modalità di scrittura: Di default è normale. Ci possono essere 4 valori, vedi la tabella sottostante.

|  Modalità di scrittura | Descrizione |
| ----- | --- |
| Normale | I dati vengono scritti normalmente in memoria, cambia carta o esci per persistere |
| Nega | Qualsiasi operazione di scrittura restituisce un fallimento |
| Ignora | L'operazione di scrittura restituisce un successo, ma non scrive in memoria o persiste |
| Cache | I dati vengono scritti normalmente in memoria, cambiando le carte o uscendo non persistono |

> Si consiglia di abilitare la modalità Gen1A e la modalità Gen2 quando si copiano le carte, e chiuderle dopo la copia per evitare di essere riconosciute come una carta clone dai lettori di carte.

### Serie NTAG
|   |
| ------------ |
|  ID [04:68:95:71:fa:5c:64]|
|  SAK [00] |
|  ATQA: [00 44] |
|  Rand. UID |
|  [RITORNO] |
|   |


* ID: Mostra l'ID corrente della carta. Non modificabile.
* SAK: Mostra l'SAK corrente della carta. Non modificabile.
* ATQA: Mostra l'ATQA corrente della carta. Non modificabile.
* Rand. UID: Premendo il pulsante centrale si può generare casualmente un nuovo ID. Nota che è necessario tornare all'interfaccia di emulazione per simulare e salvare il nuovo ID della carta.

## Impostazioni Slot


Questa interfaccia mostra lo stato di abilitazione/disabilitazione di tutti gli slot delle carte e permette di abilitare o disabilitare individualmente ogni slot delle carte.

|   |
| ------------ |
|  Num. Slot.. [8]|
|  Slot 01   [ON] |
|  Slot 02   [OFF] |
|  .. |
|  [RITORNO] |
|   |

* Num. Slot: Mostra il numero corrente di slot delle carte, che è impostato di default a 8. Attualmente, la modifica della quantità di slot non è supportata, e sarà disponibile negli aggiornamenti futuri del firmware.
* Slot xx: Mostra lo stato di apertura/chiusura dello slot della carta. Premendo il pulsante centrale si può passare tra lo stato di apertura e chiusura.

----
# Trasferimento file BLE
Questa applicazione ti permette di connetterti all'iNFC o alla pagina web pixl.js, per gestire i file sulla memoria del dispositivo o aggiornare il firmware.

La pagina web ufficiale è [https://pixl.amiibo.xyz/](https://pixl.amiibo.xyz/ "https://pixl.amiibo.xyz/.").

L'**APP iNFC** è un'app di terze parti

Entrando in questa modalità l'indirizzo MAC Bluetooth del dispositivo e l'URL ufficiale vengono mostrati sullo schermo.

----
# Impostazioni
Questa applicazione permette di configurare le impostazioni di funzionamento del dispositivo, l'applicazione ha le seguenti voci nel menu:

|   |
| ------------ |
| Versione []<BR>Lingua []<BR>Memoria Auto []<BR>Retroilluminazione / Contrasto OLED []<BR>Animazione Menu []<BR>Batteria LiPO []<BR>Memoria Utilizzata []<BR>Ripresa Veloce []<BR>Timer Sleep []<BR>Aggiornamento Firmware<BR>Riavvio Sistema |
|   |

## Versione
Questo sottomenu mostra i dettagli della versione del firmware.
- Versione
- Data di costruzione
- Githash
- Branch
- Target
- Dirty
## Lingua
Puoi cambiare la lingua visualizzata sul dispositivo
## Memoria Auto
Attivando questa impostazione le memorie locali del dispositivo sono nascoste alla radice dell'applicazione Emulatore Amiibo, se questa impostazione è OFF l'oggetto «Flash esterno» viene mostrato alla radice dell'applicazione Emulatore Amiibo.
## Retroilluminazione / Contrasto OLED
Con questa impostazione puoi controllare la luminosità dello schermo LCD o il contrasto dello schermo OLED.

## Animazione Menu
Abilita / disabilita l'animazione degli elementi più grandi dello schermo permettendo di leggerli completamente, abilitando questa opzione aumenta il consumo della batteria.
## Batteria LiPO
Abilita l'uso di una batteria LiPO. Richiede una modifica hardware. Se costruisci una versione Pixl.js del dispositivo con LiPO, puoi abilitare questa opzione. Per la versione CR2032, questa opzione non funzionerà e non dovrebbe essere abilitata.

Se abiliti l'opzione LiPO, il dispositivo utilizzerà il livello di tensione LiPO per visualizzare il livello della batteria, e può anche visualizzare un'etichetta di carica quando la batteria è in modalità di carica.
## Memoria Utilizzata
Mostra la percentuale di memoria di archiviazione utilizzata in fondo allo schermo
## Risveglio Rapido
Abilita l'ibernazione del dispositivo, permettendo una risposta più veloce alla lettura dell'amiibo corrente da parte di un gioco, e riprende all'applicazione aperta al momento del sonno, quando disabilitato la ripresa dopo il sonno richiede 1 secondo in più e il dispositivo ripristina al Menu Principale.
## Timer Sleep
Imposta il tempo in cui il dispositivo va in sleep dopo l'ultimo input dell'utente o la lettura dell'amiibo.
## Aggiornamento Firmware
Questa opzione mette il dispositivo in modalità DFU, permettendo l'aggiornamento del firmware OTA, puoi aggiornare il firmware utilizzando l'iNFC o la pagina web.

Puoi andare all'URL [https://thegecko.github.io/web-bluetooth-dfu/](https://thegecko.github.io/web-bluetooth-dfu/) per caricare il firmware, questa pagina può anche essere aperta attraverso il sito ufficiale [https://pixl.amiibo.xyz/](https://pixl.amiibo.xyz/)
## Riavvio Sistema
Ti permette di riavviare il dispositivo e tornare allo stato dopo che hai rimosso e inserito la batteria.
