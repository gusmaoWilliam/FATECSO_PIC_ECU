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

    tempo = new QTimer(this);                                           //Timer tempo: usado para contar tempo de aquisição
    tempoExcel = new QTimer(this);                                      //Tempo para salvar dados no Excel
    connect(tempo, SIGNAL(timeout()), this, SLOT(minhaFuncao()));       //Timeout do Timer tempo chama a função "minhaFuncao"
    connect(tempoExcel, SIGNAL(timeout()), this, SLOT(funcaoExcel()));  //Timeout do Timer tempoExcel chama a função "funcaoExcel"




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


/*  Função que fecha a janela ao clicar no X*/
MainWindow::~MainWindow()
{
    delete ui;
}


/* Função que lê os dados do buffer Serial e retorna em formato QByteArray */
QByteArray MainWindow::ReadData()  // AppGuiArduino
{
    QByteArray data = procSerial->Read();
    qDebug() << "RX UART: " << data << endl;
    return data;
}

/* Função que envia um buffer char com um tamanho definido pela Serial */
void MainWindow::WriteData(const char* data, int len)
{
    procSerial->Write(data, len);
    qDebug() << "TX UART: " << data << endl;
}


/*  Método referente ao botão "ABRIR" */
void MainWindow::on_abrir_button_clicked()
{
    if(!flagSerial) // Se a flag estiver em 0, ou seja, a comunicação serial está fechada
    {

        bool statusOpenSerial;

       statusOpenSerial = procSerial->Conectar(ui->portas_box->currentText(),
                                               ui->baud_box->currentText().toInt()); // Conecta-se a porta COM selecionada
                                                                                     // com o baud rate selecionado

       if (statusOpenSerial)    // Se foi possivel se conectar À porta COM
       {
           ui->abrir_button->setText("FECHAR");    //Altera o texto do botão para: "FECHAR"
           ui->textEdit->append("### Porta serial aberta com sucesso!");
           flagSerial = 1;      // Seta a flag que indica agora que a comunicação serial está aberta
       }
       else
       {
           ui->textEdit->append("Falha ao abrir conexão serial."); //Mostra no console a falha
       }
    }
    else    //Se a flag estiver setada, ou seja, comunicação serial aberta
    {
        bool statusCloseSerial;

        statusCloseSerial = procSerial->Desconectar(); // fecha a comunicação serial

        /*
         * Descontando a porta serial com sucesso
         */
        if (statusCloseSerial)  // Se foi possivel fechar a comunicação serial
        {
            ui->abrir_button->setText("ABRIR"); // altera o texto do botao
            ui->textEdit->append("### Porta serial fechada com sucesso!");
            flagSerial = 0; //Reseta flag que agora indica que a comunicação serial está fechada
        }
        else {
            ui->textEdit->append("### Falha ao fechar conexão serial.");
        }
    }
}

