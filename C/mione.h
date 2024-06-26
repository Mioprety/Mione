void toBeCompileOnFile(char* fileName,char ***THEFILE,int *L);
int compile(char **THEFILE,int L,char* *** MIO,int MEMORY_GROUP,int* NL,int **EveryLine);
void toBeCompileWithCode(char* code, char*** THEFILE,int * L);

#ifndef mione_h
#define mione_h
#include "mio.h"
#include "prerr.h"



int getWordType(int* var, char word) {
    *var = 1;
    if (word == '"') {
        *var = 2;
        return 1;
    }

    if (word == '\'') {
        *var = 13;
        return 1;
    }


    if (word == ' ') {
        *var = -1;
        return 1;
    }

    char x[] = {',', '-', '+', '=', '*', '>','<',';','\0'};
    for (int i = 0; i < strlen(x); i++) {
        if (word == x[i]) {
            *var = 7;
            return 1;
        }
    }
    char y[] = {'[',']','(',')','\0'}; //不可重複兩個的符號
    for (int i = 0; i < strlen(y); i++) {
        if (word == y[i]) {
            *var = 14;
            return 1;
        }
    }

    if (word == '/') {
        *var = 16;
        return 1;
    }

    for (int i = 0; i < 26; i++) {
        char a;
        a = 65 + i;
        char b;
        b = 97 + i;


        if (word == a || word == b || word == '_') {
            *var = 6;
            return 1;
        }

    }

    for (int i = 0; i < 10; i++) {
        char aw[5];
        sprintf(aw, "%d", i);

        if (aw) {
            if (word == aw[0]) {
                *var = 5;
                return 1;
            }
        }
    }

    if (word == '{') {
        *var = 3;
        return 1;
    }
    if (word == '}') {
        *var = 8;
        return 1;
    }


    if (word == '@') {
        *var = 4;
        return 1;
    }
    if (word == '$') {
        *var = 9;
        return 1;
    }
    if (word == '#') {
        *var = 10;
        return 1;
    }

    if (word == '\n') {
        *var = 1;
        return 1;
    }
    if (word == '\\') {
        *var = 12;
        return 1;
    }

    return 2;
};



void toBeCompileWithCode(char* code, char* ** THEFILE,int *L){




    char* aline = malloc(sizeof(char));
    int lineword = 0;
    for (int word = 0; word < strlen(code); word++) {

        if (code[word] == '\n') {}else{
            lineword++;

            aline = realloc(aline, sizeof(char) * ((lineword) + 1));
            aline[lineword-1] = code[word];
        }
        if (code[word] == '\n' ||strlen(code)-1==word) {
            aline[lineword] = 0;
            (*L)++;

            if ((*THEFILE) == NULL) {

                (*THEFILE) = malloc(sizeof(char*) * (*L));

            }
            else {
                (*THEFILE) = realloc((*THEFILE), sizeof(char*) * (*L));
            }
            (*THEFILE)[(*L) - 1] = malloc(strlen(aline) + 1);
            strcpy((*THEFILE)[(*L) - 1], aline);
            strcat((*THEFILE)[(*L) - 1],"\n");


            aline = NULL;
            aline = malloc(sizeof(char));
            lineword = 0;

        }
    }
}

void toBeCompileOnFile(char* fileName,char * **THEFILE,int *L) {

    FILE* file = fopen(fileName, "r");
    char line[2048];

    if (file == NULL) {
        printf("Cant open file\n");
        return;
    }

    while (fgets(line, sizeof(line), file) != NULL) {

        (*L)++;

        if ((*THEFILE) == NULL){
            (*THEFILE) = malloc(sizeof(char*) * (*L));

        }  else{
            (*THEFILE) = realloc((*THEFILE), sizeof(char*) * (*L));
        }
        (*THEFILE)[(*L)-1] = malloc(strlen(line) + 1);
        strcpy((*THEFILE)[(*L)-1], line);
    }

    fclose(file);
}

