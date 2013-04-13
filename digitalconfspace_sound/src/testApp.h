#pragma once

#include "ofMain.h"
#include <stdio.h>
#include <random>
#include <memory>
//Include Addons----------------------------------------------------------------
#include "ofxOsc.h"
#include "ofxXmlSettings.h"
#include "isense.h"

//Include Class Headers---------------------------------------------------------
#include "SoundBox.h"
#include "ShapeFile.h"
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

	protected:
		std::mt19937 _random;

	public:
		testApp();
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
		int selected;
		bool moving;
		float cursor_z;

		vector<string> sound_files;
		vector<string> video_files;

		void drawGrid();
		void drawCursor();
		int getSelected();
		void selectSoundBox();

		void setupTracker();
		void updateTracker();
		void getUDPMessages();
		void clearUDPMessages();
		void addSoundBox();
		void removeSoundBox();
		void positionSoundBox();
		
		void setupSession();

		void startNewSession();
		void endSession();

		void visualCue();
		void startAudioCue();
		void endAudioCue();

		void sendToXML(string tag, float result, int tag_num);
		void sendToXML(string tag, string input, int tag_num);
		
		void startTimer();
		void stopTimer();
		void checkCorrect();

		//camera
		ofCamera cam;
		float pan;
		float tilt;
		float roll;
		void rotateToDefault();

		//light
		ofLight pointLight;
		ofColor light_color;
		ofMaterial cursor_material;

		//udp
		ofxOscReceiver receiver;
		
		//vector with the sound boxes
		vector<SoundBox*> soundboxes;
		SoundBox listener;

		ofVec3f box_loc, box_vel, box_rotation;
		ofVec3f listener_position, listener_velocity, listener_forward, listener_up;
		ofColor box_color;

		//ShapeFile properties
		vector<string> shapes;
		vector<string> shape_colors;
		string shape;
		string shape_color;
		ofColor blue;
		ofColor yellow;
		ofColor red;

		string asked_shape;
		string asked_color;

		int target; //targeted soundbox by tablet
		int item_receiver; //soundbox that will receive the file
		int random_questioner; //soundbox that asked for the file and should receive it.

		string random_shape;
		string random_color;
		

		bool ask_for_file;
		void askFile();

		bool wait_for_file;
		void awaitFile();

		bool file_received;
		void drawFile();
		
		std::shared_ptr<ShapeFile> shapefile;
		//vector<ShapeFile*> shapefiles;
		float spin;

		float saved_time;
		float time_object_placed;

		//XML saving
		ofxXmlSettings XML;
		int task_tag;
		int session_tag;
		int condition;
		int iterations;
};