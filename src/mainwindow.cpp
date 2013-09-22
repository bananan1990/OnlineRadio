#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setAutoFillBackground(false);
    setWindowFlags(Qt::Window
       | Qt::WindowMinimizeButtonHint
       | Qt::WindowCloseButtonHint
       | Qt::CustomizeWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("background-image: url(:/img/back.png);");




    setWindowTitle("OnlineRadio.uz");

    ui->statusBar->showMessage("OnlineRadio.uz                  GPL-license");

    ui->playButton->setIconSize(QSize(90,90));
    ui->playButton->setMaximumSize(QSize(100,100));
    ui->playButton->setMinimumSize(QSize(100,100));
    ui->playingNow->setMaximumSize(QSize(100,100));
    ui->playingNow->setMinimumSize(QSize(100,100));
    ui->playButton->setIcon(QIcon(":/img/play.png"));

     Phonon::createPath(&mediaObject,&audioOutput);
    ui->volumeSlider->setAudioOutput(&audioOutput);

//    audioThread = new QThread(this);
//    mediaObject.moveToThread(audioThread);
    connect(&mediaObject,SIGNAL(stateChanged(Phonon::State,Phonon::State)),
            this,SLOT(mediaObjactStateChanget(Phonon::State,Phonon:: State)));



     trayicon = new QSystemTrayIcon(this);
     SetIcon(false);
     connect(trayicon,
             SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
             this,SLOT(showHide(QSystemTrayIcon::ActivationReason)));


     QMenu *Menu = new QMenu(this);
     action = new QAction(QString::fromLocal8Bit("Закрыть"),this);
     actionAbout = new QAction(QString::fromLocal8Bit("О программе"), this);
     Menu->addAction(action);
     Menu->addAction(actionAbout);
     trayicon->setContextMenu(Menu);

     connect(action,SIGNAL(triggered()),this,SLOT(close()));
     connect (actionAbout,SIGNAL(triggered()),this,SLOT(showAbout()));


     LoadStations();
     AddButtons();




}

void MainWindow::mediaObjactStateChanget(Phonon::State oldst, Phonon::State newst)
{
    if (newst == Phonon::ErrorState)
    {
        qDebug() << "Error 5";
        Stop();
        QMessageBox msg(this);
        msg.setText(QString::fromLocal8Bit("Сервер временно не доступен"));
        msg.exec();


    }
}
void MainWindow::showAbout()
{
    About * ab = new About(this);
    ab->show();
    qDebug() << ab->isVisible();
}
void MainWindow::changeEvent(QEvent * e)
{
    if ( e->type() == QEvent::WindowStateChange )
    {
        if ( windowState() & Qt::WindowMinimized)
        {
            hide();
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::LoadStations()
{
    QDir dir;
    dir.setPath("./radio");
    QStringList list = dir.entryList(QStringList("*.rs"));
    foreach (QString filename, list)
    {
        station st;
        st.name = filename.left(filename.indexOf(".rs"));
        QFile file(QString("%1/%2").arg(dir.path()).arg(filename));
        if ( file.open(QFile::ReadOnly) )
        {
            QTextStream t( &file );
            st.URL = t.readLine();
        }
        file.close();
        QFile imgfile(QString("%1/%2.png").arg(dir.path()).arg(st.name));
        if (imgfile.exists())
        {
            st.img = imgfile.fileName();
        }
        else
        {
            st.img = "";
        }

        stations.append(st);
    }

}


void MainWindow::AddButtons()
{
    ui->gridLayout->setColumnStretch(3,100);
    for(int i = 0; i < stations.count(); i++)
    {
        stations[i].btn = new QPushButton(this);
        stations[i].btn->setMinimumSize(QSize(100,100));
        stations[i].btn->setMaximumSize(QSize(100,100));

        if (stations[i].img != "")
        {
            stations[i].btn->setIcon(QIcon(stations[i].img));
        }
        else
        {
            stations[i].btn->setText(stations[i].name);
        }
        stations[i].btn->setIconSize(QSize(90,90));
        stations[i].btn->setCheckable(true);

        stations[i].btn->setProperty("BTN",
                    QVariant::fromValue(stations[i].btn));


        connect(stations[i].btn,SIGNAL(clicked()),this,SLOT(RadioChenget()));
        ui->gridLayout->addWidget(stations[i].btn);
    }
}
void MainWindow::RadioChenget()
{

    QPushButton * btn =
            sender()->property("BTN").value<QPushButton *>();
    if (btn->isChecked())
    {

        for(int i= 0; i < stations.count(); i++)
        {
            stations[i].btn->setChecked(false);
        }
        btn->setChecked(true);

        for(int i= 0; i < stations.count(); i++)
        {
            if (stations[i].btn->isChecked())
            {
//                qDebug() << QString::number(i);
                this->setWindowTitle(stations[i].name);
                Play(stations[i]);

                currentStation = stations[i];
            }

        }
    }
    else
    {
        qDebug () << "Stop";
        this->setWindowTitle("OnlineRadio");
        Stop();

    }

}

void MainWindow::Play(station st)
{
    mediaObject.setCurrentSource(Phonon::MediaSource(st.URL));
    mediaObject.play();
    ui->playButton->setIcon(QIcon(":/img/stop.png"));

    ui->playButton->setChecked(true);
    QPixmap pix(st.img);
    ui->playingNow->setPixmap(pix);

    SetIcon(true);
}
void MainWindow::Stop()
{
    mediaObject.stop();
    ui->playButton->setIcon(QIcon(":/img/play.png"));
    ui->playButton->setChecked(false);
    SetIcon(false);
}


void MainWindow::on_playButton_clicked(bool checked)
{
    if (checked)
    {
        if (currentStation.URL != "")
        {
            Play(currentStation);
        }
        else
        {
            ui->playButton->setChecked(false);
        }
    }
    else
    {
        Stop();

    }
}

void MainWindow::SetIcon(bool play )
{


    if (play)
        trayicon->setIcon(QIcon(":/img/play.png"));  //устанавливаем иконку
    else
        trayicon->setIcon(QIcon(":/img/stop.png"));  //устанавливаем иконку

    trayicon->show();  //отображаем объект
}

void MainWindow::showHide(QSystemTrayIcon::ActivationReason r) {
    if (r==QSystemTrayIcon::Trigger)
    {
        if (!this->isVisible())
        {
            this->showNormal();
        }
        else
        {
            this->hide();  //иначе скрываем
        }
    }
}
