package com.example.valeria.messeger;

import BluetoothWorker.BluetoothHandler;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {
    private BluetoothHandler bluetoothHandler;
    private Button buttonSend;
    private EditText editText;
    private TextView textViewState;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        buttonSend = (Button) findViewById(R.id.buttonSend);
        editText = (EditText) findViewById(R.id.editText);
        textViewState = (TextView) findViewById(R.id.textViewState);

        bluetoothHandler = new BluetoothHandler(this);

        if(bluetoothHandler.ckeckBluetoothState() == true)
            textViewState.setText("Sate: Bluetooth is turned on");
        else
            textViewState.setText("Sate: Bluetooth is turned off");

        buttonSend.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                bluetoothHandler.sendData(editText.getText().toString());
            }
        });
    }

    @Override
    public void onResume() {
        super.onResume();

        bluetoothHandler.createConnection();
    }

    @Override
    public void onPause() {
        super.onPause();

        bluetoothHandler.closeConnection();
    }

    public EditText getEditText() {
        return editText;
    }
}
