#include <windows.h>
#include <gl/gl.h>
#include "SOIL.h"
#include <stdio.h>
#define DISTANCIA 0.125
#define CORPO_QUANTIDADE 20

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);
int direcao=1, quantidade=5,quantidade_buffer=4, velocidade=200, atualizar=0, status=0;
float movx=0, movy=0, x[CORPO_QUANTIDADE+5], y[CORPO_QUANTIDADE+5], xmaca=2, ymaca;
char cabeca[100], corpo[100], placar[13]="Placar", cenario[100], diretorio[100], maca[100];
HWND texto, hwnd;

void Pegar_Diretorio();
void Tirar_texto();
void Gerar_Maca_Coordenadas();
void Testar_Colisoes();
void Alterar_Corpo(int quadrado_atual);
void Teclado();
void Atualizar_posicao();
void Desenhar();
void Imprimir_imagem(char nome[], float x, float y, float tamanho);


int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;
    WPARAM hFont;
    sprintf(placar,"Placar %d/%d",quantidade-5,CORPO_QUANTIDADE);
    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(RGB(99, 18, 1));
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;

    Pegar_Diretorio();
    sprintf(cabeca,"%sCobra_Cabeça_Direita.png",diretorio);
    sprintf(corpo,"%sCobra_Corpo.png",diretorio);
    sprintf(cenario,"%sCenario3.png",diretorio);
    sprintf(maca,"%sMaça.png",diretorio);

    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "OpenGL Sample",
                          WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          600,
                          600,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);
    texto=CreateWindow("STATIC",
                        placar,
                        WS_VISIBLE | WS_CHILD | WS_OVERLAPPED,
                        55,25,122,26,
                        hwnd, NULL, NULL, NULL);
                        hFont=CreateFont (22, 0, 0, 0,
                                        FW_DONTCARE,
                                        FALSE, FALSE, FALSE,
                                        ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                                        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                                        DEFAULT_PITCH | FF_SWISS, L"Arial");
                SendMessage (texto, WM_SETFONT, hFont, FALSE);
    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            /* OpenGL animation code goes here */
            if(status==0)
            {
            if(quantidade!=quantidade_buffer)
            {
                sprintf(placar,"Placar %i/%i",quantidade-5,CORPO_QUANTIDADE);
                texto=CreateWindow("STATIC",
                                    placar,
                                    WS_VISIBLE | WS_CHILD,
                                    55,25,122,26,
                                    hwnd, NULL, NULL, NULL);
                SendMessage (texto, WM_SETFONT, hFont, FALSE);
                quantidade_buffer=quantidade;
            }

            }
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glPushMatrix();
            if(xmaca==2)
            {
                Gerar_Maca_Coordenadas();
            }
            Teclado();
            if(atualizar==0)
            {
                Atualizar_posicao();
            }

            Desenhar();
            atualizar=0;
            if(quantidade==CORPO_QUANTIDADE+5)
            {
                status=1;
                sprintf(cenario,"%sWIN.png",diretorio);
            }
            if(status==0)
            {
                Testar_Colisoes();
            }
            glPopMatrix();

            SwapBuffers(hDC);
            glFlush();
            Sleep (velocidade);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
        break;

        case WM_CTLCOLORSTATIC:
                SetTextColor((HDC)wParam,RGB(0,255,0));
                SetBkMode((HDC)wParam,TRANSPARENT);
        return GetClassLong(hwnd, GCL_HBRBACKGROUND);

        case WM_DESTROY:
            return 0;

        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                break;
            }
        }
        break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}
void Desenhar()
{
    int quadrado_atual;

    x[0]=movx;
    y[0]=movy;



    Imprimir_imagem(cenario,0,0,1);
    if(status==0)
    {
    for(quadrado_atual=quantidade;quadrado_atual>=1;quadrado_atual--)
    {
        x[quadrado_atual]=x[quadrado_atual-1];
        y[quadrado_atual]=y[quadrado_atual-1];
    }
    for(quadrado_atual=2;quadrado_atual<=quantidade;quadrado_atual++)
    {
        Alterar_Corpo(quadrado_atual);

        Imprimir_imagem(corpo,x[quadrado_atual],y[quadrado_atual],0.25);
    }
    Imprimir_imagem(cabeca,x[0],y[0],0.16);
    Imprimir_imagem(maca,xmaca,ymaca,0.25);


    }

}

