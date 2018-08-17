#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QPoint pos( X_POSITION_FRAME, Y_POSITION_FRAME );
    this->move( pos );
    ui->setupUi( this );
    setWindowTitle("Квест Сервер");
    ui->stackedWidget->setCurrentIndex(1);
    connect(ui->variantsTextEdit, SIGNAL(cellChanged(int,int)), this, SLOT(resize()));
    connect(ui->variantsTextEdit, SIGNAL(cellClicked(int ,int)), this, SLOT(getMessage(int, int)));

    ui->variantsTextEdit->setColumnCount(1);
    ui->variantsTextEdit->setRowCount(0);
    ui->variantsTextEdit->setShowGrid(true);
    ui->variantsTextEdit->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->variantsTextEdit->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->variantsTextEdit->verticalHeader()->hide();
    ui->variantsTextEdit->horizontalHeader()->hide();
    ui->variantsTextEdit->horizontalHeader()->setStretchLastSection(true);
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());

    initGame();
    mainController("-1");
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

int MainWindow::randInt(int low, int high)
{
    // Random number between low and high
    return qrand() % ((high + 1) - low) + low;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::makeLogNotePage(QString s)
{
    ui->mainTextField->setText(s);
}

void MainWindow::makeLogNotePageVariants(QString s)
{
    ui->variantsTextEdit->insertRow(ui->variantsTextEdit->rowCount());
    QTableWidgetItem *item = new QTableWidgetItem;
    item->setText(s);
    ui->variantsTextEdit->setItem(ui->variantsTextEdit->rowCount()-1, 0, item);
}

void MainWindow::makeLogNoteBag(QString s)
{
    ui->bagField->append(s);
}

void MainWindow::loadImage(QString s)
{
    //QString str = "C:/QTWork/QTQuest/resources/images/";
    QString str = ":/resources/images/";
    str.append(s);
    QPixmap myPixmap( str );
    myPixmap=myPixmap.scaled(500,500);
    ui->pictureLabel->setPixmap( myPixmap );
    this->update();

}

void MainWindow::sendPage(QString pageAddress)
{
    ui->variantsTextEdit->setRowCount(0);
    //QString str = ":/resources/";
    QString str = "C:/QTWork/QTQuest/resources/";
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
                        if(domElement.tagName() == "Item") {
                            if (domElement.text() == "Сходить на концерт.") {
                                if ((step%5 == 3) || (step%5 == 4)) {
                                    makeLogNotePageVariants(domElement.text());
                                }
                            }
                            else {
                                makeLogNotePageVariants(domElement.text());
                            }
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
                        if(domElement.tagName() == "MainText") {
                           makeLogNotePage(domElement.text());
                        }
                        if(domElement.tagName() == "PagePicture") {
                           loadImage(domElement.text());
                        }
                    }
                }
                domNode = domNode.nextSibling();
            }
        }
        file.close();
    }
    else {
        makeLogNotePage("error");
    }
    if (newMove) { newMove = false; }
    ui->bagField->clear();
    sendBag();
}

void MainWindow::getMessage(int x, int y)
{
    QString str(QString::number(x,10));
    mainController(str);
}

