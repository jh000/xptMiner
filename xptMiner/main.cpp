#include"global.h"

#define MAX_TRANSACTIONS	(4096)

// miner version string (for pool statistic)
char* minerVersionString = "xptMiner 1.1";

minerSettings_t minerSettings = {0};

xptClient_t* xptClient = NULL;
CRITICAL_SECTION cs_xptClient;
volatile uint32 monitorCurrentBlockHeight; // used to notify worker threads of new block data

struct  
{
	CRITICAL_SECTION cs_work;
	uint32	algorithm;
	// block data
	uint32	version;
	uint32	height;
	uint32	nBits;
	uint32	timeBias;
	uint8	merkleRootOriginal[32]; // used to identify work
	uint8	prevBlockHash[32];
	uint8	target[32];
	uint8	targetShare[32];
	// extra nonce info
	uint8	coinBase1[1024];
	uint8	coinBase2[1024];
	uint16	coinBase1Size;
	uint16	coinBase2Size;
	// transaction hashes
	uint8	txHash[32*MAX_TRANSACTIONS];
	uint32	txHashCount;
}workDataSource;

uint32 uniqueMerkleSeedGenerator = 0;
uint32 miningStartTime = 0;

/*
 * Submit Protoshares share
 */
void xptMiner_submitShare(minerProtosharesBlock_t* block)
{
	printf("Share found! (Blockheight: %d)\n", block->height);
	EnterCriticalSection(&cs_xptClient);
	if( xptClient == NULL || xptClient_isDisconnected(xptClient, NULL) == true )
	{
		printf("Share submission failed - No connection to server\n");
		LeaveCriticalSection(&cs_xptClient);
		return;
	}
	// submit block
	xptShareToSubmit_t* xptShare = (xptShareToSubmit_t*)malloc(sizeof(xptShareToSubmit_t));
	memset(xptShare, 0x00, sizeof(xptShareToSubmit_t));
	xptShare->algorithm = ALGORITHM_PROTOSHARES;
	xptShare->version = block->version;
	xptShare->nTime = block->nTime;
	xptShare->nonce = block->nonce;
	xptShare->nBits = block->nBits;
	xptShare->nBirthdayA = block->birthdayA;
	xptShare->nBirthdayB = block->birthdayB;
	memcpy(xptShare->prevBlockHash, block->prevBlockHash, 32);
	memcpy(xptShare->merkleRoot, block->merkleRoot, 32);
	memcpy(xptShare->merkleRootOriginal, block->merkleRootOriginal, 32);
	//userExtraNonceLength = min(userExtraNonceLength, 16);
	sint32 userExtraNonceLength = sizeof(uint32);
	uint8* userExtraNonceData = (uint8*)&block->uniqueMerkleSeed;
	xptShare->userExtraNonceLength = userExtraNonceLength;
	memcpy(xptShare->userExtraNonceData, userExtraNonceData, userExtraNonceLength);
	xptClient_foundShare(xptClient, xptShare);
	LeaveCriticalSection(&cs_xptClient);
}

/*
 * Submit Scrypt share
 */
void xptMiner_submitShare(minerScryptBlock_t* block)
{
	printf("Share found! (Blockheight: %d)\n", block->height);
	EnterCriticalSection(&cs_xptClient);
	if( xptClient == NULL || xptClient_isDisconnected(xptClient, NULL) == true )
	{
		printf("Share submission failed - No connection to server\n");
		LeaveCriticalSection(&cs_xptClient);
		return;
	}
	// submit block
	xptShareToSubmit_t* xptShare = (xptShareToSubmit_t*)malloc(sizeof(xptShareToSubmit_t));
	memset(xptShare, 0x00, sizeof(xptShareToSubmit_t));
	xptShare->algorithm = ALGORITHM_SCRYPT;
	xptShare->version = block->version;
	xptShare->nTime = block->nTime;
	xptShare->nonce = block->nonce;
	xptShare->nBits = block->nBits;
	memcpy(xptShare->prevBlockHash, block->prevBlockHash, 32);
	memcpy(xptShare->merkleRoot, block->merkleRoot, 32);
	memcpy(xptShare->merkleRootOriginal, block->merkleRootOriginal, 32);
	//userExtraNonceLength = min(userExtraNonceLength, 16);
	sint32 userExtraNonceLength = sizeof(uint32);
	uint8* userExtraNonceData = (uint8*)&block->uniqueMerkleSeed;
	xptShare->userExtraNonceLength = userExtraNonceLength;
	memcpy(xptShare->userExtraNonceData, userExtraNonceData, userExtraNonceLength);
	xptClient_foundShare(xptClient, xptShare);
	LeaveCriticalSection(&cs_xptClient);
}

