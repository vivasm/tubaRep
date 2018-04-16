#include "ambiente.h"
#include <math.h>
#include <algorithm>
#include <iostream>
#include <typeinfo>

using namespace std;

ambiente::ambiente()
{
    this->tamM=1000;                // Tamanho de mundo
    this->nAg=300;                  // Numero de peixes
    this->nTb=1;                    // Numero de Tubarão

    for(int i=0;i<nAg;i++)
    {
        peixe finn;
        this->ag.push_back(finn);
    }

    for(int i=0;i<nTb;i++)
    {
        tubarao lenny;
        this->ag.push_back(lenny);
    }
}

void ambiente::roda()
{
    for(int i=0;i<(int)this->ag.size();i++)
    {
        if(this->ag[i].getToMorto() == 1)       // Se o agente está morrendo
        {
            nAg--;                              // Numero agentes cai
            ag[i].setToMorto(2);                // O agente está agora morto
        }

        else if(this->ag[i].getToMorto() == 0)  // Caso o agente esteja vivo
        {
            this->atuPerc(i);                   // Atualiza percepção do agente
            this->rodaModelo(i);                // Roda o modelo do agente
            this->atuacao(i);                   // Roda atuação do agente
            this->atuPerc(i);                   // Atualiza percepção do agente
        }
    }
}

void ambiente::rodaModelo(int i)
{
    this->ag[i].rodaModelo();   // Roda modelo do agente
}

void ambiente::atuacao(int i)
{
    this->ag[i].atua();         // Roda atuação do agente

}

void ambiente::atuPerc(int i)
{
    if(ag[i].ehPeixe())         // Caso o agente for peixe, atualiza os agentes em seu raio de percepção
        this->getViz(i);
    else
        this->getPresa(i);      // Caso o agente for tubarão, calcula possivel presa
}

void ambiente::getViz(int i)
{
    int b = 0, c = 0;;
    double d;
    double velo=0;

    vetor loc;
    vetor mloc = ag[i].getLoc();
    vetor alin = vetor(0, 0);
    vetor coes = vetor(0, 0);
    vetor sep  = vetor(0,0);

    ag[i].setCentroMassa(mloc);                 // Atribui a localização ao centro de massa inicialmente

    this->ag[i].cleanViz();                // Limpa o campo de vizinhos

    for(int j=0;j<(int)this->ag.size();j++)     // Para cada agente
    {
        if(i!=j && ag[j].getToMorto() == 0)     // Caso o agente seja diferente e ele não esteja morto
        {
            d=this->calcDist(i,j);              // Calcula distancia entre os agentes
            if(d<=this->ag[i].getRaiViz())
            {
                if(ag[j].ehPeixe() )            // Verifica se é peixe
                {
                    b++;                        // Incrementa contador de vizinhos

                    this->ag[i].addViz(&this->ag[j]);   // Adiciona ao vetor de vizinhos

                    alin.add(ag[j].getDirecao());       // Adicionar a direcao ao vetor alinhamento
                    velo+=ag[j].getVelocidadeBase();    // Adiciona velocidade a velocidade

                    loc = ag[j].getLoc();               // Pega localizacao do epixe
                    coes.add(loc);                      // Adicionar a localizacao ao vetor coesao

                    if(d < (ag[i].getTam()+ag[j].getTam())*0.0012) // Verifica se a distancia é menor que a soma do tamanho dos peixe
                    {
                        double ang = atan2 (mloc.y-loc.y, mloc.x-loc.x); // Calcula angulo de afastamento

                        sep.add(vetor(cos (ang), sin (ang)));            // Adicionar vetor de afastamento ao vetor separação

                        c++;                                             // Incremente contador de separação
                    }
                }
                else                                                    // Caso for tubarão
                {
                    this->ag[i].addPrd(&this->ag[j]);                   // Adiciona tubarão ao vetor de predadores
                    ag[i].setAlerta();                                  // Aciona o alerta no peixe
                }
            }
        }
    }

    if(b > 0)                       // Se o contador de vizinhos for maior que 0
    {
        alin.div (b);               // Divide o vetor alinhamento pela quantidade de vizinhos
        alin.normalizar();          // e normaliza

        coes.div(b);                // Divide o vetor coesão pela quantidade de vizinhos

        velo/=b;                    // Divide a soma das velocidades pela quantidade de vizinhos
    }
    if (c > 0)                      // Se o contador de separacao for maior que 0
        sep.div (c);                // Divide o vetor separação pelo contador de separacao


    ag[i].setVetorAlinhamento(alin); // Atribui o vetor alinhamento para o vetor do agente
    ag[i].setVetorCoesao(coes);      // Atribui o vetor coesão para o vetor do agente
    ag[i].setVetorSeparacao(sep);    // Atribui o vetor separação para o vetor do agente
    ag[i].setVelocidade(velo);       // Atribui o valor velocidade apra o atributo do agente

}

