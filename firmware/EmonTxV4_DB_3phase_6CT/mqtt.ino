
#include "mqtt.h"

EthernetClient ethClient;
PubSubClient client(ethClient);

void mqttSetup() {
  Serial.println("SETUP MQTT...");
  client.setServer(mqttServer, mqttPort);
  Serial.println("MQTT Setup Complete");
  if (!SPI.pins(MOSIpin, MISOpin, SCKpin)) {
    Serial.println("+++++++++++++++++++++++++++++++++++++++++");
    Serial.println("SPI pins config error!");
  } else {
    Serial.println("SPI pins set...");
  }
  Ethernet.init(USE_THIS_SS_PIN);
  delay(50);

  #ifdef FIXED_IP
  Ethernet.begin(mac[macNumber], ip2, dns2, gateway2);
  #else
  Ethernet.begin(mac[macNumber]); //DHCP
  #endif
  
  delay(1000);
  if ( (Ethernet.getChip() == w5500) || (Ethernet.getChip() == w6100) || (Ethernet.getAltChip() == w5100s) )
  {
    if (Ethernet.getChip() == w6100)
      SerialDebug.print(F("W6100 => "));
    else if (Ethernet.getChip() == w5500)
      SerialDebug.print(F("W5500 => "));
    else
      SerialDebug.print(F("W5100S => "));
    
    SerialDebug.print(F("Speed: "));
    SerialDebug.print(Ethernet.speedReport());
    SerialDebug.print(F(", Duplex: "));
    SerialDebug.print(Ethernet.duplexReport());
    SerialDebug.print(F(", Link status: "));
    SerialDebug.println(Ethernet.linkReport());
  }
  // Allow the hardware to sort itself out
  reconnect();

}

void reconnect() {
  // Loop until we're reconnected
  if(!client.connected()) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= MQTT_INTERVAL) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if ( (Ethernet.getChip() == w5500) || (Ethernet.getChip() == w6100) || (Ethernet.getAltChip() == w5100s) )
        {
          if (Ethernet.getChip() == w6100)
            SerialDebug.print(F("W6100 => "));
          else if (Ethernet.getChip() == w5500)
            SerialDebug.print(F("W5500 => "));
          else
            SerialDebug.print(F("W5100S => "));
          
          SerialDebug.print(F("Speed: "));
          SerialDebug.print(Ethernet.speedReport());
          SerialDebug.print(F(", Duplex: "));
          SerialDebug.print(Ethernet.duplexReport());
          SerialDebug.print(F(", Link status: "));
          SerialDebug.println(Ethernet.linkReport());
        }
        if (Ethernet.linkStatus() != 1) {
          previousMillis = currentMillis;
          return;
        }
        Serial.println(Ethernet.localIP());
        if (client.connect("eTxV4")) {
          Serial.println("connected");
          // Once connected, publish an announcement...
          client.publish("emonTx4","connected");
          // ... and resubscribe
          //client.subscribe("inTopic");
        } else {
          Serial.print("failed, rc=");
          Serial.print(client.state());
          Serial.println(" try again in 5 seconds");
          
        }
        previousMillis = currentMillis;
          // Wait 5 seconds before retrying
          //delay(5000);
    }
  }
}

