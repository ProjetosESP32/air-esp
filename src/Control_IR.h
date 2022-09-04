//by: Marcus Vinícius
#include <Arduino.h>
#define IR_RECEIVE_PIN          26  // D15
#define IR_SEND_PIN              4  // D4
#define REC_PIN 16

#define NUMBER_OF_RAWS          10
#define khz_start 32

//Os três próximos #define alteram alguns valores padrão da lib que não são funcionam com protocolos de ar condicionado
#define RAW_BUFFER_LENGTH  750 // bits do protocolo IR, um protocolo de 100 bits gera um array de 200 elementos ou seja 2x
#define RECORD_GAP_MICROS 52000 // Tempo do gap em micros da gravação 
#define MARK_EXCESS_MICROS  20 // Sugerido pela lib para o módulo VS1838
#include <IRremote.hpp>
#include "LITTLEFS.h" // Sistema para gravar na flash do esp

class Infra_red {
    private:
        uint8_t Onn[RAW_BUFFER_LENGTH];
        uint8_t Off[RAW_BUFFER_LENGTH];
        uint8_t temp1[RAW_BUFFER_LENGTH];
        uint8_t temp2[RAW_BUFFER_LENGTH];
        uint8_t temp3[RAW_BUFFER_LENGTH];
        uint8_t temp4[RAW_BUFFER_LENGTH];
        uint8_t temp5[RAW_BUFFER_LENGTH];
        uint8_t temp6[RAW_BUFFER_LENGTH];
        uint8_t temp7[RAW_BUFFER_LENGTH];
        uint8_t temp8[RAW_BUFFER_LENGTH];
        
        void readData(String path, uint8_t *raw) { //Lê arquivos -> utilizando ponteiros para ser o mudar a variável e não sua cópia
            String buffer_read; //String intermediária pois os dados são gravados em texto, não em int, sendo necessário conversão
            File file = LITTLEFS.open(path, FILE_READ); // Abre aquivo com o nome definido no modo leitura
            while (file.available()) { // Enquando houver informação para ser lida retorna verdadeiro
                buffer_read = file.readStringUntil('\n'); // Lê o arquivo até \n pra saber que é o fim da linha
                *raw = buffer_read.toInt(); // passa o arquivo de String para int
                raw++;// incrementa endereço do ponteiro
        }
            file.close(); // Fecha arquivo
        }

        void SendRaw(uint8_t *raw) { //Função que envia sinal para o ar condicionado -> Utiliza ponteiros pois o array é muito grande
            for (int i = 0; i < 7 ; i++) {
                IrSender.sendRaw(raw, RAW_BUFFER_LENGTH , khz_start + i); // Array do sinal, tamanho do array, frequência do sinal
                delay(5);
        }
        }

        void writeData(String path, uint8_t *raw ) { // Usando ponteiro pois o array é muito grande
            File file = LITTLEFS.open(path, FILE_WRITE); // Cria ou abre um arquivo com o nome definido no modo escrita
            for (int i = 0; i < RAW_BUFFER_LENGTH  ; i++) { // laço for que passa cada elemento do array para a flash
            file.println(*(raw + i)); // Escreve cada valor por linha e incrementa endereço do ponteiro
        }
        file.close(); // fecha o arquivo
        }

        void registerRaw(uint8_t *raw) {
            IrReceiver.compensateAndStoreIRResultInArray(raw); // Salva raw no seu respectivo array
        }
        public:

        bool Power(bool power){
            if(power) Infra_red::SendRaw(Onn);
            else Infra_red::SendRaw(Off);
        }
        

        bool begin() {
            pinMode(REC_PIN, INPUT);
            if (!LITTLEFS.begin(true)) { // Inicia comunicação
                return false;
            }
            Infra_red::readData("/rawON.txt", Onn); 
            Infra_red::readData("/rawMacro1.txt", temp1);
            Infra_red::readData("/rawMacro2.txt", temp2);
            Infra_red::readData("/rawMacro3.txt", temp3);
            Infra_red::readData("/rawMacro4.txt", temp4);
            Infra_red::readData("/rawMacro5.txt", temp5);
            Infra_red::readData("/rawMacro6.txt", temp6);
            Infra_red::readData("/rawMacro7.txt", temp7);
            Infra_red::readData("/rawMacro8.txt", temp8);
            IrSender.begin(IR_SEND_PIN, false);
            return true;
        }
        bool InitRecRaw() {
            IrReceiver.begin(IR_RECEIVE_PIN, 0);
            uint8_t idRaw = 0;
            while (idRaw < NUMBER_OF_RAWS) {

                if (IrReceiver.decode()) {
                switch (idRaw) {
                    case 0:
                    Infra_red::registerRaw(Off); //Função que grava o raw
                    break;
                    case 1:
                    Infra_red::registerRaw(Onn);
                    break;
                    case 2:
                    Infra_red::registerRaw(temp1);
                    break;
                    case 3:
                    Infra_red::registerRaw(temp2);
                    break;
                    case 4:
                    Infra_red::registerRaw(temp3);
                    break;
                    case 5:
                    Infra_red::registerRaw(temp4);
                    break;
                    case 6:
                    Infra_red::registerRaw(temp5);
                    break;
                    case 7:
                    registerRaw(temp6);
                    break;
                    case 8:
                    Infra_red::registerRaw(temp7);
                    break;
                    case 9:
                    Infra_red::registerRaw(temp8);
                    break;
                }
                idRaw++;
                IrReceiver.resume();
                }
        }
            Infra_red::writeData("/rawON.txt", Onn); //Grava o raw presenta na ram para a flash no seu respectivo Arquivo
            Infra_red::writeData("/rawOFF.txt", Off);
            Infra_red::writeData("/rawMacro1.txt", temp1);
            Infra_red::writeData("/rawMacro2.txt", temp2);
            Infra_red::writeData("/rawMacro3.txt", temp3);
            Infra_red::writeData("/rawMacro4.txt", temp4);
            Infra_red::writeData("/rawMacro5.txt", temp5);
            Infra_red::writeData("/rawMacro6.txt", temp6);
            Infra_red::writeData("/rawMacro7.txt", temp7);
            Infra_red::writeData("/rawMacro8.txt", temp8);
            IrReceiver.stop();
            return true;
        }
        bool setTemperature(uint8_t temperature) {
        switch (temperature) {
            case 18:
            Infra_red::SendRaw(temp1);
            break;
            case 19:
            Infra_red::SendRaw(temp2);
            break;
            case 20:
            Infra_red::SendRaw(temp3);
            break;
            case 21:
            Infra_red::SendRaw(temp4);
            break;
            case 22:
            Infra_red::SendRaw(temp5);
            break;
            case 23:
            Infra_red::SendRaw(temp6);
            break;
            case 24:
            Infra_red::SendRaw(temp7);
            break;
            case 25:
            Infra_red::SendRaw(temp8);
            break;
            default:
            return false;
        }
        return true;
        }

};
extern Infra_red Infra;