void ambiente::getPresa(int i)
{
    double d=0, menorDistancia=1000;

    int a = ag[i].getIdAlvo();                      // Pega o id do alvo

    vetor alvo;

    if(a == -1 || ag[a].getToMorto() != 0 || !ag[a].ehPeixe()) // Se o agente não existir, não estiver vivo ou não for peixe
        ag[i].cleanAlvo();                                     // Limpa o alvo
    else
    {
        alvo = ag[a].getLoc();                                 // Pega localização do alvo

        ag[i].setAlvo(a, ag[a].getCentroMassa());              // Seta o alvo

        d = ag[i].calcDist(alvo);                              // calcula distancia entre o tubarão e o alvo

        menorDistancia=d;                                      // atribui a menor distância a essa distancia
    }

    for(int j=0;j<this->ag.size();j++)                         // Para cada agente
    {
        if(ag[j].ehPeixe() && ag[j].getToMorto() == 0)         // Se for peixe e estiver vivo
        {
            ag[j].setBaseCor();                                // Retorna sua cor base
            d=this->calcDist(i,j);                             // Calcula nova distancia entre o tubarao e o agente
            if(d<=this->ag[i].getRaiViz() && d<=menorDistancia){ // Se a distancia estiver dentro do raio de percepção e for menor que a menor distancia
                ag[j].setCor("#000000");                       // Atribui cor preta ao alvo
                this->ag[i].setAlvo(j, ag[j].getCentroMassa());// Seta um novo alvo
                menorDistancia=d;                              // menor distancia sera agora a distancia entro o agente e o tubarao
            }
        }
    }
}

double ambiente::calcDist(int i, int j) // Calcula distancia entre dois agentes dada a sua posicao no vetor
{
    double dist=0;
    vetor posI,posJ;
    posI=this->ag[i].getLoc();
    posJ=this->ag[j].getLoc();
    double dx,dy;
    dx=posI.x-posJ.x;
    dy=posI.y-posJ.y;
    dist= sqrt(dx*dx+dy*dy);
    return dist;
}

vetor ambiente::getPosAg(int i) // pega a localização de um agente
{
    vetor pa=this->ag[i].getLoc();
    pa.x*=this->tamM;
    pa.y*=this->tamM;
    return pa;
}


vetor ambiente::getAvAg(int i) // Retorna alvo agente
{
    vetor pa = this->ag[i].getAlvo();
    pa.x*=this->tamM;
    pa.y*=this->tamM;
    return pa;
}

vector<vetor> ambiente::getPosCorpo(int i)  // Pega a localização do vetor corpo do agente
{
    vector<vetor> localizacao=this->ag[i].getLocalizacao();

    for(int i = 0; i < (int)localizacao.size(); i ++)
    {
        vetor pa = localizacao[i];
        pa.x*=this->tamM;
        pa.y*=this->tamM;
        localizacao[i] = pa;
    }

    return localizacao;
}

double ambiente::getTamM() const // Retorna tamanho do mundoi
{
    return tamM;
}

double ambiente::getTamAg(int i) // Retorna tamanho do agente
{
    return this->ag[i].getTam();
}

char* ambiente::getCorAg(int i) // Retorna cor do agene
{
    return this->ag[i].getCor();
}

int ambiente::getMoAg(int i) // Retorna estado vital do agente
{
    return this->ag[i].getToMorto();
}

double ambiente::getAlAg(int i) // Retorna coeficiente de alinhamento agente
{
    return this->ag[i].getAlinhamento();
}

double ambiente::getCoAg(int i) // Retorna coeficiente de coesão agente
{
    return this->ag[i].getCoesao();
}


double ambiente::getSeAg(int i) // Retorna coeficiente de separação agente
{
    return this->ag[i].getSeparacao();
}

void ambiente::setAlAg(double v) // Atribui coeficiente de alinhamento
{
    for(int i=0;i<this->getSAg();i++)
        this->ag[i].setAlinhamento(v);
}

void ambiente::setCoAg(double v) // Atribui coeficiente de coesão
{
    for(int i=0;i<this->getSAg();i++)
        this->ag[i].setCoesao(v);
}

void ambiente::setSeAg(double v) // Atribui coeficiente de separação
{
    for(int i=0;i<this->getSAg();i++)
        this->ag[i].setSeparacao(v);
}
