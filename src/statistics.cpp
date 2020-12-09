/* source file for statistics class -
   resposible for showing simple statistics */

#include "statistics.h"
#include "ui_statistics.h"

statistics::statistics(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::statistics)
{
    ui->setupUi(this);

    num_samples = 0;
    for (int i=0; i<9; i++)
    {
        braindata[i] = QVector<double>();
        filtbraindata[i] = QVector<double>(); // vectors for smoothed plots
    }

    smoothplots = true;
    ui->checkBox_9->setChecked(smoothplots);
    show_theta = show_delta = show_alpha = show_beta = show_gamma = show_hgamma = show_att = show_medit = true;

    bars_x = QVector<double>();
    bars_y = QVector<double>();

    recordsfound = false;        

    connect(ui->plot->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(xAxisChanged(QCPRange)));
    connect(ui->plot_2->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(xAxisChanged2(QCPRange)));

    ui->plot->installEventFilter(this);
    ui->plot_2->installEventFilter(this);
    ui->plot_2->xAxis->setVisible(false);

    // === Pie Chart of waves expressions ===
    chart = new QChart();
    chart->legend()->hide();
    chart->layout()->setContentsMargins(0, 0, 0, 0);
    chart->setBackgroundBrush(this->palette().color(QWidget::backgroundRole()));
    ui->graphicsView->setChart(chart);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);

    slice1 = new QPieSlice(); slice1->setLabel("Delta");  slice1->setBrush(Qt::gray);
    slice2 = new QPieSlice(); slice2->setLabel("Theta");  slice2->setBrush(Qt::cyan);
    slice3 = new QPieSlice(); slice3->setLabel("Alpha");  slice3->setBrush(Qt::magenta);
    slice4 = new QPieSlice(); slice4->setLabel("Beta");   slice4->setBrush(Qt::blue);
    slice5 = new QPieSlice(); slice5->setLabel("Gamma");  slice5->setBrush(QColor(255,165,0));
    slice6 = new QPieSlice(); slice6->setLabel("Hgamma"); slice6->setBrush(Qt::darkYellow);

    series = new QPieSeries();
    series->setPieSize(1);
    series->setHoleSize(0.2);

    mean_theta = 0.1;  mean_delta = 0.1;  mean_alpha = 0.3;  mean_beta = 0.3;  mean_gamma = 0.1;  mean_hgamma = 0.1;
    slice1->setValue(mean_delta);  slice2->setValue(mean_theta);  slice3->setValue(mean_alpha);
    slice4->setValue(mean_beta);   slice5->setValue(mean_gamma);  slice6->setValue(mean_hgamma);
    QFont qft;  qft.setStyleName("Times");  qft.setBold(true);  qft.setPixelSize(11);
    slice1->setLabelFont(qft); slice2->setLabelFont(qft); slice3->setLabelFont(qft);
    slice4->setLabelFont(qft); slice5->setLabelFont(qft); slice6->setLabelFont(qft);

    series->append(slice1);  series->append(slice2);  series->append(slice3);
    series->append(slice4);  series->append(slice5);  series->append(slice6);
    series->setLabelsVisible();
    series->setLabelsPosition(QPieSlice::LabelInsideHorizontal);
    chart->addSeries(series);

    // === Bar Chart of attention and meditation levels ===
    chart2 = new QChart();
    chart2->legend()->hide();
    chart2->layout()->setContentsMargins(0, 0, 0, 0);
    chart2->setBackgroundBrush(this->palette().color(QWidget::backgroundRole()));
    ui->graphicsView_2->setChart(chart2);
    ui->graphicsView_2->setRenderHint(QPainter::Antialiasing);

    att = new QBarSet("Attention");
    medit = new QBarSet("Meditation");
    mean_att = 10; mean_medit = 10;
    att->append(77); att->setColor(Qt::red);
    medit->append(66); medit->setColor(Qt::green);
    barseries = new QBarSeries();
    barseries->append(att);
    barseries->append(medit);
    chart2->addSeries(barseries);

    // parameters for filter for smoothing plot lines
    int lcutoff = 4; int hcutoff = 36; int butterord = 1; int srfr = 500;
    plotsfilt = new filters(butterord,lcutoff,hcutoff,srfr);
}

statistics::~statistics()
{
    delete ui;
}

