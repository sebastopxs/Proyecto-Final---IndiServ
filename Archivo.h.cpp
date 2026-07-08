#ifndef AUTENTICACION_H
#define AUTENTICACION_H

#include <string>

// Estructura para los datos del padrón oficial
struct InfoPadron {
    bool existe;
    std::string nombresCompletos;
    std::string carrera;
};

// Estructura para los datos de la cuenta de la app
struct InfoUsuarioApp {
    bool yaRegistrado;
    std::string correo;
    std::string passwordHash;
};

// Declaración de la función principal de seguridad
void iniciarAutenticacion();

#endif