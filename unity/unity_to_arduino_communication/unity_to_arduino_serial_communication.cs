using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO.Ports;

public class unity_to_arduino_serial_communication : MonoBehaviour
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
        // Press arrow up key to light up LED_BUILTIN on Wemos
        if (Input.GetKeyDown(KeyCode.UpArrow)){
              Debug.Log("Up Arrow key was pressed.");
              stream.Write("1"); // write 1 to serial port so that wemos can process it
            }
  }

  void onDisable(){
    stream.Close(); // closes the stream
  }
}
