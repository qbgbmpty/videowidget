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
#include <QTime>


QStringList fileName;
static QProcess *_process;
static QProcess *_process2;
static QProcess *_process_cal;
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
int count,dis_count, processParticle, processCell, obj_num, cellImg_count, cal_count,img_count;
int tracking_count,current_dis,test;
QString FirstParticle;
QTime test_time;
double **DisList;
int **recordObjectProcess;

VideoPlayer::VideoPlayer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::VideoPlayer)
    , mediaPlayer(0, QMediaPlayer::VideoSurface)
{
    ui->setupUi(this);

    _process = new QProcess;
    _process2 = new QProcess;
    _process_cal = new QProcess;
    //connect(_process, SIGNAL(finished(int, QProcess::ExitStatus)),this, SLOT(processFinishedSlot(int, QProcess::ExitStatus)));
    connect (_process,SIGNAL(readyReadStandardOutput()) , this , SLOT(processFinishedSlot()));
    connect(_process2, SIGNAL(finished(int, QProcess::ExitStatus)),this, SLOT(process2FinishedSlot(int, QProcess::ExitStatus)));
    connect(_process_cal, SIGNAL(finished(int, QProcess::ExitStatus)),this, SLOT(process_cal_FinishedSlot(int, QProcess::ExitStatus)));

    connect(this, SIGNAL(cal_trigger()),this, SLOT(cellImg_cal_queue()));

    ui->openButton->setText("Select Particle");
    ui->openButton_2->setText("Select Cell");
    ui->runButton->setText("Run");
    connect(ui->openButton, SIGNAL(clicked()), this, SLOT(openFile()));
    connect(ui->openButton_2, SIGNAL(clicked()), this, SLOT(openFile_2()));
    connect(ui->runButton, SIGNAL(clicked()), this, SLOT(Run()));


    ui->playButton->setEnabled(false);
    ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));

    connect(ui->playButton, SIGNAL(clicked()), this, SLOT(play()));
/*
    ui->playButton_2->setEnabled(false);
    ui->playButton_2->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));

    connect(ui->playButton_2, SIGNAL(clicked()), this, SLOT(play_particle()));

    ui->playButton_3->setEnabled(false);
    ui->playButton_3->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));

    connect(ui->playButton_3, SIGNAL(clicked()), this, SLOT(play_particle_2()));
*/

    ui->positionSlider->setRange(0, 0);
    connect(ui->positionSlider, SIGNAL(sliderMoved(int)), this, SLOT(setPosition(int)));
/*
    ui->positionSlider_2->setRange(0, 0);
    connect(ui->positionSlider_2, SIGNAL(sliderMoved(int)), this, SLOT(setPosition_particle(int)));

    ui->positionSlider_3->setRange(0, 0);
    connect(ui->positionSlider_3, SIGNAL(sliderMoved(int)), this, SLOT(setPosition_particle_2(int)));
*/
    mediaPlayer.setVideoOutput(ui->widget_1);



    connect(ui->comboBox, SIGNAL(activated(const QString &)), this,SLOT(particle()));
    connect(ui->comboBox_2, SIGNAL(activated(const QString &)), this,SLOT(particle_time()));

    connect(&mediaPlayer, SIGNAL(stateChanged(QMediaPlayer::State)),this, SLOT(mediaStateChanged(QMediaPlayer::State)));
    connect(&mediaPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));
    connect(&mediaPlayer, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64)));

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
    _processParticleFileList = QFileDialog::getOpenFileNames(this,"Select Partical");
}

