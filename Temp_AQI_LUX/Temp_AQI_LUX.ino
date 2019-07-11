#include <OneWire.h>

OneWire  ds(10);
float lux=0.00,ADC_value=0.0048828125,LDR_value;
const int AQI_in = A1;  
int AQI_Value = 0; 


void setup(void) {
  pinMode(AQI_in , INPUT)
  pinMode(A0,INPUT); 
  Serial.begin(9600);
  
}

void loop(void) {
  LDR_value=analogRead(A0);
  lux=(250.000000/(ADC_value*LDR_value))-50.000000;


  AQI_Value = analogRead(AQI_in);
 
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;
  
  if ( !ds.search(addr)) {
     ds.reset_search();
    delay(250);
    return;
  }
  
  if (OneWire::crc8(addr, 7) != addr[7]) {
      return;
  }
  switch (addr[0]) {
    case 0x10:
      type_s = 1;
      break;
    case 0x28:
      type_s = 0;
      break;
    case 0x22:
      type_s = 0;
      break;
    default:
      Serial.println("Device is not a DS18x20 family device.");s
      return;
  } 

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        
  delay(1000);     
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);        
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
  }
  
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;


  
  Serial.print(celsius);
  Serial.println(lux);
  Serial.println(AQI_Value);
}
