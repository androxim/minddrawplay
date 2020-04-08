/* header file for sounplayer class -
   resposible for sound samples configuration and playing in separate thread */

#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

#include <QObject>
#include <QThread>
#include <QSoundEffect>

class soundplayer : public QObject
{
    Q_OBJECT
public:
    void setvolume(qreal volume);
    void init();
    explicit soundplayer(QObject *parent = 0);

signals:

    void playGlow();
    void playD3();
    void playfdiez();
    void playA3();
    void playClow();
    void playC4();
    void playatone();
    void playA4();
    void playElow();
    void playD4();
    void playdtone();
    void playE4();
    void playDlow();
    void playF3();
    void playbtone();
    void playBlow();
    void playF4();
    void playgtone();
    void playtone_Blow();
    void playtone_b();
    void playtone_Dlowsh();
    void playtone_dsh();
    void playtone_Glowsh();
    void playtone_gsh();
    void playtone_Flowsh();
    void playtone_fsh();
    void playtone_Clowsh();
    void playtone_csh();

private:

    QThread m_thread;
    QSoundEffect Glow;
    QSoundEffect D3;
    QSoundEffect fdiez;
    QSoundEffect A3;
    QSoundEffect Clow;
    QSoundEffect C4;
    QSoundEffect atone;
    QSoundEffect A4;
    QSoundEffect Elow;
    QSoundEffect D4;
    QSoundEffect dtone;
    QSoundEffect E4;
    QSoundEffect Dlow;
    QSoundEffect F3;
    QSoundEffect btone;
    QSoundEffect Blow;
    QSoundEffect F4;
    QSoundEffect gtone;
    QSoundEffect tone_Blow;
    QSoundEffect tone_b;
    QSoundEffect tone_Dlowsh;
    QSoundEffect tone_dsh;
    QSoundEffect tone_Glowsh;
    QSoundEffect tone_gsh;
    QSoundEffect tone_Flowsh;
    QSoundEffect tone_fsh;
    QSoundEffect tone_Clowsh;
    QSoundEffect tone_csh;

};

#endif
