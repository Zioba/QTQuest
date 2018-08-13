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
    myIp.setAddress("127.0.0.1");
    targetIp.setAddress("127.0.0.1");
    makeLogNote(localIP());
    ui->stackedWidget->setCurrentIndex(0);
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

    initGame();
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

void MainWindow::makeLogNoteQ(QString s)
{
    ui->questInformation->append( tr( "%1 %2" ).arg(QTime::currentTime().toString("hh:mm:ss"))
                          .arg(s));
}

void MainWindow::sendPage(QString pageAddress)
{
    //QString str = ":/resources/";
    QString str = "C:/QTWork/QTQuestServer/resources/";
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
                        if (domElement.text() == "Сходить на концерт.") {
                            if ((step%5 == 3) || (step%5 == 4)) {
                                sendElement(domElement.text());
                            }
                        }
                        else {
                            sendElement(domElement.text());
                        }
                        if(domElement.tagName() == "Item") {
                            if (newMove) {
                                QString s = domElement.attribute("hyperLink", "");
                                if (s == "10_concert/Concert") {
                                    if ((step%5 == 3) || (step%5 == 4)) {
                                        s.append(pripiska);
                                        possibleLinks.push_back(s);
                                    }
                                }
                                else {
                                    s.append(pripiska);
                                    possibleLinks.push_back(s);
                                }
                            }
                            else {
                                possibleLinks.push_back(domElement.attribute("hyperLink", ""));
                            }
                        }
                    }
                }
                domNode = domNode.nextSibling();
            }
        }
        file.close();
    }
    if (newMove) { newMove = false; }
    sendElement("#");
    sendBag();
}

