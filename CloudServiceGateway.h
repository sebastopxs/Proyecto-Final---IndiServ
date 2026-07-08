#pragma once
#ifndef CLOUDSERVICEGATEWAY_H
#define CLOUDSERVICEGATEWAY_H

#include <string>
#include "Autenticacion.h"

// ¡Librerías activadas!
// Ahora el archivo sabe qué es PerfilHardware y ResultadoDiagnostico
#include "LectorEspecificaciones.h"
#include "MotorDiagnostico.h"

// ==========================================================
// FUNCIONES DE TUS COMPAÑEROS (SEGURIDAD Y REGISTRO)
// ==========================================================
InfoPadron consultarPadronNube(std::string dni);
InfoUsuarioApp consultarUsuarioAppEnNube(std::string dni);
bool registrarUsuarioSupabase(std::string dni, std::string correo, std::string password, std::string nickname);
bool verificarCorreoExistente(std::string correo);

// ==========================================================
// TUS FUNCIONES (DIAGNÓSTICO Y SUGERENCIAS)
// ==========================================================
// ¡Espacio de nombres activado!
// Ahora main.cpp y BuzonSugerencias.cpp pueden usar estas funciones
namespace CloudGateway {
    bool sincronizarReporteSupabase(const PerfilHardware& perfil, const ResultadoDiagnostico& resultado, const std::string& ticketID);
    bool sincronizarSugerenciaSupabase(const std::string& sugerencia, const std::string& ticketID);
}

#endif // CLOUDSERVICEGATEWAY_H