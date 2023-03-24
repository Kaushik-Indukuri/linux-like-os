#include "i8259.h"
#include "lib.h"
#include "keyboard.h"
#include "terminal.h"

int shiftFlag = 0;
int capsFlag = 0;
int ctrlFlag = 0;
int altFlag = 0;
int keystroke;
char kbdnumvals [] = {'1','2','3','4','5','6','7','8','9','0','-','=',
                        '!','@','#','$','%','^','&','*','(',')','_','+'};
char kbdletvals_1 [] = {'q','w','e','r','t','y','u','i','o','p','[',']',
                        'Q','W','E','R','T','Y','U','I','O','P','{','}'};
char kbdletvals_2 [] = {'a','s','d','f','g','h','j','k','l',';','\'','`',
                        'A','S','D','F','G','H','J','K','L',':','"' ,'~'};
char kbdletvals_3 [] = {'\\','z','x','c','v','b','n','m',',','.','/',
                        '|','Z','X','C','V','B','N','M','<','>','?'};

/* void keyboard_init() 
 * 
 * Initializes keyboard to send interrupts
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void keyboard_init()
{
    enable_irq(1); //Initializes keyboard with irq 1 on primary pic
}

/* void keyboard_ir_handler()
 * 
 * Handles interrupts from the keyboard and prints appropriate key
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void keyboard_ir_handler()
{
    sti();
    keystroke = inb(EOI);
    switch(keystroke)
    {
        case 0x2A:
        case 0x36:
            shiftFlag = 1;
            break;
        case 0xAA:
        case 0xB6:
            shiftFlag = 0;
            break;
        case 0x3A:
            capsFlag=(!capsFlag);
            //printf("%d",capsFlag);
            break;
    }
    keystroke = printKey(keystroke,shiftFlag,capsFlag);
    if(keystroke!=0x00)
    {
        putc(keystroke);
    }
    cli();
    send_eoi(1); //ends eoi at start
}

/* char printKey()
 * 
 * Outputs a key to the terminal given an input and caps/shift status
 * Inputs: key code, shift flag, caps flag
 * Outputs: Char to be printed
 * Side Effects: None
 */
char printKey(int code,int shift,int caps)
{
    if(code>=0x02 && code <= 0x0D)//0x02 - start num row, 0x0B - end num row
    {
        return(kbdnumvals[(code-0x02)+shift*12]);//offset by 0x02 - start num row
    }
    else if(code>=0x10 && code <= 0x1B)//0x10 - start let row1, 0x19 - end let row1
    {
        if(capsFlag == 1 && code<=0x19)
        {
             return(kbdletvals_1[(code-0x10)+(12*(caps^shift))]);
        }
        else
        {
            return(kbdletvals_1[(code-0x10)+shift*12]);//offset by 0x10 - start let row1 
        }
    }
    else if(code>=0x1E && code <= 0x29)//0x1E - start let row2, 0x26 - end let row2
    {
        if(capsFlag == 1 && code<=0x26)
        {
            return(kbdletvals_2[(code-0x1E)+(12*(caps^shift))]);//offset by 0x1E - start let row2
        }
        else
        {
            return(kbdletvals_2[(code-0x1E)+shift*12]);//offset by 0x1E - start let row2
        }
    }
    else if(code>=0x2B && code <= 0x35)//0x2C - start let row3, 0x32 - end let row3
    {
        if(capsFlag == 1 && code<=0x32 && code>=0x2C)
        {
            return(kbdletvals_3[(code-0x2B)+(11*(caps^shift))]);//offset by 0x2C - start let row3
        }
        else
        {
            return(kbdletvals_3[(code-0x2B)+shift*11]);//offset by 0x2C - start let row3
        }
    }
    else if(code == 0x39)//0x39 - value for spacebar
    {
        return(' ');    
    }
    return(0x00);
}

        //printf("%d",strlen(termLineBuffer));
        //update_cursor(0,0);

       
    //    int i = 0;
    //    clear();
    //    for (i = 0;i<4;i++)
    //    {
    //     //printf("%s",'\n');
    //    }
       /* int i;
        for(i = 0;i<128;i++)
        {
            termLineBuffer[i] = ' ';
        }
        termLineBuffer[0] = 'A';
        termLineBuffer[127] = 'B';
        printf("%s",termLineBuffer);*/



