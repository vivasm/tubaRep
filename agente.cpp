#include "agente.h"
#include <stdlib.h>
#include <math.h>
#define PI 3.1416

agente::agente() {

    this->alvo = vetor(0, 0);
    vetor loc((double)rand()/RAND_MAX, (double)rand()/RAND_MAX); //atribui uma localização aleatória
    for(int i=0; i < rand() % 7 + 8; i++)                        //atribui um tamanho aleatório ao agente entre 8 e 15
        this->localizacao.push_back(loc);
    this->ang=(double)rand()/RAND_MAX*360;                       //atribui um angulo de direção aleatório
    this->direcao = vetor(cos (this->ang*PI/180), sin (this->ang*PI/180)); //direção do agente em relação ao angulo
    this->pesoAlinhamento = 0.75;                                //fator de multiplicação do coeficiente de alinhamento
    this->pesoCoesao = 0.1;                                      //fator de multiplicação do coeficiente de coesão
    this->pesoSeparacao = 0.7;                                  //fator de multiplicação do coeficiente de separação
    cleanAlvo();
}

peixe::peixe() // Atribui caracteristicas individuais do peixe
{
    setCor();                                                    // Atribui uma cor aleatória

    this->baseCor = this->cor;                                   // Salva a cor aleatória como cor base
    this->velocidadeBase = (double)(rand() % 3 + 4)*this->passo; // Atribui velocidade aleatória entre 4 e 7
    this->velocidade = this->velocidadeBase;                     // Velocidade base é igual a velocidade
    this->angV=40;                                               // Atribui angulo de visao de 40
    this->raiViz=70*this->passo;                                 // Atribui raio de percepção de 70
    this->raiMorte=10*this->passo;                               // Atribui raio de morte 20
    this->tam = rand()% 4 + 5;                                   // Atribui tamanho da circuferência do peixe
    this->intervaloExplosao=30;                                  // Atribui tempo de intervalo de explosão
    this->tempoExplosao=this->intervaloExplosao;                 // Tempo de explosão é igual ao intervalo
    this->nemo = true;                                           // O agente é um peixe, não um tubarão
}

tubarao::tubarao() // Atribui caracteristicas individuais do tubarão
{
    setCor("#02a2a8");                                           // Atribui cor especifica para tubarões
    this->baseCor= this->cor;                                    // cor base igual a cor atribuida
    this->velocidadeBase =10*this->passo;                        // Atribui velocidade base constante
    this->velocidade = this->velocidadeBase;                     // Velocidade é igual a velocidade base
    this->angV=20;                                               // Atribui angulo de visão
    this->raiViz=100*this->passo;                                // Atribui angulo de percepção
    this->tam = 12;                                              // Atribui tamanho de circuferencia do tubarao
    this->intervaloExplosao=60;                                  // Atribui tempo de intervalo de explosão
    this->tempoExplosao=this->intervaloExplosao;                 // Tempo de explosão é igual ao intervalo
}

void agente::rodaModelo(){ // Atualiza as informações de percepção, direção e velocidade do agente

    if(this->ehPeixe())
        modeloPeixe();                  // Caso for peixe, roda o modelo do peixe
    else
        modeloTubarao();                // Caso for tubarão, roda o modelo do tubarão
}

void agente::modeloPeixe() // Atualiza as informações de percepção, direção e velocidade do peixe
{
    vetor loc = this->getLoc();                     // Verifica as condições de continuidade
    this->localizacao[0].x=condContPeixe(loc.x);    // do peixe. Caso ultrapasse os limites do mundo
    this->localizacao[0].y=condContPeixe(loc.y);    // sua localização é atualizada

    if(!this->alerta)                               // Caso o peixe não esteja em alerta, não tem predador na área
    {
        resetExplosao(intervaloExplosao);           // Testa o intervalo de descanso se já foi esgotado

        this->vaiComAGalera();                      // Assume comportamento de cardume
        this->vaiDeBoa();                           // Assume movimento em direção aleatória reduzida ao seu angulo de visão
    }
    else
        this->correNegada();                        // Caso o peixe esteja em alerta, tem tubarão na sua área de percepção

    this->alerta=false;                             // Reseta o alerta de predador
}

