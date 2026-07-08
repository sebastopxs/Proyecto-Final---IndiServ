/*
 * ============================================================
 * LectorEspecificaciones.cpp
 * Modulo de Auto-Diagnostico de Hardware
 * ============================================================
 */

#include "LectorEspecificaciones.h"
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <array>
#include <thread>
#include <chrono>

using namespace std;

// Definicion de colores ANSI para la consola
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"

namespace LectorImpl {

    string ejecutarComando(const string& cmd) {
        string resultado;

#if defined(_WIN32) && !defined(__GNUC__)
        string cmdFinal = cmd + " 2>NUL";
        FILE* pipe = _popen(cmdFinal.c_str(), "r");
#elif defined(_WIN32)
        string cmdFinal = cmd + " 2>NUL";
        FILE* pipe = _popen(cmdFinal.c_str(), "r");
#else
        string cmdFinal = cmd + " 2>/dev/null";
        FILE* pipe = popen(cmdFinal.c_str(), "r");
#endif

        if (!pipe) return "[ERROR: no se pudo ejecutar el comando]";

        array<char, 256> buffer{};
        while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe)) {
            resultado += buffer.data();
        }

#if defined(_WIN32) && !defined(__GNUC__)
        _pclose(pipe);
#elif defined(_WIN32)
        _pclose(pipe);
#else
        pclose(pipe);
#endif

        return resultado;
    }

    string limpiarSalida(const string& raw) {
        string limpio;
        for (char c : raw) {
            if (c != '\r') limpio += c;
        }
        size_t inicio = limpio.find_first_not_of(" \t\n");
        size_t fin    = limpio.find_last_not_of(" \t\n");

        if (inicio == string::npos) return "";
        return limpio.substr(inicio, fin - inicio + 1);
    }
}

PerfilHardware leerEspecificaciones() {
    PerfilHardware perfil;
    perfil.lecturaExitosa = false;

    cout << CYAN << "\n  [*] Escaneando componentes de hardware";
    for(int i=0; i<3; i++) {
        this_thread::sleep_for(chrono::milliseconds(400));
        cout << ".";
    }
    cout << RESET << "\n";

    // CPU Name
    string salida = LectorImpl::ejecutarComando("wmic cpu get Name /value");
    size_t pos = salida.find('=');
    perfil.nombreCPU = (pos != string::npos) ? LectorImpl::limpiarSalida(salida.substr(pos + 1)) : "No disponible";

    // CPU Speed
    salida = LectorImpl::ejecutarComando("wmic cpu get MaxClockSpeed /value");
    pos = salida.find('=');
    perfil.velocidadCPUMHz = (pos != string::npos) ? LectorImpl::limpiarSalida(salida.substr(pos + 1)) : "No disponible";

    // CPU Cores
    salida = LectorImpl::ejecutarComando("wmic cpu get NumberOfLogicalProcessors /value");
    pos = salida.find('=');
    perfil.nucleosCPU = (pos != string::npos) ? LectorImpl::limpiarSalida(salida.substr(pos + 1)) : "No disponible";

    // RAM Total
    salida = LectorImpl::ejecutarComando("wmic ComputerSystem get TotalPhysicalMemory /value");
    pos = salida.find('=');
    if (pos != string::npos) {
        string bytesStr = LectorImpl::limpiarSalida(salida.substr(pos + 1));
        try {
            long long bytes = stoll(bytesStr);
            long long mb    = bytes / (1024LL * 1024LL);
            perfil.ramTotalMB = to_string(mb) + " MB";
        } catch (...) {
            perfil.ramTotalMB = bytesStr + " bytes";
        }
    } else {
        perfil.ramTotalMB = "No disponible";
    }

    // RAM Libre
    salida = LectorImpl::ejecutarComando("wmic OS get FreePhysicalMemory /value");
    pos = salida.find('=');
    if (pos != string::npos) {
        string kbStr = LectorImpl::limpiarSalida(salida.substr(pos + 1));
        try {
            long long kb = stoll(kbStr);
            long long mb = kb / 1024LL;
            perfil.ramDisponibleMB = to_string(mb) + " MB";
        } catch (...) {
            perfil.ramDisponibleMB = kbStr + " KB";
        }
    } else {
        perfil.ramDisponibleMB = "No disponible";
    }

    // SO Version
    salida = LectorImpl::ejecutarComando("wmic OS get Caption /value");
    pos = salida.find('=');
    perfil.versionSO = (pos != string::npos) ? LectorImpl::limpiarSalida(salida.substr(pos + 1)) : "No disponible";

    // Arquitectura
    salida = LectorImpl::ejecutarComando("wmic OS get OSArchitecture /value");
    pos = salida.find('=');
    perfil.arquitectura = (pos != string::npos) ? LectorImpl::limpiarSalida(salida.substr(pos + 1)) : "No disponible";

    // Host
    salida = LectorImpl::ejecutarComando("wmic ComputerSystem get Name /value");
    pos = salida.find('=');
    perfil.nombreEquipo = (pos != string::npos) ? LectorImpl::limpiarSalida(salida.substr(pos + 1)) : "No disponible";

    if (perfil.nombreCPU != "No disponible" && !perfil.nombreCPU.empty()) {
        perfil.lecturaExitosa = true;
    }

    return perfil;
}

