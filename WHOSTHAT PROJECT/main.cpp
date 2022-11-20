#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <wchar.h>
#include <dirent.h>
#include "menu.h"
#define _CRT_SECURE_NO_WARNINGS


#define SUBMIT_BUTTON 111
#define HELP_BUTTON 222
#define SKIP_BUTTON 333
#define ROUNDS 7


LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
typedef void (__cdecl *GEN_RAN_ARR)(int*, int, int);
typedef int (__cdecl *BEST_SCORE)(char *);

void LoadImages(LPCSTR);
void LoadBar(int points);
void AddControls(HWND);
void GenerateRandomArray();
void List_Files (char* directory_name);
void Select_Randoms();
void ShowScore(HWND hWnd);
void DisplayScore(HWND hWnd, char* ds);
void SetHelps(char* celname, int celindex, int cellength);     //takes as input celebrity name and index of the question
void SetScore();
void Update_Scores_File(char *final_score, HWND hWnd);
char * getrealstring(char *);


int punteggio = 0;
int database_size;    //global variable that indicates range from where to extract random numbers, which is the number of celebrities in the database
int roundcounter=0;
int guessedcounter=0;
int skippedcounter=0;
float factor = 430/(float(ROUNDS)*10);

char strpun[12]; //put punteggio in string here
char stracr[8]; //put actualround in string here
char strtot[8]; //put totalrounds in string here
char strguc[8]; //put guessedcelebrities in string here
char strskc[8]; //put skippedcelebrities in string here
char celebname[40];     //array of charachetrs: will contain typed text



char** bitmapsnames;        //list of celebrity names with bitmap extension
char** celebritynames;      //list of celebrity names
char** gamecelebrities;
char** gamebitmaps;

int * randomarray;

HWND hImage, hCelebName, hBar, hLogo;  //hImage is handler for the control which displays images
//HMENU hBut, hHelpBut;
HBITMAP hCelebImage, hLogoImage;   //handler for images
HWND hScore;
HWND hDispScore;
HWND hDisplayHelp;
HWND hSkipBut;
HWND hActualRound, hTotalRounds;
HWND hGuessedCelebrities, hSkippedCelebrities;
HWND hAcRound, hTotRounds;
HWND hGuessed, hSkipped;
HBITMAP hBarImage;

char* filename = "WHOSTHATScores.txt";

LPCWSTR sub = L"submit";
LPCWSTR askhelp = L"Help me";

typedef struct HELP {

    int used=0;         //0 = not used, 1 = used
    char *helpstring;

} HELP;


HELP helps[ROUNDS];





int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow)
{
    WNDCLASSW wc = {0};

    wc.hbrBackground = CreateSolidBrush(RGB(100, 150, 150));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hInstance = hInst;
    wc.lpszClassName = L"myWindowClass";
    wc.lpfnWndProc = WindowProcedure;


    List_Files("\celebritiesbmp");
    gamebitmaps = (char**) malloc(database_size * sizeof(char *));
    gamecelebrities = (char**) malloc(database_size * sizeof(char *));
    randomarray = (int*) malloc(ROUNDS * sizeof(int));
    GenerateRandomArray();
    Select_Randoms();

    if(!RegisterClassW(&wc))
            return -1;

    CreateWindowW(L"myWindowClass", L"My Window", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 1000, 500, NULL, (HMENU) LoadMenu(NULL, MAKEINTRESOURCE(IDR_MYMENU)), NULL, NULL );
    MSG msg = {0};

    while ( GetMessage(&msg,NULL,NULL,NULL))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

     return 0;
}



LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)

