#include "soundplayer.h"

void soundplayer::setvolume(qreal volume)
{
    D3.setVolume(volume);
    Glow.setVolume(volume);
    A3.setVolume(volume);
    fdiez.setVolume(volume);
    C4.setVolume(volume);
    Clow.setVolume(volume);
    A4.setVolume(volume);
    atone.setVolume(volume);
    D4.setVolume(volume);
    Elow.setVolume(volume);
    E4.setVolume(volume);
    dtone.setVolume(volume);
    F3.setVolume(volume);
    Dlow.setVolume(volume);
    btone.setVolume(volume);
    F4.setVolume(volume);
    Blow.setVolume(volume);
    gtone.setVolume(volume);
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
}

void soundplayer::init()
{
    Glow.moveToThread(&m_thread);
    D3.moveToThread(&m_thread);
    fdiez.moveToThread(&m_thread);
    A3.moveToThread(&m_thread);
    Clow.moveToThread(&m_thread);
    C4.moveToThread(&m_thread);
    atone.moveToThread(&m_thread);
    A4.moveToThread(&m_thread);
    Elow.moveToThread(&m_thread);
    D4.moveToThread(&m_thread);
    dtone.moveToThread(&m_thread);
    E4.moveToThread(&m_thread);
    Dlow.moveToThread(&m_thread);
    F3.moveToThread(&m_thread);
    btone.moveToThread(&m_thread);
    Blow.moveToThread(&m_thread);
    F4.moveToThread(&m_thread);
    gtone.moveToThread(&m_thread);

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

    connect(this, SIGNAL(playGlow()), &Glow, SLOT(play()));
    connect(this, SIGNAL(playD3()), &D3, SLOT(play()));
    connect(this, SIGNAL(playfdiez()), &fdiez, SLOT(play()));
    connect(this, SIGNAL(playA3()), &A3, SLOT(play()));
    connect(this, SIGNAL(playClow()), &Clow, SLOT(play()));
    connect(this, SIGNAL(playC4()), &C4, SLOT(play()));
    connect(this, SIGNAL(playatone()), &atone, SLOT(play()));
    connect(this, SIGNAL(playA4()), &A4, SLOT(play()));
    connect(this, SIGNAL(playElow()), &Elow, SLOT(play()));
    connect(this, SIGNAL(playD4()), &D4, SLOT(play()));
    connect(this, SIGNAL(playdtone()), &dtone, SLOT(play()));
    connect(this, SIGNAL(playE4()), &E4, SLOT(play()));
    connect(this, SIGNAL(playDlow()), &Dlow, SLOT(play()));
    connect(this, SIGNAL(playF4()), &F4, SLOT(play()));
    connect(this, SIGNAL(playgtone()), &gtone, SLOT(play()));
    connect(this, SIGNAL(playF3()), &F3, SLOT(play()));
    connect(this, SIGNAL(playbtone()), &btone, SLOT(play()));
    connect(this, SIGNAL(playBlow()), &Blow, SLOT(play()));

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

   // connect(this, SIGNAL(setvolume(int)), this, SLOT(setvolume(int)));

    m_thread.start(); // QThread::exec() will be called for you, making the thread wait for events
}

soundplayer::soundplayer(QObject *parent) : QObject(parent)
{
    Glow.setSource(QUrl::fromLocalFile(":/sounds/sounds/tank_Glow_.wav"));
    D3.setSource(QUrl::fromLocalFile(":/sounds/sounds/D3_.wav"));
    fdiez.setSource(QUrl::fromLocalFile(":/sounds/sounds/tank_fdiez_.wav"));
    A3.setSource(QUrl::fromLocalFile(":/sounds/sounds/A3_.wav"));
    Clow.setSource(QUrl::fromLocalFile(":/sounds/sounds/tank_Clow_.wav"));
    C4.setSource(QUrl::fromLocalFile(":/sounds/sounds/C4_.wav"));
    atone.setSource(QUrl::fromLocalFile(":/sounds/sounds/tank_a_.wav"));
    A4.setSource(QUrl::fromLocalFile(":/sounds/sounds/A4_.wav"));
    Elow.setSource(QUrl::fromLocalFile(":/sounds/sounds/tank_Elow_.wav"));
    D4.setSource(QUrl::fromLocalFile(":/sounds/sounds/D4_.wav"));
    dtone.setSource(QUrl::fromLocalFile(":/sounds/sounds/tank_d_.wav"));
    E4.setSource(QUrl::fromLocalFile(":/sounds/sounds/E4_.wav"));
    Dlow.setSource(QUrl::fromLocalFile(":/sounds/sounds/tank_Dlow_.wav"));
    F3.setSource(QUrl::fromLocalFile(":/sounds/sounds/F3_.wav"));
    btone.setSource(QUrl::fromLocalFile(":/sounds/sounds/tank_b_.wav"));
    Blow.setSource(QUrl::fromLocalFile(":/sounds/sounds/tank_Blow_.wav"));
    F4.setSource(QUrl::fromLocalFile(":/sounds/sounds/F4_.wav"));
    gtone.setSource(QUrl::fromLocalFile(":/sounds/sounds/tank_g_.wav"));
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

   }