void statistics::update_charts_and_levels()
{
    slice1->setValue(mean_delta);
    slice2->setValue(mean_theta);
    slice3->setValue(mean_alpha);
    slice4->setValue(mean_beta);
    slice5->setValue(mean_gamma);
    slice6->setValue(mean_hgamma);
    att->replace(0,mean_att);
    medit->replace(0,mean_medit);
    for (auto slice: series->slices())
        slice->setLabel(QString("%1%").arg(100*slice->percentage(), 1, 'f', 1));
    ui->graphicsView->update();
    ui->graphicsView_2->update();
}

void statistics::update_plotlines()
{
    ui->plot->graph(0)->setVisible(show_delta);
    ui->plot->graph(1)->setVisible(show_theta);
    ui->plot->graph(2)->setVisible(show_alpha);
    ui->plot->graph(3)->setVisible(show_beta);
    ui->plot->graph(4)->setVisible(show_gamma);
    ui->plot->graph(5)->setVisible(show_hgamma);
    ui->plot_2->graph(0)->setVisible(show_att);
    ui->plot_2->graph(0)->setVisible(show_medit);
    ui->plot->replot();
    ui->plot_2->replot();
}

void statistics::initplots()
{    
    ui->plot->clearGraphs();
    ui->plot->xAxis->setRange(0,num_samples);
    ui->plot->yAxis->setRange(0,50);
    ui->plot->legend->setVisible(false);

    ui->plot->addGraph();
    ui->plot->graph(0)->setPen(QPen(Qt::gray));
    ui->plot->graph(0)->setName("Delta");
    ui->plot->graph(0)->setData(xcoords, braindata[3]);

    ui->plot->addGraph();
    ui->plot->graph(1)->setPen(QPen(Qt::cyan));
    ui->plot->graph(1)->setName("Theta");
    ui->plot->graph(1)->setData(xcoords, braindata[4]);

    ui->plot->addGraph();
    ui->plot->graph(2)->setPen(QPen(Qt::magenta));
    ui->plot->graph(2)->setName("Alpha");
    ui->plot->graph(2)->setData(xcoords, braindata[5]);

    ui->plot->addGraph();
    ui->plot->graph(3)->setPen(QPen(Qt::blue));
    ui->plot->graph(3)->setName("Beta");
    ui->plot->graph(3)->setData(xcoords, braindata[6]);

    ui->plot->addGraph();
    ui->plot->graph(4)->setPen(QPen(QColor(255,165,0)));
    ui->plot->graph(4)->setName("Gamma");
    ui->plot->graph(4)->setData(xcoords, braindata[7]);

    ui->plot->addGraph();
    ui->plot->graph(5)->setPen(QPen(Qt::darkYellow));
    ui->plot->graph(5)->setName("H-Gamma");
    ui->plot->graph(5)->setData(xcoords, braindata[8]);    

    ui->plot->setInteraction(QCP::iRangeDrag, true);
    ui->plot->setInteraction(QCP::iRangeZoom, true);
    ui->plot->axisRect()->setRangeZoom(Qt::Horizontal);
    ui->plot->axisRect()->setRangeDrag(Qt::Horizontal);

    ui->plot_2->xAxis->setRange(0,num_samples);
    ui->plot_2->yAxis->setRange(0,100);
    ui->plot_2->legend->setVisible(false);

    ui->plot_2->addGraph();
    ui->plot_2->graph(0)->setPen(QPen(Qt::red));
    ui->plot_2->graph(0)->setName("Attention");
    ui->plot_2->graph(0)->setData(xcoords, braindata[1]);

    ui->plot_2->addGraph();
    ui->plot_2->graph(1)->setPen(QPen(Qt::green));
    ui->plot_2->graph(1)->setName("Meditation");
    ui->plot_2->graph(1)->setData(xcoords, braindata[2]);

    ui->plot_2->setInteraction(QCP::iRangeDrag, true);
    ui->plot_2->setInteraction(QCP::iRangeZoom, true);
    ui->plot_2->axisRect()->setRangeZoom(Qt::Horizontal);
    ui->plot_2->axisRect()->setRangeDrag(Qt::Horizontal);

    ui->plot->replot();
    ui->plot_2->replot();
}

