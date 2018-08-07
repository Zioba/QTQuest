#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QTimer>
#include <QDateEdit>
#include <QtXml>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void getMessage();
    void on_addButton_clicked();
    void on_saveButton_clicked();
    void resize();

private:
    const int X_POSITION_FRAME = 0;
    const int Y_POSITION_FRAME = 0;
    const int LISTERNING_PORT = 5824;
    const QString myPort = "5824";
    const QString targetPort = "5825";

    Ui::MainWindow *ui;
    QUdpSocket udpSocket;
    QHostAddress myIp;
    QHostAddress targetIp;

    QList<QString> possibleLinks;
    int sum;
    int step;
    QString playerName;
    bool newMove;
    QString pripiska;

    bool hasCookie;
    bool hasKey;
    bool hasAqua;
    bool hasTestDNK;
    bool hasDisk;
    bool hasMandat;
    bool hasRing;
    bool yazhmatDone;
    bool directorDone;
    bool duckAlive;


    void makeLogNote( QString );
    void sendPage(QString pageAddress);
    void sendElement(QString element);
    void mainController(QString message);
    void initGame();
    QString localIP();
};

#endif // MAINWINDOW_H
