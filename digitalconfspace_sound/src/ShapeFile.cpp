#include "ShapeFile.h"

ShapeFile::ShapeFile(string shape_color, string shape){

	if(shape_color == "blue"){
		_shape_color = ofColor(51,181,229);
	} else if(shape_color == "yellow"){
		_shape_color = ofColor(255,187,51);
	} else if (shape_color == "red"){
		_shape_color = ofColor(255,68,68);
	}

	_shape = shape;
}

ShapeFile::ShapeFile(string shape_color, string shape, int target){

	if(shape_color == "blue"){
		_shape_color = ofColor(51,181,229);
	} else if(shape_color == "yellow"){
		_shape_color = ofColor(255,187,51);
	} else if (shape_color == "red"){
		_shape_color = ofColor(255,68,68);
	}

	_shape = shape;
	_target = target;
}

void ShapeFile::drawShapeFile(float spin, float rot, float loc){
	int size = 30;
	ofPushMatrix();
	ofRotateY(rot);
	ofTranslate(0,-120,-loc+20);
	ofRotateY(spin);
	ofTranslate(0,0,size);
	shapeMaterial.begin();
	shapeMaterial.setShininess(4);
	ofSetColor(_shape_color);

	if(_shape == "circle"){
		ofSphere(size/2);
	} else if(_shape == "triangle"){ 
		glBegin(GL_TRIANGLE_FAN);
			glNormal3f(0.0, 1.0, 0.0);
			glVertex3f(0,size,0);

			glNormal3f(-.707, 0.0, .707);
			glVertex3f(-size/2,0,size/2);

			glNormal3f(.707, 0.0, .707);
			glVertex3f(size/2,0,size/2);

			glNormal3f(.707, 0.0, -.707);
			glVertex3f(size/2,0,-size/2);

			glNormal3f(-.707, 0.0, -.707);
			glVertex3f(-size/2,0,-size/2);

			glNormal3f(-.707, 0.0, .707);
			glVertex3f(-size/2,0,size/2);
		glEnd();
	} else if (_shape == "square"){
		ofBox(size);
	}
	shapeMaterial.end();

	ofPopMatrix();

}