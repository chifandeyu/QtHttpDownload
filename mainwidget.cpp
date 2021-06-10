#include "mainwidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QDebug>
#include "httpdownloader.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
{
    this->setWindowTitle(tr("File download"));
    this->resize(450, 250);

    createWidget();
    createDownloader();

    //下载按钮信号
    connect(btnDownLoad, &QPushButton::clicked, this, &MainWidget::on_btnDownLoad_Clicked);

    //取消按钮信号
    connect(btnCancel, &QPushButton::clicked, this, &MainWidget::on_btnCancel_Clicked);
}

MainWidget::~MainWidget()
{
    if(m_pDownloader){
       m_pDownloader->deleteLater();
       m_pDownloader = nullptr;
    }
}

void MainWidget::createWidget()
{
    labAddress = new QLabel(tr("address"));
    lineEditAddr = new QLineEdit(tr("http://www.wxqt.top"));

    QFont font = QFont("sim", 16);
    textEdit = new QTextEdit();
    textEdit->setReadOnly(true);
    textEdit->setFont(font);

    btnDownLoad = new QPushButton(tr("Start"));
    btnCancel = new  QPushButton(tr("Cancel"));
    btnCancel->hide();

    progress = new QProgressBar();
    progress->hide();

    QHBoxLayout *laytop = new QHBoxLayout();
    laytop->addWidget(labAddress);
    laytop->addWidget(lineEditAddr);
    laytop->addWidget(btnDownLoad);

    QHBoxLayout *layBottom = new QHBoxLayout();
    layBottom->addWidget(progress);
    layBottom->addWidget(btnCancel);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(laytop);
    mainLayout->addWidget(textEdit);
    mainLayout->addLayout(layBottom);
}

void MainWidget::createDownloader()
{
    m_pDownloader = new httpDownloader();
    connect(m_pDownloader, &httpDownloader::sigErorr, this, &MainWidget::slotErorr);
    connect(m_pDownloader, &httpDownloader::sigStartDownload, this, &MainWidget::slotStartDownload);
    connect(m_pDownloader, &httpDownloader::sigProgress, this, &MainWidget::slotProgress);
    connect(m_pDownloader, &httpDownloader::sigFinished, this, &MainWidget::slotFinished);
}

void MainWidget::on_btnDownLoad_Clicked()
{
    //从地址栏获取URL
    QString url = lineEditAddr->text();
    m_pDownloader->download(url);
}

void MainWidget::on_btnCancel_Clicked()
{
    m_pDownloader->cancel();
}

void MainWidget::slotFinished()
{
    progress->hide();
    btnCancel->hide();
    textEdit->append(tr("download finished"));
}

void MainWidget::slotProgress(int total, int received, QString sSpeed,int leftHour, int leftMin, int leftSec)
{
    //设置进度条的最大值 和 进度值
    progress->setValue(received);
    progress->setMaximum(total);

    textEdit->setText(QString(tr("file size: %1MB    download: %2MB speed: %3MB/s time left: %4h %5m %6s"))
                      .arg(total)
                      .arg(received)
                      .arg(sSpeed)
                      .arg(leftHour)
                      .arg(leftMin)
                      .arg(leftSec));
}

void MainWidget::slotErorr(const QString &errStr)
{
    textEdit->setText(errStr);
}

void MainWidget::slotStartDownload()
{
    textEdit->clear();
    progress->setValue(0);
    progress->show();
    btnCancel->show();
}
