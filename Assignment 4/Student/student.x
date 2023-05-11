struct student {
    int serialNumber;
    string regNumber<20>;
    string firstName<50>;
    string lastName<50>;
};

program STUDENT {
    version STUDENT_PROG {
        int ADD_STUDENT(student) = 1;
    } = 1;
} = 0x20000001;