{

    switch(msg)
    {

        case WM_CREATE:
            {
            LoadImages(gamebitmaps[0]);       //add images before menus, this line works
            //LoadBar(punteggio);
            //printf("this is the window handler %d \n", hWnd); //THIS LINE DOESN'T WORK: PROBLEM IS IN hWnd
            AddControls(hWnd);      //the previous line was added for debug; I found out hWnd was the problem with this line
            ShowScore(hWnd);
            DisplayScore(hWnd, "0");
            SetScore();
            break;
            }

        case WM_COMMAND:

            switch(wp) {


                case(SUBMIT_BUTTON):        //if button is pressed
                {
                    //printf("button pressed \n");                    //print for debug
                    GetWindowText(hCelebName, celebname, 40);   //get typed text and place it in celebname
                    //printf("your answer is: %s, is it right or wrong?", celebname);     //print for debug
                    if ((strcasecmp(celebname, gamecelebrities[roundcounter])==0) && (roundcounter < ROUNDS)){     //compare typed text (in celebname) to name of the celebrity;
                            //if typed name is correct
                            //printf("\n correct answer \n");     //print for debug

                            punteggio += 10;
                            roundcounter++;
                            guessedcounter++;

                            DestroyWindow(hDisplayHelp);

                            if (roundcounter<ROUNDS){
                                LoadImages(gamebitmaps[roundcounter]);      //load image of next celebrity
                                SendMessageW(hImage, STM_SETIMAGE, (WPARAM) IMAGE_BITMAP, (LPARAM) hCelebImage);
                                //UpdateWindow(hImage);
                            }

                            SetScore();
                            SetWindowTextA(hCelebName,"");

                        if(roundcounter>=ROUNDS){

                            LoadBar(punteggio);
                            SendMessageW(hBar, STM_SETIMAGE, (WPARAM) IMAGE_BITMAP, (LPARAM) hBarImage);
                            //UpdateWindow(hBar);
                            //ShowScore(hWnd);

                            sprintf(strpun, "%d", punteggio);
                            SetWindowTextA(hDispScore, strpun);
                            //DisplayScore(hWnd, strpun);
                            //UpdateWindow(hScore);
                            //UpdateWindow(hDispScore);
                            //UpdateWindow(hWnd);
                            SetWindowTextA(hCelebName,"");

                            char finalscore[sizeof(int)+1];
                            sprintf(finalscore, "%d", punteggio);
                            Update_Scores_File(finalscore, hWnd);
                            DestroyWindow(hWnd);

                        }

                    }

                    else if (strcasecmp(celebname, gamecelebrities[roundcounter])!=0 && roundcounter < ROUNDS){
                        //if typed name is incorrect
                        MessageBeep(MB_OK);
                        punteggio -= 5;
                    }

                    LoadBar(punteggio);
                    SendMessageW(hBar, STM_SETIMAGE, (WPARAM) IMAGE_BITMAP, (LPARAM) hBarImage);
                    //UpdateWindow(hBar);
                    //ShowScore(hWnd);


                    sprintf(strpun, "%d", punteggio);
                    SetWindowTextA(hDispScore, strpun);
                    //DisplayScore(hWnd, strpun);
                    //UpdateWindow(hScore);
                    //UpdateWindow(hDispScore);
                    //UpdateWindow(hWnd);

                    break;

                }



                case (HELP_BUTTON):
                {
                    //displays the help
                    if(helps[roundcounter].used==0){
                        hDisplayHelp = CreateWindowA("Static", helps[roundcounter].helpstring , WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER , 200, 290, 200, 30, hWnd, NULL, NULL, NULL );
                        LOGFONT logfont2;
                        ZeroMemory(&logfont2, sizeof(LOGFONT));
                        logfont2.lfCharSet = DEFAULT_CHARSET;
                        logfont2.lfHeight = -20;
                        HFONT hFont2 = CreateFontIndirect(&logfont2);
                        SendMessage(hDisplayHelp, WM_SETFONT, (WPARAM)hFont2, TRUE);
                        punteggio-= ROUNDS;
                        sprintf(strpun, "%d", punteggio);
                        SetWindowTextA(hDispScore, strpun);
                        DisplayScore(hWnd, strpun);
                        //UpdateWindow(hScore);
                        LoadBar(punteggio);
                        SendMessageW(hBar, STM_SETIMAGE, (WPARAM) IMAGE_BITMAP, (LPARAM) hBarImage);

                        /*UpdateWindow(hBar);
                        UpdateWindow(hDispScore);
                        UpdateWindow(hWnd);
                        UpdateWindow(hWnd);*/
                        SetScore();
                        helps[roundcounter].used=1;
                    }

                break;
                }

                case (SKIP_BUTTON):

                {
                    punteggio-=20;

                    roundcounter++;
                    skippedcounter++;
                    SetScore();

                    DestroyWindow(hDisplayHelp);
                    //UpdateWindow(hDisplayHelp);

                    LoadBar(punteggio);
                    SendMessageW(hBar, STM_SETIMAGE, (WPARAM) IMAGE_BITMAP, (LPARAM) hBarImage);
                    //UpdateWindow(hBar);
                        //ShowScore(hWnd);

                    sprintf(strpun, "%d", punteggio);
                    SetWindowTextA(hDispScore, strpun);
                    DisplayScore(hWnd, strpun);
                    //UpdateWindow(hScore);
                    //UpdateWindow(hDispScore);
                    //UpdateWindow(hWnd);

                    if (roundcounter<ROUNDS){
                        LoadImages(gamebitmaps[roundcounter]);      //load image of next celebrity
                        SendMessageW(hImage, STM_SETIMAGE, (WPARAM) IMAGE_BITMAP, (LPARAM) hCelebImage);
                        //UpdateWindow(hImage);
                    }

                    else {

                        char finalscore[sizeof(int)+1];
                        sprintf(finalscore, "%d", punteggio);
                        Update_Scores_File(finalscore, hWnd);
                        DestroyWindow(hWnd);
                    }

                    SetWindowTextA(hCelebName,"");

                }


                break;


                case ID_FILE_EXIT:
                    {
                        PostQuitMessage(0);
                    }
                break;

                case ID_CLEAR_HISTORY:
                    {
                        int msgboxid = MessageBoxA(hWnd, "By pressing ok, you will delete all the records.\nPress OK to confirm or CANCEL to avoid that.", "Are you sure?", MB_OKCANCEL);
                        if (msgboxid==1){
                            int rem = remove(filename);
                        }
                    }
                break;

                case ID_HELP:
                    {
                        MessageBoxA(hWnd, "Welcome to WHOSTHAT, a game by Alessio Cremonesi.\nThe goal of the game is to guess the displayed celebrity's full name.\nYou don't have to care about lowercase/uppercase.\nGuessing a celebrity's name will give you 10 points.\nBy asking for help, you'll lose a number of points equal to the number of rounds.\nBy skipping a celebrity, you'll lose twice points.","WHOSTHAT Guide",MB_OK);
                    }
                break;

                case ID_CREDITS:
                    {
                        MessageBox(hWnd, "Game Designer: Alessio Cremonesi\nGame Developer: Alessio Cremonesi\nLogo Designer: Riccardo Murtas", "Credits", MB_OK);
                    }
                break;

            }

            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;



        default:
            return DefWindowProcW(hWnd,msg,wp,lp);
    }
}



