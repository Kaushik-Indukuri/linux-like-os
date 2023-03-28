#include "i8259.h"
#include "lib.h"
#include "keyboard.h"
#include "terminal.h"

int shiftFlag = 0;
int capsFlag = 0;
int ctrlFlag = 0;
int altFlag = 0;
char keyboardBuffer [128];
int keystroke;
int kbdStart_x;
int kbdStart_y;
int kbdBufPos;

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
    //int i;
    cli();
    keystroke = inb(EOI);
    switch(keystroke)
    {
        case 0x2A:
        case 0x36: //0x2A or 0x36 = Shift Pressed keycode
            shiftFlag = 1;
            break;
        case 0xAA:
        case 0xB6: //0xB6 or 0xAA = Shift Released keycode
            shiftFlag = 0;
            break;
        case 0x1D: //0x1D = Control Pressed keycode
            ctrlFlag = 1;
            break;
        case 0x9D: //0x9D = Control Released keycode
            ctrlFlag = 0;
            break;
        case 0x3A: //0x3A = Capslock keycode
            capsFlag=(!capsFlag);
            break;
    }
    if(keystroke==0x26 && ctrlFlag) //0x26 = keycode for L
    {
        terminal_open();
    }
    else if(keystroke==0x0E && strlen(keyboardBuffer) > 0) //0x0E = keycode for backspace
    {
        kbdBufPos--;
        keyboardBuffer[kbdBufPos] = ' ';
        termLineBuffer[kbdBufPos] = ' '; //Fills in backspaced spot with space
        screen_x = kbdStart_x;
        screen_y = kbdStart_y;
        terminal_write(0,keyboardBuffer,kbdBufPos+1);
        keyboardBuffer[kbdBufPos] = 0x00; //Makes backspaced spot null
        termLineBuffer[kbdBufPos] = 0x00;
        screen_x--;
        update_cursor(screen_x,screen_y);
     }   
    else if(keystroke<=0x39 && strlen(keyboardBuffer)<127 && printKey(keystroke,shiftFlag,capsFlag)!=0x00) //0x39 = max range for key inputs
    {
        if(strlen(keyboardBuffer)==0)
        {
            kbdStart_x = screen_x;
            kbdStart_y = screen_y; //Sets keyboard line start and end position
        }
        keyboardBuffer[kbdBufPos] = printKey(keystroke,shiftFlag,capsFlag);
        termLineBuffer[kbdBufPos] = printKey(keystroke,shiftFlag,capsFlag);
        kbdBufPos++;
        screen_x = kbdStart_x;
        screen_y = kbdStart_y;
        terminal_write(0,keyboardBuffer,kbdBufPos); //Rewrites line to terminal
    }
    else if(keystroke==0x1C) //0x1C = keycode for enter
    {
        if(strlen(keyboardBuffer)==0)
        {
            kbdStart_x = screen_x;
            kbdStart_y = screen_y;  
        }
        keyboardBuffer[kbdBufPos] = '\n'; //Adds new line char to buffer
        termLineBuffer[kbdBufPos] = '\n';
        screen_x = kbdStart_x;
        screen_y = kbdStart_y;
        terminal_write(0,keyboardBuffer,kbdBufPos+1);
        termBufPos = kbdBufPos;
        clear_kbdBuf(); //Clears keyboard buffer
    }
    sti();
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
    if(code>=0x02 && code <= 0x0D)//0x02 - start num row, 0x0D - end num row
    {
        return(kbdnumvals[(code-0x02)+shift*12]);//offset by 0x02 - start num row
    }
    else if(code>=0x10 && code <= 0x1B)//0x10 - start let row1, 0x1B - end let row1
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
    else if(code>=0x1E && code <= 0x29)//0x1E - start let row2, 0x29 - end let row2
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
    else if(code>=0x2B && code <= 0x35)//0x2C - start let row3, 0x35 - end let row3
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

/*
 * clear_kbdBuf
 *   DESCRIPTION: Clears Keyboard Buffer
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Clears the buffer
 */
void clear_kbdBuf()
{
    int i;
    for(i = 0;i<128;i++) //128 = max buffer size
    {
        keyboardBuffer[i] = 0x00;
    }
    kbdBufPos = 0;
}


