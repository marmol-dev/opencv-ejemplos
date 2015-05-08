#include <stdio.h>
#include <stdlib.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <emmintrin.h>

#include <pthread.h>    /* POSIX Threads */
#include <semaphore.h>  /* Semaphore */


#define NTHREADS 4

typedef struct {
    int filaInicial;
    int columnaInicial;
    IplImage* resultado;
    IplImage* objeto;
    IplImage* fondo;
} DatosProcesamiento;

IplImage* extraerSubzona(IplImage* original, int xInicial, int yInicial, int xFinal, int yFinal){
    CvSize tam = cvSize(xFinal - xInicial, yFinal - yInicial);
    IplImage* res = cvCreateImage(tam, IPL_DEPTH_8U, 4);

    __m128i temp1;

    unsigned char* pOr,
            * pRes;
    int fila,
            columna;

    for (fila = yInicial; fila < yFinal; fila ++){
        pOr = (unsigned char*) original -> imageData + fila * original -> widthStep + xInicial * 4;
        pRes = (unsigned char*) res -> imageData + (fila - yInicial) * res -> widthStep;

        for (columna = xInicial; columna < xFinal; columna+= 4){
            temp1 = _mm_loadu_si128((__m128i *) pOr);
            _mm_storeu_si128((__m128i *) pRes, temp1);
            pOr += 16;
            pRes += 16;
        }
    }

    return res;
}


void animacion_thread(void *ptr) {

    DatosProcesamiento* dp = (DatosProcesamiento*) ptr;

    IplImage* personaje4c = dp ->objeto,
            * escenario4c = dp -> fondo,
            * frame = dp -> resultado;

    //p apunta a un entero que indica el indice de fila a pintar
    int filaIni = dp -> filaInicial,
        filaFin = filaIni + personaje4c -> height,
        columnaIni = dp -> columnaInicial,
        columnaFin = columnaIni + personaje4c -> width;

    int fila, columna;

    uchar* pPersonaje, *pEscenario, *pRes;

    __m128i rPersonaje, rEscenario, temp1, temp2, mascara, fceros = _mm_set1_epi32(0xFF000000), ceros = _mm_set1_epi32(0x00000000);

    for (fila = filaIni; fila < filaFin; fila++){
        pPersonaje = (uchar*) personaje4c -> imageData + personaje4c -> widthStep * (fila - filaIni);
        pEscenario = (uchar*) escenario4c -> imageData + escenario4c -> widthStep * fila + columnaIni * 4;
        pRes = (uchar*) frame -> imageData + frame -> widthStep * fila + columnaIni * 4;

        for (columna = columnaIni; columna < columnaFin; columna += 4){
            rPersonaje = _mm_loadu_si128((__m128i*) pPersonaje);
            rEscenario = _mm_loadu_si128((__m128i*) pEscenario);
            temp1 = _mm_and_si128(rPersonaje, fceros);
            mascara = _mm_cmpeq_epi32(temp1, ceros);

            rPersonaje  = _mm_or_si128(_mm_and_si128(rEscenario, mascara), _mm_andnot_si128(mascara , rPersonaje));

            _mm_storeu_si128((__m128i*) pRes, rPersonaje);
            pPersonaje += 16;
            pRes += 16;
            pEscenario += 16;
        }
    }

}

int main(int argc, char *argv[]) {

    IplImage *personaje4c, *escenario4c, *frame;

    if (argc != 3) {
        printf("Usage: %s personajes4C.png escenario4C.png\n", argv[0]);
        return EXIT_FAILURE;
    }

    // LEE EL PERSONAJE
    IplImage* sprites = cvLoadImage(argv[1], CV_LOAD_IMAGE_UNCHANGED);
    if (!sprites) {
        printf("Error: fichero personaje %s no leido\n", argv[2]);
        return EXIT_FAILURE;
    }
    int xIinicial = 780, yInicial = 908, ancho = 56, alto = 48;
    //a 908, columna 780, con alto 48 filas y de ancho 56 columnas
    personaje4c = extraerSubzona(sprites, xIinicial, yInicial, xIinicial + ancho, yInicial + alto );


    // LEE EL ESCENARIO
    escenario4c = cvLoadImage(argv[2], CV_LOAD_IMAGE_UNCHANGED);
    if (!escenario4c) {
        printf("Error: fichero escenario %s no leido\n", argv[1]);
        return EXIT_FAILURE;
    }

    // muestra las imagenes inicial y final de cuatro canales
    cvNamedWindow("personaje4c", CV_WINDOW_AUTOSIZE);
    cvShowImage("personaje4c", personaje4c);
    cvNamedWindow("escenario4c", CV_WINDOW_AUTOSIZE);
    cvShowImage("escenario4c", escenario4c);
    cvWaitKey(0);

    frame = cvCloneImage(escenario4c); //sin mascara

    pthread_t threads[NTHREADS];

    int posiciones[NTHREADS];
    int i;

    DatosProcesamiento dp[NTHREADS];

    for (i = 0; i < NTHREADS; i++) {
        posiciones[i] = escenario4c->height / NTHREADS*i;

        dp[i].filaInicial = posiciones[i];
        dp[i].columnaInicial = 0;
        dp[i].objeto = personaje4c;
        dp[i].fondo = escenario4c;
        dp[i].resultado = frame;

        printf("\nEl thread %d dibuja personaje en la fila %d", i, posiciones[i]);
        pthread_create(&threads[i], NULL, (void *) &animacion_thread, (void *) &dp[i]);
    }

    for (i = 0; i < NTHREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    cvNamedWindow("frameTHREADS", CV_WINDOW_AUTOSIZE);
    cvShowImage("frameTHREADS", frame);
    cvWaitKey(0);

    return EXIT_SUCCESS;
}