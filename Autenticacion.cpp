#include <iostream>
#include <string>
#include "Autenticacion.h"
#include "CloudServiceGateway.h"

using namespace std;

// ====================================================================
// CONEXIÓN CON LAS FUNCIONES DE CLOUDSERVICEGATEWAY
// ====================================================================
InfoUsuarioApp consultarUsuarioApp(string dni) {
    return consultarUsuarioAppEnNube(dni);
}

// ====================================================================
// MODULE 2.1: REGISTRATION FLOW
// ====================================================================
bool ejecutarRegistro(string dni) {
    string nuevoID, nuevoCorreo, nuevoPassword;

    cout << "\n--- CREACION DE CUENTA ---" << endl;
    cout << "Elige un ID de usuario unico (Nickname): ";
    cin >> nuevoID;

    cout << "Ingresa tu correo electronico: ";
    cin >> nuevoCorreo;

    cout << "[Verificando disponibilidad de correo...]" << endl;
    if (verificarCorreoExistente(nuevoCorreo)) {
        cout << "[ERROR] El correo " << nuevoCorreo << " ya esta en uso. Intente con otro." << endl;
        return false;
    }

    cout << "Crea una contrasena segura: ";
    cin >> nuevoPassword;

    cout << "\n[Registrando usuario en la base de datos...]" << endl;
    bool exito = registrarUsuarioSupabase(dni, nuevoCorreo, nuevoPassword, nuevoID);

    if (exito) {
        cout << "[EXITO] Usuario " << nuevoID << " registrado correctamente." << endl;
        return true;
    } else {
        cout << "[ERROR] Hubo un problema al registrar. Intente de nuevo." << endl;
        return false;
    }
}

// ====================================================================
// MODULE 2.2: LOGIN FLOW
// ====================================================================
bool ejecutarInicioSesion(string dni) {
    string correoIngresado, passIngresada;

    cout << "\n--- INICIO DE SESION FORMAL ---" << endl;
    cout << "Correo electronico: ";
    cin >> correoIngresado;
    cout << "Contrasena: ";
    cin >> passIngresada;

    cout << "\n[Verificando credenciales en la nube...]" << endl;

    InfoUsuarioApp datosNube = consultarUsuarioAppEnNube(dni);

    if (datosNube.yaRegistrado &&
        datosNube.correo == correoIngresado &&
        datosNube.passwordHash == passIngresada) {

        cout << "[EXITO] Sesion iniciada correctamente. Bienvenido de nuevo!" << endl;
        return true;
    } else {
        cout << "[ERROR] Credenciales incorrectas o el correo no coincide con este DNI." << endl;
        return false;
    }
}

// ====================================================================
// MODULE 2: MAIN AUTHENTICATION MENU (SELECTOR)
// ====================================================================
bool iniciarAutenticacion(string& dniLogueado) {
    string dniIngresado;

    cout << "\n==================================================" << endl;
    cout << "  SISTEMA DE DIAGNOSTICO - CIENCIAS MATEMATICAS" << endl;
    cout << "==================================================" << endl;
    cout << "Ingrese su DNI para verificar su identidad (o '0' para salir): ";
    cin >> dniIngresado;

    if (dniIngresado == "0") {
        cout << "Regresando al menu principal..." << endl;
        return false;
    }

    cout << "\n[Conectando al Padron Oficial de la Facultad...]" << endl;

    InfoPadron datosPadron = consultarPadronNube(dniIngresado);

    if (!datosPadron.existe) {
        cout << "\n[ERROR] El DNI " << dniIngresado << " no figura en el padron oficial." << endl;
        cout << "Acceso denegado." << endl;
        return false;
    }

    cout << "\n==================================================" << endl;
    cout << "Identidad confirmada: " << datosPadron.nombresCompletos << endl;
    cout << "Programa: " << datosPadron.carrera << endl;
    cout << "==================================================" << endl;

    InfoUsuarioApp cuentaApp = consultarUsuarioApp(dniIngresado);

    int opcion;
    cout << "\nSeleccione una accion:" << endl;
    cout << "1. Registrar nueva cuenta" << endl;
    cout << "2. Iniciar sesion" << endl;
    cout << "Opcion: ";
    cin >> opcion;

    bool accesoConcedido = false;

    if (opcion == 1) {
        if (cuentaApp.yaRegistrado) {
            cout << "\n[AVISO] Tu DNI ya esta vinculado a una cuenta. Usa la opcion de inicio de sesion." << endl;
            return false;
        } else {
            accesoConcedido = ejecutarRegistro(dniIngresado);
        }
    } else if (opcion == 2) {
        if (!cuentaApp.yaRegistrado) {
            cout << "\n[AVISO] No tienes una cuenta activa en el sistema. Por favor, registrate primero." << endl;
            return false;
        } else {
            accesoConcedido = ejecutarInicioSesion(dniIngresado);
        }
    } else {
        cout << "\n[ERROR] Opcion no valida. Retornando al menu..." << endl;
        return false;
    }

    if (accesoConcedido) {
        dniLogueado = dniIngresado;
        return true;
    }

    return false;
}