/*
 * File:   plantilla.c
 * Author: AP2014
 *
 * Created on 24 de enero de 2012, 11:03
 * Last update on ?23? de ?enero? de ?2014, 1:01:57
 */

#include <stdio.h>
#include <stdlib.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>

char * nombreVentanaImagen = "escenario";

IplImage * mascaraImagen(IplImage* original){
    IplImage * mascara = cvCreateImage( cvGetSize(original), IPL_DEPTH_8U, 3 );
    unsigned char * pOriginal = (unsigned char*) original -> imageData;
    unsigned char * pMascara = (unsigned char*) mascara -> imageData;
    unsigned char bBackground = (unsigned char) * pOriginal;
    unsigned char gBackground = (unsigned char) *(pOriginal + 1);
    unsigned char rBackground = (unsigned char) *(pOriginal + 1);

    //recorremos la imagen
    int fila, columna;
    for (fila = 0; fila < original -> height; fila ++){
        pOriginal = (unsigned char*) original -> imageData + original -> widthStep * fila;
        pMascara = (unsigned char*) mascara -> imageData + mascara -> widthStep * fila;
        for (columna = 0; columna < original -> width; columna ++){
            //blue
            if (*pOriginal++ == bBackground & *pOriginal++ == gBackground & *pOriginal++ == rBackground){
                *pMascara++ = 0xFF;
                *pMascara++ = 0xFF;
                *pMascara++ = 0xFF;
            } else {
                *pMascara++ = 0x00;
                *pMascara++ = 0x00;
                *pMascara++ = 0x00;
            }
        }
    }

    return mascara;
}

void _mezclaImagenes(IplImage* fondo, IplImage* obj, int posx, int posy, IplImage* resultado){
    if (fondo -> height >= obj -> height && fondo -> width >= obj -> height) {
        //comprobamos que las posiciones de incio sean correctas

        if ( posy <= (fondo -> height - obj -> height) && posx <= fondo -> width - obj -> width ){
            //TODO store mascara in somewhere
            IplImage * mascara = mascaraImagen(obj);

            //return mascara;

            unsigned char * pFondo = (unsigned char*) fondo -> imageData;
            unsigned char * pObj = (unsigned char*) obj -> imageData;
            unsigned char * pRes = (unsigned char*) resultado -> imageData;
            unsigned char * pMas = (unsigned char*) mascara -> imageData;

            //recorremos la imagen
            int filaFondo = posy, columnaFondo = posx, filaObj, columnaObj;

            //copiamos la imagen de obj
            for (filaObj = 0; filaObj < obj -> height; filaObj ++, filaFondo++){

                pRes = (unsigned char*) resultado -> imageData + (resultado -> widthStep * filaFondo) + posx * 3;
                pObj = (unsigned char*) obj -> imageData + obj -> widthStep * filaObj;
                pFondo = (unsigned char*) fondo -> imageData + (fondo -> widthStep * filaFondo) + posx * 3;
                pMas = (unsigned char*) mascara -> imageData + mascara -> widthStep * filaObj;

                for (columnaObj = 0; columnaObj < obj -> width; columnaObj ++, columnaFondo++){
                    *pRes++ = (unsigned char) ((*pFondo) & (*pMas)) | ((*pObj & (~ *pMas)));
                    pFondo++;
                    pMas++;
                    pObj++;
                    *pRes++ = (unsigned char) ((*pFondo) & (*pMas)) | ((*pObj & (~ *pMas)));
                    pFondo++;
                    pMas++;
                    pObj++;
                    *pRes++ = (unsigned char) ((*pFondo) & (*pMas)) | ((*pObj & (~ *pMas)));
                    pFondo++;
                    pMas++;
                    pObj++;
                }
            }


        } else {
            printf("Error en posicion de inicio");
            printf("Algo %i",posx);
        }


    } else {
        printf("Error en el tamaño de la imagen de obj, esta debe ser menor o igual que el fondo");
    }
}

IplImage* mezclaImagenes(IplImage* fondo, IplImage* obj, int posx, int posy){
    IplImage* resultado = cvCloneImage(fondo);
    _mezclaImagenes(fondo, obj, posx, posy, resultado);
    return resultado;
}

