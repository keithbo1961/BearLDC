/*
 * keys.c
 *
 *  Created on: Mar 11, 2020
 *      Author:  dave, keith
 */


#include "../bearldc.h"

#ifdef BEARCLIENT
int
ClientKeysInit(SIKE_DH *keys)
{
	int ret = 0;
	crypto_kem_keypair_SIKEp751(keys->sikepublic, keys->sikeprivate);
	return ret;

}

int
ClientKeys(SIKE_DH *keys, unsigned char *msg, unsigned char *ctToLDC)
{
	int ret = 0;
	unsigned char ldcSec[CRYPTO_BYTES];
	unsigned char ctFromLDC[CRYPTO_CIPHERTEXTBYTES];
	unsigned char pubFromLDC[CRYPTO_PUBLICKEYBYTES];

	memcpy(ctFromLDC, msg, CRYPTO_CIPHERTEXTBYTES);
	memcpy(pubFromLDC, &msg[CRYPTO_CIPHERTEXTBYTES], CRYPTO_PUBLICKEYBYTES);

    /*
     * Back to Client code
     */
    // Decode message from LDC
	crypto_kem_dec_SIKEp751(ldcSec, ctFromLDC, keys->sikeprivate);
	// Client encaps
	// Client public key in, LDC shared secret and message out
	crypto_kem_enc_SIKEp751(ctToLDC, keys->Clientsecret, pubFromLDC);



	return ret;
}
#endif


int
LDCkeysM1(SIKE_DH *keys, unsigned char *ClientPublicKey, unsigned char *msg)
{
	int ret = 0;

	unsigned char ctFromLDC[CRYPTO_CIPHERTEXTBYTES];
	unsigned char pubFromLDC[CRYPTO_PUBLICKEYBYTES];

	// MSG 1: C1->LDC (Client announces himself with his public key)
	crypto_kem_keypair_SIKEp751(pubFromLDC, keys->LDC_sike_sk);					// LDC:generate LDCs "static" keypair
	crypto_kem_enc_SIKEp751(ctFromLDC, keys->LDC_part_ss, ClientPublicKey);     // LDC:generate encrypted message and LDC's (Alice's) half of shared secret
	
    // MSG 2-1: LDC->Cl (LDC send message)
	memcpy(msg, ctFromLDC, CRYPTO_CIPHERTEXTBYTES);
	// MSG 2-2: LDC->Cl (LDC send public key)
	memcpy(&msg[CRYPTO_CIPHERTEXTBYTES], pubFromLDC, CRYPTO_PUBLICKEYBYTES);

	return CRYPTO_CIPHERTEXTBYTES+ CRYPTO_PUBLICKEYBYTES;
}

int
LDCkeysM2(SIKE_DH *keys, unsigned char *ctToLDC)
{

	int ret = 0;
	crypto_kem_dec_SIKEp751(keys->LDCsecret, ctToLDC, keys->LDC_sike_sk);
	return ret;
}

#if 0
int
TestKeys()
{
	SIKE_DH *keys_client, *keys_ldc;
	unsigned char msg[4096], msg1[4096];
	unsigned char ldcSecret[CRYPTO_BYTES], clientSecret[CRYPTO_BYTES];

	keys_client = (SIKE_DH *)malloc(sizeof(SIKE_DH));

	if (keys_client == NULL)
	{
		return -1;
	}


	keys_ldc = (SIKE_DH *)malloc(sizeof(SIKE_DH));

	if (keys_ldc == NULL)
	{
		return -1;
	}

	// Client creates privat and public keys.
	ClientKeysInit(keys_client);
	// Client Sends LDC its public key. LDC Generates its reply in msg.
	LDCkeysM1(keys_ldc, keys_client->sikepublic, msg);
	// LDC reply sent back to client. Client sends create encrypted msg1.
	ClientKeys(keys_client, msg, msg1);
	// Client sends encrypted msg1.
	LDCkeysM2(keys_ldc, msg1);

	memcpy(clientSecret, keys_client->Clientsecret, CRYPTO_BYTES);
	memcpy(ldcSecret, keys_ldc->LDCsecret, CRYPTO_BYTES);

	if (!memcmp(ldcSecret, clientSecret, CRYPTO_BYTES))
	{
		printf("\nKey Success....");
	}

	return 0;

}
#endif