/*
 * Submit Primecoin share
 */
void xptMiner_submitShare(minerPrimecoinBlock_t* block)
{
	printf("Share found! (Blockheight: %d)\n", block->height);
	EnterCriticalSection(&cs_xptClient);
	if( xptClient == NULL || xptClient_isDisconnected(xptClient, NULL) == true )
	{
		printf("Share submission failed - No connection to server\n");
		LeaveCriticalSection(&cs_xptClient);
		return;
	}
	// submit block
	xptShareToSubmit_t* xptShare = (xptShareToSubmit_t*)malloc(sizeof(xptShareToSubmit_t));
	memset(xptShare, 0x00, sizeof(xptShareToSubmit_t));
	xptShare->algorithm = ALGORITHM_PRIME;
	xptShare->version = block->version;
	xptShare->nTime = block->nTime;
	xptShare->nonce = block->nonce;
	xptShare->nBits = block->nBits;
	memcpy(xptShare->prevBlockHash, block->prevBlockHash, 32);
	memcpy(xptShare->merkleRoot, block->merkleRoot, 32);
	memcpy(xptShare->merkleRootOriginal, block->merkleRootOriginal, 32);
	//userExtraNonceLength = min(userExtraNonceLength, 16);
	sint32 userExtraNonceLength = sizeof(uint32);
	uint8* userExtraNonceData = (uint8*)&block->uniqueMerkleSeed;
	xptShare->userExtraNonceLength = userExtraNonceLength;
	memcpy(xptShare->userExtraNonceData, userExtraNonceData, userExtraNonceLength);
	__debugbreak(); // xpm submission still todo
	xptClient_foundShare(xptClient, xptShare);
	LeaveCriticalSection(&cs_xptClient);
}

/*
 * Submit Metiscoin share
 */
void xptMiner_submitShare(minerMetiscoinBlock_t* block)
{
	printf("Share found! (Blockheight: %d)\n", block->height);
	EnterCriticalSection(&cs_xptClient);
	if( xptClient == NULL || xptClient_isDisconnected(xptClient, NULL) == true )
	{
		printf("Share submission failed - No connection to server\n");
		LeaveCriticalSection(&cs_xptClient);
		return;
	}
	// submit block
	xptShareToSubmit_t* xptShare = (xptShareToSubmit_t*)malloc(sizeof(xptShareToSubmit_t));
	memset(xptShare, 0x00, sizeof(xptShareToSubmit_t));
	xptShare->algorithm = ALGORITHM_METISCOIN;
	xptShare->version = block->version;
	xptShare->nTime = block->nTime;
	xptShare->nonce = block->nonce;
	xptShare->nBits = block->nBits;
	memcpy(xptShare->prevBlockHash, block->prevBlockHash, 32);
	memcpy(xptShare->merkleRoot, block->merkleRoot, 32);
	memcpy(xptShare->merkleRootOriginal, block->merkleRootOriginal, 32);
	sint32 userExtraNonceLength = sizeof(uint32);
	uint8* userExtraNonceData = (uint8*)&block->uniqueMerkleSeed;
	xptShare->userExtraNonceLength = userExtraNonceLength;
	memcpy(xptShare->userExtraNonceData, userExtraNonceData, userExtraNonceLength);
	xptClient_foundShare(xptClient, xptShare);
	LeaveCriticalSection(&cs_xptClient);
}

