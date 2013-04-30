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
testApp::testApp() : _random(125042)
{
}

void testApp::setup(){	

	//set up tracker
	setupTracker();
	tilt_offset = 0;
	roll_offset = 0;
	pan_offset = 0;

	//translate so the center of the screen is 0,0 before doing anything else
	ofTranslate(ofGetScreenWidth()/2, ofGetScreenHeight()/2, 0);

	glEnable(GL_DEPTH_TEST);
	ofEnableAlphaBlending();
	ofSetSmoothLighting(true);
	ofEnableLighting();
	ofEnableSmoothing();

	user_height = 180;
	box_distance = 400;
	selected = -1;
	pan = 0;
	tilt = 0;
	roll = 0;
	total_pan = pan;
	total_tilt = tilt;
	total_roll = roll;
	ofBackground(0);
	light_color.set(255,255,255);
	moving = false;
	ask_for_file = false;
	wait_for_file = false;;
	file_received = false;
	cue_is_visual = false;
	spin = 0;
	blue.set(51,181,229);
	yellow.set(255,187,51);
	red.set(255,68,68);

	shapes.push_back("circle");
	shapes.push_back("square");
	shapes.push_back("triangle");
	shape_colors.push_back("blue");
	shape_colors.push_back("yellow");
	shape_colors.push_back("red");

	pointLight.setPosition(0,0,0);
	pointLight.setDiffuseColor(light_color);
	pointLight.setSpecularColor(light_color);

	listener_velocity.set(0, 0, 0);
	listener_up.set(0, 1, 0);
	listener_forward.set(0, 0, 1);
	current_forward.set(0,0,0);
	current_up.set(0,0,0);
	listener_position.set(0, 0, 0);

	sound_files.push_back("0_red_square.wav");
	sound_files.push_back("1_red_square.wav");
	sound_files.push_back("2_red_square.wav");
	sound_files.push_back("3_red_square.wav");
	sound_files.push_back("4_red_square.wav");

	video_files.push_back("0_red_square.mov");
	video_files.push_back("1_red_square.mov");
	video_files.push_back("2_red_square.mov");
	video_files.push_back("3_red_square.mov");
	video_files.push_back("4_red_square.mov");

	retries = 0;
	retry = false;
	//set up camera
	cam.setPosition(ofVec3f(0,0,0));

	listener.updateListener(listener_position, listener_velocity, listener_forward, listener_up);

	//set up listening port for UDP messages
	receiver.setup(PORT_IN);
	sender.setup(HOST, PORT_OUT);
	//create the talking heads.
	setupSession();
}

//--------------------------------------------------------------
void testApp::update(){

	spin += .2;


	if(ask_for_file) askFile();
	
	if(wait_for_file) {
		awaitFile();
	} else {
		clearUDPMessages();
	}

	//listener_forward.set(-sin(ofDegToRad(pan)), 0, cos(ofDegToRad(pan)));
	//listener_up.set(0, cos(ofDegToRad(tilt)), sin(ofDegToRad(tilt)));
	listener_forward.set(current_forward);
	listener_up.set(current_up);
	listener.updateListener(listener_position, listener_velocity, listener_forward, listener_up);

	if (moving){
		positionSoundBox();
		soundboxes[selected]->setNewLocation(box_loc, box_rotation);
		soundboxes[selected]->updateSound(box_loc, box_vel);
		//	soundboxes[selected]->play();
	}

	for(auto box = soundboxes.begin(); box != soundboxes.end(); box++){
	/*	if(!(*box)->getIsPlaying() && retry){
			(*box)->play();
			retries++;
			retry = false;
			cout << "retry" << endl;
		}*/
		
		float y_rot = (*box)->getBoxRotation().y;
		float z_loc = (*box)->getBoxLocation().z;
		
		if(pan > y_rot -10 && pan < y_rot + 10){	
			cursor_material.setEmissiveColor(ofColor(blue));
			(*box)->setSelected(true);
			selectSoundBox();
			break;
		} else { 
			cursor_material.setEmissiveColor(ofColor(red));
			(*box)->setSelected(false);
		}
		
	}

	//wrap pan around
	/*if(pan > 360) {
		pan = 0;
	} else if(pan < 0) { 
		pan = 360;
	}*/

	if(soundboxes.size() > 0)
		soundboxes[0]->update();

	sendUDPMessages();
	cue_is_visual = false;
}

