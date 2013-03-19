#include "Listener.h"
//#include "ofFmodSoundPlayer.cpp"



Listener::Listener(){
	listener_velocity.x = 1;
    listener_velocity.y = 1;
    listener_velocity.z = 1;
    listener_up.x = 0.f;
    listener_up.y = 1.f;
    listener_up.z = 0;
    listener_forward.x = 0.f;
    listener_forward.y = 0.f;
    listener_forward.z = 1.0;
    listener_position.x = 0;
    listener_position.y = 0;
    listener_position.z = 0;

	//initializeFmod();
}

/*void Listener::initializeFmod(){

    if(!bFmod3DInitialized){
        FMOD_System_Create(&sys);
        FMOD_System_Init(sys, 32, FMOD_INIT_NORMAL, NULL);
        //do we want just 32 channels?
        
		FMOD_System_Set3DSettings(sys, 10.0f, 10.0f, 10.0f);
        FMOD_System_GetMasterChannelGroup(sys, &channelgroup);
        bFmod3DInitialized = true;
    }
	
}*/

void Listener::updateListener(ofVec3f position, ofVec3f velocity, ofVec3f forward, ofVec3f up)
{
	
	listener_velocity.x = velocity.x;
	listener_velocity.y = velocity.y;
	listener_velocity.z = velocity.z;
	
	listener_position.x = position.x;
	listener_position.y = position.y;
	listener_position.z = position.z;
	
	listener_forward.x = forward.x;
	listener_forward.y = forward.y;
	listener_forward.z = forward.z;
	
	listener_up.x = up.x;
	listener_up.y = up.y;
	listener_up.z = up.z;
	
	//FMOD_System_Set3DListenerAttributes(sys, 0, &listener_position, &listener_velocity, &listener_forward, &listener_up);

}