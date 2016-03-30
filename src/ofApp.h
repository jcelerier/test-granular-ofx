#pragma once

#include "ofMain.h"
#include "ofxMaxim.h"
#include "ofxMaxim.h"
#include "maxiGrains.h"
#include "ofxOsc.h"
#include <random>
#include <sys/time.h>

#define HOST "localhost"
#define RECEIVEPORT 12000
#define SENDPORT 6448

inline float randFloat(float min, float max)
{
    static std::random_device rd;
    static std::mt19937 e2(rd());
    static std::uniform_real_distribution<> dist(min, max);

    return dist(e2);
}
struct GrainPlayer;
typedef void (*HashFun)(GrainPlayer&, const ofxOscMessage&);
typedef hannWinFunctor grainPlayerWin;
struct GrainPlayer
{
        // Speed : 0 -> 1
        // Rate : -1 -> 1
        // GrainLength: 0.0004 -> 0.01
        // Overlaps : 1 -> 6
        // PosMod : 0 -> 1

        maxiSample sample;
        maxiTimePitchStretch<grainPlayerWin, maxiSample>* stretch;
        double rate{0.4}, speed{0.8}, grainLength{0.001}, pos, posMod{0.02}, filter{0.6};
        int overlaps{4};
        ofxMaxiFilter lowPass, highPass, highPass2;
        bool fadingIn = false;
        bool fadingOut = false;
        float fadeCoeff = 0;
        float randomPercent = 1;

        void handleFade()
        {
            const float f_mod = 0.5;
            if(fadingIn)
            {
                if(fadeCoeff < 1)
                {
                    fadeCoeff += f_mod;
                }
                else
                {
                    fadeCoeff = 1;
                    fadingIn = false;
                }
            }
            else if(fadingOut)
            {
                if(fadeCoeff > 0)
                {
                    fadeCoeff -= f_mod;
                }
                else
                {
                    fadeCoeff = 0;
                    fadingOut = false;
                }
            }
        }

        static void handleSpeed(GrainPlayer& pl, const ofxOscMessage& m)
        {
            pl.speed = (double) m.getArgAsFloat(0);
        }

        static void handleRate(GrainPlayer& pl, const ofxOscMessage& m)
        {
            pl.rate = 1. - 2. * m.getArgAsFloat(0);
         }

        static void handleFilter(GrainPlayer& pl, const ofxOscMessage& m)
        {
            pl.filter = (double) m.getArgAsFloat(0);
        }

        static void handlePosmod(GrainPlayer& pl, const ofxOscMessage& m)
        {
            pl.posMod = (double) m.getArgAsFloat(0);
        }


        static void handleGL(GrainPlayer& pl, const ofxOscMessage& m)
        {
            pl.grainLength = (m.getArgAsFloat(0) * 0.1) + 0.001;
        }

        static void handleClicked(GrainPlayer& pl, const ofxOscMessage& m)
        {
            static std::random_device rd;
            static std::mt19937 e2(rd());

            if(!pl.fadingIn)
            {
                pl.fadingIn = true;
                if(pl.fadingOut)
                {
                    pl.fadingOut = false;
                }
            }

            std::uniform_real_distribution<> dist(0.1, 0.9);
            auto m1 = dist(e2);
            auto m2 = dist(e2);
            auto min = std::min(m1, m2);
            auto max = std::max(m1, m2);
            std::uniform_real_distribution<> pos_dist(min, max);


            pl.stretch->setLoopStart(min);
            pl.stretch->setLoopEnd(max);
            pl.stretch->setPosition(pos_dist(e2));
        }

        static void handleReleased(GrainPlayer& pl, const ofxOscMessage& m)
        {
            if(!pl.fadingOut)
            {
                pl.fadingOut = true;
                if(pl.fadingIn)
                {
                    pl.fadingIn = false;
                }
            }
        }
        static void handleRandomPercent(GrainPlayer& pl, const ofxOscMessage& m)
        {
            pl.randomPercent = std::min(1.f, std::max(0.f, m.getArgAsFloat(0)));
        }

        void handleMessage(const ofxOscMessage& m)
        {
            static const std::map<std::string, HashFun> map{
                {"rate", &GrainPlayer::handleRate},
                {"speed", &GrainPlayer::handleSpeed},
                {"grain", &GrainPlayer::handleGL},
                {"clicked", &GrainPlayer::handleClicked},
                {"released", &GrainPlayer::handleReleased},
                {"random", &GrainPlayer::handleRandomPercent},
                {"posmod", &GrainPlayer::handlePosmod},
                {"filter", &GrainPlayer::handleFilter},
            };

            auto addr = m.getAddress();
            auto slash_pos = addr.find_last_of('/');
            if(slash_pos == std::string::npos)
                return;/*
            if(m.getNumArgs() > 0)
                std::cerr << m.getAddress() << " " << m.getArgAsFloat(0) << std::endl;
*/
            auto it = map.find(std::string(addr.begin() + slash_pos + 1, addr.end()));
            if(it != map.end()) {
                (it->second)(*this, m);
            }
        }

};



class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);


    void audioOut(float * output, int bufferSize, int nChannels);
    void audioIn(float * input, int bufferSize, int nChannels);

    int		bufferSize;
    int		initialBufferSize; /* buffer size */
    int		sampleRate;

    double wave,outputs[2];
    maxiMix mymix;

    GrainPlayer players[2]{};

    GrainPlayer& guitar{players[0]};
    GrainPlayer& viola{players[1]};

    //osc
    ofxOscReceiver receiver;


};


