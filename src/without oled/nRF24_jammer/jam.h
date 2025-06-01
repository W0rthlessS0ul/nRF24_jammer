void VSPI_init(bool FastMode) {
  sp = new SPIClass(VSPI);
  sp->begin();
  radio1.begin(sp);
  sp->setFrequency(16000000);
  sp->setBitOrder(MSBFIRST);
  sp->setDataMode(SPI_MODE0);
  radio1.setAutoAck(false);
  radio1.stopListening();
  radio1.setRetries(0, 0);
  radio1.setPALevel(RF24_PA_MAX, true);
  radio1.setDataRate(RF24_2MBPS);
  radio1.setCRCLength(RF24_CRC_DISABLED);
  if (FastMode) radio1.startConstCarrier(RF24_PA_MAX, 80);
}
void HSPI_init(bool FastMode) {
  hp = new SPIClass(HSPI);
  hp->begin();
  radio.begin(hp);
  hp->setFrequency(16000000);
  hp->setBitOrder(MSBFIRST);
  hp->setDataMode(SPI_MODE0);
  radio.setAutoAck(false);
  radio.stopListening();
  radio.setRetries(0, 0);
  radio.setPALevel(RF24_PA_MAX, true);
  radio.setDataRate(RF24_2MBPS);
  radio.setCRCLength(RF24_CRC_DISABLED);
  if (FastMode) radio.startConstCarrier(RF24_PA_MAX, 40);
}

// State variables for jamming loops
static int bluetooth_channel_idx = 0;
static int drone_channel_idx = 0;
static int ble_channel_idx = 0;
static int wifi_main_channel_idx = 0;
static int wifi_sub_channel_idx = 0;
static int zigbee_main_channel_idx = 0;
static int zigbee_sub_channel_idx = 0;
static int misc_current_channel_offset = 0;

// Target channels for MISC and WIFI_CHANNEL modes
int misc_jam_ch1 = 1;
int misc_jam_ch2 = 80;
int wifi_selected_jam_channel = 1;

void reset_jam_state_variables() {
    bluetooth_channel_idx = 0;
    drone_channel_idx = 0;
    ble_channel_idx = 0;
    wifi_main_channel_idx = 0;
    wifi_sub_channel_idx = 0;
    zigbee_main_channel_idx = 0;
    zigbee_sub_channel_idx = 0;
    misc_current_channel_offset = 0;
    // Reset any other state variables if they exist
}

void bluetooth_jam_step(){
  if (Separate_or_together == 0){
    if (bluetooth_jam_method == 0){
      radio.setChannel(bluetooth_channels[bluetooth_channel_idx]);
      radio1.setChannel(bluetooth_channels_reversed[bluetooth_channel_idx]);
      bluetooth_channel_idx = (bluetooth_channel_idx + 1) % 22;
    }
    else if (bluetooth_jam_method == 1){
      radio.setChannel(random(40));
      radio1.setChannel(random(40, 80));
    }
    else if (bluetooth_jam_method == 2){
      radio.setChannel(bluetooth_channel_idx);
      radio1.setChannel(bluetooth_channel_idx < 40 ? bluetooth_channel_idx + 40 : bluetooth_channel_idx - 40); // Example of reversed pairing for 80 channels
      bluetooth_channel_idx = (bluetooth_channel_idx + 1) % 80;
    }
  }
  else if (Separate_or_together == 1){
    if (bluetooth_jam_method == 0){
      radio.setChannel(bluetooth_channels[bluetooth_channel_idx]);
      radio1.setChannel(bluetooth_channels[bluetooth_channel_idx]);
      bluetooth_channel_idx = (bluetooth_channel_idx + 1) % 22;
    }
    else if (bluetooth_jam_method == 1){
      int random_channel = random(80);
      radio.setChannel(random_channel);
      radio1.setChannel(random_channel);
    }
    else if (bluetooth_jam_method == 2){
      radio.setChannel(bluetooth_channel_idx);
      radio1.setChannel(bluetooth_channel_idx);
      bluetooth_channel_idx = (bluetooth_channel_idx + 1) % 80;
    }
  }
}

void drone_jam_step(){
  if (Separate_or_together == 0){
    if (drone_jam_method == 0){
      radio.setChannel(random(64));
      radio1.setChannel(random(64, 125));
    }
    else if (drone_jam_method == 1){
      radio.setChannel(drone_channel_idx);
      // Ensure radio1 channel is different and within a valid range if desired
      radio1.setChannel((drone_channel_idx + 60) % 125); // Example of reversed pairing
      drone_channel_idx = (drone_channel_idx + 1) % 125;
    }
  }
  else if (Separate_or_together == 1){
    if (drone_jam_method == 0){
      int random_channel = random(125);
      radio.setChannel(random_channel);
      radio1.setChannel(random_channel);
    }
    else if (drone_jam_method == 1){
      radio.setChannel(drone_channel_idx);
      radio1.setChannel(drone_channel_idx);
      drone_channel_idx = (drone_channel_idx + 1) % 125;
    }
  }
}

