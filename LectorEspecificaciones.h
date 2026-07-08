#pragma once
#ifndef LECTOR_ESPECIFICACIONES_H
#define LECTOR_ESPECIFICACIONES_H

#include <string>

// Eliminamos el using namespace std; de la cabecera para evitar conflictos

struct PerfilHardware {
    std::string nombreCPU;
    std::string velocidadCPUMHz;
    std::string nucleosCPU;
    std::string ramTotalMB;
    std::string ramDisponibleMB;
    std::string versionSO;
    std::string arquitectura;
    std::string nombreEquipo;
    bool   lecturaExitosa;
};

PerfilHardware leerEspecificaciones();
void mostrarPerfil(const PerfilHardware& perfil);
void evaluarPerfil(const PerfilHardware& perfil);
bool guardarPerfilEnBD(const PerfilHardware& perfil, int idSesion = -1);

namespace LectorImpl {
    std::string ejecutarComando(const std::string& cmd);
    std::string limpiarSalida(const std::string& raw);
}

#endif // LECTOR_ESPECIFICACIONES_H