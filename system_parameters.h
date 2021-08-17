#pragma once

// Définition des intervalles de temps min et max de maintien d'une position (en ms)
const uint32_t MIN_MAINTAIN_TIME = 10000;
const uint32_t MAX_MAINTAIN_TIME = 30000;

// Définition des intervalles de temps min et max entre deux positions (en ms)
const uint32_t MIN_POS_INTERVAL = 20000;
const uint32_t MAX_POS_INTERVAL = 120000;

// Définition des amplitudes min et max des mouvements (entre 0 et 100)
// Si min = max alors tous les mouvements se feront à cette amplitude
const int MIN_AMPLITUDE = 60;
const int MAX_AMPLITUDE = 100; 

// Booléen a passer à false si l'on souhaite enlever les mouvements de translation simple de la boucle automatique
// (ce n'est pas forcément le mouvement le plus intéressant)
const bool ALLOW_SIMPLE_TRANSLATION = true;

// Définition des distances min et max mesurées par chaque capteur pour chaque cric (en mm)

const int MIN_DISTANCE_1 = 109;
const int MAX_DISTANCE_1 = 329;
const int MIN_DISTANCE_2 = 121;
const int MAX_DISTANCE_2 = 341;
const int MIN_DISTANCE_3 = 128;
const int MAX_DISTANCE_3 = 348;
const int MIN_DISTANCE_4 = 108;
const int MAX_DISTANCE_4 = 328;


/*
//Pour les tests
const int MIN_DISTANCE_1 = 100;
const int MAX_DISTANCE_1 = 400;
const int MIN_DISTANCE_2 = 100;
const int MAX_DISTANCE_2 = 400;
const int MIN_DISTANCE_3 = 100;
const int MAX_DISTANCE_3 = 400;
const int MIN_DISTANCE_4 = 100;
const int MAX_DISTANCE_4 = 400;
*/

// Définition de la longueur max (en mm) de descente par rapport à la position milieu (pour les mouvements pré-programmés)
// Cette longueur doit être inférieure à (MAX - MIN) / 2
const int MIN_LOW_1 = 40;
const int MIN_LOW_2 = 40;
const int MIN_LOW_3 = 40;
const int MIN_LOW_4 = 40;

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

// Définition des adresses I2C des capteurs de distance
const uint8_t I2C_ADDRESS_1 = 0x39; // 57
const uint8_t I2C_ADDRESS_2 = 0x49; // 73
const uint8_t I2C_ADDRESS_3 = 0x59; // 89
const uint8_t I2C_ADDRESS_4 = 0x69; // 105