void ble_jam_step(){
  if (Separate_or_together == 0){
    radio.setChannel(ble_channels[ble_channel_idx]);
    radio1.setChannel(ble_channels_reversed[ble_channel_idx]);
  }
  else if (Separate_or_together == 1){
    radio.setChannel(ble_channels[ble_channel_idx]);
    radio1.setChannel(ble_channels[ble_channel_idx]);
  }
  radio.writeFast(&jam_text, sizeof(jam_text));
  radio1.writeFast(&jam_text, sizeof(jam_text));
  ble_channel_idx = (ble_channel_idx + 1) % 3;
}

void wifi_jam_step(){
  // wifi_sub_channel_idx corresponds to 'i' in the original loop: (channel * 5) + 1 to (channel * 5) + 22
  // wifi_main_channel_idx corresponds to 'channel' in the original loop: 0 to 12

  int current_sub_channel_start = (wifi_main_channel_idx * 5) + 1;
  int current_rf_channel = current_sub_channel_start + wifi_sub_channel_idx;

  if (Separate_or_together == 0){
    radio.setChannel(current_rf_channel);
    radio1.setChannel(83 - current_rf_channel); // This needs careful check for valid range
  }
  else if (Separate_or_together == 1){
    radio.setChannel(current_rf_channel);
    radio1.setChannel(current_rf_channel);
  }
  radio.writeFast(&jam_text, sizeof(jam_text));
  radio1.writeFast(&jam_text, sizeof(jam_text));

  wifi_sub_channel_idx++;
  if (wifi_sub_channel_idx >= 22) { // 22 frequencies per Wi-Fi channel block
    wifi_sub_channel_idx = 0;
    wifi_main_channel_idx++;
    if (wifi_main_channel_idx >= 13) { // 13 Wi-Fi channels
      wifi_main_channel_idx = 0;
    }
  }
}

void wifi_channel_jam_step(){
  // wifi_selected_jam_channel is set by serial command
  // wifi_sub_channel_idx corresponds to 'i' in the original loop
  int current_sub_channel_start = (wifi_selected_jam_channel * 5) + 1;
  int current_rf_channel = current_sub_channel_start + wifi_sub_channel_idx;

  if (Separate_or_together == 0){
    radio.setChannel(current_rf_channel);
    // Example: radio1.setChannel((wifi_selected_jam_channel * 5 + 23) - (current_rf_channel - (wifi_selected_jam_channel * 5)));
    // Ensure this logic correctly mirrors or offsets as intended. For simplicity, using a fixed offset or different static channel for radio1
    radio1.setChannel( (current_rf_channel + 40) % 83 ); // Ensure this is a valid and distinct channel
  }
  else if (Separate_or_together == 1){
    radio.setChannel(current_rf_channel);
    radio1.setChannel(current_rf_channel);
  }
  radio.writeFast(&jam_text, sizeof(jam_text));
  radio1.writeFast(&jam_text, sizeof(jam_text));

  wifi_sub_channel_idx++;
  if (wifi_sub_channel_idx >= 22) { // 22 frequencies per Wi-Fi channel block
    wifi_sub_channel_idx = 0;
    // No need to increment main channel index here as we are jamming a specific one
  }
}

void zigbee_jam_step(){
  // zigbee_main_channel_idx: 11 to 26 (16 channels) -> 0 to 15 in array/offset logic
  // zigbee_sub_channel_idx: 0 to 5 (6 frequencies per Zigbee channel)

  int current_zigbee_channel = 11 + zigbee_main_channel_idx;
  int current_rf_channel_start = 5 + 5 * (current_zigbee_channel - 11);
  int current_rf_channel = current_rf_channel_start + zigbee_sub_channel_idx;

  if (Separate_or_together == 0){
    radio.setChannel(current_rf_channel);
    radio1.setChannel(85 - current_rf_channel); // Ensure valid and distinct
  }
  else if (Separate_or_together == 1){
    radio.setChannel(current_rf_channel);
    radio1.setChannel(current_rf_channel);
  }
  radio.writeFast(&jam_text, sizeof(jam_text));
  radio1.writeFast(&jam_text, sizeof(jam_text));

  zigbee_sub_channel_idx++;
  if (zigbee_sub_channel_idx >= 6) {
    zigbee_sub_channel_idx = 0;
    zigbee_main_channel_idx++;
    if (zigbee_main_channel_idx >= 16) { // 16 Zigbee channels (11-26)
      zigbee_main_channel_idx = 0;
    }
  }
}

void misc_jam_step(){
  // misc_jam_ch1 and misc_jam_ch2 are set by serial command
  if (misc_jam_ch1 > misc_jam_ch2) return; // Basic validation

  int current_channel_for_radio0 = misc_jam_ch1 + misc_current_channel_offset;

  radio.setChannel(current_channel_for_radio0);

  if (Separate_or_together == 0) {
    radio1.setChannel(misc_jam_ch2 - misc_current_channel_offset);
  }
  else if (Separate_or_together == 1) {
    radio1.setChannel(current_channel_for_radio0);
  }

  if (misc_jam_method == 1){ // Send payload only for method 1
    radio.writeFast(&jam_text, sizeof(jam_text));
    radio1.writeFast(&jam_text, sizeof(jam_text));
  }

  misc_current_channel_offset++;
  if ((misc_jam_ch1 + misc_current_channel_offset) > misc_jam_ch2) {
    misc_current_channel_offset = 0;
  }
}