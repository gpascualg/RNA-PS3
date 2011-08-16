#ifndef BLOWFISH_H
#define BLOWFISH_H

#include <stdio.h>
#include <Windows.h>

class cBlowFish
{
private:
	DWORD 		PArray[18];
	DWORD		SBoxes[4][256];
	void 		Blowfish_encipher(DWORD *xl, DWORD *xr);
	void 		Blowfish_decipher(DWORD *xl, DWORD *xr);

public:
	cBlowFish();
	~cBlowFish();
	void 		Initialize(BYTE key[], int keybytes);
	DWORD		GetOutputLength(DWORD lInputLong);
	DWORD		Encode(BYTE * pInput, BYTE * pOutput, DWORD lSize);
	void		Decode(BYTE * pInput, BYTE * pOutput, DWORD lSize);
};

#endif