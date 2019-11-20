# minddrawplay

The code is written in C++, Qt 5.6.0, compiled with MinGW 4.9.2 32bit

The project is specifically made for mobile EEG device MindWave NeuroSky,
but also can work with other EEG devices (with less features) through BCI2000 module.

MindDrawPlay – is a project of experimental interactive audio-visual art, 
representing translation of brain waves to drawing, graphical and musical space controls. 
It has been grown on a base of a research work in the direction of Brain-Computer Interfaces. 
Combining both latest technology advances – such as mobile EEG devices and musical knowledge – 
such as pentatonic scales, it allows everyone literally to see, to hear his brain activity 
represented by set of sounds and to use brain waves as a brush for drawing, as parameters 
for image filtering or changes in a graphical flow and in a simple puzzle gathering 
and identical pictures recognition games. 

Application allows user to control many different parameters, for example, in case of music – 
duration of tones, number of tones in a moment and their distribution (how often which tones play), 
in case of drawing – different color modes of brush and amplitude of brain signals as a brush, 
in case of graphical space flow – how many pictures in a puzzle will change and how fast. 
Moreover, you can observe in real time dynamics of your brain waves on plots, 
your attention/meditation level (estimated by build-in algorithms in EEG device) changes and 
how actions in application or your mental activity states influence brain waves. 
Therefore, essentially, MindDrawPlay is form of an interactive art, 
neurofeedback application and a tool for exploring brain activity patterns.

There are two windows in the application, on the top of both there is a plot with real-time 
brain signal from EEG device. 1st window is “MindPlay”, allowing translation of brain waves 
to music by playing samples of tones from 2 tank drums and 1 hang drum with any background image (.jpg). 
The image can be filtered at the same time with effects (blurring and change of colors) with parameters 
related to brain activity: higher attention/meditation level correspond to stronger effect. 
Music by brain waves can be combined with usual playing of samples by user. 

2nd window is “MindDraw” with 4 different modes representing various graphical spaces and controls of them. 
In all modes there is an option to show in real time brain waves and attention / meditation levels on the plots. 
It represents your brain activity patterns, when you simply look on the application window or do something there 
(or wherever within limits of bluetooth connection). Therefore, you can see how your interactions influence 
your brain activity, for example, when you start drawing a line – your attention and beta waves usually 
increase, when you are closing eyes or relaxing – alpha waves usually get higher. 
Moreover, there is an option for combining drawing with music – when your attention/meditation level 
is higher than some value (like 80% or adaptively computed value) – musical space is activating 
and you hear sounds from "MindPlay" window. 

MindDrawPlay is a part-time hobby project currently tuned for MindWave EEG device. 
However, it can be adapted for other mobile or full EEG systems. 
There are a lot of ideas and ways for development and improvement. 
Author is interested and opened for any potential collaborations. 

broader overview with screenshots in "about.pdf" 

...

Examples of running app: https://www.artstation.com/neur0forest/albums/1425498