int xptMiner_minerThread(int threadIndex)
{
	// local work data
	minerProtosharesBlock_t minerProtosharesBlock = {0};
	minerScryptBlock_t minerScryptBlock = {0};
	minerMetiscoinBlock_t minerMetiscoinBlock = {0};
	minerPrimecoinBlock_t minerPrimecoinBlock = {0}; 
	// todo: Eventually move all block structures into a union to save stack size
	while( true )
	{
		// has work?
		bool hasValidWork = false;
		EnterCriticalSection(&workDataSource.cs_work);
		if( workDataSource.height > 0 )
		{
			if( workDataSource.algorithm == ALGORITHM_PROTOSHARES )
			{
				// get protoshares work data
				minerProtosharesBlock.version = workDataSource.version;
				minerProtosharesBlock.nTime = (uint32)time(NULL) + workDataSource.timeBias;
				minerProtosharesBlock.nBits = workDataSource.nBits;
				minerProtosharesBlock.nonce = 0;
				minerProtosharesBlock.height = workDataSource.height;
				memcpy(minerProtosharesBlock.merkleRootOriginal, workDataSource.merkleRootOriginal, 32);
				memcpy(minerProtosharesBlock.prevBlockHash, workDataSource.prevBlockHash, 32);
				memcpy(minerProtosharesBlock.targetShare, workDataSource.targetShare, 32);
				minerProtosharesBlock.uniqueMerkleSeed = uniqueMerkleSeedGenerator;
				uniqueMerkleSeedGenerator++;
				// generate merkle root transaction
				bitclient_generateTxHash(sizeof(uint32), (uint8*)&minerProtosharesBlock.uniqueMerkleSeed, workDataSource.coinBase1Size, workDataSource.coinBase1, workDataSource.coinBase2Size, workDataSource.coinBase2, workDataSource.txHash);
				bitclient_calculateMerkleRoot(workDataSource.txHash, workDataSource.txHashCount+1, minerProtosharesBlock.merkleRoot);
				hasValidWork = true;
			}
			else if( workDataSource.algorithm == ALGORITHM_SCRYPT )
			{
				// get scrypt work data
				minerScryptBlock.version = workDataSource.version;
				minerScryptBlock.nTime = (uint32)time(NULL) + workDataSource.timeBias;
				minerScryptBlock.nBits = workDataSource.nBits;
				minerScryptBlock.nonce = 0;
				minerScryptBlock.height = workDataSource.height;
				memcpy(minerScryptBlock.merkleRootOriginal, workDataSource.merkleRootOriginal, 32);
				memcpy(minerScryptBlock.prevBlockHash, workDataSource.prevBlockHash, 32);
				memcpy(minerScryptBlock.targetShare, workDataSource.targetShare, 32);
				minerScryptBlock.uniqueMerkleSeed = uniqueMerkleSeedGenerator;
				uniqueMerkleSeedGenerator++;
				// generate merkle root transaction
				bitclient_generateTxHash(sizeof(uint32), (uint8*)&minerScryptBlock.uniqueMerkleSeed, workDataSource.coinBase1Size, workDataSource.coinBase1, workDataSource.coinBase2Size, workDataSource.coinBase2, workDataSource.txHash);
				bitclient_calculateMerkleRoot(workDataSource.txHash, workDataSource.txHashCount+1, minerScryptBlock.merkleRoot);
				hasValidWork = true;
			}
			else if( workDataSource.algorithm == ALGORITHM_METISCOIN )
			{
				// get metiscoin work data
				minerMetiscoinBlock.version = workDataSource.version;
				minerMetiscoinBlock.nTime = (uint32)time(NULL) + workDataSource.timeBias;
				minerMetiscoinBlock.nBits = workDataSource.nBits;
				minerMetiscoinBlock.nonce = 0;
				minerMetiscoinBlock.height = workDataSource.height;
				memcpy(minerMetiscoinBlock.merkleRootOriginal, workDataSource.merkleRootOriginal, 32);
				memcpy(minerMetiscoinBlock.prevBlockHash, workDataSource.prevBlockHash, 32);
				memcpy(minerMetiscoinBlock.targetShare, workDataSource.targetShare, 32);
				minerMetiscoinBlock.uniqueMerkleSeed = uniqueMerkleSeedGenerator;
				uniqueMerkleSeedGenerator++;
				// generate merkle root transaction
				bitclient_generateTxHash(sizeof(uint32), (uint8*)&minerMetiscoinBlock.uniqueMerkleSeed, workDataSource.coinBase1Size, workDataSource.coinBase1, workDataSource.coinBase2Size, workDataSource.coinBase2, workDataSource.txHash);
				bitclient_calculateMerkleRoot(workDataSource.txHash, workDataSource.txHashCount+1, minerMetiscoinBlock.merkleRoot);
				hasValidWork = true;
			}
			else if( workDataSource.algorithm == ALGORITHM_PRIME )
			{
				// get primecoin work data
				minerPrimecoinBlock.version = workDataSource.version;
				minerPrimecoinBlock.nTime = (uint32)time(NULL) + workDataSource.timeBias;
				minerPrimecoinBlock.nBits = workDataSource.nBits;
				minerPrimecoinBlock.nonce = 0;
				minerPrimecoinBlock.height = workDataSource.height;
				memcpy(minerPrimecoinBlock.merkleRootOriginal, workDataSource.merkleRootOriginal, 32);
				memcpy(minerPrimecoinBlock.prevBlockHash, workDataSource.prevBlockHash, 32);
				memcpy(minerPrimecoinBlock.targetShare, workDataSource.targetShare, 32);
				minerPrimecoinBlock.uniqueMerkleSeed = uniqueMerkleSeedGenerator;
				uniqueMerkleSeedGenerator++;
				// generate merkle root transaction
				bitclient_generateTxHash(sizeof(uint32), (uint8*)&minerPrimecoinBlock.uniqueMerkleSeed, workDataSource.coinBase1Size, workDataSource.coinBase1, workDataSource.coinBase2Size, workDataSource.coinBase2, workDataSource.txHash);
				bitclient_calculateMerkleRoot(workDataSource.txHash, workDataSource.txHashCount+1, minerPrimecoinBlock.merkleRoot);
				hasValidWork = true;
			}
		}
		LeaveCriticalSection(&workDataSource.cs_work);
		if( hasValidWork == false )
		{
			Sleep(1);
			continue;
		}
		// valid work data present, start processing workload
		if(	workDataSource.algorithm == ALGORITHM_PROTOSHARES )
		{
			switch( minerSettings.protoshareMemoryMode )
			{
			case PROTOSHARE_MEM_512:
				protoshares_process_512(&minerProtosharesBlock);
				break;
			case PROTOSHARE_MEM_256:
				protoshares_process_256(&minerProtosharesBlock);
				break;
			case PROTOSHARE_MEM_128:
				protoshares_process_128(&minerProtosharesBlock);
				break;
			case PROTOSHARE_MEM_32:
				protoshares_process_32(&minerProtosharesBlock);
				break;
			case PROTOSHARE_MEM_8:
				protoshares_process_8(&minerProtosharesBlock);
				break;
			default:
				printf("Unknown memory mode\n");
				Sleep(5000);
				break;
			}
		}
		else if( workDataSource.algorithm == ALGORITHM_SCRYPT )
		{
			scrypt_process_cpu(&minerScryptBlock);
		}
		else if( workDataSource.algorithm == ALGORITHM_PRIME )
		{
			primecoin_process(&minerPrimecoinBlock);
		}
		else if( workDataSource.algorithm == ALGORITHM_METISCOIN )
		{
			metiscoin_process(&minerMetiscoinBlock);
		}
		else
		{
			printf("xptMiner_minerThread(): Unknown algorithm\n");
			Sleep(5000); // dont spam the console
		}
	}
	return 0;
}


