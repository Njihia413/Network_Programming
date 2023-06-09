#include "iterative_connectionless_calculator.h"

void calculator_prog_1(char *host)
{
    CLIENT *clnt;
    int *result_1;
    calculator_input calculate_1_arg1;

#ifndef DEBUG
    clnt = clnt_create(host, CALCULATOR_PROG, CALCULATOR_VERS, "udp");
    if (clnt == NULL)
    {
        clnt_pcreateerror(host);
        exit(1);
    }
#endif /* DEBUG */

    int num1, num2;
    char op;
    printf("Enter first number: ");
    scanf("%d", &num1);
    printf("Choose the operator you would like to use:\n");
    printf("+ (Addition)\n");
    printf("- (Subtraction)\n");
    printf("* (Multiplication)\n");
    printf("/ (Division)\n");
    scanf(" %c", &op); // note the space before %c to consume whitespace
    printf("Enter second number: ");
    scanf("%d", &num2);

    calculate_1_arg1.num1 = num1;
    calculate_1_arg1.op = &op;
    calculate_1_arg1.num2 = num2;

    result_1 = calculate_1(&calculate_1_arg1, clnt);
    if (result_1 == (int *)NULL)
    {
        clnt_perror(clnt, "call failed");
    }
    else
    {
        printf("Result: %d\n", *result_1);
        printf("\n");
    }

#ifndef DEBUG
    clnt_destroy(clnt);
#endif /* DEBUG */
}

int main(int argc, char *argv[])
{
    char *host;

    if (argc < 2)
    {
        printf("Usage: %s <host>\n", argv[0]);
        exit(1);
    }

    host = argv[1];
    calculator_prog_1(host);
    exit(0);
}
