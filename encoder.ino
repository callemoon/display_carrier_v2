// Encoder
const int encoderA = 20;
const int encoderB = 21;
const int buttonPin = 10;

volatile long encoderPosition = 0;
volatile int lastEncoded = 0;

void IRAM_ATTR readEncoder() {
  int A = digitalRead(encoderA);
  int B = digitalRead(encoderB);

  int encoded = (A << 1) | B;
  int sum = (lastEncoded << 2) | encoded;

  if (sum == 0b1101 || sum == 0b0100 ||
      sum == 0b0010 || sum == 0b1011) {
    encoderPosition++;
  }

  if (sum == 0b1110 || sum == 0b0111 ||
      sum == 0b0001 || sum == 0b1000) {
    encoderPosition--;
  }

  lastEncoded = encoded;
}

void setup() {
  Serial.begin(115200);

  pinMode(encoderA, INPUT_PULLUP);
  pinMode(encoderB, INPUT_PULLUP);
  pinMode(buttonPin, INPUT_PULLUP);

  lastEncoded = (digitalRead(encoderA) << 1)
                | digitalRead(encoderB);

  attachInterrupt(digitalPinToInterrupt(encoderA),
                  readEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderB),
                  readEncoder, CHANGE);

  Serial.println("Encoder startad");
}

void loop() {
  static long lastPosition = 0;
  static bool lastButtonState = HIGH;
  static unsigned long lastDebounceTime = 0;

  const unsigned long debounceDelay = 40;

  // Läs encoderposition säkert
  long position;
  noInterrupts();
  position = encoderPosition;
  interrupts();

  if (position != lastPosition) {
    Serial.print("Position: ");
    Serial.println(position);
    lastPosition = position;
  }

  // Läs knapp med enkel debounce
  bool buttonState = digitalRead(buttonPin);

  if (buttonState != lastButtonState) {
    lastDebounceTime = millis();
    lastButtonState = buttonState;
  }

  static bool buttonPressed = false;

  if (millis() - lastDebounceTime > debounceDelay) {
    if (buttonState == LOW && !buttonPressed) {
      buttonPressed = true;
      Serial.println("Knapp tryckt!");
    }

    if (buttonState == HIGH) {
      buttonPressed = false;
    }
  }
}
