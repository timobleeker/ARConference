#include "testApp.h"

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
#include <conio.h>
#endif

#define ESC 0X1B
#define VER "1.0.7"

ISD_TRACKER_HANDLE    handle;
ISD_TRACKER_INFO_TYPE    tracker;

SoundBox * listener = new SoundBox();
//--------------------------------------------------------------
void testApp::setup(){
	
	//set up tracker
	handle = ISD_OpenTracker((Hwnd)NULL, 0, FALSE, FALSE );
	if ( handle > 0 )
		printf( "\n  Az El Rl \n" );
	else
		printf( "Tracker not found. Press any key to exit" );
	//translate so the center of the screen is 0,0 before doing anything else
	ofTranslate(ofGetScreenWidth()/2, ofGetScreenHeight()/2, 0);

	glEnable(GL_DEPTH_TEST);
	ofEnableAlphaBlending();
	ofSetSmoothLighting(true);
	ofEnableLighting();

	user_height = 180;
	box_distance = 400;
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

	//set up camera
	cam.setPosition(ofVec3f(0,0,0));

	listener.updateListener(listener_position, listener_velocity, listener_forward, listener_up);

	//set up listening port for UDP messages
	receiver.setup(PORT);
}

//--------------------------------------------------------------
void testApp::update(){
	
	updateTracker();
	getUDPMessages();
	
	listener_forward.set(-sin(ofDegToRad(pan)), 0, cos(ofDegToRad(pan)));
	listener.updateListener(listener_position, listener_velocity, listener_forward, listener_up);
	for(auto box = soundboxes.begin(); box != soundboxes.end(); box++){
		if(!(*box)->getIsPlaying())
			(*box)->play();
		
		float y_rot = (*box)->getBoxRotation().y;
		float z_loc = (*box)->getBoxLocation().z;
			if(pan > y_rot - 7 && pan < y_rot + 7 && cursor_z > -box_distance - 50 && cursor_z < -box_distance + 50){
				ofSetColor(0,255,0, 255);
				cout << "hit" << endl;
				break;
			} else { 
				ofSetColor(255,0,0, 255);
			}
	}

	if(soundboxes.size() > 0)
		soundboxes[0]->update();
}

//--------------------------------------------------------------
void testApp::draw(){
		
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
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

	switch (key)
	{
		/*case WASD_LEFT:
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
			*/
		case OF_KEY_UP:
			if (soundboxes.size() < sound_files.size())
			addSoundBox();
			break;
		case OF_KEY_DOWN:
			rotateToDefault();
			break;
		case OF_KEY_RIGHT:
			if (soundboxes.size() > 0)
				removeSoundBox();
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
	int rotation; 
	cout << "Enter a rotation (0-360): ";
	cin >> rotation;
	box_rotation.set(0, rotation, 0);
	//-------------------------------

	box_loc.set(box_distance*sin(ofDegToRad(rotation)), 0, -box_distance*cos(ofDegToRad(rotation))); 
	box_color.set(ofRandom(0,255), ofRandom(0,255), ofRandom(0,255));
	
	SoundBox * box = new SoundBox(box_loc, box_rotation, box_color);

	box->loadSound(ofToDataPath(sound_files[soundboxes.size()]));
	box->setVolume(1); //I don't want to hear birds when working on other things
	box->setMultiPlay(true);
	box->updateSound(box_loc, box_vel);
	box->play();
	soundboxes.push_back(box);
}
//--------------------------------------------------------------
void testApp::removeSoundBox(){
	int n;
	cout << "Enter the box to remove:";
	cin >> n;
	if (n < soundboxes.size()){
		soundboxes.erase(soundboxes.begin() + n);
		//add the sound file at the back of the vector and remove it from the original location.
		sound_files.push_back(sound_files[n]);
		sound_files.erase(sound_files.begin() + n);
		//do I need a destructor?
	} else {
		cout << "specified box does not exist" << endl;
	}
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
	//only draw the cursor when it's not too far away.
	if(tilt > 5 && tilt < 150){
		cursor_z = -user_height*tan(ofDegToRad(90-tilt));
				
		cursor_material.begin();
		ofEnableAlphaBlending();
		
		ofPushMatrix();
			ofTranslate(0,-user_height, cursor_z);
			ofRotateX(90);
			
			if(tilt < 10) ofSetColor(255, 0, 0, ofMap(tilt, 5, 10, 0, 255));
			ofCircle(0,0,50);
		ofPopMatrix();
		ofDisableAlphaBlending();
		cursor_material.end();
	}
}

//--------------------------------------------------------------
void testApp::rotateToDefault() {
	
	ofRotateY(-pan);
	ofRotateZ(-roll);
	ofRotateX(-tilt);

	tilt = 0;
	roll = 0;
	pan = 0;
}

//--------------------------------------------------------------
void testApp::updateTracker(){
	ISD_TRACKING_DATA_TYPE    data;

	if ( handle > 0 ) {
	ISD_GetTrackingData( handle, &data );
	//printf( "%7.2f %7.2f %7.2f  ",
	pan = data.Station[0].Euler[0];
	tilt = -data.Station[0].Euler[1];
	roll = data.Station[0].Euler[2];
	//ISD_GetCommInfo( handle, &tracker );
	//printf( "%5.2f Kb/s %d Rec/s \r",
//	tracker.KBitsPerSec, tracker.RecordsPerSec );
	fflush(0);
}
}

//--------------------------------------------------------------
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
		} else if(message.getAddress() == "Remove" && message.getArgAsInt32(0) == 1){
			removeSoundBox();
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
