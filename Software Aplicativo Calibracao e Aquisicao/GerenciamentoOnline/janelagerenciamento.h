#ifndef JANELAGERENCIAMENTO_H
#define JANELAGERENCIAMENTO_H

#include <QMainWindow>

namespace Ui {
class JanelaGerenciamento;
}

class JanelaGerenciamento : public QMainWindow
{
    Q_OBJECT

public:
    explicit JanelaGerenciamento(QWidget *parent = nullptr);
    ~JanelaGerenciamento();

private:
    Ui::JanelaGerenciamento *ui;
};

#endif // JANELAGERENCIAMENTO_H
