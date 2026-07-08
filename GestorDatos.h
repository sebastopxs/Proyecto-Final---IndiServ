#pragma once
#ifndef GESTOR_DATOS_H
#define GESTOR_DATOS_H

#include "LectorEspecificaciones.h"
#include "MotorDiagnostico.h"
#include <string>

bool exportarReporteFisico(const PerfilHardware& perfil, const ResultadoDiagnostico& resultado, const std::string& ticketID);
void leerHistorialLocal();

#endif // GESTOR_DATOS_H