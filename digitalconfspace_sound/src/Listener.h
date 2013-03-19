#pragma once

#include "ofMain.h"

class Listener: public ofFmodSoundPlayer {

public:

	Listener();

	//static void initializeFmod();
	static void closeFmod();

	FMOD_VECTOR listener_velocity, listener_up, listener_forward, listener_position;

	void updateListener(ofVec3f position, ofVec3f velocity, ofVec3f forward, ofVec3f up);
	FMOD_SYSTEM getFmodSystem();
};