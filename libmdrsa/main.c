 //
//  main.c
//  libmdrsa
//
//  Created by Mert Dümenci on 3/17/17.
//  Copyright © 2017 Mert Dümenci. All rights reserved.
//

#include <stdio.h>
#include "bnhelpers.h"
#include "prime.h"
#include "rsa.h"

extern void extendedEuclidean(const vU1024 *x, const vU1024 *y,
                       vU1024 *d, vU1024 *a, vU1024 *b);

int main(int argc, const char * argv[]) {
    vU1024 x = generatePrime(150);
    vU1024 y = generatePrime(149);
    
    vU1024 d;
    vU1024 a;
    vU1024 b;
    
    extendedEuclidean(&x, &y, &d, &a, &b);
    
    printf("X %s, Y %s, D %s, A %s, B %s\n", bignum_toString(&x),
           bignum_toString(&y), bignum_toString(&d),
           bignum_toString(&a), bignum_toString(&b));
    
    return 0;
}
