/*
 * ============================================================
 * main.cpp
 * Sistema Experto de Diagnostico de PC (Interfaz Grafica Interactiva)
 * ============================================================
 */

#include <iostream>
#include <string>
#include <limits>
#include <vector>
#include <fstream> // Necesario para el guardado local

// Importar el decodificador de imagenes
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Cabeceras Graficas de Soporte
#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

// Inclusión de los módulos logicos del equipo
#include "Autenticacion.h"
#include "LectorEspecificaciones.h"
#include "MotorDiagnostico.h"
#include "GestorDatos.h"
#include "BuzonSugerencias.h"
#include "CloudServiceGateway.h"

using namespace std;

// ====================================================================
// ENLACES EXTERNOS A TU BASE DE DATOS PARA EL REGISTRO
// (Tomados directamente de tus archivos CloudServiceGateway)
// ====================================================================
extern bool verificarCorreoExistente(string correo);
extern bool registrarUsuarioSupabase(string dni, string correo, string password, string nickname);

// ====================================================================
// MOTOR DE TEXTURAS OPENGL
// ====================================================================
bool CargarImagenComoTextura(const char* nombreArchivo, GLuint* texturaSalida, int* anchoSalida, int* altoSalida) {
    int anchoImagen = 0, altoImagen = 0, canalesImagen = 0;
    unsigned char* datosImagen = stbi_load(nombreArchivo, &anchoImagen, &altoImagen, &canalesImagen, 4);
    if (datosImagen == NULL) return false;

    GLuint texturaOpenGL;
    glGenTextures(1, &texturaOpenGL);
    glBindTexture(GL_TEXTURE_2D, texturaOpenGL);

    // Configuracion de textura con codigo hexadecimal directo (0x812F)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, anchoImagen, altoImagen, 0, GL_RGBA, GL_UNSIGNED_BYTE, datosImagen);
    stbi_image_free(datosImagen);

    *texturaSalida = texturaOpenGL;
    *anchoSalida = anchoImagen;
    *altoSalida = altoImagen;

    return true;
}

// ====================================================================
// CONFIGURACION DE ESTILO VISUAL (TEMA MODERNO / GAMER)
// ====================================================================
void AplicarEstiloVisual() {
    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowRounding = 8.0f;
    style.FrameRounding = 6.0f;
    style.GrabRounding = 6.0f;
    style.PopupRounding = 8.0f;
    style.ChildRounding = 6.0f;
    style.WindowBorderSize = 0.0f;

    ImVec4* colors = style.Colors;

    colors[ImGuiCol_WindowBg]       = ImVec4(0.12f, 0.12f, 0.13f, 0.95f);
    colors[ImGuiCol_ChildBg]        = ImVec4(0.15f, 0.15f, 0.16f, 1.00f);
    colors[ImGuiCol_PopupBg]        = ImVec4(0.14f, 0.14f, 0.15f, 1.00f);
    colors[ImGuiCol_FrameBg]        = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.25f, 0.27f, 1.00f);
    colors[ImGuiCol_FrameBgActive]  = ImVec4(0.30f, 0.30f, 0.32f, 1.00f);
    colors[ImGuiCol_Button]         = ImVec4(0.16f, 0.58f, 0.43f, 1.00f);
    colors[ImGuiCol_ButtonHovered]  = ImVec4(0.20f, 0.65f, 0.48f, 1.00f);
    colors[ImGuiCol_ButtonActive]   = ImVec4(0.12f, 0.50f, 0.36f, 1.00f);
    colors[ImGuiCol_TitleBg]        = ImVec4(0.10f, 0.10f, 0.11f, 1.00f);
    colors[ImGuiCol_TitleBgActive]  = ImVec4(0.16f, 0.58f, 0.43f, 1.00f);
    colors[ImGuiCol_Header]         = ImVec4(0.16f, 0.58f, 0.43f, 0.80f);
    colors[ImGuiCol_HeaderHovered]  = ImVec4(0.20f, 0.65f, 0.48f, 0.80f);
    colors[ImGuiCol_HeaderActive]   = ImVec4(0.16f, 0.58f, 0.43f, 1.00f);
    colors[ImGuiCol_Separator]      = ImVec4(0.25f, 0.25f, 0.27f, 1.00f);
    colors[ImGuiCol_Text]           = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
    colors[ImGuiCol_TextDisabled]   = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
}

// 1. Expandimos la máquina de estados para incluir REGISTRO
enum EstadoUI { PANTALLA_SPLASH, PANTALLA_LOGIN, PANTALLA_REGISTRO, PANTALLA_DASHBOARD, PANTALLA_DIAGNOSTICO_ACTIVO };

