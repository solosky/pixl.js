#ifndef NFC_READER_H
#define NFC_READER_H

#include "rc522.h"
#include "app_status.h"


void nfc_reader_init();
void nfc_reader_enter();
void nfc_reader_exit();

#endif