void AddControls(HWND hWnd){

    //create hCelebName, window for typing text
    hCelebName = CreateWindow("Edit", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL , 100, 140, 250, 25, hWnd, NULL, NULL, NULL );
    //create hImage, window to put the bitmap
    hImage = CreateWindow("Static", NULL, WS_VISIBLE | WS_CHILD | SS_BITMAP | WS_BORDER, 430, 100, 100, 100, hWnd, NULL, NULL, NULL );
    //send message to actually put celebimage on the window
    SendMessageW(hImage, STM_SETIMAGE, (WPARAM) IMAGE_BITMAP, (LPARAM) hCelebImage);
    //create button to submit question
    HWND hBut = CreateWindowW(L"Button", sub, WS_VISIBLE | WS_CHILD | WS_BORDER | BS_DEFPUSHBUTTON, 170, 190, 100, 50, hWnd, (HMENU)SUBMIT_BUTTON, NULL, NULL );

    HWND hHelpBut = CreateWindowW(L"Button", askhelp, WS_VISIBLE | WS_CHILD | WS_BORDER | BS_DEFPUSHBUTTON, 100, 290, 70, 30, hWnd, (HMENU)HELP_BUTTON, NULL, NULL );

    hBar = CreateWindowA("Static", NULL, WS_VISIBLE | WS_CHILD | SS_BITMAP | WS_BORDER, 20, 40, 10, 10, hWnd, NULL, NULL, NULL );

    SendMessageW(hBar,STM_SETIMAGE, (WPARAM) IMAGE_BITMAP, (LPARAM) hBarImage);

    hSkipBut = CreateWindowW(L"Button", L"Skip", WS_VISIBLE | WS_CHILD | WS_BORDER | BS_DEFPUSHBUTTON, 520, 370, 70, 30, hWnd, (HMENU)SKIP_BUTTON, NULL, NULL );

    hLogo = CreateWindow("Static", NULL, WS_VISIBLE | WS_CHILD | SS_BITMAP , 430, 30, 250, 50, hWnd, NULL, NULL, NULL );
    hLogoImage = (HBITMAP) LoadImageA(NULL, "various/WHOSTHATLogo.bmp", IMAGE_BITMAP, 250, 60, LR_LOADFROMFILE);
    SendMessageW(hLogo, STM_SETIMAGE, (WPARAM) IMAGE_BITMAP, (LPARAM) hLogoImage);



}


