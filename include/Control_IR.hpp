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
#include "LITTLEFS.h" // Sistema para gravar na flash do esp

class InfraRed
{
private:
    uint8_t data[RAW_BUFFER_LENGTH][NUMBER_OF_RAWS];

    void readData(String path, uint8_t *raw)
    {                                                      // Lê arquivos -> utilizando ponteiros para ser o mudar a variável e não sua cópia
        String buffer_read;                                // String intermediária pois os dados são gravados em texto, não em int, sendo necessário conversão
        File file = LITTLEFS.open(path, FILE_READ, false); // Abre aquivo com o nome definido no modo leitura
        while (file.available())
        {                                             // Enquando houver informação para ser lida retorna verdadeiro
            buffer_read = file.readStringUntil('\n'); // Lê o arquivo até \n pra saber que é o fim da linha
            *raw = buffer_read.toInt();               // passa o arquivo de String para int
            raw++;                                    // incrementa endereço do ponteiro
        }
        file.close(); // Fecha arquivo
    }

    void sendRaw(uint8_t *raw)
    { // Função que envia sinal para o ar condicionado -> Utiliza ponteiros pois o array é muito grande
        for (int i = 0; i < 7; i++)
        {
            IrSender.sendRaw(raw, RAW_BUFFER_LENGTH, KHZ_START + i); // Array do sinal, tamanho do array, frequência do sinal
            delay(5);
        }
    }

    void writeData(String path, uint8_t *raw)
    {                                                      // Usando ponteiro pois o array é muito grande
        File file = LITTLEFS.open(path, FILE_WRITE, true); // Cria ou abre um arquivo com o nome definido no modo escrita
        for (int i = 0; i < RAW_BUFFER_LENGTH; i++)
        {                             // laço for que passa cada elemento do array para a flash
            file.println(*(raw + i)); // Escreve cada valor por linha e incrementa endereço do ponteiro
        }
        file.close(); // fecha o arquivo
    }

    void registerRaw(uint8_t *raw)
    {
        IrReceiver.compensateAndStoreIRResultInArray(raw); // Salva raw no seu respectivo array
    }

public:
    bool begin()
    {
        pinMode(REC_PIN, INPUT);
        if (!LITTLEFS.begin(true))
        { // Inicia comunicação
            return false;
        }
        for (int i = 0; i < NUMBER_OF_RAWS; i++)
        {
            char filename[10];
            sprintf(filename, "raw%d.bin", i);
            InfraRed::readData(filename, data[i]);
        }
        IrSender.begin(IR_SEND_PIN, false);
        return true;
    }

    bool initRecRaw()
    {
        IrReceiver.begin(IR_RECEIVE_PIN, 0);
        uint8_t idRaw = 0;
        while (idRaw < NUMBER_OF_RAWS)
        {
            if (IrReceiver.decode())
            {
                InfraRed::registerRaw(data[idRaw]); // Função que grava o raw
                idRaw++;
                IrReceiver.resume();
            }
        }
        for (int i = 0; i < NUMBER_OF_RAWS; i++)
        {
            char filename[10];
            sprintf(filename, "raw%d.bin", i);
            InfraRed::writeData(filename, data[i]);
        }
        IrReceiver.stop();
        return true;
    }

    void power(bool power)
    {
        if (power)
            InfraRed::sendRaw(data[1]);
        else
            InfraRed::sendRaw(data[0]);
    }

    bool setTemperature(uint8_t temperature)
    {
        temperature -= MIN_TEMPERATURE_VALUE;
        InfraRed::sendRaw(data[temperature]);
        return true;
    }
};
extern InfraRed Infra;