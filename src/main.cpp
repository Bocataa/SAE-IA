/*
  ESP32 - IA Embarquée - GTSRB
  Université Côte d'Azur
  Hugo Meleiro - LEAT
*/

#include <Arduino.h> // Include the Arduino library
#include "model.h" // Include the model header file
#include "trafficsigns.h"
#include "math.h"

#define NBLABELS 28
#define SEUILSOFTMAX 0.85

static input_t inputs;
static output_t outputs;

unsigned long StartTime;
unsigned long CurrentTime;

int cpt = 0;

// Fonction pour convertir le rgb 565 en RGB 8 bits
void rgb565_to_rgb888(unsigned short rgb565, int* red8, int* green8, int* blue8) {
  // Decalage de la valeur rouge a droite de 11 bits.
  unsigned char red5 = rgb565 >> 11;
  // Decalage de la valeur verte a droite de 5 bits et extraction des 6 bits inferieurs.
  unsigned char green6 = (rgb565 >> 5) & 0b111111;
  // Extraction des 5 bits inferieurs.
  unsigned char blue5 = rgb565 & 0b11111;

  // Conversion du rouge 5 bits en rouge 8 bits.
  *red8 = round((float)red5 / 31 * 255);
  // Conversion du vert 6 bits en vert 8 bits.
  *green8 = round((float)green6 / 63 * 255);
  // Conversion du bleu 5 bits en bleu 8 bits.
  *blue8 = round((float)blue5 / 31 * 255);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Ready !");

  // Initialisation des entrées avec des valeurs réelles de trafficsigns
for (int i = 0; i < 32; i++) {   // Hauteur de l'image (ex: 32x32)
  for (int j = 0; j < 32; j++) { // Largeur de l'image
    uint16_t pixel = trafficsign2[i * 32 + j]; // Récupère le pixel en 16 bits trafficsignX remplacer par image souhaitée

    // Extraction des composants RGB
    inputs[i][j][0] = (pixel >> 11) & 0x1F; // Rouge (5 bits)
    inputs[i][j][1] = (pixel >> 5) & 0x3F;  // Vert (6 bits)
    inputs[i][j][2] = pixel & 0x1F;         // Bleu (5 bits)
  }
}
}

void loop() {
  if (cpt == 0) {

    StartTime = micros(); // démarre un chrono pour calculer temps inference

    // Cette fonction réalise l'inférence du modèle
    cnn(inputs, outputs);

    CurrentTime = micros(); // Fin du chrono (temps = currentTime-StartTime)

    printf("Temps d'inférence = %f\n\n", CurrentTime - StartTime);
    // TODO : Ajoutez ici votre code pour calculer le softmax,
      int label = 0;
      float sum = 0;
      float max_val = outputs[0];
      for (int i = 1; i < NBLABELS; i++) {
        sum += exp(outputs[i]/100);
        if (max_val < outputs[i]) {
          max_val = (float)outputs[i];
          label = i;
        }
      }

      //calculate prediction accuracy with softmax
      float softmax[NBLABELS];

      for (int i = 0; i < NBLABELS; i++) {
        softmax[i] = exp(outputs[i]/100) / sum;
      }
      Serial.printf("Confidence : %.2f%%\n\n", softmax[label] * 100); //print the confidence of the prediction


    // Print the label predicted
    if(softmax[label] >= SEUILSOFTMAX) { // Seuil de confiance
      switch(label){
        case 0 : Serial.println("Class Predicted : 20 km/h"); break;
        case 1 : Serial.println("Class Predicted : 30 km/h"); break;
        case 2 : Serial.println("Class Predicted : 50 km/h"); break;
        case 3 : Serial.println("Class Predicted : 60 km/h"); break;
        case 4 : Serial.println("Class Predicted : 70 km/h"); break;
        case 5 : Serial.println("Class Predicted : 80 km/h"); break;
        case 6 : Serial.println("Class Predicted : 100 km/h"); break;
        case 7 : Serial.println("Class Predicted : 120 km/h"); break;
        case 8 : Serial.println("Class Predicted : Wrong way"); break;
        case 9 : Serial.println("Class Predicted : Stop"); break;
        case 10 : Serial.println("Class Predicted : Yield"); break;
        case 11 : Serial.println("Class Predicted : Danger"); break;
        case 12 : Serial.println("Class Predicted : Dangerous left turn"); break;
        case 13 : Serial.println("Class Predicted : Dangerous right turn"); break;
        case 14 : Serial.println("Class Predicted : Winding road"); break;
        case 15 : Serial.println("Class Predicted : Slippery road"); break;
        case 16 : Serial.println("Class Predicted : Crosswalk"); break;
        case 17 : Serial.println("Class Predicted : Bicycles"); break;
        case 18 : Serial.println("Class Predicted : Animals"); break;
        case 19 : Serial.println("Class Predicted : Red light"); break;
        case 20 : Serial.println("Class Predicted : Road bumps"); break;
        case 21 : Serial.println("Class Predicted : Workers ahead"); break;
        case 22 : Serial.println("Class Predicted : Right or Forward"); break;
        case 23 : Serial.println("Class Predicted : Left or Forward"); break;
        case 24 : Serial.println("Class Predicted : Right"); break;
        case 25 : Serial.println("Class Predicted : Left"); break;
        case 26 : Serial.println("Class Predicted : Forward"); break;
        case 27 : Serial.println("Class Predicted : End of game"); break;
        default : Serial.println("Error !"); break;
      }
      //cpt++;
    } else {
      Serial.println("No class detected");
    }
  }
}