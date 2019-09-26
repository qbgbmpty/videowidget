#include "videoplayer.h"
#include "ui_videoplayer.h"
#include <QtWidgets>
#include <qvideowidget.h>
#include <qvideosurfaceformat.h>
#include "QFile"
#include "QTextStream"
#include "QString"
#include "QComboBox"
#include "QMessageBox"
#include "QProcess"

#include <QtCharts/QChartView>
#include <QtCharts/QAbstractBarSeries>
#include <QtCharts/QPercentBarSeries>
#include <QtCharts/QStackedBarSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QLegend>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtCharts/QBarCategoryAxis>
#include <QtWidgets/QApplication>
#include <QtCharts/QValueAxis>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <iostream>

QStringList fileName;
static QProcess *_process;
static QProcess *_process2;
QStringList _processParticleFileList;
QStringList _processCellFileList;
QString root;
QString saveParticleImgPath;
QString saveParticleBorderPath;
QString saveParticleFullPath;
QString saveParticleCenterPath;
QString saveParticleAreaPath;
QString saveCellImgPath;
QString saveCellBorderPath;
QString saveCellFullPath;
QString saveCellCenterPath;
QString saveCellAreaPath;
QString Particle_1;
QString Particle_2;
int count, processParticle, processCell, obj_num;
QString FirstParticle;

VideoPlayer::VideoPlayer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::VideoPlayer)
    , mediaPlayer(0, QMediaPlayer::VideoSurface)
    , mediaPlayer_2(0, QMediaPlayer::VideoSurface)
    , mediaPlayer_3(0, QMediaPlayer::VideoSurface)
    , mediaPlayer_4(0, QMediaPlayer::VideoSurface)
    , mediaPlayer_5(0, QMediaPlayer::VideoSurface)
{
    ui->setupUi(this);

    _process = new QProcess;
    _process2 = new QProcess;
    connect(_process, SIGNAL(finished(int, QProcess::ExitStatus)),this, SLOT(processFinishedSlot(int, QProcess::ExitStatus)));
    connect(_process2, SIGNAL(finished(int, QProcess::ExitStatus)),this, SLOT(process2FinishedSlot(int, QProcess::ExitStatus)));

    ui->openButton->setText("Select Partical");
    ui->openButton_2->setText("Select Cell");
    ui->runButton->setText("Run");
    connect(ui->openButton, SIGNAL(clicked()), this, SLOT(openFile()));
    connect(ui->openButton_2, SIGNAL(clicked()), this, SLOT(openFile_2()));
    connect(ui->runButton, SIGNAL(clicked()), this, SLOT(Run()));


    ui->playButton->setEnabled(false);
    ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));

    connect(ui->playButton, SIGNAL(clicked()), this, SLOT(play()));

    ui->playButton_2->setEnabled(false);
    ui->playButton_2->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));

    connect(ui->playButton_2, SIGNAL(clicked()), this, SLOT(play_particle()));

    ui->playButton_3->setEnabled(false);
    ui->playButton_3->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));

    connect(ui->playButton_3, SIGNAL(clicked()), this, SLOT(play_particle_2()));


    ui->positionSlider->setRange(0, 0);
    connect(ui->positionSlider, SIGNAL(sliderMoved(int)), this, SLOT(setPosition(int)));

    ui->positionSlider_2->setRange(0, 0);
    connect(ui->positionSlider_2, SIGNAL(sliderMoved(int)), this, SLOT(setPosition_particle(int)));

    ui->positionSlider_3->setRange(0, 0);
    connect(ui->positionSlider_3, SIGNAL(sliderMoved(int)), this, SLOT(setPosition_particle_2(int)));

    mediaPlayer.setVideoOutput(ui->widget_1);
    mediaPlayer_2.setVideoOutput(ui->widget_2);
    mediaPlayer_3.setVideoOutput(ui->widget_3);
    mediaPlayer_4.setVideoOutput(ui->widget_4);
    mediaPlayer_5.setVideoOutput(ui->widget_5);


    connect(ui->comboBox, SIGNAL(activated(const QString &)), this,SLOT(particle()));
    connect(ui->comboBox_2, SIGNAL(activated(const QString &)), this,SLOT(particle_time()));

    connect(&mediaPlayer, SIGNAL(stateChanged(QMediaPlayer::State)),this, SLOT(mediaStateChanged(QMediaPlayer::State)));
    connect(&mediaPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));
    connect(&mediaPlayer, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64)));

    connect(&mediaPlayer_4, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(mediaStateChanged_particle(QMediaPlayer::State)));
    connect(&mediaPlayer_4, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged_particle(qint64)));
    connect(&mediaPlayer_4, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged_particle(qint64)));

    connect(&mediaPlayer_5, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(mediaStateChanged_particle_2(QMediaPlayer::State)));
    connect(&mediaPlayer_5, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged_particle_2(qint64)));
    connect(&mediaPlayer_5, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged_particle_2(qint64)));


    connect(&mediaPlayer, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(handleError()));

    /*int window_width = width();
    int window_height = height();
    int image_width = m_pixmap.width();
    int image_height = m_pixmap.height();
    int r1 = window_width / image_width;
    int r2 = window_height / image_height;
    int r = qMin(r1, r2);
    int offset_x = (window_width - r * image_width) / 2;
    int offset_y = (window_height - r * image_height) / 2;
    painter.translate(offset_x, offset_y);
    painter.scale(r, r);
    painter.drawPixmap(0, 0, m_pixmap);
*/



}

