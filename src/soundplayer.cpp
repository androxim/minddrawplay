/* source file for sounplayer class -
   resposible for sound samples configuration and playing in separate thread */

#include "soundplayer.h"

void soundplayer::setvolume(qreal volume)
{
    D3.setVolume(volume);   
    A3.setVolume(volume);
    C4.setVolume(volume);
    A4.setVolume(volume);
    D4.setVolume(volume);
    E4.setVolume(volume);
    F3.setVolume(volume);
    F4.setVolume(volume);

    tone_Blow.setVolume(volume);
    tone_b.setVolume(volume);
    tone_Dlowsh.setVolume(volume);
    tone_dsh.setVolume(volume);
    tone_Glowsh.setVolume(volume);
    tone_gsh.setVolume(volume);
    tone_Flowsh.setVolume(volume);
    tone_fsh.setVolume(volume);
    tone_Clowsh.setVolume(volume);
    tone_csh.setVolume(volume);

    tank_Asharp.setVolume(volume);
    tank_bhigh.setVolume(volume);
    tank_c_highsharp.setVolume(volume);
    tank_d_highsharp.setVolume(volume);
    tank_Dsharp.setVolume(volume);
    tank_F.setVolume(volume);
    tank_f_high.setVolume(volume);
    tank_f_highsharp.setVolume(volume);
    tank_Fsharp.setVolume(volume);
    tank_Gsharp.setVolume(volume);
}

void soundplayer::init()
{ 
    D3.moveToThread(&m_thread);
    A3.moveToThread(&m_thread);
    C4.moveToThread(&m_thread);
    A4.moveToThread(&m_thread);
    D4.moveToThread(&m_thread);
    E4.moveToThread(&m_thread);
    F3.moveToThread(&m_thread);
    F4.moveToThread(&m_thread);

    tone_Blow.moveToThread(&m_thread);
    tone_b.moveToThread(&m_thread);
    tone_Dlowsh.moveToThread(&m_thread);
    tone_dsh.moveToThread(&m_thread);
    tone_Glowsh.moveToThread(&m_thread);
    tone_gsh.moveToThread(&m_thread);
    tone_Flowsh.moveToThread(&m_thread);
    tone_fsh.moveToThread(&m_thread);
    tone_Clowsh.moveToThread(&m_thread);
    tone_csh.moveToThread(&m_thread);

    tank_Asharp.moveToThread(&m_thread);
    tank_bhigh.moveToThread(&m_thread);
    tank_c_highsharp.moveToThread(&m_thread);
    tank_d_highsharp.moveToThread(&m_thread);
    tank_Dsharp.moveToThread(&m_thread);
    tank_F.moveToThread(&m_thread);
    tank_f_high.moveToThread(&m_thread);
    tank_f_highsharp.moveToThread(&m_thread);
    tank_Fsharp.moveToThread(&m_thread);
    tank_Gsharp.moveToThread(&m_thread);

    connect(this, SIGNAL(playD3()), &D3, SLOT(play()));
    connect(this, SIGNAL(playA3()), &A3, SLOT(play()));
    connect(this, SIGNAL(playC4()), &C4, SLOT(play()));
    connect(this, SIGNAL(playA4()), &A4, SLOT(play()));
    connect(this, SIGNAL(playD4()), &D4, SLOT(play()));
    connect(this, SIGNAL(playE4()), &E4, SLOT(play()));
    connect(this, SIGNAL(playF4()), &F4, SLOT(play()));
    connect(this, SIGNAL(playF3()), &F3, SLOT(play()));

    connect(this, SIGNAL(playtone_Blow()), &tone_Blow, SLOT(play()));
    connect(this, SIGNAL(playtone_b()), &tone_b, SLOT(play()));
    connect(this, SIGNAL(playtone_Dlowsh()), &tone_Dlowsh, SLOT(play()));
    connect(this, SIGNAL(playtone_dsh()), &tone_dsh, SLOT(play()));
    connect(this, SIGNAL(playtone_Glowsh()), &tone_Glowsh, SLOT(play()));
    connect(this, SIGNAL(playtone_gsh()), &tone_gsh, SLOT(play()));
    connect(this, SIGNAL(playtone_Flowsh()), &tone_Flowsh, SLOT(play()));
    connect(this, SIGNAL(playtone_fsh()), &tone_fsh, SLOT(play()));
    connect(this, SIGNAL(playtone_Clowsh()), &tone_Clowsh, SLOT(play()));
    connect(this, SIGNAL(playtone_csh()), &tone_csh, SLOT(play()));

    connect(this, SIGNAL(play_Asharp()), &tank_Asharp, SLOT(play()));
    connect(this, SIGNAL(play_bhigh()), &tank_bhigh, SLOT(play()));
    connect(this, SIGNAL(play_c_highsharp()), &tank_c_highsharp, SLOT(play()));
    connect(this, SIGNAL(play_d_highsharp()), &tank_d_highsharp, SLOT(play()));
    connect(this, SIGNAL(play_Dsharp()), &tank_Dsharp, SLOT(play()));
    connect(this, SIGNAL(play_F()), &tank_F, SLOT(play()));
    connect(this, SIGNAL(play_f_high()), &tank_f_high, SLOT(play()));
    connect(this, SIGNAL(play_f_highsharp()), &tank_f_highsharp, SLOT(play()));
    connect(this, SIGNAL(play_Fsharp()), &tank_Fsharp, SLOT(play()));
    connect(this, SIGNAL(play_Gsharp()), &tank_Gsharp, SLOT(play()));

    connect(this, SIGNAL(stopThread()), &m_thread, SLOT(terminate()));

   // connect(this, SIGNAL(setvolume(int)), this, SLOT(setvolume(int)));    
    m_thread.start(); // QThread::exec() will be called for you, making the thread wait for events
}

