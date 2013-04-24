package com.example.aroom;

public class DataPackage {
	
	private float rotation;
	private int target = -1;
	private String shape_color;
	private String shape;
	private boolean updated;
	
	protected void setIncomingData(float _rotation, int _target, String _color, String _shape){
		rotation = _rotation;
		target = _target;
		shape_color = _color;
		shape = _shape;
	}
	
	protected void setRotationData(float _rotation){
		rotation = _rotation;
	//	shape_color = null;
	//	shape = null;
	}
	
	protected void setIsUpdated(boolean _updated){
		updated = _updated;
	}
	
	protected boolean getIsUpdated(){
		return updated;
	}
	
	protected float getRotation(){
		return rotation;
	}
	protected int getTarget(){
		return target;
	}
	protected String getShapeColor(){
		return shape_color;
	}
	protected String getShape(){
		return shape;
	}
}
