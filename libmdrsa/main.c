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

#define BENCHMARK

static long _MDRSALoadTextFile(char *filename, char **text) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        *text = NULL;
        return -1;
    }
    
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    *text = (char *)malloc(size + 1);
    if (size != fread(*text, sizeof(char), size, file)) {
        free(*text);
        return -1;
    }
    
    fclose(file);
    (*text)[size] = '\0';
    
    return size;
}

int main(int argc, const char * argv[]) {
#ifdef BENCHMARK
    uint64_t counter = 0;
    
    char *testText;
    long testTextLen = 0;
    if ((testTextLen = _MDRSALoadTextFile("test_plaintext.txt", &testText))
        < 0) {
        printf("Fatal error: couldn't read test file.\n");
        exit(-1);
    }
    
    MDRSAKeyPair keyPair;
    MDRSAGenerateKeys(&keyPair);
    
    while (1) {
#endif
        MDRSAEncryptedPayload encrypted = MDRSAEncrypt(testText,
                                                       testTextLen + 1,
                                                       &keyPair.publicKey);
        char *decrypted;
        MDRSADecrypt(&encrypted, &keyPair, (void **)&decrypted);
        
        if (strcmp(testText, decrypted) != 0) {
            printf("Fatal error: RSA failed. Plaintext & roundtrip data"
                   " don't match.\n");
            exit(-1);
        }
        
        free(encrypted.chunks);
        free(decrypted);
        
#ifdef BENCHMARK
        printf("%llu round trips\n", counter);
        
        counter += 1;
    }
#endif
    
    return 0;
}
