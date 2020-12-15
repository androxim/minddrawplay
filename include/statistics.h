/* header file for statistics class -
   resposible for data and figures configuration */


#ifndef STATISTICS_H
#define STATISTICS_H

#include <QWidget>
#include "qcustomplot.h"
#include "QtCharts"
#include "filters.h"

class filters;
class MainWindow;

namespace Ui {
class statistics;
}

class statistics : public QWidget
{
    Q_OBJECT

public:
    int num_samples, max_bars_val, leftborder, rightborder;
    double mean_theta, mean_delta, mean_alpha, mean_beta, mean_gamma, mean_hgamma, mean_att, mean_medit;
    bool recordsfound, show_theta, show_delta, show_alpha, show_beta, show_gamma, show_hgamma, show_att, show_medit;
    double recordrate;
    bool smoothplots;

    QVector<double> braindata[9];
    QVector<double> filtbraindata[9];
    QVector<double> xcoords;
    QVector<double> bars_x, bars_y;
    QStringList filenames;
    QChart *chart; QPieSeries *series;
    QPieSlice* slice1; QPieSlice* slice2; QPieSlice* slice3;
    QPieSlice* slice4; QPieSlice* slice5; QPieSlice* slice6;
    QChart *chart2; QBarSet *att; QBarSet *medit; QBarSeries *barseries;
    filters *plotsfilt;
    MainWindow *mww;

    explicit statistics(QWidget *parent = 0);
    ~statistics();
    void initplots();
    void readdata_fromfile(QString fname);
    void fill_bars_vals(int minv, int maxv, int index, int left_offset, int right_offset);
    void update_plots(int left_offset, int right_offset);
    void update_filenames();
    void update_xaxis(QCPRange range, bool upper_plot);
    void update_charts_and_levels();
    void update_plotlines();
    bool eventFilter(QObject *target, QEvent *event);

private slots:

    void xAxisChanged(QCPRange range);
    void xAxisChanged2(QCPRange range);

    void on_pushButton_clicked();

    void on_comboBox_currentIndexChanged(int index);

    void on_checkBox_clicked();

    void on_checkBox_2_clicked();

    void on_checkBox_3_clicked();

    void on_checkBox_4_clicked();

    void on_checkBox_5_clicked();

    void on_checkBox_6_clicked();

    void on_checkBox_7_clicked();

    void on_checkBox_8_clicked();

    void on_checkBox_9_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::statistics *ui;
};

#endif // STATISTICS_H
