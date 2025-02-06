void HSPI_init() {
  hp = new SPIClass(HSPI);
  hp->begin();
  radio.begin(hp);
  radio.setAutoAck(false);
  radio.stopListening();
  radio.setRetries(0, 0);
  radio.setPALevel(RF24_PA_MAX, true);
  radio.setDataRate(RF24_2MBPS);
  radio.setCRCLength(RF24_CRC_DISABLED);
  radio.startConstCarrier(RF24_PA_MAX, 40); 
}
void bluetooth_jam(){
  HSPI_init();
  while (true){
    if (bluetooth_jam_method == 0){
      for (int i = 0; i < 22; i++) {
        radio.setChannel(bluetooth_channels[i]);
      }
    }
    if (bluetooth_jam_method == 1){
      int random_channel = random(80);
      radio.setChannel(random_channel);
    }
    if (bluetooth_jam_method == 2){
      for (int i = 0; i < 80; i++) {
        radio.setChannel(i);
      }
    }
  }
}
void drone_jam(){
  HSPI_init();
  while (true){
    if (drone_jam_method == 0){
      int random_channel = random(125);
      radio.setChannel(random_channel);
    }
    if (drone_jam_method == 1){
      for (int i = 0; i < 125; i++){
        radio.setChannel(i);
      }
    }
  }
}
void ble_jam(){
  HSPI_init();
  while (true){
    for (int i = 0; i < 3; i++){
      radio.setChannel(ble_channels[i]);
    }
  }
}
void wifi_jam(){
  HSPI_init();
  while (true){
    for (int channel = 0; channel < 13; channel++) {
      for (int i = (channel * 5) + 1; i < (channel * 5) + 23; i++) {
        radio.setChannel(i);
        radio.writeFast(&jam_text, sizeof(jam_text));
      }
    }
  }
}
void wifi_channel(int channel){
  HSPI_init();
  while (true){
    for (int i = (channel * 5) + 1; i < (channel * 5) + 23; i++) {
      radio.setChannel(i);
      radio.writeFast(&jam_text, sizeof(jam_text));
    }
  }
}
void zigbee_jam(){
  HSPI_init();
  while (true){
    for (int channel = 11; channel < 27; channel++){
      for (int i = 5+5*(channel-11); i < (5+5*(channel-11))+6; i++){
        radio.setChannel(i);
        radio.writeFast(&jam_text, sizeof(jam_text));
      }
    }
  }
}
void misc_jam(int channel1, int channel2){
  HSPI_init();
  while (true) {
    for (int i = 0; i <= channel2 - channel1; i++) {
      radio.setChannel(channel1 + i);
      if (misc_jam_method == 1){
        radio.writeFast(&jam_text, sizeof(jam_text));
      }
    }
  }
}