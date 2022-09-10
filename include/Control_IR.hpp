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
#include "SPIFFS.h"
#include "FS.h"

#define IR_RECEIVE_PIN 26 // D15
#define IR_SEND_PIN 4     // D4
#define LED_BUILTIN 16    // RX2

#define NUMBER_OF_RAWS 10
#define MIN_TEMPERATURE_VALUE 18
#define MAX_TEMPERATURE_VALUE 25
#define KHZ_ONE 38
#define KHZ_TWO 32

// Os três próximos #define alteram alguns valores padrão da lib que não funcionam com protocolos de ar condicionado
#define RAW_BUFFER_LENGTH 750   // bits do protocolo IR, um protocolo de 100 bits gera um array de 200 elementos ou seja 2x
#define RECORD_GAP_MICROS 52000 // Tempo do gap em micros da gravação
#define MARK_EXCESS_MICROS 20   // Sugerido pela lib para o módulo VS1838
#define SEND_PWM_BY_TIMER
#include <IRremote.hpp>

constexpr char RAWS_FILE[] = "/raws.bin";

class InfraRed
{
private:
    std::array<std::array<uint8_t, RAW_BUFFER_LENGTH>, NUMBER_OF_RAWS> raws;

    void readRaws()
    {
        auto file = SPIFFS.open(RAWS_FILE, "r");

        if (file.available())
        {
            for (auto &raw : raws)
            {
                file.read(raw.data(), RAW_BUFFER_LENGTH);
            }
        }

        file.close();
    }

    void writeRaws()
    {
        auto file = SPIFFS.open(RAWS_FILE, FILE_WRITE, true);

        for (auto &raw : raws)
        {
            file.write(raw.data(), RAW_BUFFER_LENGTH);
        }

        file.close();
    }

    void sendRaw(std::array<uint8_t, RAW_BUFFER_LENGTH> &raw)
    {
        IrSender.sendRaw(raw.data(), RAW_BUFFER_LENGTH, KHZ_ONE);
        delay(5);
        IrSender.sendRaw(raw.data(), RAW_BUFFER_LENGTH, KHZ_TWO);
    }

    void registerRaw(std::array<uint8_t, RAW_BUFFER_LENGTH> &raw)
    {
        IrReceiver.compensateAndStoreIRResultInArray(raw.data());
    }

public:
    bool begin()
    {
        if (!SPIFFS.begin(true))
        {
            return false;
        }

        InfraRed::readRaws();

        IrSender.begin(IR_SEND_PIN, ENABLE_LED_FEEDBACK);

        return true;
    }

    bool RecRaw()
    {
        IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

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

    bool listdir()
    {
        File root = SPIFFS.open("/");

        if (!root)
        {
            Serial.println(" - falha ao abrir o diretório");

            return false;
        }

        File file = root.openNextFile();
        int qtdFiles = 0;

        while (file)
        {
            Serial.print("FILE : ");
            Serial.print(file.name());
            Serial.print("\tSIZE : ");
            Serial.println(file.size());
            qtdFiles++;
            file = root.openNextFile();
        }

        if (qtdFiles == 0)
        {
            Serial.println("Nenhuma Gravação Disponível");
            return false;
        }

        return true;
    }
};