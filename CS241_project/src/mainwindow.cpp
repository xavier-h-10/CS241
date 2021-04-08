#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtWidgets>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QPieSeries>
#include <Qtcharts/QSplineSeries>
#include <QtCharts/QPieSlice>
#include <QtDebug>
#include <QVector>
#include <QMetaType>
#include "Worker.h"

//#include <QtWebEngineCore>
//#include <QWebEngineView>
//#include <QQuickWidget>

#include <QUrl>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("Final Project"));

    for(int i=0;i<60;i++)
    {
        ui->fromGrid->addItem(QString::number(i));
        ui->toGrid->addItem(QString::number(i));
    }

    ui->fromGrid->setCurrentIndex(0);
    ui->toGrid->setCurrentIndex(59);
    ui->progressBar->setTextVisible(true);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(0);
    ui->progressBar->setFormat("%p");

    time_button=new QButtonGroup;
    time_button->addButton(ui->radio_1);
    time_button->addButton(ui->radio_2);
    time_button->addButton(ui->radio_3);

    draw_button=new QButtonGroup;
    draw_button->addButton(ui->radio_4);
    draw_button->addButton(ui->radio_5);
    draw_button->addButton(ui->radio_6);
    draw_button->addButton(ui->radio_7);

    query_button=new QButtonGroup;
    query_button->addButton(ui->radio_8);
    query_button->addButton(ui->radio_9);
    query_button->addButton(ui->radio_10);
    query_button->addButton(ui->radio_11);

    qDebug()<<"starting MainWindow... thread id="<<QThread::currentThreadId();
    m_thread=new QThread();
    m_worker=new Worker();
    m_worker->moveToThread(m_thread);
   // connect(m_thread,&QThread::finished,m_worker,&QThread::deleteLater);
   // connect(m_thread,SIGNAL(started()),m_worker,SLOT(slot_work()));
    connect(this,SIGNAL(sig_work(QString,QString,int)),m_worker,SLOT(slot_work(QString,QString,int)));
    connect(this,SIGNAL(sig_clear()),m_worker,SLOT(slot_clear()));
    connect(this,SIGNAL(need_data()),m_worker,SLOT(give_data()));
    connect(this,SIGNAL(need_order()),m_worker,SLOT(give_order()));
    connect(this,SIGNAL(send_number(int)),m_worker,SLOT(receive_number(int)));

    connect(m_worker,SIGNAL(sig_finish(QString)),this,SLOT(slot_finish(QString)));
    connect(m_worker,SIGNAL(sig_fail(QString)),this,SLOT(slot_fail(QString)));
    connect(m_worker,SIGNAL(sig_progress(double)),this,SLOT(slot_progress(double)));

    connect(m_worker,SIGNAL(send_data(QVector< QVector<double> >)),this,SLOT(receive_data(QVector< QVector<double> >)));
    connect(m_worker,SIGNAL(send_order(QVector< QString >)),this,SLOT(receive_order(QVector< QString >)));

    connect(ui->spinBox,SIGNAL(valueChanged(int)),ui->slider,SLOT(setValue(int)));
    connect(ui->slider,SIGNAL(valueChanged(int)),ui->spinBox,SLOT(setValue(int)));

    ui->spinBox->setDisabled(true);
    ui->slider->setDisabled(true);

    ui->quickWidget->show();

    chart=new QChart();
    series = new QLineSeries();
    series_1= new QSplineSeries();
    series_2= new QPieSeries();
    chart->addSeries(series);
    chart->addSeries(series_1);
    chart->addSeries(series_2);
    chart->legend()->hide();
    chart->setAnimationOptions(QChart::AllAnimations);
    chart->setLocalizeNumbers(true);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);

    // ui->graphicsView->setRubberBand(QChartView::HorizontalRubberBand);
   // ui->graphicsView->setRubberBand(QChartView::VerticalRubberBand);
    ui->graphicsView->setChart(chart);
    ui->radio_2->click();

    QRegExp rx("^-?(180|1?[0-7]?\\d(\\.\\d{1,8})?)$");
    QRegExpValidator *pReg = new QRegExpValidator(rx, this);
    ui->lng_1->setValidator(pReg);
    ui->lng_2->setValidator(pReg);
    ui->lat_1->setValidator(pReg);
    ui->lat_2->setValidator(pReg);

    ui->tableView->setShowGrid(true);
    ui->tableView->setSortingEnabled(true);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    model=new QStandardItemModel();
    ui->tableView->setModel(model);
    ui->tableView->show();

    ui->pre_time->setReadOnly(true);
    ui->pre_lng4->setReadOnly(true);
    ui->pre_lat4->setReadOnly(true);
    ui->pre_lng1->setValidator(pReg);
    ui->pre_lng2->setValidator(pReg);
    ui->pre_lng3->setValidator(pReg);
    ui->pre_lat1->setValidator(pReg);
    ui->pre_lat2->setValidator(pReg);
    ui->pre_lat3->setValidator(pReg);

    axisX=new QDateTimeAxis;
    axisY=new QValueAxis;

}

