#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_VARIABLES 100
#define MAX_TOKEN_LENGTH 100
#define MAX_LINE_LENGTH 1024

// Tipos de datos
typedef enum
{
    ENTERO,
    DECIMAL,
    CADENA,
    IDENTIFICADOR,
    OPERADOR,
    DELIMITADOR,
    PALABRA_CLAVE,
    TIPO_DESCONOCIDO
} TipoDato;

typedef struct
{
    char nombre[MAX_TOKEN_LENGTH];
    TipoDato tipo;
} Variable;

// Tabla de símbolos
Variable tabla_simbolos[MAX_VARIABLES];
int num_variables = 0;

// Prototipos de funciones
int agregar_variable(const char *nombre, TipoDato tipo);
TipoDato buscar_variable(const char *nombre);
TipoDato clasificar_token(const char *token);
void procesar_linea(const char *linea);
int verificar_asignacion(const char *nombre, TipoDato tipo);
int validar_condicion(const char *variable);

// Implementación de agregar_variable
int agregar_variable(const char *nombre, TipoDato tipo)
{
    for (int i = 0; i < num_variables; i++)
    {
        if (strcmp(tabla_simbolos[i].nombre, nombre) == 0)
        {
            printf("Error semántico: La variable '%s' ya está declarada.\n", nombre);
            return 0; // Error: Variable ya definida
        }
    }
    strcpy(tabla_simbolos[num_variables].nombre, nombre);
    tabla_simbolos[num_variables].tipo = tipo;
    num_variables++;
    return 1; // Éxito
}

// Implementación de buscar_variable
TipoDato buscar_variable(const char *nombre)
{
    for (int i = 0; i < num_variables; i++)
    {
        if (strcmp(tabla_simbolos[i].nombre, nombre) == 0)
        {
            return tabla_simbolos[i].tipo;
        }
    }
    return TIPO_DESCONOCIDO; // No encontrada
}

// Implementación de clasificar_token
TipoDato clasificar_token(const char *token)
{
    if (strcmp(token, "entero") == 0 || strcmp(token, "decimal") == 0 || strcmp(token, "cadena") == 0 ||
        strcmp(token, "si") == 0 || strcmp(token, "mientras") == 0 || strcmp(token, "retorno") == 0)
    {
        return PALABRA_CLAVE;
    }

    // Verificar si es un número entero
    int es_entero = 1;
    for (int i = 0; token[i] != '\0'; i++)
    {
        if (!isdigit(token[i]))
        {
            es_entero = 0;
            break;
        }
    }
    if (es_entero)
        return ENTERO;

    // Verificar si es un número decimal
    int tiene_punto = 0, es_decimal = 1;
    for (int i = 0; token[i] != '\0'; i++)
    {
        if (token[i] == '.')
        {
            if (tiene_punto)
            {
                es_decimal = 0;
                break;
            }
            tiene_punto = 1;
        }
        else if (!isdigit(token[i]))
        {
            es_decimal = 0;
            break;
        }
    }
    if (es_decimal && tiene_punto)
        return DECIMAL;

    // Verificar si es una cadena
    if (token[0] == '"' && token[strlen(token) - 1] == '"')
        return CADENA;

    // Verificar si es un operador
    if (strchr("+-*/=<>", token[0]) && strlen(token) == 1)
        return OPERADOR;

    // Verificar si es un delimitador
    if (strchr("();{}", token[0]) && strlen(token) == 1)
        return DELIMITADOR;

    // Por defecto, considerar identificador
    return IDENTIFICADOR;
}

// Implementación de verificar_asignacion
int verificar_asignacion(const char *nombre, TipoDato tipo)
{
    TipoDato tipo_variable = buscar_variable(nombre);
    if (tipo_variable == TIPO_DESCONOCIDO)
    {
        printf("Error semántico: La variable '%s' no está declarada.\n", nombre);
        return 0;
    }

    if (tipo_variable != tipo)
    {
        printf("Error semántico: Tipo incompatible en la asignación a '%s'.\n", nombre);
        return 0;
    }

    return 1;
}

// Implementación de validar_condicion
int validar_condicion(const char *variable)
{
    TipoDato tipo_variable = buscar_variable(variable);
    if (tipo_variable == TIPO_DESCONOCIDO)
    {
        printf("Error semántico: La variable '%s' no está declarada en la condición.\n", variable);
        return 0;
    }

    if (tipo_variable != ENTERO && tipo_variable != DECIMAL)
    {
        printf("Error semántico: La variable '%s' no puede ser utilizada en la condición.\n", variable);
        return 0;
    }

    return 1;
}

