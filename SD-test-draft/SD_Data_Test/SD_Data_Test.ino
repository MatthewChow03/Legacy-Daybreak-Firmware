#include <SPI.h>
#include <SdFat.h>
#include <mcp_can.h>
#include <mcp_can_dfs.h>

File data;
SdFat sd;

MCP_CAN CAN(9);

unsigned long millisecs;

void setup()
{
  pinMode(A0, INPUT_PULLUP); // Button Pin
  pinMode(7, OUTPUT);        // LED Pin

  // see if the card is present and can be initialized:
  if (!sd.begin(4))
  {
    while (1)
      ; // don't do anything more:
  }

  int canSSOffset = 0;

  while (CAN.begin(CAN_125KBPS) != CAN_OK)
  {
    canSSOffset ^= 1;
    delay(100);
    CAN = MCP_CAN(10 + canSSOffset);
  }

  // Initialize Filters
  CAN.init_Mask(0, 0, 0x7F8);
  CAN.init_Filt(0, 0, 0x620);

  CAN.init_Mask(1, 0, 0x7F4);
  CAN.init_Filt(2, 0, 0x500);
}

void loop()
{
  int check = 1; // check if the button is on
  int buttonState = digitalRead(A0);

  // if (buttonState == HIGH) {
  //   check = 0;
  // } else {
  //   check = 1;
  // }

  millisecs = millis();
  int id1 = 0;
  byte len = 0;
  uint8_t buffer[8] = {0};

  // sd.begin(4);
  // data = sd.open("working.txt", FILE_WRITE); // set up the file to write
  // if (data)
  // {
  //   data.print("working");
  //   data.print("-");
  //   data.print(millisecs);
  //   data.print("\n");
  //   data.close();
  // }

  // sd.begin(4);
  // data = sd.open("borking.txt", FILE_WRITE); // set up the file to write
  // if (data)
  // {
  //   data.print("borking");
  //   data.print("-");
  //   data.print(millisecs);
  //   data.print("\n");
  //   data.close();
  // }

  if (CAN.checkReceive() == CAN_MSGAVAIL)
  {
    CAN.readMsgBuf(&len, buffer);
    id1 = CAN.getCanId();

    // Do not log speciifc IDs
    //      0x500: motor name
    //      0x620: battery name
    //      0x621: battery revision
    if (id1 == 0x500 || id1 == 0x620 || id1 == 0x621)
    {
      // ignore
    }

    else if (check == 1)
    {
      sd.begin(4);
      data = sd.open("data2.txt", FILE_WRITE); // set up the file to write

      if (data)
      {

        // digitalWrite(7, HIGH);
        // data.print(millisecs, HEX);
        // data.print("0");
        // data.print(id1, HEX);
        // for (int i = 0; i < len; i++)
        // {
        //   data.print(buffer[i], HEX);
        // }
        // data.print(len, HEX);
        // data.print("\n");
        // data.close();

        digitalWrite(7, HIGH); // turn led on
        data.print(id1, HEX);  // write the ID of the CAN Message
        data.print(" ");
        data.print(len, HEX);  // write the length of the CAN Message
        data.print(" ");

        for (int i = 0; i < len; i++)
        {
          data.print(buffer[i], HEX); // write the actual data in the message
        }
        data.print("  -  ");
        data.print(millisecs); // write the time stamp in milliseconds when the data was recieved
        data.print("\n");
        data.close();
      }
    }
    
    // if the file isn't open or the button is closed, pop up an error:
    else
    {
      if (check == 0 && data == 0)
      {
        digitalWrite(7, LOW); // turn off the LED
      }
    }
  }
}