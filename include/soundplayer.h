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

    void playD3();
    void playA3();
    void playC4();
    void playA4();  
    void playD4(); 
    void playE4();
    void playF3(); 
    void playF4();

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

    void play_Asharp();
    void play_bhigh();
    void play_c_highsharp();
    void play_d_highsharp();
    void play_Dsharp();
    void play_F();
    void play_f_high();
    void play_f_highsharp();
    void play_Fsharp();
    void play_Gsharp();

    void stopThread();

private:

    QThread m_thread;    

    QSoundEffect D3;    
    QSoundEffect A3;    
    QSoundEffect C4;    
    QSoundEffect A4;    
    QSoundEffect D4;
    QSoundEffect E4;    
    QSoundEffect F3;        
    QSoundEffect F4;    

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

    QSoundEffect tank_Asharp;
    QSoundEffect tank_bhigh;
    QSoundEffect tank_c_highsharp;
    QSoundEffect tank_d_highsharp;
    QSoundEffect tank_Dsharp;
    QSoundEffect tank_F;
    QSoundEffect tank_f_high;
    QSoundEffect tank_f_highsharp;
    QSoundEffect tank_Fsharp;
    QSoundEffect tank_Gsharp;
};

#endif
