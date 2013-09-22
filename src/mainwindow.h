#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QPushButton"
#include "QVector"
#include "QFile"
#include "QDir"
#include "QDebug"
#include "QVariant"
#include "phonon"
#include <QSystemTrayIcon>
#include "QWindowStateChangeEvent"
#include "QEvent"
#include "QThread"
#include "QBrush"
#include "about.h"
#include "QMessageBox"

namespace Ui {
class MainWindow;
}
Q_DECLARE_METATYPE (QPushButton*)

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


    
private:
    Ui::MainWindow *ui;

    struct station
    {
        QString name;
        QString URL;
        QString img;
        QPushButton *btn;
    };


    station currentStation;
    Phonon::MediaObject mediaObject;
    Phonon::AudioOutput audioOutput;
    QVector<station> stations;
    QSystemTrayIcon *trayicon;
    QAction * action; //Закрыть
    QAction * actionAbout;


    QThread * audioThread;


    void LoadStations();
    void AddButtons();
    void Play(station st);
    void Stop();
    void SetIcon(bool paly);

    void changeEvent( QEvent * e );

private  slots:
  void RadioChenget();
  void showHide(QSystemTrayIcon::ActivationReason);
  void showAbout();
  void mediaObjactStateChanget(Phonon::State oldst,
                               Phonon:: State newst);


  void on_playButton_clicked(bool checked);
};

#endif // MAINWINDOW_H