void agente::modeloTubarao() // Atualiza as informações de percepção, direção e velocidade do tubarão
{
    vetor loc = this->getLoc();                     // Verifica as condições de continuidade do tubarão. Caso
    condContTuba(loc);                              // ultrapasse os limites do mundo, ele quica nas bordas.

    vetor alvoLoc = this->getAlvo();                // Pega a localizacao do alvo. x=0 e y=0 caso nao tenha alvo definido
    int id = this->getIdAlvo();;                    // Pega o id do alvo. -1 caso não tenha alvo definido
    this->velocidade=this->velocidadeBase;          // Velocidade do tubarão igual a sua velocidade base

    double d=this->calcDist(alvoLoc);               // Calcula distância entre o tubarão e o seu alvo

    relogioExplosaoDelay();                         // Verifica se o intervalo de explosão(Tempo Descanso) já passou

    if(d<this->getRaiViz() && id >= 0)              // Se o id é maior que -1 (Alvo existente)  a distância do alvo é menor que o raio de percepção
    {
        relogioExplosao();                          // Calcula o tempo de explosão
        if(explosao)                                // Se o tubarão esteja em modo de explosão
        {
            computaCoesao(alvoLoc, 0.15);           // Atribui um alvo e um fator de multiplicação de atração entre o tubarão e o alvo
            this->velocidade=11*passo;              // Aumenta a sua velocidade
            setCor("#3e5f60");                      // Atribui uma cor diferente quando está em modo explosivo
        }
        else                                        // Se o tubarão não estiver em modoexplosão
        {
            computaCoesao(alvoLoc, 0.05);           // Atribui um alvo e um fator de multiplicação de atração entre o tubarão e o alvo menor
            setBaseCor();                           // Retorna a sua cor original
        }
    }
    this->vaiDeBoa();                               // Assume movimento em direção aleatória reduzida ao seu angulo de visão
}

void agente::vaiDeBoa() // Calcula uma direção aleatória baseada na direção atual e o angulo de visão do agente
{
    double na=(double)rand()/RAND_MAX;                          // Calcula um angulo aleátorio
    double ang =(na-0.5)*this->angV;                            // com valores reduzidos ao angulo
    ang = ang*PI/180;                                           // de visão do agente

    vetor dir = this->direcao;                                  // Calcula o angulo de direção
    double ang2 = atan2 (dir.y, dir.x);                         // atual do agente

    this->direcao =(vetor(cos (ang2+ang), sin (ang2+ang)));     // Cria um vetor com a soma dos dois angulos
    this->direcao.normalizar();                                   // e o normaliza
}

void agente::vaiComAGalera()
{
    computaAlinhamento(this->alinhamento, pesoAlinhamento);     // Computa o comportamento de cardume através
    computaCoesao(this->coesao, pesoCoesao);                    // dos vetores de alinhamento, coesão e separação
    computaSeparacao(this->separacao, pesoSeparacao);           // e seus respectivos valores de multiplicação
}                                                               // Caso não haja vizinhos, os valores serão nulos.


void agente::correNegada() // Comportamento do peixe ao identificar um tubarão em seu angulo de direção
{
    relogioExplosaoDelay();                     // Calcula o tempo do intervalo de explosão do peixe (descanso)

    double d;

    for(int i=0;i<this->prd.size();i++){        // Para cada predador captado em seu raio de percepção
        d = this->calcDist(prd[i]->getLoc());   // Calcula a distância entre o peixe e o predador

        if(d <= this->raiMorte)                 // Caso o tubarão esteja em seu raio de morte
        {
            this->toMorto=1;                    // O peixe morre. Obs.: Os valores da variavel toMorto são:
            return;                             // (0 == vivo; 1 == morrendo; 2 == morto)
        }
    }

    relogioExplosao();                          // Verifica o tempo de explosão do peixe

    if(explosao)                                // Caso em modo explosão
    {
        computaSeparacao(this->prd, 0.8);  // Atribui um alvo e um fator de multiplicação de repulsão entre o peixe e os tubarões
        this->velocidade = 8*this->passo;       // Atribui uma velocidade maior ao peixe
    }
    else                                        // Caso modo descanso  
        this->velocidade = this->velocidadeBase;// Velocidade volta para a velocidade base


}

