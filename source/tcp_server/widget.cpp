#include "widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    lb=new QLabel("send:");
    lb1=new QLabel("recv:");
    te=new QTextEdit;
    te1=new QTextEdit;

    QHBoxLayout *hbox=new QHBoxLayout;
    hbox->addWidget(lb);
    hbox->addWidget(lb1);
    QHBoxLayout *hbox1=new QHBoxLayout;
    hbox1->addWidget(te);
    hbox1->addWidget(te1);

    QVBoxLayout *vbox=new QVBoxLayout;
    vbox->addLayout(hbox);
    vbox->addLayout(hbox1);
    setLayout(vbox);

    tcpserver=new QTcpServer;
    tcpserver->listen(QHostAddress("127.0.0.1"),8888);
    connect(tcpserver,SIGNAL(newConnection()),this,SLOT(accepted()));

    dir=new QDir(".");
    if(!dir->cd("file"))
    {
        bool ok =dir->mkdir("./file");
        if(ok)
        {
            dir->cd("file");
        }
    }
    dir->setFilter(QDir::Files|QDir::Hidden|QDir::NoSymLinks);
    dir->setSorting(QDir::Size|QDir::Reversed);
}

Widget::~Widget()
{

}

void Widget::send_filename()
{
    dir->refresh();
    QStringList list=dir->entryList();
    int i=list.size()-1;
    QString str;
    tcpsocket->write("you can dowload:(send filename to dowload)\n");
    while(i>=0)
    {
        str=list[i];
        tcpsocket->write(str.toStdString().c_str(),str.length());
        tcpsocket->write(" ");
        i--;
    }
}
void Widget::get_file()
{
    QString buff={"./file/"};
    buff.append(buf);
    file=new QFile(buff);
    qDebug()<<buff;
    if(!file->open(QIODevice::ReadWrite))
    {
        tcpsocket->write("get false!");
        flag=0;
        return ;
    }
    data_total=file->size()+sizeof(qint64);
    QDataStream dts(&file_data1,QIODevice::ReadWrite);
    file_data.clear();
    data_get=data_total;
    dts<<qint64(0);
    dts.device()->seek(0);
    int sz=file_data1.size();
    sz=sz+file->size();
    dts<<(qint64)(sz);
    qDebug()<<file->size();
    data_get=sz-tcpsocket->write(file_data1);
    flag=4;
}
void Widget::up_file()
{
    if(fp==0)
    {
        QString buff={"./file/"};
        buff.append(buf);
        file=new QFile(buff);
        te1->append(buff);
        if(!file->open(QIODevice::ReadWrite|QIODevice::Truncate))
        {

            te1->append("dowload failed!");
            return ;
        }
        fp=1;
    }
    file->write(file_data);
    file_data.clear();
    file_data.resize(0);
}
void Widget::recv_data()
{
    if(flag==3)
    {
        QDataStream in(tcpsocket);
        qDebug()<<data_get;
        if(data_get==0)
        {
            in>>data_total;
            data_get=sizeof(qint64);
        }
        if(data_get<data_total)
        {
            data_get=data_get+tcpsocket->bytesAvailable();
            file_data=tcpsocket->readAll();
            up_file();
        }
        if(data_get==data_total)
        {
            te->append("dowload successed!");
            file->close();
            file->destroyed();
            fp=0;
            flag=0;
            data_get=0;
            data_total=0;
        }

    }
    else if(flag==2)
    {
        memset(buf,0,sizeof(buf));
        len=tcpsocket->bytesAvailable();
        tcpsocket->read(buf,len);
        tcpsocket->write("OK ");
        flag=3;
    }
    else if(flag==1)
    {
        memset(buf,0,sizeof(buf));
        len=tcpsocket->bytesAvailable();
        tcpsocket->read(buf,len);
        get_file();
        te1->append("getfile:");
        te1->append(buf);
    }
    else if(flag==0)
    {
        memset(buf,0,sizeof(buf));
        len=tcpsocket->bytesAvailable();
        tcpsocket->read(buf,len);
        if(strcmp(buf,"get_filename ")==0)
        {
            send_filename();
        }
        if(strcmp(buf,"get_file ")==0)
        {
            tcpsocket->write("get_file ");
            flag=1;
        }
        if(strcmp(buf,"up_file ")==0)
        {
            tcpsocket->write("up_file ");
            flag=2;
        }
    }

}
void Widget::gosend()
{
    if(flag==4)
    {
        if(data_get>0)
        {

            file_data=file->read(qMin(data_get,data_local));
            data_get=data_get-tcpsocket->write(file_data);
        }
        else
        {
            data_get=0;
            data_total=0;
            file->close();
            file->destroyed();
            flag=0;
            te->append("send successed!");
        }
    }
}
void Widget::accepted()
{
    te1->append("new connection");
    tcpsocket=tcpserver->nextPendingConnection();
    connect(tcpsocket,SIGNAL(readyRead()),this,SLOT(recv_data()));
    connect(tcpsocket,SIGNAL(bytesWritten(qint64)),this,SLOT(gosend()));

}
