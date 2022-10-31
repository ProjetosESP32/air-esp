/**
 * @file InfraRed.cpp
 * @author Marcus Vinícius (marcaovini07@gmail.com)
 * @brief Arquivo da gerência de infra vermelho de ar condicionado
 * @version 0.1
 * @date 2022-09-04
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "InfraRed.hpp"
#include "SPIFFS.h"
#include "FS.h"
#include <IRremote.hpp>

void InfraRed::readRaws()
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

void InfraRed::writeRaws()
{
  auto file = SPIFFS.open(RAWS_FILE, FILE_WRITE, true);

  for (auto &raw : raws)
  {
    file.write(raw.data(), RAW_BUFFER_LENGTH);
  }

  file.close();
}

void InfraRed::sendRaw(int rawIndex)
{
  IrSender.sendRaw(raws[rawIndex].data(), RAW_BUFFER_LENGTH, KHZ_ONE);
  delay(5);
  IrSender.sendRaw(raws[rawIndex].data(), RAW_BUFFER_LENGTH, KHZ_TWO);
}

void InfraRed::registerRaw(int rawIndex)
{
  IrReceiver.compensateAndStoreIRResultInArray(raws[rawIndex].data());
}

bool InfraRed::begin()
{
  if (!SPIFFS.begin(true))
  {
    return false;
  }

  readRaws();

  IrSender.begin(IR_SEND_PIN, ENABLE_LED_FEEDBACK);

  return true;
}

bool InfraRed::recRaws()
{
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

  int idRaw = 0;
  while (idRaw < NUMBER_OF_RAWS)
  {
    if (IrReceiver.decode())
    {
      registerRaw(idRaw++);
      IrReceiver.resume();
    }
  }

  writeRaws();
  IrReceiver.stop();

  return true;
}

void InfraRed::sendPower(bool power)
{
  sendRaw(power ? 1 : 0);
}

void InfraRed::sendTemperature(int temperature)
{
  if (MIN_TEMPERATURE_VALUE <= temperature && temperature <= MAX_TEMPERATURE_VALUE)
  {
    int index = temperature - (MIN_TEMPERATURE_VALUE - 2); // isso pois os index 0 e 1 são para desligar e ligar, respectivamente
    sendRaw(index);
  }
}