/*
 * Reads data from the xpt connection state and writes it to the universal workDataSource struct
 */
void xptMiner_getWorkFromXPTConnection(xptClient_t* xptClient)
{
	EnterCriticalSection(&workDataSource.cs_work);
	workDataSource.algorithm = xptClient->algorithm;
	workDataSource.version = xptClient->blockWorkInfo.version;
	workDataSource.timeBias = xptClient->blockWorkInfo.timeBias;
	workDataSource.nBits = xptClient->blockWorkInfo.nBits;
	memcpy(workDataSource.merkleRootOriginal, xptClient->blockWorkInfo.merkleRoot, 32);
	memcpy(workDataSource.prevBlockHash, xptClient->blockWorkInfo.prevBlockHash, 32);
	memcpy(workDataSource.target, xptClient->blockWorkInfo.target, 32);
	memcpy(workDataSource.targetShare, xptClient->blockWorkInfo.targetShare, 32);

	workDataSource.coinBase1Size = xptClient->blockWorkInfo.coinBase1Size;
	workDataSource.coinBase2Size = xptClient->blockWorkInfo.coinBase2Size;
	memcpy(workDataSource.coinBase1, xptClient->blockWorkInfo.coinBase1, xptClient->blockWorkInfo.coinBase1Size);
	memcpy(workDataSource.coinBase2, xptClient->blockWorkInfo.coinBase2, xptClient->blockWorkInfo.coinBase2Size);

	// get hashes
	if( xptClient->blockWorkInfo.txHashCount > MAX_TRANSACTIONS )
	{
		printf("Too many transaction hashes\n"); 
		workDataSource.txHashCount = 0;
	}
	else
		workDataSource.txHashCount = xptClient->blockWorkInfo.txHashCount;
	for(uint32 i=0; i<xptClient->blockWorkInfo.txHashCount; i++)
		memcpy(workDataSource.txHash+32*(i+1), xptClient->blockWorkInfo.txHashes+32*i, 32);
	// set blockheight last since it triggers reload of work
	workDataSource.height = xptClient->blockWorkInfo.height;
	LeaveCriticalSection(&workDataSource.cs_work);
	monitorCurrentBlockHeight = workDataSource.height;
}

