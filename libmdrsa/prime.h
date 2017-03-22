//
//  prime.h
//  libmdrsa
//
//  Created by Mert Dümenci on 3/17/17.
//  Copyright © 2017 Mert Dümenci. All rights reserved.
//

#ifndef prime_h
#define prime_h

#include <stdio.h>
#include <Accelerate/Accelerate.h>

void fastModuloPow(vU1024 *base, vU1024 *power, vU1024 *modulo, vU1024 *result);
bool fermatTest(vU1024 *a, vU1024 *p);
bool isPrime(vU1024 *candidate, int numDigits);
vU1024 generatePrime(int numDigits);

#endif /* prime_h */
