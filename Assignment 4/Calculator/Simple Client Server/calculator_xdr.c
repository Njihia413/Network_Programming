#include "calculator.h"

bool_t
xdr_calculator_input (XDR *xdrs, calculator_input *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, &objp->num1))
		 return FALSE;
	 if (!xdr_pointer (xdrs, (char **)&objp->op, sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->num2))
		 return FALSE;
	return TRUE;
}
