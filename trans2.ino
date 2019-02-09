#include <Wire.h>
#include <Adafruit_MCP4725.h>

Adafruit_MCP4725 dac; // constructor
bool base;
int arr[3] = {0,0,0};
bool job;
int var = 0;

void setup() {
  // put your setup code here, to run once:
  DDRD = B11111100;
  DDRB = B00000011;
  Serial.begin(115200); 
  dac.begin(0x62); // The I2C Address: Run the I2C Scanner if you're not sure  
}

void getData( int j ) {
  float aa0 = analogRead(A0) * 5.0 / 1023.0;
  float aa1 = analogRead(A1) * 5.0 / 1023.0;
  float aa2 = analogRead(A2) * 5.0 / 1023.0;
  float aa3 = analogRead(A3) * 5.0 / 1023.0;

  float ice = (aa2-aa3) / 470.0;

  float i1 = (aa0-aa1) / 4700.0;
  float i2 = aa1 / 1100.0;
  float ibe = i1 - i2;

  Serial.print(j);
  Serial.print(";");
  Serial.print(ibe, 6);
  Serial.print(";");
  Serial.print(ice, 6);
  Serial.print(";");
  Serial.print(aa1, 6);
  Serial.print(";");
  Serial.println(aa3, 6);
}

void setB( int i ){
  PORTB = (i>>6) & B00000011;
  PORTD = (i<<2) & B11111100;
}

void setK( int i ){
  dac.setVoltage(i, false);
}

void loop() {
  if (job){
    if (base){
      setB( arr[0] );
      for ( int j = arr[1]; j<arr[2]; j++ ){ 
        setK( j );
        getData( j );
      }
    } else { //kollector
      setK( arr[0] );
      for ( int j = arr[1]; j<arr[2]; j++ ){ 
        setB( j );
        getData( j );
      }
    }
    Serial.println('>');
    job = false;
  } else {
    if ( Serial.available() > 0) {
      char incoming = Serial.read();
      Serial.write(incoming);
      switch ( incoming ) {
        case 'B':
          base = true;
          arr[0]=arr[1]=arr[2]=0;
          var = 0;
          break;
        case 'K':
          base = false;
          arr[0]=arr[1]=arr[2]=0;
          var = 0;
          break;
        case ',':
          if (var<2) var++;
          break;
        case 'Q':
          job = true;
          break;
        default: //digits
          if (incoming<'0' || incoming>'9') break;
          int d = incoming - '0';
          arr[var] = arr[var] * 10 + d;   
          break;
      }
    }
  }
}
  
