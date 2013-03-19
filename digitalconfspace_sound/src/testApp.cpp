#include "testApp.h"

SoundBox * listener = new SoundBox();
//--------------------------------------------------------------
void testApp::setup(){
	
	glEnable(GL_DEPTH_TEST);
	ofEnableAlphaBlending();
	ofSetSmoothLighting(true);

	user_height = 180;
	pan = 0;
	tilt = 0;
	roll = 0;
	ofBackground(0);
	light_color.set(255,255,255);

	pointLight.setPosition(0,0,0);
	pointLight.setDiffuseColor(light_color);
	pointLight.setSpecularColor(light_color);

	listener_velocity.set(1, 1, 1);
	listener_up.set(0, 1, 0);
	listener_forward.set(0, 0, 1);
	listener_position.set(0, 0, 0);
	sound_files.push_back("birds.wav");
	sound_files.push_back("organ.wav");

	//translate so the center of the screen is 0,0 before doing anything else
	ofTranslate(ofGetScreenWidth()/2, ofGetScreenHeight()/2, 0);

	//set up camera
	cam.setPosition(ofVec3f(0,0,0));

	listener.updateListener(listener_position, listener_velocity, listener_forward, listener_up);

	//set up listening port for UDP messages
	receiver.setup(PORT);
}

//--------------------------------------------------------------
void testApp::update(){
		
	getUDPMessages();
	
	listener_forward.set(-sin(ofDegToRad(pan)), 0, cos(ofDegToRad(pan)));
	listener.updateListener(listener_position, listener_velocity, listener_forward, listener_up);
	for(auto box = soundboxes.begin(); box != soundboxes.end(); box++){
		if(!(*box)->getIsPlaying())
			(*box)->play();
	}

	if(soundboxes.size() > 0)
		soundboxes[0]->update();
}

//--------------------------------------------------------------
void testApp::draw(){
	ofEnableLighting();
	
	cam.begin();
	
	ofRotateX(tilt);
	ofRotateZ(roll);
	drawCursor();
	ofRotateY(pan);
	pointLight.enable();
	drawGrid();

	//create all boxes
	for(auto box = soundboxes.begin(); box != soundboxes.end(); box++){
		
		(*box)->drawSoundBox();	
	}

	cam.end();
	ofDisableLighting();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

	switch (key)
	{
		case WASD_LEFT:
			pan--;
			break;
		case WASD_RIGHT:
			pan++;
			break;
		case WASD_UP:
			tilt++;
			break;
		case WASD_DOWN:
			tilt--;
			break;
		case WASD_ROLL_LEFT:
			roll++;
			break;
		case WASD_ROLL_RIGHT:
			roll--;
			break;
		case OF_KEY_UP:
			//add a box
			addSoundBox();
			break;
		case OF_KEY_DOWN:
			rotateToDefault();
			break;
	}
		
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){
	
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

//--------------------------------------------------------------
void testApp::addSoundBox(){
		
	//Comment this out when using UDP
	int y; 
	cout << "Enter a rotation (0-360): ";
	cin >> y;
	box_rotation.set(0, y, 0);
	//-------------------------------

	box_loc.set(200*sin(ofDegToRad(y)), 0, -200*cos(ofDegToRad(y))); 
	box_color.set(ofRandom(0,255), ofRandom(0,255), ofRandom(0,255));
	
	SoundBox * box = new SoundBox(box_loc, box_rotation, box_color);
	
	
	box->loadSound(ofToDataPath(sound_files[soundboxes.size()]));
	box->setVolume(1);
	box->setMultiPlay(true);
	box->updateSound(box_loc, box_vel);
	box->play();
	soundboxes.push_back(box);
	cout << soundboxes.size() << "    " << sound_files.size() << endl;
}

//--------------------------------------------------------------
void testApp::drawGrid(){
	
	//draw a grid on the floor
	ofSetColor(255,255,255);
	for(int i=-2000;i<2200;i+=100)
	{
		ofPushMatrix();
		ofRotate(-90., 1., 0., 0.);
		ofTranslate(0,0,-user_height);

		ofLine(-2000,i,2000,i);
		ofLine(i,-2000,i,2000);
		ofPopMatrix();
	}
}

//--------------------------------------------------------------
void testApp::drawCursor(){
	if(tilt > 5 && tilt < 150){
	ofEnableAlphaBlending();
	float z_translation = -user_height*tan(ofDegToRad(90-tilt));
	cout << z_translation << "    " << tilt << "      " << ofMap(tilt, 0, 10, 0, 255) << endl;
	//draw cursor circle
	ofPushMatrix();
		ofTranslate(0,-user_height, z_translation);
		ofRotateX(90);
		ofSetColor(255,0,0, 255);
		if(tilt < 10) ofSetColor(255, 0, 0, ofMap(tilt, 5, 10, 0, 255));
		ofCircle(0,0,100);
	ofPopMatrix();
	ofDisableAlphaBlending();
	}
}

void testApp::rotateToDefault() {
	
	ofRotateY(-pan);
	ofRotateZ(-roll);
	ofRotateX(-tilt);

	tilt = 0;
	roll = 0;
	pan = 0;
}

void testApp::getUDPMessages(){
		
	// check for waiting messages
	while(receiver.hasWaitingMessages()){
		// get the next message
		ofxOscMessage message;
		receiver.getNextMessage(&message);

		if(message.getAddress() == "Location"){
			float mapped_value = ofMap(message.getArgAsFloat(0), 0, 1, -1, 1);
			cout << mapped_value << endl;
			box_rotation.set(-mapped_value*180, 0, 0);

		} else if(message.getAddress() == "Place" && message.getArgAsInt32(0) == 1){
			addSoundBox();
		}



		/*

		//print out in console
		string msg_string;
		msg_string = message.getAddress();
		msg_string += ": ";
		
		for(int i = 0; i < message.getNumArgs(); i++){
			// get the argument type
			msg_string += message.getArgTypeName(i);
			msg_string += ":";
			// display the argument - make sure we get the right type
			if(message.getArgType(i) == OFXOSC_TYPE_INT32){
				msg_string += ofToString(message.getArgAsInt32(i));
			}
			else if(message.getArgType(i) == OFXOSC_TYPE_FLOAT){
				msg_string += ofToString(message.getArgAsFloat(i));
			}
			else if(message.getArgType(i) == OFXOSC_TYPE_STRING){
				msg_string += message.getArgAsString(i);
			}
			else{
				msg_string += "unknown";
			}
				
		}
		cout << msg_string << endl;
		*/
	}

}
