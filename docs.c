/* docs.c - Generador de documentación ultraminimalista */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <dirent.h>

#define MAX_LINE 1024
#define MAX_NAME 256
#define MAX_DESC 512
#define MAX_FUNCS 512

typedef struct {
    char name[MAX_NAME];
    char params[MAX_DESC];
    char desc[MAX_DESC];
    char returns[MAX_DESC];
    char file[MAX_NAME];
    int line;
} Function;

static Function funcs[MAX_FUNCS];
static int nfuncs = 0;

static void trim(char *str) {
    char *end;
    while(isspace(*str)) str++;
    if(*str == 0) return;
    end = str + strlen(str) - 1;
    while(end > str && isspace(*end)) end--;
    end[1] = '\0';
}

static int is_function_line(const char *line) {
    if (strstr(line, "(") && strstr(line, ")") && !strstr(line, ";")) {
        if (strstr(line, "if") || strstr(line, "while") || 
            strstr(line, "for") || strstr(line, "switch"))
            return 0;
        return 1;
    }
    return 0;
}

static void extract_function(const char *line, char *name, char *params) {
    const char *paren = strchr(line, '(');
    if (!paren) return;
    
    const char *start = paren - 1;
    while (start > line && (isalnum(*start) || *start == '_')) start--;
    start++;
    
    size_t len = paren - start;
    if (len >= MAX_NAME) len = MAX_NAME - 1;
    strncpy(name, start, len);
    name[len] = '\0';
    
    const char *close = strchr(paren, ')');
    if (close) {
        len = close - paren - 1;
        if (len >= MAX_DESC) len = MAX_DESC - 1;
        strncpy(params, paren + 1, len);
        params[len] = '\0';
        trim(params);
    }
}

static void parse_file(const char *filepath) {
    FILE *f = fopen(filepath, "r");
    if (!f) return;
    
    char line[MAX_LINE];
    char comment[MAX_DESC] = {0};
    int line_num = 0;
    
    while (fgets(line, sizeof(line), f)) {
        line_num++;
        trim(line);
        
        if (strncmp(line, "//", 2) == 0 || strncmp(line, "/*", 2) == 0) {
            char *start = line + 2;
            while (isspace(*start)) start++;
            if (*start == '*') start++;
            while (isspace(*start)) start++;
            
            if (strlen(comment) > 0) strcat(comment, " ");
            strncat(comment, start, MAX_DESC - strlen(comment) - 1);
        } 
        else if (is_function_line(line) && strstr(line, "{")) {
            if (nfuncs >= MAX_FUNCS) break;
            
            Function *fn = &funcs[nfuncs++];
            extract_function(line, fn->name, fn->params);
            strncpy(fn->file, filepath, MAX_NAME - 1);
            fn->line = line_num;
            
            if (strlen(comment) > 0) {
                strncpy(fn->desc, comment, MAX_DESC - 1);
            }
            
            comment[0] = '\0';
        }
        else if (strlen(line) > 0 && line[0] != '#') {
            comment[0] = '\0';
        }
    }
    
    fclose(f);
}

static void scan_directory(const char *dir) {
    DIR *d = opendir(dir);
    if (!d) return;
    
    struct dirent *e;
    while ((e = readdir(d))) {
        if (e->d_name[0] == '.') continue;
        
        char path[MAX_LINE];
        snprintf(path, MAX_LINE, "%s/%s", dir, e->d_name);
        
        struct stat st;
        if (stat(path, &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                scan_directory(path);
            } else {
                size_t len = strlen(e->d_name);
                if ((len > 2 && strcmp(e->d_name + len - 2, ".c") == 0) ||
                    (len > 2 && strcmp(e->d_name + len - 2, ".h") == 0)) {
                    printf("  → %s\n", path);
                    parse_file(path);
                }
            }
        }
    }
    closedir(d);
}

