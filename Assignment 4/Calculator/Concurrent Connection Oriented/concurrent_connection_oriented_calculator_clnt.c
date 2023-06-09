#include <memory.h> /* for memset */
#include "concurrent_connection_oriented_calculator.h"

/* Default timeout can be changed using clnt_control() */
static struct timeval TIMEOUT = { 25, 0 };

int *
calculate_1(calculator_input *argp,  CLIENT *clnt)
{
	static int clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, calculate,
		(xdrproc_t) xdr_calculator_input, (caddr_t) argp,
		(xdrproc_t) xdr_int, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}