void MainWindow::mainController(QString message)
{
    QString nextPage;
    if (message == "-1") {
        initGame();
        nextPage = "Intro_1.xml";
    }
    else {
        nextPage = possibleLinks.at(message.toInt(Q_NULLPTR, 10));
    }
    if (nextPage == "Intro_1.xml") {
        initGame();
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
    if (nextPage == "1_waiting_room/Waiting_room_d1_18_3_Quest_5.xml") {
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
    if (nextPage == "2_director_office/Director_office_d2_15_2.xml") {
        if (hasKey) {
          nextPage = "2_director_office/Director_office_d2_15_2_2.xml";
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
    if (nextPage == "4_lake/Lake_d1_11_2_1.xml") {
        int x = randInt(1,10);
        switch (x) {
        case 1:
            nextPage = "4_lake/Lake_d1_11_2_1.xml";
            break;
        case 2:
            nextPage = "4_lake/Lake_d1_11_2_1_2.xml";
            break;
        case 3:
            nextPage = "4_lake/Lake_d1_11_2_1_3.xml";
            break;
        default:
            break;
        }
    }
    if (nextPage == "4_lake/Lake_d1_11_2_2.xml") {
        sum -=500;
    }
    if (nextPage == "4_lake/Lake_d1_15_3.xml") {
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
    }
    if (nextPage == "4_lake/Lake_d3_15_1_success.xml") {
        hasDisk = true;
        step--;
    }
    if (nextPage == "4_lake/Lake_d3_15_1_2.xml") {
        if (hasRing) {
            nextPage = "4_lake/Lake_d3_15_1_2_r.xml";
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
            else { nextPage = "5_guard_room/Guard_room_d3_8_2.xml"; }
        }
        else {
            zagadkaDone = true;
        }
    }
    if (nextPage == "5_guard_room/Guard_room_d3_15.xml") {
        if (!hasDisk) {
            if (!hasMandat) {
                nextPage = "5_guard_room/Guard_room_d3_15_2.xml";
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
    if (nextPage == "5_guard_room/Guard_room_d3_18.xml") {
        if (hasRing) {
            nextPage = "5_guard_room/Guard_room_d3_18_0.xml";
        }
    }
    if (nextPage == "6_toilet/Toilet_d1_18_3.xml") {
        hasKey = true;
    }
    if (nextPage == "6_toilet/Toilet_d1_18.xml") {
        int x = randInt(1,2);
        if( x == 1) {
            nextPage = "6_toilet/Toilet_d1_18_2.xml";
        }
    }
    if (nextPage == "6_toilet/Toilet_d2_18.xml") {
        int x = randInt(1,2);
        if( x == 1 && !hasKey) {
            nextPage = "6_toilet/Toilet_d1_18_2.xml";
        }
    }
    if (nextPage == "6_toilet/Toilet_d3_18.xml") {
        int x = randInt(1,2);
        if( x == 1 && !hasKey) {
            nextPage = "6_toilet/Toilet_d1_18_2.xml";
        }
    }
    if (nextPage == "9_storeroom/Storeroom_d1_11.xml") {
        int x = randInt(0,100);
        razvetkaDone = true;
        if( x < 5) {
            nextPage = "9_storeroom/Storeroom_1.xml";
        }
    }
    if (nextPage == "9_storeroom/Storeroom_d1_18.xml") {
        int x = randInt(1,2);
        if( x == 1) {
            nextPage = "9_storeroom/Storeroom_3.xml";
        }
    }
    if (nextPage == "9_storeroom/Storeroom_d2_11.xml") {
        int x = randInt(1,2);
        if( x == 1) {
            nextPage = "9_storeroom/Storeroom_3.xml";
        }
    }
    if (nextPage == "9_storeroom/Storeroom_d2_21.xml") {
        int x = randInt(1,2);
        if( x == 1) {
            nextPage = "9_storeroom/Storeroom_3.xml";
        }
    }
    if (nextPage == "9_storeroom/Storeroom_d3_15.xml") {
        int x = randInt(1,2);
        if( x == 1) {
            nextPage = "9_storeroom/Storeroom_3.xml";
        }
    }
    if (nextPage == "9_storeroom/Storeroom_d3_18.xml") {
        if (guardDone) {
            nextPage = "9_storeroom/Storeroom_4.xml";
        }
    }
    if (nextPage == "9_storeroom/Storeroom_4.xml") {
        if (hasTestDNK) {
            nextPage = "9_storeroom/Storeroom_4_2.xml";
        }
    }
    if (nextPage == "9_storeroom/Storeroom_4_4.xml") {
        razvetkaDone = true;
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
        if (sum == 0) {
            nextPage = "7_shop/Shop_d3_11_1_2_f.xml";
        }
        else {
            sum -=500;
            hasAqua = true;
        }
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
    step = 0; //0
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
    switch (step) {
    case 1:
        makeLogNoteBag("День: 1, время: 11:00");
        break;
    case 2:
        makeLogNoteBag("День: 1, время: 15:00");
        break;
    case 3:
        makeLogNoteBag("День: 1, время: 18:00");
        break;
    case 4:
        makeLogNoteBag("День: 1, время: 21:00");
        break;
    case 5:
        makeLogNoteBag("День: 2, время: 8:00");
        break;
    case 6:
        makeLogNoteBag("День: 2, время: 11:00");
        break;
    case 7:
        makeLogNoteBag("День: 2, время: 15:00");
        break;
    case 8:
        makeLogNoteBag("День: 2, время: 18:00");
        break;
    case 9:
        makeLogNoteBag("День: 2, время: 21:00");
        break;
    case 10:
        makeLogNoteBag("День: 3, время: 8:00");
        break;
    case 11:
        makeLogNoteBag("День: 3, время: 11:00");
        break;
    case 12:
        makeLogNoteBag("День: 3, время: 15:00");
        break;
    case 13:
        makeLogNoteBag("День: 3, время: 18:00");
        break;
    default:
        break;
    }
    QString s("Сумма: ");
    s.append(QString::number(sum,10));
    makeLogNoteBag(s);
    if (hasCookie) {
         makeLogNoteBag("Печеньки");
    }
    if (hasKey) {
         makeLogNoteBag("Газовый ключ");
    }
    if (hasAqua) {
         makeLogNoteBag("Водолазный комплект");
    }
    if (hasTestDNK) {
         makeLogNoteBag("Экспресс тестДНК");
    }
    if (hasDisk) {
         makeLogNoteBag("Диск доты");
    }
    if (hasMandat) {
         makeLogNoteBag("Мандат от директора");
    }
    if (hasRing) {
         makeLogNoteBag("Кольцо на интеллект");
    }
}

void MainWindow::on_addButton_clicked()
{
    ui->variantsTextEdit->setRowCount(ui->variantsTextEdit->rowCount()+1);
}

void MainWindow::resize()
{
    ui->variantsTextEdit->resizeColumnsToContents();
}

void MainWindow::on_pushButton_clicked()
{
    initGame();
    mainController("-1");
}
