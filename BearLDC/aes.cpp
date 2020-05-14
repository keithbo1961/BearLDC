// aes.cpp : Defines the entry point for the console application.
//



/*
*  FIPS-197 compliant AES implementation
*
*  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
*  SPDX-License-Identifier: Apache-2.0
*
*  Licensed under the Apache License, Version 2.0 (the "License"); you may
*  not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
*  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*
*  This file is part of mbed TLS (https://tls.mbed.org)
*/
/*
*  The AES block cipher was designed by Vincent Rijmen and Joan Daemen.
*
*  http://csrc.nist.gov/encryption/aes/rijndael/Rijndael.pdf
*  http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf
*/


#include "bearldc.h"

static const unsigned char aes_test_ctr_nonce_counter[3][16] =
{
	{ 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 },
	{ 0x00, 0x6C, 0xB6, 0xDB, 0xC0, 0x54, 0x3B, 0x59,
	0xDA, 0x48, 0xD9, 0x0B, 0x00, 0x00, 0x00, 0x01 },
	{ 0x00, 0xE0, 0x01, 0x7B, 0x27, 0x77, 0x7F, 0x3F,
	0x4A, 0x17, 0x86, 0xF0, 0x00, 0x00, 0x00, 0x01 }
};




/* Implementation that should never be optimized out by the compiler */
static void mbedtls_zeroize(void *v, size_t n) {
	volatile unsigned char *p = (unsigned char*)v; while (n--) *p++ = 0;
}

/*
* 32-bit integer manipulation macros (little endian)
*/

#define GET_UINT32_LE(n,b,i)                            \
{                                                       \
    (n) = ( (uint32_t) (b)[(i)    ]       )             \
        | ( (uint32_t) (b)[(i) + 1] <<  8 )             \
        | ( (uint32_t) (b)[(i) + 2] << 16 )             \
        | ( (uint32_t) (b)[(i) + 3] << 24 );            \
}



#define PUT_UINT32_LE(n,b,i)                                    \
{                                                               \
    (b)[(i)    ] = (unsigned char) ( ( (n)       ) & 0xFF );    \
    (b)[(i) + 1] = (unsigned char) ( ( (n) >>  8 ) & 0xFF );    \
    (b)[(i) + 2] = (unsigned char) ( ( (n) >> 16 ) & 0xFF );    \
    (b)[(i) + 3] = (unsigned char) ( ( (n) >> 24 ) & 0xFF );    \
}





/*
* Forward S-box & tables
*/
static unsigned char FSb[256];
static uint32_t FT0[256];
static uint32_t FT1[256];
static uint32_t FT2[256];
static uint32_t FT3[256];

/*
* Reverse S-box & tables
*/
static unsigned char RSb[256];
static uint32_t RT0[256];
static uint32_t RT1[256];
static uint32_t RT2[256];
static uint32_t RT3[256];

/*
* Round constants
*/
static uint32_t RCON[10];

/*
* Tables generation code
*/
#define ROTL8(x) ( ( x << 8 ) & 0xFFFFFFFF ) | ( x >> 24 )
#define XTIME(x) ( ( x << 1 ) ^ ( ( x & 0x80 ) ? 0x1B : 0x00 ) )
#define MUL(x,y) ( ( x && y ) ? pow[(log[x]+log[y]) % 255] : 0 )

static int aes_init_done = 0;

