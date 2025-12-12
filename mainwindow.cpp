#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStyle>
#include <QFileDialog>
#include <QTime>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

	//          Buttons style:
	ui->pushButtonAdd->setIcon(style()->standardIcon(QStyle::SP_DriveCDIcon));
	ui->pushButtonPrev->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
	ui->pushButtonNext->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
	ui->pushButtonPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
	ui->pushButtonPause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
	ui->pushButtonStop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
	ui->pushButtonMute->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
	//	ui->pushButtonShuffle->setIcon(style()->standardIcon(QStyle::));
	//ui->pushButtonLoop->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));

	ui->horizontalSliderVolume->setRange(0, 100);

	//          Player init:
	m_player = new QMediaPlayer();
	m_player->setVolume(70);
	ui->labelVolume->setText(QString("Volume: ").append(QString::number(m_player->volume())));
	ui->horizontalSliderVolume->setValue(m_player->volume());

	connect(m_player, &QMediaPlayer::durationChanged, this, &MainWindow::on_duration_changed);
	connect(m_player, &QMediaPlayer::positionChanged, this, &MainWindow::on_position_changed);
	connect(this->ui->horizontalSliderTime, &QSlider::sliderMoved, this, &MainWindow::on_horizontalSliderTime_sliderMoved);

	//			Playlist init:
	m_playlist_model = new QStandardItemModel(this);
	initPlaylist();
//	this->ui->tableViewPlaylist->setModel(m_playlist_model);
//	m_playlist_model->setHorizontalHeaderLabels(QStringList() << "Audio track" << "File path" << "Duration");
//	this->ui->tableViewPlaylist->setEditTriggers(QAbstractItemView::NoEditTriggers);
//	this->ui->tableViewPlaylist->setSelectionBehavior(QAbstractItemView::SelectRows);

//	this->ui->tableViewPlaylist->hideColumn(1);
//	int duration_width = 64;
//	this->ui->tableViewPlaylist->setColumnWidth(2, duration_width);
//	this->ui->tableViewPlaylist->setColumnWidth(0, this->ui->tableViewPlaylist->width()-duration_width*1.7);

	m_playlist = new QMediaPlaylist(m_player);
	m_player->setPlaylist(m_playlist);

	connect(this->ui->pushButtonPrev, &QPushButton::clicked, this->m_playlist, &QMediaPlaylist::previous);
	connect(this->ui->pushButtonNext, &QPushButton::clicked, this->m_playlist, &QMediaPlaylist::next);
	connect(this->m_playlist, &QMediaPlaylist::currentIndexChanged, this->ui->tableViewPlaylist, &QTableView::selectRow);
	connect(this->ui->tableViewPlaylist, &QTableView::doubleClicked,
			[this](const QModelIndex& index){m_playlist->setCurrentIndex(index.row()); this->m_player->play();}
			);
	connect(this->m_player, &QMediaPlayer::currentMediaChanged,
			[this](const QMediaContent& media)
	{
		this->ui->labelFilename->setText(media.canonicalUrl().toString());
		this->setWindowTitle(this->ui->labelFilename->text().split('/').last());
	}
			);

	shuffle = false;
	loop = false;

	//connect(this->ui->pushButtonClr, &QPushButton::clicked, this->m_playlist, &QMediaPlaylist::clear);
	//connect(this->ui->pushButtonClr, &QPushButton::clicked, this->m_playlist_model, &QStandardItemModel::clear);
    loadPlaylist("playlist.m3u");
}

MainWindow::~MainWindow()
{
    savePlaylist("playlist.m3u");
    delete m_playlist_model;
	delete m_playlist;
	delete m_player;
	delete ui;
}

void MainWindow::initPlaylist()
{
	this->ui->tableViewPlaylist->setModel(m_playlist_model);
	m_playlist_model->setHorizontalHeaderLabels(QStringList() << "Audio track" << "File path" << "Duration");
	this->ui->tableViewPlaylist->setEditTriggers(QAbstractItemView::NoEditTriggers);
	this->ui->tableViewPlaylist->setSelectionBehavior(QAbstractItemView::SelectRows);

	this->ui->tableViewPlaylist->hideColumn(1);
	int duration_width = 64;
	this->ui->tableViewPlaylist->setColumnWidth(2, duration_width);
	this->ui->tableViewPlaylist->setColumnWidth(0, this->ui->tableViewPlaylist->width()-duration_width*1.7);
}

bool isAudioExtension(const QString& filename)
{
    if(filename.split('.').last()=="mp3")return true;
    if(filename.split('.').last()=="flac")return true;
    if(filename.split('.').last()=="lfacc")return true;
    return false;
}
void MainWindow::loadFileToPlaylist(const QString &filename)
{
    if(!isAudioExtension(filename))return;
	m_playlist->addMedia(QUrl(filename));
	QList<QStandardItem*> items;
	items.append(new QStandardItem(QDir(filename).dirName()));
	items.append(new QStandardItem(filename));
//	QMediaPlayer player;
//	m_duration_player.setMedia(QUrl(filename));
//	m_duration_player.play();
//	items.append(new QStandardItem(QTime::fromMSecsSinceStartOfDay(player.duration()).toString("mm:ss")));
//	items.append(new QStandardItem(QString::number(m_duration_player.duration())));
//	m_duration_player.pause();
	m_playlist_model->appendRow(items);
    //https://stackoverflow.com/questions/43156906/qmediaplayer-duration-returns-0-always
}

