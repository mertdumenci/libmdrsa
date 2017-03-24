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

void MDRSAGenerateKeys(MDRSAKeyPair *keyPair);
vU1024 MDRSAEncrypt(vU1024 *payload, MDRSAPublicKey *publicKey);
vU1024 MDRSADecrypt(vU1024 *encryptedPayload, MDRSAKeyPair *keyPair);

#endif /* rsa_h */
