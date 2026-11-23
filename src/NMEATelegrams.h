// NMEA telegrams
// sendmode 0 = no serial, 1 = serial active

// Standard NMEA 0183 extensions. Legacy sentences are preserved.
#include <math.h>
static String standardNMEA(const String& payload) {
  String checksum = String(CheckSum(payload), HEX);
  checksum.toUpperCase();
  if (checksum.length() == 1) checksum = "0" + checksum;
  return "$" + payload + "*" + checksum;
}

static void appendStandardNMEA(String& output, const String& payload, int sendmode) {
  if (sendmode != 1) return;
  String sentence = standardNMEA(payload);
  output += "\r\n";
  output += sentence;
  // Match the original WST/WSE serial routing.
  if (int(actconf.serverMode) == 1 || int(actconf.debug) >= 3) {
    Serial.println(sentence);
  }
}

String sendMWV(int sendmode){

  String HexCheckSum;
  String NMEAWindSpeed;
  String SendWindSpeed;

  // Create NMEA string for wind speed $WIMWV,x.x,a,x.x,a,A*hh<CR><LF>
  NMEAWindSpeed = "WIMWV," + String(winddirection);
  if(String(actconf.windType) == "R"){
    NMEAWindSpeed +=  ",R,";
  }
  else{
    NMEAWindSpeed +=  ",T,";
  }
  if(String(actconf.speedUnit) == "kn"){
    NMEAWindSpeed +=  String(windspeed_kn);
    NMEAWindSpeed +=  ",N,A";
  }
  if(String(actconf.speedUnit) == "m/s"){
    NMEAWindSpeed +=  String(windspeed_mps);
    NMEAWindSpeed +=  ",M,A";
  }
  if(String(actconf.speedUnit) == "km/h"){
    NMEAWindSpeed +=  String(windspeed_kph);
    NMEAWindSpeed +=  ",K,A";
  }
  // Build CheckSum
  HexCheckSum = String(CheckSum(NMEAWindSpeed), HEX);
  // Build complete NMEA string
  SendWindSpeed = "$" + NMEAWindSpeed;
  SendWindSpeed += "*";
  SendWindSpeed += HexCheckSum;
  if((int(actconf.serverMode) == 1 && sendmode == 1) || (int(actconf.serverMode) == 4 && sendmode == 1) || (int(actconf.debug) >= 3 && sendmode == 1)){
    Serial.println(SendWindSpeed);
  }

  return SendWindSpeed;
}

String sendVWR(int sendmode){

  String HexCheckSum;
  String NMEAWindSpeed;
  String SendWindSpeed;

  // Create NMEA string for wind speed $WIVWR,x.x,a,x.x,N,x.x,M,x.x,K*hh<CR><LF>
  NMEAWindSpeed = "WIVWR," + String(winddirection2);
  if(winddirection >= 0 && winddirection < 180){
    NMEAWindSpeed += ",R,";
  }
  else{
    NMEAWindSpeed += ",L,";
  }
  NMEAWindSpeed += String(windspeed_kn);
  NMEAWindSpeed += ",N,";
  NMEAWindSpeed += String(windspeed_mps);
  NMEAWindSpeed += ",M,";
  NMEAWindSpeed += String(windspeed_kph);
  NMEAWindSpeed += ",K";
  // Build CheckSum
  HexCheckSum = String(CheckSum(NMEAWindSpeed), HEX);
  // Build complete NMEA string
  SendWindSpeed = "$" + NMEAWindSpeed;
  SendWindSpeed += "*";
  SendWindSpeed += HexCheckSum;
  if((int(actconf.serverMode) == 1 && sendmode == 1) || (int(actconf.debug) >= 3 && sendmode == 1)){
    Serial.println(SendWindSpeed);
  }

  return SendWindSpeed;
}

String sendVPW(int sendmode){

  String HexCheckSum;
  String NMEAWindSpeed;
  String SendWindSpeed;

   if (winddirection >= (180 - float(actconf.downWindRange)) && winddirection <= (180 + float(actconf.downWindRange))){
    downwindspeed_kn = windspeed_kn;
    downwindspeed_mps = windspeed_mps;
   }
   else{
    downwindspeed_kn = 0;
    downwindspeed_mps = 0;
   }

  // Create NMEA string for wind speed $WIVPW,x.x,N,x.x,M*hh<CR><LF>
  NMEAWindSpeed = "WIVPW," + String(downwindspeed_kn);
  NMEAWindSpeed += ",N,";
  NMEAWindSpeed += String(downwindspeed_mps);
  NMEAWindSpeed += ",M";
  // Build CheckSum
  HexCheckSum = String(CheckSum(NMEAWindSpeed), HEX);
  // Build complete NMEA string
  SendWindSpeed = "$" + NMEAWindSpeed;
  SendWindSpeed += "*";
  SendWindSpeed += HexCheckSum;
  if((int(actconf.serverMode) == 1 && sendmode == 1) || (int(actconf.debug) >= 3 && sendmode == 1)){
    Serial.println(SendWindSpeed);
  }

  return SendWindSpeed;
}

