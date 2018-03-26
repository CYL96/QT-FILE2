#include "widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    label =new QLabel(tr("dowload"));
    label1 =new QLabel(tr("upload"));
    label2 =new QLabel(tr("other"));
    lb=new QLabel("IP:");
    lb1=new QLabel("POST:");
    lb2=new QLabel(tr("Message:"));
    pb=new QPushButton(tr("dowload"));
    pb->setEnabled(false);
    pb1=new QPushButton(tr("connect"));
    pb2=new QPushButton(tr("get dowload filelist"));
    pb2->setEnabled(false);
    pb3=new QPushButton(tr("clear"));
    pb3->setEnabled(false);
    pb4=new QPushButton(tr("open file"));
    pb4->setEnabled(false);
    pb5=new QPushButton(tr("upload"));
    pb5->setEnabled(false);
    te1=new QTextEdit;
    le=new QLineEdit;
    le->setText("127.0.0.1");
    le1=new QLineEdit;
    le1->setText("8888");
    le2=new QLineEdit;
    le3=new QLineEdit;
    pgb=new QProgressBar;

    QHBoxLayout *hbox=new QHBoxLayout;
    hbox->addWidget(lb);
    hbox->addWidget(le);
    hbox->addWidget(lb1);
    hbox->addWidget(le1);
    hbox->addWidget(pb1);

    QHBoxLayout *hbox1=new QHBoxLayout;
    hbox1->addWidget(le2);
    hbox1->addWidget(pb);

    QHBoxLayout *hbox2=new QHBoxLayout;
    hbox2->addWidget(pb4);
    hbox2->addWidget(pb5);

    QVBoxLayout *vbox1=new QVBoxLayout;
    vbox1->addWidget(lb2);
    vbox1->addWidget(pgb);
    vbox1->addWidget(te1);

    QVBoxLayout *vbox2=new QVBoxLayout;
    vbox2->addWidget(label);
    vbox2->addLayout(hbox1);
    vbox2->addWidget(label1);
    vbox2->addWidget(le3);
    vbox2->addLayout(hbox2);
    vbox2->addWidget(label2);
    vbox2->addWidget(pb2);
    vbox2->addWidget(pb3);

    QHBoxLayout *hbox3=new QHBoxLayout;
    hbox3->addLayout(vbox1);
    hbox3->addLayout(vbox2);

    QVBoxLayout *vbox3=new QVBoxLayout;
    vbox3->addLayout(hbox);
    vbox3->addLayout(hbox3);

    setLayout(vbox3);
    tcp= new QTcpSocket;
    connect(pb1,SIGNAL(clicked(bool)),this,SLOT(connect_ser()));
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(send_data()));
    connect(pb2,SIGNAL(clicked(bool)),this,SLOT(show_file()));
    connect(pb3,SIGNAL(clicked(bool)),te1,SLOT(clear()));
    connect(pb4,SIGNAL(clicked(bool)),this,SLOT(open_file()));
    connect(pb5,SIGNAL(clicked(bool)),this,SLOT(up_file()));
    connect(le3,SIGNAL(textChanged(QString)),this,SLOT(up_lock()));

    dir=new QDir(".");
    if(!dir->cd("file"))
    {
        dir->mkdir("./file");
    }
}

Widget::~Widget()
{

}

void Widget::connect_ser()
{
    tcp->connectToHost(QHostAddress(le->text()), le1->text().toInt());
    connect(tcp, SIGNAL(connected()), this, SLOT(show_connected()));
    connect(tcp,SIGNAL(bytesWritten(qint64)),this,SLOT(gosend()));


}