// process keys and change both plots in exactly same way
bool statistics::eventFilter(QObject *target, QEvent *event)
{
    if ((target == ui->plot) && (event->type() == QEvent::KeyPress))
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Left)
        {
            double low = ui->plot->xAxis->range().lower;
            double high = ui->plot->xAxis->range().upper;
            ui->plot->xAxis->moveRange(-((high - low) / 10));
            ui->plot_2->xAxis->moveRange(-((high - low) / 10));
            ui->plot->replot();
            ui->plot_2->replot();
        }
        if (keyEvent->key() == Qt::Key_Right)
        {
            double low = ui->plot->xAxis->range().lower;
            double high = ui->plot->xAxis->range().upper;
            ui->plot->xAxis->moveRange((high - low) / 10);
            ui->plot_2->xAxis->moveRange((high - low) / 10);
            ui->plot->replot();
            ui->plot_2->replot();
        }
        return true;
    } else
    if ((target == ui->plot_2) && (event->type() == QEvent::KeyPress))
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Left)
        {
            double low = ui->plot->xAxis->range().lower;
            double high = ui->plot->xAxis->range().upper;
            ui->plot->xAxis->moveRange(-((high - low) / 10));
            ui->plot_2->xAxis->moveRange(-((high - low) / 10));
            ui->plot->replot();
            ui->plot_2->replot();
        }
        if (keyEvent->key() == Qt::Key_Right)
        {
            double low = ui->plot->xAxis->range().lower;
            double high = ui->plot->xAxis->range().upper;
            ui->plot->xAxis->moveRange((high - low) / 10);
            ui->plot_2->xAxis->moveRange((high - low) / 10);
            ui->plot->replot();
            ui->plot_2->replot();
        }
        return true;
    }
    else
        return false;
}

// reading data from file to vectors
void statistics::readdata_fromfile(QString fname)
{
    for (int i=0; i<9; i++)
    {
        braindata[i].clear();
        filtbraindata[i].clear();
    }
    num_samples = 0;

    QFile inputFile(fname);
    inputFile.open(QIODevice::ReadOnly);
    QTextStream fin(&inputFile);
    QStringList sl;

    QString line = fin.readLine();
    sl = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

    if (sl.length()>3)
    {
        if (sl[3]=="simulated")
            ui->label_16->setText("Data source: waves generator");
        else if (sl[3]=="recorded")
            ui->label_16->setText("Data source: recorded EEG");
    } else
        ui->label_16->setText("Data source: unknown");

    fin.readLine();
    line = fin.readLine();
    while (!fin.atEnd())
    {
        sl = line.split(QRegExp("\\,"), QString::SkipEmptyParts);
        if (sl.length()>1)
        {
            for (int i=0; i<9; i++)
            {
                braindata[i].append(sl[i].toInt());
                filtbraindata[i].append(sl[i].toInt());
            }
            num_samples++;
            line = fin.readLine();
        }
    }
    inputFile.close();

    xcoords.clear();
    for (int i=0; i<num_samples; i++)
        xcoords.push_back(i);

    for (int i=1; i<9; i++)
        plotsfilt->zerophasefilt(0,num_samples,filtbraindata[i]);

    double mean_val = 0;
    for (int i=1; i<9; i++)
    {
        mean_val = std::accumulate(braindata[i].begin(), braindata[i].end(),0)/num_samples;
        for (int j=0; j<num_samples; j++)
            filtbraindata[i][j]+=mean_val;
    }

    ui->label_21->setText("record rate: "+QString::number(1000/(recordrate*1000))+"Hz");

    initplots();
    if (smoothplots)
    {
        for (int i=0; i<6; i++)
            ui->plot->graph(i)->setData(xcoords, filtbraindata[i+3]);
        ui->plot->replot();
    }
}

void statistics::fill_bars_vals(int minv, int maxv, int index, int left_offset, int right_offset)
{
    bars_x.clear();
    bars_y.clear();
    for (int i=minv; i<=maxv; i++)
    {
      bars_x.append(i);
      bars_y.append(0);
    }

    for (int i=left_offset; i<num_samples-right_offset; i++)
        for (int j=minv; j<=maxv; j++)
            if ((braindata[index][i]>=j) && (braindata[index][i]<(j+1)))
                bars_y[j-minv]+=1;

    for (int i=0; i<bars_y.size(); i++) // rescaling to seconds
        bars_y[i]*=recordrate;

    max_bars_val = *std::max_element(bars_y.begin(), bars_y.end());
}