#define getFeeFromDouble(_x) ((uint16)((double)(_x)/0.002)) // integer 1 = 0.002%
/*
 * Initiates a new xpt connection object and sets up developer fee
 * The new object will be in disconnected state until xptClient_connect() is called
 */
xptClient_t* xptMiner_initateNewXptConnectionObject()
{
	xptClient_t* xptClient = xptClient_create();
	if( xptClient == NULL )
		return NULL;
	// set developer fees
	// up to 8 fee entries can be set
	// the fee base is always calculated from 100% of the share value
	// for example if you setup two fee entries with 3% and 2%, the total subtracted share value will be 5%
	//xptClient_addDeveloperFeeEntry(xptClient, "Ptbi961RSBxRqNqWt4khoNDzZQExaVn7zL", getFeeFromDouble(0.5)); // 0.5% fee (jh00, for testing)
	return xptClient;
}

void xptMiner_xptQueryWorkLoop()
{
	// init xpt connection object once
	xptClient = xptMiner_initateNewXptConnectionObject();
	uint32 timerPrintDetails = GetTickCount() + 8000;
	while( true )
	{
		uint32 currentTick = GetTickCount();
		if( currentTick >= timerPrintDetails )
		{
			// print details only when connected
			if( xptClient_isDisconnected(xptClient, NULL) == false )
			{
				uint32 passedSeconds = (uint32)time(NULL) - miningStartTime;
				double speedRate = 0.0;
				if( workDataSource.algorithm == ALGORITHM_PROTOSHARES )
				{
					// speed is represented as collisions/min
					if( passedSeconds > 5 )
					{
						speedRate = (double)totalCollisionCount / (double)passedSeconds * 60.0;
					}
					printf("collisions/min: %.4lf Shares total: %d\n", speedRate, totalShareCount);
				}
				else if( workDataSource.algorithm == ALGORITHM_SCRYPT )
				{
					// speed is represented as khash/s
					if( passedSeconds > 5 )
					{
						speedRate = (double)totalCollisionCount / (double)passedSeconds / 1000.0;
					}
					printf("kHash/s: %.2lf Shares total: %d\n", speedRate, totalShareCount);
				}
				else if( workDataSource.algorithm == ALGORITHM_METISCOIN )
				{
					// speed is represented as khash/s (in steps of 0x8000)
					if( passedSeconds > 5 )
					{
						speedRate = (double)totalCollisionCount * 32768.0 / (double)passedSeconds / 1000.0;
					}
					printf("kHash/s: %.2lf Shares total: %d\n", speedRate, totalShareCount);
				}

			}
			timerPrintDetails = currentTick + 8000;
		}
		// check stats
		if( xptClient_isDisconnected(xptClient, NULL) == false )
		{
			EnterCriticalSection(&cs_xptClient);
			xptClient_process(xptClient);
			if( xptClient->disconnected )
			{
				// mark work as invalid
				EnterCriticalSection(&workDataSource.cs_work);
				workDataSource.height = 0;
				monitorCurrentBlockHeight = 0;
				LeaveCriticalSection(&workDataSource.cs_work);
				// we lost connection :(
				printf("Connection to server lost - Reconnect in 45 seconds\n");
				xptClient_forceDisconnect(xptClient);
				LeaveCriticalSection(&cs_xptClient);
				// pause 45 seconds
				Sleep(45000);
			}
			else
			{
				// is known algorithm?
				if( xptClient->clientState == XPT_CLIENT_STATE_LOGGED_IN && (xptClient->algorithm != ALGORITHM_PROTOSHARES && xptClient->algorithm != ALGORITHM_SCRYPT && xptClient->algorithm != ALGORITHM_METISCOIN) )
				{
					printf("The login is configured for an unsupported algorithm.\n");
					printf("Make sure you miner login details are correct\n");
					// force disconnect
					//xptClient_free(xptClient);
					//xptClient = NULL;
					xptClient_forceDisconnect(xptClient);
					LeaveCriticalSection(&cs_xptClient);
					// pause 45 seconds
					Sleep(45000);
				}
				else if( xptClient->blockWorkInfo.height != workDataSource.height || memcmp(xptClient->blockWorkInfo.merkleRoot, workDataSource.merkleRootOriginal, 32) != 0  )
				{
					// update work
					xptMiner_getWorkFromXPTConnection(xptClient);
					LeaveCriticalSection(&cs_xptClient);
				}
				else
					LeaveCriticalSection(&cs_xptClient);
				Sleep(1);
			}
		}
		else
		{
			// initiate new connection
			EnterCriticalSection(&cs_xptClient);
			if( xptClient_connect(xptClient, &minerSettings.requestTarget) == false )
			{
				LeaveCriticalSection(&cs_xptClient);
				printf("Connection attempt failed, retry in 45 seconds\n");
				Sleep(45000);
			}
			else
			{
				LeaveCriticalSection(&cs_xptClient);
				printf("Connected to server using x.pushthrough(xpt) protocol\n");
				miningStartTime = (uint32)time(NULL);
				totalCollisionCount = 0;
			}
			Sleep(1);
		}
	}
}