String sendINF(int sendmode){

  String HexCheckSum;
  String NMEAWindSpeed;
  String SendWindSpeed;

  // Create NMEA string for wind speed $PWINF,0,x.x,D,x.x,D,x.x,M,x.x,K,x.x,N,x.x,B,A*hh<CR><LF>
  NMEAWindSpeed = "PWINF," + String(actconf.sensorID);
  NMEAWindSpeed +=  ",";
  NMEAWindSpeed +=  String(winddirection);
  NMEAWindSpeed +=  ",D,";
  NMEAWindSpeed +=  String(dirresolution);
  NMEAWindSpeed +=  ",D,";
  NMEAWindSpeed +=  String(windspeed_mps);
  NMEAWindSpeed +=  ",M,";
  NMEAWindSpeed +=  String(windspeed_kph);
  NMEAWindSpeed +=  ",K,";
  NMEAWindSpeed +=  String(windspeed_kn);
  NMEAWindSpeed +=  ",N,";
  NMEAWindSpeed +=  String(windspeed_bft);
  NMEAWindSpeed +=  ",B,";
  // Build CheckSum
  HexCheckSum = String(CheckSum(NMEAWindSpeed), HEX);
  // Build complete NMEA string
  SendWindSpeed = "$" + NMEAWindSpeed;
  SendWindSpeed += "*";
  SendWindSpeed += HexCheckSum;
  if((int(actconf.serverMode) == 1 && sendmode == 1) || (int(actconf.debug) >= 3 && sendmode == 1)){
    Serial.println(SendWindSpeed);
  }

  return SendWindSpeed;
}

// Send temperature data from DS18B20
String sendWST(int sendmode){

  String HexCheckSum;
  String NMEASensorTemp;
  String SendSensorTemp;

  // Create NMEA string for wind sensor temperature $PWWST,C,0,x.x,A*hh<CR><LF>
  if(String(actconf.tempUnit) == "C"){
    NMEASensorTemp = "PWWST,C," + String(actconf.sensorID);
    NMEASensorTemp += ",";
    NMEASensorTemp += String(temperature);
  }
  else{
    NMEASensorTemp = "PWWST,F," + String(actconf.sensorID);
    NMEASensorTemp += ",";
    NMEASensorTemp += String(temperature);
  }
  NMEASensorTemp += ",A";
  // Build CheckSum
  HexCheckSum = String(CheckSum(NMEASensorTemp), HEX);
  // Build complete NMEA string
  SendSensorTemp = "$" + NMEASensorTemp;
  SendSensorTemp += "*";
  SendSensorTemp += HexCheckSum;
  if((int(actconf.serverMode) == 1 && sendmode == 1) || (int(actconf.debug) >= 3 && sendmode == 1)){
    Serial.println(SendSensorTemp);
  }

  // The stored DS18B20 reading already includes original compensation.
  float tempC = temperature;
  if (String(actconf.tempUnit) != "C") tempC = (tempC - 32.0f) * 5.0f / 9.0f;
  if (isfinite(tempC) && tempC >= -61.0f && tempC <= 125.0f) {
    appendStandardNMEA(SendSensorTemp, "WIMTA," + String(tempC, 2) + ",C", sendmode);
  }
  return SendSensorTemp;
}

// Send environment data from BME280
String sendWSE(int sendmode){

  String HexCheckSum;
  String NMEAWSE;
  String SendWSE;

  // Create NMEA string for wind speed $PWWSE,0,x.x,C,x.x,B,x.x,P,x.x,C,x.x,M,A*hh<CR><LF>
  NMEAWSE = "PWWSE," + String(actconf.sensorID);
  NMEAWSE +=  ",";
  NMEAWSE +=  String(airtemperature);
  if(String(actconf.tempUnit) == "C"){
    NMEAWSE +=  ",C,";
  }
  else{
    NMEAWSE +=  ",K,";
  }
  NMEAWSE +=  String(airpressure);
  NMEAWSE +=  ",B,";
  NMEAWSE +=  String(airhumidity);
  NMEAWSE +=  ",P,";
  NMEAWSE +=  String(dewpoint);
  if(String(actconf.tempUnit) == "C"){
    NMEAWSE +=  ",C,";
  }
  else{
    NMEAWSE +=  ",K,";
  }
  NMEAWSE +=  String(altitude);
  NMEAWSE +=  ",M,";
  // Build CheckSum
  HexCheckSum = String(CheckSum(NMEAWSE), HEX);
  // Build complete NMEA string
  SendWSE = "$" + NMEAWSE;
  SendWSE += "*";
  SendWSE += HexCheckSum;
  if((int(actconf.serverMode) == 1 && sendmode == 1) || (int(actconf.debug) >= 3 && sendmode == 1)){
    Serial.println(SendWSE);
  }

  // BME280 readings use the configured temperature unit and mbar.
  float tempC = airtemperature;
  float humidity = airhumidity;
  float pressure = airpressure;
  if (String(actconf.tempUnit) != "C") tempC = (tempC - 32.0f) * 5.0f / 9.0f;
  // A valid pressure also guards against the original zero-fill on missing data.
  if (isfinite(pressure) && pressure >= 300.0f && pressure <= 1100.0f) {
    if (isfinite(tempC) && tempC >= -40.0f && tempC <= 85.0f) {
      appendStandardNMEA(SendWSE, "WIMTA," + String(tempC, 2) + ",C", sendmode);
    }
    if (isfinite(humidity) && humidity >= 0.0f && humidity <= 100.0f) {
      appendStandardNMEA(SendWSE, "WIXDR,H," + String(humidity, 2) + ",P,HUMIDITY", sendmode);
    }
    appendStandardNMEA(SendWSE, "WIMMB," + String(pressure / 33.8638867f, 2) + ",I," + String(pressure / 1000.0f, 4) + ",B", sendmode);
  }
  return SendWSE;
}
