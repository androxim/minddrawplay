#include "statistics.h"
#include "ui_statistics.h"

statistics::statistics(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::statistics)
{
    ui->setupUi(this);

    num_samples = 0;
    for (int i=0; i<9; i++)
        braindata[i] = QVector<int>();

    bars_x = QVector<double>();
    bars_y = QVector<double>();

    recordsfound = false;

    initplots();
}

statistics::~statistics()
{
    delete ui;
}

void statistics::initplots()
{    
   //
}

void statistics::readdata_fromfile(QString fname)
{
    for (int i=0; i<9; i++)
        braindata[i].clear();
    num_samples = 0;

    QFile inputFile(fname);
    inputFile.open(QIODevice::ReadOnly);
    QTextStream fin(&inputFile);
    QStringList sl;
    fin.readLine();
    fin.readLine();
    QString line = fin.readLine();
    while (!fin.atEnd())
    {
        sl = line.split(QRegExp("\\,"), QString::SkipEmptyParts);
        if (sl.length()>1)
        {
            for (int i=0; i<9; i++)
                braindata[i].append(sl[i].toInt());
            num_samples++;
            line = fin.readLine();
        }
    }
    inputFile.close();
}

void statistics::fill_bars_vals(int minv, int maxv, int index)
{
    bars_x.clear();
    bars_y.clear();
    for (int i=minv; i<=maxv; i++)
    {
      bars_x.append(i);
      bars_y.append(0);
    }

    for (int i=0; i<num_samples; i++)
        for (int j=minv; j<=maxv; j++)
            if ((braindata[index][i]>=j) && (braindata[index][i]<(j+1)))
                bars_y[j-minv]+=1;

    max_bars_val = *std::max_element(bars_y.begin(), bars_y.end());
}

