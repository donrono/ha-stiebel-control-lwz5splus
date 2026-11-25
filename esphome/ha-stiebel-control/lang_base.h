/*
 * lang_base.h — Default (German) display name macros
 *
 * This file is the single source of truth for all LNAME_* macros.
 * It defines every translatable string in German (the project default).
 *
 * Language-specific overrides (e.g. lang_en.h) include this file first,
 * then #undef and #redefine only the macros they translate. Any untranslated
 * macro keeps its German default from this file.
 *
 * To add a new language:
 *   1. Create lang_XX.h (include this file, then override macros)
 *   2. Add #elif defined(HA_LANGUAGE_XX) in language_select.h
 */

#pragma once

// ============================================================================
// ElsterTable signal friendly names
// ============================================================================
#define LNAME_BRENNER "Brenner"
#define LNAME_KESSELSOLLTEMP "Kessel Soll Temperatur"
#define LNAME_SPEICHERSOLLTEMP "WW Soll-Temperatur"
#define LNAME_RAUMSOLLTEMP_I "Raumtemperatur Tag HK1"
#define LNAME_RAUMSOLLTEMP_II "Raum Soll Temperatur II"
#define LNAME_RAUMSOLLTEMP_III "Raum Soll Temperatur III"
#define LNAME_RAUMSOLLTEMP_NACHT "Raumtemperatur Nacht HK1"
#define LNAME_AUSSENTEMP "Außen Temperatur"
#define LNAME_SAMMLERISTTEMP "Vorlauftemperatur"
#define LNAME_SPEICHERISTTEMP "WW Temperatur"
#define LNAME_VORLAUFISTTEMP "Istwert HK1"
#define LNAME_EINSTELL_SPEICHERSOLLTEMP "WW Soll Tag"
#define LNAME_EINSTELL_SPEICHERSOLLTEMP2 "Einstellung Speicher Soll Temperatur 2"
#define LNAME_VERDAMPFERTEMP "Verdampfer Temperatur"
#define LNAME_RUECKLAUFISTTEMP "Rücklauftemperatur"
#define LNAME_ABTAUUNGAKTIV "Abtauung Aktiv"
#define LNAME_EVU_SPERRE_AKTIV "EVU Sperre Aktiv"
#define LNAME_SOLAR_AKT_LEISTUNG_W "Solar Akt Leistung W"
#define LNAME_SOLAR_TAGESERTRAG_WH "Solar Tagesertrag Wh"
#define LNAME_SOLAR_TAGESERTRAG_KWH "Solar Tagesertrag kWh"
#define LNAME_SOLAR_GESAMTERTRAG_WH "Solar Gesamtertrag Wh"
#define LNAME_SOLAR_GESAMTERTRAG_KWH "Solar Gesamtertrag kWh"
#define LNAME_SOLAR_GESAMTERTRAG_MWH "Solar Gesamtertrag MWh"
#define LNAME_SOLAR_WOCHENERTRAG_WH "Solar Wochen Ertrag Wh"
#define LNAME_SOLAR_WOCHENERTRAG_KWH "Solar Wochen Ertrag kWh"
#define LNAME_SOLAR_WOCHENERTRAG_MWH "Solar Wochen Ertrag MWh"
#define LNAME_SOLAR_JAHRESERTRAG_WH "Solar Jahres Ertrag Wh"
#define LNAME_SOLAR_JAHRESERTRAG_KWH "Solar Jahres Ertrag kWh"
#define LNAME_SOLAR_JAHRESERTRAG_MWH "Solar Jahres Ertrag MWh"
#define LNAME_ANTILEGIONELLEN "Antilegionellen"
#define LNAME_ANTILEGIONELLEN_ZEITPUNKT "Antilegionellen Zeitpunkt"
#define LNAME_HEIZKURVE "Steigung HK1"
#define LNAME_PROGRAMMSCHALTER "Programmschalter"
#define LNAME_TAG "Zeit - Tag"
#define LNAME_MONAT "Zeit - Monat"
#define LNAME_JAHR "Zeit - Jahr"
#define LNAME_STUNDE "Zeit - Stunde"
#define LNAME_MINUTE "Zeit - Minute"
#define LNAME_SEKUNDE "Sekunde"
#define LNAME_WOCHENTAG "Wochentag"
#define LNAME_BETRIEBS_STATUS "Betriebsstatus"
#define LNAME_WW_HYSTERSE "WW Hysterese"
#define LNAME_WPVORLAUFIST "Wärmepumpe Vorlauf Ist Temperatur"
#define LNAME_HILFSKESSELSOLL "Hilfskessel Soll Temperatur"
#define LNAME_WW_ECO "Warmwasser ECO"
#define LNAME_WAERMEERTRAG_RUECKGE_TAG_WH "Wärme Ertrag Rückge Tag Wh"
#define LNAME_WAERMEERTRAG_RUECKGE_TAG_KWH "Wärme Ertrag Rückge Tag kWh"
#define LNAME_WAERMEERTRAG_RUECKGE_SUMME_KWH "Wärme Ertrag Rückge Summe kWh"
#define LNAME_WAERMEERTRAG_RUECKGE_SUMME_MWH "Wärme Ertrag Rückge Summe MWh"
#define LNAME_VERDICHTER "Verdichter"
#define LNAME_LZ_VERD_1_HEIZBETRIEB "Laufzeit Verdichter 1 Heizbetrieb"
#define LNAME_LZ_VERD_2_HEIZBETRIEB "Laufzeit Verdichter 2 Heizbetrieb"
#define LNAME_LZ_VERD_1_2_HEIZBETRIEB "Laufzeit Verdichter 1+2 Heizbetrieb"
#define LNAME_LZ_VERD_1_KUEHLBETRIEB "Laufzeit Verdichter 1 Kühlbetrieb"
#define LNAME_LZ_VERD_2_KUEHLBETRIEB "Laufzeit Verdichter 2 Kühlbetrieb"
#define LNAME_LZ_VERD_1_2_KUEHLBETRIEB "Laufzeit Verdichter 1+2 Kühlbetrieb"
#define LNAME_EL_AUFNAHMELEISTUNG_WW_TAG_WH "Stromverbrauch WW Tag Wh"
#define LNAME_EL_AUFNAHMELEISTUNG_WW_TAG_KWH "Stromverbrauch WW Tag kWh"
#define LNAME_EL_AUFNAHMELEISTUNG_WW_SUM_KWH "Stromverbrauch WW Summe kWh"
#define LNAME_EL_AUFNAHMELEISTUNG_WW_SUM_MWH "Stromverbrauch WW Summe MWh"
#define LNAME_EL_AUFNAHMELEISTUNG_HEIZ_TAG_WH "Stromverbrauch Heizung Tag Wh"
#define LNAME_EL_AUFNAHMELEISTUNG_HEIZ_TAG_KWH "Stromverbrauch Heizung Tag kWh"
#define LNAME_EL_AUFNAHMELEISTUNG_HEIZ_SUM_KWH "Stromverbrauch Heizung Summe kWh"
#define LNAME_EL_AUFNAHMELEISTUNG_HEIZ_SUM_MWH "Stromverbrauch Heizung Summe MWh"
#define LNAME_WAERMEERTRAG_2WE_WW_TAG_WH "Wärme Ertrag 2we Warmwasser Tag Wh"
#define LNAME_WAERMEERTRAG_2WE_WW_TAG_KWH "Wärme Ertrag 2we Warmwasser Tag kWh"
#define LNAME_WAERMEERTRAG_2WE_WW_SUM_KWH "Wärmemenge Notheizung WW Summe kWh"
#define LNAME_WAERMEERTRAG_2WE_WW_SUM_MWH "Wärmemenge Notheizung WW Summe MWh"
#define LNAME_WAERMEERTRAG_2WE_HEIZ_TAG_WH "Wärme Ertrag 2we Heizung Tag Wh"
#define LNAME_WAERMEERTRAG_2WE_HEIZ_TAG_KWH "Wärme Ertrag 2we Heizung Tag kWh"
#define LNAME_WAERMEERTRAG_2WE_HEIZ_SUM_KWH "Wärmemenge Notheizung Heizen Summe kWh"
#define LNAME_WAERMEERTRAG_2WE_HEIZ_SUM_MWH "Wärmemenge Notheizung Heizen Summe MWh"
#define LNAME_WAERMEERTRAG_WW_TAG_WH "Wärmemenge WW Tag Wh"
#define LNAME_WAERMEERTRAG_WW_TAG_KWH "Wärmemenge WW Tag kWh"
#define LNAME_WAERMEERTRAG_WW_SUM_KWH "Wärmemenge WW Summe kWh"
#define LNAME_WAERMEERTRAG_WW_SUM_MWH "Wärmemenge WW Summe MWh"
#define LNAME_WAERMEERTRAG_HEIZ_TAG_WH "Wärmemenge Heizung Tag Wh"
#define LNAME_WAERMEERTRAG_HEIZ_TAG_KWH "Wärmemenge Heizung Tag kWh"
#define LNAME_WAERMEERTRAG_HEIZ_SUM_KWH "Wärmemenge Heizung Summe kWh"
#define LNAME_WAERMEERTRAG_HEIZ_SUM_MWH "Wärmemenge Heizung Summe MWh"
#define LNAME_GEBAEUDEART "Gebäudeart"
#define LNAME_WP_STATUS "Wärmepumpe Status"
#define LNAME_SOMMERBETRIEB "Sommerbetrieb"
#define LNAME_BETRIEBSART_WP "Betriebsart Wärmepumpe"