VideoPlayer::~VideoPlayer()
{
}

void VideoPlayer::openFile()
{
    //fileName = QFileDialog::getOpenFileName(this, tr("Open Movie"),QDir::homePath());
    _processParticleFileList.clear();
    _processParticleFileList = QFileDialog::getOpenFileNames();
    processParticle=_processParticleFileList.size();



}

void VideoPlayer::openFile_2()
{
    //fileName = QFileDialog::getOpenFileName(this, tr("Open Movie"),QDir::homePath());
    _processCellFileList.clear();
    _processCellFileList = QFileDialog::getOpenFileNames();
    processCell=_processCellFileList.size();
}

void VideoPlayer::Run()
{    
    root=QDir::currentPath();
    root+="/output/";

    /*QString fileName_2 =fileName.mid(0,fileName.lastIndexOf("."));
    fileName_2+="_1.avi";
    QString fileName_3 =fileName.mid(0,fileName.lastIndexOf("."));
    fileName_3+="_2.avi";
    if (!fileName.isEmpty()) {
        mediaPlayer.setMedia(QUrl::fromLocalFile(fileName));
        mediaPlayer_2.setMedia(QUrl::fromLocalFile(fileName_2));
        mediaPlayer_3.setMedia(QUrl::fromLocalFile(fileName_3));
        ui->playButton->setEnabled(true);
    }*/

    ui->comboBox_2->clear();
    ui->comboBox->clear();
    obj_num=0;
    count=0;
    /*QString combo=fileName.mid(0,fileName.lastIndexOf("."));
    QString Line;

    //宣告檔案
    QFile file(combo);
    //Open
    file.open(QIODevice::ReadOnly);
    QTextStream streamread(&file);
    //讀
    //一次讀一行的寫法
    while(!streamread.atEnd())
    {
        Line = streamread.readLine();
        ui->comboBox->addItem(Line);
    }
    file.close();*/



    if (!_processParticleFileList.isEmpty() && !_processCellFileList.isEmpty() )
    {
        if (_processParticleFileList.size()!=_processCellFileList.size())
        {
            QMessageBox::warning(this, "Error","Particle&Cell unable to pair");
        }

        else
        {
            QDateTime current_dat_time = QDateTime::currentDateTime();
            QString current_date = current_dat_time.toString("yyyy_MM_dd_");
            int folder_num=1;

            QString rootFolder=root;
            rootFolder+=current_date;
            rootFolder+=QString::number(folder_num,10);


            QString Imgfolder=rootFolder;
            Imgfolder+="/particle_image";
            QString Borderfolder=rootFolder;
            Borderfolder+="/particle_border";
            QString Fullfolder=rootFolder;
            Fullfolder+="/particle_full";
            QString Centerfolder=rootFolder;
            Centerfolder+="/particle_center";
            QString ParticleArea=rootFolder;
            ParticleArea+="/particle_area";

            QDir *temp = new QDir;
            while(1)
            {
                if (temp->exists(rootFolder))
                {
                    folder_num++;
                    rootFolder.clear();
                    rootFolder=root;
                    rootFolder+=current_date;
                    rootFolder+=QString::number(folder_num,10);

                    Imgfolder=rootFolder;
                    Imgfolder+="/particle_image";
                    Borderfolder=rootFolder;
                    Borderfolder+="/particle_border";
                    Fullfolder=rootFolder;
                    Fullfolder+="/particle_full";
                    Centerfolder=rootFolder;
                    Centerfolder+="/particle_center";
                    ParticleArea=rootFolder;
                    ParticleArea+="/particle_area";
                }
                else
                {
                    temp->mkdir(rootFolder);
                    temp->mkdir(Imgfolder);
                    temp->mkdir(Borderfolder);
                    temp->mkdir(Fullfolder);
                    temp->mkdir(Centerfolder);
                    temp->mkdir(ParticleArea);
                    break;
                }
            }

            root=rootFolder;
            saveParticleImgPath=Imgfolder;
            saveParticleBorderPath=Borderfolder;
            saveParticleFullPath=Fullfolder;
            saveParticleCenterPath=Centerfolder;
            saveParticleAreaPath=ParticleArea;

            saveCellImgPath=root;
            saveCellImgPath+="/cell_image";
            saveCellBorderPath=root;
            saveCellBorderPath+="/cell_border";
            saveCellFullPath=root;
            saveCellFullPath+="/cell_full";
            saveCellCenterPath=root;
            saveCellCenterPath+="/cell_center";
            saveCellAreaPath=root;
            saveCellAreaPath+="/cell_area";

            temp->mkdir(saveCellImgPath);
            temp->mkdir(saveCellBorderPath);
            temp->mkdir(saveCellFullPath);
            temp->mkdir(saveCellCenterPath);
            temp->mkdir(saveCellAreaPath);
            delete temp;

            QString command="python3 prediction.py ";
            command+=_processParticleFileList.at(0);
            command=command.replace("`","\\`");
            command+=" ";
            command+=saveParticleImgPath;
            command+=" ";
            command+=saveParticleBorderPath;
            command+=" ";
            command+=saveParticleFullPath;
            command+=" ";
            command+=saveParticleCenterPath;
            command+=" ";
            command+=saveParticleAreaPath;
            //QString tmp_file=_processParticleFileList.at(0);
            //QString file_name=tmp_file.replace("\"", "");

            QString PFileList=root;
            PFileList+="/Plist.txt";
            QFile Pfile(PFileList);

            QString CFileList=root;
            CFileList+="/Clist.txt";
            QFile Cfile(CFileList);
            if (!Pfile.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QMessageBox::warning(this,"sdf","can't open",QMessageBox::Yes);
            }

            if (!Cfile.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QMessageBox::warning(this,"sdf","can't open",QMessageBox::Yes);
            }

            QTextStream Pout(&Pfile);
            QTextStream Cout(&Cfile);
            for (int i=0; i<_processParticleFileList.size(); i++)
            {
                Pout << _processParticleFileList.at(i).mid(_processParticleFileList.at(i).lastIndexOf("/"),_processParticleFileList.at(i).indexOf(".")-_processParticleFileList.at(i).lastIndexOf("/"))<<endl ;
                Cout << _processCellFileList.at(i).mid(_processCellFileList.at(i).lastIndexOf("/"),_processCellFileList.at(i).indexOf(".")-_processCellFileList.at(i).lastIndexOf("/"))<<endl ;
            }

            _process->start("/bin/sh", QStringList()<<"-c"<<command);

           /* command="python3 prediction.py ";
            command+=_processCellFileList.at(0);
            command=command.replace("`","\\`");
            command+=" ";
            command+=saveCellImgPath;
            command+=" ";
            command+=saveCellBorderPath;
            command+=" ";
            command+=saveCellFullPath;
            command+=" ";
            command+=saveCellCenterPath;
            //QString tmp_file=_processParticleFileList.at(0);
            //QString file_name=tmp_file.replace("\"", "");
            _process2->start("/bin/sh", QStringList()<<"-c"<<command);*/

            ui->openButton->setEnabled(false);
            ui->openButton_2->setEnabled(false);
        }

    }
    else
    {
        QMessageBox::warning(this, "Error","Please Select Particle/Cell.");
    }
}

