#include <avr/eeprom.h>


boolean butt_flag_open = 0; // but_flug_right
boolean butt_open = 0;      // but_right
boolean led_flag_open = 0; // led_flag_right
unsigned long last_press_open = 0; //last_press_right
//=========================================

//=========================================
boolean butt_flag_close = 0;  // butt_flag_left
boolean butt_close = 0;       // butt_left
boolean led_flag_close = 0;    // led_flag_left
unsigned long last_press_close = 0;  // last_press_left
//=========================================

//=========================================
boolean bool_open_limit_switch = 0; // bool_right_limit_switch
boolean bool_close_limit_switch = 0; // bool_left_limit_switch
//=========================================

//=========================================
byte limSW_type = 1; //1 = NORMAL OPEN AND 0 = NORMAL CLOSE
boolean LIMsw_TYPE = 0;
boolean third_lim = 0;
boolean fourth_lim = 0;
//=========================================

//=========================================
unsigned long last_press_open_for_go = 1000; // last_press_right_for_go     // потрібна для того щоб після припенення руху брами в одну зі сторін, брама почала рухатись в іншу сторону тільки після проходження 1 секунди
unsigned long last_press_close_for_go = 1000; // last_press_left_for_go      // потрібна для того щоб після припенення руху брами в одну зі сторін, брама почала рухатись в іншу сторону тільки після проходження 1 секунди
//=========================================

//=========================================
unsigned long currentMillis;
unsigned long lock = 0;
boolean but_lock = 0;
int time_afret = 100;
//=========================================

//=========================================
boolean C_butt = 0;                               // для кнопки C на пульту
boolean C_butt_flug = 0;
boolean C_signal = 0;
unsigned long C_last_butt_press = 0;
//=========================================

//=========================================
boolean D_butt = 0;                               // для кнопки D на пульту
boolean D_butt_flug = 0;
boolean D_signal = 0;
unsigned long D_last_butt_press = 0;
//=========================================

//=========================================
int ledState = LOW;             // этой переменной устанавливаем состояние светодиода
long previousMillis = 0;        // храним время последнего переключения светодиода
long interval = 650;            // интервал между включение/выключением светодиода (1 секунда)
//=========================================

//=========================================
unsigned long time_open_lm_sw = 0; // time_right_lm_sw  // для того щоб брама після спрацювання кінцевика почала рухатись через деякий час
unsigned long time_close_lm_sw = 0; // time_left_lm_sw   // для того щоб брама після спрацювання кінцевика почала рухатись через деякий час
unsigned long pause = 1125;        // той самий деякий час
unsigned long buttonPause = 500;        // пауза для того щоб брама після нажаття кнопки на пульті знову зреагувала через встановлений час

#define butt_PIN 4
#define buzer_PIN 14

#define TIME 2000 // час після якого анулюється таймер якщо не натискати на кнопку
#define butt_CLICK 5 // кількі. кліків які потрібно зробити для того щоб щось сталось

#define pres_TIME 2000 // час який потрібно постійно тиснути на кнопку щоб щось сталось
#define buzer_ON 1 // дія яка буде виконуватись після того як кнопку будуть тиснути визначену кількість часу

#define signal_is 500
#define signal_no 200

#define buzer_WORK 150
#define buzer_DONT_WORK 75

#define limSW_type fourth_lim

boolean buzer_STATUS = 0;
boolean REP = 0;

unsigned long TIME_ON = 0;
unsigned long TIME_OF = 0;
unsigned long REPETITIONS = 0;


boolean BUTTON = 0;
boolean BUTTON_FLUG = 0;
boolean LED = 0;
boolean PRES = 1;
boolean BUZER_SIGNAL = 0;
boolean butt_already_pres = 0; // для того щоб тільки раз щось відбулося коли нажали кнопку, це щось відбудеться знову тільки тоді коли кнопу відпустять і нажмуть повторно

boolean butt_turn_on = 0;



unsigned long BUTT_DELAY = 150;
unsigned long LAST_BUTT_PRES = 0;
unsigned long BUTT_COUNTER = 0;
unsigned long PRES_TIME_COUNTER = 0;
unsigned long buzer_TIME_ON = 0;

unsigned long butt_noise = 0;

int buzer_REPETITIONS = 0;

int buzer_failed = 0;




