#pragma once

#include "ofMain.h"
#include "fmod.h"

class SoundBox: public ofFmodSoundPlayer {

	protected:
    
	ofVec3f _box_location, _box_rotation, _box_velocity;
	ofColor _box_color;
	ofMaterial box_material;
	FMOD_VECTOR sound_position, sound_velocity;
	FMOD_VECTOR listener_velocity, listener_up, listener_forward, listener_position;

	public:
	
	SoundBox();
	SoundBox(ofVec3f box_location, ofVec3f box_rotation, ofColor box_color);

	static void initializeFmod();
	static void closeFmod();
	bool loadSound(string fileName, bool stream = false);
	void play();
	void stop(); 
	void updateSound(ofVec3f box_location, ofVec3f box_velocity);
	void updateListener(ofVec3f position, ofVec3f velocity, ofVec3f forward, ofVec3f up);
	void update();
	void drawSoundBox();	
	void setNewLocation(ofVec3f box_location,  ofVec3f box_rotation);
	ofVec3f getBoxLocation();
	ofVec3f getBoxRotation();
};