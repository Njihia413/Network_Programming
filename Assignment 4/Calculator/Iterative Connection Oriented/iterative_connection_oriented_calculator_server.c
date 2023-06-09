#include "iterative_connection_oriented_calculator.h"

int *
calculate_1_svc(calculator_input *argp,  struct svc_req *rqstp)
{
    /*
     * insert server code here
     */
    printf("Calculate Procedure Called\n");

    static int result;
    printf("Data from client: %d %c %d\n", argp->num1, *argp->op, argp->num2);
    switch (*argp->op) {
        case '+':
            result = argp->num1 + argp->num2;
            printf("Sum is %d\n", result);
            break;
        case '-':
            result = argp->num1 - argp->num2;
            printf("Difference is %d\n", result);
            break;
        case '*':
            result = argp->num1 * argp->num2;
            printf("Product is %d\n", result);
            break;
        case '/':
            if (argp->num2 == 0) {
                printf("Division by zero\n");
                exit(1);
            }
            result = argp->num1 / argp->num2;
            printf("Quotient is %d\n", result);
            break;
        default:
            printf("Invalid operator\n");
            exit(1);
            break;
    }

    // send the result back to the client
    return (&result);

    // exit the procedure and send the result to the client
    exit(0);

}