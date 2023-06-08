#ifndef _CONCURRENT_CONNECTION_ORIENTED_CALCULATOR_H_RPCGEN
#define _CONCURRENT_CONNECTION_ORIENTED_CALCULATOR_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif


struct calculator_input {
	int num1;
	char *op;
	int num2;
};
typedef struct calculator_input calculator_input;

#define CALCULATOR_PROG 0x20000001
#define CALCULATOR_VERS 1

#if defined(__STDC__) || defined(__cplusplus)
#define calculate 1
extern  int * calculate_1(calculator_input *, CLIENT *);
extern  int * calculate_1_svc(calculator_input *, struct svc_req *);
extern int calculator_prog_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define calculate 1
extern  int * calculate_1();
extern  int * calculate_1_svc();
extern int calculator_prog_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_calculator_input (XDR *, calculator_input*);

#else /* K&R C */
extern bool_t xdr_calculator_input ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_CONCURRENT_CONNECTION_ORIENTED_CALCULATOR_H_RPCGEN */
