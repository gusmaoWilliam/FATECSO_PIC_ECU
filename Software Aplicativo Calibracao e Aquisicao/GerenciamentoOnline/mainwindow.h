#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <stdio.h>
#include "comserial.h"
#include "janelagerenciamento.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void minhaFuncao();
    void funcaoExcel();

private slots:
    void WriteData(const char* data, int len);
    QByteArray ReadData();

   // void WriteData(const QByteArray data);

   // QByteArray ReadData();

    void on_abrir_button_clicked();

    void on_aquisitar_button_clicked();

    void on_pushButton_clicked();

    void on_defaultsButton_clicked();

    void on_excelButton_clicked();

    void on_salvar_button_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *tempo;
    QTimer *tempoExcel;

    bool PaletaLogBW;

    QSerialPort *devserial;
    comserial *procSerial;

    void CarregarInfoDispSerial(void);
};

#endif // MAINWINDOW_H
