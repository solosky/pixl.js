/*
 * ntag_emu.h
 *
 *  Created on: 2021年9月1日
 *      Author: solos
 */

#ifndef NTAG_EMU_H_
#define NTAG_EMU_H_

#include "ntag_def.h"

ret_code_t ntag_emu_init(ntag_t* tag);
void ntag_emu_uninit();
void ntag_emu_set_tag(ntag_t* tag);
void ntag_emu_set_uuid_only(ntag_t* ntag);
ntag_t* ntag_emu_get_current_tag();




#endif /* NTAG_EMU_H_ */
