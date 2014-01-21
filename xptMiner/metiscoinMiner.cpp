#include"global.h"

void metiscoin_process(minerMetiscoinBlock_t* block)
{
	sph_keccak512_context	 ctx_keccak;
	sph_shavite512_context	 ctx_shavite;
	sph_metis512_context	 ctx_metis;
	static unsigned char pblank[1];
	block->nonce = 0;

	uint32 target = *(uint32*)(block->targetShare+28);
	uint64 hash0[8];
	uint64 hash1[8];
	uint64 hash2[8];
	for(uint32 n=0; n<0x1000; n++)
	{
		if( block->height != monitorCurrentBlockHeight )
			break;
		for(uint32 f=0; f<0x8000; f++)
		{
			sph_keccak512_init(&ctx_keccak);
			sph_shavite512_init(&ctx_shavite);
			sph_metis512_init(&ctx_metis);
			sph_keccak512(&ctx_keccak, &block->version, 80);
			sph_keccak512_close(&ctx_keccak, hash0);
			sph_shavite512(&ctx_shavite, hash0, 64);
			sph_shavite512_close(&ctx_shavite, hash1);
			sph_metis512(&ctx_metis, hash1, 64);
			sph_metis512_close(&ctx_metis, hash2);
			if( *(uint32*)((uint8*)hash2+28) <= target )
			{
				totalShareCount++;
				xptMiner_submitShare(block);
			}
			block->nonce++;
		}
		totalCollisionCount += 0x8000;
	}
}