#ifndef HTTPDOWNLOADER_H
#define HTTPDOWNLOADER_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QFile>

class httpDownloader : public QObject
{
    Q_OBJECT
public:
    explicit httpDownloader(QObject *parent = nullptr);
    virtual ~httpDownloader();
    bool download(const QString& strUrl);
    void cancel();
    void pause();

private:
    void startRequest(QUrl url);

signals:
    void sigErorr(const QString &errStr);
    void sigStartDownload();
    void sigProgress(int total, int received, QString sSpeed,int leftHour, int leftMin, int leftSec);
    void sigFinished();
    void sigCancel();

public slots:
private slots:
    void httpReadyRead();
    void httpFinished();
    void updateProgress(qint64 bytesReceived, qint64 bytesTotal);
    void httpError(QNetworkReply::NetworkError);

private:
    QNetworkAccessManager *m_pManager = nullptr;
    QNetworkReply *m_pReply = nullptr;
    QUrl m_url;
    QFile *m_pFile = nullptr;
    QTime time;
    bool m_isDownloading = false;
};

#endif // HTTPDOWNLOADER_H
