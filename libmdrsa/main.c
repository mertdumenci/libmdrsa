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
                       vU1024 *d, vS1024 *a, vS1024 *b);

int main(int argc, const char * argv[]) {
    vU1024 pubE;
    vU1024 pubN;
    vS1024 privD;
    
    generateKeys(&pubE, &pubN, &privD);
    
    return 0;
}
