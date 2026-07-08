/*
 * ============================================================
 * MotorDiagnostico_Ramas.cpp
 * Continuacion de la implementacion del arbol logico
 * ============================================================
 */

#include "MotorDiagnostico.h"

ResultadoDiagnostico ramaC_SobrecalentamientoCPU() {
    simularProcesamiento("Evaluando telemetria termica");
    if (!preguntarSiNo("El ventilador del procesador gira al intentar arrancar?")) {
        return {"Enfriamiento", "Ventilador inactivo", "Falla termica por ventilador.",
                "  1. Verifique la conexion al puerto CPU_FAN en la placa base.\n  2. Si esta conectado y no gira, cambie el ventilador.", 301};
    }
    if (!preguntarSiNo("Se ha cambiado la pasta termica en los ultimos 2 anos?")) {
        return {"Enfriamiento", "Mantenimiento termico", "Pasta termica seca.",
                "  1. Retire el disipador.\n  2. Limpie con alcohol isopropilico.\n  3. Aplique pasta nueva y reinstale.", 302};
    }
    return {"Placa Base", "Falla de sensor", "Sensor reporta falso calor.",
            "  1. Actualice la BIOS.\n  2. Verifique parametros de temperatura en la utilidad del sistema.", 303};
}

ResultadoDiagnostico ramaD_CicloReinicioContinuo() {
    simularProcesamiento("Analizando fallas de arranque y bucles (Boot Loop)");
    if (preguntarSiNo("El equipo reinicia ANTES de mostrar el logo de la marca (BIOS)?")) {
        return {"Hardware", "Falla Pre-POST", "Inestabilidad electrica grave.",
                "  1. Haga Clear CMOS.\n  2. Pruebe usando un solo modulo de memoria RAM.", 401};
    }
    if (!preguntarSiNo("Llega a mostrar el logo de Windows antes del reinicio?")) {
        return {"Software", "Bootloader corrupto", "Cargador de arranque danado.",
                "  1. Inicie con USB de instalacion.\n  2. Abra CMD y ejecute: bootrec /fixmbr, /fixboot, /rebuildbcd.", 402};
    }
    return {"Software", "Conflicto de SO", "Falla de driver o actualizacion.",
            "  1. Inicie en Modo Seguro (F8 o tras 3 reinicios).\n  2. Desinstale actualizaciones recientes o drivers de video.", 403};
}

ResultadoDiagnostico ramaE_FallaNVMe() {
    simularProcesamiento("Analizando integridad de almacenamiento solido");
    if (preguntarSiNo("El SSD NVMe desaparece del sistema tras un rato de uso intensivo?")) {
        return {"Almacenamiento", "Thermal Throttling", "El SSD corta la conexion por calor extremo.",
                "  1. Instale un disipador termico pasivo en el SSD M.2.\n  2. Mejore el flujo de aire del chasis.", 501};
    }
    return {"Almacenamiento", "Chequeo Preventivo", "Sin calor extremo detectado.",
            "  1. Utilice CrystalDiskInfo para evaluar la salud (SMART) del SSD.\n  2. Realice un escaneo de firmware.", 502};
}

ResultadoDiagnostico ramaF_CuelloDeBottella() {
    simularProcesamiento("Analizando rendimiento de memoria y CPU");
    if (preguntarSiNo("El uso de CPU en reposo supera el 40% en el Administrador de Tareas?")) {
        return {"Rendimiento", "Proceso parasito", "Consumo anomal de CPU.",
                "  1. Identifique el proceso.\n  2. Realice un analisis con software antimalware.", 601};
    }
    if (preguntarSiNo("El disco duro se mantiene al 100% de uso constante?")) {
        return {"Rendimiento", "Saturacion I/O", "Cuello de botella de disco.",
                "  1. Desactive el servicio SysMain (Superfetch).\n  2. Si usa disco mecanico, considere migrar a SSD.", 602};
    }
    return {"Rendimiento", "Saturacion de Inicio", "Exceso de aplicaciones.",
            "  1. Abra el Administrador de tareas (Pestana Inicio).\n  2. Deshabilite aplicaciones innecesarias en el arranque.", 603};
}

ResultadoDiagnostico ramaG_XmpExpoRAM() {
    simularProcesamiento("Evaluando perfiles de overcloking de memoria");
    if (preguntarSiNo("Ocurren Pantallazos Azules (BSOD) mientras juega o edita?")) {
        return {"RAM", "Inestabilidad XMP/EXPO", "Perfil muy agresivo o RAM danada.",
                "  1. Ejecute MemTest86 desde un USB.\n  2. Si hay errores, baje la velocidad de la RAM en la BIOS o cambie el modulo.", 702};
    }
    return {"RAM", "Configuracion Base", "Verifique que la velocidad coincida con la caja.",
            "  1. Entre a la BIOS.\n  2. Active el Perfil XMP / EXPO para alcanzar el rendimiento pagado.", 701};
}

bool registrarDiagnosticoEnBD(const ResultadoDiagnostico& resultado, int idSesion) {
    (void)resultado;
    (void)idSesion;
    return true;
}