void clonarRegion(IplImage* res, IplImage* copia , int xInicial, int yInicial, int xFinal, int yFinal){
    //comprobamos si los puntos están dentro del rango
    if (
        //los límites iniciales son menores que los finales
            xInicial < xFinal &&
                    yInicial < yFinal &&
                    //los limites inciales son menores que los anchos y altos de las imágenes
                    xInicial < copia -> width &&
                    xInicial < res -> width &&
                    yInicial < copia -> height &&
                    yInicial < res -> height &&
                    //los límites inciales son mayores o iguales que 0
                    xInicial >= 0 &&
                    yInicial >= 0 &&
                    //los limites finales son menores o iguales que los anchos y altos de las imágenes
                            xFinal <= copia -> width &&
                    xFinal <= res -> width &&
                    yFinal <= copia -> height &&
                    yFinal <= res -> height &&
                    //los límites finales son mayores que 0
                            xFinal > 0 &&
                    yFinal > 0
            ) {
        unsigned char* pRes;
        unsigned char* pCopia;

        int fila,
                columna;

        for (fila = yInicial; fila < yFinal; fila++){
            pRes = (unsigned char*) res -> imageData + res -> widthStep * fila + xInicial * 3;
            pCopia = (unsigned char*) copia -> imageData + res -> widthStep * fila + xInicial * 3;

            for (columna = xInicial; columna < xFinal; columna ++){
                /**pRes++ = 0xFF;
                *pRes++ = 0xFF;
                *pRes++ = 0xFF;*/
                *pRes++ = *pCopia++;
                *pRes++ = *pCopia++;
                *pRes++ = *pCopia++;
            }
        }
    } else {
        fprintf(stderr, "En clonarRegion : Las coordenadas iniciales y/o finales no son correctas\n");
        exit(EXIT_FAILURE);
    }
}

/**
* Desplaza un obj hacia la derecha
* @param fondo
* @param obj
* @param pips
*/
void desplazar(IplImage* fondo, IplImage* obj, int xInicial, int yInicial, int xFinal, int yFinal, int stepsCount, int veloc){
    //TODO: comprobar valores del rango
    if (
            fondo -> height >= obj -> height &&
                    fondo -> width >= obj -> height &&
                    yInicial <= (fondo -> height) - (obj -> height) &&
                    yInicial >= 0 &&
                    xInicial <= (fondo -> width) - (obj -> width) &&
                    xInicial >= 0 &&
                    yFinal <= (fondo -> height) - (obj -> height) &&
                    yFinal >= 0 &&
                    xFinal <= (fondo -> width) - (obj -> width) &&
                    xFinal >= 0
            ) {

        IplImage * img = cvCloneImage(fondo);

        int     distanciaX = xFinal - xInicial,
                distanciaY = yFinal - yInicial;

        double  posX = (double) xInicial,
                posY = (double) yInicial,
                desplazamientoX = (double) distanciaX / (double) stepsCount,
                desplazamientoY = (double) distanciaY / (double) stepsCount;

        int stepNumber = 0;

        //si el desplazamiento en cada movimiento es mayor que 1 pixel entonces tenemos que pintar la imagen en cada movimiento
        if (desplazamientoX > 1 || desplazamientoY > 1) {
            //calcular mejor la región que se necesita clonar
            for (stepNumber = 0; stepNumber < stepsCount; stepNumber++, posX += desplazamientoX, posY += desplazamientoY){
                _mezclaImagenes(fondo, obj, (int) posX, (int) posY, img);
                cvShowImage(nombreVentanaImagen, img);
                cvWaitKey(veloc);
                clonarRegion(img, fondo, (int) posX, (int) posY, (int) posX + obj -> width, (int) posY + obj -> height);
            }
        } else {
            for (stepNumber = 0; stepNumber < stepsCount; stepNumber++, posX += desplazamientoX, posY += desplazamientoY){
                _mezclaImagenes(fondo, obj, (int) posX, (int) posY, img);
                cvShowImage(nombreVentanaImagen, img);
                cvWaitKey(veloc);
            }
        }


        cvReleaseImage(&img);
    } else {
        printf("Error en el tamaño de las imagenes y/o en los puntos de desplazamiento");
    }
}

int main(int argc, char** argv) {

    if (argc != 3) {
        printf("Usage: %s image1 image2\n", argv[0]);
        return EXIT_FAILURE;
    }

    IplImage* Img1 = cvLoadImage(argv[1], CV_LOAD_IMAGE_UNCHANGED);

    // Always check if the program can find a file
    if (!Img1) {
        printf("Error: fichero %s no leido\n", argv[1]);
        return EXIT_FAILURE;
    }

    IplImage* Img2 = cvLoadImage(argv[2], CV_LOAD_IMAGE_UNCHANGED);

    if (!Img1) {
        printf("Error: fichero %s no leido\n", argv[1]);
        return EXIT_FAILURE;
    }

    cvNamedWindow(nombreVentanaImagen, 1);

    desplazar(Img1, Img2, 0, 0, 200, 100, 500, 2);

    cvWaitKey(0);

    cvDestroyWindow(nombreVentanaImagen);

    return EXIT_SUCCESS;
}