void agente::atua() // Atualiza a localização do agente
{
    vetor loc = this->getLoc();

    vetor v = this->direcao.get();  // Pega a direção,
    v.mult(this->velocidade);       // multiplica pela velocidade
    loc.add(v);                     // e adiciona a localização anterrior

    atuaCabeca(loc);                // atualiza a cabeça do agente
    atuaCorpo();                    // atualiza o vetor de localização do corpo
}

void agente::atuaCabeca (vetor loc)  // atualiza a cabeça do agente
{
    localizacao[0] = loc;
}

void agente::atuaCorpo () // atualiza o vetor de localização do corpo
{
    for (int i = this->localizacao.size() - 1; i > 0; i--)
    {
        localizacao [i] = localizacao [i-1];
    }
}

void agente::computaAlinhamento (vetor v, double pesoAlinhamento)
{
    v.mult(pesoAlinhamento);        // Multiplica o vetor alinhamento
    this->direcao.add(v);           // pelo coeficiente de multiplicação (peso)
    this->direcao.normalizar();     // e atribui a direção e normaliza.
}

void agente::computaCoesao(vetor v, double pesoCoesao)
{
    vetor mloc = this->getLoc();                        // Pega a localização atual
    double ang = atan2 (v.y - mloc.y, v.x - mloc.x);    // e calcula o angulo entre localização e o vetor

    vetor v2 = vetor(cos(ang), sin(ang));               // Cria um vetor com o angulo calculado
    v2.mult(pesoCoesao);                                // e multiplica plo coeficiente de multiplicação (peso)

    this->direcao.add(v2);                              // atribui o vetor gerado a
    this->direcao.normalizar();                         // direção e normaliza
}

void agente::computaSeparacao (vetor v, double pesoSeparacao)
{
    v.mult(pesoSeparacao);          // Multiplica o vetor alinhamento
    this->direcao.add(v);           // pelo coeficiente de multiplicação (peso)
    this->direcao.normalizar();     // e atribui a direção e normaliza.
}

void agente::computaSeparacao (vector <agente *> prd, double pesoSeparacao)
{
    int c = prd.size();                         // Quantidade de tubarões no raio de percepção

    if(c>0)
    {
        vetor v = vetor(0, 0);
        vetor loc;
        vetor mloc = this->getLoc();                            // Localização do peixe

        for(int i=0;i<c;i++)
        {
            loc = prd[i]->getLoc();                             // Localização do tubarão
            double ang = atan2 (mloc.y-loc.y, mloc.x-loc.x);    // Angulo de fuga de peixe em relação ao tubarao

            v.add(vetor(cos (ang), sin (ang)));                 // Vetor de fuga do peixe
        }

        v.div (c);                                              // Divide o vetor de fuga pelo numero de tubaroes
        computaSeparacao(v, pesoSeparacao);                     // Computa direção de fuga do peixe
    }
}

double agente::condContPeixe(double p) // Caso o peixe encontre o limite do mundo, ele "atravessa" para o outro lado
{
    if(p>1) return p-=1;
    if(p<0) return p+=1;
    return p;
}

void agente::condContTuba(vetor v) // Caso o tubarão encontre o limite do mundo, ele quica
{
    if(v.x>1 || v.x<0)
        this->direcao.x*=-1;
    if(v.y>1 || v.y<0)
        this->direcao.y*=-1;
}

void agente::addViz(agente *pag) // adiciona peixe ao campo de vizinhos
{
    this->viz.push_back(pag);
}

void agente::addPrd(agente *pag) // adiciona predador ao campo de vizinhos
{
    this->prd.push_back(pag);
}

void agente::cleanViz() // limpa o campo de percepção de predador
{
    this->viz.clear();
    this->prd.clear();
}

void agente::cleanAlvo() // Limpa o alvo
{
    this->alvo.x=0;
    this->alvo.y=0;
    this->idAlvo=-1;
}

void agente::relogioExplosao() // Calcula tempo de explosão do agente
{
    if(explosao)
    {
        if(tempoExplosao>0)
            tempoExplosao--;
        else
        {
            resetExplosao(intervaloExplosao);
            explosao=false;
            this->cleanAlvo();
        }
    }

}

void agente::relogioExplosaoDelay() // Calcula Tempo de descanso do agente
{
    if(!explosao)
    {

        if(tempoExplosao>0)
            tempoExplosao--;
        else
        {
            resetExplosao(intervaloExplosao*2);
            explosao=true;
        }
    }
}

