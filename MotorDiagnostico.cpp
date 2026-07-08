/*
 * ============================================================
 * MotorDiagnostico.cpp
 * Motor Logico y de Soluciones
 * ============================================================
 */

#include "MotorDiagnostico.h"
#include <iostream>
#include <limits>
#include <thread>
#include <chrono>

using namespace std;

#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"

static const string LINEA_DOBLE =  CYAN + string("  ================================================================") + RESET;
static const string LINEA_SIMPLE = CYAN + string("  ----------------------------------------------------------------") + RESET;

void simularProcesamiento(const string& mensaje) {
    cout << CYAN << "\n  [*] " << mensaje;
    for(int i=0; i<4; i++) {
        this_thread::sleep_for(chrono::milliseconds(300));
        cout << ".";
    }
    cout << RESET << "\n";
}

bool preguntarSiNo(const string& pregunta) {
    char respuesta = 0;
    while (true) {
        cout << BOLD << "\n  [?] " << pregunta << RESET << " [S/N]: ";
        cin >> respuesta;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (respuesta == 'S' || respuesta == 's' || respuesta == '1') return true;
        if (respuesta == 'N' || respuesta == 'n' || respuesta == '0') return false;

        cout << RED << "  [!] Opcion invalida. Ingrese S (Si) o N (No).\n" << RESET;
    }
}

int solicitarOpcion(const string& titulo, const initializer_list<string>& opciones) {
    int maxOpc = static_cast<int>(opciones.size());
    int seleccion = 0;

    while (true) {
        cout << "\n" << LINEA_SIMPLE << "\n";
        cout << BOLD << YELLOW << "  " << titulo << RESET << "\n";
        cout << LINEA_SIMPLE << "\n";

        int idx = 1;
        for (const auto& op : opciones) {
            cout << "    " << BOLD << idx << ". " << RESET << op << "\n";
            ++idx;
        }
        cout << CYAN << "\n  Seleccione una opcion [1-" << maxOpc << "]: " << RESET;
        cin >> seleccion;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (seleccion >= 1 && seleccion <= maxOpc) {
            return seleccion - 1;
        }
        cout << RED << "  [!] Opcion fuera de rango. Intente de nuevo.\n" << RESET;
    }
}

void mostrarResultadoFinal(const ResultadoDiagnostico& resultado) {
    simularProcesamiento("Consolidando diagnostico y soluciones");

    cout << "\n" << LINEA_DOBLE << "\n";
    cout << BOLD << GREEN << "  DIAGNOSTICO COMPLETADO EXITOSAMENTE" << RESET << "\n";
    cout << LINEA_DOBLE << "\n";
    cout << BOLD << "  Categoria    : " << RESET << resultado.categoria    << "\n";
    cout << BOLD << "  Sub-categoria: " << RESET << resultado.subCategoria << "\n";
    cout << BOLD << "  Descripcion  : " << RESET << resultado.descripcion  << "\n";
    cout << LINEA_SIMPLE << "\n";
    cout << BOLD << YELLOW << "  GUIA DE SOLUCION PASO A PASO:\n\n" << RESET;
    cout << resultado.solucionPasos << "\n";
    cout << LINEA_DOBLE << "\n";
    cout << "  Codigo interno de soporte: " << BOLD << "#" << resultado.codigoInterno << RESET << "\n";
    cout << LINEA_DOBLE << "\n";

    cout << BOLD << CYAN << "\n  >> Presione ENTER para finalizar la sesion y salir del programa..." << RESET;
    cin.get();
}

ResultadoDiagnostico iniciarDiagnostico() {
    cout << "\n" << LINEA_DOBLE << "\n";
    cout << BOLD << "  SISTEMA EXPERTO DE DIAGNOSTICO DE PC" << RESET << "\n";
    cout << "  Motor de Inferencia Analitico\n";
    cout << LINEA_DOBLE << "\n";

    int opcion = solicitarOpcion(
        "Identifique el primer sintoma observable en el equipo:",
        {
            "La PC no da senales de encendido (sin luces, sin ventiladores)",
            "La PC enciende pero la pantalla esta negra y emite pitidos",
            "Advertencia critica de SOBRECALENTAMIENTO del CPU al iniciar",
            "La PC se reinicia continuamente (Boot Loop)",
            "Falla termica o desconexion de unidad SSD M.2 NVMe",
            "El sistema enciende pero sufre lentitud o cuellos de botella",
            "Inestabilidad general o pantallas azules por errores de memoria RAM"
        }
    );

    ResultadoDiagnostico resultado;

    switch (opcion) {
        case 0: resultado = ramaA_SinEnergia();              break;
        case 1: resultado = ramaB_SinPantallaConBeeps();     break;
        case 2: resultado = ramaC_SobrecalentamientoCPU();   break;
        case 3: resultado = ramaD_CicloReinicioContinuo();   break;
        case 4: resultado = ramaE_FallaNVMe();               break;
        case 5: resultado = ramaF_CuelloDeBottella();        break;
        case 6: resultado = ramaG_XmpExpoRAM();              break;
    }

    mostrarResultadoFinal(resultado);
    return resultado;
}