soundplayer::soundplayer(QObject *parent) : QObject(parent)
{    
    D3.setSource(QUrl::fromLocalFile(":/sounds/sounds/D3_.wav")); 
    A3.setSource(QUrl::fromLocalFile(":/sounds/sounds/A3_.wav"));    
    C4.setSource(QUrl::fromLocalFile(":/sounds/sounds/C4_.wav"));    
    A4.setSource(QUrl::fromLocalFile(":/sounds/sounds/A4_.wav"));    
    D4.setSource(QUrl::fromLocalFile(":/sounds/sounds/D4_.wav"));    
    E4.setSource(QUrl::fromLocalFile(":/sounds/sounds/E4_.wav"));    
    F3.setSource(QUrl::fromLocalFile(":/sounds/sounds/F3_.wav"));    
    F4.setSource(QUrl::fromLocalFile(":/sounds/sounds/F4_.wav"));

    tone_Blow.setSource(QUrl::fromLocalFile(":/sounds/sounds/tone_B.wav"));
    tone_b.setSource(QUrl::fromLocalFile(":/sounds/sounds/tone_b_.wav"));
    tone_Dlowsh.setSource(QUrl::fromLocalFile(":/sounds/sounds/tone_Dsh.wav"));
    tone_dsh.setSource(QUrl::fromLocalFile(":/sounds/sounds/tone_dsh_.wav"));
    tone_Glowsh.setSource(QUrl::fromLocalFile(":/sounds/sounds/tone_Gsh.wav"));
    tone_gsh.setSource(QUrl::fromLocalFile(":/sounds/sounds/tone_gsh_.wav"));
    tone_Flowsh.setSource(QUrl::fromLocalFile(":/sounds/sounds/tone_Fsh.wav"));
    tone_fsh.setSource(QUrl::fromLocalFile(":/sounds/sounds/tone_fsh_.wav"));
    tone_Clowsh.setSource(QUrl::fromLocalFile(":/sounds/sounds/tone_Csh.wav"));
    tone_csh.setSource(QUrl::fromLocalFile(":/sounds/sounds/tone_csh_.wav"));    

    tank_Asharp.setSource(QUrl::fromLocalFile(":/sounds/sounds/tank_Asharp.wav"));
    tank_bhigh.setSource(QUrl::fromLocalFile(":/sounds/sounds/tank_bhigh.wav"));
    tank_c_highsharp.setSource(QUrl::fromLocalFile(":/sounds/sounds/tank_c_highsharp.wav"));
    tank_d_highsharp.setSource(QUrl::fromLocalFile(":/sounds/sounds/tank_d_highsharp.wav"));
    tank_Dsharp.setSource(QUrl::fromLocalFile(":/sounds/sounds/tank_Dsharp.wav"));
    tank_F.setSource(QUrl::fromLocalFile(":/sounds/sounds/tank_F.wav"));
    tank_f_high.setSource(QUrl::fromLocalFile(":/sounds/sounds/tank_f_high.wav"));
    tank_f_highsharp.setSource(QUrl::fromLocalFile(":/sounds/sounds/tank_f_highsharp.wav"));
    tank_Fsharp.setSource(QUrl::fromLocalFile(":/sounds/sounds/tank_Fsharp.wav"));
    tank_Gsharp.setSource(QUrl::fromLocalFile(":/sounds/sounds/tank_Gsharp.wav"));
}
