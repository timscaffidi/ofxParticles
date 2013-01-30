#include "testApp.h"



//--------------------------------------------------------------
void testApp::setup(){
    ofSetFrameRate(60.0);
    ofBackground(0, 0, 0);
    mouseEmitter.velSpread = ofVec3f(25.0,25.0);
    mouseEmitter.life = 10.0;
    mouseEmitter.lifeSpread = 5.0;
    mouseEmitter.numPars = 10;
    mouseEmitter.color = ofColor(200,200,255);
    mouseEmitter.colorSpread = ofColor(20,20,0);
    
    leftEmitter.setPosition(ofVec3f(0,ofGetHeight()/3));
    leftEmitter.setVelocity(ofVec3f(150.0,0.0));
    leftEmitter.posSpread = ofVec3f(10,10.0);
    leftEmitter.velSpread = ofVec3f(10.0,10);
    leftEmitter.life = 20;
    leftEmitter.lifeSpread = 5.0;
    leftEmitter.numPars = 3;
    leftEmitter.color = ofColor(225,100,100);
    leftEmitter.colorSpread = ofColor(30,50,50);
    
    rightEmitter = leftEmitter;
    rightEmitter.setPosition(ofVec3f(ofGetWidth()-1,ofGetHeight()*2/3));
    rightEmitter.setVelocity(ofVec3f(-150.0,0.0));
    rightEmitter.color = ofColor(100,100,225);
    rightEmitter.colorSpread = ofColor(50,50,30);
    
    topEmitter = leftEmitter;
    topEmitter.setPosition(ofVec3f(ofGetWidth()*2/3,0));
    topEmitter.setVelocity(ofVec3f(0.0,150.0));
    topEmitter.color = ofColor(100,225,100);
    topEmitter.colorSpread = ofColor(50,30,50);
    
    botEmitter = leftEmitter;
    botEmitter.setPosition(ofVec3f(ofGetWidth()/3,ofGetHeight()-1));
    botEmitter.setVelocity(ofVec3f(0.0,-150.0));
    botEmitter.color = ofColor(225,255,0);
    botEmitter.colorSpread = ofColor(30,30,0);
    
    vectorField.allocate(25, 25, 3);
        
    rotAcc = 20000;
    gravAcc = 50000;
    drag = 0.99;
    fieldMult = 2.0;
    
    ofEnableBlendMode(OF_BLENDMODE_ADD);
}

//--------------------------------------------------------------
void testApp::update(){
    
    for(int y = 0; y < vectorField.getHeight(); y++)
        for(int x=0; x< vectorField.getWidth(); x++){
            int index = vectorField.getPixelIndex(x, y);
            float angle = ofNoise(x/(float)vectorField.getWidth()*4.0, y/(float)vectorField.getHeight()*4.0,ofGetElapsedTimef()*0.5)*TWO_PI*2.0;
            ofVec2f dir(cos(angle), sin(angle));
            dir.normalize().scale(ofNoise(x/(float)vectorField.getWidth()*4.0, y/(float)vectorField.getHeight()*4.0,ofGetElapsedTimef()*0.5+10.0));
            vectorField.setColor(x, y, ofColor_<float>(dir.x,dir.y, 0));
        }
    
    float dt = min(ofGetLastFrameTime(), 1.0/10.0);
    particleSystem.gravitateTo(ofPoint(ofGetWidth()/2,ofGetHeight()/2), gravAcc, 1, 10.0, false);
    particleSystem.rotateAround(ofPoint(ofGetWidth()/2,ofGetHeight()/2), rotAcc, 10.0, false);
    particleSystem.applyVectorField(vectorField.getPixels(), vectorField.getWidth(), vectorField.getHeight(), vectorField.getNumChannels(), ofGetWindowRect(), fieldMult);
    particleSystem.update(dt, drag);
    
    particleSystem.addParticles(leftEmitter);
    particleSystem.addParticles(rightEmitter);
    particleSystem.addParticles(topEmitter);
    particleSystem.addParticles(botEmitter);
    
    ofVec2f mouseVel(mouseX-pmouseX,mouseY - pmouseY);
    mouseVel*=20.0;
    if(ofGetMousePressed()){
        mouseEmitter.setPosition(ofVec3f(pmouseX,pmouseY),ofVec3f(mouseX,mouseY));
        mouseEmitter.posSpread = ofVec3f(10.0,10.0,0.0);
        mouseEmitter.setVelocity(pmouseVel, mouseVel);
        particleSystem.addParticles(mouseEmitter);
    }
    pmouseX = mouseX;
    pmouseY = mouseY;
    pmouseVel = mouseVel;
}

//--------------------------------------------------------------
void testApp::draw(){
    if(ofGetKeyPressed('v')){
        ofSetLineWidth(1.0);
        ofSetColor(50, 50, 50);
        ofPushMatrix();
        ofScale(ofGetWidth()/(float)vectorField.getWidth(), ofGetHeight()/(float)vectorField.getHeight());
        for(int y = 0; y < vectorField.getHeight(); y++)
            for(int x=0; x< vectorField.getWidth(); x++){
                ofColor_<float> c = vectorField.getColor(x, y);
                ofVec2f dir(c.r,c.g);
                
                ofLine(x, y, x+dir.x, y+dir.y);
            }
        ofPopMatrix();
    }
    
    ofNoFill();
    ofSetCircleResolution(180);
    ofSetColor(255, 0, 0, 50);
    ofCircle(ofGetWidth()/2, ofGetHeight()/2, sqrt(gravAcc));
    ofSetColor(0, 0, 255, 50);
    ofCircle(ofGetWidth()/2, ofGetHeight()/2, sqrt(rotAcc));
    
    ofSetLineWidth(2.0);
    particleSystem.draw();
    ofSetColor(255, 255, 255);
    ofDrawBitmapString(ofToString(particleSystem.getNumParticles()) + "\n" + ofToString(ofGetFrameRate()) +
                       "\ngravitation: " + ofToString(gravAcc) +
                       "\nrotational acceleration: " + ofToString(rotAcc) +
                       "\nvector field multiplier: " + ofToString(fieldMult) +
                       "\ndrag constant: " + ofToString(drag), 20,20);
    
    
    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch (key) {
        case 'r':
            if(rotAcc > 1.1)
                rotAcc /= 1.1;
            break;
        case 'R':
            rotAcc *= 1.1;
            break;
            
        case 'g':
            if(gravAcc > 1.1)
                gravAcc /= 1.1;
            break;
        case 'G':
            gravAcc *= 1.1;
            break;
            
        case 'd':
            if(drag > 0.001)
                drag /= 1.001;
            break;
        case 'D':
            drag *= 1.001;
            if(drag > 1.0) drag = 1.0;
            break;
        case 'f':
            if(fieldMult > 1.1)
                fieldMult /= 1.1;
            break;
        case 'F':
            fieldMult *= 1.1;
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}