void mqttReport() {
  digitalWrite(LEDpin,LOW); 
  emontx.Msg++;
  previousMillis2 = millis();

  if (client.connected()) {
    char cstr[30];
    char chs[3];
    char topic[20];
    itoa(emontx.Msg, cstr, 10);
    client.publish("emonTx4/Msg", cstr);
    Serial.print("Msg# ");
    Serial.println(emontx.Msg);
    for (byte ch=0; ch<NUM_V_CHANNELS; ch++) {
      //Serial.print(F(",V")); Serial.print(ch+1); Serial.print(":"); Serial.print(emontx.V[ch]*0.01);
      itoa(ch, chs, 10);
      itoa(emontx.V[ch], cstr, 10);
      strcpy(topic, "emonTx4/V");
      strcat(topic, chs);
      client.publish(topic, cstr);
    }
    for (byte ch=0; ch<NUM_I_CHANNELS; ch++) {
      //Serial.print(F(",P")); Serial.print(ch+1); Serial.print(":"); Serial.print(emontx.P[ch]);
      itoa(ch, chs, 10);
      itoa(emontx.P[ch], cstr, 10);
      strcpy(topic, "emonTx4/P");
      strcat(topic, chs);
      client.publish(topic, cstr);
    }
    for (byte ch=0; ch<NUM_I_CHANNELS; ch++) {
      //Serial.print(F(",E")); Serial.print(ch+1); Serial.print(":"); Serial.print(emontx.E[ch]);
      itoa(ch, chs, 10);
      itoa(emontx.E[ch], cstr, 10);
      strcpy(topic, "emonTx4/E");
      strcat(topic, chs);
      client.publish(topic, cstr);
    }
    for (byte ch=0; ch<NUM_I_CHANNELS; ch++) {
      //Serial.print(F(",I")); Serial.print(ch+1); Serial.print(":"); Serial.print(EmonLibDB_getIrms(ch+1),3);
      itoa(ch, chs, 10);
      itoa(int(round(EmonLibDB_getIrms(ch+1)*1000.0)), cstr, 10);
      strcpy(topic, "emonTx4/I");
      strcat(topic, chs);
      client.publish(topic, cstr);
    }
    for (byte ch=0; ch<NUM_I_CHANNELS; ch++) {
      //Serial.print(F(",pf")); Serial.print(ch+1); Serial.print(":"); Serial.print(EmonLibDB_getPF(ch+1),4);
      itoa(ch, chs, 10);
      itoa(int(round(EmonLibDB_getPF(ch+1)*10000.0)), cstr, 10);
      strcpy(topic, "emonTx4/pF");
      strcat(topic, chs);
      client.publish(topic, cstr);
    }
    itoa(int(round(EmonLibDB_getLineFrequency()*1000.0)), cstr, 10);
    strcpy(topic, "emonTx4/Hz");
    client.publish(topic, cstr);
  } else {
    Serial.print("NO CLIENT CONNECTION Msg# ");
    Serial.println(emontx.Msg);
  }
  delay(50);digitalWrite(LEDpin,HIGH);
}

void noPower() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis2 >= NO_POWER_INTERVAL) {
    
    // if (EmonLibDB_acPresent(1)) {
    //   Serial.println(F("AC1 present - Real Power calc enabled"));
    // } else {
    //   Serial.println(F("AC1 missing - Apparent Power calc enabled, assuming ")); //Serial.print(EEProm.assumedVrms); Serial.println(F(" V"));
    // }
    // if (EmonLibDB_acPresent(2)) {
    //   Serial.println(F("AC2 present - Real Power calc enabled"));
    // } else {
    //   Serial.println(F("AC2 missing - Apparent Power calc enabled, assuming ")); //Serial.print(EEProm.assumedVrms); Serial.println(F(" V"));
    // }
    // if (EmonLibDB_acPresent(3)) {
    //   Serial.println(F("AC3 present - Real Power calc enabled"));
    // } else {
    //   Serial.println(F("AC3 missing - Apparent Power calc enabled, assuming ")); //Serial.print(EEProm.assumedVrms); Serial.println(F(" V"));
    // }
    mqttReport();

    previousMillis2 = currentMillis;
  }
}

void mqttLoop() {
    if (!client.loop()) reconnect();
    noPower();
}

const int replyLength = 120;
char reply[replyLength];
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<replyLength;i++){
    reply[i] = '\0';
  }
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
    if (i < replyLength) reply[i] = (char)payload[i];
  }
  Serial.println();
  client.publish("emonTx4", reply);
}
