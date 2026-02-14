#pragma once

class B {

public:
    B(int j, int n);

    double operator()(double t);

private:
    int j;
    int n;
};