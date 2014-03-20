#include"global.h"

#define zeroesBeforeHashInPrime	8

const uint32 riecoin_primeTestLimit = 10000000;
uint32 riecoin_primorialIndex = 41;
uint32* riecoin_primeTestTable;
uint32 riecoin_primeTestSize;
uint32* riecoin_primeInverseTable;

uint32 riecoin_sieveSize = 1024*1024*4; // must be divisible by 8 (maximum sieve size)
mpz_t  z_primorial;

void riecoin_sieveTest()
{
	return;
	sint32 sieveSize = 1024*1024;
	uint8* sieve = (uint8*)malloc((sieveSize+7)/8);
	memset(sieve, 0x00, (sieveSize+7)/8);

	// start sieving
	sint32 primesToSieve = 35+10;
	for(sint32 i=0; i<primesToSieve; i++)
	{
		sint32 p = riecoin_primeTestTable[i];
		static sint32 tupleBias[6] = {0,4,6,10,12,16};
		for(sint32 f=0; f<6; f++)
		{
			sint32 index = -tupleBias[f];
			while( index < 0 ) index += p;
			while( index < sieveSize )
			{
				sieve[index>>3] |= (1<<(index&7));
				index += p;
			}
		}
	}
	for(sint32 i=0; i<1024*512; i++)
	{
		if( sieve[i>>3] & (1<<(i&7)) )
			continue;
		if( (i%210) == 97 )
			printf("c[x]: %d\n", i);
		else
			printf("c[ ]: %d\n", i);
	}
}

void riecoin_init()
{
	printf("Generating table of small primes for Riecoin...\n");
	// generate prime table
	riecoin_primeTestTable = (uint32*)malloc(sizeof(uint32)*(riecoin_primeTestLimit/4+10));
	riecoin_primeTestSize = 0;
	// generate prime table using Sieve of Eratosthenes
	uint8* vfComposite = (uint8*)malloc(sizeof(uint8)*(riecoin_primeTestLimit+7)/8);
	memset(vfComposite, 0x00, sizeof(uint8)*(riecoin_primeTestLimit+7)/8);
	for (unsigned int nFactor = 2; nFactor * nFactor < riecoin_primeTestLimit; nFactor++)
	{
		if( vfComposite[nFactor>>3] & (1<<(nFactor&7)) )
			continue;
		for (unsigned int nComposite = nFactor * nFactor; nComposite < riecoin_primeTestLimit; nComposite += nFactor)
			vfComposite[nComposite>>3] |= 1<<(nComposite&7);
	}
	for (unsigned int n = 2; n < riecoin_primeTestLimit; n++)
	{
		if ( (vfComposite[n>>3] & (1<<(n&7)))==0 )
		{
			riecoin_primeTestTable[riecoin_primeTestSize] = n;
			riecoin_primeTestSize++;
		}
	}
	riecoin_primeTestTable = (uint32*)realloc(riecoin_primeTestTable, sizeof(uint32)*riecoin_primeTestSize);
	free(vfComposite);
	printf("Table with %d entries generated\n", riecoin_primeTestSize);
	// make sure sieve size is divisible by 8
	riecoin_sieveSize = (riecoin_sieveSize&~7);
	// generate primorial for 40
	mpz_init_set_ui(z_primorial, riecoin_primeTestTable[0]);
	for(uint32 i=1; i<riecoin_primorialIndex; i++)
	{
		mpz_mul_ui(z_primorial, z_primorial, riecoin_primeTestTable[i]);
	}
	// allocate space for table of inverse primes
	riecoin_primeInverseTable = (uint32*)malloc(sizeof(uint32)*riecoin_primeTestSize);
	memset(riecoin_primeInverseTable, 0x00, sizeof(uint32)*riecoin_primeTestSize);
	// calculate inverse of each p primorial pair
	mpz_t z_inv;
	mpz_init(z_inv);
	mpz_t z_p;
	mpz_init(z_p);
	for(uint32 i=riecoin_primorialIndex; i<riecoin_primeTestSize; i++)
	{
		mpz_set_ui(z_p, riecoin_primeTestTable[i]);
		if( mpz_invert(z_inv, z_primorial, z_p) )
		{
			riecoin_primeInverseTable[i] = mpz_get_ui(z_inv);
		}
	}
	mpz_clear(z_p);
	mpz_clear(z_inv);
}

