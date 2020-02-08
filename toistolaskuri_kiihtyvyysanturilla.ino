#include <MsTimer2.h>
// Määritellään kytkentänavat kiihtyvyysanturille:
const int VccPin2 = A0;  // Käyttöjännite
const int xPin   = A1;   // x-kanavan mittaus
const int yPin   = A2;   // y-kanava
const int zPin   = A3;   // z-kanava
const int GNDPin2 = A4;  // laitteen maa-napa

int SisaanTunniste = 0; // Käyttöjännitteen asettamiseen liittyvä tunniste. Reagoidaan kun mennään ohjelmaan
// eka kerran sisään.
int x = 0; //x-kanavan sensoriarvo
int y = 0;
int z = 0;
float ax = 0.0;  // x-kanavan kiihtyvyysarvo SI-muodossa (m/s^2)
float ay = 0.0;
float az = 0.0;
float axyz = 0.0;
float ax_out = 0.0;
float ay_out = 0.0;
float az_out = 0.0;
float axyz_out = 0.0;
float painotilakerroinUusi = 0.02;
float painotilakerroinVanha = 0.98;
float nopeus = 0.0;
float nopeus_out = 0.0;
float nousuTaso = 1.8;
float laskuTaso = 0.3;
int toistot = 0;
bool laskurinTila;

void setup()
{
    Serial.begin(19200);
    // Kiihtvyys-anturi:
    pinMode(VccPin2, OUTPUT);     // Kiihtyvyysanturin käyttöjännite Vcc
    pinMode(GNDPin2, OUTPUT);     // Kiihtyvyysanturin GND
    // Asetetaan syöttöjännite (5V UNO-BOARDILLA, 3.3V Genuino 101:llä) ja maa-arvot (0V):
    digitalWrite(VccPin2, HIGH);
    delayMicroseconds(2);
    digitalWrite(GNDPin2, LOW);
    delayMicroseconds(2);
    MsTimer2::set(10, nopeudenLaskenta);
    MsTimer2::start();

    while (Serial.available() != 0) {} // Odotellaan että yhteys käynnistyy
    // jos tässä sattuu olemaan viivettä. 0 tarkoittaa että yhteys on

}

void loop()
{
    // eka sisäänmenolla annetaan 1ms aikaa käynnistyä. Muuten 1. arvo on pelkkää häiriötä
    if (SisaanTunniste == 0)
    {
        delay(1); // 1ms viive käynnistymiselle
    }
    // Kiihtyvyysmittaus
    x = analogRead(xPin);  // Luetaan x-sensoriarvo, joka tulee 10-bittisellä koodauksella eli lukualueella 0 - 1023. 0 = 0V ja 1023 = Vcc.
    y = analogRead(yPin);
    z = analogRead(zPin);

    ax = 0.1441 * x - 48.945; // Kalibrointiyhtälö x-akselin sensoriarvosta x-suunnan kiihtyvyydeksi.
    ax_out = painotilakerroinUusi * ax + painotilakerroinVanha * ax_out; // 2% uutta 98% vanhaa

    ay = 0.1443 * y - 48.854;
    ay_out = painotilakerroinUusi * ay + painotilakerroinVanha * ay_out;

    az = 0.1442 * z - 49.661;
    az_out = painotilakerroinUusi * az + painotilakerroinVanha * az_out;

    axyz = sqrt(ax * ax + ay * ay + az * az);
    axyz_out = painotilakerroinUusi * axyz + painotilakerroinVanha * axyz_out;

    ax = ax - ax_out;
    ay = ay - ay_out;
    az = az - az_out;
    axyz = axyz - axyz_out;
    nopeudenLaskenta();
    tehtava4();
}

void nopeudenLaskenta()
{
    nopeus = nopeus + axyz * 0.01; // 0.01 == 10 ms näyteväli Mstimer2:lla
    nopeus_out = painotilakerroinUusi * nopeus + painotilakerroinVanha * nopeus_out;
    nopeus = nopeus - nopeus_out;
}
void tehtava4()
{
    if ((laskurinTila == LOW) && (nopeus > nousuTaso))
    {
        toistot++;
        laskurinTila = HIGH;
    }
    if (nopeus < laskuTaso)
    {
        laskurinTila = LOW;
    }
    Serial.print("Toisto laskuri: ");
    Serial.println(toistot);
}