// ============================================================================
// Calculated sensor names (ha-stiebel-control.h calculatedSensors[])
// ============================================================================
#define LNAME_CALC_DATE "Datum"
#define LNAME_CALC_TIME "Uhrzeit"
#define LNAME_CALC_BETRIEBSART "Betriebsart"
#define LNAME_CALC_DELTA_T_CONTINUOUS "Delta T WP (kontinuierlich)"
#define LNAME_CALC_DELTA_T_RUNNING "Delta T WP (nur bei Verdichter an)"
#define LNAME_CALC_COMPRESSOR_ACTIVE "WP Verdichter aktiv"
#define LNAME_CALC_CAN_TEC "CAN TX Fehlerzähler"
#define LNAME_CALC_CAN_REC "CAN RX Fehlerzähler"
#define LNAME_CALC_CAN_BUS_ERRORS "CAN Bus Fehler"
#define LNAME_CALC_CAN_STATE "CAN Bus Zustand"
#define LNAME_CALC_AVAILABLE "Wärmepumpe erreichbar"
#define LNAME_CALC_AVAILABILITY_STATUS "Wärmepumpe Erreichbarkeit Status"
#define LNAME_CALC_HEAP_FREE "Freier Heap (intern)"
#define LNAME_CALC_HEAP_LARGEST_BLOCK "Größter freier Heap-Block"
#define LNAME_CALC_HEAP_MIN_FREE "Minimaler freier Heap seit Start"
#define LNAME_CALC_UPTIME "Laufzeit seit Start"
#define LNAME_CALC_CAN_RESTARTS "CAN Neustarts (automatisch)"
#define LNAME_CALC_BETRIEBSSTATUS_TEXT "Betriebsstatus (Klartext)"
#define LNAME_CALC_VERDICHTERSTARTS "Verdichterstarts"


