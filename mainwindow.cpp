#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QNetworkInterface>
#include <QHostAddress>
#include <QHostInfo>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QPoint pos( X_POSITION_FRAME, Y_POSITION_FRAME );
    this->move( pos );
    ui->setupUi( this );
    setWindowTitle("Квест Сервер");
    makeLogNote("Начало работы");
    udpSocket.bind( LISTERNING_PORT );
    myIp.setAddress("172.16.0.139");
    //myIp.setAddress("212.98.167.242");
    targetIp.setAddress("127.0.0.1");
    makeLogNote(localIP());
    ui->stackedWidget->setCurrentIndex(1);
    connect(&udpSocket, SIGNAL(readyRead()), this, SLOT(getMessage()));

    ui->variantsTextEdit->setColumnCount(2);
    ui->variantsTextEdit->setRowCount(10);
    ui->variantsTextEdit->setHorizontalHeaderLabels({"Команда", "Гиперссылка"});
    ui->variantsTextEdit->setShowGrid(true);
    ui->variantsTextEdit->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->variantsTextEdit->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->variantsTextEdit->verticalHeader()->hide();
    ui->variantsTextEdit->horizontalHeader()->setStretchLastSection(true);

    bag = {true, true, true, true, false, false};
    bagItemsNames = {"Пачка печенья", "Вилка", "Косметика", "Белорусские рубли", "Кольцо на интеллект", "Газовый ключ"};
    sum = 1000;
    step = 0;
    playerName = "0";
    //0 - приемная
    //1 - кабинет директора
    //2 - ресепшн
    //3 - озеро
    //4 - коморка охранников
    //5 - туалет
    //6 - магазин
    //7 - улица
    //8 - кладовка
    //9 - концерт
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::makeLogNote(QString s)
{
    ui->logField->append( tr( "%1 %2" ).arg(QTime::currentTime().toString("hh:mm:ss"))
                          .arg(s));
}

void MainWindow::sendPage(QString pageAddress)
{
    QString str = ":/resources/";
    str.append(pageAddress);
    possibleLinks.clear();
    QDomDocument domDoc;
    QFile file(str);
    if(file.open(QIODevice::ReadOnly)) {
        if(domDoc.setContent(&file)) {
            QDomNode domNode = domDoc.documentElement().firstChild();
            while(!domNode.isNull()) {
                if(domNode.isElement()) {
                    QDomElement domElement = domNode.toElement();
                    if(!domElement.isNull()) {
                        sendElement(domElement.text());
                        if(domElement.tagName() == "Item") {
                            possibleLinks.push_back(domElement.attribute("hyperLink", ""));
                        }
                    }
                }
                domNode = domNode.nextSibling();
            }
        }
        file.close();
    }
}

void MainWindow::sendElement(QString element)
{
    QByteArray datagram;
    datagram.append(element);
    udpSocket.writeDatagram(datagram, targetIp, targetPort.toLong(Q_NULLPTR, 10));
    makeLogNote("отправлен пакет");
}

void MainWindow::getMessage()
{
    QByteArray datagram;
    datagram.resize( udpSocket.pendingDatagramSize() );
    udpSocket.readDatagram( datagram.data(), datagram.size() );
    QString str(datagram);
    makeLogNote("Получено - " + str);
    mainController(str);
}

void MainWindow::mainController(QString message)
{
    QString nextPage;
    if (playerName == "0" || message == "-1") {
        playerName = message;
        nextPage = "Intro_1.xml";
    }
    else {
        nextPage = possibleLinks.at(message.toInt(Q_NULLPTR, 10));

    }
    if (nextPage == "mainMenu.xml") {
        step++;
    }
    sendPage(nextPage);
}

QString MainWindow::localIP()
{
  /*QString locIP;
  QList<QHostAddress> addr = QNetworkInterface::allAddresses();
  locIP = addr.first().toString();*/
    QString ipAddress ="";
    QHostInfo info = QHostInfo::fromName( QHostInfo::localHostName() );
    QList<QHostAddress> listAddr= info.addresses();
    for( int i = 0; i < listAddr.size(); i ++ )
    {
        if( listAddr.at(i).protocol() == QAbstractSocket::IPv4Protocol )
        {
            ipAddress += " | ";
            ipAddress += listAddr.at(i).toString();
        }
    }
    return ipAddress;
}

void MainWindow::on_addButton_clicked()
{

}
