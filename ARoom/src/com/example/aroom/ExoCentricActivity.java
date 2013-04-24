package com.example.aroom;

import java.net.ConnectException;
import java.lang.Math;

import android.app.Activity;
import android.opengl.Visibility;
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

public class ExoCentricActivity extends Activity {

	private final static String IP_ADDRESS = "192.168.43.224";

	private ImageView bluesquare, redsquare, yellowsquare, bluecircle,
			redcircle, yellowcircle, bluetriangle, redtriangle, yellowtriangle,
			radarcenter, person_1, person_2, person_3, cue;

	OSCConnection connection = new OSCConnection();
	private DataPackage data_in = new DataPackage();
	private boolean visual_cue = false;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		requestWindowFeature(Window.FEATURE_NO_TITLE);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
				WindowManager.LayoutParams.FLAG_FULLSCREEN);

		setContentView(R.layout.exocentric);

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

		person_1 = (ImageView) findViewById(R.id.person1);
		person_1.setOnDragListener(new DragListener());
		person_2 = (ImageView) findViewById(R.id.person2);
		person_2.setOnDragListener(new DragListener());
		person_3 = (ImageView) findViewById(R.id.person3);
		person_3.setOnDragListener(new DragListener());

		radarcenter = (ImageView) findViewById(R.id.radarcenter);
		radarcenter.setOnDragListener(new DragListener());

		// connect to the PC for OSC communication
		try {
			connection.connect(IP_ADDRESS + ":8001");
		} catch (ConnectException e) {
			Log.i("osc con", "no connection");
		}
		Log.i("activity", "CREATED");

		Handler m_handler = new Handler(Looper.getMainLooper()) {

			@Override
			public void handleMessage(Message msg) {

				data_in = (DataPackage) msg.obj;

				if (msg.what == 1) {
					float rot = data_in.getRotation();
					int target = data_in.getTarget();
					setRotations(rot, target);
				} else if (msg.what == 2) {

					Log.i("data", "shape change");

					String shape_color = data_in.getShapeColor();
					String shape = data_in.getShape();
					if (cue != null && cue.getVisibility() == View.VISIBLE) {
						cue.setVisibility(View.INVISIBLE);
					}
					showVisualCue(shape_color, shape);
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

	void setRotations(float rot, int target) {
		// rotations: [0] for target -1, [1] for target 0, etc.
		int[] rotations = { 0, 10, 80, 240 };

		float y_translation_1 = -300
				* (float) Math.cos(Math.toRadians(rot - rotations[1]));
		float x_translation_1 = -300
				* (float) Math.sin(Math.toRadians(rot - rotations[1]));
		float y_translation_2 = -300
				* (float) Math.cos(Math.toRadians(rot - rotations[2]));
		float x_translation_2 = -300
				* (float) Math.sin(Math.toRadians(rot - rotations[2]));
		float y_translation_3 = -300
				* (float) Math.cos(Math.toRadians(rot - rotations[3]));
		float x_translation_3 = -300
				* (float) Math.sin(Math.toRadians(rot - rotations[3]));

		person_1.setTranslationY(y_translation_1);
		person_1.setTranslationX(x_translation_1);
		person_2.setTranslationY(y_translation_2);
		person_2.setTranslationX(x_translation_2);
		person_3.setTranslationY(y_translation_3);
		person_3.setTranslationX(x_translation_3);

		if (cue != null) {
			float y_translation = -100;
			float x_translation = 0;

			if (rotations[target+1] != 0) {
				y_translation = -300
						* (float) Math.cos(Math.toRadians(rot
								- rotations[target + 1]));
				x_translation = -300
						* (float) Math.sin(Math.toRadians(rot
								- rotations[target + 1]));

			}

			cue.setTranslationX(x_translation);
			cue.setTranslationY(y_translation);
			if (cue.getVisibility() == View.INVISIBLE) {
				cue.setVisibility(View.VISIBLE);
			}

		}
	}

	void showVisualCue(String shape_color, String shape) {

		String itemID = "small" + shape_color + shape;
		int resID = getResources().getIdentifier(itemID, "id",
				"com.example.aroom");
		cue = (ImageView) findViewById(resID);
		// cue.setVisibility(View.VISIBLE);

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

				String[] split = idname.split(":");
				String send_target = (String) drop_target.getTag();
				connection.send(split[0], split[1],
						Integer.parseInt(send_target)); // target here
				Log.i("out", send_target);
				break;
			case DragEvent.ACTION_DRAG_ENDED:

			default:
				break;
			}
			return true;
		}

	}

}
