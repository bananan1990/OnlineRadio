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
        int id;
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
    QAction * actionClose; //Закрыть
    QAction * actionAbout;



    void configButton(station * st);
    void LoadStations();
    void Play(station st);
    void Stop();
    void SetIcon(bool paly);
    void changeEvent( QEvent * e );
    void setBackground();
    void resizeWindow();


private  slots:
  void showHide(QSystemTrayIcon::ActivationReason);
  void RadioButtonClicked(bool isChecked);
  void mediaObjactStateChanget(Phonon::State oldst,
                               Phonon:: State newst);
  void on_playButton_clicked(bool checked);


  void showAbout();
  void on_CloseButton_clicked();
  void on_playButton_pressed();
  void on_playButton_released();
};

#endif // MAINWINDOW_H
