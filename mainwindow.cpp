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
    connect(ui->variantsTextEdit, SIGNAL(cellChanged(int,int)), this, SLOT(resize()));

    ui->variantsTextEdit->setColumnCount(2);
    ui->variantsTextEdit->setRowCount(1);
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
    ui->variantsTextEdit->setRowCount(ui->variantsTextEdit->rowCount()+1);
}

void MainWindow::on_saveButton_clicked()
{
    QString fileName = "C:/QTWork/QTQuestServer/resources/";
    fileName = fileName.append(ui->fileNameLine->text());
    fileName = fileName.append(".xml");
    QDomDocument doc("QuestPage");
        QDomElement  domElement = doc.createElement("QuestPage");
        doc.appendChild(domElement);

        QDomElement contact = doc.createElement("MainText");
        QDomText domText = doc.createTextNode(ui->mainTextEdit->toPlainText().toUtf8());
        contact.appendChild(domText);
        domElement.appendChild(contact);
        contact = doc.createElement("PagePicture");
        domText = doc.createTextNode(ui->pictureLineEdit->text().toUtf8());
        contact.appendChild(domText);
        domElement.appendChild(contact);

        for (int i = 0; i < ui->variantsTextEdit->rowCount(); i++) {
            contact = doc.createElement("Item");
            domText = doc.createTextNode(ui->variantsTextEdit->item(i,0)->text().toUtf8());
            contact.appendChild(domText);
            QDomAttr domAttr = doc.createAttribute("hyperLink");
            domAttr.setValue(ui->variantsTextEdit->item(i,1)->text().toUtf8());
            contact.setAttributeNode(domAttr);
            domElement.appendChild(contact);
        }
        QFile file(fileName);
        if(file.open(QIODevice::WriteOnly)) {
            QTextStream(&file) << doc.toString();
            file.close();
        }
}

void MainWindow::resize()
{
    ui->variantsTextEdit->resizeColumnsToContents();
}