void ShowScore(HWND hWnd){
    hScore = CreateWindowW(L"Static", L"Score :" , WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER, 760, 40, 100, 35, hWnd, NULL, NULL, NULL );
    LOGFONT logfont;
    ZeroMemory(&logfont, sizeof(LOGFONT));
    logfont.lfCharSet = DEFAULT_CHARSET;
    logfont.lfHeight = -30;
    HFONT hFont = CreateFontIndirect(&logfont);
    SendMessage(hScore, WM_SETFONT, (WPARAM)hFont, TRUE);

    hActualRound = CreateWindowW(L"Static", L"Round :" , WS_VISIBLE | WS_CHILD | WS_BORDER | SS_LEFT , 715, 120, 70, 25, hWnd, NULL, NULL, NULL );
    LOGFONT logfont2;
    ZeroMemory(&logfont2, sizeof(LOGFONT));
    logfont2.lfCharSet = DEFAULT_CHARSET;
    logfont2.lfHeight = -20;
    HFONT hFont2 = CreateFontIndirect(&logfont2);
    SendMessage(hActualRound, WM_SETFONT, (WPARAM)hFont2, TRUE);

    hTotalRounds = CreateWindowW(L"Static", L"Total Rounds :" , WS_VISIBLE | WS_CHILD | WS_BORDER | SS_LEFT , 715, 160, 130, 27, hWnd, NULL, NULL, NULL );
    SendMessage(hTotalRounds, WM_SETFONT, (WPARAM)hFont2, TRUE);

    hGuessedCelebrities = CreateWindowW(L"Static", L"Guessed Celebrities :" , WS_VISIBLE | WS_CHILD | WS_BORDER | SS_LEFT , 715, 270, 190, 25, hWnd, NULL, NULL, NULL );
    SendMessage(hGuessedCelebrities, WM_SETFONT, (WPARAM)hFont2, TRUE);

    hSkippedCelebrities = CreateWindowW(L"Static", L"Skipped Celebrities  :" , WS_VISIBLE | WS_CHILD | WS_BORDER | SS_LEFT , 715, 310, 190, 25, hWnd, NULL, NULL, NULL );
    SendMessage(hSkippedCelebrities, WM_SETFONT, (WPARAM)hFont2, TRUE);



}

void DisplayScore(HWND hWnd, char* ds){

    hDispScore = CreateWindowA("Static", ds ,WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER, 860, 40, 70, 35, hWnd, NULL, NULL, NULL );
    LOGFONT logfont;
    ZeroMemory(&logfont, sizeof(LOGFONT));
    logfont.lfCharSet = DEFAULT_CHARSET;
    logfont.lfHeight = -30;
    HFONT hFont = CreateFontIndirect(&logfont);
    SendMessage(hDispScore, WM_SETFONT, (WPARAM)hFont, TRUE);



    hAcRound = CreateWindowA("Static", "3" ,WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER, 785, 120, 40, 25, hWnd, NULL, NULL, NULL );
    LOGFONT logfont2;
    ZeroMemory(&logfont2, sizeof(LOGFONT));
    logfont2.lfCharSet = DEFAULT_CHARSET;
    logfont2.lfHeight = -20;
    HFONT hFont2 = CreateFontIndirect(&logfont2);
    SendMessage(hAcRound, WM_SETFONT, (WPARAM)hFont2, TRUE);


    hTotRounds = CreateWindowA("Static", "5" ,WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER, 845, 160, 40, 27, hWnd, NULL, NULL, NULL );
    SendMessage(hTotRounds, WM_SETFONT, (WPARAM)hFont2, TRUE);

    hGuessed = CreateWindowA("Static", "4" ,WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER, 905, 270, 40, 25, hWnd, NULL, NULL, NULL );
    SendMessage(hGuessed, WM_SETFONT, (WPARAM)hFont2, TRUE);

    hSkipped = CreateWindowA("Static", "1" ,WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER, 905, 310, 40, 25, hWnd, NULL, NULL, NULL );
    SendMessage(hSkipped, WM_SETFONT, (WPARAM)hFont2, TRUE);


}



