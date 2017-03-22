//
//  bnhelpers.h
//  libmdrsa
//
//  Created by Mert Dümenci on 3/17/17.
//  Copyright © 2017 Mert Dümenci. All rights reserved.
//

#ifndef bnhelpers_h
#define bnhelpers_h

#include <stdio.h>
#include <Accelerate/Accelerate.h>

bool bignum_equal(vU1024 *a, vU1024 *b);
bool bignum_isZero(vU1024 *bignum);
bool bignum_isOdd(vU1024 *bignum);
void bignum_printContents(vU1024 *bignum);
vU1024 bignum_from64(uint64_t integer);
vU1024 bignum_rand(int digits);

#endif /* bnhelpers_h */
