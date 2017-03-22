//
//  bnhelpers.c
//  libmdrsa
//
//  Created by Mert Dümenci on 3/17/17.
//  Copyright © 2017 Mert Dümenci. All rights reserved.
//

#include "bnhelpers.h"
#include "list.h"

bool bignum_equal(vU1024 *a, vU1024 *b) {
    return (memcmp(a, b, sizeof(vU1024)) == 0);
}

bool bignum_isZero(vU1024 *bignum) {
    vU1024 zero = bignum_from64(0);
    return bignum_equal(bignum, &zero);
}

bool bignum_isOdd(vU1024 *bignum) {
    return (bignum->s.LSW & 0x1) != 0;
}

char *bignum_toString(vU1024 *bignum) {
    list(int, digitList);
    memset(&digitList, 0, sizeof(digitList));
    
    vU1024 zero = bignum_from64(0);
    vU1024 ten = bignum_from64(10);
    
    vU1024 intermediate;
    memcpy(&intermediate, bignum, sizeof(vU1024));

    while (!bignum_equal(&intermediate, &zero)) {
        vU1024 newIntermediate;
        vU1024 digit;
        vU1024Divide(&intermediate, &ten, &newIntermediate, &digit);
        intermediate = newIntermediate;
        
        int intDigit;
        memcpy(&intDigit, &digit, sizeof(int));
        
        list_push(digitList, intDigit);
    }
    
    char *string = malloc(list_length(digitList) * sizeof(int));
    
    int index = 0;
    list_each_r(digitList, currentDigit) {
        string[index++] = currentDigit + '0';
    }
    
    string[index] = '\0';
    list_clear(digitList);
    
    return string;
}


void bignum_printContents(vU1024 *bignum) {
    char *string = bignum_toString(bignum);
    printf("%s\n", string);
}

vU1024 bignum_from64(uint64_t integer) {
    vU1024 bignum;
    memset(&bignum, 0, sizeof(vU1024));
    memcpy(&bignum, &integer, sizeof(uint64_t));
    
    return bignum;
}

vU1024 bignum_rand(int digits) {
    int bits = (int)ceil(digits * log2(10));
    vU1024 rand;
    arc4random_buf(&rand, bits / 8);
    return rand;
}
