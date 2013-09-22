#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Настройки вида основного окна
    this->setWindowFlags(Qt::Window
       | Qt::WindowMinimizeButtonHint
       | Qt::WindowCloseButtonHint
       | Qt::CustomizeWindowHint);

    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setStyleSheet("background-image: url(:/img/back.png);");
    this->setWindowTitle("OnlineRadio.uz");

    //Кнопки станций
    ui->gridLayout->setColumnStretch(3,100);

    //Статусбар
    ui->statusBar->showMessage(
                "OnlineRadio.uz                  GPL-license");

    //PlayButton
    ui->playButton->setIconSize(QSize(90,90));
    ui->playButton->setMaximumSize(QSize(100,100));
    ui->playButton->setMinimumSize(QSize(100,100));
    ui->playingNow->setMaximumSize(QSize(100,100));
    ui->playingNow->setMinimumSize(QSize(100,100));
    ui->playButton->setIcon(QIcon(":/img/play.png"));

    //Phonon
     Phonon::createPath(&mediaObject,&audioOutput);
    ui->volumeSlider->setAudioOutput(&audioOutput);
    connect(&mediaObject,SIGNAL(stateChanged(Phonon::State,Phonon::State)),
            this,SLOT(mediaObjactStateChanget(Phonon::State,Phonon:: State)));


    //trayicon
     trayicon = new QSystemTrayIcon(this);
     SetIcon(false);
     connect(trayicon,
             SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
             this,SLOT(showHide(QSystemTrayIcon::ActivationReason)));


     QMenu *Menu = new QMenu(this);
     actionClose = new QAction(QString::fromLocal8Bit("Закрыть"),this);
     actionAbout = new QAction(QString::fromLocal8Bit("О программе"), this);
     Menu->addAction(actionClose);
     Menu->addAction(actionAbout);
     trayicon->setContextMenu(Menu);

     connect(actionClose,SIGNAL(triggered()),this,SLOT(close()));
     connect (actionAbout,SIGNAL(triggered()),this,SLOT(showAbout()));



     LoadStations();


}

void MainWindow::mediaObjactStateChanget(Phonon::State newst, Phonon::State oldst)
{
    if (newst == Phonon::ErrorState)
    {
        qDebug() << "Error 5";
        Stop();
        QMessageBox::warning(this,QString::fromLocal8Bit("Ошибка"),
                             QString::fromLocal8Bit("Сервер временно не доступен!!"),
                             QMessageBox::Ok);
    }
}
void MainWindow::showAbout()
{
    QMessageBox::about(this,QString::fromLocal8Bit("О программе"),
                       QString::fromLocal8Bit(
                           "Добро пожаловать.\n\
П.О. Распространяется по лицензии GPL v.3 \n\
git: git@github.com:bananan1990/OnlineRadio.git"));


}
void MainWindow::changeEvent(QEvent * e)
{
    if ( e->type() == QEvent::WindowStateChange )
        if ( windowState() & Qt::WindowMinimized)
        hide();
}



void MainWindow::LoadStations()
{
    QDir dir;
    dir.setPath("./radio");
    QStringList list = dir.entryList(QStringList("*.rs"));
    int i = 0;
    foreach (QString filename, list)
    {
        station st;

        st.id = i++;

        st.name = filename.left(filename.indexOf(".rs"));

        QFile file(QString("%1/%2").arg(dir.path()).arg(filename));

        file.open(QFile::ReadOnly);
        QTextStream t( &file );
        st.URL = t.readLine();
        file.close();

        QFile imgfile(QString("%1/%2.png").arg(dir.path()).arg(st.name));
        if (imgfile.exists())
            st.img = imgfile.fileName();
        else
            st.img = "";


        configButton(&st);

        stations.append(st);
    }

}

void MainWindow::configButton(station * st)
{
    st->btn = new QPushButton(this);

    if (st->img != "")
        st->btn->setIcon(QIcon(st->img));
    else
        st->btn->setText(st->name);

    st->btn->setMinimumSize(QSize(100,100));
    st->btn->setMaximumSize(QSize(100,100));
    st->btn->setIconSize(QSize(90,90));
    st->btn->setCheckable(true);
    st->btn->setProperty("ID",st->id);
    connect(st->btn,SIGNAL(clicked(bool)),this,SLOT(RadioButtonClicked(bool)));
    ui->gridLayout->addWidget(st->btn);
}


void MainWindow::RadioButtonClicked(bool isChecked)
{
    int tmpid = sender()->property("ID").value<int>();
    if (isChecked)
    {
        //Убираем флаг checked c остьльных кнопок
        for(int i= 0; i < stations.count(); i++)
        {
            if (i != tmpid)
                stations[i].btn->setChecked(false);
        }
        Play(stations[tmpid]);
    }
    else
        Stop();
}

void MainWindow::Play(station st)
{
    setWindowTitle(st.name);
    currentStation = st;
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
    setWindowTitle("OnlineRadio");
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
            QMessageBox::warning(this,
                                 QString::fromLocal8Bit("Ошибка"),
                                 QString::fromLocal8Bit("Выберите станцию."),
                                 QMessageBox::Ok);
        }
    }
    else
    {
        Stop();
    }
}

void MainWindow::SetIcon(bool play)
{


    if (play)
        trayicon->setIcon(QIcon(":/img/play.png"));  //устанавливаем иконку
    else
        trayicon->setIcon(QIcon(":/img/stop.png"));  //устанавливаем иконку

    trayicon->show();  //отображаем объект
}

void MainWindow::showHide(QSystemTrayIcon::ActivationReason r)
{
    if (r==QSystemTrayIcon::Trigger)
    {
        if (!this->isVisible())
            this->showNormal();
        else
            this->hide();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
