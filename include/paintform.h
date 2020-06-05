/* header file for MindDraw window class -
   resposible for MindDraw parameters and actions */

#ifndef PAINTFORM_H
#define PAINTFORM_H

#include <QWidget>
#include <QGraphicsScene>
#include <paintscene.h>
#include "plotwindow.h"
#include "QInputEvent"
#include "myitem.h"
#include "set"

#define TPMAX 7300 // max points before replot for attention/meditation
#define TFMAX 2100 // max points before replot for frequencies

class paintScene;
class plotwindow;
class MainWindow;
class ocvcontrols;
class MyItem;

namespace Ui {
class paintform;
}

using namespace std;

class paintform : public QWidget
{
    Q_OBJECT

public:
    // description of most variables in paintform.cpp //

    explicit paintform(QWidget *parent = 0);
    MainWindow* mww;        // pointer on MainWindow object
    plotwindow* pw;         // pointer on MindPlay window object
    ocvcontrols* ocvfm;     // pointer on MindOCV controls object
    paintScene *scene;      // pointer on paintScene object (drawing functions)
    MyItem* movingItem;     // pointer on MyItem object (moving item in game)

    QGraphicsScene* scene2; QGraphicsScene* scene3; QGraphicsScene* scene4; QGraphicsScene* scene5;
    QGraphicsScene* scene6; QGraphicsScene* scene7; QGraphicsScene* scene8; QGraphicsScene* scene9;
    QGraphicsScene* scene10; QGraphicsScene* scene11; QGraphicsScene* scene12;
    QGraphicsScene* scene13; QGraphicsScene* scene14;  QGraphicsScene* scene15;
    QGraphicsScene* scenegame;

    int puzzlew, puzzleh, soundborderlevel, borderpicchange, setsize, picsrestored;
    int numsamples, numfrsamples, lenofinterval, laststop, pointsfor_estattention;
    int eegsize, pensize, temppensize, prevpict, prevpuzzle, moveItemInterval;
    int timepics, picsforchange, mainindex, polycount, gamethroughborder, y0border;
    double thet, bet, avgv, estattn;

    bool music_adaptive_bord, firstpuzzle, spacedflow, collectiveflow, grabmindplayflow;
    bool gamemode, flowmode, puzzlegrabed, canpuzzlechange, backloaded, showestatt;
    bool erasepen, bfiltmode, puzzlemode, fixedmain, changingpics, iconsready;
    bool attent_modulaion, limitpicschange, setloaded, musicactiv, gamethrough;

    QVector<double> attent_arr, medit_arr, border_arr, xc, fxc, estatt_arr;
    QVector<double> delta_arr, theta_arr, alpha_arr, beta_arr, gamma_arr, hgamma_arr;
    vector<int> puzzlelocs, randpuzzlelocs, sortpuzzlelocs, randnumb;
    QPixmap pm, pmg, pmain, prevpic, curpic, pmtemp, mainpic, pixMap; QImage qim;
    QFutureWatcher<QImage> *imageScaling;
    QVector<QPixmap> pmarray;
    QVector<QPixmap> onepicarr;
    QVector<QPoint> itemborders;
    QVector<QGraphicsLineItem*> borderlines, borderlinesnew;
    int* currentindexes;

    QPolygonF plane; int poltypearr[108]; int centercoord[108][2];  // polygons variables (experimental mode, not finished)
    QDir fd; QString folderpath; QStringList imglist; QVector<QString> currimglist;
    QPalette sp1,sp2, qpr;

    set<int> stpic0, stpic1, stres;
    set<int>::iterator iterst;                      
    QSet<int> pressedKeys;    

    QPen mypen;
    QTimer* tpicschange;
    QTimer* moveItemTimer;
    Qt::AspectRatioMode rationmode;

    bool eventFilter(QObject *target, QEvent *event);

    void configure_ui();
    void graphicsviews_init();

    void init_brainwaves_arrays();
    void delay(int temp);
    void getimg1();
    void getimg2();
    void updateattention(int t);
    void updateattentionplot(int t);
    void updatemeditation(int t);
    void setpicfolder(QString fpath);
    void randompics();
    void initpics();

    void setdflowtime(int t);
    void setflowspace(int t);
    bool getattentmode();
    void updatefreqarrs(double deltat, double thetat, double alphat, double betat, double gammat, double hgammat);

    QImage applyEffectToImage(QImage src, QGraphicsEffect *effect, int extent);
    void filtering_puzzle(QGraphicsView *gv, QPixmap pm, int grade);
    void filteringmain_ingame(int grade);
    void filtering_allpuzzles(int grade);
    void updateset_withlimitpics();
    void updateset_allpics();
    void updateset_singlepuzzle();
    void updateset_fillcorrectpuzzles();
    void updateset_fillcorrectpuzzles_single();

    void playsometone();
    void initborderlines();
    void updateborderlines(double scale);
    void startround();
    void newroundmovegame();
    void setactiveflowtime(int t);
    void setbackimage(QPixmap pm);
    void puzzle_onepic_switch();
    void grabpuzzles();
    void matchpuzzle();
    void loadempty();
    void updatemusicmode(bool fl);
    void setsoundtype(int index);
    double getestattval();
    void update_estrate(int t);
    void setbackimageocv(QString filename);   
    void adaptivespinrate(bool fl);
    void updateset_allpics_similarly(int* picnums);
    void swappuzzles(int t1, int t2);
    void mainpuzzle_update(int t);
    void updatepuzzles();

    ~paintform();

public slots:
    void addScaledImage(int num);
    void scalingFinished();

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

    void on_pushButton_12_clicked();

    void on_spinBox_7_valueChanged(int arg1);

    void on_checkBox_22_clicked();

    void on_checkBox_23_clicked();

private:
    Ui::paintform *ui;
    QPointF previousPoint;

};

#endif // PAINTFORM_H