void setup() {
  pinMode(0, INPUT_PULLUP);     // сюди підключаємо контакт від плати приймача(рух в праву сторону) // 2 пін МК
  pinMode(1, INPUT_PULLUP);     // сюди підключаємо контакт від плати приймача(рух в ліву сторону)  // 3 пін МК
  pinMode(2, INPUT_PULLUP);     // вхі дсигналу з пульта через транзистор (кнопка C на пульту) 8-2
  pinMode(3, INPUT_PULLUP);     // вхі дсигналу з пульта через транзистор (кнопка D на пульту) 9-3


  pinMode(butt_PIN, INPUT_PULLUP); // 4 PIN
  pinMode(5, INPUT_PULLUP);     // вивод для підключенн ІЧ датчика руху 7-5
  pinMode(6, INPUT_PULLUP);     // правий кінцевик (подаємо мінус через кінцевик) 4-6
  pinMode(7, INPUT_PULLUP);     // лівий кінцевик (подаємо мінус через кінцевик) 5-7


  pinMode(8, OUTPUT);           // рух в праву сторону (до драйвера двигуна)  2-8            // 4 пін МК
  pinMode(9, OUTPUT);           // рух в ліву сторону (до драйвера двигуна) 3-9
  pinMode(10, OUTPUT);          // виход з пульта (кнопка C на пульту)
  pinMode(11, OUTPUT);          // виход з пульта (кнопка D на пульту)
  pinMode(12, OUTPUT);          // вивод для підключенн індикатора руху 6-4    4-13   13-12
  pinMode(13, OUTPUT);          // для перезапуску МК 12-13

  pinMode(buzer_PIN, OUTPUT);






}

void loop() {



  if (millis() >= 604800000) {
    allReset();
  }

  if (millis() < 3000) {
    reset();
  }

  if (digitalRead(5) == 1) {
    but_lock = 0;
  }

  if (eeprom_read_byte(0) == 1) {
    LIMsw_TYPE = 1;
  }

  if (!digitalRead(5) == 1) {
    OBSTACLE_blink_for_gate();
    open_limit_switch();
    close_limit_switch();
  }

  if (eeprom_read_byte(0) == 1) {
    third_lim = 0;
  } else {
    third_lim = 1;
  }

  if (third_lim == 1) {
    fourth_lim = 1;
  } else {
    fourth_lim = 0;
  }

  if (!millis() < 4000) {
    if (digitalRead(5) == 1) { // нічорта нічого не працюватиме якщо ІЧ датчик помітить перед напрямком руху брами перешкоду

      if (led_flag_open == 1 || led_flag_close == 1) {      // вмикаєм сигнальну лампочку якщо брама рухається в одну із сторін
        blink_for_gate();
      }

      if (led_flag_open == 0 && led_flag_close == 0) {      // вимикаємо сигнальну лампочку якщо брама припинила рух
        ledState = LOW;
        digitalWrite(12, LOW);
        if (!buzer_failed) {
          digitalWrite(buzer_PIN, LOW);
        }
      }


      if (!digitalRead(6) == limSW_type) {                                // провірка щоб дізнатися чи замнений правий кінцевик
        if (!bool_open_limit_switch == true) {
          open_limit_switch();
        }
      }

      if (!digitalRead(7) == limSW_type) {                                // провірка щоб дізнатися чи замнений лівий кінцевик
        if (!bool_close_limit_switch == true) {
          close_limit_switch();
        }
      }

      if (digitalRead(6) == limSW_type) {                                 // тільки коли брама відїде boo_right_limit_switch стане false
        bool_open_limit_switch = false;
      }

      if (digitalRead(7) == limSW_type) {                                 // тільки коли брама відїде boo_left_limit_switch стане false;
        bool_close_limit_switch = false;
      }

      if (millis() - last_press_close_for_go > 1350) {     // ця провірка потрібна для того щоб брама почала рухатись тільки після сплину 1 секунди після завершення руху брами
        OPEN();
      }

      if (millis() - last_press_open_for_go > 1350) {        // ця провірка потрібна для того щоб брама почала рухатись тільки після сплину 1 секунди після завершення руху брами
        CLOSE();
      }
    }


  }

  if (!digitalRead(2)) {
    C_button();
  } else {
    if (C_butt_flug == 1) {
      C_butt_flug = 0;
    }
  }

  if (!digitalRead(3)) {
    D_button();
  } else {
    if (D_butt_flug == 1) {
      D_butt_flug = 0;
    }
  }
  //===============================================
  if (!(led_flag_open == 1 || led_flag_close == 1)) {
    addition_BUTTON_LS();
  }

  if (butt_turn_on) {
    buzer_ALGORITHM();

  }

  if (REPETITIONS == buzer_REPETITIONS) {
    butt_turn_on = 0;
    REPETITIONS = 0;
    buzer_failed = 0;
  }

  if (limSW_type == 0) {
    buzer_REPETITIONS = 2;
  } else {
    buzer_REPETITIONS = 1;
  }
  //======================================


}


