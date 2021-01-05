/* source file for openglwin class -
   resposible for MindOGL flow parameters and actions */

#include "openglwin.h"
#include <GL/glu.h>

openglwin::openglwin(QWidget *parent) : QOpenGLWidget(parent)
{
    resize(1024,720);

    angle = 0;                  // rotation angle for textures
    scale = 1;                  // scale of object
    wintransplevel = 0.95;      // transparency of window
    angleinc = 0.3f;            // increment for rotation of textures
    scaleinc = 0.05f;           // increment for scale changes

    setWindowOpacity(wintransplevel);

    paintTimer = new QTimer(this);
    connect(paintTimer, SIGNAL(timeout()), this, SLOT(repaint()));

    hideobj = false;            // hide object
    transp_by_att = false;      // transparency of window by attention
}

void openglwin::startflow(int t)
{
    paintTimer->start(t);
}

void openglwin::set_angle_scale_incs(float angle_t, float scale_t)
{
    angleinc = angle_t;
    if (!hideobj)
        scale = scale_t;
}

void openglwin::set_window_transp(float val)
{
    wintransplevel = val;
    setWindowOpacity(wintransplevel);
}

void openglwin::initTexture(uint index, QImage &texture1)
{
    texture1.convertTo(QImage::Format_RGBA8888);
    glBindTexture(GL_TEXTURE_2D, texture[index]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, GLsizei(texture1.width()), GLsizei(texture1.height()), 0, GL_RGBA, GL_UNSIGNED_BYTE, texture1.bits());
}

void openglwin::LoadGLTextures()
{
    glGenTextures(3, texture);

    QImage texture1;
    texture1.load(":/pics/pics/bricks.jpg");
    initTexture(0, texture1);

    texture1.load(":/pics/pics/globetexture.jpg");
    initTexture(1, texture1);

    texture1.load(":/pics/pics/psytexture.jpg");
    initTexture(2, texture1);
}

void openglwin::changeSpaceTexture()
{
    QImage texture;
    int nt = qrand() % mww->imgpaths.size();
    QString texture_path = mww->getfolderpath()+"/"+mww->imgpaths[nt];
    texture.load(texture_path);
    initTexture(2, texture);
}

void openglwin::keyPressEvent(QKeyEvent *event)
{
    if (event->key()==Qt::Key_F)
        ++texture_count%=3;

    if (event->key()==Qt::Key_S)
    {
        if (paintTimer->isActive())
            paintTimer->stop();
        else paintTimer->start();
    }
    if (event->key()==Qt::Key_M)
    {
        // ++model_count%=2;
        QApplication::setActiveWindow(mww);
    }
    if (event->key()==Qt::Key_R)
    {
        angleinc += 0.1f;
      //  qDebug()<<angleinc;
    }
    if (event->key()==Qt::Key_T)
    {
        angleinc -= 0.1f;
      //  qDebug()<<angleinc;
    }
    if (event->key()==Qt::Key_Y)
    {
        scale += scaleinc;
      //  qDebug()<<scale;
    }
    if (event->key()==Qt::Key_U)
    {
        scale -= scaleinc;
       // qDebug()<<scale;
    }
    if (event->key()==Qt::Key_C)
        transp_by_att = !transp_by_att;


    if (event->key()==Qt::Key_H)
    {
        hideobj = !hideobj;
        if (hideobj)
        {
            prevscale = scale;
            scale = 0;
        }
        else
            scale = prevscale;
    }

    if (event->key()==Qt::Key_Space)
        changeSpaceTexture();

    if ((event->key()==Qt::Key_Z) && (wintransplevel>0.05))
    {
        wintransplevel -= 0.05;
        setWindowOpacity(wintransplevel);
    }

    if ((event->key()==Qt::Key_X) && (wintransplevel<1))
    {
        wintransplevel += 0.05;
        setWindowOpacity(wintransplevel);
    }
}

void openglwin::initLight()
{
    GLfloat light_ambient[] = { 0, 0, 0, 0.0 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 2.0 };
    GLfloat light_position[] = { 0.0, 2.0, 2.0, 1.0 };

    glLightfv (GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv (GL_LIGHT0, GL_POSITION, light_position);

    glEnable (GL_LIGHTING);
    glEnable (GL_LIGHT0);
}

void openglwin::initializeGL()
{
    glEnable(GL_MULTISAMPLE);
    LoadGLTextures();
    glEnable(GL_TEXTURE_2D);
    glClearColor(1,1,1,1);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    initLight();
    model[0] = objloader::Instance().load(":/objs/objs/monkey.obj");
    torus = objloader::Instance().load(":/objs/objs/torus.obj");
}

void openglwin::resizeGL(int nWidth, int nHeight)
{
    glViewport(0, 0, nWidth, nHeight);
    qreal aspectratio = qreal(nWidth) / qreal(nHeight);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( 90.0, aspectratio, 0.1, 100.0 );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void openglwin::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glTranslatef(0,0,-2.5);
    glDeleteLists(torus,1);
    torus = objloader::Instance().draw(angle/100);
    glCallList(torus);
    glTranslatef(0,0,0.1f);
    glRotatef(angle,0.0f,1.0f,0.0f);
    glScalef(scale,scale,scale);
    glBindTexture(GL_TEXTURE_2D, texture[texture_count]);
    glCallList(model[model_count]);

    angle += angleinc;
}


