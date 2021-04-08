#ifndef WORKER_H
#define WORKER_H

#include <QThread>
#include <QObject>
#include <QMetaType>
#include <QVariant>
#include <QString>
#include <QVector>

class Worker:public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent=0);

    ~Worker();

    QVector< QVector<double> > data;

    QVector< QString > order;

     struct dataset
     {
         double data;
         double data_1;
         QString order;
     };

     static bool cmp(dataset a, dataset b) {return (a.data<b.data) || (a.data==b.data && a.data_1<b.data_1);}

     QVector <dataset> Order;

signals:
    void sig_finish(const QString &fileName);

    void sig_fail(const QString &fileName);

    void sig_progress(const double &p);

    void send_data(QVector< QVector<double> > data);

    void send_order(QVector< QString > order);

public slots:
    void slot_work(const QString &dirstring,const QString &fileName_0,const int &total_day);

    void slot_clear();

    void give_data();

    void give_order();

    void sorting();

    void receive_number(int num);

private:
    QMutex mutex;

    int all=0;

    int total_file=0;

};

#endif
