//
//  prime.c
//  libmdrsa
//
//  Created by Mert Dümenci on 3/17/17.
//  Copyright © 2017 Mert Dümenci. All rights reserved.
//

#include "prime.h"
#include "bnhelpers.h"

void fastModuloPow(vU1024 *base, vU1024 *power, vU1024 *modulo, vU1024 *result) {
    bool willDoModulo = !bignum_isZero(modulo);
    vU1024 internalResult = bignum_from64(1);
    vU1024 internalPower;
    vU1024 internalBase;
    
    memcpy(&internalPower, power, sizeof(vU1024));
    memcpy(&internalBase, base, sizeof(vU1024));
    
    while (!bignum_isZero(&internalPower)) {
        if (bignum_isOdd(&internalPower)) {
            vU1024 newResult;
            vU1024HalfMultiply(&internalResult, &internalBase, &newResult);
            internalResult = newResult;
            
            if (willDoModulo) {
                vU1024 newResult;
                vU1024Mod(&internalResult, modulo, &newResult);
                internalResult = newResult;
            }
        }
        
        vU1024 newPower;
        vLR1024Shift(&internalPower, 1, &newPower);
        internalPower = newPower;
        
        vU1024 newBase;
        vU1024HalfMultiply(&internalBase, &internalBase, &newBase);
        internalBase = newBase;
        
        if (willDoModulo) {
            vU1024 newResult;
            vU1024Mod(&internalResult, modulo, &newResult);
            internalResult = newResult;
        }
    }
    
    memcpy(result, &internalResult, sizeof(vU1024));
}

bool fermatTest(vU1024 *a, vU1024 *p) {
    vU1024 result;
    vU1024 power;
    
    vU1024 one = bignum_from64(1);
    vU1024Sub(p, &one, &power); // power = p - 1
    
    fastModuloPow(a, &power, p, &result);
    
    return bignum_equal(&result, &one);
}

//func fermatTest(a: UInt64, p: UInt64) -> Bool {
//    return (power(base: a, power: p - 1, modulo: p)) == 1
//    }
//    
//    func isPrime(candidate: UInt64) -> Bool {
//        var primeCounter = UInt64(log10(Double(candidate)) * 5)
//        
//        while primeCounter > 0 {
//            let randomBase = random(to: candidate)
//            if !fermatTest(a: randomBase, p: candidate) {
//                return false
//            }
//            
//            primeCounter -= 1
//        }
//        
//        return true
//    }
//    
//    func generatePrime(digits: UInt64) -> UInt64 {
//        var primeCounter = digits * 10
//        let upperBound = power(base: 10, power: digits, modulo: nil)
//        
//        while primeCounter > 0 {
//            let candidate = random(to: upperBound)
//            
//            if isPrime(candidate: candidate) {
//                return candidate
//            }
//            
//            primeCounter -= 1
//        }
//        
//        fatalError("Couldn't find prime")
//    }
