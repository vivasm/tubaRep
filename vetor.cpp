#include "vetor.h"
#include <math.h>

vetor::vetor()
{

}

vetor::vetor(double x, double y)
{
    this->x=x;
    this->y=y;
}

double vetor::length()
{
    return sqrt(x*x+y*y);
}

void vetor::normalizar(){
    double length = this->length();

    if(length != 0){
        this->x = this->x/length;
        this->y = this->y/length;
    }
}

void vetor::add(vetor v)
{
    this->x+=v.x;
    this->y+=v.y;
}

void vetor::add(double i)
{
    this->x+=i;
    this->y+=i;
}

void vetor::mult(double i)
{
    this->x*=i;
    this->y*=i;
}

void vetor::div(double i)
{
    this->x/=i;
    this->y/=i;
}

vetor vetor::get()
{
    vetor v(this->x, this->y);
    return v;
}
