//
//  bignum.c
//  libmdrsa
//
//  Created by Mert Dümenci on 3/17/17.
//  Copyright © 2017 Mert Dümenci. All rights reserved.
//

#include "bignum.h"
#include "list.h"

bool MDRSABignumEqual(vU1024 *a, vU1024 *b) {
    return (memcmp(a, b, sizeof(vU1024)) == 0);
}

bool MDRSABignumSignedEqual(vS1024 *a, vS1024 *b) {
    return (memcmp(a, b, sizeof(vS1024)) == 0);
}

bool MDRSABignumIsZero(vU1024 *bignum) {
    vU1024 zero = MDRSABignumFromInteger(0);
    return MDRSABignumEqual(bignum, &zero);
}

bool MDRSABignumIsOdd(vU1024 *bignum) {
    return (bignum->s.LSW & 0x1) != 0;
}

char *MDRSABignumToString(vU1024 *bignum) {
    if (MDRSABignumIsZero(bignum)) {
        return "0";
    }
    
    list(int, digitList);
    memset(&digitList, 0, sizeof(digitList));
    
    vU1024 zero = MDRSABignumFromInteger(0);
    vU1024 ten = MDRSABignumFromInteger(10);
    
    vU1024 intermediate;
    memcpy(&intermediate, bignum, sizeof(vU1024));

    while (!MDRSABignumEqual(&intermediate, &zero)) {
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

char *MDRSABignumSignedToString(vS1024 *bignum) {
    bool isNegative = MDRSABignumSignedIsNegative(bignum);
    
    vU1024 positive;
    
    if (!isNegative) {
        memcpy(&positive, bignum, sizeof(vS1024));
        return MDRSABignumToString(&positive);
    }
    
    vU1024 unsignedBignum;
    memcpy(&unsignedBignum, bignum, sizeof(vS1024));
    
    // I don't even know why `vU1024Neg` isn't a 1's complement negation,
    // since it takes an unsigned number and outputs an unsigned number...
    vU1024Neg(&unsignedBignum, &positive);
    
    char *positiveRepresentation = MDRSABignumToString(&positive);
    char *string = malloc(sizeof(char) + sizeof(positiveRepresentation));
    strcpy(string + 1, positiveRepresentation);
    string[0] = '-';
    
    return string;
}

void MDRSABignumPrint(vU1024 *bignum) {
    char *string = MDRSABignumToString(bignum);
    printf("%s\n", string);
}

vU1024 MDRSABignumFromInteger(uint64_t integer) {
    vU1024 bignum;
    memset(&bignum, 0, sizeof(vU1024));
    memcpy(&bignum, &integer, sizeof(uint64_t));
    
    return bignum;
}

vS1024 MDRSABignumSignedFromInteger(int64_t integer) {
    bool isNegative = (integer < 0);
    
    vS1024 bignum;
    memset(&bignum, 0, sizeof(vS1024));
    memcpy(&bignum, &integer, sizeof(int64_t));
    
    // Sign extension
    if (isNegative) {
        memset((((int64_t *)&bignum) + 1), 1, sizeof(vU1024) - 8);
    }
    
    return bignum;
}

vU1024 MDRSABignumRand(int digits) {
    int bits = (int)(digits * (log(10) / log(2)));
    vU1024 rand;
    memset(&rand, 0, sizeof(vU1024));
    arc4random_buf(&rand, bits / 8);
    return rand;
}

vS1024 MDRSABignumCastSigned(vU1024 *unsignedBignum) {
    vS1024 signedBignum;
    memcpy(&signedBignum, unsignedBignum, sizeof(vU1024));
    
    return signedBignum;
}

vU1024 MDRSABignumCastUnsigned(vS1024 *signedBignum) {
    assert(!MDRSABignumSignedIsNegative(signedBignum));
    
    vU1024 unsignedBignum;
    memcpy(&unsignedBignum, signedBignum, sizeof(vS1024));
    
    return unsignedBignum;
}

bool MDRSABignumSignedIsNegative(vS1024 *bignum) {
    return (bignum->s.MSW & 0x10000000) != 0;
}