void SetScore(){

    int rc;
    rc=roundcounter+1;

    if(rc<=ROUNDS){
        sprintf(stracr, "%d", rc);
        SetWindowTextA(hAcRound, stracr);
    }
    else {
        sprintf(stracr, "%d", ROUNDS);
        SetWindowTextA(hAcRound, stracr);
    }

    sprintf(strtot, "%d", ROUNDS);
    SetWindowTextA(hTotRounds, strtot);


    sprintf(strguc, "%d", guessedcounter);
    SetWindowTextA(hGuessed, strguc);


    sprintf(strskc, "%d", skippedcounter);
    SetWindowTextA(hSkipped, strskc);

}




void LoadImages(LPCSTR path){      //function to load images, takes path of the image+namefile

    //hCelebImage is bitmap handler; path is the path+namefile
    char path2[20]="celebritiesbmp/";
    strcat(path2, path);
    hCelebImage = (HBITMAP)LoadImageA(NULL, path2 , IMAGE_BITMAP, 250, 250, LR_LOADFROMFILE);
    if(hCelebImage==NULL){
        printf("immagine non caricata");
    }
    //update window to show new celebrity's image
    //UpdateWindow(hImage);
}

void LoadBar(int points){




    if (points>0){
        hBarImage = (HBITMAP) LoadImageA(NULL, "various/greenbar.bmp" , IMAGE_BITMAP, float ( (float) factor * (float) points ) , 30, LR_LOADFROMFILE);
        }
    else
        hBarImage = (HBITMAP) LoadImageA(NULL, "various/greenbar.bmp" , IMAGE_BITMAP, -1, 30, LR_LOADFROMFILE);


}




void SetHelps(char* celname, int celindex, int cellength){

    int i,j;
    char *helping_string;
    helping_string = (char*) malloc(sizeof(char)*(cellength+1));
    helping_string[cellength]='\0';

    //printf("%s   \n", celname);

    for (i=0; i<cellength; i++){
        if (i==0 || i==(cellength-1))           //if first or last character, copy it
            helping_string[i]=celname[i];

        else if (celname[i]==' ')                    //if it's a space, copy it
            helping_string[i]=celname[i];

        else{   //if any other character
            if(celname[i-1]==' ') {     //if there is a space before, copy it
                helping_string[i]=celname[i];
                helping_string[i-2]=celname[i-2];    //also, put the letter at the end of every word before space
            }
            else                        //if there is no space before, put _
                helping_string[i]='_';
        }


    }

    printf("helping string:  %s  \n", helping_string);


    //take off comments down below if you want to run with true_helping_string

    /*char *true_helping_string;
    true_helping_string = (char*) malloc(sizeof(char)*2*cellength);


    for (i=0,j=0; i<cellength; i++, j+=2){

            if (helping_string[i]!='\0'){
                true_helping_string[j]=helping_string[i];
                true_helping_string[j+1]=' ';
            }

    }*/

     //printf("\n\n\n");

     //printf("true helping string %s\n", true_helping_string);

     helps[celindex].helpstring=(char*) malloc(sizeof(char*)*cellength);
     //helps[celindex].helpstring=(char*) malloc(sizeof(char*)*2*cellength+1);        //use this one if using true_helping_string

     strcpy(helps[celindex].helpstring, helping_string);
     //strcpy(helps[celindex].helpstring, true_helping_string); //use this for helping string


     //printf("true helping string:  %s  \n", true_helping_string); //use this for printing true_helping_string


     //printf("%s \n", helps[celindex].helpstring);     //use this for printing helpstring which is stored in helps array


     //free(helping_string);
     //free(true_helping_string);
}



void GenerateRandomArray(){             //might do this through DLL
HINSTANCE hLib = LoadLibrary("whosthatdll/bin/Debug/whosthatdll.dll");

    if (hLib!=NULL){
        GEN_RAN_ARR gen = (GEN_RAN_ARR) GetProcAddress(hLib, "Generate_Random_Array");
        if (gen!=NULL)
            gen(randomarray, database_size, ROUNDS);
    }
    else {
        printf("DLL not loaded");
    }

    FreeLibrary(hLib);
}



