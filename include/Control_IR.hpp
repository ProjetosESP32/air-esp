/**
 * @file Control_IR.hpp
 * @author Marcus Vinícius (marcaovini07@gmail.com)
 * @brief Arquivo de gerência de infra vermelho de ar condicionado
 * @version 0.1
 * @date 2022-09-04
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <Arduino.h>
#include <array>
#include <algorithm>

#define IR_RECEIVE_PIN 26 // D15
#define IR_SEND_PIN 4     // D4
#define REC_PIN 16

#define NUMBER_OF_RAWS 10
#define MIN_TEMPERATURE_VALUE 18
#define MAX_TEMPERATURE_VALUE 25
#define KHZ_START 32

// Os três próximos #define alteram alguns valores padrão da lib que não são funcionam com protocolos de ar condicionado
#define RAW_BUFFER_LENGTH 750   // bits do protocolo IR, um protocolo de 100 bits gera um array de 200 elementos ou seja 2x
#define RECORD_GAP_MICROS 52000 // Tempo do gap em micros da gravação
#define MARK_EXCESS_MICROS 20   // Sugerido pela lib para o módulo VS1838
#include <IRremote.hpp>
#include "LITTLEFS.h"

constexpr const char *RAWS_FILE = "raws.bin";

class InfraRed
{
private:
    std::array<std::array<uint8_t, RAW_BUFFER_LENGTH>, NUMBER_OF_RAWS> raws;

    void readRaws()
    {
        File file = LITTLEFS.open("raws.bin", FILE_READ, false);

        while(file.available()) 
        {
            file.read();
        }

        file.close();
    }

    void writeRaws()
    {
        File file = LITTLEFS.open(RAWS_FILE, FILE_WRITE, true);

        if (file.available())
        {
            for (auto &raw : raws)
            {
                file.write(raw.data(), RAW_BUFFER_LENGTH);
            }
        }

        file.close();
    }

    void sendRaw(std::array<uint8_t, RAW_BUFFER_LENGTH> &raw)
    {
        for (int i = 0; i < 7; i++)
        {
            IrSender.sendRaw(raw.data(), RAW_BUFFER_LENGTH, KHZ_START + i);
            delay(5);
        }
    }

    void registerRaw(std::array<uint8_t, RAW_BUFFER_LENGTH> &raw)
    {
        IrReceiver.compensateAndStoreIRResultInArray(raw.data());
    }

public:
    bool begin()
    {
        pinMode(REC_PIN, INPUT);

        if (!LITTLEFS.begin(true))
        {
            return false;
        }
        
        InfraRed::readRaws();

        IrSender.begin(IR_SEND_PIN, false);

        return true;
    }

    bool initRecRaw()
    {
        IrReceiver.begin(IR_RECEIVE_PIN, 0);

        int idRaw = 0;
        while (idRaw < NUMBER_OF_RAWS)
        {
            if (IrReceiver.decode())
            {
                InfraRed::registerRaw(raws[idRaw++]);
                IrReceiver.resume();
            }
        }

        InfraRed::writeRaws();
        IrReceiver.stop();

        return true;
    }

    void power(bool power)
    {
        if (power)
            InfraRed::sendRaw(raws[1]);
        else
            InfraRed::sendRaw(raws[0]);
    }

    bool setTemperature(int temperature)
    {
        if (MIN_TEMPERATURE_VALUE <= temperature && temperature <= MAX_TEMPERATURE_VALUE)
        {
            int index = temperature - (MIN_TEMPERATURE_VALUE - 2); // isso pois os index 0 e 1 são para desligar e ligar, respectivamente
            InfraRed::sendRaw(raws[index]);
        }

        return true;
    }
};
