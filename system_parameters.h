// Définition des distances min et max mesurées par chaque capteur pour chaque cric (en mm)
const int MIN_DISTANCE_1 = 125;
const int MAX_DISTANCE_1 = 345;
const int MIN_DISTANCE_2 = 125;
const int MAX_DISTANCE_2 = 345;
const int MIN_DISTANCE_3 = 125;
const int MAX_DISTANCE_3 = 345;
const int MIN_DISTANCE_4 = 125;
const int MAX_DISTANCE_4 = 345;


// Définition des pins XSHUT des 4 capteurs de distance
const uint8_t XSHUT_1 = A0;
const uint8_t XSHUT_2 = A1;
const uint8_t XSHUT_3 = A2;
const uint8_t XSHUT_4 = A3;

// Définition de l'intervale de temps entre deux mesures de distance (en ms)
const uint16_t MEASUREMENT_INTERVAL = 100;

// Définition des pins des relays pour chaque cric
const int IN_1_CTRL = 8;
const int OUT_1_CTRL = 9;
const int IN_2_CTRL = 7;
const int OUT_2_CTRL = 6;
const int IN_3_CTRL = 5;
const int OUT_3_CTRL = 4;
const int IN_4_CTRL = 2;
const int OUT_4_CTRL = 3;