void statistics::update_plots(int left_offset, int right_offset)
{
    ui->attention_hist->clearPlottables();
    ui->meditation_hist->clearPlottables();
    ui->delta_hist->clearPlottables();
    ui->theta_hist->clearPlottables();
    ui->alpha_hist->clearPlottables();
    ui->beta_hist->clearPlottables();
    ui->gamma_hist->clearPlottables();
    ui->hgamma_hist->clearPlottables();

    int numels = num_samples - left_offset - right_offset;
    fill_bars_vals(0,100,1,left_offset,right_offset);
    mean_att = std::accumulate(braindata[1].begin()+left_offset, braindata[1].end()-right_offset,0)/numels;
    QCPBars *bars1 = new QCPBars(ui->attention_hist->xAxis, ui->attention_hist->yAxis);
    bars1->setWidth(1);   
    bars1->setData(bars_x, bars_y);
    bars1->setPen(Qt::NoPen);
    bars1->setBrush(Qt::red);
    ui->attention_hist->rescaleAxes();
    ui->attention_hist->yAxis->setRange(0, max_bars_val);
    //ui->attention_hist->xAxis->setLabel("%");
    ui->attention_hist->replot();

    fill_bars_vals(0,100,2,left_offset,right_offset);
    mean_medit = std::accumulate(braindata[2].begin()+left_offset, braindata[2].end()-right_offset,0)/numels;
    QCPBars *bars2 = new QCPBars(ui->meditation_hist->xAxis, ui->meditation_hist->yAxis);
    bars2->setWidth(1);
    bars2->setData(bars_x, bars_y);
    bars2->setPen(Qt::NoPen);
    bars2->setBrush(Qt::green);
    ui->meditation_hist->rescaleAxes();
    ui->meditation_hist->yAxis->setRange(0, max_bars_val);
    //ui->meditation_hist->xAxis->setLabel("%");
    ui->meditation_hist->replot();

    int min_val = *std::min_element(braindata[3].begin()+left_offset, braindata[3].end()-right_offset);
    int max_val = *std::max_element(braindata[3].begin()+left_offset, braindata[3].end()-right_offset);
    mean_delta = std::accumulate(braindata[3].begin()+left_offset, braindata[3].end()-right_offset,0)/numels;
    fill_bars_vals(min_val,max_val,3,left_offset,right_offset);
    QCPBars *bars3 = new QCPBars(ui->delta_hist->xAxis, ui->delta_hist->yAxis);
    bars3->setWidth(1);
    bars3->setData(bars_x, bars_y);
    bars3->setPen(Qt::NoPen);
    bars3->setBrush(Qt::gray);
    ui->delta_hist->rescaleAxes();
    ui->delta_hist->yAxis->setRange(0, max_bars_val);
    //ui->delta_hist->xAxis->setLabel("%");
    ui->delta_hist->replot();

    min_val = *std::min_element(braindata[4].begin()+left_offset, braindata[4].end()-right_offset);
    max_val = *std::max_element(braindata[4].begin()+left_offset, braindata[4].end()-right_offset);
    mean_theta = std::accumulate(braindata[4].begin()+left_offset, braindata[4].end()-right_offset,0)/numels;
    fill_bars_vals(min_val,max_val,4,left_offset,right_offset);
    QCPBars *bars4 = new QCPBars(ui->theta_hist->xAxis, ui->theta_hist->yAxis);
    bars4->setWidth(1);
    bars4->setData(bars_x, bars_y);
    bars4->setPen(Qt::NoPen);
    bars4->setBrush(Qt::cyan);
    ui->theta_hist->rescaleAxes();
    ui->theta_hist->yAxis->setRange(0, max_bars_val);
    //ui->theta_hist->xAxis->setLabel("%");
    ui->theta_hist->replot();

    min_val = *std::min_element(braindata[5].begin()+left_offset, braindata[5].end()-right_offset);
    max_val = *std::max_element(braindata[5].begin()+left_offset, braindata[5].end()-right_offset);
    mean_alpha = std::accumulate(braindata[5].begin()+left_offset, braindata[5].end()-right_offset,0)/numels;
    fill_bars_vals(min_val,max_val,5,left_offset,right_offset);
    QCPBars *bars5 = new QCPBars(ui->alpha_hist->xAxis, ui->alpha_hist->yAxis);    
    bars5->setWidth(1);    
    bars5->setData(bars_x, bars_y);
    bars5->setPen(Qt::NoPen);
    bars5->setBrush(Qt::magenta);
    ui->alpha_hist->rescaleAxes();
    ui->alpha_hist->yAxis->setRange(0, max_bars_val);
    //ui->alpha_hist->xAxis->setLabel("%");
    ui->alpha_hist->replot();

    min_val = *std::min_element(braindata[6].begin()+left_offset, braindata[6].end()-right_offset);
    max_val = *std::max_element(braindata[6].begin()+left_offset, braindata[6].end()-right_offset);
    mean_beta = std::accumulate(braindata[6].begin()+left_offset, braindata[6].end()-right_offset,0)/numels;
    fill_bars_vals(min_val,max_val,6,left_offset,right_offset);
    QCPBars *bars6 = new QCPBars(ui->beta_hist->xAxis, ui->beta_hist->yAxis);
    bars6->setWidth(1);
    bars6->setData(bars_x, bars_y);
    bars6->setPen(Qt::NoPen);
    bars6->setBrush(Qt::blue);
    ui->beta_hist->rescaleAxes();
    ui->beta_hist->yAxis->setRange(0, max_bars_val);
    //ui->beta_hist->xAxis->setLabel("%");
    ui->beta_hist->replot();

    min_val = *std::min_element(braindata[7].begin()+left_offset, braindata[7].end()-right_offset);
    max_val = *std::max_element(braindata[7].begin()+left_offset, braindata[7].end()-right_offset);
    mean_gamma = std::accumulate(braindata[7].begin()+left_offset, braindata[7].end()-right_offset,0)/numels;
    fill_bars_vals(min_val,max_val,7,left_offset,right_offset);
    QCPBars *bars7 = new QCPBars(ui->gamma_hist->xAxis, ui->gamma_hist->yAxis);
    bars7->setWidth(1);
    bars7->setData(bars_x, bars_y);
    bars7->setPen(Qt::NoPen);
    bars7->setBrush(QColor(255,165,0));
    ui->gamma_hist->rescaleAxes();
    ui->gamma_hist->yAxis->setRange(0, max_bars_val);
    //ui->gamma_hist->xAxis->setLabel("%");
    ui->gamma_hist->replot();

    min_val = *std::min_element(braindata[8].begin()+left_offset, braindata[8].end()-right_offset);
    max_val = *std::max_element(braindata[8].begin()+left_offset, braindata[8].end()-right_offset);
    mean_hgamma = std::accumulate(braindata[8].begin()+left_offset, braindata[8].end()-right_offset,0)/numels;
    fill_bars_vals(min_val,max_val,8,left_offset,right_offset);
    QCPBars *bars8 = new QCPBars(ui->hgamma_hist->xAxis, ui->hgamma_hist->yAxis);
    bars8->setWidth(1);
    bars8->setData(bars_x, bars_y);
    bars8->setPen(Qt::NoPen);
    bars8->setBrush(Qt::darkYellow);
    ui->hgamma_hist->rescaleAxes();
    ui->hgamma_hist->yAxis->setRange(0, max_bars_val);
    //ui->hgamma_hist->xAxis->setLabel("%");
    ui->hgamma_hist->replot();   

    ui->label_13->setText(QString::number((double)(num_samples-right_offset-left_offset)/5,'f',1)+" s");
    ui->lineEdit->setText(QString::number((double)left_offset/5,'f',1));
    ui->lineEdit_3->setText(QString::number((double)left_offset/5,'f',1));
    ui->lineEdit_2->setText(QString::number((double)(num_samples-right_offset)/5,'f',1));
    ui->lineEdit_4->setText(QString::number((double)(num_samples-right_offset)/5,'f',1));

    ui->label_19->setText(QString::number((int)mean_att)+"%");
    ui->label_20->setText(QString::number((int)mean_medit)+"%");
    update_charts_and_levels();
}

