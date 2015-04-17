#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <emmintrin.h>

char * WINDOW_NAME = "Imagen de Dubai";

void oscurecer(IplImage* img){
    int fila,
        columna;

    unsigned char* pImg;
    int componente;

    for (fila = 0; fila < img -> height; fila ++){
        pImg = (unsigned char*) img -> imageData + img -> widthStep * fila;
        for (columna = 0; columna < img -> width; columna++){
            for (componente = 0; componente < 3; componente++){
                pImg[componente] = pImg[componente] == 0 ? 0 : pImg[componente] -1;
            }
            pImg+= 3;
        }
    }
}

void aclarar(IplImage* img, IplImage* final){
    int fila,
            columna;

    unsigned char* pImg,
        * pFinal;
    int componente;

    for (fila = 0; fila < img -> height; fila ++){
        pImg = (unsigned char*) img -> imageData + img -> widthStep * fila;
        pFinal = (unsigned char*) final -> imageData + final -> widthStep * fila;

        for (columna = 0; columna < img -> width; columna++){
            for (componente = 0; componente < 3; componente++){
                pImg[componente] = pImg[componente] == pFinal[componente] ? pImg[componente] : pImg[componente]  + 1;
            }
            pImg+= 3;
            pFinal+= 3;
        }
    }
}

void mezclar(IplImage* img1, unsigned char opacidad, IplImage* img2, IplImage* res){
    int fila,
            columna;

    unsigned char* pImg1,
            * pImg2,
            * pRes;
    int componente;

    for (fila = 0; fila < img1 -> height; fila ++){
        pImg1 = (unsigned char*) img1 -> imageData + img1 -> widthStep * fila;
        pImg2 = (unsigned char*) img2 -> imageData + img2 -> widthStep * fila;
        pRes = (unsigned char*) res -> imageData + res -> widthStep * fila;

        for (columna = 0; columna < img1 -> width; columna++){
            for (componente = 0; componente < 3; componente++){
                pRes[componente] = (unsigned char) ((float) pImg1[componente] * ((float) opacidad / 100.0) +
                        (1 - ((float) opacidad / 100.0)) * (float) pImg2[componente] );
            }
            pImg1+= 3;
            pImg2+= 3;
            pRes += 3;
        }
    }
}

void animar(IplImage* img1, IplImage* img2, int esperaEntreFrames){
    IplImage* animacion = cvCloneImage(img1);
    cvShowImage(WINDOW_NAME, animacion);
    unsigned char opacidad;
    for (opacidad = 100; opacidad > 0; opacidad --){
        mezclar(img1, opacidad, img2, animacion);
        cvShowImage(WINDOW_NAME, animacion);
        cvWaitKey(esperaEntreFrames);
    }

    /*for (opacidad = 0x00; opacidad < 0xFF; opacidad ++){
        aclarar(animacion, img2);
        cvShowImage(WINDOW_NAME, animacion);
        cvWaitKey(esperaEntreFrames);
    }*/
}

int main(int argc, char ** argv){
    IplImage* img = cvLoadImage("static/dubai.jpg", CV_LOAD_IMAGE_UNCHANGED);
    IplImage* img2 = cvLoadImage("static/dubai2.jpg", CV_LOAD_IMAGE_UNCHANGED);
    cvNamedWindow(WINDOW_NAME, 1);
    animar(img, img2, 7);
    cvWaitKey(0);
    cvReleaseImage(&img);
    cvDestroyWindow(WINDOW_NAME);
    return EXIT_SUCCESS;
}