/**
 * @file InfraRed.hpp
 * @author Marcus Vinícius (marcaovini07@gmail.com)
 * @brief Arquivo de cabeçalho da gerência de infra vermelho de ar condicionado
 * @version 0.1
 * @date 2022-09-04
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef _INFRARED_HPP_
#define _INFRARED_HPP_

#include <array>

constexpr int NUMBER_OF_RAWS = 10;
constexpr int IR_RECEIVE_PIN = 26; // D15
constexpr int IR_SEND_PIN = 4;     // D4
constexpr int LED_BUILTIN = 16;    // RX2
constexpr int MIN_TEMPERATURE_VALUE = 18;
constexpr int MAX_TEMPERATURE_VALUE = 25;
constexpr int KHZ_ONE = 38;
constexpr int KHZ_TWO = 32;
constexpr char RAWS_FILE[] = "/raws.bin";

// Os três próximos #define alteram alguns valores padrão da lib que não funcionam com protocolos de ar condicionado
#define RAW_BUFFER_LENGTH 750   // bits do protocolo IR, um protocolo de 100 bits gera um array de 200 elementos ou seja 2x
#define RECORD_GAP_MICROS 52000 // Tempo do gap em micros da gravação
#define MARK_EXCESS_MICROS 20   // Sugerido pela lib para o módulo VS1838
#define SEND_PWM_BY_TIMER

class InfraRed
{
private:
  std::array<std::array<uint8_t, RAW_BUFFER_LENGTH>, NUMBER_OF_RAWS> raws;
  void readRaws();
  void writeRaws();
  void sendRaw(int rawIndex);
  void registerRaw(int rawIndex);

public:
  bool begin();
  bool recRaws();
  void sendPower(bool power);
  void sendTemperature(int temperature);
};

#endif
