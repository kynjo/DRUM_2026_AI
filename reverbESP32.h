/*
#ifndef SYNTHESP32_REVERB_SIMPLE_H
#define SYNTHESP32_REVERB_SIMPLE_H

#include <Arduino.h>

class SimpleReverb {
private:
    // Comb filters (feedback delay lines)
    int16_t combL[882];   // 20ms for left
    int16_t combR[992];   // 22.5ms for right
    
    // All-pass filters (for diffusion)
    int16_t allpassL[221]; // 5ms
    int16_t allpassR[265]; // 6ms
    
    int posCombL, posCombR;
    int posApL, posApR;
    
    float wet = 0.0f;
    float dry = 1.0f;
    float feedback = 0.0f;
    float damping = 0.0f;
    
public:
    SimpleReverb() {
        clear();
        setLevel(0);
    }
    
    void clear() {
        memset(combL, 0, sizeof(combL));
        memset(combR, 0, sizeof(combR));
        memset(allpassL, 0, sizeof(allpassL));
        memset(allpassR, 0, sizeof(allpassR));
        
        posCombL = posCombR = 0;
        posApL = posApR = 0;
    }
    
    void setLevel(uint8_t level) {
        if (level == 0) {
            wet = 0.0f;
            dry = 1.0f;
            feedback = 0.0f;
            damping = 0.0f;
            clear();
        } else {
            // Map 0-255 to usable ranges
            wet = (level / 255.0f) * 0.5f;      // Max 50% wet
            dry = 1.0f - wet;
            feedback = 0.6f + (level / 255.0f) * 0.3f;  // 0.6-0.9
            damping = 0.4f + (level / 255.0f) * 0.3f;   // 0.4-0.7
        }
    }
    
    void process(int16_t &left, int16_t &right) {
        if (wet < 0.01f) return;
        
        // Process left channel
        int readCombL = (posCombL - 882 + 882) % 882;
        int16_t delayedL = combL[readCombL];
        
        // Apply damping
        delayedL = (int16_t)(delayedL * damping);
        
        // Update comb filter
        combL[posCombL] = left + (int16_t)(delayedL * feedback);
        posCombL = (posCombL + 1) % 882;
        
        // Process through all-pass
        int readApL = (posApL - 221 + 221) % 221;
        int16_t apDelayedL = allpassL[readApL];
        int16_t apOutL = -delayedL + apDelayedL;
        allpassL[posApL] = delayedL + (int16_t)(apDelayedL * feedback * 0.7f);
        posApL = (posApL + 1) % 221;
        
        // Process right channel
        int readCombR = (posCombR - 992 + 992) % 992;
        int16_t delayedR = combR[readCombR];
        
        delayedR = (int16_t)(delayedR * damping);
        
        combR[posCombR] = right + (int16_t)(delayedR * feedback);
        posCombR = (posCombR + 1) % 992;
        
        int readApR = (posApR - 265 + 265) % 265;
        int16_t apDelayedR = allpassR[readApR];
        int16_t apOutR = -delayedR + apDelayedR;
        allpassR[posApR] = delayedR + (int16_t)(apDelayedR * feedback * 0.7f);
        posApR = (posApR + 1) % 265;
        
        // Mix dry and wet
        left = (int16_t)(left * dry + apOutL * wet);
        right = (int16_t)(right * dry + apOutR * wet);
    }
};

#endif*/

/*#ifndef SYNTHESP32_REVERB_SIMPLE_H
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

#endif*/

