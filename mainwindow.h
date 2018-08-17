#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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
    void getMessage(int x, int y);
    void on_addButton_clicked();
    void resize();

    void on_pushButton_clicked();

private:
    const int X_POSITION_FRAME = 0;
    const int Y_POSITION_FRAME = 0;

    Ui::MainWindow *ui;

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
    bool zagadkaDone;
    bool guardDone;
    bool razvetkaDone;
    bool duckAlive;

    void sendPage(QString pageAddress);
    void mainController(QString message);
    void initGame();
    QString localIP();
    void sendBag();
    int randInt(int low, int high);
    void makeLogNotePage(QString s);
    void makeLogNotePageVariants(QString s);
    void makeLogNoteBag(QString s);
    void loadImage(QString s);
};

#endif // MAINWINDOW_H