void OPEN() {
  if ((!bool_open_limit_switch == true) && millis() - time_close_lm_sw >= pause) {
    if (!digitalRead(9)) { // провіряємо чи не рухається брама в ліву сторону при нажатті кнопки яка відповідає за рух брами в праву строну
      butt_open = !digitalRead(0); // поточне положення кнопки

      if (butt_open == 1 && butt_flag_open == 0 && millis() - last_press_open > buttonPause) {
        butt_flag_open = 1;

        led_flag_open = !led_flag_open;
        digitalWrite(8, led_flag_open);

        last_press_open_for_go = millis();
        last_press_open = millis();
      }
      if (butt_open == 0 && butt_flag_open == 1) {
        butt_flag_open = 0;
      }
    }
  }
}

void CLOSE() {
  if ((!bool_close_limit_switch == true) && millis() - time_open_lm_sw >= pause) {
    if (!digitalRead(8)) { // провіряємо чи не рухається брама в праву сторону при нажатті кнопки яка відповідає за рух брами в ліву строну
      butt_close = !digitalRead(1); // поточне положення кнопки

      if (butt_close == 1 && butt_flag_close == 0 && millis() - last_press_close > buttonPause) {
        butt_flag_close = 1;
        led_flag_close = !led_flag_close;
        digitalWrite(9, led_flag_close);

        last_press_close_for_go = millis();
        last_press_close = millis();
      }
      if (butt_close == 0 && butt_flag_close == 1) {
        butt_flag_close = 0;
      }
    }
  }
}

void open_limit_switch() {
  bool_open_limit_switch = true;
  butt_flag_open = 0;
  butt_open = 0;
  led_flag_open = 0;

  time_open_lm_sw = millis();

  digitalWrite(8, LOW);
}

void close_limit_switch() {
  bool_close_limit_switch = true;
  butt_flag_close = 0;
  butt_close = 0;
  led_flag_close = 0;

  time_close_lm_sw = millis();

  digitalWrite(9, LOW);
}

void blink_for_gate() {
  currentMillis = millis();

  if (currentMillis - previousMillis > interval) {

    previousMillis = currentMillis;
    if (ledState == LOW) {
      ledState = HIGH;
    }
    else {
      ledState = LOW;
    }
    digitalWrite(12, ledState);
    digitalWrite(buzer_PIN, ledState);
  }
}

void OBSTACLE_blink_for_gate() {
  currentMillis = millis();
  but_relay_lock();

  if (millis() - lock >= time_afret) {
    if (currentMillis - previousMillis > 175) {

      previousMillis = currentMillis;
      if (ledState == LOW) {
        ledState = HIGH;
      }
      else {
        ledState = LOW;
      }
      digitalWrite(12, ledState);
      digitalWrite(buzer_PIN, ledState);
    }
  }
}

void but_relay_lock() {
  if (but_lock == 0) {
    but_lock = !but_lock;
    lock = millis();
  }
}

void reset() {
  butt_flag_open = 0;
  butt_open = 0;
  led_flag_open = 0;
  last_press_open = 0;
  digitalWrite(8, LOW);

  //========================

  butt_flag_close = 0;
  butt_close = 0;
  led_flag_close = 0;
  last_press_close = 0;
  digitalWrite(9, LOW);

  //=======================

  bool_open_limit_switch = 0;
  bool_close_limit_switch = 0;

  //=======================

  last_press_open_for_go = 1000;     // потрібна для того щоб після припенення руху брами в одну зі сторін, брама почала рухатись в іншу сторону тільки після проходження 1 секунди
  last_press_close_for_go = 1000;      // потрібна для того щоб після припенення руху брами в одну зі сторін, брама почала рухатись в іншу сторону тільки після проходження 1 секунди

  //=======================

  currentMillis = 0;
  lock = 0;
  but_lock = 0;

  //=======================

  C_butt = 0;                               // для кнопки C на пульту
  C_butt_flug = 0;
  C_signal = 0;
  C_last_butt_press = 0;
  digitalWrite(10, LOW);

  //=======================

  D_butt = 0;                               // для кнопки D на пульту
  D_butt_flug = 0;
  D_signal = 0;
  D_last_butt_press = 0;
  digitalWrite(11, LOW);

  //=======================

  ledState = LOW;             // этой переменной устанавливаем состояние светодиода
  previousMillis = 0;
  digitalWrite(12, LOW);

  //=======================

  time_open_lm_sw = 0;
  time_close_lm_sw = 0;

}