int compile(char **THEFILE,int L,char* *** MIO,int MEMORY_GROUP,int* NL,int **EveryLine) {
    int lastCheckType = 0;
    int Line = 0;

    int	nextWordType = 0;
    int	lastWordType = -1; //here
    int wordType = 1;
    int aboutWord[] = {
            0,//slash 斜線    上個
            0,//slash 值      目前
    };


    int lvl = 0;
    int canCount = 1;
    int nextCanCount = 0;

    int forErr[] = {
            0, // 表單，括號或函數開始行
            0, // 字串開始行
    };


    int canWrite = 1;
    int nextCanWrite = 0;

    int doBREAK = 0;
    int lastIsBreak = 0;

    char* txt = malloc(sizeof(char) * 2);
    strcpy(txt, "\0");

    int cSize = 0;


    for (int me = 0;me<L;me++){

        char *line = THEFILE[me];
        Line++;

        int lastOne = strlen(line) - 1;

        for (int i = 0; strlen(line) > i; i++) {
            if (i == lastOne) {
                strcat(line, " ");
            }

            char word = line[i];
            int checkType = 1;
            getWordType(&checkType, word);

            //====================================================
            if (nextCanWrite) {
                canWrite = nextCanWrite;
                nextCanWrite = 0;
            }

            if (nextCanCount) {
                canCount = nextCanCount;
                nextCanCount = 0;
            }



            // CHECKTYPE:
            // -1:空格 1:無/換行 2:字串開頭1 3:表單開頭 4:函數結束 5:數字 6:英文字母/底線 7: 符號 8:表單結束 9:函數開頭1 10:函數開頭2 12:'\'符號
            // 13:字串開頭2 14:不可重複兩個的符號  16:`/`符號

            // WORDTYPE:
            // -1:空格 1:無/換行 2:字串1 3:表單 4:函數 5:數字 6:英文字母/底線 7: 符號 8:執行式 10:'\'符號 11:字串2 14:解說符號 15: 符號二 16:符號二之二
            //

            if (nextWordType) {
                wordType = nextWordType;
                nextWordType = 0;
            }
            else {
                if (checkType == 2) { // STRING 字串
                    if (aboutWord[0]) {}
                    else {
                        if (forErr[1]==0 && lastWordType == 2){

                            prerr(Line,"String Out!",-1);
                        }




                        if (lastWordType == 2) {
                            nextCanWrite = 1;
                            aboutWord[1] = 0;
                            forErr[1] = 0;

                        }
                        else {
                            if (canWrite == 1) {
                                wordType = 2;
                                canWrite = 0;
                                forErr[1] = Line;
                            }
                        }
                        if (lastWordType == 2){
                            if (canCount) {
                                canCount = 0;
                            }
                            else {
                                nextCanCount = 1;
                            }
                        }

                    }
                }

                if (checkType == 13) { // STRING 字串
                    if (aboutWord[0]) {
                    } else {
                        if (forErr[1]==0 && lastWordType == 11){

                            prerr(Line,"String Out!",-1);
                        }


                        if (lastWordType == 11) {
                            nextCanWrite = 1;
                            aboutWord[1] = 0;
                            forErr[1] = 0;

                        } else {
                            if (canWrite == 1) {
                                wordType = 11;
                                canWrite = 0;
                                forErr[1] = Line;
                            }
                        }
                        if (lastWordType == 11) {
                            if (canCount) {
                                canCount = 0;
                            } else {
                                nextCanCount = 1;
                            }
                        }
                    }
                }

                if (checkType == 3) { // TABLE 表單
                    if (canWrite == 1) {
                        wordType = 3;
                        canWrite = 0;

                        if (forErr[0] == 0 && lastWordType == 3){
                            prerr(Line,"Creating the table starts and fails.",3);
                        }


                        if (canCount) {
                            if (forErr[0]) {}
                            else {
                                forErr[0] = Line;
                            }
                            lvl++;
                        }
                    }
                    else {
                        if (lastWordType == 3) {
                            if (canCount) {
                                lvl++;
                            }
                        }
                    }

                }

                if (checkType == 9) { // FUNCTION 函數
                    if (canWrite == 1) {
                        wordType = 4;
                        canWrite = 0;

                        if (forErr[0] == 0 && lastWordType == 4){
                            prerr(Line,"Creating the Function starts and fails.",3);
                        }
                        if (canCount) {
                            if (forErr[0]) {}
                            else {
                                forErr[0] = Line;
                            }
                            lvl++;
                        }
                    }
                    else {
                        if (lastWordType == 4 || lastWordType == 8) {
                            if (canCount) {
                                lvl++;
                            }
                        }
                    }

                }

                if (checkType == 10) { // Runction 函數
                    if (canWrite == 1) {
                        wordType = 8;
                        canWrite = 0;

                        if (forErr[0] == 0 && lastWordType == 8){
                            prerr(Line,"Creating the RUN starts and fails.",3);
                        }

                        if (canCount) {
                            if (forErr[0]) {}
                            else {
                                forErr[0] = Line;
                            }
                            lvl++;
                        }
                    }
                    else {
                        if (lastWordType == 4 || lastWordType == 8) {
                            if (canCount) {
                                lvl++;
                            }
                        }
                    }

                }










                if (checkType == 5) {
                    if (canWrite == 1) {
                        if (lastWordType == 6) { // 為了 `set x1 = 0`
                            wordType = 6;
                        }
                        else {
                            wordType = 5;
                        }

                    }
                }

                if (checkType == 6) {
                    if (canWrite == 1) {
                        wordType = 6;
                    }
                }


                //符號


                if (checkType == 7) {
                    if (canWrite == 1) {
                        wordType = 7;
                    }
                }

                if (checkType == 14) {
                    if (canWrite == 1) {
                        if (lastWordType == 15) {
                            wordType = 16  ;
                        }
                        else if (lastWordType == 16) {
                            wordType = 15;
                        }else{
                            wordType = 15;
                        }
                    }
                }

                if (checkType == 16) { //解說符號，或是除法，這裡要判斷他是兩個解說符號在一起的

                    if (canWrite == 1) {
                        int nextCheckType = 0;
                        getWordType(&nextCheckType, line[i + 1]);
                        if (nextCheckType == 16) { //代表下一個是 `/` 符號
                            wordType = 14;
                            //不需要先 canWrite 歸零
                        }
                        else if (lastCheckType == 16) { //代表上一個是 `/`符號
                            wordType = 14;
                            canWrite = 0;
                        }
                        else {
                            wordType = 7;
                        }
                    }
                    else {
                        if (lastCheckType == 16) {
                            if (lastCheckType == 16) { //代表上一個是 `/`符號
                                wordType = 14;
                                nextCanWrite = 1;
                            }
                        }
                    }
                }




                if (checkType == 8) { // TABLE 表單
                    if (lastWordType == 3) {

                        if (canCount) {
                            lvl--;
                        }
                        if (lvl) {}
                        else {
                            wordType = 3;
                            nextCanWrite = 1;

                            forErr[0] = 0;
                        }

                    }else{
                        if (canWrite) {
                            prerr(Line, "Creating the table ends and fails.", 5);
                        }
                    }
                }

                if (checkType == 4) {
                    if (lastWordType == 4 || lastWordType == 8) {
                        if (canCount) {
                            lvl--;
                        }
                        if (lvl) {}
                        else {
                            if (lastWordType == 4) {
                                wordType = 4;
                            }
                            else {
                                wordType = 8;
                            }

                            nextCanWrite = 1;

                            forErr[0] = 0;
                        }
                    }else{
                        if (canWrite){
                            prerr(Line,"Creating the Function or Runthion ends and fails.",4);
                        }

                    }
                }

                if (checkType == -1) {
                    if (canWrite == 1) {
                        wordType = -1;
                    }
                    aboutWord[0] = 0;
                }

                if (checkType == -2) {
                    if (canWrite == 1) {
                        wordType = -2;
                    }
                    aboutWord[0] = 0;
                }

                if (checkType == 1) {
                    if (canWrite == 1) {
                        wordType = 1;
                    }

                }

                if (checkType == 12) {
                    if (aboutWord[0]) {}
                    else {
                        aboutWord[1] = 1;
                    }
                }
            }

            //===================================================

            if (wordType == lastWordType) {
                int len = strlen(txt);
                txt = realloc(txt, len + 1 + 1);
                txt[len] = word;
                txt[len + 1] = '\0';



            }
            else {
                //here
                printf("| [CASE]:`%s`				[TYPE]:`%d`| \n", txt, lastWordType);

                mio((txt), lastWordType,&cSize,NL,MIO,MEMORY_GROUP,EveryLine);
                free((txt));


                (txt) = NULL;
                (txt) = malloc(sizeof(char) * (1 + 1));
                (txt)[0] = word;
                (txt)[1] = '\0';

            }





             //printf("%d '%d' '%c' '%d' '%d' '%d'\n", i,wordType, word, checkType,canCount,forErr[0]);
            lastWordType = wordType;
            lastCheckType = checkType;


            aboutWord[0] = aboutWord[1];
            aboutWord[1] = 0;

            lastIsBreak = doBREAK;
            doBREAK = 0;
        }
    }




    mioEnd(cSize,NL,EveryLine,MEMORY_GROUP); //我不想進行大更改了 w


    if (wordType == 2 || wordType == 11) {
        prerr(forErr[1], "The end of the string has not yet been declared.", 2);
    }

    if (forErr[0]) {
        prerr(forErr[0], "The form or function has not been completed **end indicator**.", 1);
    }



    //可以使用 wordType ==14 來判斷使用者是否有做 說明結尾



    //MIONE
    int re = cSize;
    cSize=0;
    return re;
}



#endif