void Imprimir_imagem(char nome[], float x, float y, float tamanho)
{
    GLuint tex_2d = SOIL_load_OGL_texture
            (
            nome,
            SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID,
            SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB
            );
            if( 0 == tex_2d )
            {
                printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
                printf("%s\n",nome);
            }
            glColor3f(1.0, 1.0, 1.0);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, tex_2d);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                    glBegin(GL_QUADS);
                        glTexCoord2f(1.0f,0.0f); glVertex2f(x+tamanho, y+tamanho);
                        glTexCoord2f(1.0f,1.0f); glVertex2f(x+tamanho, y-tamanho);
                        glTexCoord2f(0.0f,1.0f); glVertex2f(x-tamanho, y-tamanho);
                        glTexCoord2f(0.0f,0.0f); glVertex2f(x-tamanho, y+tamanho);
                    glEnd();

            glDisable(GL_BLEND);
            glDisable(GL_TEXTURE_2D);

}


void Atualizar_posicao()
{
    switch(direcao)
    {
        case 0:
            movy+=DISTANCIA;
        break;
        case 1:
            movx+=DISTANCIA;
        break;
        case 2:
            movx-=DISTANCIA;
        break;
        case 3:
            movy-=DISTANCIA;
        break;
    }


}

void Teclado()
{
    if(atualizar==0)
    {
        if(GetAsyncKeyState(VK_UP))
        {
            if(direcao!=0)
            {
                sprintf(cabeca,"%sCobra_Cabeça_Baixo.png",diretorio);
                direcao=0;
                movy+=DISTANCIA;
                atualizar=1;
                return;

            }
        }
        else
        if(GetAsyncKeyState(VK_RIGHT))
        {
            if(direcao!=1)
            {
                sprintf(cabeca,"%sCobra_Cabeça_Direita.png",diretorio);
                direcao=1;
                movx+=DISTANCIA;
                atualizar=1;
                return;

            }
        }
        else
        if(GetAsyncKeyState(VK_LEFT))
        {
            if(direcao!=2)
            {
                sprintf(cabeca,"%sCobra_Cabeça_Esquerda.png",diretorio);
                direcao=2;
                movx-=DISTANCIA;
                atualizar=1;
                return;
            }
        }
        else
        if(GetAsyncKeyState(VK_DOWN))
        {
            if(direcao!=3)
            {
                sprintf(cabeca,"%sCobra_Cabeça_Cima.png",diretorio);
                direcao=3;
                movy-=DISTANCIA;
                atualizar=1;
                return;
            }
        }

    }

    //Desenhar();
}

