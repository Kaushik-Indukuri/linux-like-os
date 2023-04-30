#include "i8259.h"
#include "lib.h"
#include "keyboard.h"
#include "terminal.h"

int shiftFlag = 0;
int capsFlag = 0;
int ctrlFlag = 0;
int altFlag = 0;
int keystroke;
int kbdenable;

char keyboardBuffer[3][128];
int kbdStart_x[3];
int kbdStart_y[3];
int kbdBufPos[3];

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
    kbdenable=1;
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
        case 0x38: //0x38 = Alt Pressed keycode
            altFlag = 1;
            break;
        case 0xB8: //0x38 = Alt Released keycode
            altFlag = 0;
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
        int i;
        clear();
        screen_x = 0;
        screen_y = 0;
        clear_termBuf(curr_terminal);
        clear_kbdBuf(curr_terminal);
        update_cursor(screen_x,screen_y);
    }
    else if(altFlag && (keystroke==0xBB||keystroke==0xBC||keystroke==0xBD))
    {
        terminal_switch(keystroke-0xBB);
    }
    else if(keystroke==0x0E && strlen(keyboardBuffer[curr_terminal]) > 0 && kbdenable==1) //0x0E = keycode for backspace
    {
        kbdBufPos[curr_terminal]--;
        keyboardBuffer[curr_terminal][kbdBufPos[curr_terminal]] = ' ';
        termLineBuffer[curr_terminal][kbdBufPos[curr_terminal]] = ' '; //Fills in backspaced spot with space
        screen_x = kbdStart_x[curr_terminal];
        screen_y = kbdStart_y[curr_terminal];
        terminal_write(0,keyboardBuffer[curr_terminal],kbdBufPos[curr_terminal]+1);
        keyboardBuffer[curr_terminal][kbdBufPos[curr_terminal]] = 0x00; //Makes backspaced spot null
        termLineBuffer[curr_terminal][kbdBufPos[curr_terminal]] = 0x00;
        screen_x--;
        update_cursor(screen_x,screen_y);
     }   
    else if(keystroke<=0x39 && strlen(keyboardBuffer[curr_terminal])<127 && printKey(keystroke,shiftFlag,capsFlag)!=0x00 && kbdenable==1) //0x39 = max range for key inputs
    {
        if(strlen(keyboardBuffer[curr_terminal])==0)
        {
            kbdStart_x[curr_terminal] = screen_x;
            kbdStart_y[curr_terminal] = screen_y; //Sets keyboard line start and end position
        }
        keyboardBuffer[curr_terminal][kbdBufPos[curr_terminal]] = printKey(keystroke,shiftFlag,capsFlag);
        termLineBuffer[curr_terminal][kbdBufPos[curr_terminal]] = printKey(keystroke,shiftFlag,capsFlag);
        kbdBufPos[curr_terminal]++;
        screen_x = kbdStart_x[curr_terminal];
        screen_y = kbdStart_y[curr_terminal];
        terminal_write(0,keyboardBuffer[curr_terminal],kbdBufPos[curr_terminal]); //Rewrites line to terminal
    }
    else if(keystroke==0x1C && kbdenable==1) //0x1C = keycode for enter
    {
        if(strlen(keyboardBuffer[curr_terminal])==0)
        {
            kbdStart_x[curr_terminal] = screen_x;
            kbdStart_y[curr_terminal] = screen_y;  
        }
        keyboardBuffer[curr_terminal][kbdBufPos[curr_terminal]] = '\n'; //Adds new line char to buffer
        termLineBuffer[curr_terminal][kbdBufPos[curr_terminal]] = '\n';
        screen_x = kbdStart_x[curr_terminal];
        screen_y = kbdStart_y[curr_terminal];
        terminal_write(0,keyboardBuffer[curr_terminal],kbdBufPos[curr_terminal]+1);
        termBufPos[curr_terminal] = kbdBufPos[curr_terminal];
        clear_kbdBuf(curr_terminal); //Clears keyboard buffer
    }
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
    else if(code == 0x0F) //0x0F - value for tab
    {
        return('\t');
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
void clear_kbdBuf(int terminal)
{
    int i;
    for(i = 0;i<128;i++) //128 = max buffer size
    {
        keyboardBuffer[terminal][i] = 0x00;
    }
    kbdBufPos[terminal] = 0;
}



