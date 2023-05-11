struct calculator_input {
        int num1;
        char *op;
        int num2;
};


program CALCULATOR_PROG {
    version CALCULATOR_VERS {
        int calculate(calculator_input) = 1;
    } = 1;
} = 0x20000001;
