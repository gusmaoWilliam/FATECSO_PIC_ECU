#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <unistd.h>
#include <string.h>
#include <QFile>
#include <xlsxdocument.h>
#include <QDir>

#define TEMP_SERIAL 500
#define a static_cast<unsigned char>
bool flagAuxExcel = 0;
QString planilha[7][6000];
bool flagSerial = 0;
bool flagAquisicao = 0;
QByteArray sensores;
QByteArray dataAnterior;
unsigned char flagExcel = 0;
unsigned int coluna = 0;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{


    ui->setupUi(this);

    tempo = new QTimer(this);
    tempoExcel = new QTimer(this);
    connect(tempo, SIGNAL(timeout()), this, SLOT(minhaFuncao()));
    connect(tempoExcel, SIGNAL(timeout()), this, SLOT(funcaoExcel()));




    /* Create Object the Class QSerialPort*/
    devserial = new QSerialPort(this);

    /* Create Object the Class comserial to manipulate read/write of the my way */
    procSerial = new comserial(devserial);

    /* Load Device PortSerial available */
    QStringList DispSeriais = procSerial->CarregarDispositivos();

    /* Inserting in ComboxBox the Devices */
    ui->portas_box->addItems(DispSeriais);

    /* Enable PushButton "Conectar" if any port is found.
     * If an error occurs, it is reported in the Log
     */
    if(DispSeriais.length() > 0)
    {
        ui->textEdit->append("### Porta serial pronta para ser utilizada.");
    }
    else
    {
        ui->textEdit->append("### Nenhuma porta serial foi detectada!");
    }


    /* Connect Objects -> Signal and Slots
     * DevSerial with Read Data Serial
     * TextEdit "teLog" with getData() after send data WriteData() [Not apply here in version 5.X]
     */
    connect(devserial, SIGNAL(readyRead()), this, SLOT(ReadData()));

}

MainWindow::~MainWindow()
{
    delete ui;
}


/*
void MainWindow::WriteData(const QByteArray data)
{
    procSerial->Write(data);
}
*/

QByteArray MainWindow::ReadData()  // AppGuiArduino
{
    QByteArray data = procSerial->Read();
    qDebug() << "RX UART: " << data << endl;
    return data;
}
void MainWindow::WriteData(const char* data, int len)
{
    procSerial->Write(data, len);
    qDebug() << "TX UART: " << data << endl;
}
/*
QByteArray MainWindow::ReadData()
{
    QByteArray data = procSerial->Read();

    //if(data != ""){
        qDebug() << "RX UART: " << data << endl;

            return data;
            dataAnterior = data;
  //  }
    //else
  //  {
        return dataAnterior;
//    }
}
*/
void MainWindow::on_abrir_button_clicked()
{
    if(!flagSerial)
    {

        bool statusOpenSerial;

       statusOpenSerial = procSerial->Conectar(ui->portas_box->currentText(),
                                               ui->baud_box->currentText().toInt());

       if (statusOpenSerial)
       {
           ui->abrir_button->setText("FECHAR");
           ui->textEdit->append("### Porta serial aberta com sucesso!");
           flagSerial = 1;
       }
       else
       {
           ui->textEdit->append("Falha ao abrir conexão serial.");
       }
    }
    else
    {
        bool statusCloseSerial;

        statusCloseSerial = procSerial->Desconectar();

        /*
         * Descontando a porta serial com sucesso
         */
        if (statusCloseSerial)
        {
            ui->abrir_button->setText("ABRIR");
            ui->textEdit->append("### Porta serial fechada com sucesso!");
            flagSerial = 0;
        }
        else {
            ui->textEdit->append("### Falha ao fechar conexão serial.");
        }
    }
}

