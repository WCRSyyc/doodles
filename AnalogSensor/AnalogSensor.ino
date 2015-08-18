/* Analog sensor

   Handle an analog sensor where the result is to be turned into a boolean
   HIGH / LOW, true / false result, with suppression of spurious transitions
   due to analog sensor jitter near the cross over voltage.

   Fixed / [automatic] detected cutoff voltage
   Fixed / percentage / calibrated jitter dead band

   Probably to be wrapped into a library at some point
 */
unsigned const int SERIAL_SPEED = 9600;
//300, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, 115200

unsigned const int MAX_ADC = 1023;// ADC == analog to digital conversion
unsigned const int MIN_ADC = 0;
const float DEAD_BAND_FACTOR = 0.25;// +/- 25% of the size of the complete range
unsigned const int FIXED_DEAD_BAND = 40;// +/- 40 should be plenty to prevent jitter/bounce
unsigned const int MIN_DEADBAND = 5;// prevent jitter while doing initial calibration

unsigned const int SENSE_PIN = A0;

// Internal 'instance' data
// To do automatic calibration based on the seen range of values, maintain separate
// range and dead band thresholds
unsigned int threshold[2]; // bottom and top of the dead band range
unsigned int inputRange[2]; // min and max values actually seen
boolean boolState = HIGH; // the current boolean state

// Testing information
unsigned long adcCount = 0;
unsigned long stateCount = 0;
unsigned long calibrationCount = 0;
unsigned long toggleTime, calibrationTime;

boolean prevState = HIGH;
unsigned long readCount = 0;
unsigned long baseTime;

void setup() {
  Serial.begin(SERIAL_SPEED);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  // class constructor
  // For automatic (or requested) calibration, set the initial threshold to the
  // extreme limits for possible raw sensor values, reversed so that any reading
  // will change both limits.
  inputRange[HIGH] = MIN_ADC;
  inputRange[LOW] = MAX_ADC;
  toggleTime = calibrationTime;
  // Serial.print("HIGH: ");
  // Serial.print(HIGH);// 1
  // Serial.print(", LOW: ");
  // Serial.println(LOW);// 0
  calibrationTime = micros();
  baseTime = micros();
}

void loop() {
  boolean test;
  // test = getStateDBG(SENSE_PIN);
  test = getState(SENSE_PIN);
  if (test != prevState) {
    prevState = test;
    Serial.print("State = ");
    Serial.print(test);
    Serial.print(" at ");
    Serial.print(micros());
    Serial.println(" microseconds");
  }
  readCount += 1;
  if (micros() - baseTime > 1000000) {
    Serial.print(readCount);
    Serial.println(" sensor reads in the past 1 second");
    readCount = 0;
    baseTime = micros();
  }
}

boolean getState(unsigned int pinNumber) {
  unsigned int raw;
  raw = analogRead(pinNumber);

  if (raw < inputRange[LOW]) {
    inputRange[LOW] = raw;
    autoCalibrate(inputRange);
  }
  if (raw > inputRange[HIGH]) {
    inputRange[HIGH] = raw;
    autoCalibrate(inputRange);
  }

  return analogToBoolean(raw);
}
boolean getStateDBG(unsigned int pinNumber) {
  unsigned int raw;
  boolean newLimit = false;
  unsigned long timePoint;

  timePoint = micros();
  raw = analogRead(pinNumber);
  adcCount += 1;

  if (raw < inputRange[LOW]) {
    inputRange[LOW] = raw;
    newLimit = true;
  }
  if (raw > inputRange[HIGH]) {
    inputRange[HIGH] = raw;
    newLimit = true;
  }
  if (newLimit) {
    autoCalibrate(inputRange);
    calibrationCount += 1;

    // debug : report changes, to get a feel for how fast they settle
    Serial.print("adc Count ");
    Serial.print(adcCount);
    Serial.print("; new range [");
    Serial.print(inputRange[LOW]);
    Serial.print(", ");
    Serial.print(inputRange[HIGH]);
    Serial.print("]; adjusted boundaries [");
    Serial.print(threshold[LOW]);
    Serial.print(", ");
    Serial.print(threshold[HIGH]);
    Serial.print("] at: ");
    Serial.print(timePoint, DEC);
    Serial.print(" microseconds, after ");
    Serial.print(timePoint - calibrationTime, DEC);
    Serial.println(" microseconds");
    calibrationTime = timePoint;
  }

  return analogToBooleanDBG(raw, timePoint);
}

boolean analogToBoolean(unsigned int value) {
  boolean curState = boolState;
  if (threshold[HIGH] - threshold[LOW] < MIN_DEADBAND) {
    // Do not toggle state until have seen enough input range to create a
    // minimally valid dead band
    return boolState;
  }
  if (boolState == HIGH) {
    if (value < threshold[LOW]) {
      curState = LOW;
    }
  } else {
    if (value > threshold[HIGH]) {
      curState = HIGH;
    }
  }
  boolState = curState;
  return boolState;
}
boolean analogToBooleanDBG(unsigned int value, unsigned long timeRef) {
  boolean curState = boolState;
  if (threshold[HIGH] - threshold[LOW] < MIN_DEADBAND) {
    // Do not toggle state until have seen enough input range to create a
    // minimally valid dead band
    return boolState;
  }

  if (boolState == HIGH) {
    if (value < threshold[LOW]) {
      curState = LOW;
    }
  } else {
    if (value > threshold[HIGH]) {
      curState = HIGH;
    }
  }
  if (curState != boolState) {
    // debug : report boolean state changes
    Serial.print("new state ");
    Serial.print(curState ? "HIGH" : "LOW");
    Serial.print(" at ");
    Serial.print(timeRef, DEC);
    Serial.print(" microseconds, after ");
    Serial.print(timeRef - toggleTime, DEC);
    Serial.println(" microseconds");
    toggleTime = timeRef;
  }
  boolState = curState;
  return boolState;
}

// Determine new dead band threshold limits
void autoCalibrate(unsigned int * range) {
  unsigned int median, extent, halfBand;
  // Get the midpoint between the minimum and maximum values to use as
  // the base threshold between LOW and HIGH ADC values.
  median = (range[LOW] + range[HIGH]) / 2;
  // Get the extent for the sense range.
  extent = range[HIGH] - range[LOW];
  // Pick a dead-band size where the boolean state will not change.
  halfBand = min(extent * DEAD_BAND_FACTOR, FIXED_DEAD_BAND);

  // Set the threshold with the dead band range boundary limits
  threshold[LOW] = median - halfBand;
  threshold[HIGH] = median + halfBand;
}