static void aes_gen_tables(void)
{
	int i, x, y, z;
	int pow[256];
	int log[256];

	/*
	* compute pow and log tables over GF(2^8)
	*/
	for (i = 0, x = 1; i < 256; i++)
	{
		pow[i] = x;
		log[x] = i;
		x = (x ^ XTIME(x)) & 0xFF;
	}

	/*
	* calculate the round constants
	*/
	for (i = 0, x = 1; i < 10; i++)
	{
		RCON[i] = (uint32_t)x;
		x = XTIME(x) & 0xFF;
	}

	/*
	* generate the forward and reverse S-boxes
	*/
	FSb[0x00] = 0x63;
	RSb[0x63] = 0x00;

	for (i = 1; i < 256; i++)
	{
		x = pow[255 - log[i]];

		y = x; y = ((y << 1) | (y >> 7)) & 0xFF;
		x ^= y; y = ((y << 1) | (y >> 7)) & 0xFF;
		x ^= y; y = ((y << 1) | (y >> 7)) & 0xFF;
		x ^= y; y = ((y << 1) | (y >> 7)) & 0xFF;
		x ^= y ^ 0x63;

		FSb[i] = (unsigned char)x;
		RSb[x] = (unsigned char)i;
	}

	/*
	* generate the forward and reverse tables
	*/
	for (i = 0; i < 256; i++)
	{
		x = FSb[i];
		y = XTIME(x) & 0xFF;
		z = (y ^ x) & 0xFF;

		FT0[i] = ((uint32_t)y) ^
			((uint32_t)x << 8) ^
			((uint32_t)x << 16) ^
			((uint32_t)z << 24);

		FT1[i] = ROTL8(FT0[i]);
		FT2[i] = ROTL8(FT1[i]);
		FT3[i] = ROTL8(FT2[i]);

		x = RSb[i];

		RT0[i] = ((uint32_t)MUL(0x0E, x)) ^
			((uint32_t)MUL(0x09, x) << 8) ^
			((uint32_t)MUL(0x0D, x) << 16) ^
			((uint32_t)MUL(0x0B, x) << 24);

		RT1[i] = ROTL8(RT0[i]);
		RT2[i] = ROTL8(RT1[i]);
		RT3[i] = ROTL8(RT2[i]);
	}
}



void mbedtls_aes_init(mbedtls_aes_context *ctx)
{
	memset(ctx, 0, sizeof(mbedtls_aes_context));
}

void mbedtls_aes_free(mbedtls_aes_context *ctx)
{
	if (ctx == NULL)
		return;

	mbedtls_zeroize(ctx, sizeof(mbedtls_aes_context));
}

/*
* AES key schedule (encryption)
*/

int mbedtls_aes_setkey_enc(mbedtls_aes_context *ctx, const unsigned char *key,
	unsigned int keybits)
{
	unsigned int i;
	uint32_t *RK;

	if (aes_init_done == 0)
	{
		aes_gen_tables();
		aes_init_done = 1;

	}

	switch (keybits)
	{
	case 128: ctx->nr = 10; break;
	case 192: ctx->nr = 12; break;
	case 256: ctx->nr = 14; break;
	default: return(MBEDTLS_ERR_AES_INVALID_KEY_LENGTH);
	}

	ctx->rk = RK = ctx->buf;

	for (i = 0; i < (keybits >> 5); i++)
	{
		GET_UINT32_LE(RK[i], key, i << 2);
	}

	switch (ctx->nr)
	{
	case 10:

		for (i = 0; i < 10; i++, RK += 4)
		{
			RK[4] = RK[0] ^ RCON[i] ^
				((uint32_t)FSb[(RK[3] >> 8) & 0xFF]) ^
				((uint32_t)FSb[(RK[3] >> 16) & 0xFF] << 8) ^
				((uint32_t)FSb[(RK[3] >> 24) & 0xFF] << 16) ^
				((uint32_t)FSb[(RK[3]) & 0xFF] << 24);

			RK[5] = RK[1] ^ RK[4];
			RK[6] = RK[2] ^ RK[5];
			RK[7] = RK[3] ^ RK[6];
		}
		break;

	case 12:

		for (i = 0; i < 8; i++, RK += 6)
		{
			RK[6] = RK[0] ^ RCON[i] ^
				((uint32_t)FSb[(RK[5] >> 8) & 0xFF]) ^
				((uint32_t)FSb[(RK[5] >> 16) & 0xFF] << 8) ^
				((uint32_t)FSb[(RK[5] >> 24) & 0xFF] << 16) ^
				((uint32_t)FSb[(RK[5]) & 0xFF] << 24);

			RK[7] = RK[1] ^ RK[6];
			RK[8] = RK[2] ^ RK[7];
			RK[9] = RK[3] ^ RK[8];
			RK[10] = RK[4] ^ RK[9];
			RK[11] = RK[5] ^ RK[10];
		}
		break;

	case 14:

		for (i = 0; i < 7; i++, RK += 8)
		{
			RK[8] = RK[0] ^ RCON[i] ^
				((uint32_t)FSb[(RK[7] >> 8) & 0xFF]) ^
				((uint32_t)FSb[(RK[7] >> 16) & 0xFF] << 8) ^
				((uint32_t)FSb[(RK[7] >> 24) & 0xFF] << 16) ^
				((uint32_t)FSb[(RK[7]) & 0xFF] << 24);

			RK[9] = RK[1] ^ RK[8];
			RK[10] = RK[2] ^ RK[9];
			RK[11] = RK[3] ^ RK[10];

			RK[12] = RK[4] ^
				((uint32_t)FSb[(RK[11]) & 0xFF]) ^
				((uint32_t)FSb[(RK[11] >> 8) & 0xFF] << 8) ^
				((uint32_t)FSb[(RK[11] >> 16) & 0xFF] << 16) ^
				((uint32_t)FSb[(RK[11] >> 24) & 0xFF] << 24);

			RK[13] = RK[5] ^ RK[12];
			RK[14] = RK[6] ^ RK[13];
			RK[15] = RK[7] ^ RK[14];
		}
		break;
	}

	return(0);
}