__declspec( thread ) uint8* riecoin_sieve = NULL;

uint32 _getHexDigitValue(uint8 c)
{
	if( c >= '0' && c <= '9' )
		return c-'0';
	else if( c >= 'a' && c <= 'f' )
		return c-'a'+10;
	else if( c >= 'A' && c <= 'F' )
		return c-'A'+10;
	return 0;
}

/*
 * Parses a hex string
 * Length should be a multiple of 2
 */
void debug_parseHexString(char* hexString, uint32 length, uint8* output)
{
	uint32 lengthBytes = length / 2;
	for(uint32 i=0; i<lengthBytes; i++)
	{
		// high digit
		uint32 d1 = _getHexDigitValue(hexString[i*2+0]);
		// low digit
		uint32 d2 = _getHexDigitValue(hexString[i*2+1]);
		// build byte
		output[i] = (uint8)((d1<<4)|(d2));	
	}
}

void debug_parseHexStringLE(char* hexString, uint32 length, uint8* output)
{
	uint32 lengthBytes = length / 2;
	for(uint32 i=0; i<lengthBytes; i++)
	{
		// high digit
		uint32 d1 = _getHexDigitValue(hexString[i*2+0]);
		// low digit
		uint32 d2 = _getHexDigitValue(hexString[i*2+1]);
		// build byte
		output[lengthBytes-i-1] = (uint8)((d1<<4)|(d2));	
	}
}

unsigned int int_invert(unsigned int a, unsigned int nPrime)
{
	// Extended Euclidean algorithm to calculate the inverse of a in finite field defined by nPrime
	int rem0 = nPrime, rem1 = a % nPrime, rem2;
	int aux0 = 0, aux1 = 1, aux2;
	int quotient, inverse;

	while (1)
	{
		if (rem1 <= 1)
		{
			inverse = aux1;
			break;
		}

		rem2 = rem0 % rem1;
		quotient = rem0 / rem1;
		aux2 = -quotient * aux1 + aux0;

		if (rem2 <= 1)
		{
			inverse = aux2;
			break;
		}

		rem0 = rem1 % rem2;
		quotient = rem1 / rem2;
		aux0 = -quotient * aux2 + aux1;

		if (rem0 <= 1)
		{
			inverse = aux0;
			break;
		}

		rem1 = rem2 % rem0;
		quotient = rem2 / rem0;
		aux1 = -quotient * aux0 + aux2;
	}

	return (inverse + nPrime) % nPrime;
}

