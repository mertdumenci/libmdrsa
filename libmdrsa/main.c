 //
//  main.c
//  libmdrsa
//
//  Created by Mert Dümenci on 3/17/17.
//  Copyright © 2017 Mert Dümenci. All rights reserved.
//

#include <stdio.h>
#include "bignum.h"
#include "prime.h"
#include "rsa.h"

vU1024 encodeString(char *string) {
#ifdef DEBUG
    printf("DEBUG: Encoded string is %lu characters long.\n", strlen(string));
#endif
    
    if (strlen(string) > sizeof(vU1024) - 1) {
        printf("Error, can't encode string \"%s\" into a 1024-bit integer.",
               string);
        exit(-1);
    }
    
    size_t stringLength = strlen(string);
    
    vU1024 encodedString;
    memset(&encodedString, 0, sizeof(vU1024));
    memcpy(&encodedString, string, stringLength * sizeof(char));
    ((char *)&encodedString)[stringLength] = '\0';
    
    return encodedString;
}

char *decodeString(vU1024 *encodedString) {
    return (char *)encodedString;
}

int main(int argc, const char * argv[]) {
    MDRSAKeyPair keyPair;
    MDRSAGenerateKeys(&keyPair);
    
    char *stringPayload = "Hello world, this is something bigger. Chunking"
    " should handle this relatively long payload.";
    
    vU1024 payload = encodeString(stringPayload);
    
    MDRSAEncryptedPayload encrypted = MDRSAEncrypt(&payload, &keyPair.publicKey);
    vU1024 decrypted = MDRSADecrypt(&encrypted, &keyPair);
    
    if (MDRSABignumEqual(&payload, &decrypted)) {
        printf("Success! Initial and roundtrip RSA payloads match.\n");
    
        printf("Initial payload: \"%s\"\n", stringPayload);
        printf("Decrypted payload: \"%s\"\n", decodeString(&decrypted));
    }
    
    return 0;
}