#define AES_FROUND(X0,X1,X2,X3,Y0,Y1,Y2,Y3)     \
{                                               \
    X0 = *RK++ ^ FT0[ ( Y0       ) & 0xFF ] ^   \
                 FT1[ ( Y1 >>  8 ) & 0xFF ] ^   \
                 FT2[ ( Y2 >> 16 ) & 0xFF ] ^   \
                 FT3[ ( Y3 >> 24 ) & 0xFF ];    \
                                                \
    X1 = *RK++ ^ FT0[ ( Y1       ) & 0xFF ] ^   \
                 FT1[ ( Y2 >>  8 ) & 0xFF ] ^   \
                 FT2[ ( Y3 >> 16 ) & 0xFF ] ^   \
                 FT3[ ( Y0 >> 24 ) & 0xFF ];    \
                                                \
    X2 = *RK++ ^ FT0[ ( Y2       ) & 0xFF ] ^   \
                 FT1[ ( Y3 >>  8 ) & 0xFF ] ^   \
                 FT2[ ( Y0 >> 16 ) & 0xFF ] ^   \
                 FT3[ ( Y1 >> 24 ) & 0xFF ];    \
                                                \
    X3 = *RK++ ^ FT0[ ( Y3       ) & 0xFF ] ^   \
                 FT1[ ( Y0 >>  8 ) & 0xFF ] ^   \
                 FT2[ ( Y1 >> 16 ) & 0xFF ] ^   \
                 FT3[ ( Y2 >> 24 ) & 0xFF ];    \
}

#define AES_RROUND(X0,X1,X2,X3,Y0,Y1,Y2,Y3)     \
{                                               \
    X0 = *RK++ ^ RT0[ ( Y0       ) & 0xFF ] ^   \
                 RT1[ ( Y3 >>  8 ) & 0xFF ] ^   \
                 RT2[ ( Y2 >> 16 ) & 0xFF ] ^   \
                 RT3[ ( Y1 >> 24 ) & 0xFF ];    \
                                                \
    X1 = *RK++ ^ RT0[ ( Y1       ) & 0xFF ] ^   \
                 RT1[ ( Y0 >>  8 ) & 0xFF ] ^   \
                 RT2[ ( Y3 >> 16 ) & 0xFF ] ^   \
                 RT3[ ( Y2 >> 24 ) & 0xFF ];    \
                                                \
    X2 = *RK++ ^ RT0[ ( Y2       ) & 0xFF ] ^   \
                 RT1[ ( Y1 >>  8 ) & 0xFF ] ^   \
                 RT2[ ( Y0 >> 16 ) & 0xFF ] ^   \
                 RT3[ ( Y3 >> 24 ) & 0xFF ];    \
                                                \
    X3 = *RK++ ^ RT0[ ( Y3       ) & 0xFF ] ^   \
                 RT1[ ( Y2 >>  8 ) & 0xFF ] ^   \
                 RT2[ ( Y1 >> 16 ) & 0xFF ] ^   \
                 RT3[ ( Y0 >> 24 ) & 0xFF ];    \
}

