//
//  rsa.c
//  libmdrsa
//
//  Created by Mert Dümenci on 3/22/17.
//  Copyright © 2017 Mert Dümenci. All rights reserved.
//

#include "rsa.h"
#include "bignum.h"
#include <dispatch/dispatch.h>

static int kMDRSAPrimeLength = 50;
static int possibleEs[] = {3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41};

static dispatch_queue_t kMDRSAKeyGenerationQueue;
static dispatch_queue_t kMDRSAEncryptionQueue;
static dispatch_queue_t kMDRSADecryptionQueue;

void _MDRSAExtendedEuclidean(const vU1024 *x, const vU1024 *y,
                       vU1024 *d, vS1024 *a, vS1024 *b) {
    if (MDRSABignumIsZero((vU1024 *)y)) {
        vU1024 one = MDRSABignumFromInteger(1);
        vU1024 zero = MDRSABignumFromInteger(0);
        
        *d = *x;
        *a = MDRSABignumCastSigned(&one);
        *b = MDRSABignumCastSigned(&zero);
        
        return;
    }
    
    vU1024 retD;
    vS1024 retA;
    vS1024 retB;
    
    vU1024 xMody;
    vU1024 xOverY;
    vU1024Divide(x, y, &xOverY, &xMody);
    
    _MDRSAExtendedEuclidean(y, &xMody, &retD, &retA, &retB);
    
    vS1024 aDelta;
    vS1024 signedXOverY = MDRSABignumCastSigned(&xOverY);
    vS1024HalfMultiply(&signedXOverY, &retB, &aDelta);
    
    vS1024 newB;
    vS1024Sub((vS1024 *)&retA, (vS1024 *)&aDelta, &newB);
    
    *d = retD;
    *a = retB;
    *b = newB;
}

void MDRSAGenerateKeys(MDRSAKeyPair *keyPair) {
    if (kMDRSAKeyGenerationQueue == NULL) {
        kMDRSAKeyGenerationQueue =
            dispatch_queue_create("me.dumenci.rsa.keygeneration",
                                  DISPATCH_QUEUE_CONCURRENT);
    }
    
    __block vU1024 p;
    __block vU1024 q;
    
    dispatch_group_t keyGroup = dispatch_group_create();
    dispatch_group_async(keyGroup, kMDRSAKeyGenerationQueue, ^{
        p = MDRSAFindPrime(kMDRSAPrimeLength);
    });
    dispatch_group_async(keyGroup, kMDRSAKeyGenerationQueue, ^{
        q = MDRSAFindPrime(kMDRSAPrimeLength);
    });
    dispatch_group_wait(keyGroup, DISPATCH_TIME_FOREVER);
    dispatch_release(keyGroup);

    vU1024 n;
    
    vU1024HalfMultiply(&p, &q, &n);
    
    vU1024 one = MDRSABignumFromInteger(1);
    vU1024 pMinus1;
    vU1024 qMinus1;
    vU1024 pq;
    
    vU1024Sub(&p, &one, &pMinus1);
    vU1024Sub(&q, &one, &qMinus1);
    vU1024HalfMultiply(&pMinus1, &qMinus1, &pq);
    
    vU1024 e;
    vU1024 divisor;
    vS1024 a;
    vS1024 b;
    
    int eSearchIndex = 0;
    
    while (!MDRSABignumEqual(&divisor, &one)) {
        if (eSearchIndex >= (sizeof(possibleEs) / sizeof(int))) {
            printf("Can't generate keys");
            exit(-1);
        }
        
        e = MDRSABignumFromInteger(possibleEs[eSearchIndex]);
        _MDRSAExtendedEuclidean(&pq, &e, &divisor, &a, &b);
        eSearchIndex++;
    }
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconditional-uninitialized"
    vS1024 signedPQ = MDRSABignumCastSigned(&pq);
    
    // Add k * pq to b to make it positive.
    // (Observe that b (mod pq) = b + k * pq (mod pq).)
    while (MDRSABignumSignedIsNegative(&b)) {
        vS1024 newB;
        vS1024Add(&b, &signedPQ, &newB);
        b = newB;
    }
    
    vU1024 unsignedB = MDRSABignumCastUnsigned(&b);
    
    MDRSAPublicKey public = { .e = e, .n = n };
    MDRSAPrivateKey private = { .d = unsignedB };
    
    *keyPair = (MDRSAKeyPair){ .publicKey = public, .privateKey = private };
#pragma clang diagnostic pop
}

