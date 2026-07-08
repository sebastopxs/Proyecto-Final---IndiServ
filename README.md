# IndiServ Analytica - Sistema Experto de Diagnóstico de PC

IndiServ Analytica es una aplicación de escritorio desarrollada en C++17 que implementa un sistema experto lógico para diagnosticar y resolver fallos de inicio, hardware y rendimiento en computadoras personales. 

El sistema emula el proceso de toma de decisiones de un técnico informático mediante un motor de inferencia basado en encadenamiento hacia adelante (Forward Chaining) y la aplicación de reglas condicionales (IF-THEN). Integra telemetría de hardware en tiempo real y persistencia de datos en la nube.

## Características Principales

* **Interfaz Gráfica de Alto Rendimiento:** Interfaz interactiva y de bajo consumo de recursos renderizada a través de la GPU utilizando la biblioteca Dear ImGui y OpenGL.
* **Telemetría WMI Automatizada:** Extracción precisa de especificaciones de hardware (CPU, núcleos, RAM instalada y disponible) utilizando Windows Management Instrumentation (WMI), eliminando el sesgo de entrada del usuario.
* **Motor de Inferencia:** Sistema de diagnóstico estructurado en ramas lógicas para la detección de fallas eléctricas, códigos de error POST (Beeps), problemas de sobrecalentamiento, bucles de reinicio (Boot Loops) e integridad de almacenamiento.
* **Integración Cloud (BaaS):** Conexión HTTP RESTful con Supabase para la validación de identidad de usuarios, generación de reportes JSON y registro de tickets de soporte.
* **Ciberseguridad:** Sistema de autenticación de doble capa que valida el acceso de los usuarios contra un padrón oficial en la nube, manejando hashes de contraseña y prevención de colisiones en la creación de cuentas.

## Arquitectura del Proyecto

El código fuente está dividido en módulos lógicos para garantizar la escalabilidad y el mantenimiento:

* `main.cpp`: Contiene la Máquina de Estados Finitos (FSM) que controla el bucle de renderizado gráfico (Main Loop).
* `Autenticacion.cpp / .h`: Gestiona el control de acceso, registro y validación de usuarios.
* `LectorEspecificaciones.cpp / .h`: Ejecuta comandos de sistema para recolectar información del hardware anfitrión.
* `MotorDiagnostico.cpp / .h` y `MotorDiagnostico_Ramas.cpp`: Ejecutan el árbol de decisiones y almacenan las guías de resolución.
* `CloudServiceGateway.cpp / .h`: Administra las peticiones HTTP (GET/POST) hacia la base de datos externa.
* `GestorDatos.cpp / .h` y `BuzonSugerencias.cpp`: Gestionan la exportación de reportes locales (.txt) y el manejo de feedback.

## Tecnologías y Dependencias

* **Lenguaje:** C++17
* **Renderizado y Ventanas:** GLFW3, OpenGL3, Dear ImGui
* **Comunicaciones de Red:** libcurl
* **Manejo de Datos Estructurados:** nlohmann/json
* **Decodificación de Imágenes:** stb_image
* **Base de Datos / Backend:** Supabase (PostgreSQL)

## Instrucciones de Instalación y Ejecución

Para ejecutar el programa correctamente en un entorno local, es estrictamente necesario restaurar la estructura de directorios generada durante la compilación.

### Paso 1: Clonar el repositorio
```bash
git clone [https://github.com/sebastopxs/Proyecto-Final---IndiServ.git](https://github.com/sebastopxs/Proyecto-Final---IndiServ.git)