/*
* AES-ECB block encryption
*/

void mbedtls_aes_encrypt(mbedtls_aes_context *ctx,
	const unsigned char input[16],
	unsigned char output[16])
{
	int i;
	uint32_t *RK, X0, X1, X2, X3, Y0, Y1, Y2, Y3;

	RK = ctx->rk;

	GET_UINT32_LE(X0, input, 0); X0 ^= *RK++;
	GET_UINT32_LE(X1, input, 4); X1 ^= *RK++;
	GET_UINT32_LE(X2, input, 8); X2 ^= *RK++;
	GET_UINT32_LE(X3, input, 12); X3 ^= *RK++;

	for (i = (ctx->nr >> 1) - 1; i > 0; i--)
	{
		AES_FROUND(Y0, Y1, Y2, Y3, X0, X1, X2, X3);
		AES_FROUND(X0, X1, X2, X3, Y0, Y1, Y2, Y3);
	}

	AES_FROUND(Y0, Y1, Y2, Y3, X0, X1, X2, X3);

	X0 = *RK++ ^ \
		((uint32_t)FSb[(Y0) & 0xFF]) ^
		((uint32_t)FSb[(Y1 >> 8) & 0xFF] << 8) ^
		((uint32_t)FSb[(Y2 >> 16) & 0xFF] << 16) ^
		((uint32_t)FSb[(Y3 >> 24) & 0xFF] << 24);

	X1 = *RK++ ^ \
		((uint32_t)FSb[(Y1) & 0xFF]) ^
		((uint32_t)FSb[(Y2 >> 8) & 0xFF] << 8) ^
		((uint32_t)FSb[(Y3 >> 16) & 0xFF] << 16) ^
		((uint32_t)FSb[(Y0 >> 24) & 0xFF] << 24);

	X2 = *RK++ ^ \
		((uint32_t)FSb[(Y2) & 0xFF]) ^
		((uint32_t)FSb[(Y3 >> 8) & 0xFF] << 8) ^
		((uint32_t)FSb[(Y0 >> 16) & 0xFF] << 16) ^
		((uint32_t)FSb[(Y1 >> 24) & 0xFF] << 24);

	X3 = *RK++ ^ \
		((uint32_t)FSb[(Y3) & 0xFF]) ^
		((uint32_t)FSb[(Y0 >> 8) & 0xFF] << 8) ^
		((uint32_t)FSb[(Y1 >> 16) & 0xFF] << 16) ^
		((uint32_t)FSb[(Y2 >> 24) & 0xFF] << 24);

	PUT_UINT32_LE(X0, output, 0);
	PUT_UINT32_LE(X1, output, 4);
	PUT_UINT32_LE(X2, output, 8);
	PUT_UINT32_LE(X3, output, 12);
}





/*
* AES-CTR buffer encryption/decryption
*/
int mbedtls_aes_crypt_ctr(mbedtls_aes_context *ctx,
	size_t length,
	size_t *nc_off,
	unsigned char nonce_counter[16],
	unsigned char stream_block[16],
	const unsigned char *input,
	unsigned char *output)
{
	int c, i;
	size_t n = *nc_off;

	while (length--)
	{
		if (n == 0) {

			mbedtls_aes_encrypt(ctx, nonce_counter, stream_block);

			for (i = 16; i > 0; i--)
				if (++nonce_counter[i - 1] != 0)
				{
					break;
				}

		}
		c = *input++;
		*output++ = (unsigned char)(c ^ stream_block[n]);

		n = (n + 1) & 0x0F;
	}

	*nc_off = n;

	return(0);
}