vU1024 _MDRSAEncrypt(vU1024 *payload, MDRSAPublicKey *publicKey) {
    vU1024 encryptedPayload;
    MDRSAFastModuloPow(payload, &(publicKey->e), &(publicKey->n),
                       &encryptedPayload);
    
    return encryptedPayload;
}

/*
    We compute the chunk size to be the (prime length - 1) decimal digits
    in bytes.

    e.g. If we're using 50 digit primes as keys, we're going to have
        log_2(10^49) / 8 ~= 20 byte chunks.

    TODO: Declare the prime length in bytes. Digits are unnecessary and introduce
    complexity/possible loss of precision.
*/
size_t _MDRSAChunkSize() {
    int chunkSizeInBits = (int)((kMDRSAPrimeLength - 1) * (log(10) / log(2)));
    return (chunkSizeInBits / 8);
}

int _MDRSANumberOfChunks(size_t data_len, MDRSAPublicKey *publicKey) {
    return (int)ceil(data_len / (double)_MDRSAChunkSize());
}

MDRSAEncryptedPayload MDRSAEncrypt(void *payload, size_t data_len,
                                   MDRSAPublicKey *publicKey) {
    if (data_len == 0) {
        return (MDRSAEncryptedPayload){};
    }
    
    if (kMDRSAEncryptionQueue == NULL) {
        kMDRSAEncryptionQueue =
        dispatch_queue_create("me.dumenci.rsa.encryption",
                              DISPATCH_QUEUE_CONCURRENT);
    }
    
    int numOfChunks = _MDRSANumberOfChunks(data_len, publicKey);
    vU1024 *encryptedChunks = malloc(sizeof(vU1024) * numOfChunks);
    char *payloadBytes = (char *)payload;
    size_t chunkSize = _MDRSAChunkSize();
    
    dispatch_apply(numOfChunks, kMDRSAEncryptionQueue, ^(size_t i) {
        size_t applicableChunkSize = MIN(data_len - (i * chunkSize),
                                         chunkSize);
        
        vU1024 currentPayloadChunk;
        memset(&currentPayloadChunk, 0, sizeof(currentPayloadChunk));
        memcpy(&currentPayloadChunk, &payloadBytes[i * chunkSize],
               applicableChunkSize);
        
        vU1024 currentEncryptedChunk = _MDRSAEncrypt(&currentPayloadChunk,
                                                     publicKey);
        encryptedChunks[i] = currentEncryptedChunk;
    });
    
    return (MDRSAEncryptedPayload){ .chunks = encryptedChunks,
        .chunksLength = numOfChunks, .chunkSize = chunkSize,
        .dataLength = data_len };
}

vU1024 _MDRSADecrypt(vU1024 *encryptedPayload, MDRSAKeyPair *keyPair) {
    vU1024 decryptedPayload;
    MDRSAFastModuloPow(encryptedPayload, &(keyPair->privateKey.d),
                       &(keyPair->publicKey.n), &decryptedPayload);

    return decryptedPayload;
}

void MDRSADecrypt(MDRSAEncryptedPayload *encryptedPayload,
                  MDRSAKeyPair *keyPair,
                  void **decryptedData) {
    if (kMDRSADecryptionQueue == NULL) {
        kMDRSADecryptionQueue =
        dispatch_queue_create("me.dumenci.rsa.decryption",
                              DISPATCH_QUEUE_CONCURRENT);
    }
    
    *decryptedData = malloc(encryptedPayload->dataLength);
    char *_decryptedData = (char *)*decryptedData;
    
    dispatch_apply(encryptedPayload->chunksLength, kMDRSADecryptionQueue, ^(size_t i) {
        size_t applicableChunkSize = MIN(encryptedPayload->dataLength
                                         - (i * encryptedPayload->chunkSize),
                                         encryptedPayload->chunkSize);
        
        vU1024 currentChunk = encryptedPayload->chunks[i];
        vU1024 decryptedChunk = _MDRSADecrypt(&currentChunk, keyPair);
        memcpy(&_decryptedData[i * encryptedPayload->chunkSize],
               &decryptedChunk, applicableChunkSize);
    });
}
