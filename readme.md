# Docs

Generador de documentación automática ultraminimalista.

## Características

- Escanea archivos C/H automáticamente
- Extrae funciones y comentarios
- Genera HTML o Markdown
- Sin configuración necesaria
- Recursivo por directorios
- Menos de 400 líneas
- Salida visual limpia

## Instalación

```bash
gcc docs.c -o docs
```

## Uso básico

```bash
# Documentar directorio actual
./docs

# Documentar directorio específico
./docs src/

# Salida personalizada
./docs -o api.html

# Generar markdown
./docs -m -o API.md

# Directorio + output
./docs src/ -o documentacion.html
```

## Cómo documentar tu código

Simplemente pon comentarios arriba de tus funciones:

```c
// Suma dos números enteros
int suma(int a, int b) {
    return a + b;
}

/* Busca un valor en el array
   Retorna el índice o -1 */
int buscar(int *arr, int n, int val) {
    for (int i = 0; i < n; i++) {
        if (arr[i] == val) return i;
    }
    return -1;
}

// Ordena array in-place usando bubble sort
void ordenar(int *arr, int n) {
    // implementación...
}
```

## Ejemplo de salida HTML

Genera un HTML limpio y profesional con:
- Título y resumen
- Funciones organizadas por archivo
- Nombre de función destacado
- Parámetros en monospace
- Descripción del comentario
- Ubicación (archivo:línea)
- Estilo moderno y responsive

## Ejemplo de salida Markdown

```markdown
# Documentación del Proyecto

**Total de funciones:** 15

## util.c

### `suma`
**Parámetros:** `int a, int b`
Suma dos números enteros
*Ubicación:* `util.c:5`

---

### `buscar`
**Parámetros:** `int *arr, int n, int val`
Busca un valor en el array. Retorna el índice o -1
*Ubicación:* `util.c:12`
```

## Flujo de trabajo

```bash
# 1. Escribe código con comentarios
vim mi_lib.c

# 2. Genera documentación
./docs -o api.html

# 3. Abre en navegador
firefox api.html

# 4. O genera markdown para GitHub
./docs -m -o API.md
git add API.md
```

## Con watch para auto-generar

```bash
# Regenera docs cada vez que cambias el código
./watch "./docs && echo Docs actualizadas"
```

## Combinar con otros tools

```bash
# Documentar antes de empaquetar
./docs -o docs.html
./pack create proyecto.pack

# Snapshot con documentación
./docs
./snap save "versión con docs"

# Generar docs en sistema de build
# En Blazefile:
# docs: src/*.c | ./docs src/ -o api.html
```

## Qué extrae

- **Funciones**: Nombre y parámetros
- **Comentarios**: // o /* */ arriba de funciones
- **Ubicación**: Archivo y número de línea
- **Organización**: Agrupa por archivo fuente

## Qué NO hace

- No parsea sintaxis compleja
- No extrae tipos de retorno (por simplicidad)
- No documenta structs/enums
- No genera diagramas

Es minimalista intencionalmente.

## Opciones

- Sin opciones: Genera `docs.html` del directorio actual
- `directorio`: Escanea ese directorio
- `-o archivo`: Especifica archivo de salida
- `-m`: Genera Markdown en lugar de HTML
- `-h`: Muestra ayuda

## Formato de comentarios

**Funciona:**
```c
// Una línea
int func1(void);

/* Multilinea
   funciona también */
int func2(void);

// Varias líneas seguidas
// se concatenan
int func3(void);
```

**No funciona:**
```c
int func(void); // comentario después

/* comentario alejado */

int func2(void); // no se asocia
```

El comentario debe estar **inmediatamente arriba** de la función.

## Ventajas

- **Rápido**: Genera docs en segundos
- **Simple**: No necesitas aprender sintaxis especial
- **Visual**: HTML se ve profesional
- **Portable**: Markdown para GitHub/GitLab
- **Integrable**: Funciona con tu build system

## Limitaciones

- Máximo 512 funciones
- Solo archivos .c y .h
- Comentarios simples (no Doxygen)
- No thread-safe (no importa, es CLI)

## Casos de uso

**Documentar API pública:**
```bash
./docs include/ -o public_api.html
```

**Docs internas del equipo:**
```bash
./docs src/ -m -o INTERNAL.md
```

**Referencia rápida:**
```bash
./docs -o quick_ref.html
firefox quick_ref.html
```

**Integración continua:**
```bash
# En CI/CD
./docs -m -o API.md
git add API.md
git commit -m "Update API docs"
```

## Estilo del HTML

El HTML generado usa:
- Diseño responsive (se adapta a móvil)
- Fuentes del sistema
- Colores profesionales
- Scroll suave
- Sin JavaScript
- CSS inline (un solo archivo)

Puedes abrirlo directamente sin servidor web.

## Personalizar salida

El código es simple, puedes modificar:
- Colores en la sección CSS
- Formato del HTML
- Estilo del Markdown
- Información extraída

Todo en menos de 400 líneas legibles.

## Licencia

Dominio público. Úsalo como quieras.