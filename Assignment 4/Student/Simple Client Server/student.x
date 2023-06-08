struct Student {
    /* Student Properties */
    int serialNumber;
    string regNumber<20>;
    string firstName<50>;
    string lastName<50>;
};

program STUDENTPROG {
    version STUDENTVERS {
        int add_student(Student) = 1;
    } = 1;
} = 0x20000001;