// Implementación de procesar_linea
void procesar_linea(const char *linea)
{
    char token[MAX_TOKEN_LENGTH];
    int i = 0, j = 0;
    TipoDato ultimo_tipo = TIPO_DESCONOCIDO;
    char ultima_variable[MAX_TOKEN_LENGTH] = {0};
    char ultima_palabra_clave[MAX_TOKEN_LENGTH] = {0};
    char ultima_variable_asignacion[MAX_TOKEN_LENGTH] = {0};
    int en_condicion = 0;
    int esperando_valor = 0;

    while (linea[i] != '\0')
    {
        // Ignorar espacios en blanco
        if (isspace(linea[i]))
        {
            if (j > 0)
            {
                token[j] = '\0';
                TipoDato tipo = clasificar_token(token);
                printf("Token: %s, Tipo: %d\n", token, tipo);

                // Manejar declaraciones
                if (tipo == PALABRA_CLAVE)
                {
                    strcpy(ultima_palabra_clave, token);
                    if (strcmp(token, "si") == 0)
                    {
                        en_condicion = 1;
                    }
                }
                else if (tipo == IDENTIFICADOR)
                {
                    if (strcmp(ultima_palabra_clave, "entero") == 0 ||
                        strcmp(ultima_palabra_clave, "decimal") == 0 ||
                        strcmp(ultima_palabra_clave, "cadena") == 0)
                    {

                        TipoDato tipo_var = TIPO_DESCONOCIDO;
                        if (strcmp(ultima_palabra_clave, "entero") == 0)
                            tipo_var = ENTERO;
                        else if (strcmp(ultima_palabra_clave, "decimal") == 0)
                            tipo_var = DECIMAL;
                        else if (strcmp(ultima_palabra_clave, "cadena") == 0)
                            tipo_var = CADENA;

                        agregar_variable(token, tipo_var);
                        strcpy(ultima_variable_asignacion, token);
                        ultimo_tipo = tipo_var;
                    }
                    else
                    {
                        TipoDato tipo_var = buscar_variable(token);
                        if (tipo_var == TIPO_DESCONOCIDO)
                        {
                            printf("Error semántico: Variable '%s' no declarada.\n", token);
                        }
                        if (esperando_valor)
                        {
                            if (tipo_var != ultimo_tipo)
                            {
                                printf("Error semántico: Tipo incompatible en asignación.\n");
                            }
                            esperando_valor = 0;
                        }
                        strcpy(ultima_variable_asignacion, token);
                    }
                }
                else if (tipo == ENTERO || tipo == DECIMAL || tipo == CADENA)
                {
                    if (esperando_valor)
                    {
                        TipoDato tipo_var = buscar_variable(ultima_variable_asignacion);
                        if (tipo_var != tipo)
                        {
                            printf("Error semántico: Tipo incompatible en asignación a '%s'.\n",
                                   ultima_variable_asignacion);
                        }
                        esperando_valor = 0;
                    }
                }

                j = 0;
            }
            i++;
            continue;
        }

        // Manejar tokens especiales
        if (strchr("();{}+-*/=<>", linea[i]))
        {
            if (j > 0)
            {
                token[j] = '\0';
                TipoDato tipo = clasificar_token(token);
                printf("Token: %s, Tipo: %d\n", token, tipo);

                if (tipo == IDENTIFICADOR)
                {
                    strcpy(ultima_variable_asignacion, token);
                    ultimo_tipo = buscar_variable(token);
                }

                j = 0;
            }

            token[0] = linea[i];
            token[1] = '\0';
            TipoDato tipo = clasificar_token(token);
            printf("Token: %s, Tipo: %d\n", token, tipo);

            if (strcmp(token, "(") == 0 && en_condicion)
            {
                // Próximo token debe ser una variable numérica
                char siguiente_token[MAX_TOKEN_LENGTH];
                int k = 0;
                i++;
                while (!isspace(linea[i]) && linea[i] != ')')
                {
                    siguiente_token[k++] = linea[i++];
                }
                siguiente_token[k] = '\0';
                i--;

                TipoDato tipo_var = buscar_variable(siguiente_token);
                if (tipo_var != ENTERO && tipo_var != DECIMAL)
                {
                    printf("Error semántico: La variable '%s' no puede ser utilizada en la condición.\n",
                           siguiente_token);
                }
                en_condicion = 0;
            }

            if (strcmp(token, "=") == 0)
            {
                esperando_valor = 1;
            }

            i++;
            continue;
        }

        token[j++] = linea[i++];
    }

    // Procesar último token
    if (j > 0)
    {
        token[j] = '\0';
        TipoDato tipo = clasificar_token(token);
        printf("Token: %s, Tipo: %d\n", token, tipo);

        if (esperando_valor)
        {
            TipoDato tipo_var = buscar_variable(ultima_variable_asignacion);
            if (tipo_var != tipo && tipo != IDENTIFICADOR)
            {
                printf("Error semántico: Tipo incompatible en asignación a '%s'.\n",
                       ultima_variable_asignacion);
            }
        }
    }
}

// Programa principal
int main()
{
    FILE *archivo = fopen("codigo.txt", "r");
    if (archivo == NULL)
    {
        printf("Error: No se pudo abrir el archivo 'codigo.txt'.\n");
        return 1;
    }

    char linea[MAX_LINE_LENGTH];
    while (fgets(linea, MAX_LINE_LENGTH, archivo))
    {
        procesar_linea(linea);
    }

    fclose(archivo);

    return 0;
}