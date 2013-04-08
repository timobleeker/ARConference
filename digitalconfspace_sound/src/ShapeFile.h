#pragma once

#include "ofMain.h"

class ShapeFile{

	protected:
		ofColor _shape_color;
		string  _shape;
		int		_target;
		ofMaterial shapeMaterial;
	public:
	
	ShapeFile(string shape_color, string shape);
	ShapeFile(string shape_color, string shape, int target);
		
	void drawShapeFile(float spin, float rot, float loc);
	




};