//--------------------------------------------------------------
void testApp::draw(){

	cam.begin();
	pointLight.enable();
	ofRotateX(tilt);
	ofRotateZ(roll);
	//drawCursor();
	ofRotateY(pan);
	drawGrid();

	ofPushMatrix();
	ofTranslate(0,-user_height+10, 0);
	ofRotateX(90);
	
	
	ofDisableLighting();
	//floor_mat.setSpecularColor(ofColor(153,204,0));

	ofSetColor(153,204,0,100);
	ofCircle(0,0,1200);
	ofEnableLighting();

	ofPopMatrix();

	if(condition == 1) visualCue();

	//create all boxes
	for(auto box = soundboxes.begin(); box != soundboxes.end(); box++){

		(*box)->drawSoundBox();	
	}
	if(file_received == true) drawFile();


	cam.end();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

	switch (key)
	{
	case WASD_LEFT:
		pan--;
		//cout << listener_forward.x << " " <<  listener_forward.z << endl;
		break;
	case WASD_RIGHT:
		pan++;
		//cout << -sin(ofDegToRad(pan)) << " " <<  cos(ofDegToRad(pan)) << endl;
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
	case 'z':
		selectSoundBox();
		break;
	case 'x':
		if(selected >= 0){
			if(moving == false){
				moving = true;
			} else{
				moving = false;
				selected = -1;
			}
		}
		break;
	case 'u':
		iterations = 5;
		condition = 0;
		startNewSession();
		break;
	case 'i':
		iterations = 20;
		condition = 0;
		startNewSession();
		break;
	case 'o':
		iterations = 5;
		condition = 1;
		startNewSession();
		break;
	case 'p':
		iterations = 20;
		condition = 1;
		startNewSession();
		break;
	case 'l':
		retry_audio();
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

	int rotation; 
	//Comment this out when using UDP
	//cout << "Enter a rotation (0-360): ";
	//cin >> rotation;

	rotation = pan;
	box_rotation.set(0, rotation, 0);
	//-------------------------------
	box_distance = -cursor_z;
	box_loc.set(box_distance*sin(ofDegToRad(box_rotation.y)), 0, -box_distance*cos(ofDegToRad(box_rotation.y))); 
	box_color.set(ofRandom(0,255), ofRandom(0,255), ofRandom(0,255));

	SoundBox * box = new SoundBox(box_loc, box_rotation, box_color);

	box->loadVideo(ofToDataPath(video_files[soundboxes.size()]));
	box->loadSound(ofToDataPath(sound_files[soundboxes.size()]));
	box->setVolume(0);
	box->setMultiPlay(true);
	box->updateSound(box_loc, box_vel);
	box->play();
	box->setSelected(true);
	soundboxes.push_back(box);
}

//--------------------------------------------------------------
void testApp::removeSoundBox(){

	int n;
	//Comment this out when using UDP
	//cout << "Enter the box to remove:";
	//cin >> n;
	//-------------------------------

	n = getSelected();

	if (n < soundboxes.size()){
		soundboxes.erase(soundboxes.begin() + n);
		//add the sound file at the back of the vector and remove it from the original location.
		sound_files.push_back(sound_files[n]);
		sound_files.erase(sound_files.begin() + n);
		video_files.push_back(video_files[n]);
		video_files.erase(video_files.begin() + n);
		item_receiver = -1;
		//do I need a destructor?
	} else {
		cout << "specified box does not exist" << endl;
	}
}

//--------------------------------------------------------------
void testApp::positionSoundBox(){
	box_distance = -cursor_z;
	box_rotation.set(0, pan, 0);
	box_loc.set(box_distance*sin(ofDegToRad(box_rotation.y)), 0, -box_distance*cos(ofDegToRad(box_rotation.y)));

	//	selected = -1;
}

//--------------------------------------------------------------
void testApp::drawGrid(){

	//draw a grid on the floor
	
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
		cursor_material.setEmissiveColor(ofColor(255,0,0,255));
		ofEnableAlphaBlending();

		ofPushMatrix();
		ofTranslate(0,-user_height, cursor_z);
		ofRotateX(90);

		if(tilt < 10) cursor_material.setEmissiveColor(ofColor(255,0,0,ofMap(tilt, 5, 10, 0, 255)));
		ofCircle(0,0,50);
		ofPopMatrix();
		ofDisableAlphaBlending();
		cursor_material.end();
	}
}