void VideoPlayer::play()
{
    switch(mediaPlayer.state()) {
    case QMediaPlayer::PlayingState:
        mediaPlayer.pause();
        mediaPlayer_2.pause();
        mediaPlayer_3.pause();
        break;
    default:
        mediaPlayer.play();
        mediaPlayer_2.play();
        mediaPlayer_3.play();
        break;
    }
}

void VideoPlayer::particle()
{
    /*
    if (!ui->comboBox->currentText().isEmpty())
    {
        QString particleName = saveParticleImgPath;
        particleName+="/";
        particleName+=ui->comboBox->currentText();;

        QImage *image=new QImage(particleName);
        QPixmap *pixmap=new QPixmap();
        int with = ui->label->width();
        int height = ui->label->height();
        pixmap->convertFromImage(*image);
        QPixmap fitpixmap = pixmap->scaled(with, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->label->setPixmap(fitpixmap);
        ui->label->setAlignment(Qt::AlignCenter);

    }*/
    /*if (!ui->comboBox->currentText().isEmpty()) {
        QString particleName = ui->comboBox->currentText();
        particleName+=".avi";
        QString particlePath=fileName.mid(0,fileName.lastIndexOf("/")+1);
        particlePath+=particleName;
        mediaPlayer_4.setMedia(QUrl::fromLocalFile(particlePath));
        ui->playButton_2->setEnabled(true);

        QString particleName_2 = ui->comboBox->currentText();
        particleName_2+=".avi";
        QString particlePath_2=fileName.mid(0,fileName.lastIndexOf("/")+1);
        particlePath_2+="recordVideo/";
        particlePath_2+=particleName_2;
        mediaPlayer_5.setMedia(QUrl::fromLocalFile(particlePath_2));
        ui->playButton_3->setEnabled(true);
*/
        QString cahrtPath=root;
        cahrtPath+="/recordParticleandCellDistance/ParticleandCellProcessDistance.txt";
        createLineChart(cahrtPath,ui->comboBox->currentIndex()+1);

/*
        QString imagePath=fileName.mid(0,fileName.lastIndexOf("/")+1);
        imagePath+="object";
        if( ui->comboBox->currentIndex()+1<10)
            imagePath+="0";
        imagePath+=QString::number(ui->comboBox->currentIndex()+1);
        imagePath+=".tiff";
        QImage *image=new QImage(imagePath);
        QPixmap *pixmap=new QPixmap();
        int with = ui->label->width();
        int height = ui->label->height();
        pixmap->convertFromImage(*image);
        QPixmap fitpixmap = pixmap->scaled(with, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        //ui->label->setScaledContents(true);
        //ui->label->setGeometry(100,0,1000,700);
        ui->label->setPixmap(fitpixmap);
        ui->label->setAlignment(Qt::AlignCenter);
    }*/
}