void MainWindow::on_aquisitar_button_clicked()
{
    if(flagSerial)      // se a comunicação esta aberta
    {
        if(!flagAquisicao) // flag em zero, significa que a aquisição nao está ocorrendo
        {
            if(ui->tempAq_box->currentText() == "10ms") // verifica o tempo selecionado na box do tempo de aquisição
            {
                tempo->start(10);   // inicia o timer com o tempo selecionado
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
            ui->aquisitar_button->setText("PARAR"); //altera o texto do botao "aquisitar" para "parar"
            flagAquisicao = 1;                          //seta flag que agora indica que a aquisição de dados esta ativada

        }
        else  //flag setada, ou seja, aquisição ativada
        {
            ui->textEdit->append("### A aquisição de dados foi Interrompida!");
            tempo->stop();  //Para a aquisiçaõ de dados
            flagAquisicao = 0;  // reseta flag que agora indica que a aquisicao nao esta sendo feita
            ui->aquisitar_button->setText("AQUISITAR"); //altera o texto do botao de "PARAR" para "aquisitar"
        }
    }
    else    // se a comunicaçao serial nao estiver aberta, manda uma mensagem no console apenas
    {
        ui->textEdit->append("### Conecte-se à uma PORTA SERIAL!");
    }
}

/* FUNÇÃO CHAMADA DEVIDO AO TIMEOUT DO TIMER "tempo" */
void MainWindow::minhaFuncao() //FUNÇÃO PARA MANDAR AO uC o comando, seja 'R' ou 'E', para em seguida aguardar receber os dados dos sensores
{
    if(flagExcel == 1 && flagAuxExcel == 0) // flag que indica que os dados serão salvos e a flag que garante que só irá
    {                                       // entrar nesse if uma unica vez enquanto o timeout do tempoExcel nao estourar
                                            // a Aux também indica que o tempoExcel está contando

        flagAuxExcel = 1;                   // seta flag para o programa nao entrar mais nesse if antes do timeout
        if(ui->timeExcelBox->currentText() == "1s") // verifica o tempo selecionado no box do tempo do excel
            tempoExcel->start(1000);                // inicia o timer com o tempo selecionado
        else if(ui->timeExcelBox->currentText() == "5s")
            tempoExcel->start(5000);
        else if(ui->timeExcelBox->currentText() == "10s")
            tempoExcel->start(10000);
        else if(ui->timeExcelBox->currentText() == "30s")
            tempoExcel->start(30000);
        else if(ui->timeExcelBox->currentText() == "60s")
            tempoExcel->start(60000);
    }

    //declaração de variaveis referentes aos dados de aquisição recebidos
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

    QByteArray sensoresTemp; // Array que irá guardar temporariamente o buffer com os dados recebidos da ECU PIC
    WriteData("R", 1);       // envia o caractere "R" seguido do caractere "2" para a ECU PIC
    usleep(100);
    WriteData("2", 1);
    usleep(1000);           //aguarda um pequeno tempo para que a ECU tenha tempo de processar e enviar os dados
    sensoresTemp = ReadData();   // guarda os dados recebidos via serial no Array
    //while(ReadData() =! NULL)
    if(sensoresTemp != "LIXO")  // Método adotado para identificar se os dados recebidos são válidos
    {                           // foi alterado na funçao da biblioteca Serial utilizada para retornar "LIXO" caso os dados sejam inválidos
        sensores = sensoresTemp; // passa os dados do buffer temporario para o buffer principal
    }
    usleep(TEMP_SERIAL);
    rotacao =  (a(sensores[0]) << 8) + a(sensores[1]); //rotaciona os bits dos dados recebidos para converter um 2 bytes
                                                       // em um unico valor de 16 bits, passando para variavel
                                                       // referente a cada dado. Os dados que podem ser maiores que 255 foram rotacionados,
                                                       // enquanto que os dados <= 255 foram apenas atribuidos

    MAP =(a(sensores[2]) << 8) + a(sensores[3]);
    angle = (a(sensores[4]) << 8) + a(sensores[5]);
    tempAr = a(sensores[6]);
    tempAgua = a(sensores[7]);
    lambda = a(sensores[8]);
    tempInj = (a(sensores[9]) << 8) + a(sensores[10]);
    dente14 = a(sensores[11]);
    dentePer = a(sensores[12]);

    QString testeAngulo;

    pressao_ar = ((MAP * 49) - 386) / 100;  // Calculo extraido da ECU PIC para a pressão do ar advinda do sensor MAP
    angulo = (double)angle/10;              // dado de angulo foi multiplicado por 10 antes de ser enviado pela ECU

    f_tempInj = tempInj/1000;               // Tempo de injeçao foi recebido em us,e  sera mostrado em ms



    ui->rotacao_line->setText(QString("%1").arg(rotacao)); // mostra os dados recebidos nas caixas do seu respectivo dado
                                                           // na interface
    ui->map_line->setText(QString("%1").arg(pressao_ar));
    ui->ang_line->setText(QString::number(angulo));
    ui->tempAr_line->setText(QString("%1").arg(tempAr));
    ui->tempAgua_line->setText(QString("%1").arg(tempAgua));
    ui->lambda_line->setText(QString("%1").arg(lambda));
    ui->tempInj_line->setText(QString::number(f_tempInj));
    ui->lineEdit_4->setText(QString("%1").arg(dente14));
    ui->lineEdit_6->setText(QString("%1").arg(dentePer));

    if(flagAuxExcel)        // setada significa que foi dado um start no tempoExcel, porém ainda não deu timeout
    {
        planilha[0][coluna] = QString("%1").arg(tempoExcel->interval() - tempoExcel->remainingTime()); // tempo passado desde que foi iniciado
                                                                                                       // o tempo de salvar os dados

        planilha[1][coluna] = ui->rotacao_line->text();  // salva os dados a serem salvos em uma matriz de linha x coluna
        planilha[2][coluna] = ui->ang_line->text();
        planilha[3][coluna] = ui->tempAr_line->text();
        planilha[4][coluna] = ui->tempAgua_line->text();
        planilha[5][coluna] = ui->lambda_line->text();
        planilha[6][coluna] = ui->tempInj_line->text();
    }
    if(flagAuxExcel) // se o tempo ainda está passanndo, vai pra proxima coluna
    {
        coluna++;
    }
}


/* Método do botoã que envia parametro de calibração referente ao RPM de marcha lenta */
void MainWindow::on_pushButton_clicked()
{
    unsigned int RPM_MLenta = ui->lineCalibMLenta->text().toInt(); // salva na variavel o valor que foi digitado na caixa
    QByteArray teste;
    QByteArray parametros;
  //  teste[0] = 1;
    teste[0] = RPM_MLenta & 0xFF;   //guarda os 8 bits menos significativos do parametro RPM_Lenta
    parametros[0] = (RPM_MLenta >> 8) & 0xFF; // guarda os 8 bits mais significativos

    WriteData("E", 1);  // envia o caractere "E" seguido do caractere "1" para a ECU

    usleep(TEMP_SERIAL);
    WriteData("1", 1);

    usleep(TEMP_SERIAL);

    WriteData(parametros,1);   // envia byte mais significativo seguido do byte menos significativo do parametro RPM_Lenta
    usleep(TEMP_SERIAL);
    WriteData(teste, 1);
}

/* Método do botão que serve para receber os defaults de calibração da ECU */
void MainWindow::on_defaultsButton_clicked()
{
    unsigned int MLentaRef;                     //variavel referente a referencia do rpm de marcha lenta
    QByteArray calibraveisTemp, calibraveis;
    WriteData("R", 1);          // envia para a ECU os caracteres "R" E "6" respectivamente
    usleep(TEMP_SERIAL);
    WriteData("6", 1);
    usleep(TEMP_SERIAL);
    usleep(TEMP_SERIAL);

    calibraveisTemp = ReadData();   // Mesmo método de aquisição para receber os dados e verificar se os mesmos são validos

    if(calibraveisTemp != "LIXO")
    {
        calibraveis = calibraveisTemp;
    }
    MLentaRef = (a(calibraveis[1]) << 8) | (a(calibraveis[0]) & 0xFF); // dados recebidos em 2 bytes para um dado de 16 bits
    ui->lineRefMLenta->setText(QString("%1").arg(MLentaRef));          // mostra o valor default obtido de RPM Mlenta na caixa do default MLenta
}

/* Método referente ao botão "SALVAR DADOS" para salvar dados em uma planilha Excel */
void MainWindow::on_salvar_button_clicked()
{
    flagExcel = 1; // Seta a flag que indica que será iniciado o timer para salvar dados na planilha
}

/* Função chamada ao dar timeout no tempoExcel */
void MainWindow::funcaoExcel()
{
    tempoExcel->stop(); // para o timer tempoExcel
    flagExcel = 0;      // Reseta as flags referentes a excel
    flagAuxExcel = 0;
    QString col[coluna];    //variavel auxiliar para mostrar numero das colunas na planilha
    QXlsx::Document plan;   // cria o documento .xls

    plan.write("A1", "Tempo");      //salva o cabeçalho na planilha
    plan.write("B1", "Rotacao");
    plan.write("C1", "Angle");
    plan.write("D1", "Temperatura Ar");
    plan.write("E1", "Temperatura Agua");
    plan.write("F1", "Lambda");
    plan.write("G1", "Tempo Injecao");

    for(int i = 0; i < coluna; i++) // salva a matriz "planilha" a qual foi atribuida valores anteriormente no arquivo Excel
    {
        col[i] = QString("%1").arg(i+2);        //numero da coluna para caractere
        plan.write("A"+col[i], planilha[0][i]); // salva no arquivo .xls

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
    plan.saveAs(QDir::currentPath()+"\\Aquisicao.xlsx"); // Salva o arquivo na pasta "debug" ou "release", dependendo
                                                         // do modo de compilação

    ui->textEdit->append("### Dados Salvos com Sucesso!!!");

    flagExcel = 0;      // reseta as flags
    flagAuxExcel = 0;
    coluna = 0;             // zera a coluna

}
