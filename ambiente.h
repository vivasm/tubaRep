#ifndef AMBIENTE_H
#define AMBIENTE_H
#include "agente.h"
#include "vetor.h"
#include <vector>
using namespace std;

class ambiente {
public:
    ambiente();

    void roda();
    void setAlAg(double v);
    void setCoAg(double v);
    void setSeAg(double v);
    int getSAg() { return this->ag.size();}
    int getNAg() { return this->nAg;}
    int getMoAg(int i);
    double getTamM() const;
    double getTamAg(int i);
    double getAlAg(int i);
    double getCoAg(int i);
    double getSeAg(int i);
    char *getCorAg(int i);
    vetor getPosAg(int i);
    vetor getAvAg(int i);
    vector<vetor> getPosCorpo(int i);

private:
    int nAg;
    int nTb;
    double tamM;
    vector<agente> ag;
    vector<int> in;

    void rodaModelo(int i);
    void atuacao(int i);
    void atuPerc(int i);
    void getPresa(int i);
    void getViz(int i);
    double calcDist(int i, int j);
    double calcDist(vetor i);
};

#endif // AMBIENTE_H
