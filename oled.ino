void REFRESH_OLED_OPTIMIZED() {
  static byte lastSound = 255, lastPattern = 255, lastRot = 255;
  static int lastValue = -1, lastBpm = -1, lastStep = 255;
  static bool lastPlaying = false, lastRecording = false;
  
  // Обновляем только если что-то изменилось
  bool needUpdate = (selected_sound != lastSound) ||
                   (selected_pattern != lastPattern) ||
                   (selected_rot != lastRot) ||
                   (ROTvalue[selected_sound][selected_rot] != lastValue) ||
                   (bpm != lastBpm) ||
                   (sstep != lastStep) ||
                   (playing != lastPlaying) ||
                   (recording != lastRecording);
  
  if (!needUpdate && !refreshOLED) return;
  
  u8g2.clearBuffer();
  
  /*refreshOLED=false;
  u8g2.clearBuffer();*/
 // u8g2.drawFrame(0,0,31,127);
u8g2.setFont(u8g2_font_5x7_tf);       // Основной - компактный (5x7)
u8g2.setFont(u8g2_font_6x10_tf);      // Для заголовков (6x10)
u8g2.setFont(u8g2_font_profont10_tf); // Пропорциональный, читаемый
u8g2.setFont(u8g2_font_helvR08_tf);   // Чистый Helvetica 8px      // Основной - компактный (5x7)

  u8g2.setCursor(2,11);
  u8g2.print("S"); 
  u8g2.setCursor(13,11); 
  u8g2.print(selected_sound);
  
  u8g2.drawHLine(0,14,31);  

  u8g2.setCursor(3,26);
  u8g2.print(trot[selected_rot]);
  
   u8g2.setCursor(2,39); 
  if (selected_rot==11){
     u8g2.print(master_filter); 
  } else if (selected_rot==12){
    u8g2.print(octave);      
  } else if (selected_rot==8){
    u8g2.print(bpm);
  } else if (selected_rot==9){
     u8g2.print(master_vol);
  } else if (selected_rot==10){
     u8g2.print(transpose);
   } else if (selected_rot==13){
     u8g2.print(pattern_song_counter);
  //////////////
  } else if (selected_rot==14) {  // НОВОЕ: реверб
    u8g2.print(master_reverb);
  ////////////////
  
  } else if (selected_rot==0){
    // ОТОБРАЖАЕМ НАЗВАНИЕ ВОЛНЫ вместо номера
    byte waveIndex = ROTvalue[selected_sound][0];
    if (waveIndex < 16) { // Проверяем границы массива
      u8g2.print(waveNames[waveIndex]);
    } 
  } else {
    u8g2.print(ROTvalue[selected_sound][selected_rot]);  
  }


  /*u8g2.setCursor(3,41); 
  if (selected_rot==11){
     u8g2.print(master_filter); 
  } else if (selected_rot==12){
    u8g2.print(octave);      
  } else if (selected_rot==8){
    u8g2.print(bpm);
  } else if (selected_rot==9){
     u8g2.print(master_vol);
  } else if (selected_rot==10){
     u8g2.print(transpose);
   } else if (selected_rot==13){
     u8g2.print(pattern_song_counter);
  } else {
    u8g2.print(ROTvalue[selected_sound][selected_rot]);  
  }*/
      


  u8g2.drawHLine(0,42,31);

  u8g2.setCursor(2,53);
  u8g2.print("P");
  u8g2.setCursor(14,53);
  u8g2.print(selected_pattern);  

  u8g2.drawHLine(0,56,31); 
  u8g2.setCursor(2,68);
  u8g2.print("s");
  u8g2.setCursor(14,68);
  u8g2.print(selected_sndSet);
    
  u8g2.setFont(u8g2_font_6x10_tf);
  if (selected_rot==13){
  u8g2.setCursor(3,91); 
  u8g2.print(song[pattern_song_counter]);    
  }


  u8g2.setCursor(3,100); 
  u8g2.print(tmodeZ[modeZ]);

    u8g2.setCursor(2, 110);
  u8g2.print("B ");
  u8g2.print(bpm);
  
  u8g2.drawFrame(0,0,31,71);

  u8g2.sendBuffer();  

  /*// Добавить в конец файла перед последней }:

void drawWaveform(byte x, byte y, byte width, byte height) {
  if (!showWaveform) return;
  
  // Очищаем область под волну
  u8g2.setDrawColor(0);
  u8g2.drawBox(x, y, width, height);
  u8g2.setDrawColor(1);
  
  // Центральная линия
  byte centerY = y + height/2;
  u8g2.drawHLine(x, centerY, width);
  
  // Рисуем волну
  byte prevX = x;
  byte prevY = centerY;
  
  for (byte i = 0; i < width; i++) {
    // Вычисляем индекс в буфере (с учетом смещения)
    byte bufferIdx = (waveBufferIndex + i) % WAVE_VIS_WIDTH;
    
    // Нормализуем значение от -127..127 к 0..height-1
    int16_t sample = waveBuffer[bufferIdx];
    byte pixelY = centerY - map(sample, -250, 250, -height/2, height/2);
    
    // Ограничиваем по высоте
    if (pixelY < y) pixelY = y;
    if (pixelY >= y + height) pixelY = y + height - 1;
    
    // Ставим точку
    u8g2.drawPixel(x + i, pixelY);
    
    // Соединяем линией с предыдущей точкой для гладкости
    if (i > 0) {
      u8g2.drawLine(prevX, prevY, x + i, pixelY);
    }
    
    prevX = x + i;
    prevY = pixelY;
  }
  
  // Рамка вокруг
  u8g2.drawFrame(x, y, width, height);*/
}