void Alterar_Corpo(int quadrado_atual)
{
    if(quadrado_atual!=quantidade)
    {
        if(((x[quadrado_atual-1]==x[quadrado_atual]+DISTANCIA)&&(x[quadrado_atual+1]==x[quadrado_atual]-DISTANCIA))||((x[quadrado_atual+1]==x[quadrado_atual]+DISTANCIA)&&(x[quadrado_atual-1]==x[quadrado_atual]-DISTANCIA)))
        {
            sprintf(corpo,"%sCobra_Corpo.png",diretorio);
        }
        else
        if(((y[quadrado_atual-1]==y[quadrado_atual]+DISTANCIA)&&(y[quadrado_atual+1]==y[quadrado_atual]-DISTANCIA))||((y[quadrado_atual+1]==y[quadrado_atual]+DISTANCIA)&&(y[quadrado_atual-1]==y[quadrado_atual]-DISTANCIA)))
        {
            sprintf(corpo,"%sCobra_Corpo2.png",diretorio);
        }
        else
        if(((x[quadrado_atual-1]==x[quadrado_atual]+DISTANCIA)&&(y[quadrado_atual+1]==y[quadrado_atual]-DISTANCIA))||((x[quadrado_atual+1]==x[quadrado_atual]+DISTANCIA)&&(y[quadrado_atual-1]==y[quadrado_atual]-DISTANCIA)))
        {
            sprintf(corpo,"%sCobra_Corpo_Dobra2.png",diretorio);
        }
        else
        if(((x[quadrado_atual-1]==x[quadrado_atual]+DISTANCIA)&&(y[quadrado_atual+1]==y[quadrado_atual]+DISTANCIA))||((x[quadrado_atual+1]==x[quadrado_atual]+DISTANCIA)&&(y[quadrado_atual-1]==y[quadrado_atual]+DISTANCIA)))
        {
            sprintf(corpo,"%sCobra_Corpo_Dobra.png",diretorio);
        }
        else
        if(((x[quadrado_atual-1]==x[quadrado_atual]-DISTANCIA)&&(y[quadrado_atual+1]==y[quadrado_atual]+DISTANCIA))||((x[quadrado_atual+1]==x[quadrado_atual]-DISTANCIA)&&(y[quadrado_atual-1]==y[quadrado_atual]+DISTANCIA)))
        {
            sprintf(corpo,"%sCobra_Corpo_Dobra4.png",diretorio);
        }
        else
        if(((x[quadrado_atual-1]==x[quadrado_atual]-DISTANCIA)&&(y[quadrado_atual+1]==y[quadrado_atual]-DISTANCIA))||((x[quadrado_atual+1]==x[quadrado_atual]-DISTANCIA)&&(y[quadrado_atual-1]==y[quadrado_atual]-DISTANCIA)))
        {
            sprintf(corpo,"%sCobra_Corpo_Dobra3.png",diretorio);
        }

    }
    else
    if(quadrado_atual==quantidade)
    {
        if(x[quadrado_atual-1]==x[quadrado_atual]+DISTANCIA)
        {
            sprintf(corpo,"%sCobra_Rabo.png",diretorio);
        }
        else
        if(x[quadrado_atual-1]==x[quadrado_atual]-DISTANCIA)
        {
            sprintf(corpo,"%sCobra_Rabo3.png",diretorio);
        }
        else
        if(y[quadrado_atual-1]==y[quadrado_atual]+DISTANCIA)
        {
            sprintf(corpo,"%sCobra_Rabo4.png",diretorio);
        }
        else
        if(y[quadrado_atual-1]==y[quadrado_atual]-DISTANCIA)
        {
            sprintf(corpo,"%sCobra_Rabo2.png",diretorio);
        }
    }
}
void Testar_Colisoes()
{
    int quadrado_atual;
    //Borda
    if((x[0]==0.875)||(x[0]==-0.875)||(y[0]==0.875)||(y[0]==-0.875))
    {
        status=1;
        sprintf(cenario,"%sGAMEOVER.png",diretorio);
    }
    //Maçã
    if((xmaca==x[0])&&(ymaca==y[0]))
    {
        Gerar_Maca_Coordenadas();
        quantidade++;
        if(velocidade>80)
        {
            velocidade-=10;
        }
    }
    //Corpo
    for(quadrado_atual=2;quadrado_atual<=quantidade;quadrado_atual++)
    {
        if((x[quadrado_atual]==x[0])&&(y[quadrado_atual]==y[0]))
        {
            status=1;
            sprintf(cenario,"%sGAMEOVER.png",diretorio);
            printf("%s\n",cenario);
        }
    }
}
void Gerar_Maca_Coordenadas()
{
    int quadrado_atual;
    inicio:
    srand(time(NULL));
    xmaca= rand()%13-6;
    ymaca= rand()%13-6;
    xmaca=xmaca*DISTANCIA;
    ymaca=ymaca*DISTANCIA;
    for(quadrado_atual=0;quadrado_atual<=quantidade;quadrado_atual++)
    {
        if((xmaca==x[quadrado_atual])&&(ymaca==y[quadrado_atual]))
        {
            goto inicio;
        }
    }

}
void Pegar_Diretorio()
{
    FILE *fp;
    char* filename = ".//Snake V3//diretorio.txt";

    fp = fopen(filename, "r");
    if (fp == NULL){
         fp = fopen("diretorio.txt", "r");
        if (fp == NULL){
            printf("Could not open file %s",filename);
            return 1;
        }
    }
    while (fgets(diretorio, MAXCHAR, fp) != NULL)
    fclose(fp);
}