void MainWindow::on_aquisitar_button_clicked()
{
    if(flagSerial)
    {
        if(!flagAquisicao)
        {
            if(ui->tempAq_box->currentText() == "10ms")
            {
                tempo->start(10);
                ui->textEdit->append("### Tempo de aquisição: 10ms.");
            }
            else if(ui->tempAq_box->currentText() == "50ms")
            {
                tempo->start(50);
                ui->textEdit->append("### Tempo de aquisição: 50ms.");
            }
            else if(ui->tempAq_box->currentText() == "100ms")
            {
                tempo->start(100);
                ui->textEdit->append("### Tempo de aquisição: 100ms.");
            }
            else if(ui->tempAq_box->currentText() == "500ms")
            {
                tempo->start(500);
                ui->textEdit->append("### Tempo de aquisição: 500ms.");
            }
            else if(ui->tempAq_box->currentText() == "1s")
            {
                tempo->start(1000);
                ui->textEdit->append("### Tempo de aquisição: 1s.");
            }
            else if(ui->tempAq_box->currentText() == "3s")
            {
                tempo->start(3000);
                ui->textEdit->append("### Tempo de aquisição: 3s.");
            }
            ui->aquisitar_button->setText("PARAR");
            flagAquisicao = 1;

        }
        else
        {
            ui->textEdit->append("### A aquisição de dados foi Interrompida!");
            tempo->stop();
            flagAquisicao = 0;
            ui->aquisitar_button->setText("AQUISITAR");
        }
    }
    else
    {
        ui->textEdit->append("### Conecte-se à uma PORTA SERIAL!");
    }
}
void MainWindow::minhaFuncao() //FUNÇÃO PARA MANDAR AO uC o comando, seja 'R' ou 'E', para em seguida aguardar receber os dados dos sensores
{
    if(flagExcel == 1 && flagAuxExcel == 0)
    {
        flagAuxExcel = 1;
        if(ui->timeExcelBox->currentText() == "1s")
            tempoExcel->start(1000);
        else if(ui->timeExcelBox->currentText() == "5s")
            tempoExcel->start(5000);
        else if(ui->timeExcelBox->currentText() == "10s")
            tempoExcel->start(10000);
        else if(ui->timeExcelBox->currentText() == "30s")
            tempoExcel->start(30000);
        else if(ui->timeExcelBox->currentText() == "60s")
            tempoExcel->start(60000);
    }

    unsigned int rotacao;
    int MAP;
    int tempAr;
    int tempAgua;
    unsigned int angle;
    int lambda;
    unsigned int tempInj;
    unsigned int avanco;
    double f_tempInj;
    double pressao_ar, angulo;
    unsigned char dente14, dentePer, denteExtra;

    QByteArray sensoresTemp;
    WriteData("R", 1);
    usleep(100);
    WriteData("2", 1);
    usleep(1000);
    sensoresTemp = ReadData();
    //while(ReadData() =! NULL)
    if(sensoresTemp != "LIXO")
    {
        sensores = sensoresTemp;
    }
    usleep(TEMP_SERIAL);
    //sensores[0] = 0b10000001;
    //sensores[1] = 0b11111111;
    rotacao =  (a(sensores[0]) << 8) + a(sensores[1]);


    //rotacao = (unsigned int)(sensores[0] << 8) + sensores[1];

    MAP =(a(sensores[2]) << 8) + a(sensores[3]);
    angle = (a(sensores[4]) << 8) + a(sensores[5]);
    tempAr = a(sensores[6]);
    tempAgua = a(sensores[7]);
    lambda = a(sensores[8]);
    tempInj = (a(sensores[9]) << 8) + a(sensores[10]);
    dente14 = a(sensores[11]);
    dentePer = a(sensores[12]);
   // velocidade = a(sensores[11]);
    QString testeAngulo;
    pressao_ar = ((MAP * 49) - 386) / 100; //pressao_ar = ((map*49) - 386)/100;
    angulo = (double)angle/10;

    f_tempInj = tempInj/1000;


    ui->rotacao_line->setText(QString("%1").arg(rotacao));

    ui->map_line->setText(QString("%1").arg(pressao_ar));
    ui->ang_line->setText(QString::number(angulo));
    ui->tempAr_line->setText(QString("%1").arg(tempAr));
    ui->tempAgua_line->setText(QString("%1").arg(tempAgua));
    ui->lambda_line->setText(QString("%1").arg(lambda));
    ui->tempInj_line->setText(QString::number(f_tempInj));
    ui->lineEdit_4->setText(QString("%1").arg(dente14));
    ui->lineEdit_6->setText(QString("%1").arg(dentePer));
    if(flagAuxExcel)
    {
        planilha[0][coluna] = QString("%1").arg(tempoExcel->interval() - tempoExcel->remainingTime());
        planilha[1][coluna] = ui->rotacao_line->text();
        planilha[2][coluna] = ui->ang_line->text();
        planilha[3][coluna] = ui->tempAr_line->text();
        planilha[4][coluna] = ui->tempAgua_line->text();
        planilha[5][coluna] = ui->lambda_line->text();
        planilha[6][coluna] = ui->tempInj_line->text();
    }
  //  ui->veloc_line->setText(QString("%1").arg(velocidade));
    if(flagAuxExcel)
    {
        coluna++;
    }
}