void Widget::show_connected()
{

    te1->setText(tr("connected!"));
    pb->setEnabled(true);
    pb2->setEnabled(true);
    pb3->setEnabled(true);
    pb4->setEnabled(true);
    connect(tcp,SIGNAL(readyRead()),this,SLOT(recv_data()));
}
void Widget::send_data()
{
    tcp->write("get_file ");
    filename=le3->text();
}
void Widget::recv_data()
{
    if(flag==2)
    {
        char buf[100];
        memset(buf,0,sizeof(buf));
        len= tcp->bytesAvailable();
        tcp->read(buf,len);
        if(strcmp(buf,"OK ")==0)
        {
            file=new QFile(filename);
            te1->append(filename);
            if(!file->open(QIODevice::ReadWrite))
            {
                tcp->write("up false!");
                te1->append("upload failed!");
                return ;
            }
            data_total=file->size()+sizeof(qint64);
            pgb->clearFocus();
            pgb->setRange(0,data_total);
            pgb->setValue(0);
            upload_file();
            flag=3;
        }

    }
    else if(flag==1)
    {
        QDataStream in(tcp);
        qDebug()<<data_get;
        if(data_get==0)
        {
            in>>data_total;
            data_get=sizeof(qint64);
            pgb->clearFocus();
            pgb->setRange(0,data_total);
            pgb->setValue(0);
        }
        if(data_get<data_total)
        {
            data_get=data_get+tcp->bytesAvailable();
            file_data=tcp->readAll();
            get_file();
            pgb->setValue(data_get);
        }
        if(data_get==data_total)
        {
            te1->append(tr("dowload successed!"));
            file->close();
            file->destroyed();
            fp=0;
            flag=0;
            data_get=0;
            data_total=0;
        }
    }
    else if(flag==0)
    {
        char buf[100];
        memset(buf,0,sizeof(buf));
        len= tcp->bytesAvailable();
        tcp->read(buf,len);
        if(strcmp(buf,"get_file ")==0)
        {
            QString str =le2->text();
            filename=le2->text();
            tcp->write(str.toStdString().c_str(),str.length());
            flag=1;
        }
        else if(strcmp(buf,"up_file ")==0)
        {
            QStringList list =filename.split("/");
            QString str=list.last();
            tcp->write(str.toStdString().c_str(),str.length());
            flag=2;
        }
        else
            te1->append(buf);
    }

}

void Widget::show_file()
{
    tcp->write("get_filename ");
}

void Widget::open_file()
{
    filename=QFileDialog::getOpenFileName(this,tr("open file"),"./file/");
    if(filename.isEmpty())
    {
        return;
    }
    le3->setText(filename);
}

void Widget::get_file()
{
    if(fp==0)
    {
        QString buff={"./file/"};
        buff.append(filename);
        file=new QFile(buff);
        te1->append(buff);
        if(!file->open(QIODevice::ReadWrite|QIODevice::Truncate))
        {
            te1->append(tr("dowload failed!"));
            return ;
        }
        fp=1;
    }
    file->write(file_data);
    file_data.resize(0);

}

void Widget::upload_file()
{
    QDataStream dts(&file_data1,QIODevice::ReadWrite);
    file_data.clear();
    data_get=data_total;
    dts<<qint64(0);
    dts.device()->seek(0);
    int sz=file_data1.size();
    sz=sz+file->size();
    dts<<(qint64)(sz);
    qDebug()<<file->size();
    data_get=sz-tcp->write(file_data1);

}

void Widget::up_file()
{
    tcp->write("up_file ");
}

void Widget::gosend()
{
    if(flag==3)
    {
        if(data_get>0)
        {
            file_data=file->read(qMin(data_get,data_local));
            data_get=data_get-tcp->write(file_data);
            pgb->setValue(data_total-data_get);
        }
        else
        {

            data_get=0;
            data_total=0;
            file->close();
            file->destroyed();
            flag=0;
            te1->append(tr("upload successed!"));
        }
    }
}

void Widget::up_lock()
{
    if(le3->text()!="")
    {
        pb5->setEnabled(true);
    }
    else
    {
        pb5->setEnabled(false);
    }
}
