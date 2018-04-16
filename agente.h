#ifndef AGENTE_H
#define AGENTE_H
#include "vetor.h"
#include <vector>

using namespace std;

class agente {
public:
    agente();

    void rodaModelo();
    void atua();
    void addViz(agente *pag);
    void addPrd(agente *pag);
    void cleanViz();
    void setCor();
    void setCor(char* c);
    void setAlinhamento(double i);
    void setCoesao(double i);
    void setSeparacao(double i);
    void setAlerta();
    void setAlvo(int i, vetor v);
    void setToMorto(int t);
    void cleanAlvo();
    void setBaseCor();
    void setVetorAlinhamento(vetor v);
    void setVetorCoesao(vetor v);
    void setVetorSeparacao(vetor v);
    void setVelocidade(double i);
    void setCentroMassa(vetor v);
    void condContTuba(vetor v);
    bool ehPeixe();
    int getToMorto() const;
    int getIdAlvo();
    char *getCor();
    double getRaiViz() const;
    double getTam();
    double calcDist(vetor posI);
    double getAlinhamento();
    double getCoesao();
    double getSeparacao();
    double getVelocidadeBase();
    double condContPeixe(double p);
    vetor getLoc();
    vetor getDirecao();
    vetor getAlvo();
    vetor getCentroMassa();
    vector<vetor> getLocalizacao();


private:
    int id;
    double ang;
    vetor direcao;
    vector<vetor> localizacao;
    vector <agente *> viz;
    vector <agente *> prd;

    void vaiDeBoa();
    void correNegada();
    void atuaCorpo();
    void atuaCabeca(vetor loc);
    void vaiComAGalera();
    void computaAlinhamento(vetor v, double pesoAlinhamento);
    void computaCoesao(vetor v, double pesoCoesao);
    void computaSeparacao(vetor v, double pesoSeparacao);
    void computaSeparacao(vector <agente *> viz, double pesoSeparacao);
    void computaVelocidade(double velo);
    void modeloPeixe();
    void modeloTubarao();
    void relogioExplosao();
    void relogioExplosaoDelay();
    void resetExplosao(int i);

protected:
    bool nemo=false;
    bool alerta=false;
    bool explosao=true;
    int toMorto=0;
    int tempoExplosao;
    int intervaloExplosao;
    int idAlvo=-1;
    double passo=0.001;
    double velocidadeBase;
    double velocidade;
    double angV;
    double tam;
    double raiViz;
    double raiMorte;
    double pesoAlinhamento;
    double pesoCoesao;
    double pesoSeparacao;
    double pesoVelocidade;
    char* cor;
    char* baseCor;
    vetor alvo;
    vetor centroMassa;
    vetor alinhamento;
    vetor coesao;
    vetor separacao;

};

class peixe: public agente
{
public:
    peixe();

};

class tubarao: public agente
{
public:
    tubarao();

};

#endif // AGENTE_H
