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
    generateKeys(&keyPair);
    
    char *stringPayload = "Testing RSA encryption. Yay!";
    vU1024 payload = encodeString(stringPayload);
    
    vU1024 encrypted = MDRSAEncrypt(&payload, &keyPair.publicKey);
    vU1024 decrypted = MDRSADecrypt(&encrypted, &keyPair);
    
    if (bignum_equal(&payload, &decrypted)) {
        printf("Success! Initial payload and RSA encrypted -> decrypted "
               "payload match.\n");
        
        char *encryptedString = malloc(sizeof(encrypted) + sizeof(char));
        memcpy(encryptedString, &encrypted, sizeof(encrypted));
        encryptedString[sizeof(encrypted) / sizeof(char)] = '\0';
        
        printf("Initial payload: %s\n", stringPayload);
        printf("Encrypted payload: %s\n", encryptedString);
        printf("Decrypted payload: %s\n", decodeString(&decrypted));
    }
    
    return 0;
}
