#ifndef VETOR_H
#define VETOR_H


class vetor
{
public:
    vetor();
    vetor(double x, double y);

    double x,y;

    void normalizar();
    void add(vetor v);
    void add(double i);
    void mult(double i);
    void div(double i);
    vetor get();

private:
    double length();
};

#endif // VETOR_H