void VideoPlayer::openFile_2()
{
    //fileName = QFileDialog::getOpenFileName(this, tr("Open Movie"),QDir::homePath());
    _processCellFileList.clear();
    _processCellFileList = QFileDialog::getOpenFileNames(this,"Select Cell");

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

    ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    ui->positionSlider->setValue(15);
    ui->positionSlider->setRange(0, 40);

    ui->comboBox_2->clear();
    ui->comboBox->clear();
    obj_num=0;
    count=0;
    dis_count=0;
    cellImg_count=0; //跟著matlab分析，matlab分析會被呼叫（張數-1）次，所以起始值需為1
    cal_count=0;
    img_count=0;
    test=0;
    processParticle=_processParticleFileList.size();
    processCell=_processCellFileList.size();

    ui->label->clear();
    ui->label_2->clear();
    ui->label_3->clear();





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
                    temp->mkdir(Borderfolder+"/for_dis");
                    temp->mkdir(Fullfolder);
                    temp->mkdir(Fullfolder+"/for_dis");
                    temp->mkdir(Centerfolder);
                    temp->mkdir(ParticleArea);
                    temp->mkdir(Imgfolder+"/single_mask");
                    for (int i=0; i<_processParticleFileList.size(); i++)
                    {
                        temp->mkdir(Imgfolder+"/single_mask/"+QString::number(i+1));
                        temp->mkdir(Fullfolder+"/for_dis/"+_processParticleFileList.at(i).mid(_processParticleFileList.at(i).lastIndexOf("/")+1,_processParticleFileList.at(i).indexOf(".")-_processParticleFileList.at(i).lastIndexOf("/")-1));
                        temp->mkdir(Borderfolder+"/for_dis/"+_processParticleFileList.at(i).mid(_processParticleFileList.at(i).lastIndexOf("/")+1,_processParticleFileList.at(i).indexOf(".")-_processParticleFileList.at(i).lastIndexOf("/")-1));
                    }
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
            temp->mkdir(saveCellBorderPath+"/for_dis");
            temp->mkdir(saveCellFullPath+"/for_dis");
            temp->mkdir(saveCellCenterPath);
            temp->mkdir(saveCellAreaPath);
            for (int i=0; i<_processCellFileList.size(); i++)
            {
                temp->mkdir(saveCellBorderPath+"/for_dis/"+_processCellFileList.at(i).mid(_processCellFileList.at(i).lastIndexOf("/")+1,_processCellFileList.at(i).indexOf(".")-_processCellFileList.at(i).lastIndexOf("/")-1));
                temp->mkdir(saveCellFullPath+"/for_dis/"+_processCellFileList.at(i).mid(_processCellFileList.at(i).lastIndexOf("/")+1,_processCellFileList.at(i).indexOf(".")-_processCellFileList.at(i).lastIndexOf("/")-1));
            }
            delete temp;

            QString command = "python3 prediction.py ";
            command+=_processParticleFileList.at(0).mid(0,_processParticleFileList.at(0).lastIndexOf("/"));
            command+=" ";
            command+=_processCellFileList.at(0).mid(0,_processCellFileList.at(0).lastIndexOf("/"));
            command=command.replace("`","\\`");
            command+=" ";
            command+=root;
            command+=" ";
            command+=QString::number(_processParticleFileList.size());

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

            ui->label_13->setText("1/"+QString::number(_processParticleFileList.size()));

            Pfile.close();
            Cfile.close();

            QString NowAnalysName = _processParticleFileList.at(0);
            QImage *NowAnalysimage=new QImage(NowAnalysName);
            QPixmap *NowAnalyspixmap=new QPixmap();
            int NowAnalyswidth = ui->label_16->width();
            int NowAnalysheight = ui->label_16->height();
            NowAnalyspixmap->convertFromImage(*NowAnalysimage);
            QPixmap NowAnalysfitpixmap = NowAnalyspixmap->scaled(NowAnalyswidth, NowAnalysheight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            ui->label_16->setPixmap(NowAnalysfitpixmap);
            ui->label_16->setAlignment(Qt::AlignCenter);


            test_time.start();
            _process->setReadChannel(QProcess::StandardOutput);
            _process->start("/bin/sh", QStringList()<<"-c"<<command);
            //qDebug()<<command;


            ui->openButton->setEnabled(false);
            ui->openButton_2->setEnabled(false);
            ui->runButton->setEnabled(false);


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
        break;
    default:
        mediaPlayer.play();

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

        QString index;
        QString ObjectProcessPath=root;
        ObjectProcessPath+="/TrackingProcess/recordObjectProcess/ObjectProcess";
        ObjectProcessPath+=QString::number(ui->comboBox_2->currentIndex()+1);
        ObjectProcessPath+=".txt";
        QFile file(ObjectProcessPath);
        //Open
        file.open(QIODevice::ReadOnly);
        QTextStream streamread(&file);
        //讀
        //一次讀一行的寫法
        for ( int i=0; i<=ui->comboBox->currentIndex(); i++)
            index = streamread.readLine();
        //close
        file.close();

        QString FileName=_processParticleFileList.at(ui->comboBox_2->currentIndex());
        FileName=FileName.mid(FileName.lastIndexOf("/")+1);
        QString particleName = saveParticleImgPath;
        particleName+="/single_mask/";
        particleName+=QString::number(ui->comboBox_2->currentIndex()+1);
        particleName+="/";
        particleName+=FileName;
        particleName=particleName.mid(0,particleName.lastIndexOf("."));
        particleName+="_";
        particleName+=index;
        particleName+=".png";

        QImage *image=new QImage(particleName);
        QPixmap *pixmap=new QPixmap();
        int with = ui->label->width();
        int height = ui->label->height();
        pixmap->convertFromImage(*image);
        QPixmap fitpixmap = pixmap->scaled(with, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->label->setPixmap(fitpixmap);
        ui->label->setAlignment(Qt::AlignCenter);
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
        int with = ui->label_2->width();
        int height = ui->label_2->height();
        pixmap->convertFromImage(*image);
        QPixmap fitpixmap = pixmap->scaled(with, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->label_2->setPixmap(fitpixmap);
        ui->label_2->setAlignment(Qt::AlignCenter);

        QString CellName = saveCellImgPath;
        CellName+="/";
        CellName+=_processCellFileList.at(ui->comboBox_2->currentIndex()).mid(_processCellFileList.at(ui->comboBox_2->currentIndex()).lastIndexOf("/")+1);
        CellName=CellName.mid(0,CellName.lastIndexOf('.'));
        CellName+=".png";
        //qDebug()<<CellName;

        QImage *Cellimage=new QImage(CellName);
        QPixmap *Cellpixmap=new QPixmap();
        int Cellwidth = ui->label_3->width();
        int Cellheight = ui->label_3->height();
        Cellpixmap->convertFromImage(*Cellimage);
        QPixmap Cellfitpixmap = Cellpixmap->scaled(Cellwidth, Cellheight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->label_3->setPixmap(Cellfitpixmap);
        ui->label_3->setAlignment(Qt::AlignCenter);

        QString cahrtPath=root;
        cahrtPath+="/recordParticleandCellDistance/ParticleandCellProcessDistance.txt";
        createLineChart(cahrtPath,ui->comboBox->currentIndex()+1);

        QString index;
        QString ObjectProcessPath=root;
        ObjectProcessPath+="/TrackingProcess/recordObjectProcess/ObjectProcess";
        ObjectProcessPath+=QString::number(ui->comboBox_2->currentIndex()+1);
        ObjectProcessPath+=".txt";
        QFile file(ObjectProcessPath);
        //Open
        file.open(QIODevice::ReadOnly);
        QTextStream streamread(&file);
        //讀
        //一次讀一行的寫法
        for ( int i=0; i<=ui->comboBox->currentIndex(); i++)
            index = streamread.readLine();
        //close
        file.close();

        QString FileName=_processParticleFileList.at(ui->comboBox_2->currentIndex());
        FileName=FileName.mid(FileName.lastIndexOf("/")+1);
        QString single_particleName = saveParticleImgPath;
        single_particleName+="/single_mask/";
        single_particleName+=QString::number(ui->comboBox_2->currentIndex()+1);
        single_particleName+="/";
        single_particleName+=FileName;
        single_particleName=single_particleName.mid(0,single_particleName.lastIndexOf("."));
        single_particleName+="_";
        single_particleName+=index;
        single_particleName+=".png";

        QImage *single_image=new QImage(single_particleName);
        QPixmap *single_pixmap=new QPixmap();
        int single_with = ui->label->width();
        int single_height = ui->label->height();
        pixmap->convertFromImage(*image);
        QPixmap single_fitpixmap = single_pixmap->scaled(single_with, single_height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->label->setPixmap(single_fitpixmap);
        ui->label->setAlignment(Qt::AlignCenter);
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
    if (!ui->comboBox_2->currentText().isEmpty() && !ui->comboBox->currentText().isEmpty())
    {
        QString index;
        QString ObjectProcessPath=root;
        ObjectProcessPath+="/TrackingProcess/recordObjectProcess/ObjectProcess";
        ObjectProcessPath+=QString::number(ui->comboBox_2->currentIndex()+1);
        ObjectProcessPath+=".txt";
        QFile file(ObjectProcessPath);
        //Open
        file.open(QIODevice::ReadOnly);
        QTextStream streamread(&file);
        //讀
        //一次讀一行的寫法
        for ( int i=0; i<=ui->comboBox->currentIndex(); i++)
            index = streamread.readLine();
        //close
        file.close();

        QString FileName=_processParticleFileList.at(ui->comboBox_2->currentIndex());
        FileName=FileName.mid(FileName.lastIndexOf("/")+1);
        QString particleName = saveParticleImgPath;
        particleName+="/single_mask/";
        particleName+=QString::number(ui->comboBox_2->currentIndex()+1);
        particleName+="/";
        particleName+=FileName;
        particleName=particleName.mid(0,particleName.lastIndexOf("."));
        particleName+="_";
        particleName+=index;
        particleName+=".png";

        QImage *image=new QImage(particleName);
        QPixmap *pixmap=new QPixmap();
        int with = ui->label->width();
        int height = ui->label->height();
        pixmap->convertFromImage(*image);
        QPixmap fitpixmap = pixmap->scaled(with, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->label->setPixmap(fitpixmap);
        ui->label->setAlignment(Qt::AlignCenter);
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
    /*switch(state) {
    case QMediaPlayer::PlayingState:
        ui->playButton_2->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        break;
    default:
        ui->playButton_2->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        break;
    }*/
}

void VideoPlayer::mediaStateChanged_particle_2(QMediaPlayer::State state)
{
    /*switch(state) {
    case QMediaPlayer::PlayingState:
        ui->playButton_3->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        break;
    default:
        ui->playButton_3->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        break;
    }*/
}

void VideoPlayer::positionChanged(qint64 position)
{
    ui->positionSlider->setValue(position);
}

void VideoPlayer::positionChanged_particle(qint64 position)
{
    //ui->positionSlider_2->setValue(position);
}

void VideoPlayer::positionChanged_particle_2(qint64 position)
{
    //ui->positionSlider_3->setValue(position);
}

void VideoPlayer::durationChanged(qint64 duration)
{
    ui->positionSlider->setRange(0, duration);
}

void VideoPlayer::durationChanged_particle(qint64 duration)
{
   // ui->positionSlider_2->setRange(0, duration);
}

void VideoPlayer::durationChanged_particle_2(qint64 duration)
{
    //ui->positionSlider_3->setRange(0, duration);
}

void VideoPlayer::setPosition(int position)
{
    mediaPlayer.setPosition(position);

}


void VideoPlayer::handleError()
{
    ui->playButton->setEnabled(false);
    QMessageBox::warning(this,tr("Error"),mediaPlayer.errorString());
}

QChart *VideoPlayer::createLineChart(QString path, int index) const
{
    //double particleMax=0;
    QChart *chart = new QChart();
    QString title="particle";
    title+=QString::number(ui->comboBox->currentIndex()+1,10);
    chart->setTitle(title);
    //![1]

    //![2]
    QString name("Series ");
    QLineSeries *series = new QLineSeries(chart);
/*
//for matlab
    QString line;
    //宣告檔案
    QFile file(path);
    //Open
    file.open(QIODevice::ReadOnly);
    QTextStream streamread(&file);
    //讀
    //一次讀一行的寫法

    for ( int i=0; i<index; i++)
        line = streamread.readLine();
    //close
    file.close();
    QStringList  particlelist  = line.split(' ');
    for (int i=0; i<particlelist.length(); i++)
        if (!particlelist.at(i).isEmpty())
            series->append(i+1,particlelist.at(i).toDouble());
*/


//for OMP/GPU
    for (int j=0; j<current_dis; j++)
    {
        if (recordObjectProcess[index-1][j]!=0) //[obj_num][image_num]
            series->append(j+1,DisList[recordObjectProcess[index-1][j]-1][j]);
        else
        {
            QString Sup_Cal_path=root+"/recordParticleandCellDistance"+_processParticleFileList.at(j).mid(_processParticleFileList.at(j).lastIndexOf("/"),_processParticleFileList.at(j).indexOf(".")-_processParticleFileList.at(j).lastIndexOf("/"))+"_"+QString::number(index-1)+".txt";

            QFile file(Sup_Cal_path);
            //Open
            if (!file.open(QIODevice::ReadOnly))
            {
                QString cerr="Cannot open file for reading.";
                series->append(j+1,0);
            }
            else
            {
                QTextStream streamread(&file);
                QString line;
                line = streamread.readLine();
                int obj_index=line.toInt();
                //讀
                //一次讀一行的寫法

                for ( int i=0; i<obj_index; i++)
                    line = streamread.readLine();
                //close
                file.close();
                series->append(j+1,line.toDouble());
            }
        }

    }


    //ui->comboBox_2->clear();




//
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


/*
    axisX->setRange(0,particlelist.length()-1);   //設置X座標範圍
    axisX->setTitleText("time"); //設置X座標名字

    axisY->setRange(0,int(particleMax));
    axisY->setTitleText("distance");
*/
    //別忘記把座標軸添加到chart
//    chart->setAxisX(axisX, series);  //並且XY軸的位置是上和右
    //chart->setAxisY(axisY, series);

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

void VideoPlayer::processFinishedSlot()
{
    if (processParticle>=1)
        {

            //一次讀一行的寫法
            //計算object數目並新增到combobox
            if (ui->comboBox->count()==0)
            {
                QString cal_obj_num;
                cal_obj_num+=saveParticleCenterPath;
                cal_obj_num+=_processParticleFileList.at(0).mid(_processParticleFileList.at(0).lastIndexOf("/"));
                cal_obj_num=cal_obj_num.mid(0,cal_obj_num.lastIndexOf("."));
                cal_obj_num+=".txt";
                QString Line;

                //宣告檔案
                QFile file(cal_obj_num);
                //Open
                file.open(QIODevice::ReadOnly);
                QTextStream streamread(&file);
                //讀
                while(!streamread.atEnd())
                {
                    Line = streamread.readLine();
                    obj_num++;
                    ui->comboBox->addItem(QString::number(obj_num,10));
                }
                file.close();
                DisList=new double*[obj_num]; //creates a new array of pointers to int objects
                recordObjectProcess=new int*[obj_num];
                for(int i=0; i<obj_num; i++)
                {
                    DisList[i]=new double[_processParticleFileList.size()];
                    recordObjectProcess[i]=new int[_processParticleFileList.size()];
                }
            }

            //show particle detection image
            QString imagePath=saveParticleImgPath;
            imagePath+=_processParticleFileList.at(count).mid(_processParticleFileList.at(count).lastIndexOf("/"));
            imagePath=imagePath.mid(0,imagePath.lastIndexOf(".")+1);
            imagePath+="png";
            QImage *image=new QImage(imagePath);
            QPixmap *pixmap=new QPixmap();
            int with = ui->label_2->width();
            int height = ui->label_2->height();
            pixmap->convertFromImage(*image);
            QPixmap fitpixmap = pixmap->scaled(with, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            ui->label_2->setPixmap(fitpixmap);
            ui->label_2->setAlignment(Qt::AlignCenter);

            //add particle name to the combobox
            QString img_combo=_processParticleFileList.at(count).mid(_processParticleFileList.at(count).lastIndexOf("/")+1);
            img_combo=img_combo.mid(0,img_combo.lastIndexOf(".")+1);
            img_combo+="png";
            ui->comboBox_2->addItem(img_combo);
            ui->comboBox_2->setCurrentIndex(ui->comboBox_2->count()-1);

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

            //cellImg_count++;


            //若是第一次detection執行完畢，則先執行matlab的init
            //反之則執行matlab程式，並connect Tracking_imge,count++
            if (count==0)
            {
                tracking_count=0;

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
                connect(temp_process, SIGNAL(finished(int, QProcess::ExitStatus)),this, SLOT(Tracking_image(int, QProcess::ExitStatus)));
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


        //執行cell detection
/*
        QString fileName = "python3 prediction.py ";
        fileName+=_processCellFileList.at(img_count);
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
        fileName+=" 1";
        fileName+=" 1";
        //qDebug()<<fileName;
        _process2->start("/bin/sh",  QStringList()<< "-c"<<fileName);
*/

        QProcess *create_process=new QProcess;

        connect(create_process, SIGNAL(finished(int, QProcess::ExitStatus)),this, SLOT(SetVideo()));
        QString create_video="python3 img2video.py ";
        create_video+=saveParticleImgPath;
        create_video+=" ";
        create_video+=root;
        create_video+="/ ";
        create_video+=root;
        create_video+="/Plist.txt";
        create_process->start("/bin/sh",  QStringList()<< "-c"<<create_video);
        //qDebug()<<create_video;
        //qDebug()<<test_time.elapsed()/1000.0<<"s";


        return;
    }


    if (processParticle >= 1)
    {
        /*
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
        fileName+=" ";
        fileName+=saveParticleImgPath;
        fileName+="/single_mask/";
        fileName+=QString::number(count+1);
        fileName+=" 0";


        _process->start("/bin/sh",  QStringList()<< "-c"<<fileName);
        //qDebug()<<fileName;
        */

        ui->label_13->setText(QString::number(count+1)+"/"+QString::number(_processParticleFileList.size()));

        QString NowAnalysName = _processParticleFileList.at(count);
        QImage *NowAnalysimage=new QImage(NowAnalysName);
        QPixmap *NowAnalyspixmap=new QPixmap();
        int NowAnalyswidth = ui->label_16->width();
        int NowAnalysheight = ui->label_16->height();
        NowAnalyspixmap->convertFromImage(*NowAnalysimage);
        QPixmap NowAnalysfitpixmap = NowAnalyspixmap->scaled(NowAnalyswidth, NowAnalysheight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->label_16->setPixmap(NowAnalysfitpixmap);
        ui->label_16->setAlignment(Qt::AlignCenter);
/*
        QString fileName = "python3 prediction.py ";
        fileName+=_processParticleFileList.at(count).mid(count,_processParticleFileList.at(count).lastIndexOf("/"));
        fileName+=" ";
        fileName+=_processCellFileList.at(count).mid(count,_processCellFileList.at(count).lastIndexOf("/"));
        fileName=fileName.replace("`","\\`");
        fileName+=" ";
        fileName+=root;
        fileName+=" ";
        fileName+=QString::number(_processParticleFileList.size());

        //qDebug()<<fileName;
        _process->start("/bin/sh",  QStringList()<< "-c"<<fileName);
*/
    }
}

void VideoPlayer::process2FinishedSlot(int,QProcess::ExitStatus)
{
    if (processCell>=1)
    {
        //show cell detection image
        QString CellName = saveCellImgPath;
        CellName+=_processCellFileList.at(img_count).mid(_processCellFileList.at(img_count).lastIndexOf("/"));
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

        cellImg_count++;

        processCell--;
        img_count++;

    }

    if (processCell== 0)
    {


        //_processCellFileList.clear();
        //_processParticleFileList.clear();

        //QMessageBox::information(this, "P&C","Success!");
        //emit cell_image_trigger(1);

        return;
    }


    if (processCell>= 1)
    {
        /*
        QString fileName = "python3 prediction.py ";
        fileName+=_processCellFileList.at(img_count);
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
        fileName+=" 1";
        fileName+=" 1";

        //qDebug()<<fileName;
        _process2->start("/bin/sh",  QStringList()<< "-c"<<fileName);
        */
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
        fileName+=" ";
        fileName+=saveParticleImgPath;
        fileName+="/single_mask/";
        fileName+=QString::number(count+1);
        fileName+=" 0";

        ui->label_13->setText(QString::number(count+1)+"/"+QString::number(_processParticleFileList.size()));

        QString NowAnalysName = _processParticleFileList.at(count);
        QImage *NowAnalysimage=new QImage(NowAnalysName);
        QPixmap *NowAnalyspixmap=new QPixmap();
        int NowAnalyswidth = ui->label_16->width();
        int NowAnalysheight = ui->label_16->height();
        NowAnalyspixmap->convertFromImage(*NowAnalysimage);
        QPixmap NowAnalysfitpixmap = NowAnalyspixmap->scaled(NowAnalyswidth, NowAnalysheight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->label_16->setPixmap(NowAnalysfitpixmap);
        ui->label_16->setAlignment(Qt::AlignCenter);

        _process->start("/bin/sh",  QStringList()<< "-c"<<fileName);
    }

}

void VideoPlayer::cellImg_cal_queue()
{

        qDebug()<<"cellImg_count:"<<cellImg_count;
        qDebug()<<cal_count;
        if (dis_count==0)
        {
            current_dis=0;
            QString matlab_CPCD="/home/ppcb/MATLAB/R2015b/bin/glnxa64/MATLAB -nodesktop -nosplash -r ";
            matlab_CPCD+='"';
            matlab_CPCD+="calculate_particle_and_cell_distance(";
            matlab_CPCD+=QString::number(_processCellFileList.size());
            matlab_CPCD+=",1040,1392,'";

            matlab_CPCD+=root;
            matlab_CPCD+="','";

            matlab_CPCD+=saveCellFullPath;
            matlab_CPCD+=_processCellFileList.at(dis_count).mid(_processCellFileList.at(dis_count).lastIndexOf("/"));
            matlab_CPCD=matlab_CPCD.mid(0,matlab_CPCD.lastIndexOf("."));
            matlab_CPCD+=".txt";
            matlab_CPCD+="','";

            matlab_CPCD+=saveCellBorderPath;
            matlab_CPCD+=_processCellFileList.at(dis_count).mid(_processCellFileList.at(dis_count).lastIndexOf("/"));
            matlab_CPCD=matlab_CPCD.mid(0,matlab_CPCD.lastIndexOf("."));
            matlab_CPCD+=".txt";
            matlab_CPCD+="','";

            matlab_CPCD+=saveParticleFullPath;
            matlab_CPCD+=_processParticleFileList.at(dis_count).mid(_processParticleFileList.at(dis_count).lastIndexOf("/"));
            matlab_CPCD=matlab_CPCD.mid(0,matlab_CPCD.lastIndexOf("."));
            matlab_CPCD+=".txt";
            matlab_CPCD+="','";

            matlab_CPCD+=saveParticleBorderPath;
            matlab_CPCD+=_processParticleFileList.at(dis_count).mid(_processParticleFileList.at(dis_count).lastIndexOf("/"));
            matlab_CPCD=matlab_CPCD.mid(0,matlab_CPCD.lastIndexOf("."));
            matlab_CPCD+=".txt";
            matlab_CPCD+="','";

            matlab_CPCD+=root;
            matlab_CPCD+="/Plist.txt";
            matlab_CPCD+="','";

            matlab_CPCD+=root;
            matlab_CPCD+="/Clist.txt";
            matlab_CPCD+="',";

            matlab_CPCD+=QString::number(dis_count+1);
            matlab_CPCD+=");quit";
            matlab_CPCD+='"';
            matlab_CPCD=matlab_CPCD.replace("`","\\`");

            //_process_cal->start("/bin/sh",  QStringList()<< "-c"<<matlab_CPCD);
            //qDebug()<<matlab_CPCD;

            QString GPU_CPCD="./border_point_ori_with_supCal.out ";
            //QString GPU_CPCD="./border_point_test ";
            QString GPU_CPCD_1=saveCellFullPath;
            GPU_CPCD_1+="/for_dis";
            GPU_CPCD_1+=_processCellFileList.at(dis_count).mid(_processCellFileList.at(dis_count).lastIndexOf("/"));
            GPU_CPCD_1=GPU_CPCD_1.mid(0,GPU_CPCD_1.lastIndexOf("."));
            GPU_CPCD_1=GPU_CPCD_1.replace("`","\\`");
            GPU_CPCD_1+="/ ";
            QString GPU_CPCD_2=saveCellBorderPath;
            GPU_CPCD_2+="/for_dis";
            GPU_CPCD_2+=_processCellFileList.at(dis_count).mid(_processCellFileList.at(dis_count).lastIndexOf("/"));
            GPU_CPCD_2=GPU_CPCD_2.mid(0,GPU_CPCD_2.lastIndexOf("."));
            GPU_CPCD_2=GPU_CPCD_2.replace("`","\\`");
            GPU_CPCD_2+="/ ";
            QString GPU_CPCD_3=saveParticleFullPath;
            GPU_CPCD_3+="/for_dis";
            GPU_CPCD_3+=_processParticleFileList.at(dis_count).mid(_processParticleFileList.at(dis_count).lastIndexOf("/"));
            GPU_CPCD_3=GPU_CPCD_3.mid(0,GPU_CPCD_3.lastIndexOf("."));
            GPU_CPCD_3=GPU_CPCD_3.replace("`","\\`");
            GPU_CPCD_3+="/ ";
            QString GPU_CPCD_4=saveParticleBorderPath;
            GPU_CPCD_4+="/for_dis";
            GPU_CPCD_4+=_processParticleFileList.at(dis_count).mid(_processParticleFileList.at(dis_count).lastIndexOf("/"));
            GPU_CPCD_4=GPU_CPCD_4.mid(0,GPU_CPCD_4.lastIndexOf("."));
            GPU_CPCD_4=GPU_CPCD_4.replace("`","\\`");
            GPU_CPCD_4+="/ ";
            GPU_CPCD+=GPU_CPCD_1;
            GPU_CPCD+=GPU_CPCD_2;
            GPU_CPCD+=GPU_CPCD_3;
            GPU_CPCD+=GPU_CPCD_4;
            GPU_CPCD+=root;
            GPU_CPCD+="/recordParticleandCellDistance/ParticleandCellDistance";
            GPU_CPCD+=QString::number(dis_count+1);
            GPU_CPCD+=".txt ";
            GPU_CPCD+=root;
            GPU_CPCD+=" ";
            GPU_CPCD+=root;
            GPU_CPCD+="/Plist.txt ";
            GPU_CPCD+=root;
            GPU_CPCD+="/Clist.txt ";
            GPU_CPCD+=QString::number(dis_count+1);
            _process_cal->start("/bin/sh",  QStringList()<< "-c"<<GPU_CPCD);



            dis_count++;
        }
        if( cal_count==_processParticleFileList.size())
        {
            QString cahrtPath=root;
            current_dis++;
            SortDisFile();
            cahrtPath+="/recordParticleandCellDistance/ParticleandCellProcessDistance.txt";
            createLineChart(cahrtPath,ui->comboBox->currentIndex()+1);
            ui->openButton_2->setEnabled(true);
            ui->openButton->setEnabled(true);
            ui->runButton->setEnabled(true);
            qDebug()<<test_time.elapsed()/1000.0<<"s";

            return;
        }
        if (cellImg_count<=cal_count)
            return;

        if ( cellImg_count>cal_count && cal_count>current_dis)
        {
            current_dis++;
            SortDisFile();
            QString cahrtPath=root;
            cahrtPath+="/recordParticleandCellDistance/ParticleandCellProcessDistance.txt";
            createLineChart(cahrtPath,ui->comboBox->currentIndex()+1);

            QString matlab_CPCD="/home/ppcb/MATLAB/R2015b/bin/glnxa64/MATLAB -nodesktop -nosplash -r ";
            matlab_CPCD+='"';
            matlab_CPCD+="calculate_particle_and_cell_distance(";
            matlab_CPCD+=QString::number(_processCellFileList.size());
            matlab_CPCD+=",1040,1392,'";

            matlab_CPCD+=root;
            matlab_CPCD+="','";

            matlab_CPCD+=saveCellFullPath;
            matlab_CPCD+=_processCellFileList.at(dis_count).mid(_processCellFileList.at(dis_count).lastIndexOf("/"));
            matlab_CPCD=matlab_CPCD.mid(0,matlab_CPCD.lastIndexOf("."));
            matlab_CPCD+=".txt";
            matlab_CPCD+="','";

            matlab_CPCD+=saveCellBorderPath;
            matlab_CPCD+=_processCellFileList.at(dis_count).mid(_processCellFileList.at(dis_count).lastIndexOf("/"));
            matlab_CPCD=matlab_CPCD.mid(0,matlab_CPCD.lastIndexOf("."));
            matlab_CPCD+=".txt";
            matlab_CPCD+="','";

            matlab_CPCD+=saveParticleFullPath;
            matlab_CPCD+=_processParticleFileList.at(dis_count).mid(_processParticleFileList.at(dis_count).lastIndexOf("/"));
            matlab_CPCD=matlab_CPCD.mid(0,matlab_CPCD.lastIndexOf("."));
            matlab_CPCD+=".txt";
            matlab_CPCD+="','";

            matlab_CPCD+=saveParticleBorderPath;
            matlab_CPCD+=_processParticleFileList.at(dis_count).mid(_processParticleFileList.at(dis_count).lastIndexOf("/"));
            matlab_CPCD=matlab_CPCD.mid(0,matlab_CPCD.lastIndexOf("."));
            matlab_CPCD+=".txt";
            matlab_CPCD+="','";

            matlab_CPCD+=root;
            matlab_CPCD+="/Plist.txt";
            matlab_CPCD+="','";

            matlab_CPCD+=root;
            matlab_CPCD+="/Clist.txt";
            matlab_CPCD+="',";

            matlab_CPCD+=QString::number(dis_count+1);
            matlab_CPCD+=");quit";
            matlab_CPCD+='"';
            matlab_CPCD=matlab_CPCD.replace("`","\\`");

            //_process_cal->start("/bin/sh",  QStringList()<< "-c"<<matlab_CPCD);
            //qDebug()<<matlab_CPCD;

            QString GPU_CPCD="./border_point_ori_with_supCal.out ";
            //QString GPU_CPCD="./border_point_test ";
            QString GPU_CPCD_1=saveCellFullPath;
            GPU_CPCD_1+="/for_dis";
            GPU_CPCD_1+=_processCellFileList.at(dis_count).mid(_processCellFileList.at(dis_count).lastIndexOf("/"));
            GPU_CPCD_1=GPU_CPCD_1.mid(0,GPU_CPCD_1.lastIndexOf("."));
            GPU_CPCD_1=GPU_CPCD_1.replace("`","\\`");
            GPU_CPCD_1+="/ ";
            QString GPU_CPCD_2=saveCellBorderPath;
            GPU_CPCD_2+="/for_dis";
            GPU_CPCD_2+=_processCellFileList.at(dis_count).mid(_processCellFileList.at(dis_count).lastIndexOf("/"));
            GPU_CPCD_2=GPU_CPCD_2.mid(0,GPU_CPCD_2.lastIndexOf("."));
            GPU_CPCD_2=GPU_CPCD_2.replace("`","\\`");
            GPU_CPCD_2+="/ ";
            QString GPU_CPCD_3=saveParticleFullPath;
            GPU_CPCD_3+="/for_dis";
            GPU_CPCD_3+=_processParticleFileList.at(dis_count).mid(_processParticleFileList.at(dis_count).lastIndexOf("/"));
            GPU_CPCD_3=GPU_CPCD_3.mid(0,GPU_CPCD_3.lastIndexOf("."));
            GPU_CPCD_3=GPU_CPCD_3.replace("`","\\`");
            GPU_CPCD_3+="/ ";
            QString GPU_CPCD_4=saveParticleBorderPath;
            GPU_CPCD_4+="/for_dis";
            GPU_CPCD_4+=_processParticleFileList.at(dis_count).mid(_processParticleFileList.at(dis_count).lastIndexOf("/"));
            GPU_CPCD_4=GPU_CPCD_4.mid(0,GPU_CPCD_4.lastIndexOf("."));
            GPU_CPCD_4=GPU_CPCD_4.replace("`","\\`");
            GPU_CPCD_4+="/ ";
            GPU_CPCD+=GPU_CPCD_1;
            GPU_CPCD+=GPU_CPCD_2;
            GPU_CPCD+=GPU_CPCD_3;
            GPU_CPCD+=GPU_CPCD_4;
            GPU_CPCD+=root;
            GPU_CPCD+="/recordParticleandCellDistance/ParticleandCellDistance";
            GPU_CPCD+=QString::number(dis_count+1);
            GPU_CPCD+=".txt ";
            GPU_CPCD+=root;
            GPU_CPCD+=" ";
            GPU_CPCD+=root;
            GPU_CPCD+="/Clist.txt ";
            GPU_CPCD+=root;
            GPU_CPCD+="/Plist.txt ";
            GPU_CPCD+=QString::number(dis_count+1);


            _process_cal->start("/bin/sh",  QStringList()<< "-c"<<GPU_CPCD);
            //qDebug()<<GPU_CPCD;

            dis_count++;
        }



}

void VideoPlayer::process_cal_FinishedSlot(int,QProcess::ExitStatus)
{
    cal_count++;
    emit cal_trigger();
}

void VideoPlayer::Tracking_image(int,QProcess::ExitStatus)
{
    //qDebug()<<"Tracking_image";
    QFile srcFile(root+"/TrackingProcess/trackPath/Log/Log.txt");
    if (!srcFile.open(QIODevice::ReadOnly))
        QString cerr="Cannot open file for reading.";

    ui->textBrowser->setText(srcFile.readAll());
    srcFile.close();

    cellImg_count++;
    if (cellImg_count==_processParticleFileList.size()-1)
        cellImg_count++;
    tracking_count++;

    if (tracking_count<=1)
    {
        QString FileName=_processParticleFileList.at(tracking_count-1);
        FileName=FileName.mid(FileName.lastIndexOf("/")+1);
        QString particleName = saveParticleImgPath;
        particleName+="/single_mask/";
        particleName+=QString::number(tracking_count);
        particleName+="/";
        particleName+=FileName;
        particleName=particleName.mid(0,particleName.lastIndexOf("."));
        particleName+="_";
        particleName+=QString::number(ui->comboBox->currentIndex()+1);
        particleName+=".png";
        qDebug()<<particleName;
        QImage *image=new QImage(particleName);
        QPixmap *pixmap=new QPixmap();
        int with = ui->label->width();
        int height = ui->label->height();
        pixmap->convertFromImage(*image);
        QPixmap fitpixmap = pixmap->scaled(with, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->label->setPixmap(fitpixmap);
        ui->label->setAlignment(Qt::AlignCenter);
    }

    //qDebug()<<tracking_count;
    else
    {

        QString index;
        QString ObjectProcessPath=root;
        ObjectProcessPath+="/TrackingProcess/recordObjectProcess/ObjectProcess";
        ObjectProcessPath+=QString::number(tracking_count-1);
        ObjectProcessPath+=".txt";
        QFile file(ObjectProcessPath);
        //Open
        file.open(QIODevice::ReadOnly);
        QTextStream streamread(&file);
        //讀
        //一次讀一行的寫法
        for ( int i=0; i<=ui->comboBox->currentIndex(); i++)
            index = streamread.readLine();
        //close
        file.close();


        QString FileName=_processParticleFileList.at(tracking_count-1);
        FileName=FileName.mid(FileName.lastIndexOf("/")+1);
        QString particleName = saveParticleImgPath;
        particleName+="/single_mask/";
        particleName+=QString::number(tracking_count);
        particleName+="/";
        particleName+=FileName;
        particleName=particleName.mid(0,particleName.lastIndexOf("."));
        particleName+="_";
        particleName+=index;
        particleName+=".png";
        qDebug()<<particleName;
        QImage *image=new QImage(particleName);
        QPixmap *pixmap=new QPixmap();
        int with = ui->label->width();
        int height = ui->label->height();
        pixmap->convertFromImage(*image);
        QPixmap fitpixmap = pixmap->scaled(with, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->label->setPixmap(fitpixmap);
        ui->label->setAlignment(Qt::AlignCenter);

        emit cal_trigger();

        if (tracking_count==_processParticleFileList.size()-1)
        {
            QString index;
            QString ObjectProcessPath=root;
            ObjectProcessPath+="/TrackingProcess/recordObjectProcess/ObjectProcess";
            ObjectProcessPath+=QString::number(tracking_count);
            ObjectProcessPath+=".txt";
            QFile file(ObjectProcessPath);
            //Open
            file.open(QIODevice::ReadOnly);
            QTextStream streamread(&file);
            //讀
            //一次讀一行的寫法
            for ( int i=0; i<=ui->comboBox->currentIndex(); i++)
                index = streamread.readLine();
            //close
            file.close();


            QString FileName=_processParticleFileList.at(tracking_count);
            FileName=FileName.mid(FileName.lastIndexOf("/")+1);
            QString particleName = saveParticleImgPath;
            particleName+="/single_mask/";
            particleName+=QString::number(tracking_count+1);
            particleName+="/";
            particleName+=FileName;
            particleName=particleName.mid(0,particleName.lastIndexOf("."));
            particleName+="_";
            particleName+=index;
            particleName+=".png";
            qDebug()<<particleName;
            QImage *image=new QImage(particleName);
            QPixmap *pixmap=new QPixmap();
            int with = ui->label->width();
            int height = ui->label->height();
            pixmap->convertFromImage(*image);
            QPixmap fitpixmap = pixmap->scaled(with, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            ui->label->setPixmap(fitpixmap);
            ui->label->setAlignment(Qt::AlignCenter);
            //qDebug()<<particleName;
        }
        //qDebug()<<particleName;
        //
    }


}

void VideoPlayer::SetVideo()
{
    QString video_name=root;
    video_name+="/video.avi";
    mediaPlayer.setMedia(QUrl::fromLocalFile(video_name));
    ui->playButton->setEnabled(true);
    mediaPlayer.play();
}

void VideoPlayer::SortDisFile()
{
    QString Disline;
    QString recordObjectProcessline;
    //宣告檔案
    QFile Disread(root+"/recordParticleandCellDistance/ParticleandCellDistance"+QString::number(current_dis)+".txt");
    QFile recordObjectProcessread(root+"/TrackingProcess/recordObjectProcess/ObjectProcess"+QString::number(current_dis)+".txt");
    //Open
    Disread.open(QIODevice::ReadOnly);
    QTextStream streamread(&Disread);
    recordObjectProcessread.open(QIODevice::ReadOnly);
    QTextStream streamrecordObjectProcess(&recordObjectProcessread);
    //讀
    //一次讀一行的寫法


    for ( int i=0; i<obj_num; i++)
    {
        Disline = streamread.readLine();
        recordObjectProcessline=recordObjectProcessread.readLine();
        DisList[i][current_dis-1]=Disline.toDouble();
        recordObjectProcess[i][current_dis-1]=recordObjectProcessline.toInt();

    }


    //close
    Disread.close();
    recordObjectProcessread.close();


}
