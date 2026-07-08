cmake_minimum_required(VERSION 3.20)
project(SistemaDiagnostico)

set(CMAKE_CXX_STANDARD 17)

# ====================================================================
# CONFIGURACIÓN MANUAl DE LIBRERÍAS (DISCO D)
# ====================================================================

# Configuración para cURL
set(CURL_ROOT_DIR "D:/curl")
set(CMAKE_PREFIX_PATH "D:/curl")
find_package(CURL REQUIRED)

# Configuración para GLFW (Ruta manual al disco D)
set(GLFW_DIR "D:/glfw")
find_library(GLFW_LIBRARY NAMES glfw3 PATHS "${GLFW_DIR}/lib-mingw-w64")
include_directories("${GLFW_DIR}/include")

# Encontrar OpenGL (Viene nativo en Windows, no hay que descargar nada)
find_package(OpenGL REQUIRED)

# ====================================================================
# COMPILACIÓN DEL PROYECTO + IMGUI
# ====================================================================
add_executable(SistemaDiagnostico
        main.cpp
        Autenticacion.cpp
        CloudServiceGateway.cpp
        LectorEspecificaciones.cpp
        MotorDiagnostico.cpp
        MotorDiagnostico_Ramas.cpp
        GestorDatos.cpp
        BuzonSugerencias.cpp

        # Agregamos los archivos de ImGui que pusiste en su carpeta
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_impl_glfw.cpp
        imgui/imgui_impl_opengl3.cpp
)

# ====================================================================
# ENLACE DE LIBRERÍAS (LINKEADO)
# ====================================================================
target_link_libraries(SistemaDiagnostico PRIVATE
        CURL::libcurl
        ${GLFW_LIBRARY}
        OpenGL::GL
)