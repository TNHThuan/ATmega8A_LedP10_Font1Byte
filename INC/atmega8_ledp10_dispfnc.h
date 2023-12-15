#include <atmega8_ledp10_conf.h> //config ports
#include <stdio.h>      //sprintf

//---------- SCAN ----------//
void IC138QuetHang(unsigned char soHang);
void IC74HC595GuiDuLieu(unsigned char text);
void TimerHandlerShowLed();

//---------- PIXEL MATRIX ----------//
void MatrixSetup(int matrix_setupX, int matrix_setupY, unsigned char color);

//---------- APPLICATION ----------//
void Matrix_SelectPos(unsigned char x, unsigned char y);              //select position on matrix
void Matrix_ClearFromTo(unsigned char fromX, unsigned char fromY, unsigned char toX, unsigned char toY);    //clear matrix in range
void Matrix_PrintText(unsigned char text);                   //print character
void Matrix_PrintString(unsigned char *string);                     //print string

/*
IC138QuetHang quyet dinh hang nao duoc bat,
dua vao soHang - AB:
A=0, B=0: 0 4 8 12 bat
A=1, B=0: 1 5 9 13 bat
A=0, B=1  2 6 10 14 bat
A=1. B=1  3 7 11 15 bat
*/
void IC138QuetHang(unsigned char soHang)
{
    switch(soHang)
    {
        case 1: 
        {
            A = 0; B = 0; OE = 1; break;
        }         
        case 2:
        {
            A = 1; B = 0; OE = 1; break;
        }         
        case 3:
        {
            A = 0; B = 1; OE = 1; break;
        }         
        case 4:
        {
            A = 1; B = 1; OE = 1; break;
        }
    }
}

//dua 8 bit data ra output 595
//sau khi dich du 8 bit thi chot data
void IC74HC595GuiDuLieu(unsigned char text)
{
    unsigned char i;
    for(i = 0; i < 8; i++)
    {
        DS = text & (0x80 >> i);
        NCLK = 1;
        NCLK = 0;
    }
}

//Timer Interrupt
unsigned char bufferDisplay[32][8];     //LEDP10 32 dong - 8 block
void TimerHandlerShowLed()
{
    int dong, cot;
    int hangQuet;   //xem IC138QuetHang
    int luot;       //gui luot 1 va luot 2, luot 1 tu dong 0=>15, luot 2 tu dong 16=>31
    for(hangQuet = 1; hangQuet < 5; hangQuet++)
    {
        for(luot = 0; luot < 17; luot+=16)
        {
            for(cot = 0; cot < 8; cot++)
            {
                for(dong = 16 + luot; dong > 0 + luot; dong = dong - 4)
                {
                    //khong den nen
                    IC74HC595GuiDuLieu(~(bufferDisplay[dong-hangQuet][cot]));
                } 
            }
        }
        SCLK=0;
        SCLK=1;
        //sau khi nap xong cho 4 hang thi bat hang len  
        IC138QuetHang(5-hangQuet);       
        delay_us(2000);      //PWM do sang
        OE=0;   
    }
}

//setting vi tri con tro tren matrix
unsigned char matrix_selectX, matrix_selectY;     //bien toan cuc de xet vitri tren led
void Matrix_SelectPos(unsigned char x, unsigned char y)
{
    matrix_selectX = x;
    matrix_selectY = y;
}

//ON-OFF mot diem tren matrix
//color = 1 => ON
//color = 0 => OFF
void MatrixSetup(int matrix_setupX, int matrix_setupY, unsigned char color)
{
    if(matrix_setupX > 63 || matrix_setupY > 31 || matrix_setupX < 0 || matrix_setupY < 0)
        return;
    if(color)
    {
        //x/8 de chon matrix
        //vd 15/8 = 1 => diem sang tai matrix 1 
        
        //x%8 de chon toa do
        //vd 15%8 = 7 => tai toa do 7-y
        
        //=> diem sang tai toa do 7-y o matrix 1 
        bufferDisplay[matrix_setupY][matrix_setupX/8] |= (0x80 >> (matrix_setupX%8));   
    }                                            
    else
    {
        bufferDisplay[matrix_setupY][matrix_setupX/8] &= ~(0x80 >> (matrix_setupX%8));  
    }
}

//clear matrix
//quet toan bo x va y de tat led
unsigned char maxtrix_printTextX, maxtrix_printTextY;
void Matrix_ClearFromTo(unsigned char fromX, unsigned char fromY, unsigned char toX, unsigned char toY)
{
    for(maxtrix_printTextY = fromY; maxtrix_printTextY <= toY; maxtrix_printTextY++)
    {
        for(maxtrix_printTextX = fromX; maxtrix_printTextX <= toX; maxtrix_printTextX++)
        {
            MatrixSetup(maxtrix_printTextX, maxtrix_printTextY, 0);
        }
    }
}

//print text, chi 1 ky tu
//so sanh giua vi tri con tro tren matrix voi ma font
//khi co su trung nhau thi vi tri con do se ON
//tang vi tri con tro tuong ung voi width cua text
void Matrix_PrintText(unsigned char text)
{
    unsigned char widthCharBuf;
    widthCharBuf = codeFontIncX[text - 32];
    for(maxtrix_printTextY = matrix_selectY; maxtrix_printTextY < 14 + matrix_selectY; maxtrix_printTextY++)    //font height = 14
    {
        for(maxtrix_printTextX = matrix_selectX; maxtrix_printTextX < widthCharBuf + matrix_selectX; maxtrix_printTextX++)
        {
            if((codeFont[text - 32][maxtrix_printTextY - matrix_selectY] & (0x80 >> (maxtrix_printTextX - matrix_selectX))) != 0)
                MatrixSetup(maxtrix_printTextX, maxtrix_printTextY, 1);
            else
                MatrixSetup(maxtrix_printTextX, maxtrix_printTextY, 0); 
        }
    }
    matrix_selectX+=widthCharBuf + 1;   //tang vi tri X len width cho char tiep theo 
}

//print chuoi bang cach truyen tung char
void Matrix_PrintString(unsigned char *string)
{
    while(*string)
    {
        Matrix_PrintText(*string);
        string++;    
    }
}