QString transform(QString date)
{
    return "Session time: "+date.mid(4, 2)+"."+date.mid(6, 2)+'.'+date.mid(8, 4)+' '+date.mid(13, 2)+'.'+date.mid(15, 2);
}

void statistics::update_filenames()
{
   // recfilename  = QCoreApplication::applicationDirPath() + "/mdp_" + start_sessiom_time + ".dat";
    QDir directory(QCoreApplication::applicationDirPath());
    QStringList images = directory.entryList(QStringList() << "*.dat", QDir::Files);
    foreach (QString fname, images)
    {
        if (fname.contains("mdp_"))
            filenames.append(fname);
    }

    if (filenames.length()==0)
    {
        QMessageBox msgBox;
        msgBox.setText("No recorded brain data found!");
        msgBox.exec();
    }
    else
    {
        ui->comboBox->addItems(filenames);
        readdata_fromfile(QCoreApplication::applicationDirPath() + "/"+ui->comboBox->itemText(0));
        ui->label_2->setText(transform(ui->comboBox->itemText(0))+"   seconds: "+QString::number((double)num_samples*recordrate,'f',1));
        recordsfound = true;
    }
}

// process changes of X-axis range (wheel zoom, drag..) and change both plots in exactly same way
void statistics::update_xaxis(QCPRange range, bool upper_plot)
{
    if (range.lower < 0)
    {
        if (range.upper-rightborder>0)
        {
            ui->plot->xAxis->setRange(0,range.upper);
            ui->plot_2->xAxis->setRange(0,range.upper);
        }
        else
        {
            ui->plot->xAxis->setRange(0,rightborder);
            ui->plot_2->xAxis->setRange(0,rightborder);
        }
    }
    if (range.upper > num_samples)
    {
        if (range.lower-leftborder<0)
        {
            ui->plot->xAxis->setRange(range.lower, num_samples);
            ui->plot_2->xAxis->setRange(range.lower, num_samples);
        }
        else
        {
            ui->plot->xAxis->setRange(leftborder, num_samples);
            ui->plot_2->xAxis->setRange(leftborder, num_samples);
        }
    }

    if (upper_plot)
    {
        leftborder = ui->plot_2->xAxis->range().lower;
        rightborder = ui->plot_2->xAxis->range().upper;
    } else
    {
        leftborder = ui->plot->xAxis->range().lower;
        rightborder = ui->plot->xAxis->range().upper;
    }
}

