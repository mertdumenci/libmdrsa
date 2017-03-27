//
//  rsa.h
//  libmdrsa
//
//  Created by Mert Dümenci on 3/22/17.
//  Copyright © 2017 Mert Dümenci. All rights reserved.
//

#ifndef rsa_h
#define rsa_h

#include <stdio.h>
#include "prime.h"

typedef struct MDRSAPublicKey {
    vU1024 e;
    vU1024 n;
} MDRSAPublicKey;

typedef struct MDRSAPrivateKey {
    vU1024 d;
} MDRSAPrivateKey;

typedef struct MDRSAKeyPair {
    MDRSAPublicKey publicKey;
    MDRSAPrivateKey privateKey;
} MDRSAKeyPair;

typedef struct MDRSAEncryptedPayload {
    vU1024 *chunks;
    int chunksLength;
    size_t chunkSize;
} MDRSAEncryptedPayload;

void MDRSAGenerateKeys(MDRSAKeyPair *keyPair);
MDRSAEncryptedPayload MDRSAEncrypt(vU1024 *payload, MDRSAPublicKey *publicKey);
vU1024 MDRSADecrypt(MDRSAEncryptedPayload *encryptedPayload,
                    MDRSAKeyPair *keyPair);

#endif /* rsa_h */
