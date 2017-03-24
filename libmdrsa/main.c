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
    
    char *stringPayload = "Hello world!";
    vU1024 payload = encodeString(stringPayload);
    
    vU1024 encrypted = MDRSAEncrypt(&payload, &keyPair.publicKey);
    vU1024 decrypted = MDRSADecrypt(&encrypted, &keyPair);
    
    if (MDRSABignumEqual(&payload, &decrypted)) {
        printf("Success! Initial and roundtrip RSA payloads match.\n");
        
        // Treat 'encrypted' as a string for demonstration purposes
        char *encryptedString = malloc(sizeof(encrypted) + sizeof(char));
        memcpy(encryptedString, &encrypted, sizeof(encrypted));
        encryptedString[sizeof(encrypted) / sizeof(char)] = '\0';
        
        printf("Initial payload: \"%s\"\n", stringPayload);
        printf("Encrypted payload: \"%s\"\n", encryptedString);
        printf("Decrypted payload: \"%s\"\n", decodeString(&decrypted));
    }
    
    return 0;
}
