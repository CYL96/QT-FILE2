#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QTextEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QDebug>
#include <QHostAddress>
#include <QProgressBar>

#include <QFile>
#include <QTextStream>
#include <QDataStream>
#include <QFileInfo>
#include <QDateTime>
#include <QDir>
#include <QFileInfoList>
#include <QByteArray>
#include <QFileDialog>

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void connect_ser();
    void show_connected();
    void send_data();
    void recv_data();
    void show_file();
    void open_file();
    void up_file();
    void gosend();
    void up_lock();
private:
    void get_file();
    void get_filename();
    void upload_file();
private :
    QLabel *label,*label1,*label2,*lb,*lb1,*lb2;
    QTcpSocket *tcp;
    QTextEdit *te,*te1;
    QProgressBar *pgb;
    QPushButton *pb,*pb1,*pb2,*pb3,*pb4,*pb5;
    QLineEdit *le,*le1,*le2,*le3;
    int len;
    QByteArray file_data;
    QByteArray file_data1;
    int flag=0;
    int fp=0;
    QString filename;
    QFile *file;
    QDir *dir;

    qint64 data_total=0;
    qint64 data_get=0;
    qint64 data_local=1024;
};

#endif // WIDGET_H
