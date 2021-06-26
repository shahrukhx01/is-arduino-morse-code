using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO.Ports;

public class arduino_to_unity_serial_communication : MonoBehaviour
{

    SerialPort stream = new SerialPort("/dev/cu.usbserial-1410", 9600); // serial port object
    // Start is called before the first frame update
    void Start()
    {
      stream.Open(); // open the stream over serial port
    }

    // Update is called once per frame
    void Update()
    {
      int arduinoMessage = int.Parse(stream.ReadLine());
      if (arduinoMessage == 1){
        Debug.Log("message from Arduino: "+ arduinoMessage);
      }else{
        Debug.Log("No message from Arduino");
      }

    }

    void onDisable(){
      stream.Close(); // closes the stream
    }
}
