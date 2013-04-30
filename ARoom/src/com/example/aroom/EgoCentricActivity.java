package com.example.aroom;

import java.net.ConnectException;

import com.example.aroom.ExoCentricActivity.DragListener;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.view.DragEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ImageView;
import android.widget.Toast;

public class EgoCentricActivity extends Activity {

	private String IP_ADDRESS = "10.32.11.206";
	
	private ImageView bluesquare, redsquare, yellowsquare, bluecircle,
			redcircle, yellowcircle, bluetriangle, redtriangle, yellowtriangle,
			toparea, bottomarea, cue;
	
	OSCConnection connection = new OSCConnection();
	private DataPackage data_in = new DataPackage();

	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		IP_ADDRESS = getIntent().getExtras().getString("ip");
		
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
				WindowManager.LayoutParams.FLAG_FULLSCREEN);

		setContentView(R.layout.egocentric);

		bluesquare = (ImageView) findViewById(R.id.bluesquare);
		bluesquare.setOnTouchListener(new MyTouchListener());
		redsquare = (ImageView) findViewById(R.id.redsquare);
		redsquare.setOnTouchListener(new MyTouchListener());
		yellowsquare = (ImageView) findViewById(R.id.yellowsquare);
		yellowsquare.setOnTouchListener(new MyTouchListener());
		bluecircle = (ImageView) findViewById(R.id.bluecircle);
		bluecircle.setOnTouchListener(new MyTouchListener());
		redcircle = (ImageView) findViewById(R.id.redcircle);
		redcircle.setOnTouchListener(new MyTouchListener());
		yellowcircle = (ImageView) findViewById(R.id.yellowcircle);
		yellowcircle.setOnTouchListener(new MyTouchListener());

		bluetriangle = (ImageView) findViewById(R.id.bluetriangle);
		bluetriangle.setOnTouchListener(new MyTouchListener());
		redtriangle = (ImageView) findViewById(R.id.redtriangle);
		redtriangle.setOnTouchListener(new MyTouchListener());
		yellowtriangle = (ImageView) findViewById(R.id.yellowtriangle);
		yellowtriangle.setOnTouchListener(new MyTouchListener());

		// findViewById(R.id.redsquare).setOnTouchListener(new
		// MyTouchListener());
		toparea = (ImageView) findViewById(R.id.toparea);
		toparea.setOnDragListener(new DragListener());
		bottomarea = (ImageView) findViewById(R.id.bottomarea);
		bottomarea.setOnDragListener(new DragListener());
		// findViewById(R.id.bottomarea).setOnDragListener(new DragListener());

		
		try {
			connection.connect(IP_ADDRESS + ":8001");
		} catch (ConnectException e) {
			// TODO Auto-generated catch block
			Log.i("osc con", "no connection");
		}
		
		Handler m_handler = new Handler(Looper.getMainLooper()) {

			@Override
			public void handleMessage(Message msg) {

				data_in = (DataPackage) msg.obj;

				if (msg.what == 2) {

					Log.i("data", "shape change");
					int target = data_in.getTarget();
					String shape_color = data_in.getShapeColor();
					String shape = data_in.getShape();

					showVisualCue(shape_color, shape, target);
				}
				if (cue != null && cue.getVisibility() == View.VISIBLE) {
					cue.setVisibility(View.INVISIBLE);
				}

			}
		};

		OSCConnection.message_handler = m_handler;
	}
	
	@Override
	protected void onPause() {
		super.onPause();
		connection.disconnect();
		Log.i("activity", "PAUSED");

	}

	@Override
	protected void onResume() {
		super.onResume();
		Log.i("activity", "RESUMED");
		try {
			connection.connect(IP_ADDRESS + ":8001");
		} catch (ConnectException e) {
			// TODO Auto-generated catch block
			Log.i("osc con", "no connection");
		}
	}

	void showVisualCue(String shape_color, String shape, int target) {
		String itemID = "small" + shape_color + shape;
		int resID = getResources().getIdentifier(itemID, "id",
				"com.example.aroom");
		cue = (ImageView) findViewById(resID);
		//if(cue != null && cue.getVisibility() == View.INVISIBLE){
			Log.i("cue", "I should draw now");
			cue.setVisibility(View.VISIBLE);
			cue.setTranslationX(190*target);
		//}
	}
	
	
	class DragListener implements View.OnDragListener {

		@Override
		public boolean onDrag(View v, DragEvent event) {
			int action = event.getAction();
			switch (action) {
			case DragEvent.ACTION_DRAG_STARTED:
				break;
			case DragEvent.ACTION_DROP:
				View view = (View) event.getLocalState();
				ImageView drop_target = (ImageView) v;
				ImageView dropped = (ImageView) view;
				String idname = (String) dropped.getTag();
				Toast.makeText(getApplicationContext(), idname,
						Toast.LENGTH_SHORT).show();
				
				String send_target = (String) drop_target.getTag();
				String[] split = idname.split(":");

				connection.send(split[0], split[1], Integer.parseInt(send_target));
				break;
			case DragEvent.ACTION_DRAG_ENDED:

			default:
				break;
			}
			return true;
		}

	}

}