void MainWindow::sendElement(QString element)
{
    QByteArray datagram;
    datagram.append(element);
    udpSocket.writeDatagram(datagram, targetIp, targetPort.toLong(Q_NULLPTR, 10));
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
    if (message == "-1" || nextPage == "Intro_1.xml") {
        initGame();
        playerName = message;
        nextPage = "Intro_1.xml";
    }
    else {
        nextPage = possibleLinks.at(message.toInt(Q_NULLPTR, 10));
    }
    if (nextPage == "Main_menu.xml") {
        newMove = true;
        step++;
        switch (step) {
        case 1:
            pripiska = "_d1_11.xml";
            break;
        case 2:
            pripiska = "_d1_15.xml";
            break;
        case 3:
            pripiska = "_d1_18.xml";
            break;
        case 4:
            pripiska = "_d1_21.xml";
            break;
        case 5:
            pripiska = "_d2_8.xml";
            break;
        case 6:
            pripiska = "_d2_11.xml";
            break;
        case 7:
            pripiska = "_d2_15.xml";
            break;
        case 8:
            pripiska = "_d2_18.xml";
            break;
        case 9:
            pripiska = "_d2_21.xml";
            break;
        case 10:
            pripiska = "_d3_8.xml";
            break;
        case 11:
            pripiska = "_d3_11.xml";
            break;
        case 12:
            pripiska = "_d3_15.xml";
            break;
        case 13:
            pripiska = "_d3_18.xml";
            break;
        default:
            nextPage = "End.xml";
            break;
        }
        makeLogNoteQ(pripiska);
    }
    if (nextPage == "End.xml") {
        if(!razvetkaDone) {
            nextPage = "End_2.xml";
        }
    }
    if (nextPage == "End_2.xml_d3_18.xml") {
        if(!razvetkaDone) {
            nextPage = "End_2.xml";
        }
    }
    if (nextPage == "End.xml_d3_18.xml") {
        if(!razvetkaDone) {
            nextPage = "End.xml";
        }
    }
    //========================
    if (nextPage == "1_waiting_room/Waiting_room_d1_18_3_Quest.xml") {
        yazhmatDone = true;
    }
    if (nextPage == "1_waiting_room/Waiting_room_d2_8_3_Quest.xml") {
        yazhmatDone = true;
    }
    if (nextPage == "1_waiting_room/Waiting_room_d2_18.xml") {
        if (!yazhmatDone) {
            nextPage = "1_waiting_room/Waiting_room_d2_18_2.xml";
        }
    }
    if (nextPage == "2_director_office/Director_office_d2_15.xml") {
        if (yazhmatDone) {
            directorDone = true;
        }
        else {
            nextPage = "2_director_office/Director_office_d2_11.xml";
        }
    }
    if (nextPage == "2_director_office/Director_office_d2_18.xml") {
        if (!directorDone) {
            nextPage = "2_director_office/Director_office_d1_21.xml";
        }
    }
    if (nextPage == "2_director_office/Director_office_d2_21.xml") {
        if (yazhmatDone) {
            nextPage = "2_director_office/Director_office_d2_21_2.xml";
        }
    }
    if (nextPage == "4_lake/Lake_d1_11_2_2.xml") {
        sum -=500;
    }
    if (nextPage == "4_lake/Lake_d1_15.xml") {
        hasRing = true;
    }
    if (nextPage == "4_lake/Lake_d3_8.xml") {
        if (!duckAlive) {
            nextPage = "4_lake/Lake_d3_8_2.xml";
        }
    }
    if (nextPage == "4_lake/Lake_d3_11.xml") {
        if (!duckAlive) {
            nextPage = "4_lake/Lake_d3_11_2.xml";
        }
    }
    if (nextPage == "4_lake/Lake_d3_15.xml") {
        if (!hasAqua) {
            if (duckAlive) {
                nextPage = "4_lake/Lake_d3_15_2.xml";
            }
            else { nextPage = "4_lake/Lake_d3_15_3.xml"; }
        }
        else {
            hasDisk = true;
            step--;
        }
    }
    if (nextPage == "4_lake/Lake_d3_18.xml") {
        if (!duckAlive) {
            nextPage = "4_lake/Lake_d3_18_2.xml";
        }
    }
    if (nextPage == "5_guard_room/Guard_room_d2_18.xml") {
        if (hasKey) {
            nextPage = "5_guard_room/Guard_room_d2_18_2.xml";
        }
    }
    if (nextPage == "5_guard_room/Guard_room_d3_8.xml") {
        if (!hasMandat) {
            if (hasKey) {
                nextPage = "5_guard_room/Guard_room_d3_8_3.xml";
            }
            else { nextPage = "5_guard_room/Guard_room_d3_2.xml"; }
        }
        else {
            zagadkaDone = true;
        }
    }
    if (nextPage == "5_guard_room/Guard_room_d3_15.xml") {
        if (!hasDisk) {
            if (!hasMandat) {
                nextPage = "5_guard_room/Guard_room_15_2.xml";
            }
            else {
                nextPage = "5_guard_room/Guard_room_d3_15_3.xml";
                zagadkaDone = true;
            }
        }
        else {
            guardDone = true;
        }
    }
    if (nextPage == "5_guard_room/Guard_room_d3_18.xml") {
        if (!hasDisk) {
            if (!hasMandat) {
                nextPage = "5_guard_room/Guard_room_18_2.xml";
            }
            else {
                nextPage = "5_guard_room/Guard_room_d3_18_3.xml";
                zagadkaDone = true;
            }
        }
        else {
            guardDone = true;
        }
    }
    //прок ключа
    if (nextPage == "6_toilet/Toilet_d1_18_3.xml") {
        hasKey = true;
    }
    //прок ключа
    if (nextPage == "6_toilet/Toilet_d2_18_2.xml" && hasKey == true) {
        nextPage == "6_toilet/Toilet_d2_18.xml";
    }
    if (nextPage == "6_toilet/Toilet_d2_18_3.xml") {
        hasKey = true;
    }
    //прок ключа
    if (nextPage == "6_toilet/Toilet_d3_18_2.xml" && hasKey == true) {
        nextPage == "6_toilet/Toilet_d3_18.xml";
    }
    if (nextPage == "6_toilet/Toilet_d3_18_3.xml") {
        hasKey = true;
    }
    //прок победы
    if (nextPage == "9_storeroom/Storeroom_d1_11.xml") {
        //прок крыс
    }
    if (nextPage == "9_storeroom/Storeroom_d1_18.xml") {
        //прок крыс
    }
    if (nextPage == "9_storeroom/Storeroom_d2_11.xml") {
        //прок крыс
    }
    if (nextPage == "9_storeroom/Storeroom_d2_21.xml") {
        //прок крыс
    }
    if (nextPage == "9_storeroom/Storeroom_d3_15.xml") {
        //прок крыс
    }
    if (nextPage == "9_storeroom/Storeroom_d3_18.xml") {
        if (guardDone) {
            nextPage = "9_storeroom/Storeroom_4.xml";
            razvetkaDone = true;
        }
    }
    if (nextPage == "10_concert/Concert_d2_21.xml") {
        if (directorDone) {
            nextPage = "10_concert/Concert_d2_21_2_1.xml";
            hasMandat = true;
        }
    }
    if (nextPage == "10_concert/Concert_d2_21.xml") {
        if (directorDone) {
            nextPage = "10_concert/Concert_d2_21_2_1.xml";
            hasMandat = true;
        }
    }
    if (nextPage == "7_shop/Shop_d3_11.xml") {
        if (!zagadkaDone) {
            nextPage = "7_shop/Shop_d3_11_2.xml";
        }
    }
    if (nextPage == "7_shop/Shop_d3_11_1_2.xml") {
        hasAqua = true;
        sum -=500;
    }
    if (nextPage == "8_street/Street_d1_21_3.xml") {
        hasTestDNK = true;
        sum -=500;
    }
    sendPage(nextPage);
}

void MainWindow::initGame()
{
    sum = 1000;
    step = 0;
    playerName = "0";
    newMove = false;
    pripiska = "";
    hasMandat = false;
    hasKey = false;
    hasDisk = false;
    hasAqua = false;
    hasRing = false;
    hasTestDNK = false;
    yazhmatDone = false;
    directorDone = false;
    razvetkaDone = false;
    zagadkaDone = false;
    guardDone = false;
    duckAlive = true;
}

void MainWindow::sendBag() {
    QString s("Сумма: ");
    s.append(QString::number(sum,10));
    sendElement(s);
    if (hasCookie) {
        sendElement("Печеньки");
    }
    if (hasKey) {
        sendElement("Газовый ключ");
    }
    if (hasAqua) {
        sendElement("Водолазный комплект");
    }
    if (hasTestDNK) {
        sendElement("Экспресс тестДНК");
    }
    if (hasDisk) {
        sendElement("Диск доты");
    }
    if (hasMandat) {
        sendElement("Мандат от директора");
    }
    if (hasRing) {
        sendElement("Кольцо на интеллект");
    }
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