void VideoPlayer::particle_time()
{
    if (!ui->comboBox_2->currentText().isEmpty())
    {
        QString particleName = saveParticleImgPath;
        particleName+="/";
        particleName+=ui->comboBox_2->currentText();

        QImage *image=new QImage(particleName);
        QPixmap *pixmap=new QPixmap();
        int with = ui->label->width();
        int height = ui->label->height();
        pixmap->convertFromImage(*image);
        QPixmap fitpixmap = pixmap->scaled(with, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->label->setPixmap(fitpixmap);
        ui->label->setAlignment(Qt::AlignCenter);

        QString CellName = saveCellImgPath;
        CellName+="/";
        CellName+=_processCellFileList.at(ui->comboBox_2->currentIndex()).mid(_processCellFileList.at(ui->comboBox_2->currentIndex()).lastIndexOf("/")+1);
        CellName=CellName.mid(0,CellName.lastIndexOf('.'));
        CellName+=".png";
        qDebug()<<CellName;

        QImage *Cellimage=new QImage(CellName);
        QPixmap *Cellpixmap=new QPixmap();
        int Cellwidth = ui->label_3->width();
        int Cellheight = ui->label_3->height();
        Cellpixmap->convertFromImage(*Cellimage);
        QPixmap Cellfitpixmap = Cellpixmap->scaled(Cellwidth, Cellheight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->label_3->setPixmap(Cellfitpixmap);
        ui->label_3->setAlignment(Qt::AlignCenter);


    }
    /*if (!ui->comboBox_2->currentText().isEmpty()) {
        QString imagePath=fileName.mid(0,fileName.lastIndexOf("/")+1);
        imagePath+="circled/";
        imagePath+="object";
        imagePath+=QString::number(ui->comboBox->currentIndex()+1);
        imagePath+="/";
        imagePath+="image";
        if( ui->comboBox_2->currentIndex()+1<10)
            imagePath+="0";
        imagePath+=QString::number(ui->comboBox_2->currentIndex()+1);
        imagePath+=".tif";
        qDebug()<<imagePath;
        QImage *image=new QImage(imagePath);
        QPixmap *pixmap=new QPixmap();
        int width = ui->label_2->width();
        int height = ui->label_2->height();
        pixmap->convertFromImage(*image);
        QPixmap fitpixmap = pixmap->scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        //ui->label->setScaledContents(true);
        //ui->label->setGeometry(100,0,1000,700);
        ui->label_2->setPixmap(fitpixmap);
        ui->label_2->setAlignment(Qt::AlignCenter);

        QString imagePath_2=fileName.mid(0,fileName.lastIndexOf("/")+1);
        imagePath_2+="c2/";
        imagePath_2+="object";
        imagePath_2+=QString::number(ui->comboBox->currentIndex()+1);
        imagePath_2+="/";
        imagePath_2+="image";
        if( ui->comboBox_2->currentIndex()+1<10)
            imagePath_2+="0";
        imagePath_2+=QString::number(ui->comboBox_2->currentIndex()+1);
        imagePath_2+=".tif";
        qDebug()<<imagePath_2;
        QImage *image_2=new QImage(imagePath_2);
        QPixmap *pixmap_2=new QPixmap();
        int width_2 = ui->label_3->width();
        int height_2 = ui->label_3->height();
        pixmap_2->convertFromImage(*image_2);
        QPixmap fitpixmap_2 = pixmap_2->scaled(width_2, height_2, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        //ui->label->setScaledContents(true);
        //ui->label->setGeometry(100,0,1000,700);
        ui->label_3->setPixmap(fitpixmap_2);
        ui->label_3->setAlignment(Qt::AlignCenter);
    }*/
}

void VideoPlayer::play_particle()
{
    switch(mediaPlayer_4.state()) {
    case QMediaPlayer::PlayingState:
        mediaPlayer_4.pause();
        break;
    default:
        mediaPlayer_4.play();
        break;
    }
}

void VideoPlayer::play_particle_2()
{
    switch(mediaPlayer_5.state()) {
    case QMediaPlayer::PlayingState:
        mediaPlayer_5.pause();
        break;
    default:
        mediaPlayer_5.play();
        break;
    }
}

void VideoPlayer::mediaStateChanged(QMediaPlayer::State state)
{
    switch(state) {
    case QMediaPlayer::PlayingState:
        ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        break;
    default:
        ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        break;
    }
}

void VideoPlayer::mediaStateChanged_particle(QMediaPlayer::State state)
{
    switch(state) {
    case QMediaPlayer::PlayingState:
        ui->playButton_2->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        break;
    default:
        ui->playButton_2->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        break;
    }
}

void VideoPlayer::mediaStateChanged_particle_2(QMediaPlayer::State state)
{
    switch(state) {
    case QMediaPlayer::PlayingState:
        ui->playButton_3->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        break;
    default:
        ui->playButton_3->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        break;
    }
}

void VideoPlayer::positionChanged(qint64 position)
{
    ui->positionSlider->setValue(position);
}

void VideoPlayer::positionChanged_particle(qint64 position)
{
    ui->positionSlider_2->setValue(position);
}

void VideoPlayer::positionChanged_particle_2(qint64 position)
{
    ui->positionSlider_3->setValue(position);
}

void VideoPlayer::durationChanged(qint64 duration)
{
    ui->positionSlider->setRange(0, duration);
}

void VideoPlayer::durationChanged_particle(qint64 duration)
{
    ui->positionSlider_2->setRange(0, duration);
}

void VideoPlayer::durationChanged_particle_2(qint64 duration)
{
    ui->positionSlider_3->setRange(0, duration);
}

void VideoPlayer::setPosition(int position)
{
    mediaPlayer.setPosition(position);
    mediaPlayer_2.setPosition(position);
    mediaPlayer_3.setPosition(position);
}

void VideoPlayer::setPosition_particle(int position)
{
    mediaPlayer_4.setPosition(position);
}

void VideoPlayer::setPosition_particle_2(int position)
{
    mediaPlayer_5.setPosition(position);
}

void VideoPlayer::handleError()
{
    ui->playButton->setEnabled(false);
    QMessageBox::warning(this,tr("Error"),mediaPlayer.errorString());
}

QChart *VideoPlayer::createLineChart(QString path, int index) const
{
    QChart *chart = new QChart();
    QString title="particle";
    title+=QString::number(ui->comboBox->currentIndex()+1,10);
    chart->setTitle(title);
    //![1]

    //![2]
    QString name("Series ");

    QString line;
    //宣告檔案
    QFile file(path);
    //Open
    file.open(QIODevice::ReadOnly);
    QTextStream streamread(&file);
    //讀
    //一次讀一行的寫法
    QLineSeries *series = new QLineSeries(chart);
    for ( int i=0; i<index; i++)
        line = streamread.readLine();
    //close
    file.close();
    QStringList  particlelist  = line.split(' ');

    //ui->comboBox_2->clear();
    for (int i=0; i<particlelist.length(); i++)
        if (!particlelist.at(i).isEmpty())
        {
            series->append(i+1,particlelist.at(i).toDouble());
    //      ui->comboBox_2->addItem(QString::number(i+1,10));
        }


    chart->addSeries(series);



    //別忘記把座標軸添加到chart
    chart->createDefaultAxes();
    QValueAxis *axisX = new QValueAxis;
    axisX->setTitleText("time"); //設置X座標名字
    QValueAxis *axisY = new QValueAxis;
    axisY->setTitleText("distance");

    chart->addAxis(axisX, Qt::AlignBottom);  //並且XY軸的位置是上和右
    chart->addAxis(axisY, Qt::AlignLeft);



    //series->attachAxis(axisY);
    //series->attachAxis(axisX);

    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->hide(); //隱藏圖例



/*    axisX->setRange(0,particlelist.length()-1);   //設置X座標範圍
    axisX->setTitleText("time"); //設置X座標名字

    axisY->setRange(0,int(particleMax+5));
    axisY->setTitleText("distance");

    //別忘記把座標軸添加到chart
    chart->setAxisX(axisX, series);  //並且XY軸的位置是上和右
    chart->setAxisY(axisY, series);
*/
    ui->linechart->setChart(chart);
    ui->linechart->setRenderHint(QPainter::Antialiasing);
}

void VideoPlayer::on_SaveChart_clicked()
{
    /*QScreen * screen = QGuiApplication::primaryScreen();
    QPixmap p = screen->grabWindow(ui->linechart->winId());
    QImage image = p.toImage();
    QString imageName=fileName.mid(0,fileName.lastIndexOf("/")+1);
    imageName+="chart/";
    imageName+=ui->comboBox->currentText();
    imageName+=".jpeg";
    image.save(imageName);

    QMessageBox::information(this, "Save chart","Success!");*/
}

void VideoPlayer::processFinishedSlot(int,QProcess::ExitStatus)
{
    if (processParticle>=1)
    {
        QString imagePath=saveParticleImgPath;
        imagePath+=_processParticleFileList.at(count).mid(_processParticleFileList.at(count).lastIndexOf("/"));
        imagePath=imagePath.mid(0,imagePath.lastIndexOf(".")+1);
        imagePath+="png";
        QImage *image=new QImage(imagePath);
        QPixmap *pixmap=new QPixmap();
        int with = ui->label->width();
        int height = ui->label->height();
        pixmap->convertFromImage(*image);
        QPixmap fitpixmap = pixmap->scaled(with, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->label->setPixmap(fitpixmap);
        ui->label->setAlignment(Qt::AlignCenter);

        QString img_combo=_processParticleFileList.at(count).mid(_processParticleFileList.at(count).lastIndexOf("/")+1);
        img_combo=img_combo.mid(0,img_combo.lastIndexOf(".")+1);
        img_combo+="png";
        ui->comboBox_2->addItem(img_combo);
        ui->comboBox_2->setCurrentIndex(ui->comboBox_2->count()-1);

        if (count==0)
        {
            QProcess *temp_process=new QProcess;
            QString init="/home/ppcb/MATLAB/R2015b/bin/glnxa64/MATLAB -nodesktop -nosplash -r ";
            init+='"';
            init+="init('";
            init+=root;
            init+="',";
            init+=QString::number(_processParticleFileList.size());
            init+=",'";
            init+=saveParticleFullPath;
            init+=_processParticleFileList.at(count).mid(_processParticleFileList.at(0).lastIndexOf("/"));
            init=init.mid(0,init.lastIndexOf("."));
            init+=".txt";
            init+="');quit";
            init+='"';
            init=init.replace("`","\\`");
            temp_process->start("/bin/sh",  QStringList()<< "-c"<<init);
            //qDebug()<<init;
        }
        else
        {
            QProcess *temp_process=new QProcess;
            QString matlab_RPP="/home/ppcb/MATLAB/R2015b/bin/glnxa64/MATLAB -nodesktop -nosplash -r ";
            matlab_RPP+='"';
            matlab_RPP+="record_particle_process(";
            matlab_RPP+=QString::number(_processParticleFileList.size());
            matlab_RPP+=",'";

            matlab_RPP+=root;
            matlab_RPP+="','";

            matlab_RPP+=_processParticleFileList.at(count);
            matlab_RPP+="','";

            matlab_RPP+=saveParticleCenterPath;
            matlab_RPP+=_processParticleFileList.at(count).mid(_processParticleFileList.at(0).lastIndexOf("/"));
            matlab_RPP=matlab_RPP.mid(0,matlab_RPP.lastIndexOf("."));
            matlab_RPP+=".txt";
            matlab_RPP+="','";

            matlab_RPP+=saveParticleCenterPath;
            matlab_RPP+=_processParticleFileList.at(count-1).mid(_processParticleFileList.at(0).lastIndexOf("/"));
            matlab_RPP=matlab_RPP.mid(0,matlab_RPP.lastIndexOf("."));
            matlab_RPP+=".txt";
            matlab_RPP+="','";

            matlab_RPP+=saveParticleFullPath;
            matlab_RPP+=_processParticleFileList.at(count).mid(_processParticleFileList.at(0).lastIndexOf("/"));
            matlab_RPP=matlab_RPP.mid(0,matlab_RPP.lastIndexOf("."));
            matlab_RPP+=".txt";
            matlab_RPP+="','";

            matlab_RPP+=saveParticleFullPath;
            matlab_RPP+=_processParticleFileList.at(count-1).mid(_processParticleFileList.at(0).lastIndexOf("/"));
            matlab_RPP=matlab_RPP.mid(0,matlab_RPP.lastIndexOf("."));
            matlab_RPP+=".txt";
            matlab_RPP+="','";

            matlab_RPP+=saveParticleFullPath;
            matlab_RPP+=_processParticleFileList.at(0).mid(_processParticleFileList.at(0).lastIndexOf("/"));
            matlab_RPP=matlab_RPP.mid(0,matlab_RPP.lastIndexOf("."));
            matlab_RPP+=".txt";
            matlab_RPP+="','";

            matlab_RPP+=saveParticleAreaPath;
            matlab_RPP+=_processParticleFileList.at(count-1).mid(_processParticleFileList.at(0).lastIndexOf("/"));
            matlab_RPP=matlab_RPP.mid(0,matlab_RPP.lastIndexOf("."));
            matlab_RPP+=".txt";
            matlab_RPP+="',";

            matlab_RPP+=QString::number(count+1);
            matlab_RPP+=");quit";
            matlab_RPP+='"';
            matlab_RPP=matlab_RPP.replace("`","\\`");

            temp_process->start("/bin/sh",  QStringList()<< "-c"<<matlab_RPP);
            //qDebug()<<matlab_RPP;
        }

        processParticle--;
        count++;
        /*QString status;
        status.sprintf("%s : %d/%d%s", "Running...\nNumber of Files", pathstring.size()*pathstring2.size()-_processParticleFileList.size(), pathstring.size()*pathstring2.size(),"\nIt takes about 3 minutes for each comparison file......");
        ui->status->setText(status);*/
    }



    if (processParticle== 0)
    {
        //_processParticleFileList.clear();



        QString checkFileExists;
        checkFileExists=root;
        checkFileExists+="/TrackingProcess/recordObjectProcess/ObjectProcess.txt";
        QFileInfo fileInfo(checkFileExists);
        while(!fileInfo.isFile())
        {
            continue;
        }


        QString cal_obj_num;
        cal_obj_num+=saveParticleCenterPath;
        cal_obj_num+=_processParticleFileList.at(0).mid(_processParticleFileList.at(0).lastIndexOf("/"));
        cal_obj_num=cal_obj_num.mid(0,cal_obj_num.lastIndexOf("."));
        cal_obj_num+=".txt";
        qDebug()<<cal_obj_num;
        QString Line;

        //宣告檔案
        QFile file(cal_obj_num);
        //Open
        file.open(QIODevice::ReadOnly);
        QTextStream streamread(&file);
        //讀
        //一次讀一行的寫法
        while(!streamread.atEnd())
        {
            Line = streamread.readLine();
            obj_num++;
            ui->comboBox->addItem(QString::number(obj_num,10));
        }
        file.close();


        count=0;
        QString fileName = "python3 prediction.py ";
        fileName+=_processCellFileList.at(count);
        fileName=fileName.replace("`","\\`");
        fileName+=" ";
        fileName+=saveCellImgPath;
        fileName+=" ";
        fileName+=saveCellBorderPath;
        fileName+=" ";
        fileName+=saveCellFullPath;
        fileName+=" ";
        fileName+=saveCellCenterPath;
        fileName+=" ";
        fileName+=saveCellAreaPath;
        qDebug()<<fileName;
        _process2->start("/bin/sh",  QStringList()<< "-c"<<fileName);



        return;
    }


    if (processParticle >= 1)
    {
        QString fileName = "python3 prediction.py ";
        fileName+=_processParticleFileList.at(count);
        fileName=fileName.replace("`","\\`");
        fileName+=" ";
        fileName+=saveParticleImgPath;
        fileName+=" ";
        fileName+=saveParticleBorderPath;
        fileName+=" ";
        fileName+=saveParticleFullPath;
        fileName+=" ";
        fileName+=saveParticleCenterPath;
        fileName+=" ";
        fileName+=saveParticleAreaPath;

        _process->start("/bin/sh",  QStringList()<< "-c"<<fileName);
        //qDebug()<<fileName;
    }

}

void VideoPlayer::process2FinishedSlot(int,QProcess::ExitStatus)
{
    if (processCell>=1)
    {      
        if (count >0)
        {
            QString cahrtPath=root;
            cahrtPath+="/recordParticleandCellDistance/ParticleandCellProcessDistance.txt";
            createLineChart(cahrtPath,ui->comboBox->currentIndex()+1);
        }

        QString CellName = saveCellImgPath;
        CellName+=_processCellFileList.at(count).mid(_processCellFileList.at(count).lastIndexOf("/"));
        CellName=CellName.mid(0,CellName.lastIndexOf(".")+1);
        CellName+="png";

        QImage *Cellimage=new QImage(CellName);
        QPixmap *Cellpixmap=new QPixmap();
        int Cellwidth = ui->label_3->width();
        int Cellheight = ui->label_3->height();
        Cellpixmap->convertFromImage(*Cellimage);
        QPixmap Cellfitpixmap = Cellpixmap->scaled(Cellwidth, Cellheight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->label_3->setPixmap(Cellfitpixmap);
        ui->label_3->setAlignment(Qt::AlignCenter);

        QProcess *temp_process=new QProcess;
        QString matlab_CPCD="/home/ppcb/MATLAB/R2015b/bin/glnxa64/MATLAB -nodesktop -nosplash -r ";
        matlab_CPCD+='"';
        matlab_CPCD+="calculate_particle_and_cell_distance(";
        matlab_CPCD+=QString::number(_processCellFileList.size());
        matlab_CPCD+=",1040,1392,'";

        matlab_CPCD+=root;
        matlab_CPCD+="','";

        matlab_CPCD+=saveCellFullPath;
        matlab_CPCD+=_processCellFileList.at(count).mid(_processCellFileList.at(count).lastIndexOf("/"));
        matlab_CPCD=matlab_CPCD.mid(0,matlab_CPCD.lastIndexOf("."));
        matlab_CPCD+=".txt";
        matlab_CPCD+="','";

        matlab_CPCD+=saveCellBorderPath;
        matlab_CPCD+=_processCellFileList.at(count).mid(_processCellFileList.at(count).lastIndexOf("/"));
        matlab_CPCD=matlab_CPCD.mid(0,matlab_CPCD.lastIndexOf("."));
        matlab_CPCD+=".txt";
        matlab_CPCD+="','";

        matlab_CPCD+=saveParticleFullPath;
        matlab_CPCD+=_processParticleFileList.at(count).mid(_processParticleFileList.at(count).lastIndexOf("/"));
        matlab_CPCD=matlab_CPCD.mid(0,matlab_CPCD.lastIndexOf("."));
        matlab_CPCD+=".txt";
        matlab_CPCD+="','";

        matlab_CPCD+=saveParticleBorderPath;
        matlab_CPCD+=_processParticleFileList.at(count).mid(_processParticleFileList.at(count).lastIndexOf("/"));
        matlab_CPCD=matlab_CPCD.mid(0,matlab_CPCD.lastIndexOf("."));
        matlab_CPCD+=".txt";
        matlab_CPCD+="','";

        matlab_CPCD+=root;
        matlab_CPCD+="/Plist.txt";
        matlab_CPCD+="','";

        matlab_CPCD+=root;
        matlab_CPCD+="/Clist.txt";
        matlab_CPCD+="',";

        matlab_CPCD+=QString::number(count+1);
        matlab_CPCD+=");quit";
        matlab_CPCD+='"';
        matlab_CPCD=matlab_CPCD.replace("`","\\`");

        temp_process->start("/bin/sh",  QStringList()<< "-c"<<matlab_CPCD);
        //qDebug()<<matlab_CPCD;
        processCell--;
        count++;
    }

    if (processCell== 0)
    {
        QString cahrtPath=root;
        cahrtPath+="/recordParticleandCellDistance/ParticleandCellProcessDistance.txt";
        createLineChart(cahrtPath,count);

        //_processCellFileList.clear();
        //_processParticleFileList.clear();

        //QMessageBox::information(this, "P&C","Success!");
        ui->openButton_2->setEnabled(true);
        ui->openButton->setEnabled(true);    

        return;
    }


    if (processCell>= 1)
    {
        QString fileName = "python3 prediction.py ";
        fileName+=_processCellFileList.at(count);
        fileName=fileName.replace("`","\\`");
        fileName+=" ";
        fileName+=saveCellImgPath;
        fileName+=" ";
        fileName+=saveCellBorderPath;
        fileName+=" ";
        fileName+=saveCellFullPath;
        fileName+=" ";
        fileName+=saveCellCenterPath;
        fileName+=" ";
        fileName+=saveCellAreaPath;

        _process2->start("/bin/sh",  QStringList()<< "-c"<<fileName);
    }

}
