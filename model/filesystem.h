#ifndef STRUCTURES_FILESYSTEM
#define STRUCTURES_FILESYSTEM

#include <iostream>
#include <stdio.h>

int _max_bloques_por_inodo = 4380;
int _capacidad_carpeta = 17518;
int _capacidad_archivo = 280320;
std::string _carpeta_raiz = "/";
std::string _users_path = _carpeta_raiz + "users.txt";

struct Superbloque
{
    int s_filesystem_type;   // Guarda el número que identifica el sistema de archivos utilizado
    int s_inodes_count;      // Guarda el número total de inodos
    int s_blocks_count;      // Guarda el número total de bloques
    int s_free_blocks_count; // Contiene el número de bloques libres
    int s_free_inodes_count; // Contiene el número de inodos libres
    time_t s_mtime;          // Última fecha en el que el sistema fue montado
    time_t s_umtime;         // Última fecha en que el sistema fue desmontado
    int s_mnt_count;         // Indica cuantas veces se ha montado el sistema
    int s_magic;             // Valor que identifica al sistema de archivos, tendrá el valor 0xEF53
    int s_inode_size;        // Tamaño del inodo
    int s_block_size;        // Tamaño del bloque
    int s_first_ino;         // Primer inodo libre
    int s_first_blo;         // Primer bloque libre
    int s_bm_inode_start;    // Guardará el inicio del bitmap de inodos
    int s_bm_block_start;    // Guardará el inicio del bitmap de bloques
    int s_inode_start;       // Guardará el inicio de la tabla de inodos
    int s_block_start;       // Guardará el inicio de la tabla de bloques
};

struct InodosTable
{
    int i_uid;       // UID del usuario propietario del archivo o carpeta
    int i_gid;       // GID del grupo al que pertenece el archivo o carpeta
    int i_size;      // Tamaño del archivo en bytes
    time_t i_atime;  // Última fecha en que se leyó el inodo sin modificarlo
    time_t i_ctime;  // Fecha en la que se creó el inodo
    time_t i_mtime;  // Úlitma fecha en la que se modificó el inodo
    int i_block[15]; /* Array en los que los primeros 12 registros son bloques directos. 
                        El 13 será el número del bloque simple indirecto.
                        El 14 será el número del bloque doble indirecto.
                        El 15 será el número del bloque triple indirecto.
                        Si no son utilizados tendrá el valor -1 */
    char i_type;     // Indica si es archivo o carpeta. 1=archivo, 0=carpeta
    int i_perm;      // Guardará los permisos del archivo o carpeta a nivel de bits
};

struct content
{
    char b_name[12]; // Nombre de la carpeta o archivo
    int b_inodo;     // Apuntador hacia un inodo asociado al archivo o carpeta
};

struct CarpetasBlock
{
    content b_content[4]; // Array con el contenido de la carpeta
};

struct ArchivosBlock
{
    char b_content[64]; // Array con el contenido del archivo
};

struct ApuntadoresBlock
{
    int b_pointers[16]; // Array con los apuntadores hacia bloques (de archivo o carpeta)
};

struct Journaling
{
    char type_operation[10] = "-";
    char type = '-';
    char path[40] = "-";
    char content[137] = "-";
    time_t date;
    int size = 0;
};

#endif