MainWindow::~MainWindow()
{
 //   m_thread->quit();
    if(m_thread->isFinished()==false)
    {
        m_thread->terminate();
        m_thread->wait();
    }

   // delete view;
    delete ui;
}

void MainWindow::slot_finish(const QString &fileName)
{
    qDebug()<<"finish... now thread id="<<QThread::currentThreadId();
    statusBar()->showMessage(tr("Loaded %1").arg(fileName), 2000);

}

void MainWindow::slot_fail(const QString &fileName)
{
    statusBar()->showMessage(tr("Failed to load file")+fileName,2000);
    return;
}

void MainWindow::slot_progress(const double &p)
{
   // qDebug()<<"change progressBar...";
    if(p>100)
    {
        ui->progressBar->setValue(100);
    }
    else
    {
        ui->progressBar->setValue(p);
    }
    ui->progressBar->update();
}

void MainWindow::on_pushButton_clicked()
{
    dir_string=QFileDialog::getExistingDirectory();
    if(dir_string=="")
    {
        return;
    }

    if(ui->check_1->isChecked()) is_order=true; else is_order=false;
    if(ui->check_2->isChecked()) is_time=true; else is_time=false;
    if(ui->check_3->isChecked() && ui->check_4->isChecked()) is_dest=true; else is_dest=false;
    if(ui->check_5->isChecked()) is_fee=true; else is_fee=false;

    QDate d1=ui->fromDate->date();
    QDate d2=ui->toDate->date();
    d2=d2.addDays(1);
    int total_day=d2.day()-d1.day();
    QString str1="order_"+d1.toString("yyyyMMdd");
    QString str2="order_"+d2.toString("yyyyMMdd");

    QDir dir(dir_string);
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    QFileInfoList list=dir.entryInfoList();
    all=0;

    mapData.clear();
    data.clear();
    order.clear();

    emit sig_clear();
    m_thread->start();
    emit send_number(total_day*5);
    for(int i=list.size()-1;i>=0;i--)
    {
        QString filter=list.at(i).suffix();
        if(filter!="csv") continue;
        fileName=list.at(i).fileName();
        if(fileName.at(0)=='r' && fileName.at(1)=='e')
        {
            loadMap(dir_string,fileName);
            qDebug()<<dir_string+"/"+fileName;
        }
        else
        {
            qDebug()<<fileName<<" "<<str1<<"" <<str2<<" "<<QString::compare(fileName,str1)<<QString::compare(fileName,str2);
            if(fileName>=str1 && fileName<=str2)
            {
                 emit sig_work(dir_string,fileName,total_day);
            }
        }
    }

    if(is_order) emit need_order();
    emit need_data();

}

