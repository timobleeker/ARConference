#include "soundBox.h"

#include "ofFmodSoundPlayer.cpp"

bool bFmod3DInitialized = false;

//the listener. Only make one of these.
SoundBox::SoundBox() {

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

	initializeFmod();
}
//the sound origins. You can make as many of these as you have sounds.
SoundBox::SoundBox(ofVec3f box_location, ofVec3f box_rotation, ofColor box_color) {

	_box_location = box_location; 
	_box_rotation = box_rotation;
	_box_color = box_color;

	sound_position.x = _box_location.x;
	sound_position.y = _box_location.y;
	sound_position.z = _box_location.z;
	sound_velocity.x = 0;
	sound_velocity.y = 0;
	sound_velocity.z = 0;

	//player.loadMovie("me_talking.mov");
	//player.play();

	/*grayPixels = new unsigned char [w*h];

	for(int i = 0; i < w*h; i++){
		grayPixels[i] = (unsigned char)(ofRandomuf() *255);
	}
	testTex.loadData(grayPixels, w, h, GL_LUMINANCE); */
}

void SoundBox::drawSoundBox(){
		
		/*for (int i = 0; i < w; i++){
			for (int j = 0; j < h; j++){ 
				grayPixels[j*w+i] = (unsigned char)(ofRandomuf() * 255);
			}
		}	
		testTex.loadData(grayPixels, w,h, GL_LUMINANCE); */

		box_material.setSpecularColor(ofColor(51,181,229));
		box_material.setShininess(4); 
		box_material.begin();
		ofPushMatrix();
		ofTranslate(_box_location);
		ofRotateY(-_box_rotation.y);
		
		if(is_selected){
			ofSetColor(ofColor(51,181,229));
		} else {
		ofSetColor(ofColor(170,170,170));
		}
		ofRect(-w/2-10,h/2+10,-1, w+20,-h-20);
		ofSetColor(ofColor(255,255,255));
		player.update();
		video_tex.draw(-w/2,h/2,0, w,-h);
		
		//ofBox(40);
		//ofSphere(20);
		ofSetColor(255,0,0);
		ofSetLineWidth(5);
		ofLine(ofPoint(0,0,0), ofPoint(0,-180,0));
		ofSetLineWidth(1);
		ofPopMatrix();
		box_material.end();
		
}

void SoundBox::setNewLocation(ofVec3f box_location,  ofVec3f box_rotation){
	_box_location = box_location;
	_box_rotation = box_rotation;
	sound_position.x = _box_location.x;
	sound_position.y = _box_location.y;
	sound_position.z = _box_location.z;

}

ofVec3f SoundBox::getBoxLocation(){
	return _box_location;
}

ofVec3f SoundBox::getBoxRotation(){
	return _box_rotation;
}

void SoundBox::initializeFmod(){

    if(!bFmod3DInitialized){
        FMOD_System_Create(&sys);
        FMOD_System_Init(sys, 32, FMOD_INIT_SOFTWARE_HRTF, NULL);
        //do we want just 32 channels?
        
		FMOD_System_Set3DSettings(sys, 10.0f, 10.0f, 10.0f);
        FMOD_System_GetMasterChannelGroup(sys, &channelgroup);
        bFmod3DInitialized = true;
    }
	
}

void SoundBox::loadVideo(string fileName){
	player.loadMovie(fileName);

	w = 110;
	h = 160;

	video_tex.allocate(w,h,GL_RGB);
	video_tex = player.getTextureReference();
	video_tex.bind();
	player.play();
}

bool SoundBox::loadSound(string fileName, bool stream)
{
    result = FMOD_System_CreateSound(sys, ofToDataPath(fileName).c_str(), FMOD_3D, NULL, &sound);
    result = FMOD_Sound_Set3DMinMaxDistance(sound, 1.f, 5000.0f);

    if (result != FMOD_OK){
        bLoadedOk = false;
        printf("ofSoundPlayer: Could not load sound file %s \n", fileName.c_str() );
    } else {
        bLoadedOk = true;
        FMOD_Sound_GetLength(sound, &length, FMOD_TIMEUNIT_PCM);
        isStreaming = stream;
    }
	return bLoadedOk;
}

void SoundBox::play(){

    FMOD_System_PlaySound(sys, FMOD_CHANNEL_FREE, sound, bPaused, &channel);
    FMOD_Channel_Set3DAttributes(channel, &sound_position, &sound_velocity);
    FMOD_Channel_GetFrequency(channel, &internalFreq);
    FMOD_Channel_SetVolume(channel,volume);
	//if(!player.isPlaying())
	//	player.play();
}

void SoundBox::stop(){
	FMOD_Channel_Stop(channel);
}

void SoundBox::updateSound( ofVec3f box_location, ofVec3f box_velocity )
{

		sound_position.x = box_location.x/100;
		sound_position.y = box_location.y/100;
		sound_position.z = box_location.z/100;
		sound_velocity.x = box_velocity.x;
		sound_velocity.y = box_velocity.y;
		sound_velocity.z = box_velocity.z;
		
		cout << box_location << endl;

		FMOD_Channel_Set3DAttributes(channel, &sound_position, &sound_velocity);
}

void SoundBox::updateListener(ofVec3f position, ofVec3f velocity, ofVec3f forward, ofVec3f up)
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
	
	FMOD_System_Set3DListenerAttributes(sys, 0, &listener_position, &listener_velocity, &listener_forward, &listener_up);

}

void SoundBox::update()
{
	FMOD_System_Update(sys);
}

void SoundBox::setSelected(bool selected){
	is_selected = selected;
}