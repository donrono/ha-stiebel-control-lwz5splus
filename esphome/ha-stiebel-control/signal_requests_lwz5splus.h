/*
 * Signal Request Table — LWZ5SPLUS Heat Pump
 *
 * Contains the signals polled from the LWZ5SPLUS (validated on the device).
 * To add a new model, copy this file, rename it, and replace the
 * model-specific section with your pump's signals.
 */

#ifndef SIGNAL_REQUESTS_LWZ5SPLUS_H
#define SIGNAL_REQUESTS_LWZ5SPLUS_H

#include "config.h"

extern const SignalRequest signalRequests[] = {
    {"JAHR", FREQ_5MIN, cm_kessel},      // 0x0124 Jahr
    {"MONAT", FREQ_5MIN, cm_kessel},     // 0x0123 Monat
    {"TAG", FREQ_5MIN, cm_kessel},       // 0x0122 Tag
    {"WOCHENTAG", FREQ_5MIN, cm_kessel}, // 0x0121 Wochentag
    {"STUNDE", FREQ_5MIN, cm_kessel},    // 0x0125 Stunde
    {"MINUTE", FREQ_5MIN, cm_kessel},    // 0x0126 Minute

    // ========================================================================
    // LWZ5SPLUS-SPECIFIC: temperatures and heating circuits
    // ========================================================================
    {"FEHLERMELDUNG", FREQ_5MIN, cm_kessel},                     // 0x0001 Fehlerspeicher
    {"PROGRAMMSCHALTER", FREQ_5MIN, cm_kessel},                  // 0x0112 Programmschalter
    {"SPEICHERSOLLTEMP", FREQ_5MIN, cm_kessel},                  // 0x0003 Warmwasser Soll-Temperatur
    {"SPEICHERISTTEMP", FREQ_1MIN, cm_kessel},                   // 0x000E Warmwasser Temperatur
    {"AUSSENTEMP", FREQ_1MIN, cm_kessel},                        // 0x000C Außentemperatur
    {"VORLAUFISTTEMP", FREQ_1MIN, cm_kessel},                    // 0x000F Istwert HK1
    {"RAUMISTTEMP", FREQ_1MIN, cm_bedienmodul_2},                // 0x0011 Raumtemperatur HK1
    {"VERSTELLTE_RAUMSOLLTEMP", FREQ_5MIN, cm_bedienmodul_2},    // 0x0012 Raumsolltemperatur HK1
    {"FEUCHTE", FREQ_1MIN, cm_bedienmodul_2},                    // 0x0075 Raumfeuchte HK1
    {"TAUPUNKT_TEMP", FREQ_1MIN, cm_bedienmodul_2},              // 0x0264 Taupunkttemperatur HK1
    {"DRUCK_HEIZKREIS", FREQ_1MIN, cm_kessel},                   // 0x064A Druck Heizkreis
    {"WAERMEERTRAG_HEIZ_TAG_WH", FREQ_60MIN, cm_kessel},         // 0x092E Wärmemenge Heizen Tag
    {"WAERMEERTRAG_HEIZ_TAG_KWH", FREQ_60MIN, cm_kessel},        // 0x092F Wärmemenge Heizen Tag
    {"WAERMEERTRAG_HEIZ_SUM_KWH", FREQ_60MIN, cm_kessel},        // 0x0930 Wärmemenge Heizen Summe
    {"WAERMEERTRAG_HEIZ_SUM_MWH", FREQ_60MIN, cm_kessel},        // 0x0931 Wärmemenge Heizen Summe
    {"EL_AUFNAHMELEISTUNG_HEIZ_TAG_WH", FREQ_60MIN, cm_kessel},  // 0x091E Stomverbrauch Heizung Tag
    {"EL_AUFNAHMELEISTUNG_HEIZ_TAG_KWH", FREQ_60MIN, cm_kessel}, // 0x091F Stomverbrauch Heizung Tag
    {"EL_AUFNAHMELEISTUNG_HEIZ_SUM_KWH", FREQ_60MIN, cm_kessel}, // 0x0920 Stromverbrauch Heizung Summe
    {"EL_AUFNAHMELEISTUNG_HEIZ_SUM_MWH", FREQ_60MIN, cm_kessel}, // 0x0921 Stromverbrauch Heizung Summe
    {"WAERMEERTRAG_WW_TAG_WH", FREQ_60MIN, cm_kessel},           // 0x092A Wärmemenge Warmwasser Tag
    {"WAERMEERTRAG_WW_TAG_KWH", FREQ_60MIN, cm_kessel},          // 0x092B Wärmemenge Warmwasser Tag
    {"WAERMEERTRAG_WW_SUM_KWH", FREQ_60MIN, cm_kessel},          // 0x092C Wärmemenge Warmwasser Summe
    {"WAERMEERTRAG_WW_SUM_MWH", FREQ_60MIN, cm_kessel},          // 0x092D Wärmemenge Warmwasser Summe
    {"EL_AUFNAHMELEISTUNG_WW_TAG_WH", FREQ_60MIN, cm_kessel},    // 0x091A Stromverbrauch Warmwasser Tag
    {"EL_AUFNAHMELEISTUNG_WW_TAG_KWH", FREQ_60MIN, cm_kessel},   // 0x091B Stromverbrauch Warmwasser Tag
    {"EL_AUFNAHMELEISTUNG_WW_SUM_KWH", FREQ_60MIN, cm_kessel},   // 0x091C Stromverbrauch Warmwasser Summe
    {"EL_AUFNAHMELEISTUNG_WW_SUM_MWH", FREQ_60MIN, cm_kessel},   // 0x091D Stromverbrauch Warmwasser Summe
    {"WAERMEERTRAG_2WE_HEIZ_SUM_KWH", FREQ_60MIN, cm_kessel},    // 0x0928 Wärmemenge Notheizung Heizen Summe
    {"WAERMEERTRAG_2WE_HEIZ_SUM_MWH", FREQ_60MIN, cm_kessel},    // 0x0929 Wärmemenge Notheizung Heizen Summe
    {"WAERMEERTRAG_2WE_WW_SUM_KWH", FREQ_60MIN, cm_kessel},      // 0x0924 Wärmemenge Notheizung Warmwasser Summe
    {"WAERMEERTRAG_2WE_WW_SUM_MWH", FREQ_60MIN, cm_kessel},      // 0x0925 Wärmemenge Notheizung Warmwasser Summe
    {"WAERMEERTRAG_RUECKGE_TAG_KWH", FREQ_60MIN, cm_kessel},      // 0x03AF Wärmemenge Wärmerückgewinnung Tag
    {"WAERMEERTRAG_RUECKGE_SUMME_KWH", FREQ_60MIN, cm_kessel},     // 0x03B0 Wärmemenge Wärmerückgewinnung Summe
    {"WAERMEERTRAG_RUECKGE_SUMME_MWH", FREQ_60MIN, cm_kessel},   // 0x03B1 Wärmemenge Wärmerückgewinnung Summe
    {"SOFTWARE_VERSION", FREQ_60MIN, cm_kessel},                 // 0x019A Softwarestand
    {"SOFTWARE_ID", FREQ_60MIN, cm_kessel},                      // 0x024B Software-ID
    {"KUNDENKENNUNG", FREQ_60MIN, cm_kessel},                    // 0x001A Kundennummer
    {"STOPPEREIGNIS", FREQ_1MIN, cm_kessel},                     // 0x05C7 Stopp Ereigniss
    {"RUECKLAUFISTTEMP", FREQ_1MIN, cm_kessel},                  // 0x0016 Rücklauftemperatur
    {"VOLUMENSTROM", FREQ_1MIN, cm_kessel},                      // 0x01DA Volumenstrom
    {"VERDICHTERSTARTS1", FREQ_5MIN, cm_kessel},                 // 0xC0F4 Verdichterstarts (4.-6. Stelle)
    {"VERDICHTERSTARTS1000", FREQ_5MIN, cm_kessel},              // 0xC0F5 Verdichterstarts (1.-3. Stelle)
    {"LAUFZEIT_VERDICHTER_HEIZEN", FREQ_60MIN, cm_kessel},       // 0x05A4 Verdichter Heizen
    {"LAUFZEIT_VERDICHTER_WW", FREQ_60MIN, cm_kessel},           // 0x05A6 Verdichter Warmwasser
    {"KORREKTUR_AUSSENTEMPERATUR", FREQ_60MIN, cm_kessel},             // 0x05A3 Korrektur Außentemperatur
    {"SOMMER_HYSTERESE", FREQ_60MIN, cm_kessel},                 // 0x05A2 Hysterese Sommerbetrieb
    {"LAUFZEIT_NHZ_WW", FREQ_60MIN, cm_kessel},                  // 0x05A7 Elektrische Notheizung Warmwasser
    {"LAUFZEIT_NHZ_HEIZEN", FREQ_60MIN, cm_kessel},              // 0x05A8 Elektrische Notheizung Heizen
    {"BETRIEBS_STATUS", FREQ_1MIN, cm_kessel},                   // 0x0176 Betriebsstatus
    {"ZULUFT_SOLL", FREQ_1MIN, cm_kessel},                       // 0x0596 Zuluft Soll
    {"ZULUFT_IST", FREQ_1MIN, cm_kessel},                        // 0x0597 Zuluft Ist
    {"ABLUFT_SOLL", FREQ_1MIN, cm_kessel},                       // 0x0598 Abluft Soll
    {"ABLUFT_IST", FREQ_1MIN, cm_kessel},                        // 0x0599 Abluft Ist
    {"ABLUFTFEUCHTE", FREQ_1MIN, cm_kessel},                     // 0xC0EF Abluftfeuchte
    {"ABLUFTTEMP", FREQ_1MIN, cm_kessel},                        // 0x0694 Ablufttemperatur
    {"LUEFT_STUFE_TAG", FREQ_1MIN, cm_kessel},                   // 0x056C Stufe Tag
    {"FORTLUFT_IST", FREQ_1MIN, cm_kessel},                      // 0x059A Fortluft Ist
    {"ANZEIGE_HOCHDRUCK", FREQ_1MIN, cm_kessel},                 // 0x07A6 Hochdruck
    {"ANZEIGE_NIEDERDRUCK", FREQ_1MIN, cm_kessel},               // 0x07A7 Niederdruck
    {"VERDAMPFERTEMP", FREQ_1MIN, cm_kessel},                    // 0x0014 Verdampfertemperatur
    {"VERFLUESSIGERTEMP", FREQ_1MIN, cm_kessel},                 // 0x059C Verflüssigertemperatur
    {"OELSUMPFTEMP", FREQ_1MIN, cm_kessel},                      // 0x0693 Ölsumpftemperatur
    {"LAUFZEIT_FILTER", FREQ_60MIN, cm_kessel},                  // 0x0341 Laufzeit Filter
    {"LAUFZEIT_FILTER_PERC", FREQ_60MIN, cm_kessel},             // 0xC111 Laufzeit Filter
    {"VERDAMPFERISTTEMP_KOMPENSIERT", FREQ_1MIN, cm_kessel},     // 0x07A9 Verdampferausgang Temperatur
    {"HEISSGAS_TEMP", FREQ_1MIN, cm_kessel},                     // 0x0265 Heisgastemperatur
    {"COOL_TEMP", FREQ_1MIN, cm_kessel},                         // 0x063F Kühlungstemperatur
    {"DRUCK_VERDAMPFER_GEFILTERT", FREQ_1MIN, cm_kessel},        // 0x080D Niederdruck Gefiltert
    {"PWM_SOLARPUMPE", FREQ_1MIN, cm_kessel},                    // 0x0698 PWM Solarpumpe
    {"PWM_MISCHERPUMPE", FREQ_1MIN, cm_kessel},                  // 0x0699 PWM Mischerpumpe
    {"HEIZLEIST_REL", FREQ_1MIN, cm_kessel},                     // 0x069A Heizleistung Relativ
    {"VORGABE_VERDICHTER", FREQ_1MIN, cm_kessel},                // 0x069B Vorgabe Verdichter
    {"VERDICHTERDREHZAHL", FREQ_1MIN, cm_kessel},                // 0x069E Verdichterdrehzahl
    {"MOTORSTROM", FREQ_1MIN, cm_kessel},                        // 0x069F Motorstrom
    {"MOTORLEISTUNG", FREQ_1MIN, cm_kessel},                     // 0x06A0 Motorleistung
    {"TEMPORALE_LUEFTUNGSSTUFE_STUFE", FREQ_1MIN, cm_kessel},    // 0x05DD Temporale Lüftungsstufe
    {"TEMPORALE_LUEFTUNGSSTUFE_DAUER", FREQ_1MIN, cm_kessel},    // 0x05DE Temporale Lüftungsdauer
    {"RAUMSOLLTEMP_I", FREQ_1MIN, cm_bedienmodul_2},             // 0x0005 Raumtemperatur Tag HK1
    {"RAUMSOLLTEMP_NACHT", FREQ_1MIN, cm_bedienmodul_2},         // 0x0008 Raumtemperatur Nacht HK1
    {"ABWESENHEITSTEMP", FREQ_1MIN, cm_bedienmodul_2},           // 0x013D Raumtemperatur Bereitschaft HK1
    {"HEIZKREISSOLL_HAND_HK1", FREQ_1MIN, cm_bedienmodul_2},     // 0x0129 Heizkreissoll Hand HK1
    {"LUEFT_STUFE_NACHT", FREQ_1MIN, cm_kessel},                 // 0x056D Stufe Nacht
    {"LUEFT_STUFE_BEREITSCHAFT", FREQ_60MIN, cm_kessel},         // 0x056F Stufe Bereitschaft
    // ========================================================================
    // VALIDATED ON LWZ5SPLUS: signals confirmed via lwz5splus MQTT config
    // (tmp/mqtt.yaml joined with tmp/StiebelEltron.json; member = Sender of the JSON entry)
    // ========================================================================

    // --- validated: heatcircle ---
    {"VORLAUFSOLLTEMP", FREQ_1MIN, cm_bedienmodul_2},     // 0x0004 Sollwert HK1
    {"HEIZKURVE", FREQ_60MIN, cm_bedienmodul_2},          // 0x010E Steigung HK1
    {"RAUMEINFLUSS", FREQ_60MIN, cm_bedienmodul_2},       // 0x010F Raumeinfluss HK1
    {"FUSSPUNKT_HK1", FREQ_60MIN, cm_bedienmodul_2},     // 0x059E Fusspunkt HK1
    {"ANTEIL_VORLAUF_HK1", FREQ_60MIN, cm_bedienmodul_2}, // 0x059D Anteil Vorlauf HK1
    {"MAX_TEMP_KESSEL", FREQ_60MIN, cm_bedienmodul_2},    // 0x0028 Sollwert Maximum HK1
    {"MIN_TEMP_HZK", FREQ_60MIN, cm_bedienmodul_2},       // 0x012B Sollwert Minimum HK1

    // --- validated: meter ---
    {"WAERMEERTRAG_RUECKGE_TAG_WH", FREQ_60MIN, cm_kessel},                        // 0x03AE Wärmemenge Wärmerückgewinnung Tag
    {"WAERMEMENGE_SOLAR_HEIZUNG_SUMME_KWH", FREQ_60MIN, cm_kessel},                    // 0x0642 Wärmemenge Solar Heizung Summe
    {"WAERMEMENGE_SOLAR_HEIZUNG_SUMME_MWH", FREQ_60MIN, cm_kessel},                    // 0x0643 Wärmemenge Solar Heizung Summe
    {"WAERMEMENGE_SOLAR_HEIZUNG_TAG_WH", FREQ_60MIN, cm_kessel}, // 0x0640 Wärmemenge Solar Heizung Tag
    {"WAERMEMENGE_SOLAR_HEIZUNG_TAG_KWH", FREQ_60MIN, cm_kessel},                    // 0x0641 Wärmemenge Solar Heizung Tag
    {"WAERMEMENGE_SOLAR_WW_SUMME_KWH", FREQ_60MIN, cm_kessel},                    // 0x0646 Wärmemenge Solar Warmwasser Summe
    {"WAERMEMENGE_SOLAR_WW_SUMME_MWH", FREQ_60MIN, cm_kessel},                    // 0x0647 Wärmemenge Solar Warmwasser Summe
    {"WAERMEMENGE_SOLAR_WW_TAG_WH", FREQ_60MIN, cm_kessel},                    // 0x0644 Wärmemenge Solar Warmwasser Tag
    {"WAERMEMENGE_SOLAR_WW_TAG_KWH", FREQ_60MIN, cm_kessel},                    // 0x0645 Wärmemenge Solar Warmwasser Tag
    {"WAERMEMENGE_KUEHLEN_SUMME_KWH", FREQ_60MIN, cm_kessel},                    // 0x0648 Wärmemenge Kühlen Summe
    {"WAERMEMENGE_KUEHLEN_SUMME_MWH", FREQ_60MIN, cm_kessel},                    // 0x0649 Wärmemenge Kühlen Summe

    // --- validated: flow ---
    {"SAMMLERISTTEMP", FREQ_1MIN, cm_kessel}, // 0x000D Vorlauftemperatur

    // --- validated: compressor ---
    {"LAUFZEIT_VERDICHTER_KUEHLEN", FREQ_60MIN, cm_kessel}, // 0x05A5 Verdichter Kühlen

    // --- validated: air ---
    {"ABLUFTTAUPUNKT", FREQ_60MIN, cm_kessel}, // 0xC0F6 Ablufttaupunkt
    {"FORTLUFT_SOLL", FREQ_1MIN, cm_kessel},   // 0x059B Fortluft Soll

    // --- validated: processvalues ---
    {"LUEFTERDREHZAHL", FREQ_5MIN, cm_kessel}, // 0x0833 Lüfter (Prozent)
    {"VERDICHTER_N_SOLL_UNBEGRENZT", FREQ_5MIN, cm_kessel},  // 0x069C Verdichter N. Soll Unbegrenzt
    {"VERDICHTER_N_SOLL_BEGRENZT", FREQ_5MIN, cm_kessel},  // 0x069D Verdichter N. Soll Begrenzt
    {"POSITION_VENTIL", FREQ_5MIN, cm_kessel},  // 0x064B Position Ventil

    // --- validated: inverter ---
    {"MOTORSPANNUNG", FREQ_5MIN, cm_kessel}, // 0x06A1 Motorspannung
    {"INVERTERTEMPERATUR", FREQ_5MIN, cm_kessel}, // 0x06A2 Invertertemperatur
    {"INVERTERFEHLER", FREQ_5MIN, cm_kessel}, // 0x06A3 Inverterfehler

    // --- validated: analysis ---
    {"SONDERKREIS_STATUS", FREQ_5MIN, cm_kessel},                       // 0x0067 Aktuell Mode IWS
    {"BETRIEBSART_WP", FREQ_5MIN, cm_kessel},                           // 0x07F2 Aktuell Mode EVE
    {"SOLLWERT_UEBERHITZUNG", FREQ_5MIN, cm_kessel},                    // 0x07F3 Überh. Verdichter Soll
    {"ISTWERT_UEBERHITZUNG_VERDAMPFER", FREQ_5MIN, cm_kessel},          // 0x07F4 Überh. Verdichter Ist
    {"ISTWERT_UEBERHITZUNG_REKUPERATOR_KUEHLEN", FREQ_5MIN, cm_kessel}, // 0x07F5 Überh. Rekup. Ist
    {"PENDELN_RELATIV", FREQ_5MIN, cm_kessel},                          // 0x07FA Pendeln Rel.
    {"FAKTOR_REGELDYNAMIK", FREQ_5MIN, cm_kessel},                      // 0x07FB Dynamik Faktor
    {"P_ANTEIL_EXV", FREQ_5MIN, cm_kessel},                             // 0x07F7 P-Faktor
    {"I_ANTEIL_EXV", FREQ_5MIN, cm_kessel},                             // 0x07F8 I-Faktor
    {"D_ANTEIL_EXV", FREQ_5MIN, cm_kessel},                             // 0x07F9 D-Faktor
    {"VORSTEUER_OEFFNUNGSGRAD", FREQ_5MIN, cm_kessel},                  // 0x07F6 Öffnung EXV Vorst.
    {"EXV_OEFFNUNGSGRAD", FREQ_5MIN, cm_kessel},                        // 0x07CA Öffnung EXV
    {"HEIZ_KUEHLLEISTUNG_GEMESSEN", FREQ_5MIN, cm_kessel},   // 0x081C Heiz-/Kühlleistung gemessen

    // --- validated: fan ---
    {"LUEFTUNG_STUFE_PARTY", FREQ_60MIN, cm_kessel},         // 0x0570 Stufe Party
    {"LUEFTUNG_STUFE_HAND", FREQ_60MIN, cm_kessel},       // 0x0612 Stufe Hand
    {"LUEFTUNGSZEIT_AUSSERPL_ST_0", FREQ_60MIN, cm_kessel},            // 0x0571 Lüftungszeit Außerplanmäßig Stufe 0
    {"LUEFTUNGSZEIT_AUSSERPL_ST_1", FREQ_60MIN, cm_kessel}, // 0x0572 Lüftungszeit Außerplanmäßig Stufe 1
    {"LUEFTUNGSZEIT_AUSSERPL_ST_2", FREQ_60MIN, cm_kessel}, // 0x0573 Lüftungszeit Außerplanmäßig Stufe 2
    {"LUEFTUNGSZEIT_AUSSERPL_ST_3", FREQ_60MIN, cm_kessel}, // 0x0574 Lüftungszeit Außerplanmäßig Stufe 3
    {"LUEFTERSTUFE_ZULUFT_1", FREQ_60MIN, cm_kessel},         // 0x0576 Lüfterstufe Zuluft 1
    {"LUEFTERSTUFE_ZULUFT_2", FREQ_60MIN, cm_kessel},          // 0x0577 Lüfterstufe Zuluft 2
    {"LUEFTERSTUFE_ZULUFT_3", FREQ_60MIN, cm_kessel},          // 0x0578 Lüfterstufe Zuluft 3
    {"LUEFTERSTUFE_ABLUFT_1", FREQ_60MIN, cm_kessel},          // 0x0579 Lüfterstufe Abluft 1
    {"LUEFTERSTUFE_ABLUFT_2", FREQ_60MIN, cm_kessel},          // 0x057A Lüfterstufe Abluft 2
    {"LUEFTERSTUFE_ABLUFT_3", FREQ_60MIN, cm_kessel},          // 0x057B Lüfterstufe Abluft 3
    {"LUEFTUNG_FEUCHTE_SCHWELLWERT", FREQ_60MIN, cm_kessel},               // 0x0650 Feuchte Schwellwert
    {"FEUCHTE_HYSTERESE", FREQ_60MIN, cm_kessel},            // 0x008E Feuchte Hysterese
    {"LUEFTUNG_FEUCHTE_MASKIERZEIT", FREQ_60MIN, cm_kessel},               // 0x064F Feuchte Maskierzeit
    {"MIN_SOLL_FEUCHTE", FREQ_60MIN, cm_kessel},             // 0x09D2 Feuchtesoll Minimum
    {"LUEFTUNG_LEISTUNGSREDUKTION", FREQ_60MIN, cm_kessel},               // 0x06A4 Leistungsreduktion
    {"MAX_SOLL_FEUCHTE", FREQ_60MIN, cm_kessel},             // 0x09D3 Fechtesoll Maximum
    {"LUEFTUNG_LEISTUNGSERHOEHUNG", FREQ_60MIN, cm_kessel},              // 0x06A5 Leistungserhöhung
    {"MAXIMALE_ABTAUDAUER", FREQ_60MIN, cm_kessel},                // 0x057D Maximale Abtaudauer
    {"ABTAUBEGINNSCHWELLE", FREQ_60MIN, cm_kessel},               // 0x057E Abtaubeginnschwelle
    {"DREHZAHLFILTER", FREQ_60MIN, cm_kessel},          // 0x057F Drehzahlfilter
    {"PASSIVKUEHLUNG_FORTLUFT", FREQ_60MIN, cm_kessel},                   // 0x05DB Passivkühlung Fortluft
    {"FEUCHTEREGELUNG_EIN_AUS", FREQ_60MIN, cm_kessel},      // 0x09D1 Feuchteschutz
    {"PASSIVKUEHLUNG", FREQ_60MIN, cm_kessel}, // 0x0575 Passivkühlung
    {"LUEFTEN", FREQ_60MIN, cm_kessel},          // 0x057C Lüften

    // --- validated: dwh ---
    {"WW_SOLL_TAG", FREQ_60MIN, cm_kessel},   // 0x0013 Warmwasser Soll Tag
    {"WW_SOLL_NACHT", FREQ_60MIN, cm_kessel}, // 0x05BF Warmwasser Soll Nacht
    {"WW_SOLL_BEREITSCHAFT", FREQ_60MIN, cm_kessel},       // 0x0581 Warmwasser Soll Bereitschaft
    {"WW_SOLL_HANDBETRIEB", FREQ_60MIN, cm_kessel},      // 0x0580 Warmwasser Soll Handbetrieb
    {"WW_HYSTERSE", FREQ_60MIN, cm_kessel},                 // 0x0140 Hysterese
    {"WW_ZEITSPERREN_NOTHEIZUNG", FREQ_60MIN, cm_kessel},                // 0x0588 Zeitsperren Notheizung
    {"TEMPERATUR_FREIGABE_NOTHEIZUNG", FREQ_60MIN, cm_kessel},         // 0x0589 Temperatur Freigabe Notheizung
    {"ANTILEGIONELLEN_INTERVALL", FREQ_60MIN, cm_kessel},         // 0x0586 Antilegionellen
    {"MAX_WW_LADEZEIT", FREQ_60MIN, cm_kessel},             // 0x0180 Maximale Dauer Warmwasser-Erzeugung
    {"WW_TEMPERATUR_LEGIONELLEN", FREQ_60MIN, cm_kessel},      // 0x0587 Warmwasser Temperatur Legionellen
    {"NOTHEIZUNG_STUFE_WW", FREQ_60MIN, cm_kessel},    // 0x058A Notheizung Stufe Warmwasser
    {"MAX_VORLAUFTEMPERATUR_WW", FREQ_60MIN, cm_kessel},      // 0x058C Maximale Vorlauftemperatur Warmwasser
    {"WW_LEISTUNG_SOMMER", FREQ_60MIN, cm_kessel},              // 0x0690 Warmwasser Leistung Sommer
    {"WW_LEISTUNG_WINTER", FREQ_60MIN, cm_kessel},              // 0x0691 Warmwasser Leistung Winter
    {"WW_PUFFERBETRIEB", FREQ_60MIN, cm_kessel},        // 0x058B Warmwasser Pufferbetrieb
    {"WW_ECO_BETRIEB", FREQ_60MIN, cm_kessel},      // 0x058D Warmwasser Eco
    {"ANTILEGIONELLEN_ZEIT", FREQ_60MIN, cm_kessel},              // 0x0666 Antilegionellen Zeit

    // --- validated: time ---
    {"SOMMERZEIT_MANUELL_BEGINN_TAG", FREQ_60MIN, cm_kessel},      // 0xFDDE Sommerzeit Manuell Beginn (Tag)
    {"SOMMERZEIT_MANUELL_ENDE_TAG", FREQ_60MIN, cm_kessel},       // 0xFDE0 Sommerzeit Manuell Ende (Tag)
    {"SOMMERZEIT_AUTOMATIK", FREQ_60MIN, cm_kessel}, // 0x0944 Sommerzeit Automatik
    {"SOMMERZEIT_MANUELL_BEGINN_MONAT", FREQ_60MIN, cm_kessel},     // 0xFDDF Sommerzeit Manuell Beginn (Monat)
    {"SOMMERZEIT_MANUELL_ENDE_MONAT", FREQ_60MIN, cm_kessel},       // 0xFDE1 Sommerzeit Manuell Ende (Monat)

    // --- validated: setting ---
    {"VERDICHTER_TAKTUNG", FREQ_60MIN, cm_kessel},         // 0x05B5 Verdichter Taktung
    {"DREHZAHL_FORTLUEFTUNG", FREQ_60MIN, cm_kessel}, // 0x05B6 Drehzahl Fortlüftung
    {"PUMPENDREHZAHL_WW", FREQ_60MIN, cm_kessel},        // 0x02CC Pumpendrehzahl Warmwasser
    {"PUMPENDREHZAHL_HEIZEN", FREQ_60MIN, cm_kessel},  // 0x02CB Pumpendrehzahl Heizen
    {"MIN_PUMPZYKLEN", FREQ_60MIN, cm_kessel},          // 0x05B8 Minimale Zyklen
    {"MAX_PUMPZYKLEN", FREQ_60MIN, cm_kessel}, // 0x05B7 Maximale Zyklen
    {"AUSSENTEMPERATUR_MIN_PUMPZYKLEN", FREQ_60MIN, cm_kessel},  // 0x05BA Außentemperatur Minimale Zyklen
    {"AUSSENTEMPERATUR_MAX_PUMPZYKLEN", FREQ_60MIN, cm_kessel},          // 0x05B9 Außentemperatur Maximale Zyklen
    {"EVU", FREQ_60MIN, cm_kessel},                       // 0x01D3 EVU-Sperre
    {"NOTBETRIEB_AUTO", FREQ_60MIN, cm_kessel},            // 0x065A Notbetrieb Auto

    // --- validated: heating ---
    {"HEIZEN_MAXIMALE_NE_STUFE", FREQ_60MIN, cm_kessel},            // 0x059F Maximale NE-Stufe HZ
    {"HEIZEN_MAXIMALE_VORLAUFTEMPERATUR", FREQ_60MIN, cm_kessel},                     // 0x0027 Maximale Vorlauftemperatur Heizung
    {"HEIZGRENZE_TAG", FREQ_60MIN, cm_kessel},                 // 0x0116 Sommerbetrieb
    {"AUSSENTEMPVERZOEGERUNG", FREQ_60MIN, cm_kessel},         // 0x010C Dämpfung Außentemperatur
    {"BIVALENTPARALLELTEMPERATUR_HZG", FREQ_60MIN, cm_kessel}, // 0x01AC Bivalenzpunkt
    {"ZEITSPERRE_NOTHEIZUNG", FREQ_60MIN, cm_kessel},              // 0x05A0 Zeitsperre Notheizung
    {"WE_STB_TESTFUNKTION_16", FREQ_60MIN, cm_kessel},         // 0x0611 Unterdrückung Temperatur Messung

    // --- validated: program ---
    {"FERIENANFANG_TAG", FREQ_60MIN, cm_kessel},                 // 0x011B Ferienprogramm Ferienbeginn Tag
    {"FERIENANFANG_MONAT", FREQ_60MIN, cm_kessel},               // 0x011C Ferienprogramm Ferienbeginn Monat
    {"FERIENANFANG_JAHR", FREQ_60MIN, cm_kessel},                // 0x011D Ferienprogramm Ferienbeginn Jahr
    {"FERIENENDE_TAG", FREQ_60MIN, cm_kessel},                   // 0x011E Ferienprogramm Ferienende Tag
    {"FERIENENDE_MONAT", FREQ_60MIN, cm_kessel},                 // 0x011F Ferienprogramm Ferienende Monat
    {"FERIENENDE_JAHR", FREQ_60MIN, cm_kessel},                  // 0x0120 Ferienprogramm Ferienende Jahr
    {"MAX_VORVERLEGUNG", FREQ_60MIN, cm_kessel},                 // 0x0110 Max Vorverlegung
    {"HEIZPROG_1_MO_SO", FREQ_60MIN, cm_bedienmodul_2},          // 0x14A0 Heizprogramm Heizkreis 1 Mo-So Zeitspanne 1
    {"HEIZPROG_1_MO_SO_SCHALT_2", FREQ_60MIN, cm_bedienmodul_2}, // 0x14A1 Heizprogramm Heizkreis 1 Mo-So Zeitspanne 2
    {"HEIZPROG_1_MO_SO_SCHALT_3", FREQ_60MIN, cm_bedienmodul_2}, // 0x14A2 Heizprogramm Heizkreis 1 Mo-So Zeitspanne 3
    {"HEIZPROG_1_MO", FREQ_60MIN, cm_bedienmodul_2},             // 0x1410 Heizprogramm Heizkreis 1 Montag Zeitspanne 1
    {"HEIZPROG_1_MO_SCHALT_2", FREQ_60MIN, cm_bedienmodul_2},    // 0x1411 Heizprogramm Heizkreis 1 Montag Zeitspanne 2
    {"HEIZPROG_1_MO_SCHALT_3", FREQ_60MIN, cm_bedienmodul_2},    // 0x1412 Heizprogramm Heizkreis 1 Montag Zeitspanne 3
    {"HEIZPROG_1_DI", FREQ_60MIN, cm_bedienmodul_2},             // 0x1420 Heizprogramm Heizkreis 1 Dienstag Zeitspanne 1
    {"HEIZPROG_1_DI_SCHALT_2", FREQ_60MIN, cm_bedienmodul_2},    // 0x1421 Heizprogramm Heizkreis 1 Dienstag Zeitspanne 2
    {"HEIZPROG_1_DI_SCHALT_3", FREQ_60MIN, cm_bedienmodul_2},    // 0x1422 Heizprogramm Heizkreis 1 Dienstag Zeitspanne 3
    {"HEIZPROG_1_MI", FREQ_60MIN, cm_bedienmodul_2},             // 0x1430 Heizprogramm Heizkreis 1 Mittwoch Zeitspanne 1
    {"HEIZPROG_1_MI_SCHALT_2", FREQ_60MIN, cm_bedienmodul_2},    // 0x1431 Heizprogramm Heizkreis 1 Mittwoch Zeitspanne 2
    {"HEIZPROG_1_MI_SCHALT_3", FREQ_60MIN, cm_bedienmodul_2},    // 0x1432 Heizprogramm Heizkreis 1 Mittwoch Zeitspanne 3
    {"HEIZPROG_1_DO", FREQ_60MIN, cm_bedienmodul_2},             // 0x1440 Heizprogramm Heizkreis 1 Donnerstag Zeitspanne 1
    {"HEIZPROG_1_DO_SCHALT_2", FREQ_60MIN, cm_bedienmodul_2},    // 0x1441 Heizprogramm Heizkreis 1 Donnerstag Zeitspanne 2
    {"HEIZPROG_1_DO_SCHALT_3", FREQ_60MIN, cm_bedienmodul_2},    // 0x1442 Heizprogramm Heizkreis 1 Donnerstag Zeitspanne 3
    {"HEIZPROG_1_FR", FREQ_60MIN, cm_bedienmodul_2},             // 0x1450 Heizprogramm Heizkreis 1 Freitag Zeitspanne 1
    {"HEIZPROG_1_FR_SCHALT_2", FREQ_60MIN, cm_bedienmodul_2},    // 0x1451 Heizprogramm Heizkreis 1 Freitag Zeitspanne 2
    {"HEIZPROG_1_FR_SCHALT_3", FREQ_60MIN, cm_bedienmodul_2},    // 0x1452 Heizprogramm Heizkreis 1 Freitag Zeitspanne 3
    {"HEIZPROG_1_SA", FREQ_60MIN, cm_bedienmodul_2},             // 0x1460 Heizprogramm Heizkreis 1 Samstag Zeitspanne 1
    {"HEIZPROG_1_SA_SCHALT_2", FREQ_60MIN, cm_bedienmodul_2},    // 0x1461 Heizprogramm Heizkreis 1 Samstag Zeitspanne 2
    {"HEIZPROG_1_SA_SCHALT_3", FREQ_60MIN, cm_bedienmodul_2},    // 0x1462 Heizprogramm Heizkreis 1 Samstag Zeitspanne 3
    {"HEIZPROG_1_SO", FREQ_60MIN, cm_bedienmodul_2},             // 0x1470 Heizprogramm Heizkreis 1 Sonntag Zeitspanne 1
    {"HEIZPROG_1_SO_SCHALT_2", FREQ_60MIN, cm_bedienmodul_2},    // 0x1471 Heizprogramm Heizkreis 1 Sonntag Zeitspanne 2
    {"HEIZPROG_1_SO_SCHALT_3", FREQ_60MIN, cm_bedienmodul_2},    // 0x1472 Heizprogramm Heizkreis 1 Sonntag Zeitspanne 3
    {"HEIZPROG_1_MO_FR", FREQ_60MIN, cm_bedienmodul_2},          // 0x1480 Heizprogramm Heizkreis 1 Mo-Fr Zeitspanne 1
    {"HEIZPROG_1_MO_FR_SCHALT_2", FREQ_60MIN, cm_bedienmodul_2}, // 0x1481 Heizprogramm Heizkreis 1 Mo-Fr Zeitspanne 2
    {"HEIZPROG_1_MO_FR_SCHALT_3", FREQ_60MIN, cm_bedienmodul_2}, // 0x1482 Heizprogramm Heizkreis 1 Mo-Fr Zeitspanne 3
    {"HEIZPROG_1_SA_SO", FREQ_60MIN, cm_bedienmodul_2},          // 0x1490 Heizprogramm Heizkreis 1 Sa-So Zeitspanne 1
    {"HEIZPROG_1_SA_SO_SCHALT_2", FREQ_60MIN, cm_bedienmodul_2}, // 0x1491 Heizprogramm Heizkreis 1 Sa-So Zeitspanne 2
    {"HEIZPROG_1_SA_SO_SCHALT_3", FREQ_60MIN, cm_bedienmodul_2}, // 0x1492 Heizprogramm Heizkreis 1 Sa-So Zeitspanne 3
    {"W_WASSERPROG_1_MO_SO", FREQ_60MIN, cm_kessel},             // 0x17A0 Warmwasserprogramm Mo-So Zeitspanne 1
    {"W_WASSERPROG_1_MO_SO_SCHALT_2", FREQ_60MIN, cm_kessel},    // 0x17A1 Warmwasserprogramm Mo-So Zeitspanne 2
    {"W_WASSERPROG_1_MO_SO_SCHALT_3", FREQ_60MIN, cm_kessel},    // 0x17A2 Warmwasserprogramm Mo-So Zeitspanne 3
    {"W_WASSERPROG_1_MO", FREQ_60MIN, cm_kessel},                // 0x1710 Warmwasserprogramm Montag Zeitspanne 1
    {"W_WASSERPROG_1_MO_SCHALT_2", FREQ_60MIN, cm_kessel},       // 0x1711 Warmwasserprogramm Montag Zeitspanne 2
    {"W_WASSERPROG_1_MO_SCHALT_3", FREQ_60MIN, cm_kessel},       // 0x1712 Warmwasserprogramm Montag Zeitspanne 3
    {"W_WASSERPROG_1_DI", FREQ_60MIN, cm_kessel},                // 0x1720 Warmwasserprogramm Dienstag Zeitspanne 1
    {"W_WASSERPROG_1_DI_SCHALT_2", FREQ_60MIN, cm_kessel},       // 0x1721 Warmwasserprogramm Dienstag Zeitspanne 2
    {"W_WASSERPROG_1_DI_SCHALT_3", FREQ_60MIN, cm_kessel},       // 0x1722 Warmwasserprogramm Dienstag Zeitspanne 3
    {"W_WASSERPROG_1_MI", FREQ_60MIN, cm_kessel},                // 0x1730 Warmwasserprogramm Mittwoch Zeitspanne 1
    {"W_WASSERPROG_1_MI_SCHALT_2", FREQ_60MIN, cm_kessel},       // 0x1731 Warmwasserprogramm Mittwoch Zeitspanne 2
    {"W_WASSERPROG_1_MI_SCHALT_3", FREQ_60MIN, cm_kessel},       // 0x1732 Warmwasserprogramm Mittwoch Zeitspanne 3
    {"W_WASSERPROG_1_DO", FREQ_60MIN, cm_kessel},                // 0x1740 Warmwasserprogramm Donnerstag Zeitspanne 1
    {"W_WASSERPROG_1_DO_SCHALT_2", FREQ_60MIN, cm_kessel},       // 0x1741 Warmwasserprogramm Donnerstag Zeitspanne 2
    {"W_WASSERPROG_1_DO_SCHALT_3", FREQ_60MIN, cm_kessel},       // 0x1742 Warmwasserprogramm Donnerstag Zeitspanne 3
    {"W_WASSERPROG_1_FR", FREQ_60MIN, cm_kessel},                // 0x1750 Warmwasserprogramm Freitag Zeitspanne 1
    {"W_WASSERPROG_1_FR_SCHALT_2", FREQ_60MIN, cm_kessel},       // 0x1751 Warmwasserprogramm Freitag Zeitspanne 2
    {"W_WASSERPROG_1_FR_SCHALT_3", FREQ_60MIN, cm_kessel},       // 0x1752 Warmwasserprogramm Freitag Zeitspanne 3
    {"W_WASSERPROG_1_SA", FREQ_60MIN, cm_kessel},                // 0x1760 Warmwasserprogramm Samstag Zeitspanne 1
    {"W_WASSERPROG_1_SA_SCHALT_2", FREQ_60MIN, cm_kessel},       // 0x1761 Warmwasserprogramm Samstag Zeitspanne 2
    {"W_WASSERPROG_1_SA_SCHALT_3", FREQ_60MIN, cm_kessel},       // 0x1762 Warmwasserprogramm Samstag Zeitspanne 3
    {"W_WASSERPROG_1_SO", FREQ_60MIN, cm_kessel},                // 0x1770 Warmwasserprogramm Sonntag Zeitspanne 1
    {"W_WASSERPROG_1_SO_SCHALT_2", FREQ_60MIN, cm_kessel},       // 0x1771 Warmwasserprogramm Sonntag Zeitspanne 2
    {"W_WASSERPROG_1_SO_SCHALT_3", FREQ_60MIN, cm_kessel},       // 0x1772 Warmwasserprogramm Sonntag Zeitspanne 3
    {"W_WASSERPROG_1_MO_FR", FREQ_60MIN, cm_kessel},             // 0x1780 Warmwasserprogramm Mo-Fr Zeitspanne 1
    {"W_WASSERPROG_1_MO_FR_SCHALT_2", FREQ_60MIN, cm_kessel},    // 0x1781 Warmwasserprogramm Mo-Fr Zeitspanne 2
    {"W_WASSERPROG_1_MO_FR_SCHALT_3", FREQ_60MIN, cm_kessel},    // 0x1782 Warmwasserprogramm Mo-Fr Zeitspanne 3
    {"W_WASSERPROG_1_SA_SO", FREQ_60MIN, cm_kessel},             // 0x1790 Warmwasserprogramm Sa-So Zeitspanne 1
    {"W_WASSERPROG_1_SA_SO_SCHALT_2", FREQ_60MIN, cm_kessel},    // 0x1791 Warmwasserprogramm Sa-So Zeitspanne 2
    {"W_WASSERPROG_1_SA_SO_SCHALT_3", FREQ_60MIN, cm_kessel},    // 0x1792 Warmwasserprogramm Sa-So Zeitspanne 3
    {"ZBV_PROG_1_MO_SO", FREQ_60MIN, cm_kessel},                 // 0x1DA0 Lüfterprogramm Mo-So Zeitspanne 1
    {"ZBV_PROG_1_MO_SO_SCHALT_2", FREQ_60MIN, cm_kessel},        // 0x1DA1 Lüfterprogramm Mo-So Zeitspanne 2
    {"ZBV_PROG_1_MO_SO_SCHALT_3", FREQ_60MIN, cm_kessel},        // 0x1DA2 Lüfterprogramm Mo-So Zeitspanne 3
    {"ZBV_PROG_1_MO", FREQ_60MIN, cm_kessel},                    // 0x1D10 Lüfterprogramm Montag Zeitspanne 1
    {"ZBV_PROG_1_MO_SCHALT_2", FREQ_60MIN, cm_kessel},           // 0x1D11 Lüfterprogramm Montag Zeitspanne 2
    {"ZBV_PROG_1_MO_SCHALT_3", FREQ_60MIN, cm_kessel},           // 0x1D12 Lüfterprogramm Montag Zeitspanne 3
    {"ZBV_PROG_1_DI", FREQ_60MIN, cm_kessel},                    // 0x1D20 Lüfterprogramm Dienstag Zeitspanne 1
    {"ZBV_PROG_1_DI_SCHALT_2", FREQ_60MIN, cm_kessel},           // 0x1D21 Lüfterprogramm Dienstag Zeitspanne 2
    {"ZBV_PROG_1_DI_SCHALT_3", FREQ_60MIN, cm_kessel},           // 0x1D22 Lüfterprogramm Dienstag Zeitspanne 3
    {"ZBV_PROG_1_MI", FREQ_60MIN, cm_kessel},                    // 0x1D30 Lüfterprogramm Mittwoch Zeitspanne 1
    {"ZBV_PROG_1_MI_SCHALT_2", FREQ_60MIN, cm_kessel},           // 0x1D31 Lüfterprogramm Mittwoch Zeitspanne 2
    {"ZBV_PROG_1_MI_SCHALT_3", FREQ_60MIN, cm_kessel},           // 0x1D32 Lüfterprogramm Mittwoch Zeitspanne 3
    {"ZBV_PROG_1_DO", FREQ_60MIN, cm_kessel},                    // 0x1D40 Lüfterprogramm Donnerstag Zeitspanne 1
    {"ZBV_PROG_1_DO_SCHALT_2", FREQ_60MIN, cm_kessel},           // 0x1D41 Lüfterprogramm Donnerstag Zeitspanne 2
    {"ZBV_PROG_1_DO_SCHALT_3", FREQ_60MIN, cm_kessel},           // 0x1D42 Lüfterprogramm Donnerstag Zeitspanne 3
    {"ZBV_PROG_1_FR", FREQ_60MIN, cm_kessel},                    // 0x1D50 Lüfterprogramm Freitag Zeitspanne 1
    {"ZBV_PROG_1_FR_SCHALT_2", FREQ_60MIN, cm_kessel},           // 0x1D51 Lüfterprogramm Freitag Zeitspanne 2
    {"ZBV_PROG_1_FR_SCHALT_3", FREQ_60MIN, cm_kessel},           // 0x1D52 Lüfterprogramm Freitag Zeitspanne 3
    {"ZBV_PROG_1_SA", FREQ_60MIN, cm_kessel},                    // 0x1D60 Lüfterprogramm Samstag Zeitspanne 1
    {"ZBV_PROG_1_SA_SCHALT_2", FREQ_60MIN, cm_kessel},           // 0x1D61 Lüfterprogramm Samstag Zeitspanne 2
    {"ZBV_PROG_1_SA_SCHALT_3", FREQ_60MIN, cm_kessel},           // 0x1D62 Lüfterprogramm Samstag Zeitspanne 3
    {"ZBV_PROG_1_SO", FREQ_60MIN, cm_kessel},                    // 0x1D70 Lüfterprogramm Sonntag Zeitspanne 1
    {"ZBV_PROG_1_SO_SCHALT_2", FREQ_60MIN, cm_kessel},           // 0x1D71 Lüfterprogramm Sonntag Zeitspanne 2
    {"ZBV_PROG_1_SO_SCHALT_3", FREQ_60MIN, cm_kessel},           // 0x1D72 Lüfterprogramm Sonntag Zeitspanne 3
    {"ZBV_PROG_1_MO_FR", FREQ_60MIN, cm_kessel},                 // 0x1D80 Lüfterprogramm Mo-Fr Zeitspanne 1
    {"ZBV_PROG_1_MO_FR_SCHALT_2", FREQ_60MIN, cm_kessel},        // 0x1D81 Lüfterprogramm Mo-Fr Zeitspanne 2
    {"ZBV_PROG_1_MO_FR_SCHALT_3", FREQ_60MIN, cm_kessel},        // 0x1D82 Lüfterprogramm Mo-Fr Zeitspanne 3
    {"ZBV_PROG_1_SA_SO", FREQ_60MIN, cm_kessel},                 // 0x1D90 Lüfterprogramm Sa-So Zeitspanne 1
    {"ZBV_PROG_1_SA_SO_SCHALT_2", FREQ_60MIN, cm_kessel},        // 0x1D91 Lüfterprogramm Sa-So Zeitspanne 2
    {"ZBV_PROG_1_SA_SO_SCHALT_3", FREQ_60MIN, cm_kessel},        // 0x1D92 Lüfterprogramm Sa-So Zeitspanne 3
    {"FERIEN_BEGINN_ZEIT", FREQ_60MIN, cm_kessel},               // 0x05D3 Ferienprogramm Ferienbeginn Zeit
    {"FERIEN_ENDE_ZEIT", FREQ_60MIN, cm_kessel},            // 0x05D4 Ferienprogramm Ferienende Zeit
    {"PARTYPROGRAMM_START", FREQ_60MIN, cm_kessel},                   // 0x05D1 Partyprogramm Start
    {"PARTYPROGRAMM_ENDE", FREQ_60MIN, cm_kessel},             // 0x05D2 Partyprogramm Ende

    // --- validated: concrete ---
    {"ESTRICH_SOCKELTEMPERATUR", FREQ_60MIN, cm_kessel},        // 0x01BB Sockeltemperatur
    {"ESTRICH_MAX_TEMPERATUR", FREQ_60MIN, cm_kessel},          // 0x01BD Maximale Temperatur
    {"ESTRICH_HALTEN_SOCKELTEMPERATUR", FREQ_60MIN, cm_kessel}, // 0x01BC Dauer Sockeltemperatur
    {"ESTRICH_HALTEN_MAX_TEMPERATUR", FREQ_60MIN, cm_kessel},   // 0x01BE Dauer Maximale Temperatur
    {"ESTRICH_STEIGUNG_PRO_TAG", FREQ_60MIN, cm_kessel},        // 0x01BA Steigung
    {"ESTRICHFUNKTION", FREQ_60MIN, cm_kessel},                 // 0x011A Trockenheizprogramm

    // --- validated: controller ---
    {"RAUMFUEHLERKORREKTUR", FREQ_60MIN, cm_kessel}, // 0x0109 Korrektur Raumtemperatur Bedienteil

    // --- validated: defrost ---
    {"TEMPERATUR_ABTAUENDE", FREQ_60MIN, cm_kessel}, // 0x05B2 Temperatur Abtauende
    {"RELATIVE_ABTAUDAUER2", FREQ_60MIN, cm_kessel},   // 0x03AB Maximale Abtaudauer
    {"EINFRIERSCHUTZ_NOTHEIZUNG", FREQ_60MIN, cm_kessel},         // 0x05B3 Einfrierschutz Notheizung
    {"ABTAUABBRUCH", FREQ_60MIN, cm_kessel},        // 0x05B4 Abtauabbruch

    // --- validated: manualswitch ---
    {"STELLVENTIL_HEIZUNG_MANUELL", FREQ_60MIN, cm_kessel},             // 0x0652 Stellventil Heizung - Manuell
    {"STELLVENTIL_WARMWASSER_MANUELL", FREQ_60MIN, cm_kessel},             // 0x0653 Stellventil Warmwasser - Manuell
    {"MISCHERPUMPE_MANUELL", FREQ_60MIN, cm_kessel},             // 0x0654 Mischerpumpe - Manuell
    {"MISCHER_AUF_MANUELL", FREQ_60MIN, cm_kessel},                        // 0x0056 Mischer auf - Manuell
    {"MISCHER_ZU_MANUELL", FREQ_60MIN, cm_kessel},                // 0x0057 Mischer zu - Manuell
    {"NOTHEIZUNG_STUFE_1_MANUELL", FREQ_60MIN, cm_kessel},         // 0x05AA Notheizung Stufe - Manuell
    {"UMSCHALTVENTIL_MANUELL", FREQ_60MIN, cm_kessel},   // 0x05B1 Umschaltventil - Manuell
    {"DREHZAHL_ABLUEFTER_MANUELL", FREQ_60MIN, cm_kessel},          // 0x05AB Drehzahl Ablüfter - Manuell
    {"DREHZAHL_ZULUEFTER_MANUELL", FREQ_60MIN, cm_kessel}, // 0x05AC Drehzahl Zulüfter - Manuell
    {"FENSTER_AUF_MANUELL", FREQ_60MIN, cm_kessel},        // 0x05AF Fenster auf - Manuell

    // --- validated, added to ElsterTable.h ---
    {"EXV_OEFFNUNGSGRAD_KUEHLEN", FREQ_1MIN, cm_kessel},    // 0xC0EE Öffnung EXV Kühlen
    {"ANLAUFSTROMBEGRENZUNG", FREQ_60MIN, cm_kessel},       // 0xC0AD Anlaufstrombegrenzung
    {"SOLAR_P_TYP", FREQ_60MIN, cm_kessel},                 // 0xC0E1 Solar-P Typ
    {"MISCHER_P_TYP", FREQ_60MIN, cm_kessel},               // 0xC0E2 Mischer-P Typ
    {"VERHAELTNIS_MISCHER_HEIZEN", FREQ_60MIN, cm_kessel},  // 0xC0E3 Verhältniss Mischer-Heizen
    {"LUEFTERREGELUNG", FREQ_60MIN, cm_kessel},             // 0xC0FA Lüfterregelung
    {"DIFFERENZDRUCK_SOLL", FREQ_60MIN, cm_kessel},         // 0xC0FB Differenzdruck Sollwert
    {"I_FAKTOR_LUEFTER", FREQ_60MIN, cm_kessel},            // 0xC0FC I-Faktor
    {"P_FAKTOR_LUEFTER", FREQ_60MIN, cm_kessel},            // 0xC0FD P-Faktor
    {"FILTERWECHSEL_VOLUMEN", FREQ_60MIN, cm_kessel},       // 0xC0FE Filterwechsel-Volumen
    {"PROPORTIONALANTEIL_HEIZEN", FREQ_60MIN, cm_kessel},   // 0xC0DF Proportionaler Anteil
    {"INTEGRALANTEIL_INVERTER", FREQ_60MIN, cm_kessel},     // 0xC0E0 Integralanteil Inverter
    {"AUSLEGUNGSTEMPERATUR_HEIZEN", FREQ_60MIN, cm_kessel}, // 0xC0D6 Auslegungstemperatur Heizen
    {"LEISTUNG_AUSLEGUNG_HEIZEN", FREQ_60MIN, cm_kessel},   // 0xC0F1 Leistung Auslegung Heizen
    {"LUFTSTOP_SOMMER_WW", FREQ_60MIN, cm_kessel},          // 0xC0F2 Luftstop Sommer Warmwasser
    {"LUEFTUNG_FREIGABE", FREQ_60MIN, cm_kessel},           // 0xC113 Lüftung
    {"INTEGRALSENSOR", FREQ_60MIN, cm_kessel},              // 0xC0DB Integralsensor
    {"ZWEITER_WW_SPEICHER", FREQ_60MIN, cm_kessel},         // 0xC0DE 2. WW-Speicher
    {"VERDICHTER_MANUELL", FREQ_60MIN, cm_kessel},          // 0x05A9 Verdichter - Manuell

    // --- states of the writable buttons (read-only polling) ---
    {"LUEFTER_FILTER_RESET", FREQ_60MIN, cm_kessel},    // 0x033B Lüfter Filter Reset
    {"FEHLERSPEICHER_LOESCHEN", FREQ_60MIN, cm_kessel}, // 0x027A Fehlerspeicher löschen
    {"RESET_REGLER", FREQ_60MIN, cm_kessel},            // 0x00FB Reset Regler
    {"WERKSEINSTELLUNG", FREQ_60MIN, cm_kessel},        // 0x00EF Werkseinstellung

    // --- answered by KESSEL in the read-only probe of 2026-10-02, meaning verified via tmp/StiebelEltron.json ---
    {"SOLARDIFFERENZ", FREQ_60MIN, cm_kessel}, // 0x01B8 Temperatur Differenz (Solar)
    {"KOLLEKTORGRENZTEMPERATUR", FREQ_60MIN, cm_kessel}, // 0x02F6 Kollektorgrenztemperatur
    {"KOLLEKTORSCHUTZ", FREQ_60MIN, cm_kessel}, // 0x02F5 Kollektorschutz
    {"KOLLEKTORSPERRTEMPERATUR", FREQ_60MIN, cm_kessel}, // 0x03B8 Kollektorsperrtemperatur
    {"KOLLEKTORSCHUTZTEMPERATUR", FREQ_60MIN, cm_kessel}, // 0x03BB Kollektorschutztemperatur
    {"FREIGABE_SOLAR", FREQ_60MIN, cm_kessel}, // 0x03C1 Freigabe Solar
    {"VERZOEGERUNG_VERDICHTER_WARMWASSER", FREQ_60MIN, cm_kessel}, // 0x058E Verzögerung Verdichter Warmwasser
    {"HYSTERESE_SOLAR", FREQ_60MIN, cm_kessel}, // 0x058F Hysterese Solar
    {"SOLAR_WARMWASSER_TEMPERATUR", FREQ_60MIN, cm_kessel}, // 0x05BE Warmwasser Temperatur (Solar)
    {"VORRANG_SOLAR", FREQ_60MIN, cm_kessel}, // 0x064C Vorrang Solar
    {"KUEHLZEIT", FREQ_60MIN, cm_kessel}, // 0x064D Kühlzeit
    {"LEISTUNG_KUEHLEN", FREQ_60MIN, cm_kessel}, // 0x0692 Leistung Kühlen
    {"GERAETETYP", FREQ_60MIN, cm_kessel}, // 0x07CB Gerätetyp
    {"VERDICHTER", FREQ_60MIN, cm_kessel}, // 0x07A8 Verdichter (meaning not verified; used for calculated compressor_active)

    // --- probe signals with medium/low/no confidence: not requested (not in HA) ---
    // Names corrected for the index shift of the generic Elster table (ElsterHeatingBridge).
    // Format: signal // index, description | raw value of the probe | confidence of the research
    // {"BRENNER", FREQ_60MIN, cm_kessel}, // 0x0052 BRENNER | Wert: 0 | low; Recherche: BRENNER (unbekannt), et_little_endian
    // {"SCHALTFKT_IWS", FREQ_60MIN, cm_kessel}, // 0x0060 SCHALTFKT_IWS | Wert: n/a | medium; Recherche: Abtauen Verdampfer (Status), et_bool
    // {"GERAETE_KENNUNG", FREQ_60MIN, cm_kessel}, // 0x0148 Gerätekennung | Wert: -3789 | low; Recherche: Gerätekennung
    // {"KESSELREGLER_I_ANTEIL", FREQ_60MIN, cm_kessel}, // 0x0162 Kessel Regler I Anteil | Wert: 100 | keine Info
    // {"ZUSTAND_BCC", FREQ_60MIN, cm_kessel}, // 0x0177 Zustand Bcc | Wert: 1 | keine Info
    // {"SOLARPUMPEN_STATUS_1", FREQ_60MIN, cm_kessel}, // 0x02CE Solar Pumpe N Status 1 | Wert: 0 | medium; Recherche: Solarpumpen Status 1
    // {"PERIODENDAUER_ABTAUEN", FREQ_60MIN, cm_kessel}, // 0x03A8 Perioden Dauer Abtauen | Wert: 60 | low; Recherche: Periodendauer Abtauen, min (inference)
    // {"RELATIVE_ABTAUDAUER1", FREQ_60MIN, cm_kessel}, // 0x03A9 R Elektrisch Aktive Abtau Dauer 1 | Wert: 10 | low; Recherche: Relative Abtaudauer 1
    // {"RELATIVE_ABTAUDAUER2_03AA", FREQ_60MIN, cm_kessel}, // 0x03AA RELATIVE_ABTAUDAUER2_03AA | Wert: 50 | low; Recherche: Relative Abtaudauer 2
    // {"NHZ_LEISTUNG1", FREQ_60MIN, cm_kessel}, // 0x05A1 NHZ_LEISTUNG1 | Wert: 293 | low; Recherche: NHZ Leistung 1 (?)
    // {"MANUELL_LUEFT_PARTY", FREQ_60MIN, cm_kessel}, // 0x05AD Manueller Elektrisch Lüftung Party | Wert: 0 | low; Recherche: Manuell Lüftung Party
    // {"MANUELL_LUEFT_NACHT", FREQ_60MIN, cm_kessel}, // 0x05AE MANUELL_LUEFT_NACHT | Wert: 0 | low; Recherche: Manuell Lüftung Nacht
    // {"MANUELL_HEAT_PIPE_VENTIL", FREQ_60MIN, cm_kessel}, // 0x05B0 MANUELL_HEAT_PIPE_VENTIL | Wert: 0 | low; Recherche: Manuell Heat-Pipe-Ventil
    // {"AKTIVE_HEIZSTUFEN", FREQ_60MIN, cm_kessel}, // 0x05BB Aktive Heizung Stufen | Wert: 0 | medium; Recherche: Aktive Heizstufen / Heizstufe
    // {"ABTAUEN_LL_WT_AKTIV", FREQ_60MIN, cm_kessel}, // 0x05BC Abtauen LL WT Aktiv | Wert: 0 | low; Recherche: (unklar) Abtauen LL-WT aktiv / Pumpenzyklen Außentemp. max
    // {"HYST_HEIZSTUFE1", FREQ_60MIN, cm_kessel}, // 0x05C0 Hyst Heizung Stufe 1 | Wert: 40 | medium; Recherche: Hysterese Heizstufe 1, et_dec_val (inference), K
    // {"HYST_HEIZSTUFE2", FREQ_60MIN, cm_kessel}, // 0x05C1 Hyst Heizung Stufe 2 | Wert: 30 | medium; Recherche: Hysterese Heizstufe 2, et_dec_val (inference), K
    // {"HYST_HEIZSTUFE3", FREQ_60MIN, cm_kessel}, // 0x05C2 Hyst Heizung Stufe 3 | Wert: 30 | medium; Recherche: Hysterese Heizstufe 3, et_dec_val (inference), K
    // {"HYST_HEIZSTUFE4", FREQ_60MIN, cm_kessel}, // 0x05C3 Hyst Heizung Stufe 4 | Wert: 20 | medium; Recherche: Hysterese Heizstufe 4, et_dec_val (inference), K
    // {"HYST_HEIZSTUFE5", FREQ_60MIN, cm_kessel}, // 0x05C4 Hyst Heizung Stufe 5 | Wert: 20 | medium; Recherche: Hysterese Heizstufe 5, et_dec_val (inference), K
    // {"ASYMETRIE_HEIZSTUFEN", FREQ_60MIN, cm_kessel}, // 0x05C5 Asymmetrie Heizung Stufen | Wert: 2 | medium; Recherche: Asymmetrie der Hysteresen
    // {"VOLUMENSTROMMESSUNG", FREQ_60MIN, cm_kessel}, // 0x05C6 VOLUMENSTROMMESSUNG | Wert: 0 | low; Recherche: Volumenstrommessung
    // {"TEST_OBJEKT_21", FREQ_60MIN, cm_kessel}, // 0x0655 Test Objekt 21 | Wert: 0 | keine Info
    // {"TEST_OBJEKT_24", FREQ_60MIN, cm_kessel}, // 0x0658 Test Objekt 24 | Wert: 5 | low
    // {"TEST_OBJEKT_25", FREQ_60MIN, cm_kessel}, // 0x0659 TEST_OBJEKT_25 | Wert: 120 | low
    // {"TEST_OBJEKT_31", FREQ_60MIN, cm_kessel}, // 0x065F Test Objekt 31 | Wert: 0 | keine Info
    // {"TEST_OBJEKT_32", FREQ_60MIN, cm_kessel}, // 0x0660 Test Objekt 32 | Wert: 10 | keine Info
    // {"TEST_OBJEKT_35", FREQ_60MIN, cm_kessel}, // 0x0663 Test Objekt 35 | Wert: 0 | keine Info
    // {"TEST_OBJEKT_36", FREQ_60MIN, cm_kessel}, // 0x0664 Test Objekt 36 | Wert: 0 | keine Info
    // {"TEST_OBJEKT_37", FREQ_60MIN, cm_kessel}, // 0x0665 TEST_OBJEKT_37 | Wert: 0 | low
    // {"TEST_OBJEKT_39", FREQ_60MIN, cm_kessel}, // 0x0667 Test Objekt 39 | Wert: 0 | keine Info
    // {"TEST_OBJEKT_40", FREQ_60MIN, cm_kessel}, // 0x0668 Test Objekt 40 | Wert: 177 | keine Info
    // {"TEST_OBJEKT_41", FREQ_60MIN, cm_kessel}, // 0x0669 Test Objekt 41 | Wert: 177 | keine Info
    // {"TEST_OBJEKT_42", FREQ_60MIN, cm_kessel}, // 0x066A Test Objekt 42 | Wert: 892 | keine Info
    // {"TEST_OBJEKT_43", FREQ_60MIN, cm_kessel}, // 0x066B Test Objekt 43 | Wert: 892 | keine Info
    // {"TEST_OBJEKT_44", FREQ_60MIN, cm_kessel}, // 0x066C Test Objekt 44 | Wert: 0 | keine Info
    // {"TEST_OBJEKT_45", FREQ_60MIN, cm_kessel}, // 0x066D Test Objekt 45 | Wert: 0 | keine Info
    // {"TEST_OBJEKT_46", FREQ_60MIN, cm_kessel}, // 0x066E Test Objekt 46 | Wert: 0 | keine Info
    // {"TEST_OBJEKT_47", FREQ_60MIN, cm_kessel}, // 0x066F Test Objekt 47 | Wert: 10 | low
    // {"TEST_OBJEKT_50", FREQ_60MIN, cm_kessel}, // 0x0672 Test Objekt 50 | Wert: 22 | keine Info
    // {"TEST_OBJEKT_51", FREQ_60MIN, cm_kessel}, // 0x0673 Test Objekt 51 | Wert: -8 | low
    // {"TEST_OBJEKT_52", FREQ_60MIN, cm_kessel}, // 0x0674 Test Objekt 52 | Wert: -10 | low
    // {"TEST_OBJEKT_53", FREQ_60MIN, cm_kessel}, // 0x0675 Test Objekt 53 | Wert: -10 | low
    // {"TEST_OBJEKT_54", FREQ_60MIN, cm_kessel}, // 0x0676 Test Objekt 54 | Wert: -20 | keine Info
    // {"TEST_OBJEKT_55", FREQ_60MIN, cm_kessel}, // 0x0677 Test Objekt 55 | Wert: -27 | keine Info
    // {"TEST_OBJEKT_56", FREQ_60MIN, cm_kessel}, // 0x0678 Test Objekt 56 | Wert: -10 | keine Info
    // {"TEST_OBJEKT_57", FREQ_60MIN, cm_kessel}, // 0x0679 Test Objekt 57 | Wert: -34 | keine Info
    // {"TEST_OBJEKT_58", FREQ_60MIN, cm_kessel}, // 0x067A Test Objekt 58 | Wert: -27 | keine Info
    // {"TEST_OBJEKT_59", FREQ_60MIN, cm_kessel}, // 0x067B Test Objekt 59 | Wert: -11 | keine Info
    // {"TEST_OBJEKT_60", FREQ_60MIN, cm_kessel}, // 0x067C Test Objekt 60 | Wert: -20 | keine Info
    // {"TEST_OBJEKT_61", FREQ_60MIN, cm_kessel}, // 0x067D Test Objekt 61 | Wert: -19 | low
    // {"TEST_OBJEKT_62", FREQ_60MIN, cm_kessel}, // 0x067E Test Objekt 62 | Wert: -21 | low
    // {"TEST_OBJEKT_63", FREQ_60MIN, cm_kessel}, // 0x067F Test Objekt 63 | Wert: -30 | low
    // {"TEST_OBJEKT_64", FREQ_60MIN, cm_kessel}, // 0x0680 Test Objekt 64 | Wert: -8 | keine Info
    // {"TEST_OBJEKT_65", FREQ_60MIN, cm_kessel}, // 0x0681 Test Objekt 65 | Wert: 3 | keine Info
    // {"TEST_OBJEKT_66", FREQ_60MIN, cm_kessel}, // 0x0682 Test Objekt 66 | Wert: -20 | keine Info
    // {"TEST_OBJEKT_67", FREQ_60MIN, cm_kessel}, // 0x0683 Test Objekt 67 | Wert: -9 | keine Info
    // {"TEST_OBJEKT_68", FREQ_60MIN, cm_kessel}, // 0x0684 Test Objekt 68 | Wert: -19 | keine Info
    // {"TEST_OBJEKT_69", FREQ_60MIN, cm_kessel}, // 0x0685 Test Objekt 69 | Wert: -42 | keine Info
    // {"TEST_OBJEKT_70", FREQ_60MIN, cm_kessel}, // 0x0686 Test Objekt 70 | Wert: 5210 | keine Info
    // {"TEST_OBJEKT_71", FREQ_60MIN, cm_kessel}, // 0x0687 Test Objekt 71 | Wert: 0 | keine Info
    // {"TEST_OBJEKT_72", FREQ_60MIN, cm_kessel}, // 0x0688 Test Objekt 72 | Wert: 1 | keine Info
    // {"TEST_OBJEKT_85", FREQ_60MIN, cm_kessel}, // 0x0695 Test Objekt 85 | Wert: 0 | keine Info
    // {"TEST_OBJEKT_86", FREQ_60MIN, cm_kessel}, // 0x0696 Test Objekt 86 | Wert: 0 | keine Info
    // {"TEST_OBJEKT_87", FREQ_60MIN, cm_kessel}, // 0x0697 TEST_OBJEKT_87 | Wert: 0 | keine Info
    // {"TEST_OBJEKT_102", FREQ_60MIN, cm_kessel}, // 0x06A6 Test Objekt 102 | Wert: 0 | keine Info
    // {"TEST_OBJEKT_110", FREQ_60MIN, cm_kessel}, // 0x06AE Test Objekt 110 | Wert: 0 | keine Info
    // {"TEST_OBJEKT_134", FREQ_60MIN, cm_kessel}, // 0x06C6 Test Objekt 134 | Wert: 0 | keine Info
    // {"TEST_OBJEKT_135", FREQ_60MIN, cm_kessel}, // 0x06C7 Test Objekt 135 | Wert: 0 | keine Info
    // {"TEST_OBJEKT_136", FREQ_60MIN, cm_kessel}, // 0x06C8 Test Objekt 136 | Wert: 0 | keine Info
    // {"TEST_OBJEKT_137", FREQ_60MIN, cm_kessel}, // 0x06C9 Test Objekt 137 | Wert: 0 | keine Info
    // {"STUETZSTELLE_ND1", FREQ_60MIN, cm_kessel}, // 0x079F STUETZSTELLE_ND1 | Wert: n/a | low; Recherche: Stützstelle Niederdruck 1, et_cent_val (inference), bar (inference)
    // {"STUETZSTELLE_ND2", FREQ_60MIN, cm_kessel}, // 0x07A0 STUETZSTELLE_ND2 | Wert: n/a | low; Recherche: Stützstelle Niederdruck 2, et_cent_val (inference), bar (inference)
    // {"STUETZSTELLE_HD1", FREQ_60MIN, cm_kessel}, // 0x07A1 STUETZSTELLE_HD1 | Wert: n/a | low; Recherche: Stützstelle Hochdruck 1, et_cent_val (inference), bar (inference)
    // {"STUETZSTELLE_HD2", FREQ_60MIN, cm_kessel}, // 0x07A2 STUETZSTELLE_HD2 | Wert: n/a | low; Recherche: Stützstelle Hochdruck 2, et_cent_val (inference), bar (inference)
    // {"TAU_PENDEL_ERKENNUNGSZEIT", FREQ_60MIN, cm_kessel}, // 0x07AA Tau Pendel Erkennungszeit | Wert: 120 | keine Info
    // {"VERZOEGERUNG_PENDELERKENNUNG", FREQ_60MIN, cm_kessel}, // 0x07AB Verzögerung Pendel Erkennung | Wert: 120 | keine Info
    // {"UEBERHITZUNG_VERDAMPFER_LOW", FREQ_60MIN, cm_kessel}, // 0x07AC Überhitzung Verdampfer Low | Wert: 80 | low; Recherche: Überhitzung Verdampfer low, et_dec_val (inference), K (inference)
    // {"UEBERHITZUNG_VERDAMPFER_MID", FREQ_60MIN, cm_kessel}, // 0x07AD Überhitzung Verdampfer Mid | Wert: 80 | low; Recherche: Überhitzung Verdampfer mid, et_dec_val (inference), K (inference)
    // {"UEBERHITZUNG_VERDAMPFER_HIGH", FREQ_60MIN, cm_kessel}, // 0x07AE Überhitzung Verdampfer High | Wert: 80 | low; Recherche: Überhitzung Verdampfer high, et_dec_val (inference), K (inference)
    // {"UEBERHITZUNG_REKUPERATOR", FREQ_60MIN, cm_kessel}, // 0x07AF Überhitzung Rekuperator | Wert: 110 | low; Recherche: Überhitzung Rekuperator, et_dec_val (inference), K (inference)
    // {"UEBERHITZUNG_BEI_KUEHLBETRIEB", FREQ_60MIN, cm_kessel}, // 0x07B0 Überhitzung Bei Kühlbetrieb | Wert: 110 | low; Recherche: Überhitzung bei Kühlbetrieb, et_dec_val (inference), K (inference)
    // {"ANFAHRFAKTOR_UEBERHITZUNG", FREQ_60MIN, cm_kessel}, // 0x07B1 Anfahrfaktor Überhitzung | Wert: 1000 | keine Info
    // {"ANFAHRZEIT_UEBERHITZUNG", FREQ_60MIN, cm_kessel}, // 0x07B2 Anfahrzeit Überhitzung | Wert: 120 | keine Info
    // {"GRENZE_PENDELN_VERDAMPFER", FREQ_60MIN, cm_kessel}, // 0x07B3 Grenze Pendeln Verdampfer | Wert: 30 | keine Info
    // {"GRENZE_PENDELN_REKUPERATOR", FREQ_60MIN, cm_kessel}, // 0x07B4 Grenze Pendeln Rekuperator | Wert: 40 | keine Info
    // {"TAU_UEBERHITZUNG_VERKLEINERN", FREQ_60MIN, cm_kessel}, // 0x07B5 Tau Überhitzung Verkleinern | Wert: 10 | keine Info
    // {"TAU_UEBERHITZUNG_VERGROESSERN", FREQ_60MIN, cm_kessel}, // 0x07B6 Tau Überhitzung Vergrößern | Wert: 10 | keine Info
    // {"VARIATION_UEBERHITZUNG", FREQ_60MIN, cm_kessel}, // 0x07B7 Variation Überhitzung | Wert: 25 | keine Info
    // {"ABTAUVERFAHREN", FREQ_60MIN, cm_kessel}, // 0x07E0 Abtauverfahren | Wert: 1 | keine Info
    // {"V_H_EXPONENT_VSKL", FREQ_60MIN, cm_kessel}, // 0x07B8 V H Exponent Vskl | Wert: 115 | keine Info
    // {"V_H_FAKTOR_VSKL", FREQ_60MIN, cm_kessel}, // 0x07B9 V H Faktor Vskl | Wert: 140 | keine Info
    // {"V_H_OFFSET_VSKL", FREQ_60MIN, cm_kessel}, // 0x07BA V H Offset Vskl | Wert: 8 | keine Info
    // {"V_K_EXPONENT_VSKL", FREQ_60MIN, cm_kessel}, // 0x07BB V K Exponent Vskl | Wert: 115 | keine Info
    // {"V_K_FAKTOR_VSKL", FREQ_60MIN, cm_kessel}, // 0x07BC V K Faktor Vskl | Wert: 280 | keine Info
    // {"V_K_OFFSET_VSKL", FREQ_60MIN, cm_kessel}, // 0x07BD V K Offset Vskl | Wert: 8 | keine Info
    // {"WICHTUNG_REGELABWEICHUNG", FREQ_60MIN, cm_kessel}, // 0x07BE Gewichtung Regel Abweichung | Wert: 100 | keine Info
    // {"P_VERDAMPFER", FREQ_60MIN, cm_kessel}, // 0x07BF P Verdampfer | Wert: 10 | keine Info
    // {"I_VERDAMPFER", FREQ_60MIN, cm_kessel}, // 0x07C0 I Verdampfer | Wert: 20 | keine Info
    // {"D_VERDAMPFER", FREQ_60MIN, cm_kessel}, // 0x07C1 D Verdampfer | Wert: 0 | keine Info
    // {"P_REKUPERATOR", FREQ_60MIN, cm_kessel}, // 0x07C2 P Rekuperator | Wert: 100 | keine Info
    // {"I_REKUPERATOR", FREQ_60MIN, cm_kessel}, // 0x07C3 I Rekuperator | Wert: 5 | keine Info
    // {"D_REKUPERATOR", FREQ_60MIN, cm_kessel}, // 0x07C4 D Rekuperator | Wert: 20 | keine Info
    // {"MIN_REGELDYNAMIK", FREQ_60MIN, cm_kessel}, // 0x07C5 Regel Minimum Dynamik | Wert: 20 | keine Info
    // {"TAU_REGELDYNAMIK", FREQ_60MIN, cm_kessel}, // 0x07C6 Tau Regel Dynamik | Wert: 3 | keine Info
    // {"EXV_OEFFNUNGSGRAD_MIN", FREQ_60MIN, cm_kessel}, // 0x07C7 Exv Öffnungsgrad Minimum | Wert: 30 | low; Recherche: EXV Öffnungsgrad min, et_dec_val (inference), % (inference)
    // {"EXV_OEFFNUNGSGRAD_MAX", FREQ_60MIN, cm_kessel}, // 0x07C8 Exv Öffnungsgrad Maximum | Wert: 1000 | low; Recherche: EXV Öffnungsgrad max, et_dec_val (inference), % (inference)
    // {"EXV_TOTZONE", FREQ_60MIN, cm_kessel}, // 0x07C9 Exv Totzone | Wert: 20 | keine Info
    // {"TIEFPASS_ND", FREQ_60MIN, cm_kessel}, // 0x07CC Tiefpass ND | Wert: 180 | keine Info
    // {"OEFFNUNGSGRAD_ABTAUEN", FREQ_60MIN, cm_kessel}, // 0x07CF Öffnungsgrad Abtauen | Wert: 1000 | low; Recherche: Öffnungsgrad Abtauen, et_dec_val (inference), % (inference)
    // {"DRUCKDIFFERENZ_ABTAUEN", FREQ_60MIN, cm_kessel}, // 0x07D0 Druck Differenz Abtauen | Wert: 0 | keine Info
    // {"OEFFNUNGSGRAD_HANDBETRIEB", FREQ_60MIN, cm_kessel}, // 0x07D1 Öffnungsgrad Handbetrieb | Wert: 0 | keine Info
    // {"GRENZDRUCK_PUMPDOWN", FREQ_60MIN, cm_kessel}, // 0x07D2 Grenz Druck Pumpdown | Wert: 70 | low; Recherche: Grenzdruck Pumpdown, et_dec_val (inference), bar (inference)
    // {"INJEKTION_PUMPDOWN", FREQ_60MIN, cm_kessel}, // 0x07D3 Injektion Pumpdown | Wert: 10 | keine Info
    // {"UMSCHALTUNG_VERFLUESSIGERTEMP", FREQ_60MIN, cm_kessel}, // 0x07D4 Umschaltung Verflüssiger Temperatur | Wert: 0 | keine Info
    // {"UMSCHALTUNG_TAUPUNKTTEMP", FREQ_60MIN, cm_kessel}, // 0x07D5 Umschaltung Taupunkt Temperatur | Wert: 0 | keine Info
    // {"KALIBRIERUNG_ON_OFF", FREQ_60MIN, cm_kessel}, // 0x07D6 Kalibrierung Ein Aus | Wert: 0 | keine Info
    // {"STARTZAHL_BIS_KALIBRIERUNG", FREQ_60MIN, cm_kessel}, // 0x07D7 Startzahl Bis Kalibrierung | Wert: 100 | keine Info
    // {"LAUFZEIT_BIS_KALIBRIERUNG", FREQ_60MIN, cm_kessel}, // 0x07D8 Laufzeit Bis Kalibrierung | Wert: 100 | keine Info
    // {"ARBEITSPUNKT_KALIBRIERUNG", FREQ_60MIN, cm_kessel}, // 0x07D9 Arbeitspunkt Kalibrierung | Wert: 0 | keine Info
    // {"OEG_KALIBRIERUNG_FAKTOR", FREQ_60MIN, cm_kessel}, // 0x07DA Oeg Kalibrierung Faktor | Wert: 20 | keine Info
    // {"KALIBRIERDAUER", FREQ_60MIN, cm_kessel}, // 0x07DB Kalibrier Dauer | Wert: 60 | keine Info
    // {"KALIBRIERABWEICHUNG_GRENZ", FREQ_60MIN, cm_kessel}, // 0x07DC Kalibrierabweichung Grenz | Wert: 30 | keine Info
    // {"KALIBRIERABWEICHUNG_ABSCHALTUNG", FREQ_60MIN, cm_kessel}, // 0x07DD Kalibrierabweichung Abschaltung | Wert: 60 | keine Info
    // {"FREIGABE_AUSSENTEMP_ZWISCHENEINSPRITZUNG", FREQ_60MIN, cm_kessel}, // 0x07DE Freigabe Außen Temperatur Zwischeneinspritzung | Wert: 400 | low; Recherche: Freigabe Außentemperatur Zwischeneinspritzung, et_dec_val (inference), °C (inference)
    // {"NIEDERDRUCK_MOP_ND", FREQ_60MIN, cm_kessel}, // 0x07DF Nieder Druck Mop ND | Wert: 120 | low; Recherche: Niederdruck MOP, et_dec_val (inference), bar (inference)
    // {"GRENZDRUCK_ABTAUENDE", FREQ_60MIN, cm_kessel}, // 0x07E1 Grenz Druck Abtauende | Wert: 200 | low; Recherche: Grenzdruck Abtauende, et_dec_val (inference), bar (inference)
    // {"DAEMPFUNG_TAUTEMP_ABTAUAUSLOESUNG", FREQ_60MIN, cm_kessel}, // 0x07E2 Dämpfung Tau Temperatur Abtauauslösung | Wert: 5 | keine Info
    // {"TEMPERATURDIFFERENZ_ABTAUAUSLOESUNG", FREQ_60MIN, cm_kessel}, // 0x07E3 Temperatur Differenz Abtauauslösung | Wert: 50 | low; Recherche: Temperaturdifferenz Abtauauslösung, et_dec_val (inference), K (inference)
    // {"FREIGABE_ABTAUERKENNUNG", FREQ_60MIN, cm_kessel}, // 0x07E4 Freigabe Abtauerkennung | Wert: 0 | keine Info
    // {"BEGRENZUNG_HEISSGASTEMPERATUR", FREQ_60MIN, cm_kessel}, // 0x07E5 Begrenzung Heißgas Temperatur | Wert: 1200 | low; Recherche: Begrenzung Heißgastemperatur, et_dec_val (inference), °C (inference)
    // {"P_FAKTOR_BEGRENZUNG_HEISSGASTEMPERATUR", FREQ_60MIN, cm_kessel}, // 0x07E6 P Faktor Begrenzung Heißgas Temperatur | Wert: 20 | keine Info
    // {"LUEFTERLEISTUNG_AT_MIN", FREQ_60MIN, cm_kessel}, // 0x07E7 Lüfterleistung At Minimum | Wert: 80 | low; Recherche: Lüfterleistung bei AT min, et_default (inference), % (inference)
    // {"LUEFTERLEISTUNG_AT_MAX", FREQ_60MIN, cm_kessel}, // 0x07E8 Lüfterleistung At Maximum | Wert: 60 | low; Recherche: Lüfterleistung bei AT max, et_default (inference), % (inference)
    // {"GRENZWERT_ABWEICHUNG_V_KENNLINIE", FREQ_60MIN, cm_kessel}, // 0x07E9 Grenzwert Abweichung V Kennlinie | Wert: 200 | keine Info
    // {"ZEIT_WAECHTER_ABWEICHUNG_V_KENNLINIE", FREQ_60MIN, cm_kessel}, // 0x07EA Zeit Wächter Abweichung V Kennlinie | Wert: 10 | keine Info
    // {"MINIMALER_GRENZWERT_UEBERHITZUNG", FREQ_60MIN, cm_kessel}, // 0x07EB Minimaler Grenzwert Überhitzung | Wert: 300 | keine Info
    // {"ZEIT_WAECHTER_UEBERHITZUNG", FREQ_60MIN, cm_kessel}, // 0x07EC Ueberhitzung Überhitzung | Wert: 10 | keine Info
    // {"ND_EVE_FUNKTION", FREQ_60MIN, cm_kessel}, // 0x07ED Nd Eve Funktion | Wert: 1 | keine Info
    // {"ND_EVE_GRENZWERT", FREQ_60MIN, cm_kessel}, // 0x07EE Nd Eve Grenzwert | Wert: 12 | keine Info
    // {"ND_MASKIERZEIT", FREQ_60MIN, cm_kessel}, // 0x07EF Nd Maskierzeit | Wert: 10 | keine Info
    // {"HD_EVE_FUNKTION", FREQ_60MIN, cm_kessel}, // 0x07F0 Hd Eve Funktion | Wert: 2 | keine Info
    // {"HD_EVE_GRENZWERT_VHD", FREQ_60MIN, cm_kessel}, // 0x07F1 Hd Eve Grenzwert Vhd | Wert: 420 | low; Recherche: HD EVE Grenzwert, et_dec_val (inference), bar (inference)
    // {"UEBERHITZ_ZU_LEISTUNG_K", FREQ_60MIN, cm_kessel}, // 0x0817 Überhitz Zu Leistung K | Wert: 50 | keine Info
    // {"V_OG_ZU_LEISTUNG_K", FREQ_60MIN, cm_kessel}, // 0x0818 V Og Zu Leistung K | Wert: 150 | keine Info
    // {"AUSSEN_LUEFTERLEISTUNG_AT_MIN", FREQ_60MIN, cm_kessel}, // 0x081E Außen Lüfterleistung At Minimum | Wert: -200 | low; Recherche: Außen-Lüfterleistung bei AT min (AT-Stützpunkt), et_dec_val (inference), °C (inference)
    // {"AUSSEN_LUEFTERLEISTUNG_AT_MAX", FREQ_60MIN, cm_kessel}, // 0x081F Außen Lüfterleistung At Maximum | Wert: 0 | low; Recherche: Außen-Lüfterleistung bei AT max (AT-Stützpunkt), et_dec_val (inference), °C (inference)
    // {"FUEHLERZEITKONSTANTE", FREQ_60MIN, cm_kessel}, // 0x082C Fühlerzeitkonstante | Wert: 30 | keine Info
    // {"MIN_VORLAUF_KUEHLBETRIEB", FREQ_60MIN, cm_kessel}, // 0x082A Minimum Vorlauf Kühlbetrieb | Wert: 40 | keine Info
    // {"AKT_KALIBRIERWERT_V", FREQ_60MIN, cm_kessel}, // 0x082B Akt Kalibrierwert V | Wert: 70 | keine Info
    // {"KONFIG_0_BIS_10V", FREQ_60MIN, cm_kessel}, // 0x082D Konfig 0 Bis 10 V | Wert: 10 | keine Info
    // {"EVE_GRENZWERT_KUEHLBETRIEB", FREQ_60MIN, cm_kessel}, // 0x082E Eve Grenzwert Kühlbetrieb | Wert: 50 | keine Info
    // {"MODKLAPPENLAUFZEIT", FREQ_60MIN, cm_kessel}, // 0x082F Modklappen Laufzeit | Wert: 80 | keine Info
    // {"KONFIG_0_BIS_5V", FREQ_60MIN, cm_kessel}, // 0x0830 Konfig 0 Bis 5 V | Wert: 20 | keine Info
    // {"STUETZSTELLE_MD1", FREQ_60MIN, cm_kessel}, // 0x0915 Stützstelle Md1 | Wert: 100 | low; Recherche: Stützstelle Mitteldruck 1, et_cent_val (inference), bar (inference)
    // {"STUETZSTELLE_MD2", FREQ_60MIN, cm_kessel}, // 0x0916 Stützstelle Md2 | Wert: 3000 | low; Recherche: Stützstelle Mitteldruck 2, et_cent_val (inference), bar (inference)
    // {"FEHLERSPEICHER_FELDINDEX", FREQ_60MIN, cm_kessel}, // 0x0B9A Fehler Speicher F Elektrisch Dindex | Wert: 0 | keine Info
    // {"FEHLERSPEICHER_FEHLERNUMMER", FREQ_60MIN, cm_kessel}, // 0x0B9B Fehler Speicher Fehlernummer | Wert: 0 | keine Info
    // {"FEHLERSPEICHER_MINUTE", FREQ_60MIN, cm_kessel}, // 0x0B9F Fehler Speicher Minute | Wert: 0 | keine Info
    // {"FEHLERSPEICHER_STUNDE", FREQ_60MIN, cm_kessel}, // 0x0BA0 Fehler Speicher Stunde | Wert: 0 | keine Info
    // {"FEHLERSPEICHER_TAG", FREQ_60MIN, cm_kessel}, // 0x0BA1 Fehler Speicher Tag | Wert: 0 | keine Info
    // {"FEHLERSPEICHER_MONAT", FREQ_60MIN, cm_kessel}, // 0x0BA2 Fehler Speicher Monat | Wert: 0 | keine Info
    // {"FEHLERSPEICHER_JAHR", FREQ_60MIN, cm_kessel}, // 0x0BA3 Fehler Speicher Jahr | Wert: 26 | keine Info
    // {"HF_MONITOR_TYP", FREQ_60MIN, cm_kessel}, // 0xC34F Hf Monitor Typ | Wert: 85 | keine Info
    // {"SOFTWARE_SIMULATION_0", FREQ_60MIN, cm_kessel}, // 0xC350 Software Simulation 0 | Wert: 88 | low; Recherche: Software Simulation 0
    // {"SOFTWARE_SIMULATION_1", FREQ_60MIN, cm_kessel}, // 0xC351 Software Simulation 1 | Wert: 24 | keine Info
    // {"SOFTWARE_SIMULATION_2", FREQ_60MIN, cm_kessel}, // 0xC352 Software Simulation 2 | Wert: 0 | keine Info
    // {"SOFTWARE_SIMULATION_3", FREQ_60MIN, cm_kessel}, // 0xC353 Software Simulation 3 | Wert: 750 | keine Info
    // {"SOFTWARE_SIMULATION_4", FREQ_60MIN, cm_kessel}, // 0xC354 Software Simulation 4 | Wert: 0 | keine Info
    // {"SOFTWARE_SIMULATION_5", FREQ_60MIN, cm_kessel}, // 0xC355 Software Simulation 5 | Wert: 0 | keine Info
    // {"SOFTWARE_SIMULATION_6", FREQ_60MIN, cm_kessel}, // 0xC356 Software Simulation 6 | Wert: 1 | medium; Recherche: Betriebsstatus 2 (Bitfeld), et_default (bit field)
    // {"SOFTWARE_SIMULATION_200", FREQ_60MIN, cm_kessel}, // 0xC418 Software Simulation 200 | Wert: 100 | keine Info
    // {"SOFTWARE_SIMULATION_201", FREQ_60MIN, cm_kessel}, // 0xC419 Software Simulation 201 | Wert: 0 | keine Info
    // {"SOFTWARE_SIMULATION_202", FREQ_60MIN, cm_kessel}, // 0xC41A Software Simulation 202 | Wert: 0 | keine Info
    // {"SOFTWARE_SIMULATION_203", FREQ_60MIN, cm_kessel}, // 0xC41B Software Simulation 203 | Wert: 0 | keine Info
    // {"SOFTWARE_SIMULATION_204", FREQ_60MIN, cm_kessel}, // 0xC41C Software Simulation 204 | Wert: 0 | keine Info
    // {"SOFTWARE_SIMULATION_205", FREQ_60MIN, cm_kessel}, // 0xC41D Software Simulation 205 | Wert: 0 | keine Info
    // {"SOFTWARE_SIMULATION_207", FREQ_60MIN, cm_kessel}, // 0xC41F Software Simulation 207 | Wert: 100 | keine Info

    // ========================================================================
    // PROBE SIGNALS — may or may not respond on LWZ5SPLUS
    // If the pump responds, these appear automatically in HA via MQTT discovery
    // ========================================================================

    // ========================================================================
    // COMPRESSOR RUNTIME COUNTERS (Laufzeit Verdichter)
    // Poll both HEIZMODUL and MANAGER — which one has valid data is model-dependent
    // ========================================================================

};

extern const size_t SIGNAL_REQUEST_COUNT_VALUE = sizeof(signalRequests) / sizeof(SignalRequest);

#endif // SIGNAL_REQUESTS_LWZ5SPLUS_H