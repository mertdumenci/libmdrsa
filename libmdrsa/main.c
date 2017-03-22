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
    vU1024 base = bignum_from64(2);
    vU1024 power = bignum_from64(22);
    
    vU1024 result;
    vU1024 zero = bignum_from64(0);
    fastModuloPow(&base, &power, &zero, &result);
    bignum_printContents(&result);
    
//    vU1024 random = bignum_rand(200);
//    bignum_printContents(&random);
//    
//    vU1024 a = bignum_from64(5);
//    vU1024 p = bignum_from64(7);
//    
//    printf("%d", fermatTest(&a, &p));
    
    return 0;
}
