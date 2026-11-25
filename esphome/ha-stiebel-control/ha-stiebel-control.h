/*
 *
 *  Copyright (C) 2023 Bastian Stahmer (bastian@stahmer.net)
 *  This program is part of the ESPHome / Home Assistant Program "ha-stiebel-control"
 *  and is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation version 3 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program. If not, see http://www.gnu.org/licenses/ .
 */

#if !defined(ha_stiebel_control_H)
#define ha_stiebel_control_H

// ============================================================================
// INCLUDES
// ============================================================================
#include "ElsterTable.h"
#include "KElsterTable.h"
#include "config.h"
#include "language_select.h"
#include "sg_ready_controller.h"
#include <driver/twai.h>
#include <esp_heap_caps.h>
#include <esp_system.h>
#include <sstream>
#include <iomanip>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <cmath>
// ============================================================================
// CAN BUS MEMBER DEFINITIONS
// ============================================================================

typedef struct
{
    const char *Name;
    uint32_t CanId;
} CanMember;

static const CanMember CanMembers[] =
    {
        //  Name  CanId
        {"KESSEL", 0x180},
        {"ATEZ", 0x280},
        {"BEDIENMODUL_1", 0x300},
        {"BEDIENMODUL_2", 0x301},
        {"BEDIENMODUL_3", 0x302},
        {"BEDIENMODUL_4", 0x303},
        {"RAUMFERNFUEHLER", 0x400},
        {"MANAGER", 0x480},
        {"HEIZMODUL", 0x500},
        {"BUSKOPPLER", 0x580},
        {"MISCHERMODUL_1", 0x600},
        {"MISCHERMODUL_2", 0x601},
        {"MISCHERMODUL_3", 0x602},
        {"MISCHERMODUL_4", 0x603},
        {"PC", HA_CAN_ID_PC}, // from substitution can_id_pc (see config.h)
        {"FREMDGERAET", 0x700},
        {"DCF_MODUL", 0x780},
        {"OTHER", 0x000}};

typedef enum
{
    // Die Reihenfolge muss mit CanMembers übereinstimmen!
    cm_kessel = 0,
    cm_atez,
    cm_bedienmodul_1,
    cm_bedienmodul_2,
    cm_bedienmodul_3,
    cm_bedienmodul_4,
    cm_raumfernfuehler,
    cm_manager,
    cm_heizmodul,
    cm_buskoppler,
    cm_mischermodul_1,
    cm_mischermodul_2,
    cm_mischermodul_3,
    cm_mischermodul_4,
    cm_pc,
    cm_fremdgeraet,
    cm_dcf_modul,
    cm_other
} CanMemberType;

// ============================================================================
// MQTT AUTO-DISCOVERY CONFIGURATION
// ============================================================================

// ============================================================================
// CALCULATED SENSOR DISCOVERY CONFIGURATION
// ============================================================================

struct CalculatedSensorConfig
{
    const char *uniqueId;       // Unique ID for Home Assistant
    const char *name;           // Friendly name
    const char *stateTopic;     // MQTT state topic
    const char *component;      // "sensor" or "binary_sensor"
    const char *deviceClass;    // Device class (empty string if none)
    const char *unit;           // Unit of measurement (empty string if none)
    const char *stateClass;     // State class (empty string if none)
    const char *icon;           // MDI icon
    const char *payloadOn;      // For binary sensors (empty string if not applicable)
    const char *payloadOff;     // For binary sensors (empty string if not applicable)
    const char *entityCategory; // "diagnostic" | "config" | "" (empty = normal)
    bool enabledByDefault;      // false = disabled in HA until user enables
};

static const CalculatedSensorConfig calculatedSensors[] = {
    // Date sensor
    {"stiebel_calculated_date", LNAME_CALC_DATE, "heatingpump/calculated/date/state",
     "sensor", "", "", "", "mdi:calendar", "", "", "", true},

    // Time sensor
    {"stiebel_calculated_time", LNAME_CALC_TIME, "heatingpump/calculated/time/state",
     "sensor", "", "", "", "mdi:clock", "", "", "", true},

    // Betriebsart sensor
    {"stiebel_calculated_betriebsart", LNAME_CALC_BETRIEBSART, "heatingpump/calculated/betriebsart/state",
     "sensor", "", "", "", "mdi:cog", "", "", "", true},

    // Delta T continuous
    {"stiebel_calculated_delta_t_continuous", LNAME_CALC_DELTA_T_CONTINUOUS, "heatingpump/calculated/delta_t_continuous/state",
     "sensor", "temperature", "K", "measurement", "mdi:thermometer", "", "", "", true},

    // Delta T running (only when compressor active)
    {"stiebel_calculated_delta_t_running", LNAME_CALC_DELTA_T_RUNNING, "heatingpump/calculated/delta_t_running/state",
     "sensor", "temperature", "K", "measurement", "mdi:thermometer-chevron-up", "", "", "", true},

    // Compressor active binary sensor
    {"stiebel_calculated_compressor_active", LNAME_CALC_COMPRESSOR_ACTIVE, "heatingpump/calculated/compressor_active/state",
     "binary_sensor", "running", "", "", "mdi:engine", "on", "off", "", true},

    // CAN bus diagnostic sensors (TWAI / ESP32-S3)
    {"stiebel_calculated_can_tec", LNAME_CALC_CAN_TEC, "heatingpump/calculated/can_tec/state",
     "sensor", "", "", "measurement", "mdi:alert-network", "", "", "diagnostic", false},
    {"stiebel_calculated_can_rec", LNAME_CALC_CAN_REC, "heatingpump/calculated/can_rec/state",
     "sensor", "", "", "measurement", "mdi:alert-network-outline", "", "", "diagnostic", false},
    {"stiebel_calculated_can_bus_errors", LNAME_CALC_CAN_BUS_ERRORS, "heatingpump/calculated/can_bus_errors/state",
     "sensor", "", "", "total_increasing", "mdi:network-off", "", "", "diagnostic", false},
    {"stiebel_calculated_can_state", LNAME_CALC_CAN_STATE, "heatingpump/calculated/can_state/state",
     "sensor", "", "", "", "mdi:can", "", "", "diagnostic", false},

    // Heat pump availability (CAN liveness, clock plausibility, answered writes)
    {"stiebel_calculated_heatpump_available", LNAME_CALC_AVAILABLE, "heatingpump/calculated/heatpump_available/state",
     "binary_sensor", "connectivity", "", "", "mdi:heat-pump", "on", "off", "", true},
    {"stiebel_calculated_heatpump_availability_status", LNAME_CALC_AVAILABILITY_STATUS, "heatingpump/calculated/heatpump_availability_status/state",
     "sensor", "", "", "", "mdi:information-outline", "", "", "diagnostic", true},

    // System diagnostics (heap, uptime, automatic CAN restarts)
    {"stiebel_calculated_heap_free", LNAME_CALC_HEAP_FREE, "heatingpump/calculated/heap_free/state",
     "sensor", "data_size", "B", "measurement", "mdi:memory", "", "", "diagnostic", true},
    {"stiebel_calculated_heap_largest_block", LNAME_CALC_HEAP_LARGEST_BLOCK, "heatingpump/calculated/heap_largest_block/state",
     "sensor", "data_size", "B", "measurement", "mdi:memory", "", "", "diagnostic", true},
    {"stiebel_calculated_heap_min_free", LNAME_CALC_HEAP_MIN_FREE, "heatingpump/calculated/heap_min_free/state",
     "sensor", "data_size", "B", "measurement", "mdi:memory", "", "", "diagnostic", true},
    {"stiebel_calculated_uptime", LNAME_CALC_UPTIME, "heatingpump/calculated/uptime/state",
     "sensor", "duration", "h", "total_increasing", "mdi:timer-outline", "", "", "diagnostic", true},
    {"stiebel_calculated_can_restarts", LNAME_CALC_CAN_RESTARTS, "heatingpump/calculated/can_restarts/state",
     "sensor", "", "", "total_increasing", "mdi:restart-alert", "", "", "diagnostic", true},

    // BETRIEBS_STATUS bit field decoded as text (index 17)
    {"stiebel_calculated_betriebsstatus_text", LNAME_CALC_BETRIEBSSTATUS_TEXT, "heatingpump/calculated/betriebsstatus_text/state",
     "sensor", "", "", "", "mdi:heat-pump-outline", "", "", "", true},
};

static const size_t CALCULATED_SENSOR_COUNT = sizeof(calculatedSensors) / sizeof(CalculatedSensorConfig);

// ============================================================================
// WRITABLE NUMBER CONFIGURATION (for MQTT Number entities)
// ============================================================================

struct WritableNumberConfig
{
    const char *signalName;   // CAN signal name like "WW_SOLL_TAG"
    const char *friendlyName; // Display name in Home Assistant
    CanMemberType member;     // Which CAN member to write to
    float min;                // Minimum value
    float max;                // Maximum value
    float step;               // Step size
    const char *unit;         // Unit of measurement
    const char *icon;         // MDI icon
    const char *deviceClass;  // HA device class
};

static const WritableNumberConfig writableNumbers[] = {
    // Primary storage target temperature
    {"WW_SOLL_TAG", LNAME_NUM_SPEICHERSOLLTEMP,
     cm_kessel, 0.0, 65.0, 0.5, "°C", "mdi:thermometer-high", "temperature"},

    // SG Ready boost temperatures (not real CAN signals, handled internally)
    {"SG_READY_BOOST_STATE3", LNAME_NUM_SG_READY_BOOST3,
     cm_kessel, 0.0, 10.0, 0.5, "°C", "mdi:thermometer-plus", "temperature"},

    {"SG_READY_BOOST_STATE4", LNAME_NUM_SG_READY_BOOST4,
     cm_kessel, 0.0, 15.0, 0.5, "°C", "mdi:thermometer-chevron-up", "temperature"},

    // Room temperature setpoints for heating circuits 1–3 and night/reduced mode
    {"RAUMSOLLTEMP_I", "Raum Soll Temperatur Tag HK1",
     cm_bedienmodul_2, 10.0, 30.0, 0.5, "°C", "mdi:home-thermometer", "temperature"},

    {"RAUMSOLLTEMP_NACHT", "Raumtemperatur Nacht HK1",
     cm_bedienmodul_2, 10.0, 30.0, 0.5, "°C", "mdi:home-thermometer-outline", "temperature"},

    {"LUEFT_STUFE_TAG", "Lüftung Stufe Tag", cm_kessel, 0, 3, 1, "", "mdi:fan", ""},
    {"TEMPORALE_LUEFTUNGSSTUFE_STUFE", "Temp. Lüftung Stufe", cm_kessel, 0, 3, 1, "", "mdi:fan", ""},
    {"TEMPORALE_LUEFTUNGSSTUFE_DAUER", "Temp. Lüftung Dauer", cm_kessel, 0, 1000, 1, "min", "mdi:fan", ""},
    {"ABWESENHEITSTEMP", "Raumtemperatur Bereitschaft HK1", cm_bedienmodul_2, 10.0, 30.0, 0.5, "°C", "mdi:home-thermometer-outline", "temperature"},
    {"HEIZKREISSOLL_HAND_HK1", "Heizkreissoll Hand HK1", cm_bedienmodul_2, 10.0, 65.0, 0.5, "°C", "mdi:home-thermometer", "temperature"},
    {"LUEFT_STUFE_NACHT", "Lüftung Stufe Nacht", cm_kessel, 0, 3, 1, "", "mdi:fan", ""},
    {"LUEFT_STUFE_BEREITSCHAFT", "Lüftung Stufe Bereitschaft", cm_kessel, 0.0, 3.0, 1.0, "", "mdi:fan", ""},
    {"LUEFTUNG_STUFE_PARTY", "Lüftung Stufe Party", cm_kessel, 0.0, 3.0, 1.0, "", "mdi:fan", ""},
    {"LUEFTUNG_STUFE_HAND", "Lüftung Stufe Hand", cm_kessel, 0.0, 3.0, 1.0, "", "mdi:fan", ""},
    {"LUEFTUNGSZEIT_AUSSERPL_ST_0", "Lüftungszeit Außerpl. St. 0", cm_kessel, 0.0, 1000.0, 1.0, "min", "mdi:fan", ""},
    {"LUEFTUNGSZEIT_AUSSERPL_ST_1", "Lüftungszeit Außerpl. St. 1", cm_kessel, 0.0, 1000.0, 1.0, "min", "mdi:fan", ""},
    {"LUEFTUNGSZEIT_AUSSERPL_ST_2", "Lüftungszeit Außerpl. St. 2", cm_kessel, 0.0, 1000.0, 1.0, "min", "mdi:fan", ""},
    {"LUEFTUNGSZEIT_AUSSERPL_ST_3", "Lüftungszeit Außerpl. St. 3", cm_kessel, 0.0, 1000.0, 1.0, "min", "mdi:fan", ""},
    {"LUEFTERSTUFE_ZULUFT_1", "Lüfterstufe Zuluft 1", cm_kessel, 10.0, 80.0, 1.0, "m³/h", "mdi:fan", ""},
    {"LUEFTERSTUFE_ZULUFT_2", "Lüfterstufe Zuluft 2", cm_kessel, 80.0, 180.0, 1.0, "m³/h", "mdi:fan", ""},
    {"LUEFTERSTUFE_ZULUFT_3", "Lüfterstufe Zuluft 3", cm_kessel, 180.0, 300.0, 1.0, "m³/h", "mdi:fan", ""},
    {"LUEFTERSTUFE_ABLUFT_1", "Lüfterstufe Abluft 1", cm_kessel, 10.0, 80.0, 1.0, "m³/h", "mdi:fan", ""},
    {"LUEFTERSTUFE_ABLUFT_2", "Lüfterstufe Abluft 2", cm_kessel, 80.0, 180.0, 1.0, "m³/h", "mdi:fan", ""},
    {"LUEFTERSTUFE_ABLUFT_3", "Lüfterstufe Abluft 3", cm_kessel, 180.0, 300.0, 1.0, "m³/h", "mdi:fan", ""},
    {"HEIZKURVE", "Steigung HK1", cm_bedienmodul_2, 0.0, 5.0, 0.01, "", "", ""},
    {"RAUMEINFLUSS", "Raumeinfluss HK1", cm_bedienmodul_2, 0.0, 100.0, 1.0, "", "", ""},
    {"FUSSPUNKT_HK1", "Fusspunkt HK1", cm_bedienmodul_2, 0.0, 20.0, 0.1, "°C", "", "temperature"},
    {"ANTEIL_VORLAUF_HK1", "Anteil Vorlauf HK1", cm_bedienmodul_2, 0.0, 100.0, 1.0, "%", "", ""},
    {"MAX_TEMP_KESSEL", "Sollwert Max. HK1", cm_bedienmodul_2, 10.0, 65.0, 1.0, "°C", "", "temperature"},
    {"MIN_TEMP_HZK", "Sollwert Min. HK1", cm_bedienmodul_2, 0.0, 40.0, 1.0, "°C", "", "temperature"},
    {"WW_SOLL_NACHT", "WW Soll Nacht", cm_kessel, 0.0, 65.0, 0.5, "°C", "", "temperature"},
    {"WW_SOLL_BEREITSCHAFT", "WW Soll Bereitschaft", cm_kessel, 0.0, 65.0, 0.5, "°C", "", "temperature"},
    {"WW_SOLL_HANDBETRIEB", "WW Soll Handbetrieb", cm_kessel, 0.0, 65.0, 0.5, "°C", "", "temperature"},
    {"WW_HYSTERSE", "WW Hysterese", cm_kessel, 2.0, 10.0, 1.0, "K", "", ""},
    {"WW_ZEITSPERREN_NOTHEIZUNG", "WW Zeitsperren Notheizung", cm_kessel, 0.0, 360.0, 1.0, "min", "", ""},
    {"TEMPERATUR_FREIGABE_NOTHEIZUNG", "Temperatur Freigabe Notheizung", cm_kessel, -20.0, 10.0, 1.0, "°C", "", "temperature"},
    {"ANTILEGIONELLEN_INTERVALL", "Antilegionellen", cm_kessel, 1.0, 30.0, 1.0, "d", "", ""},
    {"MAX_WW_LADEZEIT", "Max. Dauer WW-Erzeugung", cm_kessel, 6.0, 12.0, 1.0, "h", "", ""},
    {"WW_TEMPERATUR_LEGIONELLEN", "WW Temperatur Legionellen", cm_kessel, 10.0, 65.0, 1.0, "°C", "", "temperature"},
    {"NOTHEIZUNG_STUFE_WW", "Notheizung Stufe WW", cm_kessel, 0.0, 3.0, 1.0, "", "", ""},
    {"MAX_VORLAUFTEMPERATUR_WW", "Max. Vorlauftemperatur WW", cm_kessel, 10.0, 75.0, 1.0, "°C", "", "temperature"},
    {"WW_LEISTUNG_SOMMER", "WW Leistung Sommer", cm_kessel, 30.0, 100.0, 1.0, "%", "", ""},
    {"WW_LEISTUNG_WINTER", "WW Leistung Winter", cm_kessel, 30.0, 100.0, 1.0, "%", "", ""},
    {"SOMMERZEIT_MANUELL_BEGINN_TAG", "Sommerzeit Manuell Beginn (Tag)", cm_kessel, 1.0, 31.0, 1.0, "", "", ""},
    {"SOMMERZEIT_MANUELL_ENDE_TAG", "Sommerzeit Manuell Ende (Tag)", cm_kessel, 1.0, 31.0, 1.0, "", "", ""},
    {"VERDICHTER_TAKTUNG", "Verdichter Taktung", cm_kessel, 0.0, 20.0, 1.0, "min", "", ""},
    {"DREHZAHL_FORTLUEFTUNG", "Drehzahl Fortlüftung", cm_kessel, 10.0, 100.0, 1.0, "%", "", ""},
    {"ANLAUFSTROMBEGRENZUNG", "Anlaufstrombegrenzung", cm_kessel, 5.0, 30.0, 1.0, "A", "", ""},
    {"PUMPENDREHZAHL_WW", "Pumpendrehzahl WW", cm_kessel, 40.0, 100.0, 1.0, "%", "", ""},
    {"PUMPENDREHZAHL_HEIZEN", "Pumpendrehzahl Heizen", cm_kessel, 40.0, 100.0, 1.0, "%", "", ""},
    {"VERHAELTNIS_MISCHER_HEIZEN", "Verhältniss Mischer-Heizen", cm_kessel, 0.0, 100.0, 1.0, "%", "", ""},
    {"MISCHER_P_TYP", "Mischer-P Typ", cm_kessel, 0.0, 2.0, 1.0, "", "", ""},
    {"SOLAR_P_TYP", "Solar-P Typ", cm_kessel, 0.0, 2.0, 1.0, "", "", ""},
    {"MIN_PUMPZYKLEN", "Min. Pumpzyklen", cm_kessel, 1.0, 100.0, 1.0, "/d", "", ""},
    {"MAX_PUMPZYKLEN", "Max. Pumpzyklen", cm_kessel, 101.0, 288.0, 1.0, "/d", "", ""},
    {"AUSSENTEMPERATUR_MIN_PUMPZYKLEN", "Außentemperatur Min. Pumpzyklen", cm_kessel, 0.0, 25.0, 1.0, "°C", "", "temperature"},
    {"AUSSENTEMPERATUR_MAX_PUMPZYKLEN", "Außentemperatur Max. Pumpzyklen", cm_kessel, 0.0, 20.0, 1.0, "°C", "", "temperature"},
    {"EVU", "EVU-Sperre", cm_kessel, 0.0, 5.0, 1.0, "", "", ""},
    {"DIFFERENZDRUCK_SOLL", "Differenzdruck Sollwert", cm_kessel, 1.0, 100.0, 1.0, "Pa", "", ""},
    {"I_FAKTOR_LUEFTER", "I-Faktor", cm_kessel, 1.0, 100.0, 0.1, "Pa/m²", "", ""},
    {"P_FAKTOR_LUEFTER", "P-Faktor", cm_kessel, 1.0, 100.0, 0.1, "Pa/m²", "", ""},
    {"PROPORTIONALANTEIL_HEIZEN", "Heizen Proportionaler Anteil", cm_kessel, 0.0, 10.0, 1.0, "%/K", "", ""},
    {"INTEGRALANTEIL_INVERTER", "Heizen Integralanteil Inverter", cm_kessel, 0.0, 500.0, 1.0, "Kmin", "", ""},
    {"HEIZEN_MAXIMALE_NE_STUFE", "Heizen Maximale NE-Stufe", cm_kessel, 0.0, 3.0, 1.0, "", "", ""},
    {"HEIZEN_MAXIMALE_VORLAUFTEMPERATUR", "Heizen Maximale Vorlauftemperatur", cm_kessel, 10.0, 75.0, 1.0, "°C", "", "temperature"},
    {"HEIZGRENZE_TAG", "Heizen Sommerbetrieb", cm_kessel, 10.0, 25.0, 1.0, "°C", "", "temperature"},
    {"SOMMER_HYSTERESE", "Hysterese Sommerbetrieb", cm_kessel, 1.0, 7.0, 1.0, "K", "", ""},
    {"AUSSENTEMPVERZOEGERUNG", "Dämpfung Außentemperatur", cm_kessel, 0.0, 300.0, 1.0, "h", "", ""},
    {"BIVALENTPARALLELTEMPERATUR_HZG", "Bivalenzpunkt", cm_kessel, -20.0, 10.0, 0.5, "°C", "", "temperature"},
    {"ZEITSPERRE_NOTHEIZUNG", "Zeitsperre Notheizung", cm_kessel, 0.0, 60.0, 1.0, "min", "", ""},
    {"KORREKTUR_AUSSENTEMPERATUR", "Korrektur Außentemperatur", cm_kessel, -20.0, 30.0, 0.1, "°C", "", "temperature"},
    {"WE_STB_TESTFUNKTION_16", "Unterdrückung Temperatur Messung", cm_kessel, 0.0, 120.0, 1.0, "s", "", ""},
    {"AUSLEGUNGSTEMPERATUR_HEIZEN", "Auslegungstemperatur Heizen", cm_kessel, -25.0, 5.0, 1.0, "°C", "", "temperature"},
    {"LEISTUNG_AUSLEGUNG_HEIZEN", "Leistung Auslegung Heizen", cm_kessel, 40.0, 100.0, 1.0, "%", "", ""},
    // Solar / Kühlen (tmp/StiebelEltron.json)
    {"KOLLEKTORGRENZTEMPERATUR", "Kollektorgrenztemperatur", cm_kessel, 50.0, 80.0, 1.0, "°C", "", "temperature"},
    {"KOLLEKTORSPERRTEMPERATUR", "Kollektorsperrtemperatur", cm_kessel, 130.0, 200.0, 1.0, "°C", "", "temperature"},
    {"KOLLEKTORSCHUTZTEMPERATUR", "Kollektorschutztemperatur", cm_kessel, 100.0, 150.0, 1.0, "°C", "", "temperature"},
    {"VERZOEGERUNG_VERDICHTER_WARMWASSER", "Verzögerung Verdichter Warmwasser", cm_kessel, 0.0, 500.0, 1.0, "min", "", ""},
    {"HYSTERESE_SOLAR", "Hysterese Solar", cm_kessel, 0.6, 10.0, 0.1, "K", "", ""},
    {"SOLAR_WARMWASSER_TEMPERATUR", "Solar Warmwasser Temperatur", cm_kessel, 10.0, 75.0, 0.5, "°C", "", "temperature"},
    {"LEISTUNG_KUEHLEN", "Leistung Kühlen", cm_kessel, 30.0, 50.0, 1.0, "%", "", ""},
    {"FERIENANFANG_TAG", "Ferien Beginn Tag", cm_kessel, 1.0, 31.0, 1.0, "", "", ""},
    {"FERIENANFANG_MONAT", "Ferien Beginn Monat", cm_kessel, 1.0, 12.0, 1.0, "", "", ""},
    {"FERIENANFANG_JAHR", "Ferien Beginn Jahr", cm_kessel, 0.0, 99.0, 1.0, "", "", ""},
    {"FERIENENDE_TAG", "Ferien Ende Tag", cm_kessel, 1.0, 31.0, 1.0, "", "", ""},
    {"FERIENENDE_MONAT", "Ferien Ende Monat", cm_kessel, 1.0, 12.0, 1.0, "", "", ""},
    {"FERIENENDE_JAHR", "Ferien Ende Jahr", cm_kessel, 0.0, 99.0, 1.0, "", "", ""},
    {"MAX_VORVERLEGUNG", "Programm Max. Vorverlegung", cm_kessel, 0.0, 300.0, 1.0, "min", "", ""},
    {"FORTLUFT_SOLL", "Fortluft Soll", cm_kessel, 0.0, 100.0, 1.0, "%", "", ""},
    {"LUEFTUNG_FEUCHTE_SCHWELLWERT", "Lüftung Feuchte Schwellwert", cm_kessel, 30.0, 70.0, 1.0, "%", "", ""},
    {"FEUCHTE_HYSTERESE", "Lüftung Feuchte Hysterese", cm_kessel, 2.0, 10.0, 1.0, "%", "", ""},
    {"LUEFTUNG_FEUCHTE_MASKIERZEIT", "Lüftung Feuchte Maskierzeit", cm_kessel, 1.0, 10.0, 1.0, "min", "", ""},
    {"MIN_SOLL_FEUCHTE", "Lüftung Feuchtesoll Minimum", cm_kessel, 30.0, 60.0, 1.0, "%", "", ""},
    {"LUEFTUNG_LEISTUNGSREDUKTION", "Lüftung Leistungsreduktion", cm_kessel, 0.0, 50.0, 1.0, "%", "", ""},
    {"MAX_SOLL_FEUCHTE", "Lüftung Fechtesoll Maximum", cm_kessel, 60.0, 90.0, 1.0, "%", "", ""},
    {"LUEFTUNG_LEISTUNGSERHOEHUNG", "Lüftung Leistungserhöhung", cm_kessel, 0.0, 100.0, 1.0, "%", "", ""},
    {"MAXIMALE_ABTAUDAUER", "Maximale Abtaudauer", cm_kessel, 60.0, 250.0, 1.0, "min", "", ""},
    {"ABTAUBEGINNSCHWELLE", "Abtaubeginnschwelle", cm_kessel, 0.0, 50.0, 1.0, "%", "", ""},
    {"DREHZAHLFILTER", "Drehzahlfilter", cm_kessel, 0.0, 100.0, 1.0, "%", "", ""},
    {"FILTERWECHSEL_VOLUMEN", "Filterwechsel-Volumen", cm_kessel, 0.0, 1000.0, 1.0, "1000m³", "", ""},
    {"ESTRICH_SOCKELTEMPERATUR", "Trockenheiz. Min.", cm_kessel, 20.0, 40.0, 1.0, "°C", "", "temperature"},
    {"ESTRICH_MAX_TEMPERATUR", "Trockenheiz. Max.", cm_kessel, 25.0, 55.0, 1.0, "°C", "", "temperature"},
    {"ESTRICH_HALTEN_SOCKELTEMPERATUR", "Trockenheiz. Dauer Min. Temp.", cm_kessel, 0.0, 30.0, 1.0, "Tage", "", ""},
    {"ESTRICH_HALTEN_MAX_TEMPERATUR", "Trockenheiz. Dauer Max. Temp.", cm_kessel, 0.0, 30.0, 1.0, "Tage", "", ""},
    {"ESTRICH_STEIGUNG_PRO_TAG", "Trockenheiz. Steigung", cm_kessel, 1.0, 10.0, 1.0, "K/d", "", ""},
    {"RAUMFUEHLERKORREKTUR", "Korrektur Raumtemperatur Bedienteil", cm_kessel, -5.0, 5.0, 0.1, "°C", "", "temperature"},
    {"TEMPERATUR_ABTAUENDE", "Temperatur Abtauende", cm_kessel, 0.0, 30.0, 1.0, "°C", "", "temperature"},
    {"RELATIVE_ABTAUDAUER2", "Maximale Abtaudauer", cm_kessel, 2.0, 60.0, 1.0, "min", "", ""},
    {"EINFRIERSCHUTZ_NOTHEIZUNG", "Einfrierschutz Notheizung", cm_kessel, 10.0, 30.0, 1.0, "min", "", ""},
    {"ABTAUABBRUCH", "Abtauabbruch", cm_kessel, 0.0, 20.0, 1.0, "min", "", ""}

};

