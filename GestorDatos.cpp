#include "GestorDatos.h"
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define GREEN   "\033[32m"
#define RED     "\033[31m"
#define YELLOW  "\033[33m"

bool exportarReporteFisico(const PerfilHardware& perfil, const ResultadoDiagnostico& resultado, const string& ticketID) {
    string nombreArchivo = "Reporte_Diagnostico_" + ticketID + ".txt";
    ofstream archivo(nombreArchivo);

    if (!archivo.is_open()) {
        cout << RED << "  [ERROR] No se pudo crear el archivo de reporte fisico.\n" << RESET;
        return false;
    }

    archivo << "================================================================\n";
    archivo << "          REPORTE TECNICO DE DIAGNOSTICO DE PC                  \n";
    archivo << "================================================================\n";
    archivo << " TICKET ID: " << ticketID << "\n";
    archivo << "----------------------------------------------------------------\n";
    archivo << " [1] ESPECIFICACIONES DEL EQUIPO\n";
    archivo << "----------------------------------------------------------------\n";
    archivo << " CPU detectado : " << perfil.nombreCPU << "\n";
    archivo << " RAM instalada : " << perfil.ramTotalMB << "\n";
    archivo << " Sistema Oper. : " << perfil.versionSO << "\n";
    archivo << "----------------------------------------------------------------\n";
    archivo << " [2] RESULTADO DEL DIAGNOSTICO\n";
    archivo << "----------------------------------------------------------------\n";
    archivo << " Categoria     : " << resultado.categoria << "\n";
    archivo << " Sub-categoria : " << resultado.subCategoria << "\n";
    archivo << " Falla tecnica : " << resultado.descripcion << "\n";
    archivo << "----------------------------------------------------------------\n";
    archivo << " [3] GUIA DE SOLUCION SEGURA\n";
    archivo << "----------------------------------------------------------------\n";
    
    // Quitar colores ANSI si existieran en el string de solucion para el TXT
    string solucionLimpia = resultado.solucionPasos;
    archivo << solucionLimpia << "\n";
    
    archivo << "================================================================\n";
    archivo << " Entregue este archivo a su tecnico de confianza si es necesario.\n";
    archivo << "================================================================\n";

    archivo.close();
    cout << GREEN << "  [EXITO] Reporte exportado correctamente como: " << BOLD << nombreArchivo << RESET << "\n";
    return true;
}

void leerHistorialLocal() {
    ifstream archivo("historial_local.txt");
    if (!archivo.is_open()) {
        cout << YELLOW << "  [INFO] No hay historial previo registrado en este equipo.\n" << RESET;
        return;
    }

    cout << "\n" << BOLD << "  --- HISTORIAL DE DIAGNOSTICOS ---" << RESET << "\n";
    string linea;
    int contador = 0;

    // INICIALIZACION ESTRICTA para prevenir fallos logicos en memoria residual
    while (getline(archivo, linea)) {
        if (linea.empty()) continue; 
        
        string datoLimpio = linea; 
        contador++;
        cout << "  Registro " << contador << ": " << datoLimpio << "\n";
        
        // Reset de variables temporales al final de cada iteracion (Buenas practicas)
        datoLimpio.clear(); 
    }
    archivo.close();
    
    // Reset del contador por seguridad
    contador = 0; 
}