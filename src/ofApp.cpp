/* This is an example of how to integrate maximilain into openFrameworks,
 including using audio received for input and audio requested for output.


 You can copy and paste this and use it as a starting example.

 */

#include "ofApp.h"
//#include "maximilian.h"/* include the lib */
#include "time.h"

//--------------------------------------------------------------
void ofApp::setup(){

    /* This is stuff you always need.*/
    receiver.setup(12000);

    ofSetFrameRate(30);

    viola.sample.load("/home/jcelerier/bosch/audio/SFX/Vielle-1-mono.wav");
    viola.stretch = new maxiTimePitchStretch<grainPlayerWin, maxiSample>(&viola.sample);
    guitar.sample.load("/home/jcelerier/bosch/audio/SFX/Guitar-1-mono.wav");
    guitar.stretch = new maxiTimePitchStretch<grainPlayerWin, maxiSample>(&guitar.sample);

    sampleRate 	= 44100; /* Sampling Rate */
    bufferSize	= 512; /* Buffer Size. you have to fill this buffer with sound using the for loop in the audioOut method */


    ofxMaxiSettings::setup(sampleRate, 2, initialBufferSize);

    ofSoundStreamSetup(2,2,this, sampleRate, bufferSize, 4); /* this has to happen at the end of setup - it switches on the DAC */
}

//--------------------------------------------------------------
void ofApp::update(){

    while(receiver.hasWaitingMessages())
    {
        // get the next message
        ofxOscMessage m;
        receiver.getNextMessage(m);

        auto addr = m.getAddress();
        if(addr.find("/viola") != std::string::npos)
        {
            viola.handleMessage(m);
        }
        else if(addr.find("/guitar") != std::string::npos)
        {
            guitar.handleMessage(m);
        }
    }

    for(auto& elt : players)
        elt.handleFade();
}

//--------------------------------------------------------------
void ofApp::draw(){

}

//--------------------------------------------------------------
void ofApp::audioOut(float * output, int bufferSize, int nChannels) {

    for (int i = 0; i < bufferSize; i++){
        output[i*nChannels    ] = 0;
        output[i*nChannels + 1] = 0;
    }

    for(GrainPlayer& player : players)
    {
        float rand_ratio = randFloat(-0.01, 0.01) * player.randomPercent;
        for (int i = 0; i < bufferSize; i++){

            wave = player.stretch->play(
                       player.speed,
                       player.rate,
                       player.grainLength + player.grainLength * rand_ratio,
                       player.overlaps,
                       player.posMod);

            // Filter : 0 -> 1
            wave = player.lowPass.lopass(wave, player.filter);
            wave = player.highPass.hipass(wave, 0.3);
            wave = player.highPass2.hipass(wave, 0.7);

            //play result
            mymix.stereo(wave, outputs, 0.5);
            output[i*nChannels    ] += 1.5 * player.fadeCoeff * outputs[0];
            output[i*nChannels + 1] += 1.5 * player.fadeCoeff * outputs[1];

        }
    }

}

//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels){

}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