void MainWindow::savePlaylist(const QString &filename)
{
    QString format = filename.split('.').last();
    QUrl url = QUrl::fromLocalFile(filename);
    bool result = m_playlist->save(url, format.toStdString().c_str());
}

void MainWindow::loadPlaylist(const QString &filename)
{
    //QVector<QString> lines =
    m_playlist->load(QUrl::fromLocalFile(filename));
    int n = m_playlist->mediaCount();
    for(int i=0;i < n;i++)
    {
        QList<QStandardItem*> item;
        item.append(new QStandardItem(m_playlist->media(i).canonicalUrl().fileName()));
        item.append(new QStandardItem(m_playlist->media(i).canonicalUrl().path()));
        m_playlist_model->appendRow(item);
    }
}

QVector<QString> MainWindow::loadPlaylistToArray(const QString &filename)
{
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    QList<QString> lines;
    while(!file.atEnd())
    {
        QByteArray line = file.readLine();
        lines.append(line);
    }
    file.close();
    return lines.toVector();
}

void MainWindow::on_pushButtonAdd_clicked()
{
	/*QString file = QFileDialog::getOpenFileName
	(
		this,
		"Open file",
		"D:\\Users\\User\\Music\\",
		"Audio files (*.mp3 *.flac);; MP-3 (*.mp3);; Flac (*.flac)"
	);
	ui->labelFilename->setText(QString("File: ").append(file));
	this->m_player->setMedia(QUrl(file));
	this->m_player->play();*/

	QStringList files = QFileDialog::getOpenFileNames
			(
				this,
				"Open file",
				"D:\\Users\\User\\Music\\",
				"Audio files (*.mp3 *.flac *.flacc);; mp3 (*.mp3);; Flac (*.flac *.flacc)"
			);
	//https://legacy.cplusplus.com/doc/tutorial/control/#:~:text=equal%20to%2050.-,Range%2Dbased%20for%20loop,-The%20for%2Dloop
	//https://www.youtube.com/watch?v=11Yqy4YLppw&list=PLeqyOOqxeiINwoETYVNufASRa3i53K2Gb&index=21
	for(QString file:files)
	{
		loadFileToPlaylist(file);
	}
}


void MainWindow::on_pushButtonPlay_clicked()
{
	this->m_player->play();
}


void MainWindow::on_pushButtonPause_clicked()
{
	m_player->state() == QMediaPlayer::State::PausedState ? m_player->play() : this->m_player->pause();
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
	this->ui->horizontalSliderTime->setRange(0, duration);
	this->ui->labelDuration->setText(QTime::fromMSecsSinceStartOfDay(duration).toString("hh:mm:ss"));
}

void MainWindow::on_position_changed(qint64 position)
{
	this->ui->labelPosition->setText(QString(QTime::fromMSecsSinceStartOfDay(position).toString("hh:mm:ss")));
	this->ui->horizontalSliderTime->setValue(position);
}
void MainWindow::on_horizontalSliderTime_sliderMoved(int position)
{
	this->m_player->setPosition(position);
}


void MainWindow::on_pushButtonShuffle_clicked()
{
	shuffle = !shuffle;
	this->ui->pushButtonShuffle->setCheckable(true);
	this->m_playlist->setPlaybackMode(shuffle ? QMediaPlaylist::PlaybackMode::Random : QMediaPlaylist::PlaybackMode::Sequential);
	this->ui->pushButtonShuffle->setChecked(shuffle);
}


void MainWindow::on_pushButtonLoop_clicked()
{
	loop = !loop;
	this->ui->pushButtonLoop->setCheckable(true);
	this->m_playlist->setPlaybackMode(loop ? QMediaPlaylist::PlaybackMode::Loop : QMediaPlaylist::PlaybackMode::Sequential);
	this->ui->pushButtonLoop->setChecked(loop);
}


void MainWindow::on_pushButtonDel_clicked()
{
//	QItemSelectionModel* selection = ui->tableViewPlaylist->selectionModel();
//	QModelIndexList indexes = selection->selectedRows();
//	for(QModelIndex i : indexes)
//	{
//		m_playlist_model->removeRow(i.row());
//		m_playlist->removeMedia(i.row());
//	}
	QItemSelectionModel* selection = nullptr;
	do
	{
		selection = ui->tableViewPlaylist->selectionModel();
//		if(selection == nullptr)break;
		QModelIndexList indexes = selection->selectedRows();
		if(selection->selectedRows().count()>0)
		{
			m_playlist_model->removeRow(indexes.first().row());
			m_playlist->removeMedia(indexes.first().row());
		}
	}while(selection->selectedRows().count());
}


void MainWindow::on_pushButtonClr_clicked()
{
	m_playlist->clear();
	m_playlist_model->clear();
	initPlaylist();
}

void MainWindow::traverseDirectories(const QString& dirname)
{
    QDir dir = QDir(dirname);
    for(QFileInfo file : dir.entryInfoList())
    {
        QString filename = file.fileName();
        if(file.fileName() == "." || file.fileName() == "..")continue;
        //QString filename = dirname.append("/").append(file.fileName());
        if(file.isFile())
        {
            QString filename = dir.filePath(file.fileName());
            loadFileToPlaylist(filename);
        }
        if(file.isDir())
        {
            traverseDirectories(dir.filePath(file.fileName()));
        }

    }
}

void MainWindow::on_pushButtonDir_clicked()
{
    QString dirname = QFileDialog::getExistingDirectory
            (
                this,
                "Add directory",
                "C:\\Users\\User\\Music\\",
                QFileDialog::ShowDirsOnly
                );

    traverseDirectories(dirname);
}

