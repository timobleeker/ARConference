package com.example.aroom;

import android.content.ClipData;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.DragShadowBuilder;
import android.view.View.OnTouchListener;

public class MyTouchListener implements OnTouchListener {
	
	public boolean onTouch(View view, MotionEvent motionEvent){
		
		if (motionEvent.getAction() == MotionEvent.ACTION_DOWN){
			ClipData data = ClipData.newPlainText("", "");
			DragShadowBuilder shadowBuilder = new View.DragShadowBuilder(view);
			view.startDrag(data, shadowBuilder, view, 0);
			return true;
		} else{
		return false;
	}
}
}