typedef struct  
{
	char* workername;
	char* workerpass;
	char* host;
	sint32 port;
	sint32 numThreads;
	uint32 ptsMemoryMode;
	// GPU / OpenCL options

	// mode option
	uint32 mode;
}commandlineInput_t;

commandlineInput_t commandlineInput;

void xptMiner_printHelp()
{
	puts("Usage: xptMiner.exe [options]");
	puts("General options:");
	puts("   -o, -O                        The miner will connect to this url");
	puts("                                 You can specify a port after the url using -o url:port");
	puts("   -u                            The username (workername) used for login");
	puts("   -p                            The password used for login");
	puts("   -t <num>                      The number of threads for mining (default is set to number of cores)");
	puts("                                 For most efficient mining, set to number of physical CPU cores");
	puts("Protoshares specific:");
	puts("   -m<amount>                    Defines how many megabytes of memory are used per thread.");
	puts("                                 Default is 256mb, allowed constants are:");
	puts("                                 -m512 -m256 -m128 -m32 -m8");
	puts("Example usage:");
	puts("   xptMiner.exe -o http://poolurl.com:10034 -u workername.pts_1 -p workerpass -t 4");
}

void xptMiner_parseCommandline(int argc, char **argv)
{
	sint32 cIdx = 1;
	while( cIdx < argc )
	{
		char* argument = argv[cIdx];
		cIdx++;
		if( memcmp(argument, "-o", 3)==0 || memcmp(argument, "-O", 3)==0 )
		{
			// -o
			if( cIdx >= argc )
			{
				printf("Missing URL after -o option\n");
				exit(0);
			}
			if( strstr(argv[cIdx], "http://") )
				commandlineInput.host = _strdup(strstr(argv[cIdx], "http://")+7);
			else
				commandlineInput.host = _strdup(argv[cIdx]);
			char* portStr = strstr(commandlineInput.host, ":");
			if( portStr )
			{
				*portStr = '\0';
				commandlineInput.port = atoi(portStr+1);
			}
			cIdx++;
		}
		else if( memcmp(argument, "-u", 3)==0 )
		{
			// -u
			if( cIdx >= argc )
			{
				printf("Missing username/workername after -u option\n");
				exit(0);
			}
			commandlineInput.workername = _strdup(argv[cIdx]);
			cIdx++;
		}
		else if( memcmp(argument, "-p", 3)==0 )
		{
			// -p
			if( cIdx >= argc )
			{
				printf("Missing password after -p option\n");
				exit(0);
			}
			commandlineInput.workerpass = _strdup(argv[cIdx]);
			cIdx++;
		}
		else if( memcmp(argument, "-t", 3)==0 )
		{
			// -t
			if( cIdx >= argc )
			{
				printf("Missing thread number after -t option\n");
				exit(0);
			}
			commandlineInput.numThreads = atoi(argv[cIdx]);
			if( commandlineInput.numThreads < 1 || commandlineInput.numThreads > 128 )
			{
				printf("-t parameter out of range");
				exit(0);
			}
			cIdx++;
		}
		else if( memcmp(argument, "-m512", 6)==0 )
		{
			commandlineInput.ptsMemoryMode = PROTOSHARE_MEM_512;
		}
		else if( memcmp(argument, "-m256", 6)==0 )
		{
			commandlineInput.ptsMemoryMode = PROTOSHARE_MEM_256;
		}
		else if( memcmp(argument, "-m128", 6)==0 )
		{
			commandlineInput.ptsMemoryMode = PROTOSHARE_MEM_128;
		}
		else if( memcmp(argument, "-m32", 5)==0 )
		{
			commandlineInput.ptsMemoryMode = PROTOSHARE_MEM_32;
		}
		else if( memcmp(argument, "-m8", 4)==0 )
		{
			commandlineInput.ptsMemoryMode = PROTOSHARE_MEM_8;
		}
		else if( memcmp(argument, "-help", 6)==0 || memcmp(argument, "--help", 7)==0 )
		{
			xptMiner_printHelp();
			exit(0);
		}
		else
		{
			printf("'%s' is an unknown option.\nType jhPrimeminer.exe --help for more info\n", argument); 
			exit(-1);
		}
	}
	if( argc <= 1 )
	{
		xptMiner_printHelp();
		exit(0);
	}
}


