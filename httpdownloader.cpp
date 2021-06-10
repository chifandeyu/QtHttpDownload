#include "httpdownloader.h"
#include <QFileInfo>

httpDownloader::httpDownloader(QObject *parent) : QObject(parent)
{
    m_pManager = new QNetworkAccessManager(this);
}

httpDownloader::~httpDownloader()
{
    cancel();
}

bool httpDownloader::download(const QString &strUrl)
{
    m_url = QUrl(strUrl);
    //将URL路径存入QFileInfo类
    QFileInfo info(m_url.path());

    //从info中得到文件名
    QString fileName(info.fileName());

    //如果没有获取到info
    if(fileName.isEmpty()){
        //textEdit->setText(tr("<h2> 下载失败 请检查下载地址!!! </h2>"));
        emit sigErorr("download failed, please check adress!!!");
        qDebug() << "==== download failed, please check adress!!!"<<strUrl;
        return false;
    }
    else{
        //建立文件
        m_pFile = new QFile(fileName);

        //如果文件没有打开的情况
        if(!m_pFile->open((QIODevice::WriteOnly))){
            delete m_pFile;
            m_pFile = nullptr;
            return false;
        }

        //执行网络请求函数
        startRequest(m_url);

        //开始下载
        emit sigStartDownload();
        qDebug() << "==== start download"<<strUrl;
    }
    return true;
}

void httpDownloader::cancel()
{
    if(m_pReply){
        m_pReply->deleteLater();
        m_pReply = nullptr;
    }
    if(m_pFile){
        m_pFile->close();
        m_pFile = nullptr;
    }
    emit sigCancel();
    qDebug() << "==== httpcancel";
}

void httpDownloader::pause()
{

}

void httpDownloader::startRequest(QUrl url)
{
    m_isDownloading = true;
    //get发送一个网络请求 返回网络应答
    m_pReply = m_pManager->get(QNetworkRequest(url));

    //每当有新的数据要读取时 发射信号
    connect(m_pReply, &QNetworkReply::readyRead, this, &httpDownloader::httpReadyRead);

    //每当有下载进度更新时 发射信号 更新进度条和文本窗
    connect(m_pReply, &QNetworkReply::downloadProgress, this, &httpDownloader::updateProgress);

    //应答处理结束时会发射finished信号
    connect(m_pReply, &QNetworkReply::finished, this, &httpDownloader::httpFinished);

    connect(m_pReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(httpError(QNetworkReply::NetworkError)));
    //时间计时开始
    time.start();
}

void httpDownloader::httpReadyRead()
{
    if(m_pFile){
        m_pFile->write(m_pReply->readAll());
    }
}

void httpDownloader::httpFinished()
{
    qDebug() << "==== httpFinished";
    if(m_pFile){
        m_pFile->close();
        delete m_pFile;
        m_pFile = nullptr;
    }

    m_pReply->deleteLater();
    m_pReply = nullptr;
    emit sigFinished();
    m_isDownloading = false;
}

void httpDownloader::updateProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    //将qint64的数据转为MB 并且转为String
    QString strReceived = QString::number(bytesReceived / (1024 * 1024));
    QString strTotal = QString::number(bytesTotal / (1024 * 1024));

    //QString 转为INT 类型
    int received = strReceived.toInt();
    int total = strTotal.toInt();

    //已用时间
    int useTime = time.elapsed();

    //下载速度 接收到的值 / 已用时间
    double speed = bytesReceived / useTime;

    //剩余时间
    double leftTime = bytesTotal / speed / 1000 - useTime / 1000;

    int intLeftTime = (int) leftTime;
    int leftHour = intLeftTime / 3600;
    int leftMin = intLeftTime % 3600 / 60;
    int leftSec = intLeftTime % 60;

    //格式化文本窗
    //文件总大小  bytesTotal / (1024 * 1024) MB
    //已经下载    bytesReceived / (1024 * 1024 MB
    //下载速度    已经下载 / 已经用时  将毫秒改为秒 *1000  将bytes改为MB (1024 * 1024)
    //总时间      总大小 / 下载速度  /秒
    //剩余时间    总时间 - 已用时间
    QString sSpeed = QString("%1").arg(speed * 1000 / (1024 * 1024), 0, 'f', 2);
    emit sigProgress(total, received, sSpeed, leftHour, leftMin, leftSec);
//    textEdit->setText(QString("file size: %1MB 已下载: %2MB 下载速度: %3MB/s 剩余时间: %4小时 %5分钟 %6秒 ")
//                      .arg(total)
//                      .arg(received)
//                      .arg()
//                      .arg(leftHour)
//                      .arg(leftMin)
//                      .arg(leftSec));
    qDebug() << QString("**** file size: %1MB    download: %2MB speed: %3MB/s time left: %4h %5m %6s")
                .arg(total)
                .arg(received)
                .arg(sSpeed)
                .arg(leftHour)
                .arg(leftMin)
                .arg(leftSec);
}

void httpDownloader::httpError(QNetworkReply::NetworkError error)
{
    m_isDownloading = false;
    QString errStr = m_pReply->errorString();
    emit sigErorr(errStr);
    qDebug() << "==== httpError"<<errStr;
}