// ============================================================================
// Writable number friendly names (writableNumbers[])
// ============================================================================
#define LNAME_NUM_SPEICHERSOLLTEMP "Speicher Soll Temperatur Einstellung"
#define LNAME_NUM_SPEICHERSOLLTEMP2 "Speicher Soll Temperatur 2 Einstellung"
#define LNAME_NUM_SG_READY_BOOST3 "SG Ready Boost Zustand 3"
#define LNAME_NUM_SG_READY_BOOST4 "SG Ready Boost Zustand 4"
#define LNAME_NUM_RAUMSOLLTEMP_I "Raum Soll Temperatur I Einstellung"
#define LNAME_NUM_RAUMSOLLTEMP_II "Raum Soll Temperatur II Einstellung"
#define LNAME_NUM_RAUMSOLLTEMP_III "Raum Soll Temperatur III Einstellung"
#define LNAME_NUM_RAUMSOLLTEMP_NACHT "Raum Soll Temperatur Nacht Einstellung"

// ============================================================================
// Writable select friendly names (writableSelects[])
// ============================================================================
#define LNAME_SEL_PROGRAMMSCHALTER "Programmschalter"
#define LNAME_SEL_SG_READY "SG Ready Zustand"

// ============================================================================
// Select option strings (programmschalterOptions[], sgReadyOptions[])
// ============================================================================

#define LNAME_OPT_NOTBETRIEB "Notbetrieb"
#define LNAME_OPT_BEREITSCHAFT "Bereitschaft"
#define LNAME_OPT_AUTOMATIK "Automatik"
#define LNAME_OPT_TAGBETRIEB "Tagbetrieb"
#define LNAME_OPT_ABSENKBETRIEB "Absenkbetrieb"
#define LNAME_OPT_WARMWASSER "Warmwasser"
#define LNAME_OPT_HANDBETRIEB "Handbetrieb"
#define LNAME_OPT_SG_EVU_SPERRE "1 - EVU Sperre"
#define LNAME_OPT_SG_NORMAL "2 - Normal"
#define LNAME_OPT_SG_EMPFOHLEN "3 - Empfohlen"
#define LNAME_OPT_SG_ZWANG "4 - Zwang"

// ============================================================================
// COP sensor names (publishCOPDiscovery())
// ============================================================================
#define LNAME_COP_WW "COP Warmwasser"
#define LNAME_COP_HEIZ "COP Heizung"
#define LNAME_COP_GESAMT "COP Gesamt"