int main() {
    if (!glfwInit()) { cerr << "[ERROR] No se pudo inicializar GLFW" << endl; return -1; }

    GLFWwindow* window = glfwCreateWindow(900, 650, "Sistema Experto Diagnostico PC - Computacion Cientifica", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }

    // ====================================================================
    // LÓGICA PARA CENTRAR LA VENTANA EN PANTALLA
    // ====================================================================
    GLFWmonitor* monitorPrincipal = glfwGetPrimaryMonitor();
    if (monitorPrincipal != NULL) {
        const GLFWvidmode* modoVideo = glfwGetVideoMode(monitorPrincipal);
        if (modoVideo != NULL) {
            int posicionX = (modoVideo->width - 900) / 2;
            int posicionY = (modoVideo->height - 650) / 2;
            glfwSetWindowPos(window, posicionX, posicionY);
        }
    }
    // ====================================================================

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();
    AplicarEstiloVisual();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // =====================================================
    // VARIABLES GLOBALES DE ESTADO Y TELEMETRÍA
    // =====================================================
    EstadoUI estadoActual = PANTALLA_SPLASH;
    double tiempoInicioCarga = glfwGetTime();

    string dniUsuarioLogueado = "";
    string stringMensajeError = "";

    // Buffers Login
    char bufferDni[16] = "";
    char bufferCorreo[64] = "";
    char bufferPassword[64] = "";

    // Buffers Registro
    char bufferRegDni[16] = "";
    char bufferRegNickname[32] = "";
    char bufferRegCorreo[64] = "";
    char bufferRegPassword[64] = "";

    PerfilHardware hardwareDetectado;
    ResultadoDiagnostico dictamenFinal;
    bool hardwareEscaneado = false;

    int faseSintoma = 0;
    int ramaActiva = -1;
    int pasoRama = 0;
    bool temporalRamaAGira = false;

    char bufferSugerencia[512] = "";
    bool mostrarModalSugerencia = false;
    string ticketSugerenciaGenerado = "";

    GLuint texturaFondo = 0;
    int anchoFondo = 0;
    int altoFondo = 0;
    bool tieneFondo = CargarImagenComoTextura("fondo.jpg", &texturaFondo, &anchoFondo, &altoFondo);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ----------------------------------------------------------------
        // DIBUJAR EL FONDO (Background)
        // ----------------------------------------------------------------
        if (tieneFondo) {
            ImVec2 tamanoPantalla = io.DisplaySize;
            ImGui::GetBackgroundDrawList()->AddImage(
                (void*)(intptr_t)texturaFondo, ImVec2(0, 0), tamanoPantalla,
                ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 255)
            );
        }

        // ----------------------------------------------------------------
        // INTERFAZ 0: PANTALLA DE CARGA (SPLASH SCREEN)
        // ----------------------------------------------------------------
        if (estadoActual == PANTALLA_SPLASH) {
            double tiempoTranscurrido = glfwGetTime() - tiempoInicioCarga;
            float progreso = (float)(tiempoTranscurrido / 3.0);

            if (progreso >= 1.0f) {
                estadoActual = PANTALLA_LOGIN;
            }

            ImVec2 tamanoSplash(400, 250);
            ImGui::SetNextWindowPos(ImVec2((io.DisplaySize.x - tamanoSplash.x) * 0.5f, (io.DisplaySize.y - tamanoSplash.y) * 0.5f), ImGuiCond_Always);
            ImGui::SetNextWindowSize(tamanoSplash, ImGuiCond_Always);

            ImGui::Begin("##Splash", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground);

            float anchoVentana = ImGui::GetWindowSize().x;

            ImGui::SetWindowFontScale(3.5f);
            const char* textoIDS = "I D S";
            float anchoIDS = ImGui::CalcTextSize(textoIDS).x;
            ImGui::SetCursorPosX((anchoVentana - anchoIDS) * 0.5f);
            ImGui::TextColored(ImVec4(0.16f, 0.58f, 0.43f, 1.00f), "%s", textoIDS);

            ImGui::SetWindowFontScale(1.3f);
            const char* textoSub = "IndiServ Analytica";
            float anchoSub = ImGui::CalcTextSize(textoSub).x;
            ImGui::SetCursorPosX((anchoVentana - anchoSub) * 0.5f);
            ImGui::Text("%s", textoSub);

            ImGui::SetWindowFontScale(1.0f);

            ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

            const char* textoEstado = "Iniciando modulos del sistema...";
            float anchoEstado = ImGui::CalcTextSize(textoEstado).x;
            ImGui::SetCursorPosX((anchoVentana - anchoEstado) * 0.5f);
            ImGui::TextDisabled("%s", textoEstado);

            ImGui::Spacing();
            ImGui::ProgressBar(progreso, ImVec2(-1, 6), "");

            ImGui::End();
        }

        // ----------------------------------------------------------------
        // INTERFAZ 1: PORTAL DE SEGURIDAD (LOGIN)
        // ----------------------------------------------------------------
        else if (estadoActual == PANTALLA_LOGIN) {
            ImVec2 tamanoLogin(350, 360); // Ajuste de tamaño para el nuevo botón
            ImGui::SetNextWindowPos(ImVec2((io.DisplaySize.x - tamanoLogin.x) * 0.5f, (io.DisplaySize.y - tamanoLogin.y) * 0.5f), ImGuiCond_Always);
            ImGui::SetNextWindowSize(tamanoLogin, ImGuiCond_Always);

            ImGui::Begin("Portal de Seguridad - Autenticacion", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
            ImGui::Text("Ingrese sus datos de validacion:");
            ImGui::Separator(); ImGui::Spacing();

            ImGui::InputText("DNI", bufferDni, IM_ARRAYSIZE(bufferDni));
            ImGui::InputText("Correo", bufferCorreo, IM_ARRAYSIZE(bufferCorreo));
            ImGui::InputText("Contrasena", bufferPassword, IM_ARRAYSIZE(bufferPassword), ImGuiInputTextFlags_Password);

            ImGui::Spacing();
            if (!stringMensajeError.empty()) { ImGui::TextColored(ImVec4(1.0f, 0.35f, 0.35f, 1.0f), stringMensajeError.c_str()); }
            ImGui::Spacing();

            if (ImGui::Button("Ingresar al Sistema", ImVec2(-1, 38))) {
                string dniStr(bufferDni);
                InfoUsuarioApp cuentaNube = consultarUsuarioApp(dniStr);

                if (cuentaNube.yaRegistrado && cuentaNube.passwordHash == string(bufferPassword) && cuentaNube.correo == string(bufferCorreo)) {
                    dniUsuarioLogueado = dniStr;
                    estadoActual = PANTALLA_DASHBOARD;
                    stringMensajeError = "";
                    // Limpiar contrasena por seguridad
                    memset(bufferPassword, 0, sizeof(bufferPassword));
                } else {
                    stringMensajeError = "Credenciales incorrectas o cuenta inexistente.";
                }
            }

            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
            ImGui::TextDisabled("¿Aun no tiene una cuenta autorizada?");

            // BOTON DE REDIRECCION AL REGISTRO
            if (ImGui::Button("Crear Nueva Cuenta", ImVec2(-1, 30))) {
                estadoActual = PANTALLA_REGISTRO;
                stringMensajeError = "";
            }

            ImGui::End();
        }

        // ----------------------------------------------------------------
        // INTERFAZ 1.5: PORTAL DE REGISTRO (NUEVO)
        // ----------------------------------------------------------------
        else if (estadoActual == PANTALLA_REGISTRO) {
            ImVec2 tamanoReg(360, 420);
            ImGui::SetNextWindowPos(ImVec2((io.DisplaySize.x - tamanoReg.x) * 0.5f, (io.DisplaySize.y - tamanoReg.y) * 0.5f), ImGuiCond_Always);
            ImGui::SetNextWindowSize(tamanoReg, ImGuiCond_Always);

            ImGui::Begin("Registro en el Sistema Experto", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
            ImGui::Text("Complete sus datos corporativos:");
            ImGui::Separator(); ImGui::Spacing();

            ImGui::InputText("DNI", bufferRegDni, IM_ARRAYSIZE(bufferRegDni));
            ImGui::InputText("Nickname", bufferRegNickname, IM_ARRAYSIZE(bufferRegNickname));
            ImGui::InputText("Correo", bufferRegCorreo, IM_ARRAYSIZE(bufferRegCorreo));
            ImGui::InputText("Contrasena", bufferRegPassword, IM_ARRAYSIZE(bufferRegPassword), ImGuiInputTextFlags_Password);

            ImGui::Spacing();
            if (!stringMensajeError.empty()) { ImGui::TextColored(ImVec4(1.0f, 0.35f, 0.35f, 1.0f), stringMensajeError.c_str()); }
            ImGui::Spacing();

            // LOGICA DEL BOTON DE REGISTRO
            if (ImGui::Button("Registrar en Base de Datos", ImVec2(-1, 38))) {
                string dniStr(bufferRegDni);
                string nickStr(bufferRegNickname);
                string correoStr(bufferRegCorreo);
                string passStr(bufferRegPassword);

                if (dniStr.empty() || nickStr.empty() || correoStr.empty() || passStr.empty()) {
                    stringMensajeError = "Por favor, llene todos los campos.";
                } else {
                    InfoUsuarioApp cuentaNube = consultarUsuarioApp(dniStr);
                    if (cuentaNube.yaRegistrado) {
                        stringMensajeError = "Error: Este DNI ya posee una cuenta activa.";
                    } else if (verificarCorreoExistente(correoStr)) {
                        stringMensajeError = "Error: El correo ingresado ya esta en uso.";
                    } else {
                        bool exito = registrarUsuarioSupabase(dniStr, correoStr, passStr, nickStr);
                        if (exito) {
                            stringMensajeError = "Exito. Puede iniciar sesion ahora.";
                            estadoActual = PANTALLA_LOGIN;
                            // Limpiar buffers
                            memset(bufferRegDni, 0, sizeof(bufferRegDni));
                            memset(bufferRegNickname, 0, sizeof(bufferRegNickname));
                            memset(bufferRegCorreo, 0, sizeof(bufferRegCorreo));
                            memset(bufferRegPassword, 0, sizeof(bufferRegPassword));
                        } else {
                            stringMensajeError = "Fallo de conexion con la nube. Intente luego.";
                        }
                    }
                }
            }

            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
            if (ImGui::Button("Cancelar y Volver", ImVec2(-1, 30))) {
                estadoActual = PANTALLA_LOGIN;
                stringMensajeError = "";
            }

            ImGui::End();
        }

        // ----------------------------------------------------------------
        // INTERFAZ 2: PANEL DE OPERACIONES (DASHBOARD TÉCNICO)
        // ----------------------------------------------------------------
        else if (estadoActual == PANTALLA_DASHBOARD) {
            ImVec2 tamanoDashboard(800, 520);
            ImGui::SetNextWindowPos(ImVec2((io.DisplaySize.x - tamanoDashboard.x) * 0.5f, (io.DisplaySize.y - tamanoDashboard.y) * 0.5f), ImGuiCond_Always);
            ImGui::SetNextWindowSize(tamanoDashboard, ImGuiCond_Always);

            ImGui::Begin("Consola Central de Operaciones - Computacion Cientifica", NULL, ImGuiWindowFlags_NoCollapse);
            ImGui::TextColored(ImVec4(0.16f, 0.58f, 0.43f, 1.00f), "Sesion Certificada del Operador: %s", dniUsuarioLogueado.c_str());
            ImGui::Separator(); ImGui::Spacing();

            if (!hardwareEscaneado) {
                hardwareDetectado = leerEspecificaciones();
                hardwareEscaneado = true;
            }

            if (ImGui::CollapsingHeader("Analisis Automatizado del Entorno de Hardware", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::BulletText("Modelo de Unidad Central (CPU): %s", hardwareDetectado.nombreCPU.c_str());
                ImGui::BulletText("Memoria de Intercambio (RAM): %s MB", hardwareDetectado.ramTotalMB.c_str());
                ImGui::TextColored(ImVec4(0.2f, 0.9f, 0.2f, 1.0f), "  [ESTADO DEL ENTORNO DE ESPECIFICACIONES]: VERIFICADO");
            }

            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

            ImGui::Text("Herramientas Analiticas del Sistema Experto:");
            if (ImGui::Button("Lanzar Motor de Diagnostico (Arbol de Decisiones)", ImVec2(400, 45))) {
                faseSintoma = 0; ramaActiva = -1; pasoRama = 0; temporalRamaAGira = false;
                estadoActual = PANTALLA_DIAGNOSTICO_ACTIVO;
            }

            ImGui::Spacing();
            if (ImGui::Button("Consultar Bitacora e Historial Local (.txt)", ImVec2(400, 35))) {
                leerHistorialLocal();
            }

            ImGui::Spacing();
            if (ImGui::Button("Buzon de Soporte y Sugerencias", ImVec2(400, 35))) {
                mostrarModalSugerencia = true;
                ticketSugerenciaGenerado = "";
            }

            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

            if (ImGui::Button("Cerrar Sesion y Bloquear Consola", ImVec2(220, 30))) {
                estadoActual = PANTALLA_LOGIN;
                dniUsuarioLogueado = ""; hardwareEscaneado = false;
            }

            // MODAL DE SUGERENCIAS
            if (mostrarModalSugerencia) {
                ImGui::OpenPopup("Enviar Sugerencia al Equipo");
            }

            ImGui::SetNextWindowSize(ImVec2(550, 300), ImGuiCond_Appearing);
            if (ImGui::BeginPopupModal("Enviar Sugerencia al Equipo", &mostrarModalSugerencia, ImGuiWindowFlags_NoResize)) {
                ImGui::TextWrapped("Describa cualquier error encontrado o mejora que desee proponer:");
                ImGui::Spacing();

                ImGui::InputTextMultiline("##sug_texto", bufferSugerencia, IM_ARRAYSIZE(bufferSugerencia), ImVec2(-1, 120));
                ImGui::Spacing();

                if (!ticketSugerenciaGenerado.empty()) {
                    ImGui::TextColored(ImVec4(0.2f, 0.9f, 0.2f, 1.0f), "Sugerencia enviada con exito. Ticket: %s", ticketSugerenciaGenerado.c_str());
                }

                ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

                if (ImGui::Button("Enviar a Base de Datos", ImVec2(200, 40))) {
                    string textoSug(bufferSugerencia);
                    if(!textoSug.empty()) {
                        string miTicket = generarTicketID();

                        ofstream archivo("sugerencias_locales.txt", ios::app);
                        if (archivo.is_open()) {
                            archivo << "[" << miTicket << "] " << textoSug << "\n";
                            archivo.close();
                        }

                        CloudGateway::sincronizarSugerenciaSupabase(textoSug, miTicket);

                        ticketSugerenciaGenerado = miTicket;
                        memset(bufferSugerencia, 0, sizeof(bufferSugerencia));
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Cerrar", ImVec2(120, 40))) {
                    mostrarModalSugerencia = false;
                }
                ImGui::EndPopup();
            }
            ImGui::End();
        }

        // ----------------------------------------------------------------
        // INTERFAZ 3: MOTOR DE INFERENCIA ANALÍTICO MATEADO A IMGUI
        // ----------------------------------------------------------------
        else if (estadoActual == PANTALLA_DIAGNOSTICO_ACTIVO) {
            ImVec2 tamanoDiagnostico(720, 480);
            ImGui::SetNextWindowPos(ImVec2((io.DisplaySize.x - tamanoDiagnostico.x) * 0.5f, (io.DisplaySize.y - tamanoDiagnostico.y) * 0.5f), ImGuiCond_Always);
            ImGui::SetNextWindowSize(tamanoDiagnostico, ImGuiCond_Always);

            ImGui::Begin("Asistente Inferencial Avanzado - Diagnostico", NULL, ImGuiWindowFlags_NoCollapse);
            ImGui::TextColored(ImVec4(0.16f, 0.58f, 0.43f, 1.00f), "Motor de Inferencia Analitico");
            ImGui::Separator(); ImGui::Spacing();

            // FASE 0
            if (faseSintoma == 0) {
                ImGui::Text("Identifique el primer sintoma observable en el equipo:");
                ImGui::Spacing();

                if (ImGui::Button("1. La PC no da senales de encendido (sin luces/ventiladores)", ImVec2(-1, 35))) { ramaActiva = 0; faseSintoma = 1; pasoRama = 0; }
                if (ImGui::Button("2. La PC enciende pero la pantalla esta negra y emite pitidos", ImVec2(-1, 35))) { ramaActiva = 1; faseSintoma = 1; pasoRama = 0; }
                if (ImGui::Button("3. Advertencia critica de SOBRECALENTAMIENTO del CPU al iniciar", ImVec2(-1, 35))) { ramaActiva = 2; faseSintoma = 1; pasoRama = 0; }
                if (ImGui::Button("4. La PC se reinicia continuamente (Boot Loop)", ImVec2(-1, 35))) { ramaActiva = 3; faseSintoma = 1; pasoRama = 0; }
                if (ImGui::Button("5. Falla termica o desconexion de unidad SSD M.2 NVMe", ImVec2(-1, 35))) { ramaActiva = 4; faseSintoma = 1; pasoRama = 0; }
                if (ImGui::Button("6. El sistema enciende pero sufre lentitud o cuellos de botella", ImVec2(-1, 35))) { ramaActiva = 5; faseSintoma = 1; pasoRama = 0; }
                if (ImGui::Button("7. Inestabilidad general o pantallas azules (RAM)", ImVec2(-1, 35))) { ramaActiva = 6; faseSintoma = 1; pasoRama = 0; }
            }

            // FASE 1
            else if (faseSintoma == 1) {
                if (ramaActiva == 0) {
                    if (pasoRama == 0) {
                        ImGui::TextWrapped("¿El cable de energia esta firmemente conectado al equipo y al tomacorriente?");
                        ImGui::Spacing();
                        if (ImGui::Button("SI", ImVec2(150, 40))) { pasoRama = 1; } ImGui::SameLine();
                        if (ImGui::Button("NO", ImVec2(150, 40))) { dictamenFinal = {"Fuente de Poder", "Cable desconectado", "Falla de alimentacion basica.", "1. Desconecte el cable de poder de la parte trasera.\n2. Reconecte firmemente en ambos extremos.\n3. Pruebe encender.", 101}; faseSintoma = 2; }
                    }
                    else if (pasoRama == 1) {
                        ImGui::TextWrapped("¿El interruptor de la fuente de poder (parte trasera) esta en ON / I?");
                        ImGui::Spacing();
                        if (ImGui::Button("SI", ImVec2(150, 40))) { pasoRama = 2; } ImGui::SameLine();
                        if (ImGui::Button("NO", ImVec2(150, 40))) { dictamenFinal = {"Fuente de Poder", "Interruptor apagado", "Interruptor principal en OFF.", "1. Posicione el interruptor trasero en 'I'.\n2. Intente encender el equipo.", 102}; faseSintoma = 2; }
                    }
                    else if (pasoRama == 2) {
                        ImGui::TextWrapped("¿Al encender, el ventilador de la fuente gira un instante?");
                        ImGui::Spacing();
                        if (ImGui::Button("SI (Gira)", ImVec2(150, 40))) { temporalRamaAGira = true; pasoRama = 3; } ImGui::SameLine();
                        if (ImGui::Button("NO (No gira)", ImVec2(150, 40))) { temporalRamaAGira = false; pasoRama = 3; }
                    }
                    else if (pasoRama == 3) {
                        ImGui::TextWrapped("¿El equipo a veces enciende y otras veces no (es aleatorio)?");
                        ImGui::Spacing();
                        bool respondio = false; bool intermitente = false;
                        if (ImGui::Button("SI", ImVec2(150, 40))) { intermitente = true; respondio = true; } ImGui::SameLine();
                        if (ImGui::Button("NO", ImVec2(150, 40))) { intermitente = false; respondio = true; }

                        if (respondio) {
                            if (!temporalRamaAGira && !intermitente) { dictamenFinal = {"Fuente de Poder", "Falla total de PSU", "La fuente no entrega senal.", "1. Desconecte el equipo.\n2. Realice la prueba del clip (puentear PS_ON y GND).\n3. Si no gira, reemplace la fuente de poder.", 103}; }
                            else if (!temporalRamaAGira && intermitente) { dictamenFinal = {"Fuente de Poder", "Ventilador trabado", "Riesgo de sobrecalentamiento en PSU.", "1. Apague de inmediato.\n2. Limpie el polvo de la PSU.\n3. Si el ventilador sigue fallando, reemplace la fuente.", 104}; }
                            else if (temporalRamaAGira && intermitente) { dictamenFinal = {"Fuente de Poder", "Voltaje inestable", "Potencia insuficiente o condensador danado.", "1. Desconecte dispositivos externos.\n2. Si enciende, la fuente no soporta la carga total. Reemplace por una de mayor wattage.", 105}; }
                            else { dictamenFinal = {"Placa Base", "Falla de POST", "Fuente operativa pero placa inactiva.", "1. Realice un Clear CMOS (retire pila 30s).\n2. Pruebe encender solo con CPU, 1 RAM y PSU.", 106}; }
                            faseSintoma = 2;
                        }
                    }
                }
                else if (ramaActiva == 1) {
                    ImGui::TextWrapped("Seleccione el patron de pitidos que escucha desde la placa base:"); ImGui::Spacing();
                    if (ImGui::Button("1. Pitidos cortos repetitivos", ImVec2(-1, 35))) { dictamenFinal = {"Memoria RAM", "No detectada", "Fallo de inicializacion de RAM.", "1. Retire la RAM, limpie contactos con goma de borrar.\n2. Reinserte un solo modulo en el slot principal.", 201}; faseSintoma = 2; }
                    if (ImGui::Button("2. 1 pitido largo + 2 o 3 cortos", ImVec2(-1, 35))) { dictamenFinal = {"Tarjeta Grafica", "No inicializada", "Falla en GPU.", "1. Retire la GPU y limpie el slot PCIe.\n2. Verifique conectores PCIe de energia.\n3. Pruebe otra GPU.", 202}; faseSintoma = 2; }
                    if (ImGui::Button("3. Tono continuo", ImVec2(-1, 35))) { dictamenFinal = {"Placa Base", "Corto circuito", "Falla critica general.", "1. Desconecte todo lo no esencial.\n2. Inspeccione la placa buscando quemaduras o condensadores inflados.", 203}; faseSintoma = 2; }
                    if (ImGui::Button("4. Sin pitidos, pantalla negra", ImVec2(-1, 35))) { dictamenFinal = {"Video", "Sin senal", "Problema de cable o monitor.", "1. Verifique cables de video.\n2. Pruebe conectar al puerto de la placa base en vez de la GPU.", 204}; faseSintoma = 2; }
                }
                else if (ramaActiva == 2) {
                    if (pasoRama == 0) {
                        ImGui::TextWrapped("¿El ventilador del procesador gira al intentar arrancar?"); ImGui::Spacing();
                        if (ImGui::Button("SI", ImVec2(150, 40))) { pasoRama = 1; } ImGui::SameLine();
                        if (ImGui::Button("NO", ImVec2(150, 40))) { dictamenFinal = {"Enfriamiento", "Ventilador inactivo", "Falla termica por ventilador.", "1. Verifique la conexion al puerto CPU_FAN en la placa base.\n2. Si esta conectado y no gira, cambie el ventilador.", 301}; faseSintoma = 2; }
                    }
                    else if (pasoRama == 1) {
                        ImGui::TextWrapped("¿Se ha cambiado la pasta termica en los ultimos 2 anos?"); ImGui::Spacing();
                        if (ImGui::Button("SI", ImVec2(150, 40))) { dictamenFinal = {"Placa Base", "Falla de sensor", "Sensor reporta falso calor.", "1. Actualice la BIOS.\n2. Verifique parametros de temperatura en la utilidad del sistema.", 303}; faseSintoma = 2; } ImGui::SameLine();
                        if (ImGui::Button("NO", ImVec2(150, 40))) { dictamenFinal = {"Enfriamiento", "Mantenimiento termico", "Pasta termica seca.", "1. Retire el disipador.\n2. Limpie con alcohol isopropilico.\n3. Aplique pasta nueva y reinstale.", 302}; faseSintoma = 2; }
                    }
                }
                else if (ramaActiva == 3) {
                    if (pasoRama == 0) {
                        ImGui::TextWrapped("¿El equipo reinicia ANTES de mostrar el logo de la marca (BIOS)?"); ImGui::Spacing();
                        if (ImGui::Button("SI", ImVec2(150, 40))) { dictamenFinal = {"Hardware", "Falla Pre-POST", "Inestabilidad electrica grave.", "1. Haga Clear CMOS.\n2. Pruebe usando un solo modulo de memoria RAM.", 401}; faseSintoma = 2; } ImGui::SameLine();
                        if (ImGui::Button("NO", ImVec2(150, 40))) { pasoRama = 1; }
                    }
                    else if (pasoRama == 1) {
                        ImGui::TextWrapped("¿Llega a mostrar el logo de Windows antes del reinicio?"); ImGui::Spacing();
                        if (ImGui::Button("SI", ImVec2(150, 40))) { dictamenFinal = {"Software", "Conflicto de SO", "Falla de driver o actualizacion.", "1. Inicie en Modo Seguro (F8 o tras 3 reinicios).\n2. Desinstale actualizaciones recientes o drivers de video.", 403}; faseSintoma = 2; } ImGui::SameLine();
                        if (ImGui::Button("NO", ImVec2(150, 40))) { dictamenFinal = {"Software", "Bootloader corrupto", "Cargador de arranque danado.", "1. Inicie con USB de instalacion.\n2. Abra CMD y ejecute: bootrec /fixmbr, /fixboot, /rebuildbcd.", 402}; faseSintoma = 2; }
                    }
                }
                else if (ramaActiva == 4) {
                    if (pasoRama == 0) {
                        ImGui::TextWrapped("¿El SSD NVMe desaparece del sistema tras un rato de uso intensivo?"); ImGui::Spacing();
                        if (ImGui::Button("SI", ImVec2(150, 40))) { dictamenFinal = {"Almacenamiento", "Thermal Throttling", "El SSD corta la conexion por calor extremo.", "1. Instale un disipador termico pasivo en el SSD M.2.\n2. Mejore el flujo de aire del chasis.", 501}; faseSintoma = 2; } ImGui::SameLine();
                        if (ImGui::Button("NO", ImVec2(150, 40))) { dictamenFinal = {"Almacenamiento", "Chequeo Preventivo", "Sin calor extremo detectado.", "1. Utilice CrystalDiskInfo para evaluar la salud (SMART) del SSD.\n2. Realice un escaneo de firmware.", 502}; faseSintoma = 2; }
                    }
                }
                else if (ramaActiva == 5) {
                    if (pasoRama == 0) {
                        ImGui::TextWrapped("¿El uso de CPU en reposo supera el 40%% en el Administrador de Tareas?"); ImGui::Spacing();
                        if (ImGui::Button("SI", ImVec2(150, 40))) { dictamenFinal = {"Rendimiento", "Proceso parasito", "Consumo anomal de CPU.", "1. Identifique el proceso.\n2. Realice un analisis con software antimalware.", 601}; faseSintoma = 2; } ImGui::SameLine();
                        if (ImGui::Button("NO", ImVec2(150, 40))) { pasoRama = 1; }
                    }
                    else if (pasoRama == 1) {
                        ImGui::TextWrapped("¿El disco duro se mantiene al 100%% de uso constante?"); ImGui::Spacing();
                        if (ImGui::Button("SI", ImVec2(150, 40))) { dictamenFinal = {"Rendimiento", "Saturacion I/O", "Cuello de botella de disco.", "1. Desactive el servicio SysMain (Superfetch).\n2. Si usa disco mecanico, considere migrar a SSD.", 602}; faseSintoma = 2; } ImGui::SameLine();
                        if (ImGui::Button("NO", ImVec2(150, 40))) { dictamenFinal = {"Rendimiento", "Saturacion de Inicio", "Exceso de aplicaciones.", "1. Abra el Administrador de tareas (Pestana Inicio).\n2. Deshabilite aplicaciones innecesarias en el arranque.", 603}; faseSintoma = 2; }
                    }
                }
                else if (ramaActiva == 6) {
                    if (pasoRama == 0) {
                        ImGui::TextWrapped("¿Ocurren Pantallazos Azules (BSOD) mientras juega o edita?"); ImGui::Spacing();
                        if (ImGui::Button("SI", ImVec2(150, 40))) { dictamenFinal = {"RAM", "Inestabilidad XMP/EXPO", "Perfil muy agresivo o RAM danada.", "1. Ejecute MemTest86 desde un USB.\n2. Si hay errores, baje la velocidad de la RAM en la BIOS o cambie el modulo.", 702}; faseSintoma = 2; } ImGui::SameLine();
                        if (ImGui::Button("NO", ImVec2(150, 40))) { dictamenFinal = {"RAM", "Configuracion Base", "Verifique que la velocidad coincida con la caja.", "1. Entre a la BIOS.\n2. Active el Perfil XMP / EXPO para alcanzar el rendimiento pagado.", 701}; faseSintoma = 2; }
                    }
                }
            }

            // FASE 2
            else if (faseSintoma == 2) {
                ImGui::TextColored(ImVec4(0.2f, 0.9f, 0.2f, 1.0f), "DIAGNOSTICO COMPLETADO EXITOSAMENTE");
                ImGui::Separator();
                ImGui::Text("Categoria: %s", dictamenFinal.categoria.c_str());
                ImGui::Text("Sub-categoria: %s", dictamenFinal.subCategoria.c_str());
                ImGui::TextWrapped("Descripcion: %s", dictamenFinal.descripcion.c_str());

                ImGui::Spacing();
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "GUIA DE SOLUCION PASO A PASO:");
                ImGui::TextWrapped("%s", dictamenFinal.solucionPasos.c_str());

                ImGui::Spacing();
                ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Codigo interno de soporte: #%d", dictamenFinal.codigoInterno);

                ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
                if (ImGui::Button("Guardar Reporte en Nube y Volver al Menu", ImVec2(-1, 40))) {

                    string prefijo = (dniUsuarioLogueado.length() >= 4) ? dniUsuarioLogueado.substr(0, 4) : "USER";
                    string ticketGeneradoID = prefijo + "-" + generarTicketID();

                    CloudGateway::sincronizarReporteSupabase(hardwareDetectado, dictamenFinal, ticketGeneradoID);
                    exportarReporteFisico(hardwareDetectado, dictamenFinal, ticketGeneradoID);

                    estadoActual = PANTALLA_DASHBOARD;
                }
            }

            ImGui::Spacing(); ImGui::Separator();
            if (ImGui::Button("Abortar Diagnostico", ImVec2(-1, 30))) {
                estadoActual = PANTALLA_DASHBOARD;
            }
            ImGui::End();
        }

        // 4. Renderizado grafico final en la pantalla
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        glClearColor(0.10f, 0.11f, 0.14f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // 5. Destrucción de punteros al cerrar el programa
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}