void MainWindow::on_pushButton_clicked()
{
    unsigned int RPM_MLenta = ui->lineCalibMLenta->text().toInt();
    QByteArray teste;
    QByteArray parametros;
  //  teste[0] = 1;
    teste[0] = RPM_MLenta & 0xFF;
    parametros[0] = (RPM_MLenta >> 8) & 0xFF;
   // tempo->stop();
    WriteData("E", 1);
   // usleep(100);
    usleep(TEMP_SERIAL);
    WriteData("1", 1);
    //usleep(100);
    usleep(TEMP_SERIAL);
    //WriteData(parametros, 2);
    WriteData(parametros,1);
    usleep(TEMP_SERIAL);
    WriteData(teste, 1);
    //WriteData(RPM_MLenta, 1);
   // on_aquisitar_button_clicked();
}

void MainWindow::on_defaultsButton_clicked()
{
    unsigned int MLentaRef;
    QByteArray calibraveisTemp, calibraveis;
    WriteData("R", 1);
    usleep(TEMP_SERIAL);
    WriteData("6", 1);
    usleep(TEMP_SERIAL);
    usleep(TEMP_SERIAL);
    calibraveisTemp = ReadData();
    //while(ReadData() =! NULL)
    if(calibraveisTemp != "LIXO")
    {
        calibraveis = calibraveisTemp;
    }
    MLentaRef = (a(calibraveis[1]) << 8) | (a(calibraveis[0]) & 0xFF);
    ui->lineRefMLenta->setText(QString("%1").arg(MLentaRef));

}

void MainWindow::on_excelButton_clicked()
{
    /*
    QXlsx::Document planilha;

    planilha.write("A1", "LUCAS ");
    planilha.write("A2", ui->lineExcel->text());
    planilha.saveAs(QDir::currentPath()+"\\Lucas.xlsx");
    */
}

void MainWindow::on_salvar_button_clicked()
{
    flagExcel = 1;
}
void MainWindow::funcaoExcel()
{
    tempoExcel->stop();
    flagExcel = 0;
    flagAuxExcel = 0;
    QString col[coluna];
    QXlsx::Document plan;

    plan.write("A1", "Tempo");
    plan.write("B1", "Rotacao");
    plan.write("C1", "Angle");
    plan.write("D1", "Temperatura Ar");
    plan.write("E1", "Temperatura Agua");
    plan.write("F1", "Lambda");
    plan.write("G1", "Tempo Injecao");

    for(int i = 0; i < coluna; i++)
    {
        col[i] = QString("%1").arg(i+2);
        plan.write("A"+col[i], planilha[0][i]);

        col[i] = QString("%1").arg(i+2);
        plan.write("B"+col[i], planilha[1][i]);

        col[i] = QString("%1").arg(i+2);
        plan.write("C"+col[i], planilha[2][i]);

        col[i] = QString("%1").arg(i+2);
        plan.write("D"+col[i], planilha[3][i]);

        col[i] = QString("%1").arg(i+2);
        plan.write("E"+col[i], planilha[4][i]);

        col[i] = QString("%1").arg(i+2);
        plan.write("F"+col[i], planilha[5][i]);

        col[i] = QString("%1").arg(i+2);
        plan.write("G"+col[i], planilha[6][i]);
    }
    plan.saveAs(QDir::currentPath()+"\\Aquisicao.xlsx");

    ui->textEdit->append("### Dados Salvos com Sucesso!!!");

    flagExcel = 0;
    flagAuxExcel = 0;
    coluna = 0;

}