static const size_t WRITABLE_NUMBER_COUNT = sizeof(writableNumbers) / sizeof(WritableNumberConfig);

// ============================================================================
// WRITABLE SELECT CONFIGURATION (for MQTT Select entities)
// ============================================================================

struct WritableSelectConfig
{
    const char *signalName;   // CAN signal name like "PROGRAMMSCHALTER"
    const char *friendlyName; // Display name in Home Assistant
    CanMemberType member;     // Which CAN member to write to
    const char **options;     // Array of option strings
    size_t optionCount;       // Number of options
    const char *icon;         // MDI icon
    const char **values;      // CAN value per option (same order); nullptr = option text is written as-is; "" = option is display-only
};

// Operating mode options for PROGRAMMSCHALTER
static const char *programmschalterOptions[] = {
    LNAME_OPT_NOTBETRIEB,
    LNAME_OPT_BEREITSCHAFT,
    LNAME_OPT_AUTOMATIK,
    LNAME_OPT_TAGBETRIEB,
    LNAME_OPT_ABSENKBETRIEB,
    LNAME_OPT_WARMWASSER,
    LNAME_OPT_HANDBETRIEB};

// SG Ready state options (Smart Grid Ready)
static const char *sgReadyOptions[] = {
    LNAME_OPT_SG_EVU_SPERRE,
    LNAME_OPT_SG_NORMAL,
    LNAME_OPT_SG_EMPFOHLEN,
    LNAME_OPT_SG_ZWANG,
};

// Selects whose CAN value is a plain number: option text <-> CAN value
static const char *positionVentilOptions[] = {"Heizkreis", "Heizkreis, Verdichter aktiv", "Warmwasser", "Nicht definiert"};
static const char *positionVentilValues[] = {"2", "1", "0", ""};

static const char *monthOptions[] = {"Januar", "Februar", "März", "April", "Mai", "Juni",
                                     "Juli", "August", "September", "Oktober", "November", "Dezember"};
