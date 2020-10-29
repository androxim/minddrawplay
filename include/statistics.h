#ifndef STATISTICS_H
#define STATISTICS_H

#include <QWidget>
#include "qcustomplot.h"

namespace Ui {
class statistics;
}

class statistics : public QWidget
{
    Q_OBJECT

public:
    int num_samples, max_bars_val, label_shift;
    bool recordsfound;

    QVector<int> braindata[9];
    QVector<double> bars_x, bars_y;
    QStringList filenames;


    explicit statistics(QWidget *parent = 0);
    ~statistics();
    void initplots();
    void readdata_fromfile(QString fname);
    void fill_bars_vals(int minv, int maxv, int index);
    void update_plots();
    void update_filenames();

private slots:
    void on_pushButton_clicked();

    void on_comboBox_currentIndexChanged(int index);

private:
    Ui::statistics *ui;
};

#endif // STATISTICS_H
