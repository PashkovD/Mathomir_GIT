#pragma once
#include <stdlib.h>
#include "Mathomir.h"

// CRYPTO LIBRARY FOR EXCHANGING KEYS
// USING THE DIFFIE-HELLMAN KEY EXCHANGE PROTOCOL

// The diffie-hellman can be used to securely exchange keys
// between parties, where a third party eavesdropper given
// the values being transmitted cannot determine the key.

// Implemented by Lee Griffiths, Jan 2004.
// This software is freeware, you may use it to your discretion,
// however by doing so you take full responsibility for any damage
// it may cause.

// Hope you find it useful, even if you just use some of the functions
// out of it like the prime number generator and the XtoYmodN function.

// It would be great if you could send me emails to: griffter@hotmail.co.uk
// with any suggestions, comments, or questions!

// Enjoy.

// Updated 12 Novemeber 2008 - class appropriately renamed

// Updated 06 July 2006 - no longer requires Windows API's
//                        random number generation replaced.
//                        Thanks to Ilya O. Levin on help with q-rng


#define MAX_RANDOM_INTEGER 2147483648 //Should make these numbers massive to be more secure
#define MAX_PRIME_NUMBER   2147483648 //Bigger the number the slower the algorithm

// Thanks to Ilya O. Levin (whoever you are, thanks!)
//Linear Feedback Shift Registers
#define LFSR(n)    {if ((n)&1) (n)=(((n)^0x80000055)>>1)|0x80000000; else (n)>>=1;}

//Rotate32
#define ROT(x, y)  ((x)=((x)<<(y))|((x)>>(32-(y))))

#ifdef TEACHER_VERSION
class CDiffieHellman
{
public:
    CDiffieHellman();
    ~CDiffieHellman();
    void DerivePublicKey(char* password, int64_t* N, int64_t* X);
    void CreateDecryptionKey(int64_t Y, int64_t N, int64_t* Key);
    void CreateEncryptionKey(int64_t N, int64_t X, int64_t* Key, int64_t* Y);

private:
    int64_t XpowYmodN(int64_t x, int64_t y, int64_t N);
    uint64_t GenerateRandomNumber();
    uint64_t GeneratePrime();
    bool MillerRabin(int64_t n);
    int64_t a;
};
#endif
