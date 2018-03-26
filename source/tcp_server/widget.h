#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTcpSocket>
#include <QTcpServer>

#include <QFile>
#include <QTextStream>
#include <QDataStream>
#include <QFileInfo>
#include <QDateTime>
#include <QDir>
#include <QFileInfoList>
class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();
private:
    QLabel *lb,*lb1;
    QPushButton *pb;
    QTextEdit *te,*te1;
    QTcpSocket *tcpsocket;
    QTcpServer *tcpserver;
    QHostAddress host;
    quint16 port;
    int flag=0;
    int fp=0;
    QDir *dir;
    char buf[1024];
    int len;
    QByteArray file_data;
    QByteArray file_data1;
    QFile *file;


    qint64 data_get=0;
    qint64 data_total=0;
    qint64 data_local=1024;


private:
    void send_filename();
    void get_file();
    void up_file();
private slots:
    void recv_data();
    void accepted();
    void gosend();
};

#endif // WIDGET_H