void statistics::update_plots()
{
    fill_bars_vals(0,100,1);
    ui->attention_hist->clearGraphs();
    QCPBars *bars1 = new QCPBars(ui->attention_hist->xAxis, ui->attention_hist->yAxis);
    bars1->setWidth(1);   
    bars1->setData(bars_x, bars_y);
    bars1->setPen(Qt::NoPen);
    bars1->setBrush(Qt::red);
    ui->attention_hist->rescaleAxes();
    ui->attention_hist->yAxis->setRange(0, max_bars_val);
    ui->attention_hist->xAxis->setLabel("%");
    ui->attention_hist->replot();

    fill_bars_vals(0,100,2);
    QCPBars *bars2 = new QCPBars(ui->meditation_hist->xAxis, ui->meditation_hist->yAxis);
    bars2->setWidth(1);
    bars2->setData(bars_x, bars_y);
    bars2->setPen(Qt::NoPen);
    bars2->setBrush(Qt::green);
    ui->meditation_hist->rescaleAxes();
    ui->meditation_hist->yAxis->setRange(0, max_bars_val);
    ui->meditation_hist->xAxis->setLabel("%");
    ui->meditation_hist->replot();

    int min_val = *std::min_element(braindata[3].begin(), braindata[3].end());
    int max_val = *std::max_element(braindata[3].begin(), braindata[3].end());
    fill_bars_vals(min_val,max_val,3);
    QCPBars *bars3 = new QCPBars(ui->delta_hist->xAxis, ui->delta_hist->yAxis);
    bars3->setWidth(1);
    bars3->setData(bars_x, bars_y);
    bars3->setPen(Qt::NoPen);
    bars3->setBrush(Qt::yellow);
    ui->delta_hist->rescaleAxes();
    ui->delta_hist->yAxis->setRange(0, max_bars_val);
    ui->delta_hist->xAxis->setLabel("%");
    ui->delta_hist->replot();

    min_val = *std::min_element(braindata[4].begin(), braindata[4].end());
    max_val = *std::max_element(braindata[4].begin(), braindata[4].end());
    fill_bars_vals(min_val,max_val,4);
    QCPBars *bars4 = new QCPBars(ui->theta_hist->xAxis, ui->theta_hist->yAxis);
    bars4->setWidth(1);
    bars4->setData(bars_x, bars_y);
    bars4->setPen(Qt::NoPen);
    bars4->setBrush(Qt::cyan);
    ui->theta_hist->rescaleAxes();
    ui->theta_hist->yAxis->setRange(0, max_bars_val);
    ui->theta_hist->xAxis->setLabel("%");
    ui->theta_hist->replot();

    min_val = *std::min_element(braindata[5].begin(), braindata[5].end());
    max_val = *std::max_element(braindata[5].begin(), braindata[5].end());    
    fill_bars_vals(min_val,max_val,5);
    QCPBars *bars5 = new QCPBars(ui->alpha_hist->xAxis, ui->alpha_hist->yAxis);    
    bars5->setWidth(1);    
    bars5->setData(bars_x, bars_y);
    bars5->setPen(Qt::NoPen);
    bars5->setBrush(Qt::magenta);
    ui->alpha_hist->rescaleAxes();
    ui->alpha_hist->yAxis->setRange(0, max_bars_val);
    ui->alpha_hist->xAxis->setLabel("%");
    ui->alpha_hist->replot();

    min_val = *std::min_element(braindata[6].begin(), braindata[6].end());
    max_val = *std::max_element(braindata[6].begin(), braindata[6].end());
    fill_bars_vals(min_val,max_val,6);    
    QCPBars *bars6 = new QCPBars(ui->beta_hist->xAxis, ui->beta_hist->yAxis);
    bars6->setWidth(1);
    bars6->setData(bars_x, bars_y);
    bars6->setPen(Qt::NoPen);
    bars6->setBrush(Qt::blue);
    ui->beta_hist->rescaleAxes();
    ui->beta_hist->yAxis->setRange(0, max_bars_val);
    ui->beta_hist->xAxis->setLabel("%");
    ui->beta_hist->replot();

    min_val = *std::min_element(braindata[7].begin(), braindata[7].end());
    max_val = *std::max_element(braindata[7].begin(), braindata[7].end());
    fill_bars_vals(min_val,max_val,7);
    QCPBars *bars7 = new QCPBars(ui->gamma_hist->xAxis, ui->gamma_hist->yAxis);
    bars7->setWidth(1);
    bars7->setData(bars_x, bars_y);
    bars7->setPen(Qt::NoPen);
    bars7->setBrush(Qt::gray);
    ui->gamma_hist->rescaleAxes();
    ui->gamma_hist->yAxis->setRange(0, max_bars_val);
    ui->gamma_hist->xAxis->setLabel("%");
    ui->gamma_hist->replot();

    min_val = *std::min_element(braindata[8].begin(), braindata[8].end());
    max_val = *std::max_element(braindata[8].begin(), braindata[8].end());
    fill_bars_vals(min_val,max_val,8);
    QCPBars *bars8 = new QCPBars(ui->hgamma_hist->xAxis, ui->hgamma_hist->yAxis);
    bars8->setWidth(1);
    bars8->setData(bars_x, bars_y);
    bars8->setPen(Qt::NoPen);
    bars8->setBrush(Qt::darkGray);
    ui->hgamma_hist->rescaleAxes();
    ui->hgamma_hist->yAxis->setRange(0, max_bars_val);
    ui->hgamma_hist->xAxis->setLabel("%");
    ui->hgamma_hist->replot();
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
        ui->label_2->setText(transform(ui->comboBox->itemText(0))+"   seconds: "+QString::number((double)num_samples/5,'f',1));
        recordsfound = true;
    }
}

void statistics::on_pushButton_clicked()
{
    close();
}

void statistics::on_comboBox_currentIndexChanged(int index)
{
    readdata_fromfile(QCoreApplication::applicationDirPath() + "/"+ui->comboBox->itemText(index));
    ui->label_2->setText(transform(ui->comboBox->itemText(index))+"   seconds: "+QString::number((double)num_samples/5,'f',1));
    ui->attention_hist->clearPlottables();
    ui->meditation_hist->clearPlottables();
    ui->delta_hist->clearPlottables();
    ui->theta_hist->clearPlottables();
    ui->alpha_hist->clearPlottables();
    ui->beta_hist->clearPlottables();
    ui->gamma_hist->clearPlottables();
    ui->hgamma_hist->clearPlottables();
    update_plots();
}
