#include "../include/ds.h"

int ds_setup(ds_W_struct *W, ds_S_struct *S, csprng RNG)
{
	char ikm[64];
	octet IKM = {0, sizeof(ikm), ikm};
	OCT_rand(&IKM, &RNG, 32);
	if (BLS_INIT() != BLS_OK)
	{
		printf("\tDS ERROR, Failed to initialize BLS.\n");
		return EXIT_FAILURE;
	}

	if(BLS_KEY_PAIR_GENERATE(&IKM, &S->S, &W->W) != BLS_OK){
		printf("\tDS ERROR, DS key gen failed.\n"); 
		return EXIT_FAILURE;
	} 
	return EXIT_SUCCESS;
}

int ds_sign(octet *sig, octet M, octet S)
{
	if(BLS_CORE_SIGN(sig, &M, &S) != BLS_OK)
	{
		printf("\tDS ERROR, DS signing failed.\n"); 
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int ds_verify(octet sig, octet M, octet W)
{
	if(BLS_CORE_VERIFY(&sig, &M, &W) != BLS_OK)
	{
		printf("\tDS ERROR, DS signature does not verify.\n"); 
		return EXIT_FAILURE;
	} 
	return EXIT_SUCCESS;
}