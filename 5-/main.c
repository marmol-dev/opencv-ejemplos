#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <emmintrin.h>
#include <stdio.h>
#include <stdlib.h>

char * NOMBRE_VENTANA = "Sprite";

IplImage* mezclarImagenesSSE(IplImage* background, IplImage* obj, int x, int y){
    IplImage* res = cvCloneImage(background);



    return res;
}

IplImage* extraerSubzona(IplImage* original, int xInicial, int yInicial, int xFinal, int yFinal){
    CvSize tam = cvSize(xFinal - xInicial, yFinal - yInicial);
    IplImage* res = cvCreateImage(tam, IPL_DEPTH_8U, 4);

    __m128i reg;

    unsigned char* pOr,
                * pRes;
    int fila,
        columna;

    for (fila = yInicial; fila < yFinal; fila ++){
        pOr = (unsigned char*) original -> imageData + fila * original -> widthStep + xInicial * 4;
        pRes = (unsigned char*) res -> imageData + (fila - yInicial) * res -> widthStep;

        for (columna = xInicial; columna < xFinal; columna+= 4){
            reg = _mm_loadu_si128((__m128i *) pOr);
            _mm_store_si128((__m128i *) pRes, reg);
            pOr += 16;
            pRes += 16;
        }
    }

    return res;
}

int main(int argc, char** argv){
    if (argc != 3){
        printf("Error nº argumentos");
        exit(EXIT_FAILURE);
    }

    IplImage* background = cvLoadImage(argv[1], CV_LOAD_IMAGE_UNCHANGED);
    if (!background){
        printf("Error cargando background");
        exit(EXIT_FAILURE);
    }

    IplImage* sprite = cvLoadImage(argv[2], CV_LOAD_IMAGE_UNCHANGED);
    if (!sprite){
        printf("Error cargando sprite");
        exit(EXIT_FAILURE);
    }

    cvNamedWindow(NOMBRE_VENTANA, 1);

    IplImage* porcion = extraerSubzona(sprite, 780, 908, 780 + 56, 908 + 56);
    cvShowImage(NOMBRE_VENTANA, porcion);
    cvWaitKey(0);

    cvReleaseImage(&background);
    cvReleaseImage(&sprite);
    cvDestroyWindow(NOMBRE_VENTANA);

}