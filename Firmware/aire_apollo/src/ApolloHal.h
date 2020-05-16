#ifndef APOLLO_HALL_H
#define APOLLO_HALL_H

#include "../include/defaults.h"
#include "strings.h"

#include "Sensor/FlowSensor/ApolloFlowSensor.h"
#include "Sensor/Pressure/ApolloPressureSensor.h"
#include "Valve/ApolloValve.h"
#include "ApolloAlarms.h"
#include <PID_v1.h>
#include <ArduinoJson.h>



class ApolloHal
{

enum pressureMode
{
  none,
  rampUpPressure,
  constantPressure,
  lastItem
};


public:
//    ApolloHal(ApolloPressureSensor *preSensor, ApolloFlowSensor *entryFlowSensor, ApolloFlowSensor *exitFlowSensor, ApolloValve *entryEV, ApolloValve *exitEV, ApolloAlarms *alarms);
    ApolloHal();
    ~ApolloHal();

    void addValves        (ApolloValve* inputValve,ApolloValve* outputValve);
    void addPressureSensor(ApolloPressureSensor* pressureSensor);
    void addFlowSensors   (ApolloFlowSensor* inputFlow, ApolloFlowSensor* outputFlow);

    void debug(String debugmsg);
    void sendData();


    bool  begin();
    bool  test();
    bool  calibrate();
    bool  calibratePressure();


    void  risePressure(float pressure, uint16_t targetTime);
    bool  setConstantFlow     (float flow, float maxPressure);
    bool  setConstantPressure (float pressure);


    void openInputValve     (uint8_t percent = 100, bool wait = false);
    void closeInputValve    (bool wait = false);
    void openOutputValve    (uint8_t percent = 100, bool wait = false);
    void closeOutputValve   (bool wait = false);


    double getPressure()          { return _lastPressure;}
    double getInputValveStatus()  { return _lastInputValveStatus;}
    double getOutputValveStatus() { return _lastOutputValveStatus;}
    double getInputValveTarget()  { return _lastInputValveTarget;}
    double getOutputValveTarget() { return _lastOutputValveTarget;}

    double getInputFlow()         { return _lastInputFlow;}
    double getInputInstantFlow()  { return _lastInputInstantFlow;}
    void   resetInputFlow()       { _inputFlowSensor->resetFlow();}

    double getOutputFlow()        { return _lastOutputFlow;}
    double getOutputInstantFlow() { return _lastOutputInstantFlow;}
    void   resetOutputFlow()      { _outputFlowSensor->resetFlow();}


    void update();


  #ifdef INTFLOWSENSOR // Gestion de los sensores de flujo que usan interrupciones
    void flowIn()
    {
      _inputFlowSensor->pulse();
    }

    void flowOut()
    {
      _outputFlowSensor->pulse();
    }
  #endif


  private:

    void initPIDs();
    void computePIDs();
    void highFrecuencyUpdate();
    void sensorUpdate();
    void sendMedicalData();
    void sendHardwareInfo();
    void sendConfig();

    ApolloPressureSensor  *_inputPressureSensor;
    ApolloFlowSensor      *_inputFlowSensor;
    ApolloFlowSensor      *_outputFlowSensor;
    ApolloValve           *_inputValve;
    ApolloValve           *_outputValve;
    ApolloAlarms          *_alarms;

//
    float  _lastInputValveStatus;
    float  _lastOutputValveStatus;
    float  _lastInputValveTarget;
    float  _lastOutputValveTarget;
    float  _lastPressure;
    float  _lastInputFlow;
    float  _lastOutputFlow;
    float  _lastInputInstantFlow;
    float  _lastOutputInstantFlow;


    uint16_t _lastInspiratoryRiseTimeMS;
    unsigned long _lastInspiratoryRiseStart;
    float    _lastInspiratoryValveStatus;
    bool     _pressureTargetArchived;

    bool   _hasPressureSensor;
    bool   _hasFlowSensors;

    pressureMode _pressureMode = none;
    float        _pressureTarget;

//CONTROL PID
    bool    _constantPressureEnabled;
    PID     _constantPressurePID;
    double  _constantPressurePIDInput;
    double  _constantPressurePIDTarget;
    double  _constantPressurePIDOutput;
    double  _constantPressurePIDKp;
    double  _constantPressurePIDKd;
    double  _constantPressurePIDKi;

    bool   _constantFlowEnabled;
    PID    _constantFlowPID;
    double _constantFlowPIDInput;
    double _constantFlowPIDTarget;
    double _constantFlowPIDOutput;
    double _constantFlowPIDKp;
    double _constantFlowPIDKd;
    double _constantFlowPIDKi;

    bool    _overPressureTriggered;
    PID     _overPressurePID;
    double  _overPressurePIDInput;
    double  _overPressurePIDTarget;
    double  _overPressurePIDOutput;
    double  _overPressurePIDKp;
    double  _overPressurePIDKd;
    double  _overPressurePIDKi;


    PID     _inspiratoryRisePID;
    double  _inspiratoryRisePIDInput;
    double  _inspiratoryRisePIDTarget;
    double  _inspiratoryRisePIDOutput;
    double  _inspiratoryRisePIDKp;
    double  _inspiratoryRisePIDKd;
    double  _inspiratoryRisePIDKi;

//
    unsigned long _lastSensorLoopMicros;
    unsigned long _lastHighFreqUpdateMicros;
    unsigned long _lastTelemetryUpdateMicros;

    unsigned long _lastTelemetryUpdate;
    unsigned long _lastSensorsUpdate ;
    unsigned long _lastCommunicationsUpdate;
};

#endif
