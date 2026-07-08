#pragma once
#ifndef MOTOR_DIAGNOSTICO_H
#define MOTOR_DIAGNOSTICO_H

#include <string>
#include <initializer_list>

// Eliminamos el using namespace std; de la cabecera

struct ResultadoDiagnostico {
    std::string categoria;
    std::string subCategoria;
    std::string descripcion;
    std::string solucionPasos;
    int    codigoInterno;
};

ResultadoDiagnostico iniciarDiagnostico();

ResultadoDiagnostico ramaA_SinEnergia();
ResultadoDiagnostico ramaB_SinPantallaConBeeps();
ResultadoDiagnostico ramaC_SobrecalentamientoCPU();
ResultadoDiagnostico ramaD_CicloReinicioContinuo();
ResultadoDiagnostico ramaE_FallaNVMe();
ResultadoDiagnostico ramaF_CuelloDeBottella();
ResultadoDiagnostico ramaG_XmpExpoRAM();

bool preguntarSiNo(const std::string& pregunta);
int solicitarOpcion(const std::string& titulo, const std::initializer_list<std::string>& opciones);
void mostrarResultadoFinal(const ResultadoDiagnostico& resultado);
bool registrarDiagnosticoEnBD(const ResultadoDiagnostico& resultado, int idSesion = -1);
void simularProcesamiento(const std::string& mensaje);

#endif // MOTOR_DIAGNOSTICO_H