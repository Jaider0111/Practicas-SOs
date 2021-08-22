#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#define PERMISSIONS 0666

int main(){
   //tuberia para el envio de los datos de consulta
    char *pipea = "/usr/pipea";
    //tuberia para recibir el resultado de la consulta
    char *pipeb = "/usr/pipeb";
    
    //pw y pr descriptores de archivo para las 2 tuberias anteriores
    int pw, pr, r, option;
    int kb = 1024;
    //tamaños
    int sizes[] = {kb, 10*kb, 100*kb, kb*kb, 10*kb*kb, 100*kb*kb};
    for(int j = 0; j < 6; j++){
        //Determina el tamaño de los datos a transmitir
        int size = sizes[j];
        char *msg = (char *)malloc(sizeof(char) * size);
        //llenado del mensaje
        for(int i = 0; i < size; i++){
            msg[i] = '1';
        }
        //Variables para contar el tiempo
        struct timespec begin, end; 
        //numero de veces que se repitira la transmicion
        int reps = (size <= 1000000) ? 10 : 5;
        //variable para la suma de los tiempos
        double total_time = 0;
        long seconds, nanoseconds;
        //Apuntador al archivo
        FILE *file;
        char c[2];
        for(int i = 0; i < reps; i++){
            file =fopen("file", "w");
            int r = fwrite(msg, sizeof(char) * size, 1, file);
            //printf("escrito: %d\n", r);
            fclose(file);
            clock_gettime(CLOCK_REALTIME, &end);
            seconds = end.tv_sec - begin.tv_sec;
            nanoseconds = end.tv_nsec - begin.tv_nsec;
            total_time = seconds + nanoseconds*1e-9;
            pw = open(pipea, O_WRONLY);
            write(pw, "r", 1);
            close(pw);
            clock_gettime(CLOCK_REALTIME, &begin);
            pr = open(pipeb, O_RDONLY);
            r = read(pr, c, 2);
            close(pr);
        }
        double prom = total_time / reps;
        if(size < 1000000){
            size /= kb;
            printf("El tiempo promedio para compatir %3dKB es: %.10f segundos\n", size, prom);
        }else{
            size /= kb*kb;
            printf("El tiempo promedio para compatir %3dMB es: %.10f segundos\n", size, prom);
        }
        //Liberacion de la memoria asignada al mensaje
        free(msg);
    }
	return 0;
}