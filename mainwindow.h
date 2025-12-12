#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

	void initPlaylist();

	void loadFileToPlaylist(const QString& filename);
    void savePlaylist(const QString& filename);
    void loadPlaylist(const QString& filename);
    QVector<QString> loadPlaylistToArray(const QString& filename);


private slots:
	void on_pushButtonAdd_clicked();

	void on_pushButtonPlay_clicked();

	void on_pushButtonPause_clicked();

	void on_pushButtonStop_clicked();

	void on_pushButtonMute_clicked();

	void on_horizontalSliderVolume_valueChanged(int value);

	void on_duration_changed(qint64 duration);

	void on_position_changed(qint64 position);

	void on_horizontalSliderTime_sliderMoved(int position);

	void on_pushButtonShuffle_clicked();

	void on_pushButtonLoop_clicked();

	void on_pushButtonDel_clicked();

	void on_pushButtonClr_clicked();

    void on_pushButtonDir_clicked();

private:
    void traverseDirectories(const QString& dirname);

    Ui::MainWindow *ui;
	QMediaPlayer* m_player;
	QMediaPlaylist* m_playlist;
	QStandardItemModel* m_playlist_model;

	bool shuffle;
	bool loop;

};
#endif // MAINWINDOW_H