void statistics::xAxisChanged(QCPRange range)
{
    update_xaxis(range,false);

    ui->plot_2->xAxis->setRange(ui->plot->xAxis->range());
    ui->plot_2->replot();

    update_plots((int)ui->plot->xAxis->range().lower,num_samples-(int)ui->plot->xAxis->range().upper);
}

void statistics::xAxisChanged2(QCPRange range)
{
    update_xaxis(range,true);

    ui->plot->xAxis->setRange(ui->plot_2->xAxis->range());
    ui->plot->replot();

    update_plots((int)ui->plot->xAxis->range().lower,num_samples-(int)ui->plot->xAxis->range().upper);
}

void statistics::on_comboBox_currentIndexChanged(int index)
{
    readdata_fromfile(QCoreApplication::applicationDirPath() + "/"+ui->comboBox->itemText(index));
    ui->label_2->setText(transform(ui->comboBox->itemText(index))+"   seconds: "+QString::number((double)num_samples*recordrate,'f',1));
    update_plots(0,0);
}

void statistics::on_pushButton_clicked()
{
    close();
}

void statistics::on_checkBox_clicked()
{
    show_delta = !show_delta;
    update_plotlines();
}

void statistics::on_checkBox_2_clicked()
{
    show_theta = !show_theta;
    update_plotlines();
}

void statistics::on_checkBox_3_clicked()
{
    show_alpha = !show_alpha;
    update_plotlines();
}

void statistics::on_checkBox_4_clicked()
{
    show_beta = !show_beta;
    update_plotlines();
}

void statistics::on_checkBox_5_clicked()
{
    show_gamma = !show_gamma;
    update_plotlines();
}

void statistics::on_checkBox_6_clicked()
{
    show_hgamma = !show_hgamma;
    update_plotlines();
}

void statistics::on_checkBox_7_clicked()
{
    show_att = !show_att;
    update_plotlines();
}

void statistics::on_checkBox_8_clicked()
{
    show_medit = !show_medit;
    update_plotlines();
}

void statistics::on_checkBox_9_clicked()
{
    smoothplots = !smoothplots;

    if (smoothplots)
        for (int i=0; i<6; i++)
            ui->plot->graph(i)->setData(xcoords, filtbraindata[i+3]);    
    else    
        for (int i=0; i<6; i++)
            ui->plot->graph(i)->setData(xcoords, braindata[i+3]);    

    ui->plot->replot();
}