//--------------------------------------------------------------
int testApp::getSelected(){
	signed int n = -1;
	for(auto box = soundboxes.begin(); box != soundboxes.end(); box++){		
		n++;
		float y_rot = (*box)->getBoxRotation().y;
		float z_loc = (*box)->getBoxLocation().z;
		//if(pan > y_rot && pan < y_rot && cursor_z < z_loc / cos(ofDegToRad(y_rot)) + 50 && cursor_z > z_loc / cos(ofDegToRad(y_rot)) - 50){
		if(pan > y_rot - 10 && pan < y_rot + 10){
			return n;
		}
	}
	//return -1;
}

//--------------------------------------------------------------
void testApp::selectSoundBox(){
	selected = getSelected();
	//cout << "selected box #: " << selected << endl;
}

//--------------------------------------------------------------
void testApp::rotateToDefault() {


	tilt_offset += -tilt;
	roll_offset += -roll;
	pan_offset += -pan;

}

//--------------------------------------------------------------
void testApp::setupTracker(){

	serial.setup("COM4", 57600);
	serial.startContinuesRead();
	ofAddListener(serial.NEW_MESSAGE,this,&testApp::onNewMessage);

}

//--------------------------------------------------------------
void testApp::onNewMessage(string & message)
{

	static float prev_pan;
	static float prev_tilt;
	static float prev_roll;

    vector<string> initial = ofSplitString(message, "=");
    if (initial.size()==2)
    {
        
    vector<string> input = ofSplitString(initial[1], ",");

        if (input.size() == 3){
 

            pan = ofToFloat(input[0]) + 90 + pan_offset;
            roll = -ofToFloat(input[1]) + roll_offset;
            tilt = -ofToFloat(input[2]) - 90 + tilt_offset;
			//cout << "pan: " << pan << " pan_offset: " << pan_offset << " tilt: " << tilt << " tilt_offset: " << tilt_offset << " roll: " << roll << " roll_offset: " << roll_offset << endl;
			
			total_pan += abs(pan - prev_pan);
			total_tilt += abs(tilt - prev_tilt);
			total_roll += abs(roll - prev_roll);

			cout << "pan: " << pan << " pan_c: " << total_pan << endl;
			prev_pan = pan;
			prev_tilt = tilt;
			prev_roll = roll;
			
			ofVec3f new_forward(0,0,1);
			current_forward = new_forward.rotate(tilt, -pan, roll);
			ofVec3f new_up(0,1,0);
			current_up = new_up.rotate(tilt, -pan, roll);

        }   else {
            cout << "message size: " <<input.size() << endl;
        }
    }
    
}

//--------------------------------------------------------------
void testApp::retry_audio(){
	int _random_questioner = random_questioner;
	if(_random_questioner == -1) _random_questioner = 1;
	if(!soundboxes[_random_questioner]->getIsPlaying()){
			soundboxes[_random_questioner]->play();
			retries++;
			cout << "retry" << endl;
	}
}

