#include "Worker.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QtDebug>
#include <QObject>

Worker::Worker(QObject *parent):QObject(parent)
{
    qDebug()<<"worker()";
}

Worker::~Worker()
{
    qDebug()<<"~worker()";
}

void Worker::slot_work(const QString &dirstring,const QString &fileName_0,const int &total_day)
{
    qDebug()<<"working... thread id="<<QThread::currentThreadId();
   /* QMutexLocker locker(&mutex);
    QString fileName=MainWindow::dir_string+"/"+MainWindow::fileName;
    qDebug()<<fileName;*/

    QString fileName=dirstring+"/"+fileName_0;
    qDebug()<<fileName;
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) emit sig_fail(fileName);
    QTextStream stream(&file);
    QStringList headers = stream.readLine().split(',');
    int now=0;
    double progress=0;
    total_file--;
    QVector<double> q;
    while(!stream.atEnd())
    {
        QString tmpLine=stream.readLine();
        if(!tmpLine.isEmpty())
        {
            QStringList p=tmpLine.split(',');
            //qDebug()<<now;
            q.clear();
            for(int i=0;i<7;i++)
            {
                q.append(p.value(i+1).toDouble());
            }
            order.append(p.value(0));
            data.append(q);
            now++;
            progress=std::min(95.0,(double)(now+all)*100*15/2943723/total_day);
            if(now%10000==0) emit sig_progress(progress);
        }
    }
    all+=now;
    qDebug()<<"all="<<all;
    progress=std::min(95.0,(double)all*100/2943723/total_day*15);
    emit sig_progress(progress);
    //emit sig_progress((double)all*100/2943723/total_day*15);
  //  if(all>=2943723*total_day/15) emit sig_progress(100);
    file.close();
    emit sig_finish(fileName);
    qDebug()<<"data="<<data.size();

    if(total_file==0) sorting();

}

void Worker::sorting()
{
    Order.clear();
    dataset p;
    QVector < QString > str;
    for(int i=0;i<data.size();i++)
    {
        p.order=order.at(i);
        p.data=data.at(i).at(0);
        p.data_1=data.at(i).at(1);
        Order.append(p);
    }

    std::sort(Order.begin(),Order.end(),cmp);
    std::sort(data.begin(),data.end());
    for(int i=0;i<data.size();i++)
    {
        str.append(Order.at(i).order);
    }
    order=str;
}

void Worker::slot_clear()
{
    qDebug()<<"slot clear...";
    all=0;
    data.clear();
    order.clear();
}

void Worker::give_data()
{
    emit send_data(data);
}

void Worker::give_order()
{
    emit send_order(order);
}

void Worker::receive_number(int num)
{
    total_file=num;
}