void MainWindow::loadMap(const QString &dir_string,const QString &fileName_0)
{
    QString fileName=dir_string+"/"+fileName_0;
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        statusBar()->showMessage(tr("Failed to load file")+file.fileName(),2000);
        return;
    }

    QTextStream stream(&file);
    QStringList headers = stream.readLine().split(',');
    int now=0;
    while(!stream.atEnd())
    {
        QString tmpLine=stream.readLine();
        if(!tmpLine.isEmpty())
        {
            QStringList p=tmpLine.split(',');
            QVector<double> q;
            q.clear();
            for(int i=0;i<7;i++)
            {
                q.append(p.value(i+1).toDouble());
            }
            mapData.append(q);
            now++;
        }
    }
    file.close();
    for(int i=0;i<mapData.size();i++)
    {
        double p=mapData.at(i).at(0),q=mapData.at(i).at(2);
        if((p-q)>1e-6) std::swap(p,q);
        map[i][0]=p; map[i][1]=q;    //lng
        p=mapData.at(i).at(1),q=mapData.at(i).at(5);
        if((p-q)>1e-6) std::swap(p,q);
        map[i][2]=p; map[i][3]=q;    //lat
    }
    statusBar()->showMessage(tr("Loaded %1").arg(fileName), 2000);
}

void MainWindow::receive_data(QVector< QVector<double> > r_data)
{
    data=r_data;
    qDebug()<<"successful transmitted"<<data.size();

    QDate d1=ui->fromDate->date();
    QDate d2=ui->toDate->date();

    ui->fromTime->setMinimumDate(d1);
    ui->fromTime->setDate(d1);
    ui->query_fromTime->setMinimumDate(d1);
    ui->query_fromTime->setDate(d1);

    ui->toTime->setMaximumDate(d2);
    ui->toTime->setDate(d2);
    ui->query_toTime->setMaximumDate(d2);
    ui->query_toTime->setDate(d2);

    loaded=true;

    ui->progressBar->setValue(100);
    ui->progressBar->update();

    statusBar()->showMessage(tr("Successful Transmitted!"), 2000);
}

void MainWindow::receive_order(QVector< QString > r_order)
{
    order=r_order;
    qDebug()<<"receive order";
}

void MainWindow::on_fromTime_dateTimeChanged(const QDateTime &dateTime)
{
    if(ui->toTime->dateTime()<dateTime)
    {
        ui->toTime->setDateTime(dateTime);
    }
}

void MainWindow::on_toTime_dateTimeChanged(const QDateTime &dateTime)
{
    if(ui->fromTime->dateTime()>dateTime)
    {
        ui->fromTime->setDateTime(dateTime);
    }
}

void MainWindow::on_query_fromTime_dateTimeChanged(const QDateTime &dateTime)
{
    if(ui->query_toTime->dateTime()<dateTime)
    {
        ui->query_toTime->setDateTime(dateTime);
    }
}

void MainWindow::on_query_toTime_dateTimeChanged(const QDateTime &dateTime)
{
    if(ui->query_fromTime->dateTime()>dateTime)
    {
        ui->query_fromTime->setDateTime(dateTime);
    }
}


void MainWindow::on_fromDate_userDateChanged(const QDate &date)
{
    if(ui->toDate->date()<date)
    {
        ui->toDate->setDate(date);
    }
}

void MainWindow::on_toDate_userDateChanged(const QDate &date)
{
    if(ui->fromDate->date()>date)
    {
        ui->fromDate->setDate(date);
    }
}


void MainWindow::on_fromGrid_currentIndexChanged(int index)
{
    if(ui->toGrid->currentIndex()<index)
    {
        ui->toGrid->setCurrentIndex(index);
    }
}