void riecoin_process(minerRiecoinBlock_t* block, sint32 threadIndex)
{
	uint32 searchBits = block->targetCompact;

	if( riecoin_sieve )
		memset(riecoin_sieve, 0x00, riecoin_sieveSize/8);
	else
	{
		riecoin_sieve = (uint8*)malloc(riecoin_sieveSize/8);
		memset(riecoin_sieve, 0x00, riecoin_sieveSize/8);
	}
	uint8* sieve = riecoin_sieve;
	uint32 primesToSieve = riecoin_primeTestSize;
	uint32 sieveSize = riecoin_sieveSize;

	// test data
	// getblock 16ee31c116b75d0299dc03cab2b6cbcb885aa29adf292b2697625bc9d28b2b64
	//debug_parseHexStringLE("c59ba5357285de73b878fed43039a37f85887c8960e66bcb6e86bdad565924bd", 64, block->merkleRoot);
	//block->version = 2;
	//debug_parseHexStringLE("c64673c670fb327c2e009b3b626d2def01d51ad4131a7a1040e9cef7bfa34838", 64, block->prevBlockHash);
	//block->nTime = 1392151955;
	//block->nBits = 0x02013000;
	//debug_parseHexStringLE("0000000000000000000000000000000000000000000000000000000070b67515", 64, block->nOffset);
	// generate PoW hash (version to nBits)
	uint8 powHash[32];
	sha256_ctx ctx;
	sha256_init(&ctx);
	sha256_update(&ctx, (uint8*)block, 80);
	sha256_final(&ctx, powHash);
	sha256_init(&ctx);
	sha256_update(&ctx, powHash, 32);
	sha256_final(&ctx, powHash);
	// generatePrimeBase
	mpz_t z_remainder;
	mpz_init(z_remainder);
	uint32* powHashU32 = (uint32*)powHash;
	mpz_t z_target;
	mpz_t z_temp;
	mpz_init(z_temp);
	mpz_init_set_ui(z_target, 1);
	mpz_mul_2exp(z_target, z_target, zeroesBeforeHashInPrime);
	for(uint32 i=0; i<256; i++)
	{
		mpz_mul_2exp(z_target, z_target, 1);
		if( (powHashU32[i/32]>>(i))&1 )
			z_target->_mp_d[0]++;
	}
	unsigned int trailingZeros = searchBits - 1 - zeroesBeforeHashInPrime - 256;
	mpz_mul_2exp(z_target, z_target, trailingZeros);
	// find first offset where x%primorial = 97
	mpz_tdiv_r(z_remainder, z_target, z_primorial);
	mpz_sub(z_remainder, z_primorial, z_remainder);
	if( mpz_cmp(z_remainder, z_primorial) == 0 )
		mpz_set_ui(z_remainder, 0);
	mpz_add_ui(z_remainder, z_remainder, 16057);
	// some other possible offsets:
	//16057
	//19417
	//43777
	//340927
	//402037
	//419047
	//420307
	//460627
	mpz_add(z_temp, z_target, z_remainder);

	mpz_t z_temp2;
	mpz_init(z_temp2);
	mpz_t z_ft_r;
	mpz_init(z_ft_r);
	mpz_t z_ft_b;
	mpz_init_set_ui(z_ft_b, 2);
	mpz_t z_ft_n;
	mpz_init(z_ft_n);

	static uint32 primeTupleBias[6] = {0,4,6,10,12,16};
	for(uint32 i=riecoin_primorialIndex; i<primesToSieve; i++)
	{
		if( block->height != monitorCurrentBlockHeight )
			break;
		sint64 p = riecoin_primeTestTable[i];
		sint32 pRemainder = mpz_tdiv_ui(z_temp, p);
		sint64 remainder = pRemainder;
		sint64 pa;
		// p1
		remainder += 0;
		remainder %= p;
		pa = (p<remainder)?(p-remainder+p):(p-remainder);
		sint64 index1 = ((sint64)pa*(sint64)riecoin_primeInverseTable[i])%p;
		// p2
		remainder += 4;
		remainder %= p;
		pa = (p<remainder)?(p-remainder+p):(p-remainder);
		sint64 index2 = ((sint64)pa*(sint64)riecoin_primeInverseTable[i])%p;
		// p3
		remainder += 2;
		remainder %= p;
		pa = (p<remainder)?(p-remainder+p):(p-remainder);
		sint64 index3 = ((sint64)pa*(sint64)riecoin_primeInverseTable[i])%p;
		// p4
		remainder += 4;
		remainder %= p;
		pa = (p<remainder)?(p-remainder+p):(p-remainder);
		sint64 index4 = ((sint64)pa*(sint64)riecoin_primeInverseTable[i])%p;
		// p5
		remainder += 2;
		remainder %= p;
		pa = (p<remainder)?(p-remainder+p):(p-remainder);
		sint64 index5 = ((sint64)pa*(sint64)riecoin_primeInverseTable[i])%p;
		// p6
		remainder += 4;
		remainder %= p;
		pa = (p<remainder)?(p-remainder+p):(p-remainder);
		sint64 index6 = ((sint64)pa*(sint64)riecoin_primeInverseTable[i])%p;

		for(uint32 f=0; f<6; f++)
			{
				sint64 remainder = pRemainder;
				remainder += primeTupleBias[f];
				remainder %= p;
				sint64 index;
				// a+b*x=0 (mod p) => b*x=p-a => x = (p-a)*modinv(b)
				sint64 pa = (p<remainder)?(p-remainder+p):(p-remainder);
				index = ((sint64)pa*(sint64)riecoin_primeInverseTable[i])%p;
				while(index < riecoin_sieveSize)
				{
					sieve[(index)>>3] |= (1<<((index)&7));
					index += p;
				}
			}		
	}
	if( block->height != monitorCurrentBlockHeight )
		return;
	// scan for candidates
	for(uint32 i=1; i<sieveSize; i++)
	{
		if( sieve[(i)>>3] & (1<<((i)&7)) )
			continue;
		if( block->height != monitorCurrentBlockHeight )
			break;
		// we use fermat test as it is slightly faster for virtually the same accuracy
		mpz_add(z_temp, z_target, z_remainder);
		mpz_addmul_ui(z_temp, z_primorial, i);
		// p1
		sint32 validPrimes = 0;
		bool allPrimes = true;
		mpz_sub_ui(z_ft_n, z_temp, 1);
		mpz_powm(z_ft_r, z_ft_b, z_ft_n, z_temp);
		if (mpz_cmp_ui(z_ft_r, 1) == 0)
			validPrimes++;
		else
			allPrimes = false;
		// p2
		mpz_add_ui(z_temp, z_temp, 4);
		mpz_sub_ui(z_ft_n, z_temp, 1);
		mpz_powm(z_ft_r, z_ft_b, z_ft_n, z_temp);
		if (mpz_cmp_ui(z_ft_r, 1) == 0)
			validPrimes++;
		else
			allPrimes = false;
		if( allPrimes )
			total2ChainCount++;
		// p3
		mpz_add_ui(z_temp, z_temp, 2);
		mpz_sub_ui(z_ft_n, z_temp, 1);
		mpz_powm(z_ft_r, z_ft_b, z_ft_n, z_temp);
		if (mpz_cmp_ui(z_ft_r, 1) == 0)
			validPrimes++;
		else
			allPrimes = false;
		if( validPrimes < 1 )
			continue;
		if( allPrimes )
			total3ChainCount++;
		// p4
		mpz_add_ui(z_temp, z_temp, 4);
		mpz_sub_ui(z_ft_n, z_temp, 1);
		mpz_powm(z_ft_r, z_ft_b, z_ft_n, z_temp);
		if (mpz_cmp_ui(z_ft_r, 1) == 0)
			validPrimes++;
		else
			allPrimes = false;
		if( validPrimes < 2 )
			continue;	
		if( allPrimes )
			total4ChainCount++;
		// p5
		if( validPrimes < 4 )
		{
			mpz_add_ui(z_temp, z_temp, 2);
			mpz_sub_ui(z_ft_n, z_temp, 1);
			mpz_powm(z_ft_r, z_ft_b, z_ft_n, z_temp);
			if (mpz_cmp_ui(z_ft_r, 1) == 0)
				validPrimes++;
			if( validPrimes < 3 )
				continue;	
		}
		// p6
		if( validPrimes < 4 )
		{
			mpz_add_ui(z_temp, z_temp, 4);
			mpz_sub_ui(z_ft_n, z_temp, 1);
			mpz_powm(z_ft_r, z_ft_b, z_ft_n, z_temp);
			if (mpz_cmp_ui(z_ft_r, 1) == 0)
				validPrimes++;
		}
		if( validPrimes < 4 )
			continue;	
		// calculate offset
		//mpz_add_ui(z_temp, z_target, (uint64)remainder2310 + 2310ULL*(uint64)i);
		mpz_add(z_temp, z_target, z_remainder);
		mpz_addmul_ui(z_temp, z_primorial, i);
		mpz_sub(z_temp2, z_temp, z_target);
		// submit share
		uint8 nOffset[32];
		memset(nOffset, 0x00, 32);
#ifdef _WIN64
		for(uint32 d=0; d<min(32/8, z_temp2->_mp_size); d++)
		{
			*(uint64*)(nOffset+d*8) = z_temp2->_mp_d[d];
		}
#else
		for(uint32 d=0; d<min(32/4, z_temp2->_mp_size); d++)
		{
			*(uint32*)(nOffset+d*4) = z_temp2->_mp_d[d];
		}
#endif
		totalShareCount++;
		xptMiner_submitShare(block, nOffset);
	}
	totalSieveCount++;
	// clean up
	mpz_clear(z_remainder);
	mpz_clear(z_temp);
	mpz_clear(z_target);
	mpz_clear(z_temp2);
	mpz_clear(z_ft_r);
	mpz_clear(z_ft_b);
	mpz_clear(z_ft_n);
}