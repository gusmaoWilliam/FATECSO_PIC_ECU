#include "janelagerenciamento.h"
#include "ui_janelagerenciamento.h"

JanelaGerenciamento::JanelaGerenciamento(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::JanelaGerenciamento)
{
    ui->setupUi(this);
}

JanelaGerenciamento::~JanelaGerenciamento()
{
    delete ui;
}