void agente::resetExplosao(int i) // Reseta o tempo de explosao
{
    this->tempoExplosao=i;
}

double agente::calcDist(vetor posI) // Calcula distancia entre posições
{
    double dist=0;
    vetor posJ=this->getLoc();
    double dx,dy;
    dx=posI.x-posJ.x;
    dy=posI.y-posJ.y;
    dist= sqrt(dx*dx+dy*dy);
    return dist;
}

bool agente::ehPeixe(){ // Verifica se é peixe
    return this->nemo;
}

int agente::getIdAlvo() // Retorna o id do alvo
{
    return this->idAlvo;
}

int agente::getToMorto() const // Retorna se o peixe está morto(0 == vivo; 1 == morrendo; 2 == morto)
{
    return toMorto;
}

double agente::getTam() // Retorna tamanho do agente
{
    return this->tam;
}

double agente::getAlinhamento() // Retorna coeficiente de alinhamento
{
    return this->pesoAlinhamento;
}

double agente::getCoesao() // Retorna coeficiente de coesão
{
    return this->pesoCoesao;
}

double agente::getSeparacao() // Retorna coeficiente de separação
{
    return this->pesoSeparacao;
}

double agente::getVelocidadeBase() // Retorna velocidade base
{
    return this->velocidadeBase;
}

double agente::getRaiViz() const // Retorna raio de percepção
{
    return raiViz;
}

char* agente::getCor() // Retorna cor atual
{
    return this->cor;
}

vetor agente::getAlvo() // Retorna alvo atual
{
    return this->alvo;
}

vetor agente::getLoc() // Retorna localização atual
{
    return localizacao[0];
}

vetor agente::getDirecao() // Retorna direção atual
{
    return this->direcao;
}

vetor agente::getCentroMassa() // Retorna centro de massa atual
{
    int n = this->viz.size();
    if(n > 0)
    {

        agente *ag;
        vetor loc;
        vetor v = vetor(0,0);
        for(int i=0;i<n;i++)
        {
            ag = this->viz[i];
            loc = ag->getLoc();

            v.add(loc);
        }

        v.div(n);

        return v;
    }
    else
        return this->getLoc();
}

vector<vetor> agente::getLocalizacao() // Retorna vetor de localização
{
    return this->localizacao;
}

void agente::setCentroMassa(vetor v) // Atribui valor de Centro de Massa
{
    this->centroMassa = v;
}

void agente::setBaseCor() // Atribui valor de cor base
{
    this->cor = this->baseCor;
}

void agente::setToMorto(int i) // Atribui valor de morte/vida do agente
{
    toMorto = i;
}

void agente::setAlinhamento(double i) // Atribui valor de coeficiente de alinhamento
{
    this->pesoAlinhamento = i;
}
void agente::setCoesao(double i) // Atribui valor de coeficiente de coesão
{
    this->pesoCoesao = i;
}
void agente::setSeparacao(double i) // Atribui valor de coeficiente de separação
{
    this->pesoSeparacao = i;
}

void agente::setVetorAlinhamento(vetor v) // Atribui valor de vetor de alinhamento
{
    this->alinhamento = v;
}

void agente::setVetorCoesao(vetor v) // Atribui valor de vetor de coesão
{
    this->coesao = v;
}

void agente::setVetorSeparacao(vetor v) // Atribui valor de vetor de separação
{
    this->separacao = v;
}

void agente::setVelocidade(double i) // Atribui valor de velocidade
{
    if(i == 0)
        this->velocidade = this->velocidadeBase;
    else
        this->velocidade = i;
}
void agente::setAlerta() // Dispara o alerta do peixe em caso de tubarão
{
    this->alerta=true;
}

void agente::setAlvo(int i, vetor v) // Atribui alvo ao tubarão
{
    this->idAlvo = i;
    this->alvo = v;
}

void agente::setCor () // Atribui uma cor aleatória entre quatro valores de cor
{
    int a = (int) rand() % 4;
    char* c;

    switch (a) {
    case 0:
        c = "#ec582f";
        break;
    case 1:
        c = "#f39052";
        break;
    case 2:
        c = "#f7ab95";
        break;
    default:
        c = "#ecc875";
        break;
    }

    this->cor = c;
}

void agente::setCor (char* c) // Atribui valor de cor
{
    this->cor = c;
}
