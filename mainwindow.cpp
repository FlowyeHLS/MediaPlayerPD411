#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStyle>
#include <QFileDialog>
#include <QTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

	//button style
	ui->pushButtonAdd->setIcon(style()->standardIcon(QStyle::SP_DriveCDIcon));
	ui->pushButtonPrev->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
	ui->pushButtonNext->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
	ui->pushButtonPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
	ui->pushButtonPause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
	ui->pushButtonStop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
	ui->pushButtonMute->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));

	ui->horizontalSliderVolume->setRange(0,100);


	//          Player init:
	m_player = new QMediaPlayer();
	m_player->setVolume(70);
	ui->labelVolume->setText(QString("Volume: ").append(QString::number(m_player->volume())));
	ui->horizontalSliderVolume->setValue(m_player->volume());

	connect(m_player, &QMediaPlayer::durationChanged, this,  &MainWindow::on_duration_changed);
	connect(m_player, &QMediaPlayer::positionChanged,this, &MainWindow::on_position_changed);
	connect(this->ui->horizontalSliderTime, &QSlider::sliderMoved, this ,&MainWindow::on_horizontalSliderTime_sliderMoved);
}

MainWindow::~MainWindow()
{
	delete m_player;
    delete ui;
}


void MainWindow::on_pushButtonAdd_clicked()
{
	QString file = QFileDialog::getOpenFileName
	   (
				this,
				"Open file",
				"C:\\Users\\user\\Documents\\MediaPlayerPD411\\ICO",
				"Audio files (*.mp3 *.flac);; MP-3 (*.mp3);; Flac (*.flac)"
	   );
	ui->labelFileName->setText(QString("File: ").append(file));
	this->m_player->setMedia(QUrl(file));
	this->m_player->play();
}



void MainWindow::on_pushButtonPlay_clicked()
{
	this->m_player->play();
}


void MainWindow::on_pushButtonPause_clicked()
{
	m_player->state() == QMediaPlayer::PausedState ?
				m_player->play() :
				this->m_player->pause();
}


void MainWindow::on_pushButtonStop_clicked()
{
	this->m_player->stop();
}


void MainWindow::on_pushButtonMute_clicked()
{
	m_player->setMuted(!m_player->isMuted());
	ui->pushButtonMute->setIcon(style()->standardIcon(m_player->isMuted()?QStyle::SP_MediaVolumeMuted:QStyle::SP_MediaVolume));
}


void MainWindow::on_horizontalSliderVolume_valueChanged(int value)
{
	m_player->setVolume(value);
	ui->labelVolume->setText(QString("Volume: ").append(QString::number(value)));
}

void MainWindow::on_duration_changed(qint64 duration)
{
	this->ui->horizontalSliderTime->setRange(0,duration);
	this->ui->labelDuration->setText(QTime::fromMSecsSinceStartOfDay(duration).toString("hh:mm:ss"));
}

void MainWindow::on_position_changed(qint64 position)
{
	this->ui->labelPosotion->setText(QString(QTime::fromMSecsSinceStartOfDay(position).toString("hh.mm.ss")));
	this->ui->horizontalSliderTime->setValue(position);
}





void MainWindow::on_horizontalSliderTime_sliderMoved(qint64 position)
{
	this->m_player->setPosition(position);
}