int
MakeEncrpytIPSeed(unsigned char *key, int keylenkey, char * iv, mbedtls_aes_context *ctx)
{

	mbedtls_aes_init(ctx);
	mbedtls_aes_setkey_enc(ctx, key, 256);

	return 0;
}

int
MangleEncryptOut(unsigned char *in, unsigned char *out, int len, mbedtls_aes_context *ctx)
{

	unsigned char input[16];
	unsigned char stream_block[16];
	size_t offset;

	offset = 0;
	memcpy(input, aes_test_ctr_nonce_counter[1], 16);


	mbedtls_aes_crypt_ctr(ctx, len, &offset, input, stream_block, (unsigned char *)in, (unsigned char *)out);


	return len;

}

int
MangleDecryptIn(unsigned char *in, unsigned char *out, int len, mbedtls_aes_context *ctx)
{

	unsigned char input[16];
	unsigned char stream_block[16];
	size_t offset;

	offset = 0;
	memcpy(input, aes_test_ctr_nonce_counter[1], 16);
	mbedtls_aes_crypt_ctr(ctx, len, &offset, input, stream_block, (unsigned char *)in, (unsigned char *)out);


	return len;

}

#if 0
int
TestAES()
{
	char stg[256], stg1[256], stg2[256];
	int len;
	mbedtls_aes_context ctx;
	unsigned char input[16], input1[16], key[32];
	unsigned char stream_block[16], stream_block1[16];
	size_t offset, offset1;

	//mbedtls_aes_self_test(1);
	strcpy_s(stg, "Encrypting Testing. XyzXy XEncrypting Tes HCFGXBH1ekslprhmalkryu;fgfioasuytw[]192834lghEB");
	len = strlen(stg);

	memcpy(input, aes_test_ctr_nonce_counter[1], 16);
	memcpy(input1, aes_test_ctr_nonce_counter[1], 16);
	memcpy(key, "68Hjsk)&8UkL;[)pikmshjexgetwnhts", 32);
	mbedtls_aes_setkey_enc(&ctx, key, 256);

	offset = 0;
	mbedtls_aes_crypt_ctr(&ctx, len, &offset, input, stream_block, (unsigned char *)stg, (unsigned char *)stg1);
	offset = 0;
	mbedtls_aes_crypt_ctr(&ctx, len, &offset, input1, stream_block1, (unsigned char *)stg1, (unsigned char *)stg2);

	return 0;
}
#endif

/*


int main()
{
	char stg[256], stg1[256], stg2[256];
	int len;
	mbedtls_aes_context ctx;
	unsigned char input[16], key[32];
	unsigned char stream_block[16];
	size_t offset;

	//mbedtls_aes_self_test(1);
	strcpy(stg, "Encrypting Testing. XyzXy XEncrypting Tes HCFGXBH1ekslprhmalkryu;fgfioasuytw[]192834lghEB");
	len = strlen(stg);

	memcpy(input, aes_test_ctr_nonce_counter[1], 16);
	memcpy(key, aes_test_ctr_key[0], 32);



	offset = 0;
	mbedtls_aes_setkey_enc(&ctx, key, 256);


	mbedtls_aes_crypt_ctr(&ctx, len, &offset, input, stream_block, (unsigned char *)stg, (unsigned char *)stg1);
	offset = 0;

	memcpy(input, aes_test_ctr_nonce_counter[1], 16);
	memcpy(key, aes_test_ctr_key[0], 16);



	offset = 0;
	mbedtls_aes_setkey_enc(&ctx, key, 256);
	mbedtls_aes_crypt_ctr(&ctx, len, &offset, input, stream_block, (unsigned char *)stg1, (unsigned char *)stg2);

	strncpy(stg1, stg2, len);
	stg1[len] = 0;
	return 0;
}

*/