/*
#ifndef SYNTHESP32_REVERB_SIMPLE_H
#define SYNTHESP32_REVERB_SIMPLE_H

#include <Arduino.h>

class VoiceDelay {
private:
    // Кольцевой буфер для каждого голоса
    static const int DELAY_BUFFER_SIZE = 11025; // 250ms при 44100 Гц
    int16_t delayBuffer[DELAY_BUFFER_SIZE];
    
    // Позиции в буфере
    int writePos = 0;
    
    // Параметры delay для голоса
    int delayTime = 4410; // 100ms по умолчанию
    float wetMix = 0.0f;    // Уровень delay сигнала
    float dryMix = 1.0f;    // Уровень исходного сигнала
    float feedback = 0.0f;  // Обратная связь
    float damping = 0.7f;   // Демпфирование
    
    // Состояние обратной связи
    int16_t fbState = 0;
    
public:
    VoiceDelay() {
        clear();
    }
    
    void clear() {
        memset(delayBuffer, 0, sizeof(delayBuffer));
        writePos = 0;
        fbState = 0;
    }
    
    void setParams(uint8_t level, int customDelayTime = -1) {
        if (level == 0) {
            wetMix = 0.0f;
            dryMix = 1.0f;
            feedback = 0.0f;
        } else {
            // Маппинг уровня 0-255 в параметры delay
            wetMix = (level / 255.0f) * 0.6f;        // Макс 60% wet
            dryMix = 1.0f - wetMix;
            feedback = (level / 255.0f) * 0.6f;      // Обратная связь до 60%
            
            // Настраиваем время задержки
            if (customDelayTime > 0) {
                delayTime = constrain(customDelayTime, 44, DELAY_BUFFER_SIZE - 1);
            } else {
                // Автонастройка в зависимости от уровня
                delayTime = 2205 + (level * 17); // 50ms - 200ms
                delayTime = constrain(delayTime, 44, DELAY_BUFFER_SIZE - 1);
            }
        }
    }
    
    int16_t process(int16_t input) {
        if (wetMix < 0.01f) return input; // Если эффект выключен
        
        // Чтение задержанного сигнала
        int readPos = (writePos - delayTime + DELAY_BUFFER_SIZE) % DELAY_BUFFER_SIZE;
        int16_t delayed = delayBuffer[readPos];
        
        // Применяем демпфирование к обратной связи
        fbState = fbState * damping + delayed * (1.0f - damping);
        
        // Записываем в буфер текущий сигнал + обратная связь
        delayBuffer[writePos] = input + (int16_t)(fbState * feedback);
        
        // Обновляем позицию записи
        writePos = (writePos + 1) % DELAY_BUFFER_SIZE;
        
        // Смешиваем dry и wet сигналы
        int16_t output = (int16_t)(input * dryMix + delayed * wetMix);
        
        // Мягкое ограничение
        return constrain(output, -32768, 32767);
    }
    
    // Метод для установки времени задержки напрямую
    void setDelayTime(int timeMs) {
        delayTime = (timeMs * 44.1f); // 44.1 сэмпла на мс
        delayTime = constrain(delayTime, 44, DELAY_BUFFER_SIZE - 1);
    }
};

class SimpleReverb {
private:
    // Массив delay'ев для 16 голосов
    VoiceDelay voiceDelays[16];
    
    // Общие параметры (будут применяться ко всем голосам при setLevel)
    uint8_t currentLevel = 0;
    int masterDelayTime = 100; // ms
    
public:
    SimpleReverb() {
        clear();
        setLevel(0);
    }
    
    void clear() {
        for (int i = 0; i < 16; i++) {
            voiceDelays[i].clear();
        }
    }
    
    void setLevel(uint8_t level) {
        currentLevel = level;
        // Применяем параметры ко всем голосам
        for (int i = 0; i < 16; i++) {
            voiceDelays[i].setParams(level, masterDelayTime * 44.1f);
        }
    }
    
    // Установить время задержки (в миллисекундах)
    void setDelayTime(int timeMs) {
        masterDelayTime = constrain(timeMs, 10, 500); // от 10ms до 500ms
        if (currentLevel > 0) {
            setLevel(currentLevel); // Переприменить параметры с новым временем
        }
    }
    
    // Обработка для конкретного голоса
    void processVoice(int voice, int16_t &sample) {
        if (currentLevel > 0) {
            sample = voiceDelays[voice].process(sample);
        }
    }
    
    // Старый метод для совместимости (обрабатывает master канал)
    void process(int16_t &left, int16_t &right) {
        // Этот метод теперь не используется для индивидуальной обработки голосов
        // Но оставим для совместимости - просто обработаем голос 0 для левого и 1 для правого
        // (хотя это не совсем корректно)
        processVoice(0, left);
        processVoice(1, right);
    }
};

#endif*/







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



