# libmdrsa

libmdrsa is a toy RSA implementation in C using `Accelerate` for fast arithmetic
operations.

## Author
Mert Dumenci `mert@dumenci.me`

## Disclaimer
Do _not_ use `libmdrsa` for encryption. I only built it to understand RSA better,
and it's probably broken in a million ways that can be destructive in any serious
usage.

## Usage
```C
// Generate a key pair
MDRSAKeyPair keyPair;
MDRSAGenerateKeys(&keyPair);

// `encodeString` is an omitted function that encodes a string in an instance
// of the 1024-bit integer type `vU1024` (see bignum.h for `vU1024` helpers.)
char *stringPayload = "Hello world!";
vU1024 payload = encodeString(stringPayload);

// Encrypt the payload using the generated public key
MDRSAEncryptedPayload encrypted = MDRSAEncrypt(&payload, &keyPair.publicKey);

// Decrypt the encrypted payload using the private key
vU1024 decrypted = MDRSADecrypt(&encrypted, &keyPair);

// `encrypted` should equal `decrypted`!
```

## Internals
`libmdrsa` uses the Fermat (Probabilistic) Primality Test in its key generation
step. Check `prime.h` for detailed documentation (and why I used this primality
test over better alternatives). It can handle primes as big as 512-bit (~150
digit) in a very fast way thanks to the vectorized arithmetic operations
provided by the `Accelerate` framework.

## Notes
Key generation (50-digit, ~160-bit), the encryption and decryption of the string
`"Hello world, this is something bigger. Chunking should handle this relatively
long payload."` takes `~0.06s` on my MacBook Pro (2012, 2.3 GHz i7).

I found virtually no documentation about `Accelerate`'s bignum types (`vBigNum.h`)
and ended up figuring out a lot about these very useful types and their hardware
accelerated arithmetic operations. Check `bignum.c` if you're curious/want to
learn more about them!