ResultadoDiagnostico ramaA_SinEnergia() {
    simularProcesamiento("Iniciando modulo de analisis de energia");
    if (!preguntarSiNo("El cable de energia esta firmemente conectado al equipo y al tomacorriente?")) {
        return {"Fuente de Poder", "Cable desconectado", "Falla de alimentacion basica.",
                "  1. Desconecte el cable de poder de la parte trasera.\n  2. Reconecte firmemente en ambos extremos.\n  3. Pruebe encender.", 101};
    }
    if (!preguntarSiNo("El interruptor de la fuente de poder (parte trasera) esta en ON / I?")) {
        return {"Fuente de Poder", "Interruptor apagado", "Interruptor principal en OFF.",
                "  1. Posicione el interruptor trasero en 'I'.\n  2. Intente encender el equipo.", 102};
    }
    bool gira = preguntarSiNo("Al encender, el ventilador de la fuente gira un instante?");
    bool intermitente = preguntarSiNo("El equipo a veces enciende y otras veces no (aleatorio)?");

    if (!gira && !intermitente) {
        return {"Fuente de Poder", "Falla total de PSU", "La fuente no entrega senal.",
                "  1. Desconecte el equipo.\n  2. Realice la prueba del clip (puentear PS_ON y GND).\n  3. Si no gira, reemplace la fuente de poder.", 103};
    }
    if (!gira && intermitente) {
        return {"Fuente de Poder", "Ventilador trabado", "Riesgo de sobrecalentamiento en PSU.",
                "  1. Apague de inmediato.\n  2. Limpie el polvo de la PSU.\n  3. Si el ventilador sigue fallando, reemplace la fuente.", 104};
    }
    if (gira && intermitente) {
        return {"Fuente de Poder", "Voltaje inestable", "Potencia insuficiente o condensador danado.",
                "  1. Desconecte dispositivos externos.\n  2. Si enciende, la fuente no soporta la carga total. Reemplace por una de mayor wattage.", 105};
    }

    return {"Placa Base", "Falla de POST", "Fuente operativa pero placa inactiva.",
            "  1. Realice un Clear CMOS (retire pila 30s).\n  2. Pruebe encender solo con CPU, 1 RAM y PSU.", 106};
}

ResultadoDiagnostico ramaB_SinPantallaConBeeps() {
    simularProcesamiento("Iniciando analisis de codigos de error POST");
    int patron = solicitarOpcion("Seleccione el patron de pitidos que escucha:",
        {"Pitidos cortos repetitivos", "1 pitido largo + 2 o 3 cortos", "Tono continuo", "Sin pitidos, pantalla negra"});

    if (patron == 0) return {"Memoria RAM", "No detectada", "Fallo de inicializacion de RAM.",
                             "  1. Retire la RAM, limpie contactos con goma de borrar.\n  2. Reinserte un solo modulo en el slot principal.", 201};
    if (patron == 1) return {"Tarjeta Grafica", "No inicializada", "Falla en GPU.",
                             "  1. Retire la GPU y limpie el slot PCIe.\n  2. Verifique conectores PCIe de energia.\n  3. Pruebe otra GPU.", 202};
    if (patron == 2) return {"Placa Base", "Corto circuito", "Falla critica general.",
                             "  1. Desconecte todo lo no esencial.\n  2. Inspeccione la placa buscando quemaduras o condensadores inflados.", 203};

    return {"Video", "Sin senal", "Problema de cable o monitor.",
            "  1. Verifique cables de video.\n  2. Pruebe conectar al puerto de la placa base en vez de la GPU.", 204};
}