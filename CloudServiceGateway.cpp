#include <iostream>
#include <string>
#include <curl/curl.h>
#include "CloudServiceGateway.h"
#include "json.hpp" // nlohmann/json library for parsing

using namespace std;
using json = nlohmann::json;

// Credenciales de Base de Datos (Supabase)
const string SUPABASE_URL = "https://pvrfnvrxaxulcxacdino.supabase.co";
const string SUPABASE_KEY = "sb_publishable_G7D4dhh3Q2LWzgRnzRRa9A_4ngZ5FiW";

// Callback function to handle incoming HTTP response data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// ====================================================================
// FUNCIONES DE TUS COMPAÑEROS (SEGURIDAD Y REGISTRO)
// ====================================================================

InfoPadron consultarPadronNube(string dni) {
    InfoPadron result = {false, "", ""};
    CURL* curl = curl_easy_init();
    string readBuffer;

    if(curl) {
        string url = SUPABASE_URL + "/rest/v1/padron_oficial?dni=eq." + dni + "&select=*";
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, ("apikey: " + SUPABASE_KEY).c_str());
        headers = curl_slist_append(headers, ("Authorization: Bearer " + SUPABASE_KEY).c_str());

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        if(curl_easy_perform(curl) == CURLE_OK) {
            auto jsonResponse = json::parse(readBuffer);
            if (!jsonResponse.empty()) {
                result.existe = true;
                result.nombresCompletos = jsonResponse[0]["nombres_completos"];
                result.carrera = jsonResponse[0]["carrera"];
            }
        }
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    return result;
}

bool registrarUsuarioSupabase(string dni, string correo, string password, string nickname) {
    CURL* curl = curl_easy_init();
    bool exito = false;

    if(curl) {
        string url = SUPABASE_URL + "/rest/v1/usuarios_app";
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, ("apikey: " + SUPABASE_KEY).c_str());
        headers = curl_slist_append(headers, ("Authorization: Bearer " + SUPABASE_KEY).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "Prefer: return=minimal");

        json cuerpoJson;
        cuerpoJson["dni"] = dni;
        cuerpoJson["correo"] = correo;
        cuerpoJson["password_hash"] = password;
        cuerpoJson["nickname"] = nickname;

        string jsonStr = cuerpoJson.dump();

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonStr.c_str());

        long http_code = 0;
        if(curl_easy_perform(curl) == CURLE_OK) {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            if(http_code >= 200 && http_code < 300) exito = true;
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    return exito;
}

bool verificarCorreoExistente(string correo) {
    CURL* curl = curl_easy_init();
    string readBuffer;
    bool existe = false;

    if(curl) {
        string url = SUPABASE_URL + "/rest/v1/usuarios_app?correo=eq." + correo + "&select=correo";
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, ("apikey: " + SUPABASE_KEY).c_str());
        headers = curl_slist_append(headers, ("Authorization: Bearer " + SUPABASE_KEY).c_str());

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        if(curl_easy_perform(curl) == CURLE_OK) {
            try {
                auto jsonRespuesta = json::parse(readBuffer);
                if (jsonRespuesta.is_array() && !jsonRespuesta.empty()) {
                    existe = true;
                }
            } catch (...) {}
        }
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    return existe;
}

InfoUsuarioApp consultarUsuarioAppEnNube(string dni) {
    InfoUsuarioApp resultado = {false, "", ""};
    CURL* curl = curl_easy_init();
    string readBuffer;

    if(curl) {
        string url = SUPABASE_URL + "/rest/v1/usuarios_app?dni=eq." + dni + "&select=*";
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, ("apikey: " + SUPABASE_KEY).c_str());
        headers = curl_slist_append(headers, ("Authorization: Bearer " + SUPABASE_KEY).c_str());

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        if(curl_easy_perform(curl) == CURLE_OK) {
            auto jsonRespuesta = json::parse(readBuffer);
            if (!jsonRespuesta.empty()) {
                resultado.yaRegistrado = true;
                resultado.correo = jsonRespuesta[0]["correo"];
                resultado.passwordHash = jsonRespuesta[0]["password_hash"];
            }
        }
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    return resultado;
}

// ====================================================================
// TUS FUNCIONES DE DIAGNOSTICO Y SUGERENCIAS (SISTEMA EXPERTO)
// ====================================================================
namespace CloudGateway {

    bool sincronizarReporteSupabase(const PerfilHardware& perfil, const ResultadoDiagnostico& resultado, const string& ticketID) {
        CURL* curl = curl_easy_init();
        bool exito = false;

        if(curl) {
            string url = SUPABASE_URL + "/rest/v1/reportes_diagnostico";

            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, ("apikey: " + SUPABASE_KEY).c_str());
            headers = curl_slist_append(headers, ("Authorization: Bearer " + SUPABASE_KEY).c_str());
            headers = curl_slist_append(headers, "Content-Type: application/json");
            headers = curl_slist_append(headers, "Prefer: return=minimal");

            json payload;
            payload["id_ticket"] = ticketID;
            payload["cpu"] = perfil.nombreCPU;
            payload["ram"] = perfil.ramTotalMB;
            payload["diagnostico"] = resultado.categoria + " - " + resultado.subCategoria;
            payload["solucion"] = resultado.solucionPasos;

            string jsonStr = payload.dump();

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonStr.c_str());

            long http_code = 0;
            if(curl_easy_perform(curl) == CURLE_OK) {
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
                if(http_code >= 200 && http_code < 300) {
                    cout << "\033[32m  [CLOUD-OK] Telemetria del Ticket #" << ticketID << " respaldada en la nube.\033[0m\n";
                    exito = true;
                } else {
                    cout << "\033[31m  [ERROR CLOUD] Fallo la subida del reporte. Codigo HTTP: " << http_code << "\033[0m\n";
                }
            }

            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
        }
        return exito;
    }

    bool sincronizarSugerenciaSupabase(const string& sugerencia, const string& ticketID) {
        CURL* curl = curl_easy_init();
        bool exito = false;

        if(curl) {
            string url = SUPABASE_URL + "/rest/v1/sugerencias_locales";

            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, ("apikey: " + SUPABASE_KEY).c_str());
            headers = curl_slist_append(headers, ("Authorization: Bearer " + SUPABASE_KEY).c_str());
            headers = curl_slist_append(headers, "Content-Type: application/json");
            headers = curl_slist_append(headers, "Prefer: return=minimal");

            json payload;
            payload["id_ticket"] = ticketID;
            payload["sugerencia"] = sugerencia;

            string jsonStr = payload.dump();

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonStr.c_str());

            long http_code = 0;
            if(curl_easy_perform(curl) == CURLE_OK) {
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
                if(http_code >= 200 && http_code < 300) {
                    cout << "\033[32m  [CLOUD-OK] Sugerencia #" << ticketID << " registrada en la base de datos de los desarrolladores.\033[0m\n";
                    exito = true;
                }
            }

            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
        }
        return exito;
    }
}