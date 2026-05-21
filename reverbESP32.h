#ifndef SYNTHESP32_REVERB_SIMPLE_H
#define SYNTHESP32_REVERB_SIMPLE_H

#include <Arduino.h>

class SimpleReverb {
private:
    // Кольцевые буферы для delay (стерео)
    static const int DELAY_BUFFER_SIZE = 22050; // 500ms при 44100 Гц
    int16_t delayBufferL[DELAY_BUFFER_SIZE];
    int16_t delayBufferR[DELAY_BUFFER_SIZE];
    
    // Позиции в буферах
    int writePosL = 0, writePosR = 0;
    
    // Параметры delay
    int delayTimeL = 11025; // 250ms для левого
    int delayTimeR = 13230; // 300ms для правого (для стерео эффекта)
    
    float wetMix = 0.0f;    // Уровень delay сигнала
    float dryMix = 1.0f;    // Уровень исходного сигнала
    float feedback = 0.0f;  // Обратная связь
    float damping = 0.7f;   // Демпфирование высоких частот при обратной связи
    
public:
    SimpleReverb() {
        clear();
        setLevel(0);
    }
    
    void clear() {
        memset(delayBufferL, 0, sizeof(delayBufferL));
        memset(delayBufferR, 0, sizeof(delayBufferR));
        writePosL = writePosR = 0;
    }
    
    void setLevel(uint8_t level) {
        if (level == 0) {
            wetMix = 0.0f;
            dryMix = 1.0f;
            feedback = 0.0f;
            clear();
        } else {
            // Маппинг уровня 0-255 в параметры delay
            wetMix = (level / 255.0f) * 0.7f;        // Макс 70% wet
            dryMix = 1.0f - wetMix;
            feedback = (level / 255.0f) * 0.5f;      // Обратная связь до 50%
            
            // Можно регулировать время задержки в зависимости от уровня
            delayTimeL = 4410 + (level * 35); // 100ms - 300ms
            delayTimeR = 4851 + (level * 38); // 110ms - 330ms (немного отличается для стерео)
            
            // Ограничение размера буфера
            if (delayTimeL >= DELAY_BUFFER_SIZE) delayTimeL = DELAY_BUFFER_SIZE - 1;
            if (delayTimeR >= DELAY_BUFFER_SIZE) delayTimeR = DELAY_BUFFER_SIZE - 1;
        }
    }
    
    // Метод для установки времени задержки напрямую (если нужно)
    void setDelayTime(int timeMs) {
        int samples = (timeMs * 44.1f); // 44.1 сэмпла на мс при 44100 Гц
        delayTimeL = constrain(samples, 100, DELAY_BUFFER_SIZE - 1);
        delayTimeR = constrain(samples * 1.1f, 100, DELAY_BUFFER_SIZE - 1);
    }
    
    // Метод для установки обратной связи
    void setFeedback(float fb) {
        feedback = constrain(fb, 0.0f, 0.9f);
    }
    
    void process(int16_t &left, int16_t &right) {
        if (wetMix < 0.01f) return; // Если эффект выключен
        
        // Чтение задержанных сигналов
        int readPosL = (writePosL - delayTimeL + DELAY_BUFFER_SIZE) % DELAY_BUFFER_SIZE;
        int readPosR = (writePosR - delayTimeR + DELAY_BUFFER_SIZE) % DELAY_BUFFER_SIZE;
        
        int16_t delayedL = delayBufferL[readPosL];
        int16_t delayedR = delayBufferR[readPosR];
        
        // Применяем демпфирование к обратной связи (фильтр низких частот)
        static int16_t fbL = 0, fbR = 0;
        fbL = fbL * damping + delayedL * (1.0f - damping);
        fbR = fbR * damping + delayedR * (1.0f - damping);
        
        // Записываем в буферы текущий сигнал + обратная связь
        delayBufferL[writePosL] = left + (int16_t)(fbL * feedback);
        delayBufferR[writePosR] = right + (int16_t)(fbR * feedback);
        
        // Обновляем позиции записи
        writePosL = (writePosL + 1) % DELAY_BUFFER_SIZE;
        writePosR = (writePosR + 1) % DELAY_BUFFER_SIZE;
        
        // Смешиваем dry и wet сигналы
        left = (int16_t)(left * dryMix + delayedL * wetMix);
        right = (int16_t)(right * dryMix + delayedR * wetMix);
        
        // Мягкое ограничение для предотвращения клиппинга
        left = constrain(left, -32768, 32767);
        right = constrain(right, -32768, 32767);
    }
};

#endif