static void generate_html(const char *output) {
    FILE *f = fopen(output, "w");
    if (!f) {
        fprintf(stderr, "error: no se pudo crear %s\n", output);
        return;
    }
    
    fprintf(f, "<!DOCTYPE html>\n");
    fprintf(f, "<html>\n<head>\n");
    fprintf(f, "<meta charset='utf-8'>\n");
    fprintf(f, "<title>Documentación del Proyecto</title>\n");
    fprintf(f, "<style>\n");
    fprintf(f, "body { font-family: system-ui; max-width: 900px; margin: 40px auto; padding: 20px; line-height: 1.6; }\n");
    fprintf(f, "h1 { color: #333; border-bottom: 3px solid #0066cc; padding-bottom: 10px; }\n");
    fprintf(f, "h2 { color: #0066cc; margin-top: 30px; }\n");
    fprintf(f, ".function { background: #f5f5f5; padding: 20px; margin: 20px 0; border-left: 4px solid #0066cc; }\n");
    fprintf(f, ".name { font-size: 1.3em; font-weight: bold; color: #333; }\n");
    fprintf(f, ".params { color: #666; margin: 10px 0; font-family: monospace; }\n");
    fprintf(f, ".desc { margin: 10px 0; }\n");
    fprintf(f, ".meta { color: #999; font-size: 0.9em; margin-top: 10px; }\n");
    fprintf(f, "</style>\n");
    fprintf(f, "</head>\n<body>\n");
    
    fprintf(f, "<h1>📚 Documentación del Proyecto</h1>\n");
    fprintf(f, "<p>Generada automáticamente con docs</p>\n");
    fprintf(f, "<p><strong>Total de funciones:</strong> %d</p>\n", nfuncs);
    
    char current_file[MAX_NAME] = {0};
    for (int i = 0; i < nfuncs; i++) {
        Function *fn = &funcs[i];
        
        if (strcmp(current_file, fn->file) != 0) {
            if (current_file[0]) fprintf(f, "\n");
            fprintf(f, "<h2>📄 %s</h2>\n", fn->file);
            strncpy(current_file, fn->file, MAX_NAME - 1);
        }
        
        fprintf(f, "<div class='function'>\n");
        fprintf(f, "  <div class='name'>%s</div>\n", fn->name);
        
        if (strlen(fn->params) > 0) {
            fprintf(f, "  <div class='params'>(%s)</div>\n", fn->params);
        }
        
        if (strlen(fn->desc) > 0) {
            fprintf(f, "  <div class='desc'>%s</div>\n", fn->desc);
        }
        
        fprintf(f, "  <div class='meta'>%s:%d</div>\n", fn->file, fn->line);
        fprintf(f, "</div>\n");
    }
    
    fprintf(f, "</body>\n</html>\n");
    fclose(f);
}

static void generate_markdown(const char *output) {
    FILE *f = fopen(output, "w");
    if (!f) {
        fprintf(stderr, "error: no se pudo crear %s\n", output);
        return;
    }
    
    fprintf(f, "# Documentación del Proyecto\n\n");
    fprintf(f, "Generada automáticamente con docs\n\n");
    fprintf(f, "**Total de funciones:** %d\n\n", nfuncs);
    
    char current_file[MAX_NAME] = {0};
    for (int i = 0; i < nfuncs; i++) {
        Function *fn = &funcs[i];
        
        if (strcmp(current_file, fn->file) != 0) {
            fprintf(f, "\n## %s\n\n", fn->file);
            strncpy(current_file, fn->file, MAX_NAME - 1);
        }
        
        fprintf(f, "### `%s`\n\n", fn->name);
        
        if (strlen(fn->params) > 0) {
            fprintf(f, "**Parámetros:** `%s`\n\n", fn->params);
        }
        
        if (strlen(fn->desc) > 0) {
            fprintf(f, "%s\n\n", fn->desc);
        }
        
        fprintf(f, "*Ubicación:* `%s:%d`\n\n", fn->file, fn->line);
        fprintf(f, "---\n\n");
    }
    
    fclose(f);
}

static void usage(void) {
    printf("docs - generador de documentación\n\n");
    printf("Uso:\n");
    printf("  docs [directorio] [opciones]\n\n");
    printf("Opciones:\n");
    printf("  -o <archivo>  archivo de salida (default: docs.html)\n");
    printf("  -m            generar markdown en lugar de HTML\n\n");
    printf("Ejemplos:\n");
    printf("  docs                    # documenta directorio actual\n");
    printf("  docs src/               # documenta src/\n");
    printf("  docs -o api.html        # salida a api.html\n");
    printf("  docs -m -o API.md       # salida markdown\n");
    exit(0);
}

int main(int argc, char **argv) {
    const char *dir = ".";
    const char *output = "docs.html";
    int markdown = 0;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            usage();
        } else if (strcmp(argv[i], "-m") == 0) {
            markdown = 1;
            output = "API.md";
        } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            output = argv[++i];
        } else {
            dir = argv[i];
        }
    }
    
    printf("Escaneando archivos en %s...\n", dir);
    scan_directory(dir);
    
    printf("\nEncontradas %d funciones\n", nfuncs);
    printf("Generando documentación...\n");
    
    if (markdown) {
        generate_markdown(output);
    } else {
        generate_html(output);
    }
    
    printf("✓ Documentación generada en %s\n", output);
    
    return 0;
}