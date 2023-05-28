#include "EnergyMeter.hpp"

PZEM004Tv30 Pzem(Serial2, 16, 17);

bool EnergyMeter::init()
{
    Pzem.setAddress(0x01);
    int responseHardware = Pzem.readAddress();

    if(responseHardware != 0x01)
    {
        return false;
    }

    Pzem.resetEnergy();
    
    return true;
}

bool EnergyMeter::getStateAirConditioner()
{
    return stateAirConditioner;
}

void EnergyMeter::handleMeter(unsigned long int *timeNow)
{

    if ((*timeNow) - lastTimeUpdated > UPDATEINTERVALTIME)
    {
        lastTimeUpdated = (*timeNow);

        dataMeasured.voltage = Pzem.voltage();
        dataMeasured.current = Pzem.current();
        dataMeasured.power = Pzem.power();
        dataMeasured.energy = Pzem.energy();
        dataMeasured.pf = Pzem.pf();

        if (dataMeasured.power == 0)
        {
            stateAirConditioner = POWER_OFF;     
        }
        else
        {
            stateAirConditioner = POWER_ON;   
        }
    }
}

void EnergyMeter::resetDataMeasured()
{
    Pzem.resetEnergy();
    Serial.println("resetDataMeasured");
}

float EnergyMeter::getAirConditionerVoltage()
{
    return dataMeasured.voltage;
}

float EnergyMeter::getAirConditionerCurrent()
{
    return dataMeasured.current;
}

float EnergyMeter::getAirConditionPower()
{
    return dataMeasured.power;
}

float EnergyMeter::getAirConditionerEnergy()
{
    return dataMeasured.energy;
}

float EnergyMeter::getAirConditionerPowerFactor()
{
    return dataMeasured.pf;
}

