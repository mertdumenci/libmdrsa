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

int main(int argc, const char * argv[]) {    
    vU1024 prime = generatePrime(150);
    printf("Prime generated: %s\n", bignum_toString(&prime));
    
    return 0;
}
