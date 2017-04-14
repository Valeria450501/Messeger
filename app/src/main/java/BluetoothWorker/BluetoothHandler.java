package BluetoothWorker;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.os.Handler;
import android.util.Log;
import com.example.valeria.messeger.MainActivity;

import java.io.IOException;
import java.io.OutputStream;
import java.util.UUID;

/**
 * Created by Valeria on 06.04.2017.
 */
public class BluetoothHandler {
    private static final int RECIEVE_MESSAGE = 1;
    private static final String TAG = "BluetoothHandler";
    private static final String MAC_ADDRESS = "20:16:02:25:94:27";
    private static final UUID SPP_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");;
    private BluetoothAdapter bluetoothAdapter = null;
    private BluetoothSocket bluetoothSocket = null;
    private BluetoothReader bluetoothReader;
    private Handler handler;
    private StringBuilder stringBuilder = new StringBuilder();
    private MainActivity mainActivity = null;

    public static int getRECIEVE_MESSAGE() {
        return RECIEVE_MESSAGE;
    }

    public BluetoothHandler(MainActivity mActivity) {
        this.mainActivity = mActivity;
        this.handler = new Handler() {
            public void handleMessage(android.os.Message msg) {
                switch (msg.what) {
                    case RECIEVE_MESSAGE:                                                   // если приняли сообщение в Handler
                        byte[] readBuf = (byte[]) msg.obj;
                        String strIncom = new String(readBuf, 0, msg.arg1);
                        stringBuilder.append(strIncom);                                                // формируем строку
                        int endOfLineIndex = stringBuilder.indexOf("\r\n");
                        // определяем символы конца строки
                        if (endOfLineIndex > 0) {                                            // если встречаем конец строки,
                            String sbprint = stringBuilder.substring(0, endOfLineIndex);               // то извлекаем строку
                            stringBuilder.delete(0, stringBuilder.length());
                            mainActivity.getEditText().setText(sbprint);
                        }
                        break;
                }
            }
        };
        this.bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
    }

    public boolean sendData(String sendingString) {
        Log.d(BluetoothHandler.TAG, "...Sending Data...");
        Log.d(BluetoothHandler.TAG, "Send data: " + sendingString);

        //TODO
        //Check sendingString
        //разбивать на маленькие сообшения по 9 символов
        if(!this.bluetoothReader.write(Integer.toString(sendingString.length())))
            return false;
        if(!this.bluetoothReader.write(sendingString))
            return false;

        return true;
    }

    /*public String readString(){
        Log.d(BluetoothHandler.TAG, "...Reading Data...");
    }*/

    public boolean createConnection() {
        Log.d(BluetoothHandler.TAG, "Connection attempt in createConnection()");
        BluetoothDevice bluetoothDevice = bluetoothAdapter.getRemoteDevice(BluetoothHandler.MAC_ADDRESS);

        try {
            this.bluetoothSocket = bluetoothDevice.createRfcommSocketToServiceRecord(BluetoothHandler.SPP_UUID);
            Log.d(BluetoothHandler.TAG, "Socket is created");
        } catch (IOException e) {
            Log.d(BluetoothHandler.TAG, "Fatal error in createConnection() with socket creating" + e);
            return false;
        }

        this.bluetoothAdapter.cancelDiscovery();

        Log.d(BluetoothHandler.TAG, "...Establish connection...");
        try {
            this.bluetoothSocket.connect();
            Log.d(BluetoothHandler.TAG, "Connection established and ready for use");
        } catch (IOException e) {
            try {
                this.bluetoothSocket.close();
            } catch (IOException ex) {
                Log.d(BluetoothHandler.TAG, "Fatal Error: Can't close socket in createConnection()" + ex);
                return false;
            }
            Log.d(BluetoothHandler.TAG, "Can't create connection in createConnection()" + e);

            return false;
        }

        this.bluetoothReader = new BluetoothReader(bluetoothSocket, this.handler);
        this.bluetoothReader.start();

        return true;
    }

    public boolean closeConnection() {
        Log.d(BluetoothHandler.TAG, "...Close Connection...");

        try {
            this.bluetoothSocket.close();
            Log.d(BluetoothHandler.TAG, "Socket is closed");
        } catch (IOException e) {
            Log.d(BluetoothHandler.TAG, "Closing socket failed" + e);
            return false;
        }

        return true;
    }

    public boolean ckeckBluetoothState() {
        Log.d(BluetoothHandler.TAG, "...Check bluetooth state...");

        if(this.bluetoothAdapter == null) {
            Log.d(BluetoothHandler.TAG, "Fatal error: bluetooth doesn't support");
            return false;
        } else {
            if (this.bluetoothAdapter.isEnabled()) {
                Log.d(BluetoothHandler.TAG, "Bluetooth is turned on");
            } else {
                Log.d(BluetoothHandler.TAG, "Bluetooth is turned off");
                //TODO
                //Invite user to enable bluetooth
                return false;
            }
        }

        return true;
    }
}
