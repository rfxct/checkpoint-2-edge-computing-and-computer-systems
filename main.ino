#include <LiquidCrystal.h>

#define LED_OK 4
#define LED_ALERTA 3
#define LED_PROBLEMA 2

#define LDR_SENSOR A0
#define TMP_SENSOR A1
#define HMD_POTENCIOMETRO A2
#define BUZZER 5

#define TEMPO_ATUALIZACAO 5000

// LCD 
LiquidCrystal lcd(
  13, // Register Select 
  12, // Leitura e Escrita
  11, // Enable
  10, // Pin de dados 4
  9, // Pin de dados 5
  8, // Pin de dados 6
  7 // Pin de dados 7
);

float luminosidade = 0;
float umidade = 0;
float temperatura = 0;

long ultimaAtualizacaoLCD = 0;
int etapaAtualIdx = 0;
String etapas[] = {
  "luminosidade",
  "temperatura",
  "umidade"
};

void setup() {
  lcd.begin(16, 2);
  pinMode(LED_OK, OUTPUT);
  pinMode(LED_ALERTA, OUTPUT);
  pinMode(LED_PROBLEMA, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(HMD_POTENCIOMETRO, INPUT);

  Serial.begin(9600);
}

void loop() {
  long agora = millis();

  temperatura = lerTemperatura();
  umidade = lerUmidade();
  luminosidade = lerLuminosidade();

  if (agora - ultimaAtualizacaoLCD >= TEMPO_ATUALIZACAO) {
    ultimaAtualizacaoLCD = agora;

    String etapaAtual = etapas[etapaAtualIdx];
    Serial.print("Etapa: ");
    Serial.println(etapaAtual);

    switch (etapaAtualIdx) {
    case 0:
      etapaAtualIdx += 1;
      atualizarDisplayLumi();
      break;
    case 1:
      etapaAtualIdx += 1;
      atualizarDisplayTemp();
      break;
    case 2:
      etapaAtualIdx = 0;
      atualizarDisplayUmi();
      break;
    }
  }

  // LED OK
  if (luminosidade < 300) {
    digitalWrite(LED_OK, HIGH);

    // Reset
    resetarPadroes(LED_ALERTA, LED_PROBLEMA, true);
  }

  // LED Alerta
  if (
    (luminosidade >= 300 && luminosidade < 800) ||
    (temperatura < 10 || temperatura > 15)
  ) {
    digitalWrite(LED_ALERTA, HIGH);

    if (temperatura < 10 || temperatura > 15) {
      tone(BUZZER, 1000);
    } else noTone(BUZZER);

    // Reset
    resetarPadroes(LED_OK, LED_PROBLEMA, false);
  }

  // LED Problema  
  if (
    luminosidade >= 800 ||
    (umidade < 50 || umidade > 70)
  ) {
    digitalWrite(LED_PROBLEMA, HIGH);
    tone(BUZZER, 1000);

    // Reset
    resetarPadroes(LED_OK, LED_ALERTA, false);
  }
}

void atualizarDisplayLumi() {
  if (
    (luminosidade >= 300 && luminosidade < 800)
  ) {
    lcd.clear();
    lcd.print("Ambiente a meia");
    lcd.setCursor(0, 1);
    lcd.println("luz");
  } else if (
    luminosidade >= 800
  ) {

    lcd.clear();
    lcd.print("Ambiente muito");
    lcd.setCursor(0, 1);
    lcd.println("CLARO");
  }
}

void atualizarDisplayTemp() {
  if (temperatura > 15) {
    lcd.clear();
    lcd.print("Temp. Alta");
    lcd.setCursor(0, 1);
    lcd.print("Temp. = ");
    lcd.print(temperatura);
    lcd.println("C");
  } else if (temperatura < 10) {
    lcd.clear();
    lcd.print("Temp. Baixa");
    lcd.setCursor(0, 1);
    lcd.print("Temp. = ");
    lcd.print(temperatura);
    lcd.println("C");
  } else {
    lcd.clear();
    lcd.print("Temp. OK");
    lcd.setCursor(0, 1);
    lcd.print("Temp. = ");
    lcd.print(temperatura);
    lcd.println("C");
  }
}
void atualizarDisplayUmi() {
  if (umidade > 70) {
    lcd.clear();
    lcd.print("Umidade. Alta");
    lcd.setCursor(0, 1);
    lcd.print("Umidade = ");
    lcd.print(umidade);
    lcd.println("%");
  } else if (umidade < 10) {
    lcd.clear();
    lcd.print("Umidade. Baixa");
    lcd.setCursor(0, 1);
    lcd.print("Umidade = ");
    lcd.print(umidade);
    lcd.println("%");
  } else {
    lcd.clear();
    lcd.print("Umidade. OK");
    lcd.setCursor(0, 1);
    lcd.print("Umidade = ");
    lcd.print(umidade);
    lcd.println("%");
  }
}

float lerLuminosidade() {
  int total = 0;
  for (int i = 0; i < 5; i++) {
    luminosidade = analogRead(LDR_SENSOR);
    luminosidade = map(luminosidade, 54, 974, 0, 900);

    total += luminosidade;
    delay(10);
  }
  return total / 5;
}

float lerTemperatura() {
  int total = 0;
  for (int i = 0; i < 5; i++) {
    // Temperatura [-∞...∞]
    temperatura = (analogRead(TMP_SENSOR) / 1023.0) * 5000;
    temperatura = (temperatura - 500) * 0.1;

    total += temperatura;
    delay(10);
  }
  return total / 5;
}

float lerUmidade() {
  int total = 0;
  for (int i = 0; i < 5; i++) {
    // Umidade [20...90]
    umidade = analogRead(HMD_POTENCIOMETRO);
    umidade = map(umidade, 0, 1023, 20, 90);

    total += umidade;
    delay(10);
  }
  return total / 5;
}

void resetarPadroes(int led1, int led2, bool resetBuzzer) {
  if (resetBuzzer) noTone(BUZZER);

  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
}