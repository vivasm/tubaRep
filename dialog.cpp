#include "dialog.h"
#include "ui_dialog.h"
#include <QPainter>
#include <QTimer>
#include "vetor.h"
#include <iostream>

using namespace std;

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::update()
{
    this->oceano.roda();
    this->repaint();
    this->ui->alinhamentoBox->setValue(this->oceano.getAlAg(0));
    this->ui->coesaoBox->setValue(this->oceano.getCoAg(0));
    this->ui->separacaoBox->setValue(this->oceano.getSeAg(0));
}

void Dialog::on_pushButtonRun_clicked()
{
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(50);
}

int map_value(int valor, int min1, int max1, int min2, int max2) {
    return min2 + (max2 - min2) * (valor - min1) / (max1 - min1);
}

void Dialog::paintEvent(QPaintEvent *e)
{
    QPainter estojo;
    estojo.begin(this);
    int tamJx=this->geometry().width();
    int tamJy=this->geometry().height();
    double fx=(double)tamJx/this->oceano.getTamM();
    double fy=(double)tamJy/this->oceano.getTamM();
    this->ui->spinBoxPopSa->setValue(this->oceano.getNAg());

    this->setStyleSheet("background-color:#f3e6cc;");

    vetor pm;
    double s, sz;
    char* cor;
    vector<vetor> localizacao;
    estojo.setPen(QColor(0,0,0,0));
    for(int i=0;i<this->oceano.getSAg();i++)
    {

        if(this->oceano.getMoAg(i) == 0)
        {
            pm=this->oceano.getPosAg(i);
            sz=this->oceano.getTamAg(i);
            cor=this->oceano.getCorAg(i);
            localizacao = this->oceano.getPosCorpo(i);

            for(int j = 0; j < localizacao.size(); j++){
                pm=localizacao[j];
                s = map_value(localizacao.size()-j-1, 0, localizacao.size(), 1, (int) sz );
                estojo.save();
                estojo.translate(pm.x*fx,pm.y*fy);

                estojo.setBrush(QColor(cor));
                estojo.drawEllipse(QPoint(0,0),s,s);

                estojo.restore();
            }
        }
    }

    estojo.end();
}

void Dialog::on_alinhamentoBox_valueChanged(double arg1)
{
    this->oceano.setAlAg(arg1);
}

void Dialog::on_coesaoBox_valueChanged(double arg1)
{
    this->oceano.setCoAg(arg1);
}

void Dialog::on_separacaoBox_valueChanged(double arg1)
{
    this->oceano.setSeAg(arg1);
}