void MainWindow::on_toGrid_currentIndexChanged(int index)
{
    if(ui->fromGrid->currentIndex()>index)
    {
        ui->fromGrid->setCurrentIndex(index);
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    m_thread->terminate();
    m_thread->wait();

    ui->progressBar->reset();
    emit sig_clear();
    memset(map,0,sizeof(map));
    mapData.clear();
    data.clear();
    loaded=false;
    is_time=is_fee=is_dest=is_order=false;
    ui->fromTime->setMinimumDate(QDate(2016,11,1));
    ui->fromTime->setDate(QDate(2016,11,1));
    ui->toTime->setMaximumDate(QDate(2016,11,15));
    ui->toTime->setDate(QDate(2016,11,15));

    ui->query_fromTime->setMinimumDate(QDate(2016,11,1));
    ui->query_fromTime->setDate(QDate(2016,11,1));
    ui->query_toTime->setMaximumDate(QDate(2016,11,15));
    ui->query_toTime->setDate(QDate(2016,11,15));

    statusBar()->showMessage(tr("Transmission Canceled!"), 2000);
}

void MainWindow::on_query_demand_clicked()
{
    if(loaded==false)
    {
        QMessageBox::information(NULL,"Information","Data has not been loaded!");
        return;
    }

    if((ui->radio_6->isChecked() || ui->radio_7->isChecked()) && !is_fee)
    {
        QMessageBox::information(NULL,"Information","Fee of order has not been loaded!");
        return;
    }

    if(ui->radio_5->isChecked() && !is_time)
    {
        QMessageBox::information(NULL,"Information","Ride End Time has not been loaded!");
        return;
    }

    emit on_clear_clicked();

    int fromTime=ui->fromTime->dateTime().toTime_t();
    int toTime=ui->toTime->dateTime().toTime_t();
    int fromGrid=ui->fromGrid->currentIndex();
    int toGrid=ui->toGrid->currentIndex();

    int timeStep;
    if(ui->radio_1->isChecked()) timeStep=86400;
    if(ui->radio_2->isChecked()) timeStep=3600;
    if(ui->radio_3->isChecked()) timeStep=(ui->spinBox->value())*3600;

    int left=0;
    int right=data.size()-1;
    while(left<data.size())
    {
        int tmp=data.at(left).at(0);
        if(tmp>=fromTime) break;
        left++;
    }
    while(right>0)
    {
        int tmp=data.at(right).at(0);
        if(tmp<=toTime) break;
        right--;
    }

    QVector<int> index;
    index.clear();
    for(int i=left;i<=right;i++)
    {
        bool check_i=false;
        for(int j=fromGrid;j<=toGrid;j++)
        {
            if((data.at(i).at(2)-map[j][0])>1e-6 && (map[j][1]-data.at(i).at(2))>1e-6
                    && (data.at(i).at(3)-map[j][2])>1e-6 && (map[j][3]-data.at(i).at(3))>1e-6)
            {
                check_i=true;
                break;
            }
        }
        if(check_i) index.push_back(i);
    }

    qDebug()<<"calc index...";

   // if(!(ui->radio_4->isChecked())) timeStep/=3;
    int totStep=(toTime-fromTime)/timeStep;
    if((toTime-fromTime)%timeStep!=0) totStep++;
    int num[totStep+5];
    int fee[totStep+5];

    int tot=index.size();

    double mx,mn;
    mx=0;
    mn=100000005;

    QFont ft;
    ft.setPointSize(18);
    chart->setTitleFont(ft);
    chart->setTheme(QChart::ChartThemeLight);

    if(ui->radio_4->isChecked() || ui->radio_7->isChecked()) //绘制折线图
    {
        memset(num,0,sizeof(num));
        memset(fee,0,sizeof(fee));

        for(int i=0;i<tot;i++)
         {
             int now=(data.at(index.at(i)).at(0)-fromTime)/timeStep; //根据时间间隔，找出落入哪个点
             if(now<0) continue;
             num[now]++;
             fee[now]+=data.at(index.at(i)).at(6);
         }

        series->setPen(QPen(QColor(100,149,237),2));
        series_1->setPen(QPen(QColor(238,44,44),1.5,Qt::DashLine));

        if(ui->radio_4->isChecked())
        {
            for(int i=0;i<totStep;i++)
            {
                series->append((double)i,num[i]);
                mx=std::max((double)num[i],mx);
                mn=std::min((double)num[i],mn);
            }

            if(totStep<=25)
            {
                for(int i=0;i<totStep;i++) series_1->append((double)i,num[i]);
            }
            axisY->setTitleText("Number of Orders");
            chart->setTitle("Spatio-temporal Demand Patterns");
        }

        if(ui->radio_7->isChecked())
        {
            for(int i=0;i<totStep;i++)
            {
                series->append((double)i,fee[i]);
                mx=std::max((double)fee[i],mx);
                mn=std::min((double)fee[i],mn);
            }

            if(totStep<=25)
            {
                for(int i=0;i<totStep;i++) series_1->append((double)i,fee[i]);
            }
            chart->setTitle("Total Revenue");
            axisY->setTitleText("Fee");
        }

        axisX->setGridLineVisible(true);
        axisX->setMin(ui->fromTime->dateTime());
        axisX->setMax(ui->toTime->dateTime());
        axisX->setFormat("MM-dd HH:mm");
        axisX->setTitleText("Time");
        axisY->setGridLineVisible(true);

        if(totStep<=25)
        {
            axisY->setMin(mn/3);
            axisY->setMax(mx*3);
        }
        else
        {
            axisY->setMin(mn);
            axisY->setMax(mx+10);
        }

        chart->addSeries(series_1);
        chart->addSeries(series);
        chart->addAxis(axisX,Qt::AlignBottom);
        chart->addAxis(axisY,Qt::AlignLeft);
    }

    if(ui->radio_5->isChecked() || ui->radio_6->isChecked())
    {
        double tmp,d;
        int s[10],idx;
        memset(s,0,sizeof(s));
        if(ui->radio_5->isChecked())
        {
            for(int i=0;i<tot;i++)
            {
                tmp=data.at(index.at(i)).at(1)-data.at(index.at(i)).at(0);   //时间长度
                mn=std::min(mn,tmp);
                mx=std::max(mx,tmp);
            }
            mx=std::min(mx,14400.0); //大于4小时舍去
            d=(mx-mn)/10;
            for(int i=0;i<tot;i++)
            {
                tmp=data.at(index.at(i)).at(1)-data.at(index.at(i)).at(0);
                idx=(int)((tmp-mn)/d);
                if(idx<10) s[idx]++;
            }
            QTime t1(0,0,mn),t2(0,0,mn);
            for(int i=0;i<10;i++)
            {
                t2=t2.addSecs(d);
                series_2->append(t1.toString("hh:mm:ss")+"~"+t2.toString("hh:mm:ss"),s[i]);
                t1=t2;
            }
            chart->setTitle("Distribution of Travel Time");
        }

        if(ui->radio_6->isChecked())
        {
            for(int i=0;i<tot;i++)
            {
                tmp=data.at(index.at(i)).at(6);   //费用
                mn=std::min(mn,tmp);
                mx=std::max(mx,tmp);
            }
            d=(mx-mn)/10;
            for(int i=0;i<tot;i++)
            {
                tmp=data.at(index.at(i)).at(1)-data.at(index.at(i)).at(0);
                idx=(int)((tmp-mn)/d);
                if(idx<10) s[idx]++;
            }
            double last,now;
            last=now=mn;
            for(int i=0;i<10;i++)
            {
                now=std::min(mx,now+d);
                series_2->append(QString::number(last,10,2)+"~"+QString::number(now,10,2),s[i]);
                last=now;
            }
            chart->setTitle("Distribution of Order Fees");
        }

        series_2->setLabelsVisible(false);
        series_2->setUseOpenGL(true);
        chart->addSeries(series_2);

        chart->legend()->setVisible(true);
        chart->legend()->setAlignment(Qt::AlignRight);
        chart->legend()->setBackgroundVisible(true);
        chart->legend()->setAutoFillBackground(true);
    }

}

void MainWindow::on_radio_1_clicked()
{
    ui->spinBox->setDisabled(true);
    ui->slider->setDisabled(true);
}

void MainWindow::on_radio_2_clicked()
{
    ui->spinBox->setDisabled(true);
    ui->slider->setDisabled(true);
}

void MainWindow::on_radio_3_clicked()
{
    ui->spinBox->setEnabled(true);
    ui->slider->setEnabled(true);
}



void MainWindow::on_query_similar_clicked()
{
    if(!loaded)
    {
        QMessageBox::information(NULL,"Information","Data has not been loaded!");
        return;
    }

    int t1=ui->query_fromTime->dateTime().toTime_t();
    int t2=ui->query_toTime->dateTime().toTime_t();

    bool f1=true,f2=true;
    double lng_1=ui->lng_1->text().toDouble();
    double lng_2=ui->lng_2->text().toDouble();
    double lat_1=ui->lat_1->text().toDouble();
    double lat_2=ui->lat_2->text().toDouble();

    if(std::fabs(lng_1)<1e-6 || std::fabs(lat_1)<1e-6) f1=false;
    if(std::fabs(lng_2)<1e-6 || std::fabs(lat_2)<1e-6) f2=false;

    v.clear();
    double tmp;

    if(ui->radio_8->isChecked())
    {
        if(!is_time)
        {
            QMessageBox::information(NULL,"Information","Ride end time has not been loaded!");
            return;
        }
        set_header();
        for(int i=0;i<data.size();i++)
        {
           tmp=std::fabs(data.at(i).at(0)-t1)+std::fabs(data.at(i).at(1)-t2);
           v.push_back(qMakePair(tmp,i));
        }
        std::sort(v.begin(),v.end());
        draw_table();
    }

    if(ui->radio_9->isChecked())
    {
        if(!is_dest)
        {
            QMessageBox::information(NULL,"Information","Drop-off location has not been loaded!");
            return;
        }
        if(!f1 || !f2)
        {
            QMessageBox::information(NULL,"Information","Please input valid location first!");
            return;
        }
        set_header();
        for(int i=0;i<data.size();i++)
        {
            tmp=std::fabs(data.at(i).at(2)-lng_1)+std::fabs(data.at(i).at(3)-lat_1)+std::fabs(data.at(i).at(4)-lng_2)+std::fabs(data.at(i).at(5)-lat_2);
            v.push_back(qMakePair(tmp,i));
        }
        std::sort(v.begin(),v.end());
        draw_table();
    }

    if(ui->radio_10->isChecked())
    {
        if(!f1)
        {
            QMessageBox::information(NULL,"Information","Please input valid pick-up location first!");
            return;
        }
        set_header();
        for(int i=0;i<data.size();i++)
        {
            tmp=std::fabs(data.at(i).at(2)-lng_1)+std::fabs(data.at(i).at(3)-lat_1);
            v.push_back(qMakePair(tmp,i));
        }
        std::sort(v.begin(),v.end());
        draw_table();
    }

    if(ui->radio_11->isChecked())
    {
        if(!is_dest)
        {
            QMessageBox::information(NULL,"Information","Drop-off location has not been loaded!");
            return;
        }
        if(!f2)
        {
            QMessageBox::information(NULL,"Information","Please input valid drop-off location first!");
            return;
        }
        set_header();
        for(int i=0;i<data.size();i++)
        {
            tmp=std::fabs(data.at(i).at(4)-lng_2)+std::fabs(data.at(i).at(5)-lat_2);
            v.push_back(qMakePair(tmp,i));
        }
        std::sort(v.begin(),v.end());
        draw_table();
    }
}


void MainWindow::on_clear_similar_clicked()
{
    model->clear();
    ui->lng_1->clear();
    ui->lat_1->clear();
    ui->lng_2->clear();
    ui->lat_2->clear();
}

void MainWindow::set_header()
{
    model->clear();
    model->setColumnCount(7);
    model->setRowCount(100);
    model->setHorizontalHeaderItem(0,new QStandardItem(QObject::tr("Id of order")));
    model->setHorizontalHeaderItem(1,new QStandardItem(QObject::tr("Ride Start Time")));
    model->setHorizontalHeaderItem(2,new QStandardItem(QObject::tr("Ride End Time")));
    model->setHorizontalHeaderItem(3,new QStandardItem(QObject::tr("Pick-up Longitude")));
    model->setHorizontalHeaderItem(4,new QStandardItem(QObject::tr("Pick-up Latitude")));
    model->setHorizontalHeaderItem(5,new QStandardItem(QObject::tr("Drop-off Longitude")));
    model->setHorizontalHeaderItem(6,new QStandardItem(QObject::tr("Drop-off Latitude")));
    model->setHorizontalHeaderItem(7,new QStandardItem(QObject::tr("Drop-off Longitude")));
}

void MainWindow::draw_table()
{
    int sum=std::min(v.size(),100);
    for(int i=0;i<sum;i++)
    {
        int pos=v.at(i).second;
        if(is_order) model->setItem(i,0,new QStandardItem(order.at(pos)));
        model->setItem(i,1,new QStandardItem(QDateTime::fromTime_t((int)data.at(pos).at(0)).toString("yyyy-MM-dd hh:mm:ss")));
        if(is_time) model->setItem(i,2,new QStandardItem(QDateTime::fromTime_t((int)data.at(pos).at(1)).toString("yyyy-MM-dd hh:mm:ss")));
        model->setItem(i,3,new QStandardItem(QString::number(data.at(pos).at(2),'f',8)));
        model->setItem(i,4,new QStandardItem(QString::number(data.at(pos).at(3),'f',8)));
        if(is_dest)
        {
            model->setItem(i,5,new QStandardItem(QString::number(data.at(pos).at(4),'f',8)));
            model->setItem(i,6,new QStandardItem(QString::number(data.at(pos).at(5),'f',8)));
        }
        if(is_fee) model->setItem(i,7,new QStandardItem(QString::number(data.at(pos).at(6),'f',2)));
    }
}

void MainWindow::on_clear_clicked()
{
    chart->removeSeries(series);
    chart->removeSeries(series_1);
    chart->removeSeries(series_2);
    chart->legend()->setVisible(false);
    QVector<QPointF> points;
    series->replace(points);
    series_1->replace(points);
    series_2->clear();
    chart->removeAxis(axisX);
    chart->removeAxis(axisY);

}

void MainWindow::on_query_prediction_clicked()
{
    if(!is_time)
    {
        QMessageBox::information(NULL,"Information","Ride End Time has not been loaded!");
        return;
    }
    if(!is_dest)
    {
        QMessageBox::information(NULL,"Information","Drop-off location has not been loaded!");
        return;
    }

    double lat1=ui->pre_lat1->text().toDouble();
    double lng1=ui->pre_lng1->text().toDouble();
    double lat2=ui->pre_lat2->text().toDouble();
    double lng2=ui->pre_lng2->text().toDouble();
    if(std::fabs(lat1)<1e-6 || std::fabs(lng1)<1e-6 || std::fabs(lat2)<1e-6 || std::fabs(lng2)<1e-6)
    {
        QMessageBox::information(NULL,"Information","Please input complete origin and destination first!");
        return;
    }

    QVariant Lat1(lat1),Lng1(lng1),Lat2(lat2),Lng2(lng2);
    QObject *r1=ui->quickWidget->rootObject()->findChild<QObject*> ("Map");
    QMetaObject::invokeMethod(r1,"update_point1",Q_ARG(QVariant,Lat1),Q_ARG(QVariant,Lng1));
    QMetaObject::invokeMethod(r1,"update_point2",Q_ARG(QVariant,Lat2),Q_ARG(QVariant,Lng2));

    double tmp=0;
    double x1,y1,x2,y2;
    int pos,hour,min,sec;
    double pre_time=0;
    v.clear();
    for(int i=0;i<data.size();i++)
    {
        x1=std::fabs(data.at(i).at(2)-lng1)*100;
        y1=std::fabs(data.at(i).at(3)-lat1)*100;
        x2=std::fabs(data.at(i).at(4)-lng2)*100;
        y2=std::fabs(data.at(i).at(5)-lat2)*100;
        tmp=x1+y1+x2+y2;
        v.push_back(qMakePair(tmp,i));
    }
    std::sort(v.begin(),v.end());
    for(int i=0;i<10;i++)
    {
        pos=v.at(i).second;
        pre_time=(pre_time+(data.at(pos).at(1)-data.at(pos).at(0))*(10-i)*(10-i)*(10-i)/3025);   //3025=1^3+2^3+...+10^3
    }
    hour=pre_time/60/60;
    min=(pre_time-hour*3600)/60;
    sec=pre_time-hour*3600-min*60;
    ui->pre_time->setText(QTime(hour,min,sec).toString("hh:mm:ss"));
    statusBar()->showMessage(tr("Prediction completed!"), 2000);



}

void MainWindow::on_clear_prediction_clicked()
{
    ui->pre_lat1->clear();
    ui->pre_lng1->clear();
    ui->pre_lat2->clear();
    ui->pre_lng2->clear();
    ui->pre_time->clear();
}

void MainWindow::on_clear_prediction_2_clicked()
{
    ui->pre_lat3->clear();
    ui->pre_lng3->clear();
    ui->pre_lat4->clear();
    ui->pre_lng4->clear();
}

void MainWindow::on_query_prediction_2_clicked()
{
    double lat=ui->pre_lat3->text().toDouble();
    double lng=ui->pre_lng3->text().toDouble();
    QTime time=ui->timeEdit->time();
    if(std::fabs(lat)<1e-6 || std::fabs(lng)<1e-6)
    {
        QMessageBox::information(NULL,"Information","Please input complete origin and destination first!");
        return;
    }
    if(!is_dest)
    {
        QMessageBox::information(NULL,"Information","Drop-off location has not been loaded!");
        return;
    }

    double tmp=0;
    double x1,y1,dtime;
    v.clear();
    for(int i=0;i<data.size();i++)
    {
        x1=std::fabs(data.at(i).at(2)-lng);
        y1=std::fabs(data.at(i).at(3)-lat);
        dtime=std::abs(QDateTime::fromTime_t(data.at(i).at(0)).time().secsTo(time))/60;
        tmp=(x1*x1+y1*y1)*100000+dtime;
        v.push_back(qMakePair(tmp,i));
    }
    std::sort(v.begin(),v.end());
    double pre_lng=0,pre_lat=0;
    int pos;
    for(int i=0;i<10;i++)
    {
        pos=v.at(i).second;
        pre_lng=pre_lng+data.at(pos).at(4)*(10-i)*(10-i)*(10-i)/3025;
        pre_lat=pre_lat+data.at(pos).at(5)*(10-i)*(10-i)*(10-i)/3025;
    }
    ui->pre_lng4->setText(QString::number(pre_lng,10,6));
    ui->pre_lat4->setText(QString::number(pre_lat,10,6));

    QVariant Lat1(lat),Lng1(lng),Lat2(pre_lat),Lng2(pre_lng);
    QObject *r1=ui->quickWidget->rootObject()->findChild<QObject*> ("Map");
    QMetaObject::invokeMethod(r1,"update_point1",Q_ARG(QVariant,Lat1),Q_ARG(QVariant,Lng1));
    QMetaObject::invokeMethod(r1,"update_point2",Q_ARG(QVariant,Lat2),Q_ARG(QVariant,Lng2));

    statusBar()->showMessage(tr("Prediction completed!"), 2000);
}