//--------------------------------------------------------------
void testApp::askFile(){
	ask_for_file = false;
	cue_is_visual = true;

	if(iterations > 0){
		iterations--;
		startTimer();
		task_tag = XML.addTag("TASK");

		//ask for a file. randomly select the VP that will ask and the requested object
		std::tr1::uniform_int_distribution<int> questioner_dist(0, soundboxes.size());
		random_questioner = questioner_dist(_random)-1;

		std::tr1::uniform_int_distribution<int> shape_dist(0, shapes.size()-1);
		switch (shape_dist(_random))
		{
		case 0:
			random_shape = "circle";
			break;
		case 1:
			random_shape = "triangle";
			break;
		case 2:
			random_shape = "square";
			break;
		}
		std::tr1::uniform_int_distribution<int> color_dist(0, shape_colors.size()-1);
		switch (color_dist(_random))
		{
		case 0:
			random_color = "blue";
			break;
		case 1:
			random_color = "yellow";
			break;
		case 2:
			random_color = "red";
			break;
		}
		//cout << "random_questioner: " << random_questioner << "  random_shape: " << random_shape << "  random_color: " << random_color << endl;
		
		wait_for_file = true;
		XML.pushTag("TASK", task_tag);
		int _tag_num = XML.addTag("Asked");
		ostringstream convert;
		convert << random_questioner;
		string target_string = convert.str();
		sendToXML("Asked", target_string + "_" + random_color + "_" + random_shape, _tag_num);

		if(condition == 0) startAudioCue();

	} else {
		endSession();
	}
}

//--------------------------------------------------------------
void testApp::awaitFile(){
	getUDPMessages();
	//start timer
}

//--------------------------------------------------------------
void testApp::drawFile(){
	if(item_receiver >= 0 && ofGetElapsedTimef() - saved_time < 5.0){
		float rot = -soundboxes[item_receiver]->getBoxRotation().y;
		float loc = -soundboxes[item_receiver]->getBoxLocation().z;
		shapefile->drawShapeFile(spin, rot, loc / cos(ofDegToRad(rot)));

	} else if(item_receiver == -1 && ofGetElapsedTimef() - saved_time < 5.0) {
		shapefile->drawShapeFile(spin, -pan, 250);
	}

}

//--------------------------------------------------------------
void testApp::checkCorrect(){

	//check target, color and shape.
	XML.pushTag("TASK", task_tag);
	int _tag_num = XML.addTag("Result");
	cout << item_receiver << " : " << random_questioner << "   " << shape << " : " << random_shape << "   " << shape_color << " : " << random_color << endl;
	if(item_receiver == random_questioner && shape == random_shape && shape_color == random_color){
		//correct!
		cout << "correct!" << endl;
		result_tone.loadSound("correct.wav");
		result_tone.setVolume(.2);
		result_tone.play();
		sendToXML("Result", 1, _tag_num);

	} else {
		//wrong!
		cout << "wrong!" << endl;
		result_tone.loadSound("wrong.wav");
		result_tone.setVolume(.2);
		result_tone.play();
		sendToXML("Result", 0, _tag_num);
	}
	
	XML.pushTag("TASK", task_tag);
	int _retry_tag = XML.addTag("Retries");
	sendToXML("Retries", retries, _retry_tag);
	XML.saveFile("mySettings.xml");

	XML.pushTag("TASK", task_tag);
	int _total_pan = XML.addTag("Pan");
	sendToXML("Pan", total_pan, _total_pan);

	XML.pushTag("TASK", task_tag);
	int _total_tilt = XML.addTag("Tilt");
	sendToXML("Tilt", total_tilt, _total_tilt);
	
	XML.pushTag("TASK", task_tag);
	int _total_roll = XML.addTag("Roll");
	sendToXML("Roll", total_roll, _total_roll);

	XML.saveFile("mySettings.xml");

	wait_for_file = false;
	ask_for_file = true;
	retries = 0;
	total_pan = 0.0f;
	total_tilt = 0.0f;
	total_roll = 0.0f;

}

