/********************************************************************************************
* SIDH: an efficient supersingular isogeny cryptography library
*
* Abstract: benchmarking/testing isogeny-based key encapsulation mechanism
*********************************************************************************************/ 
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "../Inc/P751_api.h"
#include "../Inc/P751_internal.h"
#include "../Inc/random.h"

#define PASSED 0
#define FAILED 1    
//int ake_sike_keypair(unsigned char *pk, unsigned char *sk);
void DH(char *label, char *buf, int count);

/*
int ake_sike_kem()
{
    unsigned int i;
    unsigned char skA[CRYPTO_SECRETKEYBYTES] = {0};
    unsigned char pkA[CRYPTO_PUBLICKEYBYTES] = {0};
    unsigned char ctA[CRYPTO_CIPHERTEXTBYTES] = {0};
    unsigned char kA[CRYPTO_BYTES] = {0};
    unsigned char skB[CRYPTO_SECRETKEYBYTES] = {0};
    unsigned char pkB[CRYPTO_PUBLICKEYBYTES] = {0};
    unsigned char ctB[CRYPTO_CIPHERTEXTBYTES] = {0};
    unsigned char kB[CRYPTO_BYTES] = {0};
    unsigned char kAfromB[CRYPTO_BYTES] = {0};
    unsigned char kBfromA[CRYPTO_BYTES] = {0};
    unsigned char kAout[CRYPTO_BYTES*2] = {0};
    unsigned char kBout[CRYPTO_BYTES*2] = {0};
    unsigned char aesA[CRYPTO_BYTES] = {0};
    unsigned char aesB[CRYPTO_BYTES] = {0};
    bool passed = PASSED;

    printf("start\n");

    ake_sike_keypair(pkA, skA);
 //   DH("Alice private Key", skA, CRYPTO_SECRETKEYBYTES);
 //   DH("Alice Public  Key", pkA, CRYPTO_PUBLICKEYBYTES);
    ake_sike_keypair(pkB, skB);
 //   DH("Bob private Key", skB, CRYPTO_SECRETKEYBYTES);
 //   DH("Bob Public  Key", pkB, CRYPTO_PUBLICKEYBYTES);

    // corrupt Bob's public key
    // pkA[5]= 0x22;
    // Alice encaps
    crypto_kem_enc_SIKEp751(ctA, kA, pkB);      // Bob public key in, Alice shared key and message out
 //   DH("Alice Msg", ctA, CRYPTO_CIPHERTEXTBYTES);
    DH("Alice Secret Key", kA, CRYPTO_BYTES);
    // Bob encaps
    crypto_kem_enc_SIKEp751(ctB, kB, pkA);      // Alice public key in, Bob shared key and message out
//    DH("Bob Msg", ctB, CRYPTO_CIPHERTEXTBYTES);
    DH("Bob Secret   Key", kB, CRYPTO_BYTES);

    // ct transferred to other entity
    // corrupt ctA
//    ctA[9]= 0x34;
//    ctB[9]= 0x34;

    // Alice decaps, the SIKE implementation has public key appended in the secret key
    crypto_kem_dec_SIKEp751(kAfromB, ctB, skA);
    memcpy(kAout, kA, CRYPTO_BYTES);
    memcpy(&kAout[CRYPTO_BYTES], kAfromB, CRYPTO_BYTES);
    DH("Alice ss", kAout, CRYPTO_BYTES*2);
    shake256(aesA, CRYPTO_BYTES, kAout, CRYPTO_BYTES*2);

    // Bob decaps
    crypto_kem_dec_SIKEp751(kBfromA, ctA, skB);
    memcpy(kBout, kBfromA, CRYPTO_BYTES);
//    DH("Bob Secret   Key", kB, CRYPTO_BYTES);
    memcpy(&kBout[CRYPTO_BYTES], kB, CRYPTO_BYTES);
//    DH("Bob Secret   Key", &kBout[CRYPTO_PUBLICKEYBYTES], CRYPTO_BYTES);
    DH("Bob   ss", kBout, CRYPTO_BYTES*2);
    shake256(aesB, CRYPTO_BYTES, kBout, CRYPTO_BYTES*2);

    DH("Alice AES", aesA, CRYPTO_BYTES);
    DH("Bob   AES", aesB, CRYPTO_BYTES);

    if (memcmp(aesA, aesB, CRYPTO_BYTES) != 0) {
        passed = FAILED;
    }
    return passed;
}

void ake_sike_keypair(unsigned char *pk, unsigned char *sk)
{ // Bob's SIKE keys generation
  // Outputs: secret key sk (CRYPTO_SECRETKEYBYTES = MSG_BYTES + SECRETKEY_B_BYTES + CRYPTO_PUBLICKEYBYTES bytes)
  //          public key pk (CRYPTO_PUBLICKEYBYTES bytes) 

    // Generate lower portion of secret key sk <- s||SK
    randombytes(sk, MSG_BYTES);
//    random_mod_order_SIDHp751(sk + MSG_BYTES);

    // Generate public key pk
    EphemeralKeyGeneration_A(sk + MSG_BYTES, pk);

    // Append public key pk to secret key sk
    memcpy(&sk[MSG_BYTES + SECRETKEY_B_BYTES], pk, CRYPTO_PUBLICKEYBYTES);

    return;
}

void ake_sike_keypair_A(unsigned char *pk, unsigned char *sk)
{ // Alice SIKE keys generation
  // Outputs: secret key sk (CRYPTO_SECRETKEYBYTES = MSG_BYTES + SECRETKEY_A_BYTES + CRYPTO_PUBLICKEYBYTES bytes)
  //          public key pk (CRYPTO_PUBLICKEYBYTES bytes) 

    // Generate lower portion of secret key sk <- s||SK
    randombytes(sk, MSG_BYTES);
    random_mod_order_A_SIDHp751(sk + MSG_BYTES);

    // Generate public key pk
    EphemeralKeyGeneration_A_SIDHp751(sk + MSG_BYTES, pk);

    // Append public key pk to secret key sk
    memcpy(&sk[MSG_BYTES + SECRETKEY_B_BYTES], pk, CRYPTO_PUBLICKEYBYTES);

    return;
}
*/
void DH(char *label, char *buf, int count)
{
    int i;

    printf("%s: ", label);
    for (i=0; i < count - 1; i++)
    {
        printf("%02hhX", buf[i]);
    }
        printf("%02hhX\n", buf[i]);
}
/*
int main()
{
    int Status = PASSED;
    
    Status = ake_sike_kem();             // Test key encapsulation mechanism
    if (Status != PASSED) {
        printf("\n\n   Error detected: KEM_ERROR_SHARED_KEY \n\n");
        return FAILED;
    }
    else
    {
        printf("\n\n   Passed\n\n");
        return PASSED;
    }
    

}
*/