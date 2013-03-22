#pragma once

#include "ofMain.h"
#include <stdio.h>
//Include Addons----------------------------------------------------------------
#include "ofxOsc.h"
#include "isense.h"

//Include Class Headers---------------------------------------------------------
#include "SoundBox.h"
//#include "Listener.h"

//Definitions-------------------------------------------------------------------
#define TAU 6.2831853
#define PORT 8001
#define WASD_LEFT 97
#define WASD_RIGHT 100
#define WASD_UP 119
#define WASD_DOWN 115
#define WASD_ROLL_LEFT 113
#define WASD_ROLL_RIGHT 101

class testApp : public ofBaseApp{
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
		
		
		int user_height;
		int box_distance;
		float cursor_z;
		vector<string> sound_files;

		void drawGrid();
		void drawCursor();

		void updateTracker();
		void getUDPMessages();
		void addSoundBox();
		void removeSoundBox();

		ofCamera cam;
		float pan;
		float tilt;
		float roll;
		void rotateToDefault();

		ofLight pointLight;
		ofColor light_color;
		ofMaterial cursor_material;

		ofxOscReceiver receiver;
				
		vector<SoundBox*> soundboxes;
		SoundBox listener;

		ofVec3f box_loc, box_vel, box_rotation;
		ofVec3f listener_position, listener_velocity, listener_forward, listener_up;
		ofColor box_color;
		
		
};