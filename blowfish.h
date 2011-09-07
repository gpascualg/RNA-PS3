#ifndef BLOWFISH_H
#define BLOWFISH_H

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <errno.h>
#include <psl1ght/types.h>

class cBlowFish
{
private:
	u32 		PArray[18];
	u32		SBoxes[4][256];
	void 		Blowfish_encipher(u32 *xl, u32 *xr);
	void 		Blowfish_decipher(u32 *xl, u32 *xr);

public:
	cBlowFish();
	~cBlowFish();
	void 		Initialize(u8 key[], int keybytes);
	u32		GetOutputLength(u32 lInputLong);
	u32		Encode(u8 * pInput, u8 * pOutput, u32 lSize);
	void		Decode(u8 * pInput, u8 * pOutput, u32 lSize);
};

#endif