package BluetoothWorker;

import android.bluetooth.BluetoothSocket;
import android.os.Handler;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * Created by Valeria on 09.04.2017.
 */
public class BluetoothReader extends Thread{
    private final BluetoothSocket bluetoothSocket;
    private final InputStream inputStream;
    private final OutputStream outputStream;
    private Handler handler;
    private static final String TAG = "BluetoothReader";

    public BluetoothReader(BluetoothSocket socket, Handler handler) {
        this.bluetoothSocket = socket;
        this.handler = handler;

        InputStream inpStream = null;
        OutputStream outStream = null;

        try {
            inpStream = this.bluetoothSocket.getInputStream();
            outStream = this.bluetoothSocket.getOutputStream();
        } catch (IOException e) { }

        this.inputStream = inpStream;
        this.outputStream = outStream;
    }

    public void run() {
        byte[] buffer = new byte[256];
        int bytes;

        while (true) {
            try {
                bytes = this.inputStream.read(buffer);
                this.handler.obtainMessage(BluetoothHandler.getRECIEVE_MESSAGE(), bytes, -1, buffer).sendToTarget();     // Отправляем в очередь сообщений Handler
            } catch (IOException e) {
                break;
            }
        }
    }


    public boolean write(String message) {
        byte[] msgBuffer = message.getBytes();
        try {
            this.outputStream.write(msgBuffer);
        } catch (IOException e) {
            Log.d(TAG, "...Error data sending: " + e.getMessage() + "...");
            return false;
        }
        return true;
    }

    /* Call this from the main activity to shutdown the connection */
    public void cancel() {
        try {
            this.bluetoothSocket.close();
        } catch (IOException e) { }
    }
}