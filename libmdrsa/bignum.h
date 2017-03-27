//
//  bignum.h
//  libmdrsa
//
//  Created by Mert Dümenci on 3/17/17.
//  Copyright © 2017 Mert Dümenci. All rights reserved.
//

#ifndef bignum_h
#define bignum_h

#include <stdio.h>
#include <Accelerate/Accelerate.h>

bool MDRSABignumEqual(vU1024 *a, vU1024 *b);
bool MDRSABignumSignedEqual(vS1024 *a, vS1024 *b);
bool MDRSABignumIsZero(vU1024 *bignum);
bool MDRSABignumIsOdd(vU1024 *bignum);
char *MDRSABignumToString(vU1024 *bignum);
char *MDRSABignumSignedToString(vS1024 *bignum);
void MDRSABignumPrint(vU1024 *bignum);
vU1024 MDRSABignumFromInteger(uint64_t integer);
vS1024 MDRSABignumSignedFromInteger(int64_t integer);
vU1024 MDRSABignumRand(int digits);
vS1024 MDRSABignumCastSigned(vU1024 *unsignedBignum);
vU1024 MDRSABignumCastUnsigned(vS1024 *signedBignum);
bool MDRSABignumSignedIsNegative(vS1024 *bignum);
size_t MDRSABignumLengthInBytes(vU1024 *bignum);

#endif /* bignum_h */
