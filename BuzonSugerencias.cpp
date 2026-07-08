#include "BuzonSugerencias.h"
#include "CloudServiceGateway.h"
#include <iostream>
#include <fstream>
#include <random>
#include <set>
#include <limits>

using namespace std;

#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define MAGENTA "\033[35m"
#define YELLOW  "\033[33m"
#define GREEN   "\033[32m"

// Funcion interna para cargar IDs existentes y evitar duplicados
set<string> cargarTicketsExistentes() {
    set<string> tickets;
    ifstream archivo("sugerencias_locales.txt");
    string linea;
    
    // Inicializacion estricta para bucle
    while (getline(archivo, linea)) {
        if (linea.find("TICKET-") != string::npos) {
            size_t pos = linea.find("TICKET-");
            // Extraer el ID asumiendo formato fijo "TICKET-XXXX"
            string idExtraido = linea.substr(pos, 11);
            tickets.insert(idExtraido);
            idExtraido.clear();
        }
        linea.clear();
    }
    archivo.close();
    return tickets;
}

string generarTicketID() {
    // 1. Cargar tickets previos para validacion
    set<string> ticketsOcupados = cargarTicketsExistentes();
    
    // 2. Configurar el motor de numeros aleatorios de C++ moderno
    random_device rd;  
    mt19937 gen(rd()); 
    uniform_int_distribution<> distrib(1000, 9999);

    string nuevoTicket;
    bool esUnico = false;
    int intentosSeguridad = 0; // Prevenir ciclo infinito en peores escenarios

    // Lógica estricta de validación
    while (!esUnico && intentosSeguridad < 1000) {
        int numeroAleatorio = distrib(gen);
        nuevoTicket = "TICKET-" + to_string(numeroAleatorio);
        
        // Si el ticket NO se encuentra en el set, es unico
        if (ticketsOcupados.find(nuevoTicket) == ticketsOcupados.end()) {
            esUnico = true;
        }
        intentosSeguridad++;
    }

    // Fallback de seguridad extrema
    if (!esUnico) {
        nuevoTicket = "TICKET-FALLBACK";
    }

    return nuevoTicket;
}

void dejarSugerencia() {
    cout << "\n" << BOLD << MAGENTA << "  ================================================================" << RESET << "\n";
    cout << BOLD << "                BUZON DE SOPORTE Y SUGERENCIAS                    " << RESET << "\n";
    cout << BOLD << MAGENTA << "  ================================================================" << RESET << "\n";
    cout << "  Describa cualquier error encontrado o mejora que desee proponer.\n";
    cout << "  Presione ENTER para enviar.\n";
    cout << "  > ";

    string textoSugerencia;
    // Usamos getline para permitir espacios en la sugerencia
    getline(cin, textoSugerencia);

    if (textoSugerencia.empty()) {
        cout << YELLOW << "  [!] Sugerencia vacia. Operacion cancelada.\n" << RESET;
        return;
    }

    string miTicket = generarTicketID();

    // Guardado Oculto Local
    ofstream archivo("sugerencias_locales.txt", ios::app);
    if (archivo.is_open()) {
        archivo << "[" << miTicket << "] " << textoSugerencia << "\n";
        archivo.close();
    }

    // Sincronizacion Cloud (Supabase)
    CloudGateway::sincronizarSugerenciaSupabase(textoSugerencia, miTicket);

    cout << "\n" << GREEN << "  [EXITO] Gracias por su feedback." << RESET << "\n";
    cout << "  Su codigo de seguimiento es: " << BOLD << miTicket << RESET << "\n";
    cout << "  Conserve este codigo si necesita contactar a desarrollo.\n";
}