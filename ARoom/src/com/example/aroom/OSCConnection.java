package com.example.aroom;

import java.io.IOException;
import java.net.ConnectException;
import java.net.InetAddress;
import java.util.Date;

import android.database.CursorJoiner.Result;
import android.os.AsyncTask;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import com.illposed.osc.OSCListener;
import com.illposed.osc.OSCMessage;
import com.illposed.osc.OSCPortIn;
import com.illposed.osc.OSCPortOut;

public class OSCConnection {

	static final int ROTATION_MESSAGE = 1;
	static final int OBJECT_MESSAGE = 2;
	String server = null;
	int portout = 8001; // default port
	int portin = 8002;
	float rotation = 0;
	private OSCPortOut sender = null;
	private OSCPortIn receiver = null;

	public static Handler message_handler;
	private DataPackage incoming_data = new DataPackage();

	public OSCConnection() {

	}

	public void connect(String serverAddress) throws ConnectException {

		if (server == null) {
			String[] split = serverAddress.split(":");
			if (split != null && split.length == 2) {
				server = split[0];
				portout = Integer.valueOf(split[1]);
				Log.i("osc", "used IP");
			} else {
				server = serverAddress;
				portout = 8001;
				Log.i("osc", "used null");
			}
			try {
				sender = new OSCPortOut(InetAddress.getByName(server), portout);
				receiver = new OSCPortIn(portin);
				OSCListener rotation_listener = new OSCListener() {
					public void acceptMessage(java.util.Date time,
							OSCMessage message) {
						receive(message);
					}
				};
				receiver.addListener("rotation", rotation_listener);
				OSCListener object_listener = new OSCListener() {
					
					@Override
					public void acceptMessage(Date time, OSCMessage message) {
						receive_object(message);
						
					}
				};
				
				receiver.addListener("object", object_listener);
				receiver.startListening();
				Log.i("osc", "Connection Established");
			} catch (Exception e) {
				Log.i("osc", e.toString());
				throw new ConnectException(e.toString());
			}
		}
	}

	protected void disconnect() {
		if (sender != null) {
			sender.close();
			sender = null;
			receiver.stopListening();
			receiver.close();
			receiver = null;
			Log.i("osc", "DISCONNECTED");
		}
	}

	private void receive(OSCMessage message) {
		Object args[] = message.getArguments();
		Message msg = message_handler.obtainMessage();

		msg.what = ROTATION_MESSAGE;
		incoming_data.setRotationData((Float) args[0]);

/*		if (args.length == 4) {
			incoming_data.setIncomingData((Float) args[0], (Integer) args[1],
					(String) args[2], (String) args[3]);
			incoming_data.setIsUpdated(true);
			Log.i("data", "updated");
		} else {
			Log.i("data", "NOT updated");
			incoming_data.setIsUpdated(false);
		}*/

		msg.obj = (DataPackage) incoming_data;
		message_handler.sendMessage(msg);
	}
	
	private void receive_object(OSCMessage message){
		Object args[] = message.getArguments();
		Message msg = message_handler.obtainMessage();
		msg.what = OBJECT_MESSAGE;
		incoming_data.setIncomingData((Float) args[0], (Integer) args[1], (String) args[2], (String) args[3]);
		
		msg.obj = (DataPackage) incoming_data;
		message_handler.sendMessage(msg);
	}

	protected void send(String shape_color, String shape, int target) {

		new OSCAsyncSend().execute(shape_color, shape, target);
	}

	private class OSCAsyncSend extends AsyncTask<Object, Void, Result> {

		@Override
		protected Result doInBackground(Object... objects) {

			OSCMessage msg = new OSCMessage();

			msg.setAddress("object");
			msg.addArgument((String) objects[0]);
			msg.addArgument((String) objects[1]);
			msg.addArgument((Integer) objects[2]);

			try {
				sender.send(msg);
				Log.i("osc msg", "Message Sent");
			} catch (IOException e) {
				Log.i("osc msg", "Could not send");
			}
			return null;
		}

	}

}
