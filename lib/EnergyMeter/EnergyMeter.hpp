#include <Arduino.h>
#include <PZEM004Tv30.h>

#define POWER_ON true
#define POWER_OFF false

#define UPDATEINTERVALTIME 1000

#ifndef _ENERGY_METER_
#define _ENERGY_METER_

typedef struct energyData
{
  float voltage=0;
  float current=0;
  float power=0;
  float energy=0;
  float pf=0;
} data;



class EnergyMeter {
    private:
        float lastCurrentMeasured = 0;
        bool stateAirConditioner = NULL;
        unsigned long int *time=0;
        unsigned long int lastTimeUpdated=0;
    public:
        data dataMeasured;
        bool init();
        void handleMeter(unsigned long int * timeNow);        
        bool getStateAirConditioner();
        void resetDataMeasured();
        float getAirConditionerVoltage();
        float getAirConditionerCurrent();
        float getAirConditionPower();
        float getAirConditionerEnergy();
        float getAirConditionerPowerFactor();
};
#endif