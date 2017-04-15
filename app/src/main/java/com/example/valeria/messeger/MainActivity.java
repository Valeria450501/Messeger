package com.example.valeria.messeger;

import BluetoothWorker.BluetoothHandler;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.*;
import MessageHandler.MessageHandler;

public class MainActivity extends AppCompatActivity {
    private BluetoothHandler bluetoothHandler;
    private MessageHandler messageHandler;
    private Button buttonSend;
    private EditText editText;
    private TextView textViewState;
    private Spinner spinnerRoute;
    private ArrayAdapter<String> arrayAdapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        buttonSend = (Button) findViewById(R.id.buttonSend);
        editText = (EditText) findViewById(R.id.editText);
        textViewState = (TextView) findViewById(R.id.textViewState);
        spinnerRoute = (Spinner) findViewById(R.id.spinnerRoute);

        String[] startRouters = {"Demo1", "Demo2", "Demo3"};
        arrayAdapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item, startRouters);
        arrayAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);

        spinnerRoute.setAdapter(arrayAdapter);
        spinnerRoute.setPrompt("set route");
        spinnerRoute.setSelection(0);

        bluetoothHandler = new BluetoothHandler(this);
        messageHandler = new MessageHandler(this.bluetoothHandler);

        if(bluetoothHandler.ckeckBluetoothState() == true)
            textViewState.setText("Sate: Bluetooth is turned on");
        else
            textViewState.setText("Sate: Bluetooth is turned off");

        buttonSend.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                messageHandler.sendMessage();
            }
        });

        spinnerRoute.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view,
                                       int position, long id) {
            }
            @Override
            public void onNothingSelected(AdapterView<?> arg0) {
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
    public String getMessage() {
        return editText.getText().toString();
    }
    public int getCoosen(){
        return this.spinnerRoute.getSelectedItemPosition();
    }
}
