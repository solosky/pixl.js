
# Traduzione

## Come aggiornare una traduzione esistente

1. Modifica fw/data/i18n.csv con il tuo editor di testo preferito. Per vscode, si consiglia l'estensione "Edit csv".
2. Esegui `fw/scripts/i18n_gen.py` per generare nuovi file di lingua.
3. Opzionale: Esegui `fw/scripts/font_data_gen.sh` per generare nuovi dati di font se aggiungi nuovi caratteri in i18n.csv
4. Esegui `make full` per ricostruire il firmware

nota: <br />
Lo script `font_data_gen.sh` può essere eseguito solo in Git Bash windows in Windows.

## Come aggiungere una nuova traduzione di lingua

1. Aggiungi una nuova colonna in fw/data/i18n.csv, ad esempio "ja_JP"
2. Esegui `fw/scripts/i18n_gen.py` per generare nuovi file di lingua.
3. Opzionale: Esegui `fw/scripts/font_data_gen.sh` per generare nuovi dati di font se aggiungi nuovi caratteri in i18n.csv
4. Modifica `fw/application/src/i18n/language.h` e `fw/application/src/i18n/language.c` per adottare la nuova lingua
5. Modifica il Makefile per includere `$(PROJ_DIR)/i18n/ja_JP.c` come file sorgente C
6. Esegui `make full` per ricostruire il firmware

## Note sui font

Per la build di rilascio (RELEASE=1), il firmware utilizza wenquanyi_9pt_u8g2.bdf per visualizzare i caratteri unicode. <br />
Controlla se il punto di codice del carattere della nuova lingua è incluso in wenquanyi_9pt_u8g2.bdf o meno. <br />
In caso contrario, non si consiglia di supportare la nuova lingua a causa della restrizione del flash interno dell'MCU.