void mostrarPerfil(const PerfilHardware& perfil) {
    string sep = "  +--------------------------------------------------+";
    cout << "\n" << BOLD << CYAN << sep << "\n";
    cout << "  |        PERFIL DE HARDWARE DETECTADO              |\n";
    cout << sep << RESET << "\n";
    cout << BOLD << "  | Equipo     : " << RESET << perfil.nombreEquipo    << "\n";
    cout << BOLD << "  | CPU        : " << RESET << perfil.nombreCPU       << "\n";
    cout << BOLD << "  | Velocidad  : " << RESET << perfil.velocidadCPUMHz << " MHz\n";
    cout << BOLD << "  | Nucleos    : " << RESET << perfil.nucleosCPU      << "\n";
    cout << BOLD << "  | RAM Total  : " << RESET << perfil.ramTotalMB      << "\n";
    cout << BOLD << "  | RAM Libre  : " << RESET << perfil.ramDisponibleMB << "\n";
    cout << BOLD << "  | SO         : " << RESET << perfil.versionSO       << "\n";
    cout << BOLD << "  | Arq.       : " << RESET << perfil.arquitectura    << "\n";
    cout << CYAN << sep << RESET << "\n";

    if (!perfil.lecturaExitosa) {
        cout << RED << "  [!] ADVERTENCIA: Lectura parcial. Verifica permisos de administrador.\n" << RESET;
    }
}

void evaluarPerfil(const PerfilHardware& perfil) {
    bool hayAlertas = false;

    try {
        string ramStr = perfil.ramTotalMB;
        size_t pos = ramStr.find(' ');
        if (pos != string::npos) ramStr = ramStr.substr(0, pos);
        long long ramMB = stoll(ramStr);

        if (ramMB <= 2048) {
            cout << YELLOW << "\n  [!] ALERTA DE RAM: Solo " << perfil.ramTotalMB
                 << " instalados. Considere ampliar a minimo 4 GB.\n" << RESET;
            hayAlertas = true;
        }
    } catch (...) {}

    if (perfil.versionSO.find("Windows 7") != string::npos ||
        perfil.versionSO.find("Windows XP") != string::npos) {
        cout << YELLOW << "\n  [!] ALERTA DE SO: " << perfil.versionSO
             << " obsoleto. Se recomienda actualizar a Windows 10/11.\n" << RESET;
        hayAlertas = true;
    }

    try {
        long long nucleos = stoll(perfil.nucleosCPU);
        if (nucleos <= 2) {
            cout << YELLOW << "\n  [!] ALERTA DE CPU: Solo " << perfil.nucleosCPU
                 << " nucleos detectados. Posible cuello de botella.\n" << RESET;
            hayAlertas = true;
        }
    } catch (...) {}

    if (!hayAlertas) {
        cout << GREEN << "\n  [OK] Perfil de hardware dentro de parametros optimos.\n" << RESET;
    }
}

bool guardarPerfilEnBD(const PerfilHardware& perfil, int idSesion) {
    (void)perfil;
    (void)idSesion;
    // cout << CYAN << "\n  [BD-STUB] Perfil listo para persistir en modulo SQL." << RESET << "\n";
    return false;
}