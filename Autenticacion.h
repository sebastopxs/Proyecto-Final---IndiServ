#pragma once
#ifndef AUTENTICACION_H
#define AUTENTICACION_H

#include <string>

// ====================================================================
// DATA STRUCTURES
// ====================================================================
struct InfoPadron {
    bool existe;
    std::string nombresCompletos;
    std::string carrera;
};

struct InfoUsuarioApp {
    bool yaRegistrado;
    std::string correo;
    std::string passwordHash;
};

// ====================================================================
// FUNCTION DECLARATIONS
// ====================================================================

// El puente principal
bool iniciarAutenticacion(std::string& dniLogueado);

// Sub-flujos ahora devuelven bool para avisar si tuvieron exito
bool ejecutarRegistro(std::string dni);
bool ejecutarInicioSesion(std::string dni);

// Funciones de consulta
InfoUsuarioApp consultarUsuarioApp(std::string dni);

#endif // AUTENTICACION_H