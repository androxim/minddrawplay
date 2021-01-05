/* header file for openglwin class -
   resposible for MindOGL flow parameters and actions */

#ifndef OPENGLWIN_H
#define OPENGLWIN_H

#include <QOpenGLWidget>
#include <QSurfaceFormat>
#include <QTimer>
#include <QKeyEvent>
#include "objloader.h"
#include "mainwindow.h"

class MainWindow;

class openglwin : public QOpenGLWidget
{
    Q_OBJECT

    QTimer* paintTimer;
    GLubyte texture_count = 0;  // current texture number
    GLubyte model_count = 0;    // current model number
    GLuint model[3];            // number of display list for current model
    GLuint texture[3]; 
    GLuint torus;
    GLfloat angle, scale, prevscale;

    void initLight();
    void LoadGLTextures();
    void initTexture(uint index, QImage &texture1);
    int loadObject(const QString &filename);
    virtual void keyPressEvent(QKeyEvent *event);

public:

    explicit openglwin(QWidget *parent = 0);

    MainWindow* mww;

    float angleinc, scaleinc, wintransplevel;

    bool hideobj, transp_by_att;

    void set_angle_scale_incs(float angle_t, float scale_t);
    void set_window_transp(float val);
    void startflow(int t);
    void changeSpaceTexture();

protected:
   void initializeGL();
   void resizeGL(int nWidth, int nHeight);
   void paintGL();
};
#endif // OPENGLWIN_H


