#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QtNetwork/QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QFile>
#include <QTime>

class httpDownloader;

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

private:
    void createWidget();
    void createDownloader();
public slots:
    void slotProgress(int total, int received, QString sSpeed,int leftHour, int leftMin, int leftSec);
    void slotErorr(const QString &errStr);
    void slotStartDownload();
    void slotFinished();

private slots:
    void on_btnDownLoad_Clicked();
    void on_btnCancel_Clicked();

private:
    QLabel *labAddress;
    QLineEdit *lineEditAddr;
    QTextEdit *textEdit;
    QProgressBar *progress;
    QPushButton *btnDownLoad;
    QPushButton *btnCancel;
    httpDownloader *m_pDownloader = nullptr;
};

#endif // MAINWIDGET_H
