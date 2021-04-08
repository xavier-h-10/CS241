#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QThread>
#include <QtDebug>
#include <QButtonGroup>
#include "Worker.h"

#include <QtWebEngineCore>
//#include <QWebEngineView>
//#include <QQuickWidget>
#include <QtCharts>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

   // Q_PROPERTY(double location READ getLocation WRITE setLocation NOTIFY locationChanged)


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
  //  QWebEngineView *view;

    double lat1=0,lng1=0;
    double lat2=0,lng2=0;

signals:
    void sig_work(const QString &dirstring,const QString &fileName_0,const int &total_day);

    void sig_clear();

    void need_data();

    void need_order();

   // void need_sorting();

    void send_number(int num);

public slots:
    void slot_finish(const QString &fileName);

    void slot_fail(const QString &fileName);

    void slot_progress(const double &p);

    void receive_data(QVector< QVector<double> > r_data);

    void receive_order(QVector< QString > r_order);


private slots:
    void on_pushButton_clicked();

    void on_fromTime_dateTimeChanged(const QDateTime &dateTime);

    void on_toTime_dateTimeChanged(const QDateTime &dateTime);

    void on_fromGrid_currentIndexChanged(int index);

    void on_toGrid_currentIndexChanged(int index);

    void on_pushButton_2_clicked();

    void on_query_demand_clicked();

    void on_radio_1_clicked();

    void on_radio_2_clicked();

    void on_radio_3_clicked();

    void on_fromDate_userDateChanged(const QDate &date);

    void on_toDate_userDateChanged(const QDate &date);

    void on_query_similar_clicked();

    void on_query_fromTime_dateTimeChanged(const QDateTime &dateTime);

    void on_query_toTime_dateTimeChanged(const QDateTime &dateTime);

    void on_clear_similar_clicked();

    void on_clear_clicked();

    void on_query_prediction_clicked();

    void on_clear_prediction_clicked();

    void on_clear_prediction_2_clicked();

    void on_query_prediction_2_clicked();

private:
    Ui::MainWindow *ui;

    void loadMap(const QString &dirstring,const QString &fileName_0);

    QVector< QVector<double> > mapData;

    double map[105][5];

    QVector< QVector<double> > data;

   // QVector< QString > Order;

   /* struct dataset
    {
        double data;
        double data_1;
        QString order;
    };
    static bool cmp(dataset a, dataset b) {return (a.data<b.data) || (a.data==b.data && a.data_1<b.data_1);}

    QVector <dataset> order;*/

    QVector < QString > order;

    int all, tot_file;

    Worker * m_worker;

    QThread *m_thread;

    QString dir_string,fileName;

    QButtonGroup *time_button,*draw_button,*query_button;

    bool loaded=false;

//    QQuickWidget *qw;

    bool is_fee=false,is_time=false,is_dest=false,is_order=false;

    QChart * chart;

    QLineSeries *series;

    QSplineSeries *series_1;

   //QBarSeries *series_2;
    QPieSeries *series_2;

    QStandardItemModel* model;

    void set_header();

    QVector< QPair<double,int> > v;

    void draw_table();

    QDateTimeAxis *axisX;

    QValueAxis *axisY;

};
#endif // MAINWINDOW_H