void List_Files (char* directory_name){
    char buffer[4096];
    int length=0;
    DIR *d;
    char *str1 = ".";
    char *str2 = "..";
    int i,j; //counters
    database_size=0;   //set number of celebrities in the database to 0

    struct dirent *dir;
    d = opendir(directory_name);

    if (d) {//check directory's existance
        while ((dir=readdir(d))!= NULL) {
            if (strcmp(dir->d_name, str1)!=0 && strcmp(dir->d_name, str2)!=0){  //check if it's not . and ..
                database_size=database_size+1;    //everytime you find a true file, add 1 to the files counter
            }
        }
        closedir(d);
    }


    bitmapsnames = (char**) malloc(database_size * sizeof(char *));
    celebritynames = (char**) malloc(database_size * sizeof(char *));

    d = opendir(directory_name);
    i=0;

    if (d) {//check directory's existance
        while ((dir=readdir(d))!= NULL) {
            if (strcmp(dir->d_name, str1)!=0 && strcmp(dir->d_name, str2)!=0){  //check if it's not . and ..
                //printf("%s \n", dir->d_name);   //print for debug
                strcpy(buffer, dir->d_name);
                length=strlen(buffer);
                buffer[length]='\0';
                bitmapsnames[i]= (char*) malloc(length*sizeof(char));       //get name of the file with extension (.bmp) and store it in bitmapsname[i]
                strcpy(bitmapsnames[i], buffer);
                buffer[length-4]='\0';
                celebritynames[i]= (char*) malloc((length-3)*sizeof(char)); //get name of the celebrity without extension and store it in celebritynames[i];
                strcpy(celebritynames[i], buffer);
                i++;

            }
        }
        closedir(d);
    }

    //now we have celebritynames which has list of celebrities and bitmapnsames which has list of bitmapsnames
}


void Select_Randoms(){


    char bufferb[4096]; //buffer for bitmaps names
    char bufferc[4096]; //buffer for celebrities names
    int lengthb=0;
    int lengthc=0;
    int i;

        for(i=0;i<ROUNDS;i++){

            strcpy(bufferb, bitmapsnames[randomarray[i]]);
            lengthb=strlen(bufferb);
            bufferb[lengthb]='\0';
            gamebitmaps[i]= (char*) malloc((lengthb+1)*sizeof(char));
            strcpy(gamebitmaps[i], bufferb);


            strcpy(bufferc, celebritynames[randomarray[i]]);
            lengthc=strlen(bufferc);
            bufferc[lengthc]='\0';
            gamecelebrities[i]= (char*) malloc((lengthc+1)*sizeof(char));
            strcpy(gamecelebrities[i], bufferc);

            SetHelps(gamecelebrities[i], i, lengthc);
        }

    free(celebritynames);
    free(bitmapsnames);

}


void Update_Scores_File(char *final_score, HWND hWnd){

    //first of all, create the file if it doesn't exist yet

    HANDLE hScoresFile;
    BOOL bScoresFile;
    char fin_score[strlen(final_score)+1];
    strcpy(fin_score, final_score);
    strcat(fin_score, "\n");
    DWORD dwBytesToWrite = strlen(fin_score);
    DWORD dwByteWritten = 0;


    //printf("fin_score = %s", fin_score);

    hScoresFile = CreateFileA(filename,  GENERIC_READ | FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hScoresFile == INVALID_HANDLE_VALUE){
        printf("could not create the file\n");
        CloseHandle(hScoresFile);
        return;
    }
    //printf("everything is ok, file created or opened \n");


    //now get the max value from the file
    int bestscore;

    HINSTANCE hLib = LoadLibrary("whosthatdll/bin/Debug/whosthatdll.dll");

    if (hLib!=NULL){
        BEST_SCORE b_s = (BEST_SCORE) GetProcAddress(hLib, "Get_Best_Score");
        if (b_s!=NULL)
            bestscore = b_s(filename);
    }
    else {
        printf("DLL not loaded");
    }


    FreeLibrary(hLib);


    bScoresFile = WriteFile(hScoresFile, fin_score, dwBytesToWrite, &dwByteWritten, NULL);

    if(bScoresFile == FALSE){
        printf("something went wrong on writing \n");
    }

    char gostrp1[40]="Game over! Your score was ";  //gameoverstring part 1
    strcat(gostrp1, final_score);


    if (punteggio==bestscore){
        char gostrp2[60] = "\nYour score is the same as the previous record!";
        strcat(gostrp1, gostrp2);
        MessageBoxA(hWnd, gostrp1, "Game Over", MB_OK);
    }

    else if (punteggio > bestscore){
        char gostrp3[40]="\nYour score is the new record!";
        strcat(gostrp1, gostrp3);
        MessageBoxA(hWnd, gostrp1, "Game Over", MB_OK);
    }

    else {
        char gostrp4[60]="\nYour score didn't beat the previous record";
        strcat(gostrp1, gostrp4);
        MessageBoxA(hWnd, gostrp1, "Game Over", MB_OK);
    }

    //closing handle
    CloseHandle(hScoresFile);
    return;

}