void C_button() {
  C_butt = !digitalRead(2); // поточне положення кнопки

  if (C_butt == 1 && C_butt_flug == 0 && millis() - C_last_butt_press > 500) {
    C_butt_flug = 1;
    C_signal = !C_signal;
    digitalWrite(10, C_signal);

    C_last_butt_press = millis();
  }
}

void D_button() {
  D_butt = !digitalRead(3); // поточне положення кнопки

  if (D_butt == 1 && D_butt_flug == 0 && millis() - D_last_butt_press > 500) {
    D_butt_flug = 1;
    D_signal = !D_signal;
    digitalWrite(11, D_signal);

    D_last_butt_press = millis();
  }
}

void allReset() {
  digitalWrite(13, 1);
}
//======================================================================================================================
void BUTTON_LS() {
  BUTTON = !digitalRead(butt_PIN);

  if (BUTTON == 1 && BUTTON_FLUG == 0 && millis() - LAST_BUTT_PRES > BUTT_DELAY && millis() - butt_noise > 200) {

    butt_noise = millis();

    BUTTON_FLUG = 1;
    BUTT_COUNTER++;
    if (BUTT_COUNTER == butt_CLICK) {

      LIMsw_TYPE = ! LIMsw_TYPE;
      eeprom_write_byte(0, LIMsw_TYPE);

      if (eeprom_read_byte(0) == 1) {
        third_lim = 0;
      } else {
        third_lim = 1;
      }

      butt_turn_on = 1;
      buzer_ALGORITHM();

      BUTT_COUNTER = 0;
    }
    LAST_BUTT_PRES = millis();
  }
}

void addition_BUTTON_LS() {
  if (!digitalRead(butt_PIN)) {
    BUTTON_LS();
  } else {
    if (BUTTON_FLUG == 1) {
      BUTTON_FLUG = 0;
    }
  }

  if (BUTT_COUNTER > 0) {
    if (millis() - LAST_BUTT_PRES > TIME) {
      BUTT_COUNTER = 0;

    }
  }

  //реалізація функції - кнопка нажата протягом n-го часу значить виконуємо якусь дію протягом часу x

  if (!digitalRead(butt_PIN) && !digitalRead(buzer_PIN)) {
    if (PRES) {
      PRES = ! PRES;
      PRES_TIME_COUNTER = millis();

    }

    if (millis() - PRES_TIME_COUNTER > pres_TIME && !butt_already_pres) {
      butt_turn_on = 1;

      buzer_TIME_ON = millis();
      butt_already_pres = 1;
    }

  } else {
    PRES = 1;
    PRES_TIME_COUNTER = 0;
  }

  if (digitalRead(butt_PIN)) {
    butt_already_pres = 0;

  }

  if (digitalRead(buzer_PIN) == 1 && millis() - buzer_TIME_ON > buzer_ON) {
    //digitalWrite(buzer_PIN, 0);
    BUZER_SIGNAL = 0;

  }
}

void buzer_ALGORITHM() {
  if (!digitalRead(buzer_PIN) && millis() - TIME_OF > buzer_DONT_WORK && REPETITIONS != buzer_REPET  ITIONS) {

    buzer_STATUS = ! buzer_STATUS;
    digitalWrite(buzer_PIN, buzer_STATUS);
    buzer_failed = 1;
    TIME_ON = millis();
    REP = ! REP;
  }

  if (millis() - TIME_ON > buzer_WORK && REP) {
    buzer_STATUS = ! buzer_STATUS;
    digitalWrite(buzer_PIN, buzer_STATUS);
    TIME_OF = millis();
    REP = ! REP;у
    REPETITIONS = REPETITIONS + 1;

  }
}