static const char *monthValues[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12"};

static const char *lueftungRegelungOptions[] = {"Volumenstrom", "Differenzdruck"};
static const char *lueftungRegelungValues[] = {"0", "1"};

static const char *passivkuehlungOptions[] = {"Aus", "Ablüften", "Zulüften", "Bypass", "Sommerkassette"};
static const char *passivkuehlungValues[] = {"0", "1", "2", "3", "4"};

static const char *luftenOfenOptions[] = {"Aus", "Schließer-Aus", "Öffner-Aus", "Schließer-Überwach.", "Öffner-Überwach."};
static const char *luftenOfenValues[] = {"0", "1", "2", "3", "4"};
static const char *vorrangSolarOptions[] = {"Solar", "Kühlen", "Kühlzeit"};
static const char *vorrangSolarValues[] = {"0", "1", "2"};
static const WritableSelectConfig writableSelects[] = {
    // Heat pump operating mode
    {"PROGRAMMSCHALTER", LNAME_SEL_PROGRAMMSCHALTER,
     cm_kessel, programmschalterOptions, 7, "mdi:dip-switch", nullptr},
    {"SG_READY_STATE", LNAME_SEL_SG_READY,
     cm_kessel, sgReadyOptions, 4, "mdi:solar-power", nullptr},
    {"POSITION_VENTIL", "Position Ventil", cm_kessel, positionVentilOptions, 4, "", positionVentilValues},
    {"SOMMERZEIT_MANUELL_BEGINN_MONAT", "Sommerzeit Manuell Beginn (Monat)", cm_kessel, monthOptions, 12, "", monthValues},
    {"SOMMERZEIT_MANUELL_ENDE_MONAT", "Sommerzeit Manuell Ende (Monat)", cm_kessel, monthOptions, 12, "", monthValues},
    {"LUEFTERREGELUNG", "Lüfterregelung", cm_kessel, lueftungRegelungOptions, 2, "mdi:fan", lueftungRegelungValues},
    {"PASSIVKUEHLUNG", "Passivkühlung", cm_kessel, passivkuehlungOptions, 5, "", passivkuehlungValues},
    {"LUEFTEN", "Lüften", cm_kessel, luftenOfenOptions, 5, "mdi:fan", luftenOfenValues},
    {"VORRANG_SOLAR", "Vorrang Solar", cm_kessel, vorrangSolarOptions, 3, "mdi:solar-power", vorrangSolarValues}};

static const size_t WRITABLE_SELECT_COUNT = sizeof(writableSelects) / sizeof(WritableSelectConfig);

// ============================================================================
// WRITABLE SWITCH / BUTTON / TEXT CONFIGURATION (MQTT Switch, Button and Text entities)
// ============================================================================

struct WritableSwitchConfig
{
    const char *signalName;   // CAN signal name (also used for the state topic)
    const char *entityId;     // Entity id for command topic / unique id; nullptr = signalName
    const char *friendlyName; // Display name in Home Assistant
    CanMemberType member;     // Which CAN member to write to
    const char *payloadOn;    // Payload HA sends for ON (= string written to the CAN signal)
    const char *payloadOff;   // Payload HA sends for OFF (= string written to the CAN signal)
    const char *stateOn;      // State payload meaning ON
    const char *stateOff;     // State payload meaning OFF
    const char *icon;         // MDI icon
};

struct WritableButtonConfig
{
    const char *signalName;    // CAN signal name
    const char *friendlyName;  // Display name in Home Assistant
    CanMemberType member;      // Which CAN member to write to
    const char *payload;       // Payload HA sends on press (= string written to the CAN signal)
    const char *icon;          // MDI icon
    const char *deviceClass;   // HA device class (empty = none)
    bool enabledByDefault;     // false = disabled in HA until enabled by the user
};

struct WritableTextConfig
{
    const char *signalName;   // CAN signal name
    const char *friendlyName; // Display name in Home Assistant
    CanMemberType member;     // Which CAN member to write to
    int minLength;            // Minimum text length
    int maxLength;            // Maximum text length
    const char *pattern;      // Regular expression the text must match
    const char *icon;         // MDI icon
};

static const WritableSwitchConfig writableSwitches[] = {
    // Solar
    {"KOLLEKTORSCHUTZ", nullptr, "Kollektorschutz", cm_kessel, "on", "off", "on", "off", "mdi:solar-power"},
    {"FREIGABE_SOLAR", nullptr, "Freigabe Solar", cm_kessel, "on", "off", "on", "off", "mdi:solar-power"},
    // Manual switches
    {"STELLVENTIL_HEIZUNG_MANUELL", nullptr, "Stellventil Heizung - Manuell", cm_kessel, "on", "off", "on", "off", ""},
    {"STELLVENTIL_WARMWASSER_MANUELL", nullptr, "Stellventil Warmwasser - Manuell", cm_kessel, "on", "off", "on", "off", ""},
    {"MISCHERPUMPE_MANUELL", nullptr, "Mischerpumpe - Manuell", cm_kessel, "on", "off", "on", "off", ""},
    {"MISCHER_AUF_MANUELL", nullptr, "Mischer auf - Manuell", cm_kessel, "1", "0", "1", "0", ""},
    {"MISCHER_ZU_MANUELL", nullptr, "Mischer zu - Manuell", cm_kessel, "1", "0", "1", "0", ""},
    // Emergency heater stages share one CAN signal (bit 1/2/4)
    {"NOTHEIZUNG_STUFE_1_MANUELL", "NOTHEIZUNG_STUFE_1_MANUELL", "Notheizung Stufe 1 - Manuell", cm_kessel, "1", "0", "1", "0", ""},
    {"NOTHEIZUNG_STUFE_1_MANUELL", "NOTHEIZUNG_STUFE_2_MANUELL", "Notheizung Stufe 2 - Manuell", cm_kessel, "2", "0", "2", "0", ""},
    {"NOTHEIZUNG_STUFE_1_MANUELL", "NOTHEIZUNG_STUFE_3_MANUELL", "Notheizung Stufe 3 - Manuell", cm_kessel, "4", "0", "4", "0", ""},
    {"UMSCHALTVENTIL_MANUELL", nullptr, "Umschaltventil - Manuell", cm_kessel, "on", "off", "on", "off", ""},
    {"VERDICHTER_MANUELL", nullptr, "Verdichter - Manuell", cm_kessel, "on", "off", "on", "off", ""},
    {"DREHZAHL_ABLUEFTER_MANUELL", nullptr, "Drehzahl Ablüfter - Manuell", cm_kessel, "on", "off", "on", "off", "mdi:fan"},
    {"DREHZAHL_ZULUEFTER_MANUELL", nullptr, "Drehzahl Zulüfter - Manuell", cm_kessel, "on", "off", "on", "off", "mdi:fan"},
    {"FENSTER_AUF_MANUELL", nullptr, "Fenster auf - Manuell", cm_kessel, "on", "off", "on", "off", ""},
    // Settings
    {"WW_PUFFERBETRIEB", nullptr, "WW Pufferbetrieb", cm_kessel, "on", "off", "on", "off", ""},
    {"WW_ECO_BETRIEB", nullptr, "WW Eco", cm_kessel, "on", "off", "on", "off", ""},
    {"INTEGRALSENSOR", nullptr, "Integralsensor", cm_kessel, "1", "0", "1", "0", ""},
    {"ZWEITER_WW_SPEICHER", nullptr, "2. WW-Speicher", cm_kessel, "on", "off", "on", "off", ""},
    {"SOMMERZEIT_AUTOMATIK", nullptr, "Sommerzeit Automatik", cm_kessel, "on", "off", "on", "off", ""},
    {"NOTBETRIEB_AUTO", nullptr, "Notbetrieb Auto", cm_kessel, "on", "off", "on", "off", ""},
    {"PASSIVKUEHLUNG_FORTLUFT", nullptr, "Passivkühlung Fortluft", cm_kessel, "on", "off", "on", "off", "mdi:fan"},
    {"LUFTSTOP_SOMMER_WW", nullptr, "Luftstop Sommer Warmwasser", cm_kessel, "on", "off", "on", "off", "mdi:fan"},
    {"FEUCHTEREGELUNG_EIN_AUS", nullptr, "Feuchteschutz", cm_kessel, "on", "off", "on", "off", "mdi:water-percent"},
    {"LUEFTUNG_FREIGABE", nullptr, "Lüftung einschalten", cm_kessel, "on", "off", "on", "off", "mdi:fan"},
    {"ESTRICHFUNKTION", nullptr, "Trockenheizprogramm einschalten", cm_kessel, "on", "off", "on", "off", ""}};

static const size_t WRITABLE_SWITCH_COUNT = sizeof(writableSwitches) / sizeof(WritableSwitchConfig);

// Buttons write one fixed value. Reset and factory default are disabled in HA by default.
static const WritableButtonConfig writableButtons[] = {
    {"LUEFTER_FILTER_RESET", "Lüfter Filter Reset", cm_kessel, "on", "mdi:air-filter", "restart", true},
    {"FEHLERSPEICHER_LOESCHEN", "Fehlerspeicher löschen", cm_kessel, "on", "mdi:alert-remove", "", true},
    {"RESET_REGLER", "Reset Regler", cm_kessel, "on", "mdi:restart-alert", "restart", false},
    {"WERKSEINSTELLUNG", "Werkseinstellung", cm_kessel, "on", "mdi:factory", "", false}};

static const size_t WRITABLE_BUTTON_COUNT = sizeof(writableButtons) / sizeof(WritableButtonConfig);

// Switching programs (time ranges, quarter-hour resolution) and single times.
// A range is written as "HH:MM-HH:MM"; "---" disables the range.
#define TEXT_PATTERN_TIME_RANGE "^(([01][0-9]|2[0-3]):(00|15|30|45)|24:00)-(([01][0-9]|2[0-3]):(00|15|30|45)|24:00)$|^---$"
#define TEXT_PATTERN_TIME "^(([01][0-9]|2[0-3]):(00|15|30|45)|24:00)$"

static const WritableTextConfig writableTexts[] = {
    {"ANTILEGIONELLEN_ZEIT", "Antilegionellen Zeit", cm_kessel, 5, 5, TEXT_PATTERN_TIME, "mdi:clock-outline"},
    {"HEIZPROG_1_MO_SO", "Heizprogramm HK1 Mo-So P1", cm_bedienmodul_2, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"HEIZPROG_1_MO_SO_SCHALT_2", "Heizprogramm HK1 Mo-So P2", cm_bedienmodul_2, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"HEIZPROG_1_MO_SO_SCHALT_3", "Heizprogramm HK1 Mo-So P3", cm_bedienmodul_2, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"HEIZPROG_1_MO", "Heizprogramm HK1 Montag P1", cm_bedienmodul_2, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"HEIZPROG_1_MO_SCHALT_2", "Heizprogramm HK1 Montag P2", cm_bedienmodul_2, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"HEIZPROG_1_MO_SCHALT_3", "Heizprogramm HK1 Montag P3", cm_bedienmodul_2, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"HEIZPROG_1_DI", "Heizprogramm HK1 Dienstag P1", cm_bedienmodul_2, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"HEIZPROG_1_DI_SCHALT_2", "Heizprogramm HK1 Dienstag P2", cm_bedienmodul_2, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"HEIZPROG_1_DI_SCHALT_3", "Heizprogramm HK1 Dienstag P3", cm_bedienmodul_2, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"HEIZPROG_1_MI", "Heizprogramm HK1 Mittwoch P1", cm_bedienmodul_2, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"HEIZPROG_1_MI_SCHALT_2", "Heizprogramm HK1 Mittwoch P2", cm_bedienmodul_2, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"HEIZPROG_1_MI_SCHALT_3", "Heizprogramm HK1 Mittwoch P3", cm_bedienmodul_2, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"HEIZPROG_1_DO", "Heizprogramm HK1 Donnerstag P1", cm_bedienmodul_2, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"HEIZPROG_1_DO_SCHALT_2", "Heizprogramm HK1 Donnerstag P2", cm_bedienmodul_2, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"HEIZPROG_1_DO_SCHALT_3", "Heizprogramm HK1 Donnerstag P3", cm_bedienmodul_2, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"HEIZPROG_1_FR", "Heizprogramm HK1 Freitag P1", cm_bedienmodul_2, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"HEIZPROG_1_FR_SCHALT_2", "Heizprogramm HK1 Freitag P2", cm_bedienmodul_2, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"HEIZPROG_1_FR_SCHALT_3", "Heizprogramm HK1 Freitag P3", cm_bedienmodul_2, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"HEIZPROG_1_SA", "Heizprogramm HK1 Samstag P1", cm_bedienmodul_2, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"HEIZPROG_1_SA_SCHALT_2", "Heizprogramm HK1 Samstag P2", cm_bedienmodul_2, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"HEIZPROG_1_SA_SCHALT_3", "Heizprogramm HK1 Samstag P3", cm_bedienmodul_2, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"HEIZPROG_1_SO", "Heizprogramm HK1 Sonntag P1", cm_bedienmodul_2, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"HEIZPROG_1_SO_SCHALT_2", "Heizprogramm HK1 Sonntag P2", cm_bedienmodul_2, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"HEIZPROG_1_SO_SCHALT_3", "Heizprogramm HK1 Sonntag P3", cm_bedienmodul_2, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"HEIZPROG_1_MO_FR", "Heizprogramm HK1 Mo-Fr P1", cm_bedienmodul_2, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"HEIZPROG_1_MO_FR_SCHALT_2", "Heizprogramm HK1 Mo-Fr P2", cm_bedienmodul_2, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"HEIZPROG_1_MO_FR_SCHALT_3", "Heizprogramm HK1 Mo-Fr P3", cm_bedienmodul_2, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"HEIZPROG_1_SA_SO", "Heizprogramm HK1 Sa-So P1", cm_bedienmodul_2, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"HEIZPROG_1_SA_SO_SCHALT_2", "Heizprogramm HK1 Sa-So P2", cm_bedienmodul_2, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"HEIZPROG_1_SA_SO_SCHALT_3", "Heizprogramm HK1 Sa-So P3", cm_bedienmodul_2, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"W_WASSERPROG_1_MO_SO", "WW-Programm Mo-So P1", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"W_WASSERPROG_1_MO_SO_SCHALT_2", "WW-Programm Mo-So P2", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"W_WASSERPROG_1_MO_SO_SCHALT_3", "WW-Programm Mo-So P3", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"W_WASSERPROG_1_MO", "WW-Programm Montag P1", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"W_WASSERPROG_1_MO_SCHALT_2", "WW-Programm Montag P2", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"W_WASSERPROG_1_MO_SCHALT_3", "WW-Programm Montag P3", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"W_WASSERPROG_1_DI", "WW-Programm Dienstag P1", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"W_WASSERPROG_1_DI_SCHALT_2", "WW-Programm Dienstag P2", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"W_WASSERPROG_1_DI_SCHALT_3", "WW-Programm Dienstag P3", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"W_WASSERPROG_1_MI", "WW-Programm Mittwoch P1", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"W_WASSERPROG_1_MI_SCHALT_2", "WW-Programm Mittwoch P2", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"W_WASSERPROG_1_MI_SCHALT_3", "WW-Programm Mittwoch P3", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"W_WASSERPROG_1_DO", "WW-Programm Donnerstag P1", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"W_WASSERPROG_1_DO_SCHALT_2", "WW-Programm Donnerstag P2", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"W_WASSERPROG_1_DO_SCHALT_3", "WW-Programm Donnerstag P3", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"W_WASSERPROG_1_FR", "WW-Programm Freitag P1", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"W_WASSERPROG_1_FR_SCHALT_2", "WW-Programm Freitag P2", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"W_WASSERPROG_1_FR_SCHALT_3", "WW-Programm Freitag P3", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"W_WASSERPROG_1_SA", "WW-Programm Samstag P1", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"W_WASSERPROG_1_SA_SCHALT_2", "WW-Programm Samstag P2", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"W_WASSERPROG_1_SA_SCHALT_3", "WW-Programm Samstag P3", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"W_WASSERPROG_1_SO", "WW-Programm Sonntag P1", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"W_WASSERPROG_1_SO_SCHALT_2", "WW-Programm Sonntag P2", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"W_WASSERPROG_1_SO_SCHALT_3", "WW-Programm Sonntag P3", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"W_WASSERPROG_1_MO_FR", "WW-Programm Mo-Fr P1", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"W_WASSERPROG_1_MO_FR_SCHALT_2", "WW-Programm Mo-Fr P2", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"W_WASSERPROG_1_MO_FR_SCHALT_3", "WW-Programm Mo-Fr P3", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"W_WASSERPROG_1_SA_SO", "WW-Programm Sa-So P1", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"W_WASSERPROG_1_SA_SO_SCHALT_2", "WW-Programm Sa-So P2", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"W_WASSERPROG_1_SA_SO_SCHALT_3", "WW-Programm Sa-So P3", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"ZBV_PROG_1_MO_SO", "Lüfter-Programm Mo-So P1", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"ZBV_PROG_1_MO_SO_SCHALT_2", "Lüfter-Programm Mo-So P2", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"ZBV_PROG_1_MO_SO_SCHALT_3", "Lüfter-Programm Mo-So P3", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"ZBV_PROG_1_MO", "Lüfter-Programm Montag P1", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"ZBV_PROG_1_MO_SCHALT_2", "Lüfter-Programm Montag P2", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"ZBV_PROG_1_MO_SCHALT_3", "Lüfter-Programm Montag P3", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"ZBV_PROG_1_DI", "Lüfter-Programm Dienstag P1", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"ZBV_PROG_1_DI_SCHALT_2", "Lüfter-Programm Dienstag P2", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"ZBV_PROG_1_DI_SCHALT_3", "Lüfter-Programm Dienstag P3", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"ZBV_PROG_1_MI", "Lüfter-Programm Mittwoch P1", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"ZBV_PROG_1_MI_SCHALT_2", "Lüfter-Programm Mittwoch P2", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"ZBV_PROG_1_MI_SCHALT_3", "Lüfter-Programm Mittwoch P3", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"ZBV_PROG_1_DO", "Lüfter-Programm Donnerstag P1", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"ZBV_PROG_1_DO_SCHALT_2", "Lüfter-Programm Donnerstag P2", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"ZBV_PROG_1_DO_SCHALT_3", "Lüfter-Programm Donnerstag P3", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"ZBV_PROG_1_FR", "Lüfter-Programm Freitag P1", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"ZBV_PROG_1_FR_SCHALT_2", "Lüfter-Programm Freitag P2", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"ZBV_PROG_1_FR_SCHALT_3", "Lüfter-Programm Freitag P3", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"ZBV_PROG_1_SA", "Lüfter-Programm Samstag P1", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"ZBV_PROG_1_SA_SCHALT_2", "Lüfter-Programm Samstag P2", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"ZBV_PROG_1_SA_SCHALT_3", "Lüfter-Programm Samstag P3", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"ZBV_PROG_1_SO", "Lüfter-Programm Sonntag P1", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"ZBV_PROG_1_SO_SCHALT_2", "Lüfter-Programm Sonntag P2", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"ZBV_PROG_1_SO_SCHALT_3", "Lüfter-Programm Sonntag P3", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"ZBV_PROG_1_MO_FR", "Lüfter-Programm Mo-Fr P1", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"ZBV_PROG_1_MO_FR_SCHALT_2", "Lüfter-Programm Mo-Fr P2", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"ZBV_PROG_1_MO_FR_SCHALT_3", "Lüfter-Programm Mo-Fr P3", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"ZBV_PROG_1_SA_SO", "Lüfter-Programm Sa-So P1", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"ZBV_PROG_1_SA_SO_SCHALT_2", "Lüfter-Programm Sa-So P2", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"ZBV_PROG_1_SA_SO_SCHALT_3", "Lüfter-Programm Sa-So P3", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"FERIEN_BEGINN_ZEIT", "Ferien Beginn Zeit", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"FERIEN_ENDE_ZEIT", "Ferien Ende Zeit", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"PARTYPROGRAMM_START", "Partyprogramm Start", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"PARTYPROGRAMM_ENDE", "Partyprogramm Ende", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"},
    {"KUEHLZEIT", "Kühlzeit", cm_kessel, 3, 11, TEXT_PATTERN_TIME_RANGE, "mdi:calendar-clock"}};

static const size_t WRITABLE_TEXT_COUNT = sizeof(writableTexts) / sizeof(WritableTextConfig);
// ============================================================================
// WRITABLE DATETIME CONFIGURATION (for MQTT Datetime entities)
// ============================================================================
// RUNTIME STATE TRACKING
// ============================================================================

// SG Ready: legacy globals kept for YAML sensor lambdas that read currentSgReadyState
// The controller is the authoritative source; these are updated via sgReadyStateInt().
static int currentSgReadyState = 2;

// Track which signals have been discovered
// Signals whose discovery was published: key = CAN id << 16 | Elster index (4 bytes instead of a uid string)
static std::unordered_set<uint32_t> discoveredSignals;
static inline uint32_t discoveryKey(const CanMember &cm, const ElsterIndex *ei) { return ((uint32_t)cm.CanId << 16) | ei->Index; }

// Track which calculated sensors have been discovered
static std::set<std::string> discoveredCalculatedSensors;

// Track which writable numbers have been discovered
static std::set<std::string> discoveredWritableNumbers;

// Track which writable selects have been discovered
static std::set<std::string> discoveredWritableSelects;

// Track which writable switches / buttons / texts have been discovered
static std::set<std::string> discoveredWritableSwitches;
static std::set<std::string> discoveredWritableButtons;
static std::set<std::string> discoveredWritableTexts;

// Discovery of the calculated compressor starts sensor (KESSEL device) has been published
static bool verdichterStartsDiscoveryDone = false;

// Track next scheduled request time per unique signal key (MEMBER_SIGNAL)
// Next request time per request: index = request index * 3 + member slot (slot 0, or 0..2 for cm_other).
// A plain vector instead of a string-keyed map: ~450 requests cost ~40 kB internal heap in the map
// and every check (4 times per second) allocated a std::string key.
static std::vector<unsigned long> nextRequestDue;

// Track starting position for round-robin signal processing (prevents starvation)
static int signalProcessingStartIndex = 0;

// Track sensor values for calculated sensors
static float lastVorlaufIst = NAN;
static float lastRuecklaufIstTemp = NAN;
static float lastVerdichterValue = NAN;

// Track date/time component values
static int lastJahr = -1;
static int lastMonat = -1;
static int lastTag = -1;
static int lastStunde = -1;
static int lastMinute = -1;
static int lastSekunde = -1;

// ---- Heat pump availability tracking -------------------------------------------------
// Timestamps (millis) of the last update of each date/time component (0 = never)
static unsigned long tsJahr = 0, tsMonat = 0, tsTag = 0, tsStunde = 0, tsMinute = 0;

// Last CAN message received from the heat pump (KESSEL)
static bool kesselRxSeen = false;
static unsigned long lastKesselRxMs = 0;

// Writes waiting for the read-back answer of the same signal
struct PendingWrite
{
    bool active;
    uint16_t index;
    const char *memberName;
    const char *signalName;
    unsigned long sentMs;
};
static PendingWrite pendingWrites[8] = {};
static bool suppressWriteAck = false;

// Clock deviation tracking
static bool clockMismatchActive = false;
static unsigned long clockMismatchSinceMs = 0;
static long lastClockDeviationMin = 0;

// Publication tracking
static unsigned long nextAvailabilityUpdate = 0;
static unsigned long lastAvailabilityPublishMs = 0;
static std::string lastAvailabilityStatus;

// Self healing / system diagnostics
static unsigned long nextSystemDiagUpdate = 0;
static unsigned long lastCanRestartMs = 0;
static uint32_t canRestartCount = 0;
static unsigned long lowHeapSinceMs = 0;
static bool selfHealRebootPending = false;
// Remember a write that was sent, so we can check whether the heat pump answers it
static void registerPendingWrite(const CanMember *cm, const ElsterIndex *ei)
{
    unsigned long now = millis();
    int slot = -1;
    for (int i = 0; i < 8; i++)
    {
        // Same signal written again: restart its timer
        if (pendingWrites[i].active && pendingWrites[i].index == ei->Index && pendingWrites[i].memberName == cm->Name)
        {
            slot = i;
            break;
        }
        if (slot < 0 && !pendingWrites[i].active)
            slot = i;
    }
    if (slot < 0)
    {
        // Table full: overwrite the oldest entry
        slot = 0;
        for (int i = 1; i < 8; i++)
            if (pendingWrites[i].sentMs < pendingWrites[slot].sentMs)
                slot = i;
    }
    pendingWrites[slot] = {true, ei->Index, cm->Name, ei->Name, now};
}

// Called for every CAN message the heat pump sends
static void onCanResponse(const CanMember &cm, const ElsterIndex *ei)
{
    if (strcmp(cm.Name, "KESSEL") == 0)
    {
        kesselRxSeen = true;
        lastKesselRxMs = millis();
    }
    for (int i = 0; i < 8; i++)
    {
        if (pendingWrites[i].active && pendingWrites[i].index == ei->Index && pendingWrites[i].memberName == cm.Name)
            pendingWrites[i].active = false;
    }
}

// Days since 1970-01-01 for a civil date (proleptic Gregorian calendar)
static long long daysFromCivil(int y, int m, int d)
{
    y -= m <= 2;
    const long long era = (y >= 0 ? y : y - 399) / 400;
    const long long yoe = y - era * 400;
    const long long doy = (153LL * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
    const long long doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + doe - 719468;
}
// Scheduled update times for calculated sensors (milliseconds)
static unsigned long nextDeltaTUpdate = 0;
static unsigned long nextCompressorUpdate = 0;
static unsigned long nextDateTimeUpdate = 0;
static unsigned long nextBetriebsartUpdate = 0;
static unsigned long nextCanDiagUpdate = 0;


// ============================================================================
// SIGNAL REQUEST CONFIGURATION
// ============================================================================

// Signal request configuration structure
typedef struct
{
    const char *signalName;
    unsigned long frequency; // Request frequency in seconds
    CanMemberType member;    // Use cm_other for "all members"
} SignalRequest;

// Forward declarations for the model-specific signal request table.
// The actual definition comes from signal_requests_*.h, included by the
// model's YAML package (e.g. lwz5splus.yaml via esphome: includes:).
extern const SignalRequest signalRequests[];
extern const size_t SIGNAL_REQUEST_COUNT_VALUE;
#define SIGNAL_REQUEST_COUNT SIGNAL_REQUEST_COUNT_VALUE

// Runtime state for signal request manager
static bool requestManagerStarted = false;
static unsigned long requestManagerStartTime = 0;

// ============================================================================
// CAN BUS HELPER FUNCTIONS
// ============================================================================

// Only compile function implementations when building with ESPHome framework
// Skip when compiling ha-dummy.cpp or other standalone contexts
#if !defined(HA_DUMMY_BUILD)

/**
 * Simple struct for CAN ID bytes (avoids heap allocation)
 */
struct CanIdBytes
{
    uint8_t first;
    uint8_t second;
};

/**
 * Generate CAN read ID from member CAN ID
 */
CanIdBytes generate_read_id(unsigned short can_id)
{
    uint8_t address = (can_id & 0x780) / 8;
    return {static_cast<uint8_t>((address & 0xF0) + 1), static_cast<uint8_t>(can_id & 7)};
}

CanIdBytes generate_write_id(unsigned short can_id)
{
    uint8_t address = (can_id & 0x780) / 8;
    return {static_cast<uint8_t>(address & 0xF0), static_cast<uint8_t>(can_id & 7)};
}

const CanMember &lookupCanMember(uint32_t canId)
{
    for (size_t i = 0; i < sizeof(CanMembers) / sizeof(CanMember); ++i)
    {
        if (CanMembers[i].CanId == canId)
        {
            return CanMembers[i];
        }
    }
    // Return the last element if member is not found
    return CanMembers[sizeof(CanMembers) / sizeof(CanMembers[0]) - 1];
}

// Check if signal is permanently blacklisted (lookup in ElsterTable)
inline bool isPermanentlyBlacklisted(const char *signalName)
{
    const ElsterIndex *ei = GetElsterIndex(signalName);
    return ei->isBlacklisted;
}

// Helper: Get type-based defaults for Home Assistant metadata
inline void getTypeDefaults(ElsterType type, const char *&component, const char *&deviceClass, const char *&unit, const char *&stateClass, const char *&icon)
{
    switch (type)
    {
    case et_dec_val:
    case et_cent_val:
        component = "sensor";
        deviceClass = "temperature";
        unit = "°C";
        stateClass = "measurement";
        icon = "mdi:thermometer";
        break;
    case et_mil_val:
        component = "sensor";
        deviceClass = "duration";
        unit = "h";
        stateClass = "total_increasing";
        icon = "mdi:timer";
        break;
    case et_bool:
    case et_little_bool:
        component = "binary_sensor";
        deviceClass = "";
        unit = "";
        stateClass = "";
        icon = "mdi:electric-switch";
        break;
    case et_byte:
    case et_little_endian:
        component = "sensor";
        deviceClass = "";
        unit = "";
        stateClass = "measurement";
        icon = "mdi:counter";
        break;
    case et_err_nr:
    case et_err_idx:
        component = "sensor";
        deviceClass = "";
        unit = "";
        stateClass = "";
        icon = "mdi:alert-circle";
        break;
    case et_dev_id:
    case et_dev_nr:
        component = "text";
        deviceClass = "";
        unit = "";
        stateClass = "";
        icon = "mdi:identifier";
        break;
    case et_betriebsart:
        component = "sensor";
        deviceClass = "enum";
        unit = "";
        stateClass = "";
        icon = "mdi:hvac";
        break;
    case et_zeit:
        component = "sensor";
        deviceClass = "timestamp";
        unit = "";
        stateClass = "";
        icon = "mdi:clock";
        break;
    case et_datum:
        component = "sensor";
        deviceClass = "date";
        unit = "";
        stateClass = "";
        icon = "mdi:calendar";
        break;
    default:
        component = "sensor";
        deviceClass = "";
        unit = "";
        stateClass = "";
        icon = "mdi:gauge";
        break;
    }
}

const ElsterIndex *processCanMessage(const std::vector<uint8_t> &msg, uint32_t can_id, std::string &signalValue, const CanMember **outCanMember)
{
    // Return if the message is too small
    if (msg.size() < 7)
    {
        return &ElsterTable[0];
    }

    const CanMember &cm = lookupCanMember(can_id);
    *outCanMember = &cm; // Return CanMember to caller

    const ElsterIndex *ei;
    uint8_t byte1;
    uint8_t byte2;
    char charValue[64]; // long texts: "not used time domain", error descriptions (et_err_idx)

    if (msg[2] == 0xfa)
    {
        byte1 = msg[5];
        byte2 = msg[6];
        ei = GetElsterIndex(msg[4] + (msg[3] << 8));
    }
    else
    {
        byte1 = msg[3];
        byte2 = msg[4];
        ei = GetElsterIndex(msg[2]);
    }

    switch (ei->Type)
    {
    case et_double_val:
        SetDoubleType(charValue, ei->Type, static_cast<double>(byte2 + (byte1 << 8)));
        break;
    case et_triple_val:
        SetDoubleType(charValue, ei->Type, static_cast<double>(byte2 + (byte1 << 8)));
        break;
    default:
        SetValueType(charValue, ei->Type, static_cast<int>(byte2 + (byte1 << 8)));
        break;
    }

    ESP_LOGD("processCanMessage()", "%s (0x%02x):\t%s:\t%s\t(%s)", cm.Name, (unsigned)cm.CanId, ei->Name, charValue, ElsterTypeStr[ei->Type]);

    signalValue = charValue;
    return ei;
}

void readSignal(const CanMember *cm, const ElsterIndex *ei)
{
    constexpr bool use_extended_id = false; // No use of extended ID
    const uint8_t IndexByte1 = static_cast<uint8_t>(ei->Index >> 8);
    const uint8_t IndexByte2 = static_cast<uint8_t>(ei->Index & 0xFF);
    std::vector<uint8_t> data;
    CanIdBytes readId = generate_read_id(cm->CanId);

    // Always use the extended index form (0xFA + 16 bit index), like can2mqtt does.
    // The LWZ 5S Plus does not answer the short form (index < 0x100 in byte 2).
    data = {readId.first,
            readId.second,
            0xFA,
            IndexByte1,
            IndexByte2,
            0x00,
            0x00};

    char logmsg[120];
    snprintf(logmsg, sizeof(logmsg), "READ \"%s\" (0x%04x) FROM %s (0x%02x {0x%02x, 0x%02x}): %02x, %02x, %02x, %02x, %02x, %02x, %02x", ei->Name, ei->Index, cm->Name, (unsigned)cm->CanId, readId.first, readId.second, data[0], data[1], data[2], data[3], data[4], data[5], data[6]);
    ESP_LOGD("readSignal()", "%s", logmsg);

    id(my_can).send_data(CanMembers[cm_pc].CanId, use_extended_id, data);
}

void readSignal(const CanMember *cm, const char *elsterName)
{
    readSignal(cm, GetElsterIndex(elsterName));
    return;
}

void writeSignal(const CanMember *cm, const ElsterIndex *ei, const char *&str)
{
    bool use_extended_id = false;
    int writeValue = TranslateString(str, ei->Type);
    if (writeValue == -1)
    {
        ESP_LOGW("writeSignal()", "TranslateString failed for \"%s\" (input: \"%s\") — refusing to write", ei->Name, str);
        return;
    }
    uint8_t IndexByte1 = static_cast<uint8_t>(ei->Index >> 8);
    uint8_t IndexByte2 = static_cast<uint8_t>(ei->Index & 0xFF);
    std::vector<uint8_t> data;
    CanIdBytes writeId = generate_write_id(cm->CanId);

    // Always use the extended index form (0xFA + 16 bit index), like readSignal()
    data = {writeId.first,
            writeId.second,
            0xfa,
            IndexByte1,
            IndexByte2,
            static_cast<uint8_t>(writeValue >> 8),
            static_cast<uint8_t>(writeValue & 0xFF)};

    char logmsg[120];
    snprintf(logmsg, sizeof(logmsg), "WRITE \"%s\" (0x%04x): \"%d\" TO: %s (0x%02x {0x%02x, 0x%02x}): %02x, %02x, %02x, %02x, %02x, %02x, %02x",
             ei->Name, ei->Index, writeValue, cm->Name, (unsigned)cm->CanId, writeId.first, writeId.second,
             data[0], data[1], data[2], data[3], data[4], data[5], data[6]);
    ESP_LOGI("writeSignal()", "%s", logmsg);

    // Expect the heat pump to answer the read-back of this signal (see availability sensor)
    if (!suppressWriteAck)
        registerPendingWrite(cm, ei);

    id(my_can).send_data(CanMembers[cm_pc].CanId, use_extended_id, data);
}

void writeSignal(const CanMember *cm, const char *elsterName, const char *str)
{
    const char *strCopy = str; // Create a non-const pointer we can pass by reference
    writeSignal(cm, GetElsterIndex(elsterName), strCopy);
    return;
}

// Write without expecting an answer (e.g. buttons that trigger an action and have no read-back)
void writeSignalNoAck(const CanMember *cm, const char *elsterName, const char *str)
{
    suppressWriteAck = true;
    writeSignal(cm, elsterName, str);
    suppressWriteAck = false;
}
// Unified function to publish MQTT discovery for calculated sensors
void publishCalculatedSensorDiscovery(const CalculatedSensorConfig &config, bool forceRepublish = false)
{
    // Check if already published (unless force republish)
    if (!forceRepublish && discoveredCalculatedSensors.find(config.uniqueId) != discoveredCalculatedSensors.end())
    {
        return;
    }

    // Mark as discovered
    discoveredCalculatedSensors.insert(config.uniqueId);

    // Build discovery topic
    char discoveryTopic[256];
    snprintf(discoveryTopic, sizeof(discoveryTopic),
             "homeassistant/%s/heatingpump/%s/config", config.component, config.uniqueId);

    // Build JSON payload
    std::ostringstream payload;
    payload << "{\"name\":\"" << config.name << "\","
            << "\"unique_id\":\"" << config.uniqueId << "\","
            << "\"state_topic\":\"" << config.stateTopic << "\"";

    // Add device class if specified
    if (config.deviceClass[0] != '\0')
    {
        payload << ",\"device_class\":\"" << config.deviceClass << "\"";
    }

    // Add unit if specified
    if (config.unit[0] != '\0')
    {
        payload << ",\"unit_of_measurement\":\"" << config.unit << "\"";
    }

    // Add state class if specified
    if (config.stateClass[0] != '\0')
    {
        payload << ",\"state_class\":\"" << config.stateClass << "\"";
    }

    // Add icon if specified
    if (config.icon[0] != '\0')
    {
        payload << ",\"icon\":\"" << config.icon << "\"";
    }

    // For binary sensors, add payload on/off
    if (config.payloadOn[0] != '\0' && config.payloadOff[0] != '\0')
    {
        payload << ",\"payload_on\":\"" << config.payloadOn << "\","
                << "\"payload_off\":\"" << config.payloadOff << "\"";
    }

    // Add entity category if specified
    if (config.entityCategory[0] != '\0')
    {
        payload << ",\"entity_category\":\"" << config.entityCategory << "\"";
    }

    // Add enabled_by_default if false (true is HA default, no need to emit)
    if (!config.enabledByDefault)
    {
        payload << ",\"enabled_by_default\":false";
    }

    // Add device info
    payload << ",\"device\":{\"identifiers\":[\"stiebel_eltron_" HA_DEVICE_MODEL_STR "\"],"
            << "\"name\":\"Stiebel Eltron Wärmepumpe\","
            << "\"manufacturer\":\"Stiebel Eltron\"}}";

    // Publish discovery message
    std::string payloadStr = payload.str();
    id(mqtt_client).publish(discoveryTopic, payloadStr.c_str(), payloadStr.length(), 0, true);

    ESP_LOGI("MQTT", "Discovery published for calculated sensor: %s", config.name);
}

// Publish all calculated sensor discoveries (used during startup and republish)
void publishAllCalculatedSensorDiscoveries(bool forceRepublish = false)
{
    if (forceRepublish)
    {
        verdichterStartsDiscoveryDone = false;
        discoveredCalculatedSensors.clear();
        ESP_LOGI("MQTT", "Republishing all calculated sensor discoveries");
    }

    for (size_t i = 0; i < CALCULATED_SENSOR_COUNT; i++)
    {
        publishCalculatedSensorDiscovery(calculatedSensors[i], forceRepublish);
    }
}

// Publish a retained discovery message. Returns false if the MQTT client could not queue it;
// lastDiscoveryOk is reset by the caller and read by the discovery pump (see below).
static bool lastDiscoveryOk = true;

static bool publishDiscoveryMessage(const std::string &topic, const std::string &payload)
{
    bool ok = id(mqtt_client).publish(topic.c_str(), payload.c_str(), payload.length(), 0, true);
    if (!ok)
    {
        lastDiscoveryOk = false;
        ESP_LOGW("MQTT_DISC", "Publishing %s failed - will retry", topic.c_str());
    }
    return ok;
}
// ============================================================================
// MQTT WRITABLE NUMBER (for temperature setpoints, etc.)
// ============================================================================

// Publish MQTT discovery for a writable number entity
void publishWritableNumberDiscovery(const WritableNumberConfig &config, bool forceRepublish = false)
{
    // Get CanMember for building topics
    const CanMember *cm = &CanMembers[config.member];

    // Build unique ID
    char uniqueId[128];
    snprintf(uniqueId, sizeof(uniqueId), "stiebel_%s_%s", cm->Name, config.signalName);
    std::string uid(uniqueId);
    std::transform(uid.begin(), uid.end(), uid.begin(), ::tolower);

    // Check if already published (unless force republish)
    if (!forceRepublish && discoveredWritableNumbers.find(uid) != discoveredWritableNumbers.end())
    {
        return;
    }

    // Build discovery topic: homeassistant/number/heatingpump/<unique_id>/config
    char discoveryTopic[256];
    snprintf(discoveryTopic, sizeof(discoveryTopic),
             "homeassistant/number/heatingpump/%s/config", uid.c_str());

    // Build command topic: heatingpump/<MEMBER>/<SIGNAL>/set
    char commandTopic[128];
    snprintf(commandTopic, sizeof(commandTopic), "heatingpump/%s/%s/set", cm->Name, config.signalName);

    // Build state topic: heatingpump/<MEMBER>/<SIGNAL>/state
    char stateTopic[128];
    snprintf(stateTopic, sizeof(stateTopic), "heatingpump/%s/%s/state", cm->Name, config.signalName);

    // Build JSON payload
    std::ostringstream payload;
    payload << "{\"name\":\"" << config.friendlyName << "\","
            << "\"unique_id\":\"" << uid << "\","
            << "\"command_topic\":\"" << commandTopic << "\","
            << "\"state_topic\":\"" << stateTopic << "\"";

    payload << ",\"min\":\"" << config.min << "\"";
    payload << ",\"max\":\"" << config.max << "\"";

    if (config.step != 1)
    {
        payload << ",\"step\":\"" << config.step << "\"";
    }

    /*if ((config.max - config.min) / config.step <= 10)
    {
        payload << ",\"mode\":\"box\"";
    }*/

    if (config.unit[0] != '\0')
    {
        payload << ",\"unit_of_measurement\":\"" << config.unit << "\"";
    }
    if (config.deviceClass[0] != '\0')
    {
        payload << ",\"device_class\":\"" << config.deviceClass << "\"";
    }
    if (config.icon[0] != '\0')
    {
        payload << ",\"icon\":\"" << config.icon << "\"";
    }

    // Check if this is an SG Ready control - assign to main device
    bool isSgReady = (strncmp(config.signalName, "SG_READY_", 9) == 0);

    if (isSgReady)
    {
        // SG Ready controls go to main device
        payload << ",\"device\":{\"identifiers\":[\"stiebel_eltron_" HA_DEVICE_MODEL_STR "\"],"
                << "\"name\":\"Stiebel Eltron Wärmepumpe\","
                << "\"manufacturer\":\"Stiebel Eltron\"}}";
    }
    else
    {
        // Regular controls go to their respective CAN member device
        char canMemberDeviceId[64];
        snprintf(canMemberDeviceId, sizeof(canMemberDeviceId), "stiebel_%s", cm->Name);

        const char *canMemberFriendlyName = cm->Name;
        if (strcmp(cm->Name, "KESSEL") == 0)
            canMemberFriendlyName = "Kessel";
        else if (strcmp(cm->Name, "MANAGER") == 0)
            canMemberFriendlyName = "Manager";
        else if (strcmp(cm->Name, "HEIZMODUL") == 0)
            canMemberFriendlyName = "Heizmodul";

        payload << ",\"device\":{\"identifiers\":[\"" << canMemberDeviceId << "\"],"
                << "\"name\":\"" << canMemberFriendlyName << "\","
                << "\"via_device\":\"stiebel_eltron_" HA_DEVICE_MODEL_STR "\","
                << "\"manufacturer\":\"Stiebel Eltron\"}}";
    }

    // Publish discovery message
    std::string payloadStr = payload.str();
    if (!publishDiscoveryMessage(discoveryTopic, payloadStr))
        return;
    discoveredWritableNumbers.insert(uid);

    ESP_LOGI("MQTT", "Discovery published for writable number: %s", config.friendlyName);
}

// Look up a writable number by signal name and CAN member, then publish its discovery.
// Avoids hard-coded writableNumbers[] indices in the MQTT command handlers.
void publishWritableNumberDiscoveryByName(const char *signalName, CanMemberType member, bool forceRepublish = false)
{
    for (size_t i = 0; i < WRITABLE_NUMBER_COUNT; i++)
    {
        if (writableNumbers[i].member == member && strcmp(writableNumbers[i].signalName, signalName) == 0)
        {
            publishWritableNumberDiscovery(writableNumbers[i], forceRepublish);
            return;
        }
    }
    ESP_LOGW("MQTT_DISC", "No writable number configured for %s (member %d)", signalName, (int)member);
}
// Publish all writable number discoveries
void publishAllWritableNumberDiscoveries(bool forceRepublish = false)
{
    if (forceRepublish)
    {
        discoveredWritableNumbers.clear();
        ESP_LOGI("MQTT", "Republishing all writable number discoveries");
    }

    ESP_LOGI("MQTT_DISC", "Publishing %d writable number discoveries", WRITABLE_NUMBER_COUNT);
    for (size_t i = 0; i < WRITABLE_NUMBER_COUNT; i++)
    {
        ESP_LOGI("MQTT_DISC", "Publishing writable number %d: %s", i, writableNumbers[i].signalName);
        publishWritableNumberDiscovery(writableNumbers[i], forceRepublish);
    }
    ESP_LOGI("MQTT_DISC", "Completed publishing writable number discoveries");
}

// ============================================================================
// WRITABLE SELECT DISCOVERY (MQTT Select entities)
// ============================================================================

void publishWritableSelectDiscovery(const WritableSelectConfig &config, bool forceRepublish = false)
{
    // Get CAN member
    const CanMember *cm = &CanMembers[config.member];

    // Build unique ID (lowercase with underscores)
    char uniqueId[128];
    snprintf(uniqueId, sizeof(uniqueId), "stiebel_%s_%s", cm->Name, config.signalName);
    std::string uniqueIdStr(uniqueId);
    std::transform(uniqueIdStr.begin(), uniqueIdStr.end(), uniqueIdStr.begin(), ::tolower);

    // Check if already published (unless forcing)
    if (!forceRepublish && discoveredWritableSelects.count(uniqueIdStr) > 0)
    {
        return;
    }

    // Build discovery topic
    std::ostringstream discoveryTopicStream;
    discoveryTopicStream << "homeassistant/select/heatingpump/" << uniqueIdStr << "/config";
    std::string discoveryTopic = discoveryTopicStream.str();

    // Build command and state topics
    std::ostringstream commandTopicStream;
    commandTopicStream << "heatingpump/" << cm->Name << "/" << config.signalName << "/set";
    std::string commandTopic = commandTopicStream.str();

    std::ostringstream stateTopicStream;
    stateTopicStream << "heatingpump/" << cm->Name << "/" << config.signalName << "/state";
    std::string stateTopic = stateTopicStream.str();

    // Build JSON payload
    std::ostringstream payload;
    payload << "{\"name\":\"" << config.friendlyName << "\","
            << "\"unique_id\":\"" << uniqueIdStr << "\","
            << "\"command_topic\":\"" << commandTopic << "\","
            << "\"state_topic\":\"" << stateTopic << "\","
            << "\"options\":[";

    // Add options array
    for (size_t i = 0; i < config.optionCount; i++)
    {
        if (i > 0)
            payload << ",";
        payload << "\"" << config.options[i] << "\"";
    }
    payload << "],";

    // Add icon if specified
    if (config.icon && strlen(config.icon) > 0)
    {
        payload << "\"icon\":\"" << config.icon << "\",";
    }

    // Check if this is an SG Ready control - assign to main device
    bool isSgReady = (strncmp(config.signalName, "SG_READY_", 9) == 0);

    if (isSgReady)
    {
        // SG Ready controls go to main device
        payload << "\"device\":{\"identifiers\":[\"stiebel_eltron_" HA_DEVICE_MODEL_STR "\"],"
                << "\"name\":\"Stiebel Eltron Wärmepumpe\","
                << "\"manufacturer\":\"Stiebel Eltron\"}}";
    }
    else
    {
        // Regular controls go to their respective CAN member device
        char canMemberDeviceId[64];
        snprintf(canMemberDeviceId, sizeof(canMemberDeviceId), "stiebel_%s", cm->Name);

        const char *canMemberFriendlyName = cm->Name;
        if (strcmp(cm->Name, "KESSEL") == 0)
            canMemberFriendlyName = "Kessel";
        else if (strcmp(cm->Name, "MANAGER") == 0)
            canMemberFriendlyName = "Manager";
        else if (strcmp(cm->Name, "HEIZMODUL") == 0)
            canMemberFriendlyName = "Heizmodul";

        payload << "\"device\":{\"identifiers\":[\"" << canMemberDeviceId << "\"],"
                << "\"name\":\"" << canMemberFriendlyName << "\","
                << "\"via_device\":\"stiebel_eltron_" HA_DEVICE_MODEL_STR "\","
                << "\"manufacturer\":\"Stiebel Eltron\"}}";
    }

    // Publish discovery message
    std::string payloadStr = payload.str();
    if (!publishDiscoveryMessage(discoveryTopic, payloadStr))
        return;

    // Mark as discovered
    discoveredWritableSelects.insert(uniqueIdStr);

    ESP_LOGI("MQTT", "Discovery published for writable select: %s", config.friendlyName);
}

// Publish all writable select discoveries
void publishAllWritableSelectDiscoveries(bool forceRepublish = false)
{
    if (forceRepublish)
    {
        discoveredWritableSelects.clear();
        ESP_LOGI("MQTT", "Republishing all writable select discoveries");
    }

    ESP_LOGI("MQTT_DISC", "Publishing %d writable select discoveries", WRITABLE_SELECT_COUNT);
    for (size_t i = 0; i < WRITABLE_SELECT_COUNT; i++)
    {
        ESP_LOGI("MQTT_DISC", "Publishing writable select %d: %s", i, writableSelects[i].signalName);
        publishWritableSelectDiscovery(writableSelects[i], forceRepublish);
    }
    ESP_LOGI("MQTT_DISC", "Completed publishing writable select discoveries");
}

// Look up a writable select by signal name and CAN member, then publish its discovery.
void publishWritableSelectDiscoveryByName(const char *signalName, CanMemberType member, bool forceRepublish = false)
{
    for (size_t i = 0; i < WRITABLE_SELECT_COUNT; i++)
    {
        if (writableSelects[i].member == member && strcmp(writableSelects[i].signalName, signalName) == 0)
        {
            publishWritableSelectDiscovery(writableSelects[i], forceRepublish);
            return;
        }
    }
    ESP_LOGW("MQTT_DISC", "No writable select configured for %s (member %d)", signalName, (int)member);
}

// Translate a select option text into the value written to the CAN bus.
// Returns the option text itself for selects without a value table (e.g. PROGRAMMSCHALTER),
// and nullptr if the option is unknown or display-only.
const char *writableSelectCanValue(const char *signalName, CanMemberType member, const char *option)
{
    for (size_t i = 0; i < WRITABLE_SELECT_COUNT; i++)
    {
        const WritableSelectConfig &cfg = writableSelects[i];
        if (cfg.member != member || strcmp(cfg.signalName, signalName) != 0)
            continue;
        if (cfg.values == nullptr)
            return option;
        for (size_t k = 0; k < cfg.optionCount; k++)
        {
            if (strcmp(cfg.options[k], option) == 0)
                return cfg.values[k][0] != '\0' ? cfg.values[k] : nullptr;
        }
        return nullptr;
    }
    return nullptr;
}

// Translate a CAN value received for a select with a value table into its option text.
// Returns nullptr if the signal is not such a select. Unknown values map to a display-only
// option (empty value) if the select has one, otherwise the raw value is kept (nullptr).
const char *translateWritableSelectState(const CanMember &cm, const char *signalName, const std::string &value)
{
    for (size_t i = 0; i < WRITABLE_SELECT_COUNT; i++)
    {
        const WritableSelectConfig &cfg = writableSelects[i];
        if (cfg.values == nullptr || strcmp(CanMembers[cfg.member].Name, cm.Name) != 0 || strcmp(cfg.signalName, signalName) != 0)
            continue;
        const char *fallback = nullptr;
        for (size_t k = 0; k < cfg.optionCount; k++)
        {
            if (cfg.values[k][0] == '\0')
                fallback = cfg.options[k];
            else if (value == cfg.values[k])
                return cfg.options[k];
        }
        return fallback;
    }
    return nullptr;
}


// ============================================================================
// WRITABLE SWITCH / BUTTON / TEXT DISCOVERY (MQTT Switch, Button and Text entities)
// ============================================================================

// Append the "device" part of a discovery payload (sub-device of the CAN member) and close the JSON object.
static void appendMemberDeviceAndClose(std::ostringstream &payload, const CanMember *cm)
{
    const char *canMemberFriendlyName = cm->Name;
    if (strcmp(cm->Name, "KESSEL") == 0)
        canMemberFriendlyName = "Kessel";
    else if (strcmp(cm->Name, "MANAGER") == 0)
        canMemberFriendlyName = "Manager";
    else if (strcmp(cm->Name, "HEIZMODUL") == 0)
        canMemberFriendlyName = "Heizmodul";

    payload << ",\"device\":{\"identifiers\":[\"stiebel_" << cm->Name << "\"],"
            << "\"name\":\"" << canMemberFriendlyName << "\","
            << "\"via_device\":\"stiebel_eltron_" HA_DEVICE_MODEL_STR "\","
            << "\"manufacturer\":\"Stiebel Eltron\"}}";
}

static std::string writableEntityUid(const CanMember *cm, const char *entityId)
{
    char uniqueId[160];
    snprintf(uniqueId, sizeof(uniqueId), "stiebel_%s_%s", cm->Name, entityId);
    std::string uid(uniqueId);
    std::transform(uid.begin(), uid.end(), uid.begin(), ::tolower);
    return uid;
}

static const char *switchEntityId(const WritableSwitchConfig &config)
{
    return config.entityId ? config.entityId : config.signalName;
}

void publishWritableSwitchDiscovery(const WritableSwitchConfig &config, bool forceRepublish = false)
{
    const CanMember *cm = &CanMembers[config.member];
    const char *entityId = switchEntityId(config);
    std::string uid = writableEntityUid(cm, entityId);

    if (!forceRepublish && discoveredWritableSwitches.count(uid) > 0)
        return;

    std::ostringstream payload;
    payload << "{\"name\":\"" << config.friendlyName << "\","
            << "\"unique_id\":\"" << uid << "\","
            << "\"command_topic\":\"heatingpump/" << cm->Name << "/" << entityId << "/set\","
            << "\"state_topic\":\"heatingpump/" << cm->Name << "/" << config.signalName << "/state\","
            << "\"payload_on\":\"" << config.payloadOn << "\","
            << "\"payload_off\":\"" << config.payloadOff << "\","
            << "\"state_on\":\"" << config.stateOn << "\","
            << "\"state_off\":\"" << config.stateOff << "\"";
    if (config.icon && config.icon[0] != '\0')
        payload << ",\"icon\":\"" << config.icon << "\"";
    appendMemberDeviceAndClose(payload, cm);

    std::string topic = "homeassistant/switch/heatingpump/" + uid + "/config";
    std::string payloadStr = payload.str();
    if (!publishDiscoveryMessage(topic, payloadStr))
        return;
    discoveredWritableSwitches.insert(uid);
    ESP_LOGI("MQTT", "Discovery published for writable switch: %s", config.friendlyName);
}

void publishWritableButtonDiscovery(const WritableButtonConfig &config, bool forceRepublish = false)
{
    const CanMember *cm = &CanMembers[config.member];
    std::string uid = writableEntityUid(cm, config.signalName);

    if (!forceRepublish && discoveredWritableButtons.count(uid) > 0)
        return;

    std::ostringstream payload;
    payload << "{\"name\":\"" << config.friendlyName << "\","
            << "\"unique_id\":\"" << uid << "\","
            << "\"command_topic\":\"heatingpump/" << cm->Name << "/" << config.signalName << "/set\","
            << "\"payload_press\":\"" << config.payload << "\","
            << "\"entity_category\":\"config\"";
    if (config.deviceClass && config.deviceClass[0] != '\0')
        payload << ",\"device_class\":\"" << config.deviceClass << "\"";
    if (config.icon && config.icon[0] != '\0')
        payload << ",\"icon\":\"" << config.icon << "\"";
    if (!config.enabledByDefault)
        payload << ",\"enabled_by_default\":false";
    appendMemberDeviceAndClose(payload, cm);

    std::string topic = "homeassistant/button/heatingpump/" + uid + "/config";
    std::string payloadStr = payload.str();
    if (!publishDiscoveryMessage(topic, payloadStr))
        return;
    discoveredWritableButtons.insert(uid);
    ESP_LOGI("MQTT", "Discovery published for writable button: %s", config.friendlyName);
}

void publishWritableTextDiscovery(const WritableTextConfig &config, bool forceRepublish = false)
{
    const CanMember *cm = &CanMembers[config.member];
    std::string uid = writableEntityUid(cm, config.signalName);

    if (!forceRepublish && discoveredWritableTexts.count(uid) > 0)
        return;

    std::ostringstream payload;
    payload << "{\"name\":\"" << config.friendlyName << "\","
            << "\"unique_id\":\"" << uid << "\","
            << "\"command_topic\":\"heatingpump/" << cm->Name << "/" << config.signalName << "/set\","
            << "\"state_topic\":\"heatingpump/" << cm->Name << "/" << config.signalName << "/state\","
            << "\"min\":" << config.minLength << ",\"max\":" << config.maxLength << ","
            << "\"pattern\":\"" << config.pattern << "\",\"mode\":\"text\"";
    if (config.icon && config.icon[0] != '\0')
        payload << ",\"icon\":\"" << config.icon << "\"";
    appendMemberDeviceAndClose(payload, cm);

    std::string topic = "homeassistant/text/heatingpump/" + uid + "/config";
    std::string payloadStr = payload.str();
    if (!publishDiscoveryMessage(topic, payloadStr))
        return;
    discoveredWritableTexts.insert(uid);
    ESP_LOGI("MQTT", "Discovery published for writable text: %s", config.friendlyName);
}

void publishAllWritableSwitchDiscoveries(bool forceRepublish = false)
{
    if (forceRepublish)
        discoveredWritableSwitches.clear();
    for (size_t i = 0; i < WRITABLE_SWITCH_COUNT; i++)
        publishWritableSwitchDiscovery(writableSwitches[i], forceRepublish);
}

void publishAllWritableButtonDiscoveries(bool forceRepublish = false)
{
    if (forceRepublish)
        discoveredWritableButtons.clear();
    for (size_t i = 0; i < WRITABLE_BUTTON_COUNT; i++)
        publishWritableButtonDiscovery(writableButtons[i], forceRepublish);
}

void publishAllWritableTextDiscoveries(bool forceRepublish = false)
{
    if (forceRepublish)
        discoveredWritableTexts.clear();
    for (size_t i = 0; i < WRITABLE_TEXT_COUNT; i++)
        publishWritableTextDiscovery(writableTexts[i], forceRepublish);
}

// ---- Discovery pump ---------------------------------------------------------------
// Publishing ~150 retained discovery messages in one burst right after the MQTT connect was
// lost on the device (nothing arrived at the broker). The pump publishes a few messages per
// second instead and retries an entity if the MQTT client could not queue its message.
static bool discoveryPumpActive = false;
static size_t discoveryPumpIndex = 0;
// SG Ready control states are published after the pump (not in the on_connect burst, where they got lost)
static bool sgReadyStatesPending = false;
bool publishSgReadyStates();

// Start (or restart) publishing all writable entity discoveries. force = also already published ones.
void startWritableDiscoveryPump(bool forceRepublish = false)
{
    if (forceRepublish)
    {
        discoveredWritableNumbers.clear();
        discoveredWritableSelects.clear();
        discoveredWritableSwitches.clear();
        discoveredWritableButtons.clear();
        discoveredWritableTexts.clear();
    }
    discoveryPumpIndex = 0;
    discoveryPumpActive = true;
}

// Call about once per second. Publishes up to DISCOVERY_PUMP_BATCH entities per call.
void runWritableDiscoveryPump()
{
    if (!id(mqtt_client).is_connected())
        return;
    if (!discoveryPumpActive)
    {
        if (sgReadyStatesPending && publishSgReadyStates())
            sgReadyStatesPending = false;
        return;
    }

    const size_t total = WRITABLE_NUMBER_COUNT + WRITABLE_SELECT_COUNT + WRITABLE_SWITCH_COUNT +
                         WRITABLE_BUTTON_COUNT + WRITABLE_TEXT_COUNT;

    for (int n = 0; n < DISCOVERY_PUMP_BATCH && discoveryPumpIndex < total; n++)
    {
        size_t i = discoveryPumpIndex;
        lastDiscoveryOk = true;
        if (i < WRITABLE_NUMBER_COUNT)
            publishWritableNumberDiscovery(writableNumbers[i]);
        else if ((i -= WRITABLE_NUMBER_COUNT) < WRITABLE_SELECT_COUNT)
            publishWritableSelectDiscovery(writableSelects[i]);
        else if ((i -= WRITABLE_SELECT_COUNT) < WRITABLE_SWITCH_COUNT)
            publishWritableSwitchDiscovery(writableSwitches[i]);
        else if ((i -= WRITABLE_SWITCH_COUNT) < WRITABLE_BUTTON_COUNT)
            publishWritableButtonDiscovery(writableButtons[i]);
        else
            publishWritableTextDiscovery(writableTexts[i - WRITABLE_BUTTON_COUNT]);

        if (!lastDiscoveryOk)
            return; // MQTT client busy: retry the same entity on the next call
        discoveryPumpIndex++;
    }

    if (discoveryPumpIndex >= total)
    {
        discoveryPumpActive = false;
        sgReadyStatesPending = true; // published on the next call
        ESP_LOGI("MQTT_DISC", "Writable entity discoveries published (%u entities)", (unsigned)total);
    }
}
// Lookups by entity id (or signal name) and CAN member, used by the MQTT command handlers.
void publishWritableSwitchDiscoveryByName(const char *entityId, CanMemberType member, bool forceRepublish = false)
{
    for (size_t i = 0; i < WRITABLE_SWITCH_COUNT; i++)
    {
        if (writableSwitches[i].member == member && strcmp(switchEntityId(writableSwitches[i]), entityId) == 0)
        {
            publishWritableSwitchDiscovery(writableSwitches[i], forceRepublish);
            return;
        }
    }
    ESP_LOGW("MQTT_DISC", "No writable switch configured for %s (member %d)", entityId, (int)member);
}

void publishWritableButtonDiscoveryByName(const char *signalName, CanMemberType member, bool forceRepublish = false)
{
    for (size_t i = 0; i < WRITABLE_BUTTON_COUNT; i++)
    {
        if (writableButtons[i].member == member && strcmp(writableButtons[i].signalName, signalName) == 0)
        {
            publishWritableButtonDiscovery(writableButtons[i], forceRepublish);
            return;
        }
    }
    ESP_LOGW("MQTT_DISC", "No writable button configured for %s (member %d)", signalName, (int)member);
}

void publishWritableTextDiscoveryByName(const char *signalName, CanMemberType member, bool forceRepublish = false)
{
    for (size_t i = 0; i < WRITABLE_TEXT_COUNT; i++)
    {
        if (writableTexts[i].member == member && strcmp(writableTexts[i].signalName, signalName) == 0)
        {
            publishWritableTextDiscovery(writableTexts[i], forceRepublish);
            return;
        }
    }
    ESP_LOGW("MQTT_DISC", "No writable text configured for %s (member %d)", signalName, (int)member);
}

// Returns the CAN signal of a configured switch and, via canValue, the string to write
// (nullptr if the payload is neither the ON nor the OFF payload of that switch).
const char *writableSwitchCanSignal(const char *entityId, CanMemberType member, const char *payload, const char *&canValue)
{
    canValue = nullptr;
    for (size_t i = 0; i < WRITABLE_SWITCH_COUNT; i++)
    {
        const WritableSwitchConfig &cfg = writableSwitches[i];
        if (cfg.member != member || strcmp(switchEntityId(cfg), entityId) != 0)
            continue;
        if (strcmp(payload, cfg.payloadOn) == 0)
            canValue = cfg.payloadOn;
        else if (strcmp(payload, cfg.payloadOff) == 0)
            canValue = cfg.payloadOff;
        return cfg.signalName;
    }
    return nullptr;
}

// Returns the string to write for a configured button press, nullptr if payload is not the press payload.
const char *writableButtonCanValue(const char *signalName, CanMemberType member, const char *payload)
{
    for (size_t i = 0; i < WRITABLE_BUTTON_COUNT; i++)
    {
        const WritableButtonConfig &cfg = writableButtons[i];
        if (cfg.member == member && strcmp(cfg.signalName, signalName) == 0)
            return strcmp(payload, cfg.payload) == 0 ? cfg.payload : nullptr;
    }
    return nullptr;
}

// ============================================================================
// Note: The heat pump clock is set from the ESP's SNTP time via the
// "Update Uhrzeit" / "Update Datum" buttons (updateTime() / updateDate()).
// MQTT datetime discovery is not supported by Home Assistant.
// ============================================================================

void publishDate()
{
    // Publish discovery (only once - cached)
    publishCalculatedSensorDiscovery(calculatedSensors[0]);

    // Validate that all values have been received
    if (lastJahr < 0 || lastMonat < 0 || lastTag < 0)
    {
        ESP_LOGW("CALC", "Cannot publish date: sensors not initialized (Jahr=%d, Monat=%d, Tag=%d)",
                 lastJahr, lastMonat, lastTag);
        return;
    }

    int ijahr = lastJahr;
    int imonat = lastMonat;
    int itag = lastTag;

    // Log raw values for debugging
    // ESP_LOGD("CALC", "Date values: Jahr=%d, Monat=%d, Tag=%d", ijahr, imonat, itag);

    // Validate ranges
    if (ijahr < 0 || ijahr > 99 || imonat < 1 || imonat > 12 || itag < 1 || itag > 31)
    {
        ESP_LOGW("CALC", "Date values out of range: Jahr=%d, Monat=%d, Tag=%d", ijahr, imonat, itag);
        return;
    }

    // Format date components
    std::string jahr = (ijahr < 10) ? "0" + std::to_string(ijahr) : std::to_string(ijahr);
    std::string monat = (imonat < 10) ? "0" + std::to_string(imonat) : std::to_string(imonat);
    std::string tag = (itag < 10) ? "0" + std::to_string(itag) : std::to_string(itag);

    std::string datum = "20" + jahr + "-" + monat + "-" + tag;

    // Publish state to MQTT
    const char *stateTopic = "heatingpump/calculated/date/state";
    id(mqtt_client).publish(stateTopic, datum.c_str(), datum.length(), 0, true);
    ESP_LOGI("CALC", "Published date: %s (Jahr=%d, Monat=%d, Tag=%d)", datum.c_str(), ijahr, imonat, itag);
}

void publishTime()
{
    // Publish discovery (only once - cached)
    publishCalculatedSensorDiscovery(calculatedSensors[1]);

    // Validate that all values have been received
    if (lastStunde < 0 || lastMinute < 0)
    {
        ESP_LOGW("CALC", "Cannot publish time: sensors not initialized (Stunde=%d, Minute=%d, Sekunde=%d)",
                 lastStunde, lastMinute, lastSekunde);
        return;
    }
    if (lastSekunde < 0)
    {
        lastSekunde = 0;
    }

    int istunde = lastStunde;
    int iminute = lastMinute;
    int isekunde = lastSekunde;

    // Log raw values for debugging
    // ESP_LOGD("CALC", "Time values: Stunde=%d, Minute=%d, Sekunde=%d", istunde, iminute, isekunde);

    // Validate ranges
    if (istunde < 0 || istunde > 23 || iminute < 0 || iminute > 59 || isekunde < 0 || isekunde > 59)
    {
        ESP_LOGW("CALC", "Time values out of range: Stunde=%d, Minute=%d, Sekunde=%d", istunde, iminute, isekunde);
        return;
    }

    // Format time directly without helper function
    char zeit[9];
    snprintf(zeit, sizeof(zeit), "%02d:%02d:%02d", istunde, iminute, isekunde);

    // Publish state to MQTT
    const char *stateTopic = "heatingpump/calculated/time/state";
    id(mqtt_client).publish(stateTopic, zeit, strlen(zeit), 0, true);
    ESP_LOGI("CALC", "Published time: %s (Stunde=%d, Minute=%d, Sekunde=%d)", zeit, istunde, iminute, isekunde);
}

void publishBetriebsart(const std::string &sommerBetriebValue)
{
    // Publish discovery (only once - cached)
    publishCalculatedSensorDiscovery(calculatedSensors[2]);

    // Determine Betriebsart based on SOMMERBETRIEB value
    // SOMMERBETRIEB is et_little_bool type, so value is "on" or "off"
    std::string betriebsart;
    std::string icon;

    if (sommerBetriebValue == "on")
    {
        betriebsart = "Sommerbetrieb";
        icon = "mdi:white-balance-sunny";
    }
    else
    {
        betriebsart = "Normalbetrieb";
        icon = "mdi:circle-outline";
    }

    // Publish state to MQTT
    const char *stateTopic = "heatingpump/calculated/betriebsart/state";
    id(mqtt_client).publish(stateTopic, betriebsart.c_str(), betriebsart.length(), 0, true);
    // ESP_LOGD("CALC", "Published Betriebsart: %s (SOMMERBETRIEB=%s)", betriebsart.c_str(), sommerBetriebValue.c_str());
}

// BETRIEBS_STATUS (0x0176) is a bit field. Bit meanings after kr0ner/OneESP32ToRuleThemAll (THZ),
// checked against LWZ 5S Plus values (e.g. 1027 = Heizkreispumpe + Verdichter + Schaltprogramm).
// Bit 2 (Heizen) was never seen set on the LWZ 5S Plus while heating.
static const char *const betriebsStatusBits[16] = {
    "Schaltprogramm aktiv", "Verdichter", "Heizen", "Kühlen",
    "Warmwasserbereitung", "Elektrische Nacherwärmung", "Service", "EVU-Sperre",
    "Filterwechsel", "Lüftung", "Heizkreispumpe", "Abtauen Verdampfer",
    "Filterwechsel Abluft", "Filterwechsel Zuluft", "Aufheizprogramm aktiv", "Bit 15"};

void publishBetriebsStatusText(const std::string &value)
{
    char *endPtr;
    long raw = strtol(value.c_str(), &endPtr, 10);
    if (endPtr == value.c_str() || *endPtr != '\0')
    {
        ESP_LOGW("CALC", "Failed to parse BETRIEBS_STATUS value: %s", value.c_str());
        return;
    }
    uint16_t bits = (uint16_t)(raw & 0xFFFF);

    publishCalculatedSensorDiscovery(calculatedSensors[17]);

    std::string text;
    for (int i = 0; i < 16; i++)
    {
        if (bits & (1u << i))
        {
            if (!text.empty())
                text += ", ";
            text += betriebsStatusBits[i];
        }
    }
    if (text.empty())
        text = "Bereitschaft";

    id(mqtt_client).publish("heatingpump/calculated/betriebsstatus_text/state", text.c_str(), text.length(), 0, true);
}

void publishDeltaTContinuous()
{
    // Publish discovery (only once - cached)
    publishCalculatedSensorDiscovery(calculatedSensors[3]);

    // Check if both temperature values are valid
    if (std::isnan(lastVorlaufIst) || std::isnan(lastRuecklaufIstTemp))
    {
        return; // Silently skip if values not ready
    }

    // Validate temperature range (sanity check)
    if (lastVorlaufIst < -50 || lastRuecklaufIstTemp < -50)
    {
        return; // Silently skip invalid ranges
    }

    float deltaT = lastVorlaufIst - lastRuecklaufIstTemp;

    // Publish state to MQTT
    char value[16];
    snprintf(value, sizeof(value), "%.2f", deltaT);
    const char *stateTopic = "heatingpump/calculated/delta_t_continuous/state";
    id(mqtt_client).publish(stateTopic, value, strlen(value), 0, true);
}

void publishDeltaTRunning()
{
    // Publish discovery (only once - cached)
    publishCalculatedSensorDiscovery(calculatedSensors[4]);

    // Check if compressor is running (value > 2 or not NaN and not 0)
    bool compressorRunning = (!std::isnan(lastVerdichterValue) && lastVerdichterValue > 2.0);

    if (!compressorRunning)
    {
        return; // Silently skip if compressor not running
    }

    // Check if both temperature values are valid
    if (std::isnan(lastVorlaufIst) || std::isnan(lastRuecklaufIstTemp))
    {
        return; // Silently skip if values not ready
    }

    // Validate temperature range (sanity check)
    if (lastVorlaufIst < -50 || lastRuecklaufIstTemp < -50)
    {
        return; // Silently skip invalid ranges
    }

    float deltaT = lastVorlaufIst - lastRuecklaufIstTemp;

    // Publish state to MQTT
    char value[16];
    snprintf(value, sizeof(value), "%.2f", deltaT);
    const char *stateTopic = "heatingpump/calculated/delta_t_running/state";
    id(mqtt_client).publish(stateTopic, value, strlen(value), 0, true);
}

void publishCompressorActive()
{
    // Publish discovery (only once - cached)
    publishCalculatedSensorDiscovery(calculatedSensors[5]);

    // Check if compressor value is valid
    if (std::isnan(lastVerdichterValue))
    {
        return; // Silently skip if value invalid
    }

    // Compressor is active if value > 2
    bool isActive = (lastVerdichterValue > 2.0);

    // Publish state to MQTT
    const char *state = isActive ? "on" : "off";
    const char *stateTopic = "heatingpump/calculated/compressor_active/state";
    id(mqtt_client).publish(stateTopic, state, strlen(state), 0, true);
    // Note: Delta T running is published separately by scheduler
}

// Helper: Check if pattern appears anywhere in text (case-insensitive substring match)
// Get or create cached UID for a signal (eliminates repeated string ops)
inline std::string getOrCreateUID(const CanMember &cm, const char *signalName)
{
    // Computed on every call: a cache (std::string -> std::string) cost ~150 bytes internal heap per signal
    char uid[128];
    snprintf(uid, sizeof(uid), "stiebel_%s_%s", cm.Name, signalName);
    std::string result(uid);
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    std::replace(result.begin(), result.end(), ' ', '_');
    return result;
}

// Publish MQTT Discovery config for a signal
void publishMqttDiscovery(const CanMember &cm, const ElsterIndex *ei)
{

    // Get cached UID (avoids repeated string operations)
    std::string uid = getOrCreateUID(cm, ei->Name);

    // Note: Caller is responsible for checking discoveredSignals and inserting uid
    // This function just publishes the MQTT discovery message

    // Get friendly name: use ei->friendlyName or fallback to ei->Name
    const char *friendlyName = (ei->hasMetadata && ei->friendlyName) ? ei->friendlyName : ei->Name;

    // Get metadata: use ei fields if available, otherwise fall back to type defaults
    const char *component, *deviceClass, *unit, *stateClass, *icon;
    const char *payloadOn = nullptr, *payloadOff = nullptr;

    if (ei->hasMetadata && ei->haComponent)
    {
        // Use metadata from ElsterTable
        component = ei->haComponent;
        deviceClass = ei->haDeviceClass ? ei->haDeviceClass : "";
        unit = ei->unit ? ei->unit : "";
        stateClass = ei->stateClass ? ei->stateClass : "";
        icon = ei->icon ? ei->icon : "";
        payloadOn = ei->payloadOn;
        payloadOff = ei->payloadOff;
    }
    else
    {
        // Fall back to type-based defaults
        getTypeDefaults((ElsterType)ei->Type, component, deviceClass, unit, stateClass, icon);
        // For binary sensors, set default payloads
        if (strcmp(component, "binary_sensor") == 0)
        {
            payloadOn = "on";
            payloadOff = "off";
        }
    }

    // Build discovery topic
    char discoveryTopic[256];
    snprintf(discoveryTopic, sizeof(discoveryTopic),
             "homeassistant/%s/heatingpump/%s/config", component, uid.c_str());

    // Build state topic
    char stateTopic[128];
    snprintf(stateTopic, sizeof(stateTopic), "heatingpump/%s/%s/state", cm.Name, ei->Name);

    // Build JSON payload efficiently using ostringstream
    std::ostringstream payload;
    payload << "{\"name\":\"" << friendlyName << "\","
            << "\"unique_id\":\"" << uid << "\","
            << "\"state_topic\":\"" << stateTopic << "\","
            << "\"availability_topic\":\"heatingpump/status\"";

    // For binary sensors, specify payload values
    if (strcmp(component, "binary_sensor") == 0 && payloadOn && payloadOff)
    {
        payload << ",\"payload_on\":\"" << payloadOn << "\","
                << "\"payload_off\":\"" << payloadOff << "\"";
    }

    // Add optional fields only if non-empty
    if (deviceClass[0] != '\0')
    {
        payload << ",\"device_class\":\"" << deviceClass << "\"";
    }
    if (unit[0] != '\0')
    {
        payload << ",\"unit_of_measurement\":\"" << unit << "\"";
    }
    // State class only for numeric sensors
    if (stateClass[0] != '\0')
    {
        bool isNumericType = (ei->Type == et_dec_val ||
                              ei->Type == et_cent_val ||
                              ei->Type == et_mil_val ||
                              ei->Type == et_byte ||
                              ei->Type == et_double_val ||
                              ei->Type == et_triple_val ||
                              ei->Type == et_little_endian);
        if (isNumericType)
        {
            payload << ",\"state_class\":\"" << stateClass << "\"";
        }
    }
    if (icon[0] != '\0')
    {
        payload << ",\"icon\":\"" << icon << "\"";
    }

    // Device info - create individual device per CAN member as sub-device
    // Main device ID for the heat pump
    const char *mainDeviceId = "stiebel_eltron_" HA_DEVICE_MODEL_STR;

    // Create unique device ID for this CAN member
    char canMemberDeviceId[64];
    snprintf(canMemberDeviceId, sizeof(canMemberDeviceId), "stiebel_%s", cm.Name);

    // Convert CAN member name to friendly German name
    const char *canMemberFriendlyName = cm.Name;
    if (strcmp(cm.Name, "KESSEL") == 0)
        canMemberFriendlyName = "Kessel";
    else if (strcmp(cm.Name, "MANAGER") == 0)
        canMemberFriendlyName = "Manager";
    else if (strcmp(cm.Name, "BEDIENMODUL_2") == 0)
        canMemberFriendlyName = "Bedienmodul";
    else if (strcmp(cm.Name, "HEIZMODUL") == 0)
        canMemberFriendlyName = "Heizmodul";
    else if (strcmp(cm.Name, "FEHLERSPEICHER") == 0)
        canMemberFriendlyName = "Fehlerspeicher";
    else if (strcmp(cm.Name, "MIXER1") == 0)
        canMemberFriendlyName = "Mischer 1";
    else if (strcmp(cm.Name, "MIXER2") == 0)
        canMemberFriendlyName = "Mischer 2";
    else if (strcmp(cm.Name, "WMZ1") == 0)
        canMemberFriendlyName = "Wärmemengenzähler 1";
    else if (strcmp(cm.Name, "WMZ2") == 0)
        canMemberFriendlyName = "Wärmemengenzähler 2";
    else if (strcmp(cm.Name, "OTHER") == 0)
        canMemberFriendlyName = "Sonstige";

    payload << ",\"device\":{\"identifiers\":[\"" << canMemberDeviceId << "\"],"
            << "\"name\":\"" << canMemberFriendlyName << "\","
            << "\"via_device\":\"" << mainDeviceId << "\","
            << "\"manufacturer\":\"Stiebel Eltron\"}}";

    // Publish discovery message with retain flag
    std::string payloadStr = payload.str();
    id(mqtt_client).publish(discoveryTopic, payloadStr.c_str(), payloadStr.length(), 0, true);

    ESP_LOGI("MQTT", "Discovery published for %s", friendlyName);
}

// Republish all MQTT discoveries (for periodic refresh)
void republishAllDiscoveries()
{
    ESP_LOGI("MQTT", "Republishing all MQTT discoveries (%d signals)", discoveredSignals.size());

    // Forget all published sensor discoveries: each signal publishes its discovery again with its next
    // value (blacklisted signals never reach updateSensor, so they are not republished).
    discoveredSignals.clear();

    // Republish all calculated sensor discoveries using unified system
    publishAllCalculatedSensorDiscoveries(true);

    ESP_LOGI("MQTT", "Discovery refresh complete - will republish as signals are received");
}

// Publish signal state to MQTT
void publishMqttState(const CanMember &cm, const ElsterIndex *ei, const std::string &value)
{
    // Input validation
    if (!ei || !ei->Name || value.empty())
    {
        ESP_LOGW("MQTT", "Invalid signal data, skipping state publish");
        return;
    }

    // Build state topic with bounds checking
    char stateTopic[128];
    int written = snprintf(stateTopic, sizeof(stateTopic), "heatingpump/%s/%s/state", cm.Name, ei->Name);
    if (written >= sizeof(stateTopic))
    {
        ESP_LOGW("MQTT", "Topic too long for %s/%s, truncated", cm.Name, ei->Name);
    }

    // Publish state with retain flag
    id(mqtt_client).publish(stateTopic, value.c_str(), value.length(), 0, true);
}

// Diagnostics removed for simplification

// Track which COP values we have valid data for
static std::unordered_map<std::string, float> copEnergyValues;

// Publish MQTT discovery for COP sensors
void publishCOPDiscovery()
{
    static bool discoveryPublished = false;
    if (discoveryPublished)
        return;

    // COP WW
    {
        const char *discoveryTopic = "homeassistant/sensor/heatingpump/cop_ww/config";
        std::ostringstream payload;
        payload << "{\"name\":\"" LNAME_COP_WW "\","
                << "\"unique_id\":\"stiebel_cop_ww\","
                << "\"state_topic\":\"heatingpump/calculated/cop_ww/state\","
                << "\"icon\":\"mdi:water-boiler\","
                << "\"state_class\":\"measurement\","
                << "\"device\":{\"identifiers\":[\"stiebel_eltron_" HA_DEVICE_MODEL_STR "\"],"
                << "\"name\":\"Stiebel Eltron Wärmepumpe\","
                << "\"manufacturer\":\"Stiebel Eltron\"}}";
        std::string payloadStr = payload.str();
        id(mqtt_client).publish(discoveryTopic, payloadStr.c_str(), payloadStr.length(), 0, true);
    }

    // COP Heizung
    {
        const char *discoveryTopic = "homeassistant/sensor/heatingpump/cop_heiz/config";
        std::ostringstream payload;
        payload << "{\"name\":\"" LNAME_COP_HEIZ "\","
                << "\"unique_id\":\"stiebel_cop_heiz\","
                << "\"state_topic\":\"heatingpump/calculated/cop_heiz/state\","
                << "\"icon\":\"mdi:radiator\","
                << "\"state_class\":\"measurement\","
                << "\"device\":{\"identifiers\":[\"stiebel_eltron_" HA_DEVICE_MODEL_STR "\"],"
                << "\"name\":\"Stiebel Eltron Wärmepumpe\","
                << "\"manufacturer\":\"Stiebel Eltron\"}}";
        std::string payloadStr = payload.str();
        id(mqtt_client).publish(discoveryTopic, payloadStr.c_str(), payloadStr.length(), 0, true);
    }

    // COP Gesamt
    {
        const char *discoveryTopic = "homeassistant/sensor/heatingpump/cop_gesamt/config";
        std::ostringstream payload;
        payload << "{\"name\":\"" LNAME_COP_GESAMT "\","
                << "\"unique_id\":\"stiebel_cop_gesamt\","
                << "\"state_topic\":\"heatingpump/calculated/cop_gesamt/state\","
                << "\"icon\":\"mdi:chart-line\","
                << "\"state_class\":\"measurement\","
                << "\"device\":{\"identifiers\":[\"stiebel_eltron_" HA_DEVICE_MODEL_STR "\"],"
                << "\"name\":\"Stiebel Eltron Wärmepumpe\","
                << "\"manufacturer\":\"Stiebel Eltron\"}}";
        std::string payloadStr = payload.str();
        id(mqtt_client).publish(discoveryTopic, payloadStr.c_str(), payloadStr.length(), 0, true);
    }

    discoveryPublished = true;
    ESP_LOGI("MQTT", "Discovery published for COP sensors");
}

// Store energy value when received for COP calculation
void storeCOPEnergyValue(const char *signalName, const std::string &value)
{
    // Validate the string contains a valid number (no exceptions available)
    if (value.empty())
    {
        ESP_LOGW("COP", "Empty value for %s", signalName);
        return;
    }

    // Check for valid numeric characters
    bool hasDigit = false;
    bool isValid = true;
    for (size_t i = 0; i < value.length(); i++)
    {
        char c = value[i];
        if (c >= '0' && c <= '9')
        {
            hasDigit = true;
        }
        else if (c != '.' && c != '-' && c != '+' && c != ' ')
        {
            isValid = false;
            break;
        }
    }

    if (!hasDigit || !isValid)
    {
        ESP_LOGW("COP", "Invalid numeric value for %s: %s", signalName, value.c_str());
        return;
    }

    // Parse the value (std::stof may still fail but won't throw in this context)
    float fval = std::atof(value.c_str());
    copEnergyValues[signalName] = fval;
    ESP_LOGD("COP", "Stored %s = %.3f", signalName, fval);
}

// The heat pump splits every energy counter into a MWh part (<base>_SUM_MWH) and a kWh part
// (<base>_SUM_KWH, 0..999). Returns the combined value in MWh; false until both parts were received.
static bool copEnergyMWh(const char *base, float &mwh)
{
    std::string key(base);
    auto m = copEnergyValues.find(key + "_SUM_MWH");
    auto k = copEnergyValues.find(key + "_SUM_KWH");
    if (m == copEnergyValues.end() || k == copEnergyValues.end())
        return false;
    mwh = m->second + k->second / 1000.0f;
    return true;
}

static void publishCOP(const char *stateTopic, const char *label, float waerme, float el)
{
    if (el <= 0.001f) // Avoid division by zero
        return;
    float cop = waerme / el;
    char valueStr[16];
    snprintf(valueStr, sizeof(valueStr), "%.2f", cop);
    id(mqtt_client).publish(stateTopic, valueStr, strlen(valueStr), 0, true);
    ESP_LOGI("COP", "COP %s: %.2f (Wärme: %.3f MWh, El: %.3f MWh)", label, cop, waerme, el);
}

// Calculate and publish COP values if all required data is available
void updateCOPCalculations()
{
    publishCOPDiscovery();

    float wWw, w2Ww, elWw, wHeiz, w2Heiz, elHeiz;
    bool ww = copEnergyMWh("WAERMEERTRAG_WW", wWw) && copEnergyMWh("WAERMEERTRAG_2WE_WW", w2Ww) &&
              copEnergyMWh("EL_AUFNAHMELEISTUNG_WW", elWw);
    bool heiz = copEnergyMWh("WAERMEERTRAG_HEIZ", wHeiz) && copEnergyMWh("WAERMEERTRAG_2WE_HEIZ", w2Heiz) &&
                copEnergyMWh("EL_AUFNAHMELEISTUNG_HEIZ", elHeiz);

    // COP WW: (WAERMEERTRAG_WW_SUM + WAERMEERTRAG_2WE_WW_SUM) / EL_AUFNAHMELEISTUNG_WW_SUM
    if (ww)
        publishCOP("heatingpump/calculated/cop_ww/state", "WW", wWw + w2Ww, elWw);

    // COP Heizung: (WAERMEERTRAG_HEIZ_SUM + WAERMEERTRAG_2WE_HEIZ_SUM) / EL_AUFNAHMELEISTUNG_HEIZ_SUM
    if (heiz)
        publishCOP("heatingpump/calculated/cop_heiz/state", "Heizung", wHeiz + w2Heiz, elHeiz);

    // COP Gesamt: (all WAERMEERTRAG) / (all EL_AUFNAHMELEISTUNG)
    if (ww && heiz)
        publishCOP("heatingpump/calculated/cop_gesamt/state", "Gesamt", wWw + w2Ww + wHeiz + w2Heiz, elWw + elHeiz);
}

// Validation removed for simplification - all values are published as-is
// Invalid values like -255, -32768, etc. will be visible in Home Assistant
// Users can filter these in HA automations/dashboards if needed

// ============================================================================
// ESPHOME IO ADAPTER FOR SG READY CONTROLLER
// ============================================================================

class EspHomeSgReadyIO : public ISgReadyIO
{
public:
    static ESPPreferenceObject nvsDhw;
    static ESPPreferenceObject nvsRoom;
    static ESPPreferenceObject nvsActive;

    void writeCanSignal(const char *signalName, const char *value) override
    {
        writeSignal(&CanMembers[cm_kessel], signalName, value);
        delay(100);
        readSignal(&CanMembers[cm_kessel], signalName);
    }

    void publishMqtt(const char *topic, const char *value) override
    {
        id(mqtt_client).publish(topic, value, strlen(value), 0, true);
    }

    void saveBoostState(float dhw, float room, bool active) override
    {
        nvsDhw.save(&dhw);
        nvsRoom.save(&room);
        nvsActive.save(&active);
        ESP_LOGD("NVS", "Saved boost state: DHW=%.1f, Room=%.1f, active=%d",
                 dhw, room, (int)active);
    }

    bool loadBoostState(float &dhw, float &room, bool &active) override
    {
        bool ok = nvsActive.load(&active);
        nvsDhw.load(&dhw);
        nvsRoom.load(&room);
        return ok;
    }
};

ESPPreferenceObject EspHomeSgReadyIO::nvsDhw;
ESPPreferenceObject EspHomeSgReadyIO::nvsRoom;
ESPPreferenceObject EspHomeSgReadyIO::nvsActive;

static EspHomeSgReadyIO sgReadyIO;
static SgReadyController sgReadyController(sgReadyIO);

// Called from on_boot lambda in common.yaml
void initBoostStatePrefs()
{
    EspHomeSgReadyIO::nvsDhw = global_preferences->make_preference<float>(0xB007BA5E, true);
    EspHomeSgReadyIO::nvsRoom = global_preferences->make_preference<float>(0xB007BA5F, true);
    EspHomeSgReadyIO::nvsActive = global_preferences->make_preference<bool>(0xB007BA60, true);
    ESP_LOGI("NVS", "Boost state preferences initialized");
}

// Called from on_boot lambda in common.yaml
void loadBoostState()
{
    sgReadyController.loadFromNvs();
    currentSgReadyState = sgReadyController.currentState();
    ESP_LOGI("NVS", "Boost state loaded, active=%d", (int)sgReadyController.isActive());
}

// Called from MQTT on_message lambda in common.yaml
void applySgReadyState(int state)
{
    ESP_LOGI("SG_READY", "Applying SG Ready state %d", state);
    if (sgReadyController.applyState(state))
    {
        currentSgReadyState = sgReadyController.currentState();
    }
    else
    {
        ESP_LOGW("SG_READY", "Invalid SG Ready state: %d", state);
    }
}

// Accessor helpers used by YAML sensor lambdas and MQTT on_connect block
inline float sgReadyBoostState3Value() { return sgReadyController.boostState3(); }
inline float sgReadyBoostState4Value() { return sgReadyController.boostState4(); }
inline void setSgReadyBoost3(float v) { sgReadyController.setBoostState3(v); }
inline void setSgReadyBoost4(float v) { sgReadyController.setBoostState4(v); }

// Retained states of the SG Ready MQTT controls (select + two numbers). Returns false if the MQTT
// client could not queue a message, the caller retries later.
bool publishSgReadyStates()
{
    char buf[16];
    if (currentSgReadyState >= 1 && currentSgReadyState <= 4)
    {
        const char *opt = sgReadyOptions[currentSgReadyState - 1];
        if (!id(mqtt_client).publish("heatingpump/KESSEL/SG_READY_STATE/state", opt, strlen(opt), 0, true))
            return false;
    }
    snprintf(buf, sizeof(buf), "%.1f", sgReadyBoostState3Value());
    if (!id(mqtt_client).publish("heatingpump/KESSEL/SG_READY_BOOST_STATE3/state", buf, strlen(buf), 0, true))
        return false;
    snprintf(buf, sizeof(buf), "%.1f", sgReadyBoostState4Value());
    if (!id(mqtt_client).publish("heatingpump/KESSEL/SG_READY_BOOST_STATE4/state", buf, strlen(buf), 0, true))
        return false;
    ESP_LOGI("SG_READY", "SG Ready control states published");
    return true;
}

// ============================================================================
// COMPILE-TIME STRING HASHING FOR FAST SIGNAL DISPATCH
// ============================================================================

// Compile-time DJB2 hash function (constexpr for C++11 compatibility)
constexpr uint32_t hash_impl(const char *str, uint32_t hash = 5381)
{
    return (*str == '\0') ? hash : hash_impl(str + 1, ((hash << 5) + hash) + static_cast<uint32_t>(*str));
}

constexpr uint32_t hash(const char *str)
{
    return hash_impl(str);
}

// Runtime hash for signal names (inline for performance)
inline uint32_t hash_runtime(const char *str)
{
    uint32_t h = 5381;
    while (*str)
    {
        h = ((h << 5) + h) + static_cast<uint32_t>(*str);
        str++;
    }
    return h;
}

// Pre-computed compile-time hashes for all monitored signals
constexpr uint32_t HASH_JAHR = hash("JAHR");
constexpr uint32_t HASH_MONAT = hash("MONAT");
constexpr uint32_t HASH_TAG = hash("TAG");
constexpr uint32_t HASH_STUNDE = hash("STUNDE");
constexpr uint32_t HASH_MINUTE = hash("MINUTE");
constexpr uint32_t HASH_VERDICHTERSTARTS1 = hash("VERDICHTERSTARTS1");
constexpr uint32_t HASH_VERDICHTERSTARTS1000 = hash("VERDICHTERSTARTS1000");
constexpr uint32_t HASH_SEKUNDE = hash("SEKUNDE");
constexpr uint32_t HASH_SOMMERBETRIEB = hash("SOMMERBETRIEB");
constexpr uint32_t HASH_SAMMLERISTTEMP = hash("SAMMLERISTTEMP");
constexpr uint32_t HASH_RUECKLAUFISTTEMP = hash("RUECKLAUFISTTEMP");
constexpr uint32_t HASH_VERDICHTER = hash("VERDICHTER");
constexpr uint32_t HASH_BETRIEBS_STATUS = hash("BETRIEBS_STATUS");
constexpr uint32_t HASH_EL_AUFNAHMELEISTUNG_HEIZ = hash("EL_AUFNAHMELEISTUNG_HEIZ_SUM_MWH");
constexpr uint32_t HASH_EL_AUFNAHMELEISTUNG_WW = hash("EL_AUFNAHMELEISTUNG_WW_SUM_MWH");
constexpr uint32_t HASH_WAERMEERTRAG_2WE_WW = hash("WAERMEERTRAG_2WE_WW_SUM_MWH");
constexpr uint32_t HASH_WAERMEERTRAG_2WE_HEIZ = hash("WAERMEERTRAG_2WE_HEIZ_SUM_MWH");
constexpr uint32_t HASH_WAERMEERTRAG_WW = hash("WAERMEERTRAG_WW_SUM_MWH");
constexpr uint32_t HASH_WAERMEERTRAG_HEIZ = hash("WAERMEERTRAG_HEIZ_SUM_MWH");
constexpr uint32_t HASH_EL_AUFNAHMELEISTUNG_HEIZ_KWH = hash("EL_AUFNAHMELEISTUNG_HEIZ_SUM_KWH");
constexpr uint32_t HASH_EL_AUFNAHMELEISTUNG_WW_KWH = hash("EL_AUFNAHMELEISTUNG_WW_SUM_KWH");
constexpr uint32_t HASH_WAERMEERTRAG_2WE_WW_KWH = hash("WAERMEERTRAG_2WE_WW_SUM_KWH");
constexpr uint32_t HASH_WAERMEERTRAG_2WE_HEIZ_KWH = hash("WAERMEERTRAG_2WE_HEIZ_SUM_KWH");
constexpr uint32_t HASH_WAERMEERTRAG_WW_KWH = hash("WAERMEERTRAG_WW_SUM_KWH");
constexpr uint32_t HASH_WAERMEERTRAG_HEIZ_KWH = hash("WAERMEERTRAG_HEIZ_SUM_KWH");

// ---- Compressor starts (VERDICHTERSTARTS1000 * 1000 + VERDICHTERSTARTS1), published on the KESSEL device ----
static long lastVerdichterStarts1 = -1;
static long lastVerdichterStarts1000 = -1;
// Both parts are polled separately, so around a roll-over (x999 -> (x+1)000) one part can be newer
// than the other: old high + wrapped low gives -1000, new high + old low 999 gives +999. A part
// that changed in a roll-over-relevant way therefore waits until the other part was read again.
static bool verdichterStartsNeedLow = true;  // high part changed (or first read) -> wait for a fresh low part
static bool verdichterStartsNeedHigh = true; // low part wrapped (or first read) -> wait for a fresh high part

static void updateCompressorStartsPart(bool isHighPart, long v)
{
    if (isHighPart)
    {
        if (v != lastVerdichterStarts1000)
            verdichterStartsNeedLow = true;
        verdichterStartsNeedHigh = false;
        lastVerdichterStarts1000 = v;
    }
    else
    {
        if (lastVerdichterStarts1 < 0 || v < lastVerdichterStarts1)
            verdichterStartsNeedHigh = true;
        verdichterStartsNeedLow = false;
        lastVerdichterStarts1 = v;
    }
}

static void publishCompressorStartsTotal()
{
    if (lastVerdichterStarts1 < 0 || lastVerdichterStarts1000 < 0)
        return;
    if (verdichterStartsNeedLow || verdichterStartsNeedHigh)
        return;
    long total = lastVerdichterStarts1000 * 1000 + lastVerdichterStarts1;

    const CanMember *cm = &CanMembers[cm_kessel];
    if (!verdichterStartsDiscoveryDone)
    {
        std::string uid = writableEntityUid(cm, "VERDICHTERSTARTS_GESAMT");
        std::ostringstream payload;
        payload << "{\"name\":\"" << LNAME_CALC_VERDICHTERSTARTS << "\","
                << "\"unique_id\":\"" << uid << "\","
                << "\"state_topic\":\"heatingpump/" << cm->Name << "/VERDICHTERSTARTS_GESAMT/state\","
                << "\"availability_topic\":\"heatingpump/status\","
                << "\"state_class\":\"total_increasing\",\"icon\":\"mdi:counter\"";
        appendMemberDeviceAndClose(payload, cm);
        if (!publishDiscoveryMessage("homeassistant/sensor/heatingpump/" + uid + "/config", payload.str()))
            return;
        verdichterStartsDiscoveryDone = true;
    }

    char buf[16];
    snprintf(buf, sizeof(buf), "%ld", total);
    id(mqtt_client).publish("heatingpump/KESSEL/VERDICHTERSTARTS_GESAMT/state", buf, strlen(buf), 0, true);
}
void updateSensor(const CanMember &cm, const ElsterIndex *ei, const std::string &value)
{
    // Skip empty values
    if (value.empty())
    {
        return;
    }

    // Invert boolean logic for EVU_SPERRE_AKTIV signal
    // CAN: 1 = lock inactive (off), 0 = lock active (on)
    // MQTT: "off" = lock inactive, "on" = lock active
    std::string publishValue = value;
    if (strcmp(ei->Name, "EVU_SPERRE_AKTIV") == 0)
    {
        if (value == "on")
        {
            publishValue = "off";
        }
        else if (value == "off")
        {
            publishValue = "on";
        }
    }

    // Unused time domains (switching programs) are shown as "---" and written back the same way
    if ((ei->Type == et_time_domain || ei->Type == et_time_domain_le) && (value == "not used time domain" || value == "-255"))
    {
        publishValue = "---";
    }

    // Selects with a value table publish the option text instead of the raw CAN value
    if (const char *selectLabel = translateWritableSelectState(cm, ei->Name, value))
    {
        publishValue = selectLabel;
    }

    // Check if discovery is needed
    uint32_t key = discoveryKey(cm, ei);
    bool needsDiscovery = (discoveredSignals.find(key) == discoveredSignals.end());

    if (needsDiscovery)
    {
        // Mark as discovered and publish discovery immediately
        discoveredSignals.insert(key);
        publishMqttDiscovery(cm, ei);
    }

    // Publish state
    publishMqttState(cm, ei, publishValue);

    // Fast signal dispatch using compile-time hash (O(1) switch/jump table)
    const char *signalName = ei->Name;
    uint32_t signalHash = hash_runtime(signalName);

    switch (signalHash)
    {
    case HASH_JAHR:
    {
        char *endPtr;
        long intValue = strtol(value.c_str(), &endPtr, 10);
        if (endPtr != value.c_str() && *endPtr == '\0')
        {
            lastJahr = static_cast<int>(intValue);
            tsJahr = millis();
            // ESP_LOGD("CALC", "Updated JAHR: %d", lastJahr);
        }
        break;
    }

    case HASH_MONAT:
    {
        char *endPtr;
        long intValue = strtol(value.c_str(), &endPtr, 10);
        if (endPtr != value.c_str() && *endPtr == '\0')
        {
            lastMonat = static_cast<int>(intValue);
            tsMonat = millis();
            // ESP_LOGD("CALC", "Updated MONAT: %d", lastMonat);
        }
        break;
    }

    case HASH_TAG:
    {
        char *endPtr;
        long intValue = strtol(value.c_str(), &endPtr, 10);
        if (endPtr != value.c_str() && *endPtr == '\0')
        {
            lastTag = static_cast<int>(intValue);
            tsTag = millis();
            // ESP_LOGD("CALC", "Updated TAG: %d", lastTag);
            publishDate();
        }
        break;
    }

    case HASH_STUNDE:
    {
        char *endPtr;
        long intValue = strtol(value.c_str(), &endPtr, 10);
        if (endPtr != value.c_str() && *endPtr == '\0')
        {
            lastStunde = static_cast<int>(intValue);
            tsStunde = millis();
            // ESP_LOGD("CALC", "Updated STUNDE: %d", lastStunde);
        }
        break;
    }

    case HASH_VERDICHTERSTARTS1:
    case HASH_VERDICHTERSTARTS1000:
    {
        char *endPtr;
        long v = strtol(value.c_str(), &endPtr, 10);
        if (endPtr != value.c_str() && *endPtr == '\0' && strcmp(cm.Name, "KESSEL") == 0)
        {
            updateCompressorStartsPart(signalHash == HASH_VERDICHTERSTARTS1000, v);
            publishCompressorStartsTotal();
        }
        break;
    }
    case HASH_MINUTE:
    {
        char *endPtr;
        long intValue = strtol(value.c_str(), &endPtr, 10);
        if (endPtr != value.c_str() && *endPtr == '\0')
        {
            lastMinute = static_cast<int>(intValue);
            tsMinute = millis();
            // ESP_LOGD("CALC", "Updated MINUTE: %d", lastMinute);
            publishTime();
        }
        break;
    }

    case HASH_SEKUNDE:
    {
        char *endPtr;
        long intValue = strtol(value.c_str(), &endPtr, 10);
        if (endPtr != value.c_str() && *endPtr == '\0')
        {
            lastSekunde = static_cast<int>(intValue);
            // ESP_LOGD("CALC", "Updated SEKUNDE: %d", lastSekunde);
        }
        break;
    }

    case HASH_SOMMERBETRIEB:
        publishBetriebsart(value);
        break;

    case HASH_BETRIEBS_STATUS:
        publishBetriebsStatusText(value);
        break;

    case HASH_SAMMLERISTTEMP:
    {
        char *endPtr;
        float floatValue = strtof(value.c_str(), &endPtr);
        if (endPtr != value.c_str() && *endPtr == '\0')
        {
            lastVorlaufIst = floatValue;
            // Delta T will be calculated and published by scheduler
        }
        else
        {
            ESP_LOGW("CALC", "Failed to parse SAMMLERISTTEMP value: %s", value.c_str());
        }
        break;
    }

    case HASH_RUECKLAUFISTTEMP:
    {
        char *endPtr;
        float floatValue = strtof(value.c_str(), &endPtr);
        if (endPtr != value.c_str() && *endPtr == '\0')
        {
            lastRuecklaufIstTemp = floatValue;
            // Delta T will be calculated and published by scheduler
        }
        else
        {
            ESP_LOGW("CALC", "Failed to parse RUECKLAUFISTTEMP value: %s", value.c_str());
        }
        break;
    }

    case HASH_VERDICHTER:
    {
        char *endPtr;
        float floatValue = strtof(value.c_str(), &endPtr);
        if (endPtr != value.c_str() && *endPtr == '\0')
        {
            lastVerdichterValue = floatValue;
            // Compressor state will be calculated and published by scheduler
        }
        else
        {
            ESP_LOGW("CALC", "Failed to parse VERDICHTER value: %s", value.c_str());
        }
        break;
    }

    case HASH_EL_AUFNAHMELEISTUNG_HEIZ:
    case HASH_EL_AUFNAHMELEISTUNG_WW:
    case HASH_WAERMEERTRAG_2WE_WW:
    case HASH_WAERMEERTRAG_2WE_HEIZ:
    case HASH_WAERMEERTRAG_WW:
    case HASH_WAERMEERTRAG_HEIZ:
    case HASH_EL_AUFNAHMELEISTUNG_HEIZ_KWH:
    case HASH_EL_AUFNAHMELEISTUNG_WW_KWH:
    case HASH_WAERMEERTRAG_2WE_WW_KWH:
    case HASH_WAERMEERTRAG_2WE_HEIZ_KWH:
    case HASH_WAERMEERTRAG_WW_KWH:
    case HASH_WAERMEERTRAG_HEIZ_KWH:
        storeCOPEnergyValue(signalName, value);
        updateCOPCalculations();
        break;

    default:
        // Signal not monitored for calculated sensors - no action needed
        break;
    }
}

// Helper function to generate random number in range [min, max) using ESP32 hardware RNG
// esp_random() uses hardware entropy sources and doesn't need seeding
unsigned long getRandomInRange(unsigned long min, unsigned long max)
{
    if (min >= max)
        return min;
    unsigned long range = max - min;
    return min + (esp_random() % range);
}

void publishCanDiagnostics()
{
    twai_status_info_t status;
    if (twai_get_status_info(&status) != ESP_OK)
        return;

    if (status.state == TWAI_STATE_BUS_OFF)
    {
        ESP_LOGW("CAN_RECOVERY", "TWAI bus-off detected, initiating recovery");
        twai_initiate_recovery();
    }
    else if (status.state == TWAI_STATE_STOPPED)
    {
        // twai_initiate_recovery() leaves the controller STOPPED once recovery
        // completes (128 bus-free occurrences) — must explicitly restart.
        // STOPPED here always means "post-recovery": the boot-time STOPPED
        // window (before twai_start_v2() in setup_internal()) completes
        // long before this 30s-cadence diagnostic loop ever runs.
        ESP_LOGW("CAN_RECOVERY", "TWAI stopped after recovery, restarting");
        twai_start();
    }

    // Publish discovery (only once per UID — cached internally)
    publishCalculatedSensorDiscovery(calculatedSensors[6]);
    publishCalculatedSensorDiscovery(calculatedSensors[7]);
    publishCalculatedSensorDiscovery(calculatedSensors[8]);
    publishCalculatedSensorDiscovery(calculatedSensors[9]);

    char buf[16];

    snprintf(buf, sizeof(buf), "%lu", (unsigned long)status.tx_error_counter);
    id(mqtt_client).publish("heatingpump/calculated/can_tec/state", buf, strlen(buf), 0, true);

    snprintf(buf, sizeof(buf), "%lu", (unsigned long)status.rx_error_counter);
    id(mqtt_client).publish("heatingpump/calculated/can_rec/state", buf, strlen(buf), 0, true);

    snprintf(buf, sizeof(buf), "%lu", (unsigned long)status.bus_error_count);
    id(mqtt_client).publish("heatingpump/calculated/can_bus_errors/state", buf, strlen(buf), 0, true);

    const char *stateStr;
    switch (status.state)
    {
    case TWAI_STATE_STOPPED:
        stateStr = "Gestoppt";
        break;
    case TWAI_STATE_RUNNING:
        stateStr = "Läuft";
        break;
    case TWAI_STATE_BUS_OFF:
        stateStr = "Bus-Off";
        break;
    case TWAI_STATE_RECOVERING:
        stateStr = "Erholung";
        break;
    default:
        stateStr = "Unbekannt";
        break;
    }
    id(mqtt_client).publish("heatingpump/calculated/can_state/state", stateStr, strlen(stateStr), 0, true);
}

// ============================================================================
// HEAT PUMP AVAILABILITY
// Available = heat pump sends CAN messages, its clock is plausible and it answers our writes.
// ============================================================================

// Current local time of Home Assistant in minutes (comparable with daysFromCivil). false if the clock is not set.
static bool currentLocalMinutes(long long &minutes)
{
    auto t = id(time_source).now();
    if (!t.is_valid())
        return false;
    minutes = daysFromCivil(t.year, t.month, t.day_of_month) * 1440LL + t.hour * 60 + t.minute;
    return true;
}

// Heat pump clock minus real time in minutes, extrapolated to "now". false if not (yet) comparable.
static bool heatPumpClockDeviation(unsigned long now, long &deviationMin)
{
    if (lastJahr < 0 || lastMonat < 1 || lastTag < 1 || lastStunde < 0 || lastMinute < 0)
        return false;
    if (tsJahr == 0 || tsMonat == 0 || tsTag == 0 || tsStunde == 0 || tsMinute == 0)
        return false;

    long long realMinutes;
    if (!currentLocalMinutes(realMinutes))
        return false;

    // The values were polled at different times: advance the heat pump clock by the age of the oldest sample
    unsigned long oldest = std::min({tsJahr, tsMonat, tsTag, tsStunde, tsMinute});
    long long elapsedMin = (long long)((now - oldest) / 60000UL);

    long long pumpMinutes = daysFromCivil(2000 + lastJahr, lastMonat, lastTag) * 1440LL + lastStunde * 60 + lastMinute + elapsedMin;
    deviationMin = (long)(pumpMinutes - realMinutes);
    return true;
}

// Returns true if the heat pump is considered available. reason describes the first problem found.
static bool evaluateAvailability(unsigned long now, std::string &reason)
{
    char buf[96];

    // 1) CAN liveness
    if (!kesselRxSeen)
    {
        // Grace period after start: first requests are sent after STARTUP_DELAY_MS
        if (now < (unsigned long)STARTUP_DELAY_MS + AVAILABILITY_CAN_TIMEOUT_S * 1000UL)
        {
            reason = "Warte auf erste CAN-Antwort";
            return false;
        }
        reason = "Keine CAN-Antwort seit dem Start";
        return false;
    }
    unsigned long silentS = (now - lastKesselRxMs) / 1000UL;
    if (silentS > AVAILABILITY_CAN_TIMEOUT_S)
    {
        snprintf(buf, sizeof(buf), "Keine CAN-Antwort seit %lu s", silentS);
        reason = buf;
        return false;
    }

    // 2) Answers to our writes
    for (int i = 0; i < 8; i++)
    {
        if (!pendingWrites[i].active)
            continue;
        unsigned long ageS = (now - pendingWrites[i].sentMs) / 1000UL;
        if (ageS > AVAILABILITY_WRITE_ACK_EXPIRE_S)
        {
            pendingWrites[i].active = false; // forget very old writes
            continue;
        }
        if (ageS > AVAILABILITY_WRITE_ACK_TIMEOUT_S)
        {
            snprintf(buf, sizeof(buf), "Schreibbefehl ohne Antwort: %s", pendingWrites[i].signalName);
            reason = buf;
            return false;
        }
    }

    // 3) Heat pump clock
    long deviationMin = 0;
    if (heatPumpClockDeviation(now, deviationMin))
    {
        lastClockDeviationMin = deviationMin;
        if (deviationMin > AVAILABILITY_CLOCK_TOLERANCE_MIN || deviationMin < -AVAILABILITY_CLOCK_TOLERANCE_MIN)
        {
            if (!clockMismatchActive)
            {
                clockMismatchActive = true;
                clockMismatchSinceMs = now;
            }
            if ((now - clockMismatchSinceMs) / 1000UL > AVAILABILITY_CLOCK_CONFIRM_S)
            {
                snprintf(buf, sizeof(buf), "Uhrzeit der Wärmepumpe weicht um %ld min ab", deviationMin);
                reason = buf;
                return false;
            }
        }
        else
        {
            clockMismatchActive = false;
        }
    }

    reason = "OK";
    return true;
}

void publishAvailability()
{
    // Publish discovery (only once - cached)
    publishCalculatedSensorDiscovery(calculatedSensors[10]);
    publishCalculatedSensorDiscovery(calculatedSensors[11]);

    unsigned long now = millis();
    std::string reason;
    bool available = evaluateAvailability(now, reason);

    // Publish on change and at least every 5 minutes (retained messages survive a broker restart)
    bool changed = (reason != lastAvailabilityStatus);
    if (!changed && lastAvailabilityPublishMs != 0 && now - lastAvailabilityPublishMs < 300000UL)
        return;

    const char *state = available ? "on" : "off";
    id(mqtt_client).publish("heatingpump/calculated/heatpump_available/state", state, strlen(state), 0, true);
    id(mqtt_client).publish("heatingpump/calculated/heatpump_availability_status/state", reason.c_str(), reason.length(), 0, true);
    lastAvailabilityStatus = reason;
    lastAvailabilityPublishMs = now;
    if (changed)
        ESP_LOGI("AVAILABILITY", "Heat pump %s: %s", available ? "available" : "NOT available", reason.c_str());
}
// ============================================================================
// SELF HEALING AND SYSTEM DIAGNOSTICS
// ============================================================================

// Restart the TWAI controller (stop + start). Returns true if the restart was issued.
static bool restartCanController()
{
    esp_err_t stopErr = twai_stop();
    esp_err_t startErr = twai_start();
    canRestartCount++;
    lastCanRestartMs = millis();
    ESP_LOGW("SELF_HEAL", "CAN controller restarted (#%lu, stop=%d, start=%d)", (unsigned long)canRestartCount, (int)stopErr, (int)startErr);
    return startErr == ESP_OK;
}

// Checks the heat pump liveness and the heap. Restarts the CAN controller on a long silence and
// requests a reboot (selfHealRebootRequested()) if the silence lasts or the heap stays too low.
void processSelfHealing()
{
    unsigned long now = millis();

    if (kesselRxSeen)
    {
        unsigned long silentS = (now - lastKesselRxMs) / 1000UL;

        if (silentS > SELF_HEAL_REBOOT_S)
        {
            ESP_LOGE("SELF_HEAL", "Heat pump silent for %lu s - requesting reboot", silentS);
            selfHealRebootPending = true;
            return;
        }
        if (silentS > SELF_HEAL_CAN_RESTART_S &&
            (lastCanRestartMs == 0 || now - lastCanRestartMs > SELF_HEAL_CAN_RESTART_S * 1000UL))
        {
            ESP_LOGW("SELF_HEAL", "Heat pump silent for %lu s - restarting CAN controller", silentS);
            restartCanController();
        }
    }
    else if (now > (unsigned long)STARTUP_DELAY_MS + SELF_HEAL_CAN_RESTART_S * 1000UL &&
             (lastCanRestartMs == 0 || now - lastCanRestartMs > SELF_HEAL_CAN_RESTART_S * 1000UL))
    {
        // Never heard from the heat pump since boot: retry the CAN controller, but never reboot
        ESP_LOGW("SELF_HEAL", "No CAN message since boot - restarting CAN controller");
        restartCanController();
    }

    // Heap: reboot if the internal heap stays low (fragmentation / leak)
    size_t freeInternal = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    if (now / 1000UL > SELF_HEAL_MIN_UPTIME_S && freeInternal < SELF_HEAL_LOW_HEAP_B)
    {
        if (lowHeapSinceMs == 0)
            lowHeapSinceMs = now;
        if ((now - lowHeapSinceMs) / 1000UL > SELF_HEAL_LOW_HEAP_CONFIRM_S)
        {
            ESP_LOGE("SELF_HEAL", "Free internal heap %u B below %u B for %lu s - requesting reboot",
                     (unsigned)freeInternal, (unsigned)SELF_HEAL_LOW_HEAP_B, (now - lowHeapSinceMs) / 1000UL);
            selfHealRebootPending = true;
        }
    }
    else
    {
        lowHeapSinceMs = 0;
    }
}

// True if self healing wants the ESP to reboot (polled from the interval in common.yaml)
bool selfHealRebootRequested()
{
    return selfHealRebootPending;
}

void publishSystemDiagnostics()
{
    for (int i = 12; i <= 16; i++)
        publishCalculatedSensorDiscovery(calculatedSensors[i]);

    char buf[24];
    snprintf(buf, sizeof(buf), "%u", (unsigned)heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    id(mqtt_client).publish("heatingpump/calculated/heap_free/state", buf, strlen(buf), 0, true);

    snprintf(buf, sizeof(buf), "%u", (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL));
    id(mqtt_client).publish("heatingpump/calculated/heap_largest_block/state", buf, strlen(buf), 0, true);

    snprintf(buf, sizeof(buf), "%u", (unsigned)heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL)); // internal RAM only (PSRAM would hide shortages)
    id(mqtt_client).publish("heatingpump/calculated/heap_min_free/state", buf, strlen(buf), 0, true);

    snprintf(buf, sizeof(buf), "%.2f", millis() / 3600000.0f); // hours
    id(mqtt_client).publish("heatingpump/calculated/uptime/state", buf, strlen(buf), 0, true);

    snprintf(buf, sizeof(buf), "%lu", (unsigned long)canRestartCount);
    id(mqtt_client).publish("heatingpump/calculated/can_restarts/state", buf, strlen(buf), 0, true);
}
// Process calculated sensor updates with frequency-based scheduling
// This function should be called regularly from the main loop
void processCalculatedSensors()
{
    unsigned long now = millis();

    // Initialize scheduled update times on first run (with random offsets to spread load)
    static bool initialized = false;
    if (!initialized && requestManagerStarted)
    {
        // Initialize all calculated sensor schedules with random offsets
        ESP_LOGI("CALC_SCHED", "Initializing calculated sensor schedules with random offsets");

        // Delta T sensors (30s frequency)
        unsigned long deltaT_interval = CALC_DELTA_T_FREQUENCY * 1000UL;
        nextDeltaTUpdate = now + getRandomInRange(0, deltaT_interval + 1);

        // Compressor sensor (30s frequency)
        unsigned long compressor_interval = CALC_COMPRESSOR_FREQUENCY * 1000UL;
        nextCompressorUpdate = now + getRandomInRange(0, compressor_interval + 1);

        // Date/Time sensors (1min frequency)
        unsigned long datetime_interval = CALC_DATETIME_FREQUENCY * 1000UL;
        nextDateTimeUpdate = now + getRandomInRange(0, datetime_interval + 1);

        // Betriebsart sensor (1min frequency)
        unsigned long betriebsart_interval = CALC_BETRIEBSART_FREQUENCY * 1000UL;
        nextBetriebsartUpdate = now + getRandomInRange(0, betriebsart_interval + 1);

        // CAN diagnostics (30s frequency)
        unsigned long can_diag_interval = CALC_CAN_DIAG_FREQUENCY * 1000UL;
        nextCanDiagUpdate = now + getRandomInRange(0, can_diag_interval + 1);

        // Availability (10s frequency)
        nextAvailabilityUpdate = now + getRandomInRange(0, CALC_AVAILABILITY_FREQUENCY * 1000UL + 1);

        // System diagnostics (1min frequency)
        nextSystemDiagUpdate = now + getRandomInRange(0, CALC_SYSTEM_DIAG_FREQUENCY * 1000UL + 1);

        initialized = true;
        ESP_LOGI("CALC_SCHED", "Calculated sensor scheduler initialized");
    }

    if (!initialized)
        return; // Wait for request manager to start

    // Check and publish Delta T sensors
    if (now >= nextDeltaTUpdate)
    {
        publishDeltaTContinuous();
        publishDeltaTRunning();
        nextDeltaTUpdate = now + (CALC_DELTA_T_FREQUENCY * 1000UL) + getRandomInRange(0, 1000);
    }

    // Check and publish Compressor sensor
    if (now >= nextCompressorUpdate)
    {
        publishCompressorActive();
        nextCompressorUpdate = now + (CALC_COMPRESSOR_FREQUENCY * 1000UL) + getRandomInRange(0, 1000);
    }

    // Check and publish Date/Time sensors
    if (now >= nextDateTimeUpdate)
    {
        publishDate();
        publishTime();
        nextDateTimeUpdate = now + (CALC_DATETIME_FREQUENCY * 1000UL) + getRandomInRange(0, 1000);
    }

    // Check and publish Betriebsart sensor (only if SOMMERBETRIEB value is available)
    if (now >= nextBetriebsartUpdate)
    {
        // Betriebsart requires a value parameter, so we skip auto-publishing
        // It will be published when SOMMERBETRIEB signal is received
        nextBetriebsartUpdate = now + (CALC_BETRIEBSART_FREQUENCY * 1000UL) + getRandomInRange(0, 1000);
    }

    // Check and publish CAN diagnostic sensors
    if (now >= nextCanDiagUpdate)
    {
        publishCanDiagnostics();
        nextCanDiagUpdate = now + (CALC_CAN_DIAG_FREQUENCY * 1000UL) + getRandomInRange(0, 1000);
    }

    // Check and publish heat pump availability
    if (now >= nextAvailabilityUpdate)
    {
        publishAvailability();
        processSelfHealing();
        nextAvailabilityUpdate = now + (CALC_AVAILABILITY_FREQUENCY * 1000UL) + getRandomInRange(0, 500);
    }

    // Check and publish system diagnostics (heap, uptime)
    if (now >= nextSystemDiagUpdate)
    {
        publishSystemDiagnostics();
        nextSystemDiagUpdate = now + (CALC_SYSTEM_DIAG_FREQUENCY * 1000UL) + getRandomInRange(0, 1000);
    }
}

// Timeout tracking removed for simplification
// Signals that don't respond will simply not update in Home Assistant

// Process signal request table with frequency-based scheduling
void processSignalRequests()
{
    unsigned long now = millis();

    // Startup delay: wait before starting signal requests
    if (!requestManagerStarted)
    {
        if (requestManagerStartTime == 0)
        {
            requestManagerStartTime = now;
            ESP_LOGI("REQUEST_MGR", "Starting signal request manager (%ds startup delay)", STARTUP_DELAY_MS / 1000);
            return;
        }

        if (now - requestManagerStartTime < STARTUP_DELAY_MS)
        {
            return; // Still in startup delay
        }

        requestManagerStarted = true;
        ESP_LOGI("REQUEST_MGR", "Signal request manager active - processing %d signal definitions",
                 SIGNAL_REQUEST_COUNT);

        unsigned long initStartMs = millis();
        {
            std::vector<const char *> requestedNames;
            requestedNames.reserve(SIGNAL_REQUEST_COUNT);
            for (int i = 0; i < SIGNAL_REQUEST_COUNT; i++)
                requestedNames.push_back(signalRequests[i].signalName);
            PreloadElsterNameCache(requestedNames.data(), requestedNames.size());
        }

        // Initialize all signal schedules with random offsets to spread out initial load
        ESP_LOGI("REQUEST_MGR", "Initializing signal schedules with random offsets to prevent burst");
        nextRequestDue.assign(SIGNAL_REQUEST_COUNT * 3, 0);
        int scheduledCount = 0;
        for (int i = 0; i < SIGNAL_REQUEST_COUNT; i++)
        {
            const SignalRequest &req = signalRequests[i];
            const ElsterIndex *ei = GetElsterIndex(req.signalName);
            if (!ei || ei->Index == 0xFFFF)
                continue;

            // Skip blacklisted signals during initialization
            if (isPermanentlyBlacklisted(ei->Name))
                continue;

            unsigned long intervalMs = req.frequency * 1000UL;

            if (req.member == cm_other)
            {
                // Schedule for all members
                const CanMember *allMembers[] = {
                    &CanMembers[cm_kessel],
                    &CanMembers[cm_manager],
                    &CanMembers[cm_heizmodul]};
                for (size_t m = 0; m < 3; m++)
                {
                    // Random offset between 0 and min(interval, INITIAL_REQUEST_SPREAD_MS) using ESP32 hardware RNG
                    unsigned long randomOffset = getRandomInRange(0, std::min<unsigned long>(intervalMs, INITIAL_REQUEST_SPREAD_MS) + 1);
                    nextRequestDue[i * 3 + m] = now + randomOffset;
                    scheduledCount++;
                }
            }
            else
            {
                // Schedule for specific member
                // Random offset between 0 and min(interval, INITIAL_REQUEST_SPREAD_MS) using ESP32 hardware RNG
                unsigned long randomOffset = getRandomInRange(0, std::min<unsigned long>(intervalMs, INITIAL_REQUEST_SPREAD_MS) + 1);
                nextRequestDue[i * 3] = now + randomOffset;
                scheduledCount++;
            }
        }
        ESP_LOGI("REQUEST_MGR", "Initialized %d signal schedules in %lu ms", scheduledCount, millis() - initStartMs);
    }

    // Rate limiting: send up to MAX_REQUESTS_PER_ITERATION requests per iteration
    // Randomization in scheduling prevents bursts
    int requestsSentThisIteration = 0;

    // Round-robin processing: start from last position to prevent starvation of signals at end of array
    // Process all signals, wrapping around, until we hit rate limit or complete full cycle
    int processedCount = 0;
    int currentIndex = signalProcessingStartIndex;

    while (processedCount < SIGNAL_REQUEST_COUNT && requestsSentThisIteration < MAX_REQUESTS_PER_ITERATION)
    {
        const SignalRequest &req = signalRequests[currentIndex];
        processedCount++;

        const ElsterIndex *ei = GetElsterIndex(req.signalName);
        if (!ei || ei->Index == 0xFFFF)
        {
            // Move to next signal and continue
            currentIndex = (currentIndex + 1) % SIGNAL_REQUEST_COUNT;
            continue; // Signal not found in table
        }

        // Skip blacklisted signals - don't request them on CAN bus
        if (isPermanentlyBlacklisted(ei->Name))
        {
            currentIndex = (currentIndex + 1) % SIGNAL_REQUEST_COUNT;
            continue; // Signal is blacklisted
        }

        unsigned long intervalMs = req.frequency * 1000UL;

        // Determine which members to request from
        if (req.member == cm_other)
        {
            // Request from all members (each tracked independently)
            const CanMember *allMembers[] = {
                &CanMembers[cm_kessel],
                &CanMembers[cm_manager],
                &CanMembers[cm_heizmodul]};

            // Track how many members we've sent to in this cm_other group to stagger them
            int sentInThisGroup = 0;

            for (size_t m = 0; m < 3; m++)
            {
                // Don't break - just skip remaining members for this signal
                // The outer loop will continue with next signal
                if (requestsSentThisIteration >= MAX_REQUESTS_PER_ITERATION)
                {
                    break; // Hit rate limit for this iteration
                }

                const CanMember *member = allMembers[m];
                unsigned long &nextScheduled = nextRequestDue[currentIndex * 3 + m];

                // Check if this signal is overdue (current time >= scheduled time)
                if (now >= nextScheduled)
                {
                    // For cm_other: only send to ONE member per iteration to prevent bursts
                    // Other members will be checked in subsequent iterations
                    if (sentInThisGroup == 0)
                    {
                        readSignal(member, ei);
                        requestsSentThisIteration++;
                        sentInThisGroup++;

                        // Calculate next scheduled time with random offset (0 to 5% of interval)
                        // This keeps signals from synchronizing while staying close to target frequency
                        unsigned long maxJitter = (intervalMs / 20); // 5% of interval
                        if (maxJitter < 500)
                            maxJitter = 500; // Minimum 500ms jitter
                        unsigned long randomDelay = getRandomInRange(0, maxJitter + 1);
                        nextScheduled = now + intervalMs + randomDelay;

                        // ESP_LOGD("REQUEST_MGR", "Sent %s, next in %lums", key.c_str(), intervalMs + randomDelay);
                    }
                    // Else: skip this member for now, will be checked next iteration
                }
            }
        }
        else
        {
            // Request from specific member
            const CanMember *member = &CanMembers[req.member];
            unsigned long &nextScheduled = nextRequestDue[currentIndex * 3];

            // Check if this signal is overdue (current time >= scheduled time)
            if (now >= nextScheduled)
            {
                readSignal(member, ei);
                requestsSentThisIteration++;

                // Calculate next scheduled time with random offset (0 to 5% of interval)
                // This keeps signals from synchronizing while staying close to target frequency
                unsigned long maxJitter = (intervalMs / 20); // 5% of interval
                if (maxJitter < 500)
                    maxJitter = 500; // Minimum 500ms jitter
                unsigned long randomDelay = getRandomInRange(0, maxJitter + 1);
                nextScheduled = now + intervalMs + randomDelay;

                // ESP_LOGD("REQUEST_MGR", "Sent %s, next in %lums", key.c_str(), intervalMs + randomDelay);
            }
        }

        // Move to next signal in round-robin fashion (wrap around at end)
        currentIndex = (currentIndex + 1) % SIGNAL_REQUEST_COUNT;
    }

    // Resume after the last processed signal next time, so signals at the end of the table are not starved
    signalProcessingStartIndex = currentIndex;
}

// ============================================================================
// SIGNAL PROBE (read-only, started via MQTT "heatingpump/probe/start")
// Phase 1: every requested signal configured for BEDIENMODUL_2 is read from BEDIENMODUL_2 and from KESSEL.
// Phase 2: every ElsterTable signal that is not requested yet is read from KESSEL.
// Each answer is published (not retained) to heatingpump/probe/result as JSON, the summary to
// heatingpump/probe/done. Answers to probe reads of not-requested signals do not create HA entities.
// Only read requests are sent - nothing is written.
// ============================================================================

struct ProbeAsk
{
    bool used;
    uint16_t index;
    uint32_t memberId;
    unsigned long sentMs;
};

static bool probeActive = false;
static int probePhase = 0;
static size_t probePos = 0;
static int probeTargetToggle = 0;
static uint32_t probeSent = 0, probeAnswered = 0;
static unsigned long probeLastSendMs = 0;
static ProbeAsk probeAsks[24] = {};
static std::set<std::string> probeRegularNames;

// Read request with the real read id of the target (independent of readSignal())
static void sendProbeRead(const CanMember *target, uint16_t index)
{
    CanIdBytes readId = generate_read_id(target->CanId);
    std::vector<uint8_t> data = {readId.first, readId.second, 0xFA,
                                 static_cast<uint8_t>(index >> 8), static_cast<uint8_t>(index & 0xFF), 0x00, 0x00};
    id(my_can).send_data(CanMembers[cm_pc].CanId, false, data);

    // Free slot, otherwise the oldest one
    int slot = 0;
    for (int i = 0; i < 24; i++)
    {
        if (!probeAsks[i].used)
        {
            slot = i;
            break;
        }
        if (probeAsks[i].sentMs < probeAsks[slot].sentMs)
            slot = i;
    }
    probeAsks[slot] = {true, index, target->CanId, millis()};
    probeSent++;
    probeLastSendMs = millis();
}

void startSignalProbe()
{
    probeRegularNames.clear();
    for (int i = 0; i < SIGNAL_REQUEST_COUNT; i++)
        probeRegularNames.insert(signalRequests[i].signalName);
    for (auto &a : probeAsks)
        a.used = false;
    probePhase = 1;
    probePos = 0;
    probeTargetToggle = 0;
    probeSent = probeAnswered = 0;
    probeActive = true;
    ESP_LOGI("PROBE", "Signal probe started (read-only)");
}

void stopSignalProbe()
{
    if (!probeActive)
        return;
    probeActive = false;
    char buf[96];
    snprintf(buf, sizeof(buf), "{\"sent\":%lu,\"answered\":%lu}", (unsigned long)probeSent, (unsigned long)probeAnswered);
    id(mqtt_client).publish("heatingpump/probe/done", buf, strlen(buf), 0, false);
    ESP_LOGI("PROBE", "Signal probe finished: %s", buf);
    probeRegularNames.clear();
}

// Targeted probe (MQTT "heatingpump/probe/read"): reads the given indices from KESSEL and BEDIENMODUL_2.
// Payload: "0x05D0-0x05DF" (range) or "0x05D2,0xFDE0" (list), at most PROBE_TARGET_MAX indices.
// Answers are reported like the full probe; only indices listed in ElsterTable can be matched.
#define PROBE_TARGET_MAX 64
static std::vector<uint16_t> probeTargets;

void startTargetedProbe(const std::string &payload)
{
    std::vector<uint16_t> targets;
    const char *p = payload.c_str();
    while (*p && targets.size() < PROBE_TARGET_MAX)
    {
        char *end;
        unsigned long first = strtoul(p, &end, 16);
        if (end == p)
            break;
        unsigned long last = first;
        p = end;
        while (*p == ' ')
            p++;
        if (*p == '-')
        {
            last = strtoul(p + 1, &end, 16);
            if (end == p + 1)
                break;
            p = end;
        }
        for (unsigned long i = first; i <= last && i <= 0xFFFF && targets.size() < PROBE_TARGET_MAX; i++)
            targets.push_back(static_cast<uint16_t>(i));
        while (*p == ',' || *p == ' ')
            p++;
    }
    if (targets.empty())
    {
        ESP_LOGW("PROBE", "Targeted probe: no index in payload '%s'", payload.c_str());
        return;
    }

    probeRegularNames.clear();
    for (int i = 0; i < SIGNAL_REQUEST_COUNT; i++)
        probeRegularNames.insert(signalRequests[i].signalName);
    for (auto &a : probeAsks)
        a.used = false;
    probeTargets = std::move(targets);
    probePhase = 3;
    probePos = 0;
    probeTargetToggle = 0;
    probeSent = probeAnswered = 0;
    probeActive = true;
    ESP_LOGI("PROBE", "Targeted probe started: %u indices (read-only)", (unsigned)probeTargets.size());
}

// Call every 250 ms: sends one probe read per call (4 per second)
void runSignalProbe()
{
    if (!probeActive)
        return;

    if (probePhase == 3)
    {
        if (probePos < probeTargets.size())
        {
            sendProbeRead(&CanMembers[probeTargetToggle == 0 ? cm_kessel : cm_bedienmodul_2], probeTargets[probePos]);
            if (++probeTargetToggle > 1)
            {
                probeTargetToggle = 0;
                probePos++;
            }
            return;
        }
        if (millis() - probeLastSendMs > 3000)
        {
            probeTargets.clear();
            probeTargets.shrink_to_fit();
            stopSignalProbe();
        }
        return;
    }

    if (probePhase == 1)
    {
        // Requested signals configured for BEDIENMODUL_2: ask BEDIENMODUL_2, then KESSEL
        while (probePos < (size_t)SIGNAL_REQUEST_COUNT && signalRequests[probePos].member != cm_bedienmodul_2)
            probePos++;
        if (probePos < (size_t)SIGNAL_REQUEST_COUNT)
        {
            const ElsterIndex *ei = GetElsterIndex(signalRequests[probePos].signalName);
            sendProbeRead(&CanMembers[probeTargetToggle == 0 ? cm_bedienmodul_2 : cm_kessel], ei->Index);
            if (++probeTargetToggle > 1)
            {
                probeTargetToggle = 0;
                probePos++;
            }
            return;
        }
        probePhase = 2;
        probePos = 1; // entry 0 is INDEX_NOT_FOUND
    }

    // Phase 2: table signals that are not requested yet, asked at KESSEL
    while (probePos <= High(ElsterTable) &&
           (probeRegularNames.count(ElsterTable[probePos].Name) > 0 || ElsterTable[probePos].Index == 0xFFFF))
        probePos++;
    if (probePos <= High(ElsterTable))
    {
        sendProbeRead(&CanMembers[cm_kessel], ElsterTable[probePos].Index);
        probePos++;
        return;
    }

    // Give the last answers 3 s to arrive
    if (millis() - probeLastSendMs > 3000)
        stopSignalProbe();
}

// Called for every received value. Returns true if the message answers a probe read of a signal
// that is not requested regularly (then no HA entity / state is created for it).
bool probeHandleResponse(const CanMember &cm, const ElsterIndex *ei, const std::string &value)
{
    if (!probeActive)
        return false;
    unsigned long now = millis();
    for (auto &a : probeAsks)
    {
        if (!a.used || a.index != ei->Index || a.memberId != cm.CanId || now - a.sentMs > 3000)
            continue;
        a.used = false;
        probeAnswered++;
        bool regular = probeRegularNames.count(ei->Name) > 0;
        char buf[200];
        snprintf(buf, sizeof(buf), "{\"signal\":\"%s\",\"index\":\"0x%04x\",\"from\":\"%s\",\"value\":\"%s\",\"regular\":%s}",
                 ei->Name, ei->Index, cm.Name, value.c_str(), regular ? "true" : "false");
        id(mqtt_client).publish("heatingpump/probe/result", buf, strlen(buf), 0, false);
        return !regular;
    }
    return false;
}
void processAndUpdate(uint32_t can_id, std::vector<uint8_t> msg)
{
    // Elster frame type = low nibble of byte 0 (0 = write, 1 = read request, 2 = response, ...).
    // Read requests from other bus members carry no value (they showed up as "0.0" values) - skip them.
    if (msg.size() >= 7 && (msg[0] & 0x0F) == ELSTER_FRAME_READ)
        return;

    std::string value;
    const CanMember *cm = nullptr;
    const ElsterIndex *ei = processCanMessage(msg, can_id, value, &cm);

    // Ignore senders that are not known CAN members (they resolve to OTHER)
    if (cm == nullptr || cm == &CanMembers[cm_other])
        return;

    // Track heat pump liveness and answers to our writes (availability sensor)
    if (cm != nullptr && ei != nullptr)
        onCanResponse(*cm, ei);

    // Answers to probe reads of signals that are not requested regularly: report only
    if (probeHandleResponse(*cm, ei, value))
        return;

    // Skip permanently blacklisted signals
    if (isPermanentlyBlacklisted(ei->Name))
    {
        return; // Reject before lookup, parsing, formatting, logging
    }

    updateSensor(*cm, ei, value);
    return;
}

// True if the model's request table polls this signal (e.g. SEKUNDE: the LWZ 5S Plus has no seconds)
static bool isSignalRequested(const char *signalName)
{
    for (size_t i = 0; i < (size_t)SIGNAL_REQUEST_COUNT; i++)
        if (strcmp(signalRequests[i].signalName, signalName) == 0)
            return true;
    return false;
}

// Sets the heat pump clock to the ESP's SNTP time. SEKUNDE is only written if the model requests it.
void updateTime(CanMember cm)
{
    auto t = id(time_source).now();
    if (!t.is_valid())
    {
        ESP_LOGE("WRITE UHRZEIT", "No valid SNTP time yet - not writing");
        return;
    }

    char stunde[4], minute[4];
    snprintf(stunde, sizeof(stunde), "%d", t.hour);
    snprintf(minute, sizeof(minute), "%d", t.minute);
    ESP_LOGI("WRITE UHRZEIT", "%02d:%02d:%02d (SNTP)", t.hour, t.minute, t.second);
    writeSignal(&cm, "STUNDE", stunde);
    readSignal(&cm, "STUNDE");
    writeSignal(&cm, "MINUTE", minute);
    readSignal(&cm, "MINUTE");
    if (isSignalRequested("SEKUNDE"))
    {
        char sekunde[4];
        snprintf(sekunde, sizeof(sekunde), "%d", t.second);
        writeSignal(&cm, "SEKUNDE", sekunde);
        readSignal(&cm, "SEKUNDE");
    }
}

// Sets the heat pump date to the ESP's SNTP date. JAHR is written as two digits (2026 -> 26).
void updateDate(CanMember cm)
{
    auto t = id(time_source).now();
    if (!t.is_valid())
    {
        ESP_LOGE("WRITE DATUM", "No valid SNTP time yet - not writing");
        return;
    }

    char jahr[4], monat[4], tag[4];
    snprintf(jahr, sizeof(jahr), "%d", t.year % 100);
    snprintf(monat, sizeof(monat), "%d", t.month);
    snprintf(tag, sizeof(tag), "%d", t.day_of_month);
    ESP_LOGI("WRITE DATUM", "%04d-%02d-%02d (SNTP)", t.year, t.month, t.day_of_month);
    writeSignal(&cm, "JAHR", jahr);
    readSignal(&cm, "JAHR");
    writeSignal(&cm, "MONAT", monat);
    readSignal(&cm, "MONAT");
    writeSignal(&cm, "TAG", tag);
    readSignal(&cm, "TAG");
}

#endif // !defined(HA_DUMMY_BUILD)

#endif // ha_stiebel_control_H