//--------------------------------------------------------------
void testApp::getUDPMessages(){
	//by default, reset the target to -1 (=the environment)
	target = -1;
	// check for waiting messages
	while(receiver.hasWaitingMessages()){
		// get the next waiting message
		ofxOscMessage message;
		receiver.getNextMessage(&message);

		//check if the message has the address we're expecting
		if(message.getAddress() == "object"){
			
			//retrieve the arguments
			shape_color = message.getArgAsString(0);
			shape = message.getArgAsString(1);
							
			shapefile.reset(new ShapeFile(shape_color, shape));
				
			target = message.getArgAsInt32(2);
			shapefile.reset(new ShapeFile(shape_color, shape, target));
			if(target == -2){
				item_receiver = getSelected();
			} else{
				item_receiver = target;
			}
			
			//push into the TASK tag and set the data of the received element up for writing to XML
			XML.pushTag("TASK", task_tag);
			int _tag_num = XML.addTag("Received");
			ostringstream convert;
			convert << item_receiver;
			string target_string = convert.str();
			sendToXML("Received", target_string + "_" + shape_color + " " + shape, _tag_num);
			
			//stop timer
			stopTimer();

			//check if the answer is correct
			checkCorrect();
			
			//end the listening and clear the message
			file_received = true;
			message.clear();

		} else if(message.getAddress() == "retry"){
			retry_audio();	
		}

		/*	if(message.getAddress() == "Location"){
		float mapped_value = ofMap(message.getArgAsFloat(0), 0, 1, -1, 1);
		cout << mapped_value << endl;
		box_rotation.set(0, -mapped_value*180, 0);


		} else if(message.getAddress() == "Place" && message.getArgAsInt32(0) == 1){
		addSoundBox();
		} else if(message.getAddress() == "Remove" && message.getArgAsInt32(0) == 1){
		removeSoundBox();
		}

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

	//	cout << "shape: " << shape << " color: " << shape_color << " target: " << target << endl;

}

//--------------------------------------------------------------
void testApp::clearUDPMessages(){
	while(receiver.hasWaitingMessages()){
		//get the message as if we are using it, but don't do anything with it. This clears the queue.
		ofxOscMessage message;
		receiver.getNextMessage(&message);
	}
}

//--------------------------------------------------------------
void testApp::sendUDPMessages(){
	ofxOscMessage m;
	m.setAddress("rotation");
	m.addFloatArg(pan);
	if(cue_is_visual){
		m.setAddress("object");
		m.addIntArg(random_questioner);
		m.addStringArg(random_color);
		m.addStringArg(random_shape);
			cout << "Sent" << endl;
	}
	sender.sendMessage(m);

}

//--------------------------------------------------------------
void testApp::sendToXML(string tag, float result, int tag_num){

	XML.setValue(tag, result, tag_num);
	XML.popTag();
}

//--------------------------------------------------------------
void testApp::sendToXML(string tag, string input, int tag_num){

	XML.setValue(tag, input, tag_num);
	XML.popTag();
}

//--------------------------------------------------------------
void testApp::startTimer(){
	saved_time = ofGetElapsedTimef();
}

//--------------------------------------------------------------
void testApp::stopTimer(){
	XML.pushTag("TASK", task_tag);
	int _tag_num = XML.addTag("Time");
	float _current_time = ofGetElapsedTimef();
	float _task_time = _current_time - saved_time;
	sendToXML("Time", _task_time, _tag_num);
	cout << "task time: " << _task_time << "  saved_time: " << saved_time << "  time: " << _current_time << endl;
}
 
//--------------------------------------------------------------
void testApp::setupSession(){
	std::vector<int> rotation; 
	rotation.push_back(10);
	rotation.push_back(80);
	rotation.push_back(160);
	rotation.push_back(240);
	rotation.push_back(320);

	for(int i = 0; i < rotation.size(); i++){
	box_rotation.set(0, rotation[i], 0);
	//-------------------------------
	box_loc.set(box_distance*sin(ofDegToRad(box_rotation.y)), 0, -box_distance*cos(ofDegToRad(box_rotation.y))); 
	box_color.set(ofRandom(0,255), ofRandom(0,255), ofRandom(0,255));

	SoundBox * box = new SoundBox(box_loc, box_rotation, box_color);

	box->loadVideo(ofToDataPath(video_files[soundboxes.size()]));
	box->loadSound(ofToDataPath(sound_files[soundboxes.size()]));
	box->setVolume(0);
	box->setMultiPlay(true);
	box->updateSound(box_loc, box_vel);
	box->play();
	soundboxes.push_back(box);
	}
}

//--------------------------------------------------------------
void testApp::startNewSession(){

	ofResetElapsedTimeCounter();
	session_tag = XML.addTag("SESSION");
	XML.pushTag("SESSION", session_tag);
	ask_for_file = true;
	cout << "started session" << endl;
}

//--------------------------------------------------------------
void testApp::endSession(){
	condition = -1;
	int _tag_num = XML.addTag("SessionTime");
	float _current_time = ofGetElapsedTimef();
	sendToXML("SessionTime", _current_time, _tag_num);
	XML.saveFile("mySettings.xml");
	XML.popTag();
	cout << "session ended" << endl;
}

//--------------------------------------------------------------
void testApp::visualCue(){
	
	ofMaterial cue_material;
	cue_material.begin();
	cue_material.setShininess(0);
	
	if(random_color == "blue"){
		ofSetColor(blue);
		cue_material.setEmissiveColor(blue);
	} else if(random_color == "yellow"){
		ofSetColor(yellow);
		cue_material.setEmissiveColor(yellow);
	} else if (random_color == "red"){
		ofSetColor(red);
		cue_material.setEmissiveColor(red);
	}
	
	ofPushMatrix();

	float rotY;
	float rotX = 0.0;
	float locZ;
	float locY = 0.0;
	int size;

	if(random_questioner >= 0){
	rotY = -soundboxes[random_questioner]->getBoxRotation().y;
	locZ = -soundboxes[random_questioner]->getBoxLocation().z;
	ofRotateY(rotY);
	ofTranslate(0, 140,-(locZ / cos(ofDegToRad(rotY))));
	size = 40;
	} else {
		rotY = -pan;
		rotX = -90;
		locZ = 40.0;
		locY = -20;
		size = 10;
		ofRotateY(rotY);
		ofTranslate(0,locY, -locZ);
		ofRotateX(rotX);
	}

	
	if(random_shape == "circle"){
		ofCircle(0,-size/2,size/2);
		
	} else if(random_shape == "triangle"){ 
		glBegin(GL_TRIANGLES);
			glVertex2f(0,0);
			glVertex2f(-size/2, -size);
			glVertex2f(size/2, -size);
		glEnd();
	} else if (random_shape == "square"){
		ofRect(-size/2,-40,size,size);
	}
	ofPopMatrix();
	cue_material.end();

}

//--------------------------------------------------------------
void testApp::startAudioCue(){
	cue_is_visual = false;
	int _random_questioner = random_questioner;
	std::stringstream audio_cue_file; 
	//audio_cue_file << random_questioner << "_" << random_color << "_" << random_shape;
	audio_cue_file << random_questioner << "_" << random_color << "_" << random_shape; //change this back to ^!!!!
	if(_random_questioner == -1) _random_questioner = 1;
	soundboxes[_random_questioner]->loadSound(ofToDataPath(audio_cue_file.str() + ".wav"));
	soundboxes[_random_questioner]->loadVideo(ofToDataPath(audio_cue_file.str() + ".mov"));
	
	soundboxes[_random_questioner]->play();
	soundboxes[_random_questioner]->setVolume(1);
	cout << audio_cue_file.str() << endl;
}

//--------------------------------------------------------------
void testApp::endAudioCue(){

}