#ifndef PAINTFORM_H
#define PAINTFORM_H

#include <QWidget>
#include <QGraphicsScene>
#include <paintscene.h>
#include "plotwindow.h"
#include "QInputEvent"
#include "set"

#define TPMAX 10000
#define TFMAX 5000

class paintScene;
class plotwindow;
class MainWindow;

namespace Ui {
class paintform;
}

class paintform : public QWidget
{
    Q_OBJECT

public:
    explicit paintform(QWidget *parent = 0);
    paintScene *scene;
    plotwindow *pw;
    MainWindow *mww;
    QPolygonF plane;
    int poltypearr[108];
    int centercoord[108][2];
    QDir fd;
    QPalette sp1,sp2;
    QString folderpath;
    bool adaptivebord, firstpuzzle, spacedflow;
    int activatedcell;
    double thet;
    double bet;
    int pt;
    double avgv;
    QPixmap pmg;
    bool collectiveflow, updateback;
    int picsrestored;
    double estattn;
    set<int> stpic0, stpic1, stres;
    set<int>::iterator iterst;
    vector<int> puzzlelocs, randpuzzlelocs, sortpuzzlelocs;
    QString fnameattent,fnamefreq;
    int puzzlew, puzzleh, borderlevel, borderpicchange, setsize;
    int numsamples, numfrsamples, lenofinterval, laststop;
    QVector<double> attent_arr, medit_arr, border_arr, xc, fxc, estatt_arr, delta_arr, theta_arr, alpha_arr, beta_arr, gamma_arr, hgamma_arr;
    QStringList imglist;
    QVector<QString> currimglist;
    QSet<int> pressedKeys;
    QPalette qpr;
    QPixmap pm, pmain, prevpic, curpic, pmtemp, mainpic;
    QVector<QPixmap> pmarray;
    QVector<QPixmap> onepicarr;
    QImage qim;
    QPen mypen;
    QTimer* tpicschange;
    QTimer* polyt;
    Qt::AspectRatioMode rationmode;
    int timepics, picsforchange, mainindex,polycount;
    int* currentindexes;
    bool gamemode, flowmode, puzzlegrabed, updatingpuzzle, canpuzzlechange, backloaded, showestatt;
    vector<int> randnumb;
    int eegsize, pensize, temppensize;
    int prevpict, prevpuzzle;
    bool erasepen, qimload, bfiltmode, puzzlemode, fixedmain, changingpics, minimode, attentmodu, limitpicschange, setloaded, musicactiv;
    bool eventFilter(QObject *target, QEvent *event);
    void delay(int temp);
    void getimg1();
    void getimg2();
    void updateattention(int t);
    void updateattentionplot(int t);
    void updatemeditation(int t);
    void randompics();
    void initpics();
    void setdflowtime(int t);
    void setflowspace(int t);
    bool getattentmode();
    void updatefreqarrs(double deltat, double thetat, double alphat, double betat, double gammat, double hgammat);
    void wheelEvent(QWheelEvent *event);
    void updateplots(bool fl);
    QImage applyEffectToImage(QImage src, QGraphicsEffect *effect, int extent);
    void filtering_puzzle(QGraphicsView *gv, QPixmap pm, int grade);
    void filteringmain_ingame(int grade);
    void filtering_allpuzzles(int grade);
    void startround();
    void setactiveflowtime(int t);
    void setbackimage(QPixmap pm);
    void grabpuzzles();
    void matchpuzzle();
    void startpolyt();
    void loadempty();
    void setsoundtype(int index);
    double getestattval();
    void setbackimageocv(QString filename);
    int getgraphicview_width();
    int getgraphicview_height();
    ~paintform();


private slots:

    void tpicschangeUpdate();

    void on_pushButton_clicked();

    void on_checkBox_clicked();

    void on_pushButton_2_clicked();

    void on_spinBox_valueChanged(int arg1);

    void on_spinBox_2_valueChanged(int arg1);

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_checkBox_2_clicked();

    void on_radioButton_clicked();

    void on_radioButton_2_clicked();

    void on_radioButton_3_clicked();

    void on_pushButton_5_clicked();

    void on_checkBox_3_clicked();

    void on_checkBox_4_clicked();

    void on_checkBox_5_clicked();

    void on_checkBox_6_clicked();

    void on_pushButton_6_clicked();

    void on_checkBox_7_clicked();

    void on_checkBox_8_clicked();

    void on_pushButton_7_clicked();

    void on_checkBox_9_clicked();

    void on_spinBox_3_valueChanged(int arg1);

    void on_checkBox_10_clicked();

    void on_pushButton_8_clicked();


    void on_radioButton_4_clicked();

    void on_radioButton_5_clicked();

    void on_checkBox_11_clicked();

    void on_spinBox_4_valueChanged(int arg1);

    void on_comboBox_currentIndexChanged(int index);

    void on_checkBox_12_clicked();

    void on_checkBox_13_clicked();

    void on_checkBox_14_clicked();

    void on_radioButton_6_clicked();

    void on_checkBox_15_clicked();

    void on_spinBox_5_valueChanged(int arg1);

    void on_verticalSlider_2_sliderMoved(int position);

    void on_comboBox_2_currentIndexChanged(int index);

    void on_pushButton_9_clicked();

    void on_spinBox_6_valueChanged(int arg1);

    void on_checkBox_16_clicked();

    void on_checkBox_17_clicked();

    void on_pushButton_10_clicked();

    void on_checkBox_19_clicked();

    void on_checkBox_20_clicked();

    void on_checkBox_21_clicked();

    void on_checkBox_18_clicked();

    void on_pushButton_11_clicked();

private:
    Ui::paintform *ui;
    QPointF previousPoint;

    void resizeEvent(QResizeEvent *);
    void showEvent(QShowEvent *);
    void fitView();

};

#endif // PAINTFORM_H