int main(int argc, char** argv)
{
	commandlineInput.host = "ypool.net";
	srand(GetTickCount());
	commandlineInput.port = 8080 + (rand()%8); // use random port between 8080 and 8087
	commandlineInput.ptsMemoryMode = PROTOSHARE_MEM_256;
	SYSTEM_INFO sysinfo;
	GetSystemInfo( &sysinfo );
	commandlineInput.numThreads = sysinfo.dwNumberOfProcessors;
	commandlineInput.numThreads = min(max(commandlineInput.numThreads, 1), 4);
	xptMiner_parseCommandline(argc, argv);
	minerSettings.protoshareMemoryMode = commandlineInput.ptsMemoryMode;
	printf("\xC9\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBB\n");
	printf("\xBA  xptMiner (v1.1)                                 \xBA\n");
	printf("\xBA  author: jh00                                    \xBA\n");
	printf("\xBA  http://ypool.net                                \xBA\n");
	printf("\xC8\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBC\n");
	printf("Launching miner...\n");
	uint32 mbTable[] = {512,256,128,32,8};
	//printf("Using %d megabytes of memory per thread\n", mbTable[min(commandlineInput.ptsMemoryMode,(sizeof(mbTable)/sizeof(mbTable[0])))]);
	printf("Using %d threads\n", commandlineInput.numThreads);
	// set priority to below normal
	SetPriorityClass(GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS);
	// init winsock
	WSADATA wsa;
	WSAStartup(MAKEWORD(2,2),&wsa);
	// get IP of pool url (default ypool.net)
	char* poolURL = commandlineInput.host;//"ypool.net";
	hostent* hostInfo = gethostbyname(poolURL);
	if( hostInfo == NULL )
	{
		printf("Cannot resolve '%s'. Is it a valid URL?\n", poolURL);
		exit(-1);
	}
	void** ipListPtr = (void**)hostInfo->h_addr_list;
	uint32 ip = 0xFFFFFFFF;
	if( ipListPtr[0] )
	{
		ip = *(uint32*)ipListPtr[0];
	}
	char* ipText = (char*)malloc(32);
	sprintf(ipText, "%d.%d.%d.%d", ((ip>>0)&0xFF), ((ip>>8)&0xFF), ((ip>>16)&0xFF), ((ip>>24)&0xFF));
	// init work source
	InitializeCriticalSection(&workDataSource.cs_work);
	InitializeCriticalSection(&cs_xptClient);
	// setup connection info
	minerSettings.requestTarget.ip = ipText;
	minerSettings.requestTarget.port = commandlineInput.port;
	minerSettings.requestTarget.authUser = commandlineInput.workername;
	minerSettings.requestTarget.authPass = commandlineInput.workerpass;
	// start miner threads
	for(uint32 i=0; i<commandlineInput.numThreads; i++)
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)xptMiner_minerThread, (LPVOID)0, 0, NULL);
	// enter work management loop
	xptMiner_xptQueryWorkLoop();
	return 0;
}