#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <irq.h>
#include <libbase/uart.h>
#include <libbase/console.h>
#include <generated/csr.h>

/*-----------------------------------------------------------------------*/
/* Uart                                                                  */
/*-----------------------------------------------------------------------*/

static char *readstr(void)
{
	char c[2];
	static char s[64];
	static int ptr = 0;

	if(readchar_nonblock()) {
		c[0] = getchar();
		c[1] = 0;
		switch(c[0]) {
			case 0x7f:
			case 0x08:
				if(ptr > 0) {
					ptr--;
					fputs("\x08 \x08", stdout);
				}
				break;
			case 0x07:
				break;
			case '\r':
			case '\n':
				s[ptr] = 0x00;
				fputs("\n", stdout);
				ptr = 0;
				return s;
			default:
				if(ptr >= (sizeof(s) - 1))
					break;
				fputs(c, stdout);
				s[ptr] = c[0];
				ptr++;
				break;
		}
	}

	return NULL;
}

static char *get_token(char **str)
{
	char *c, *d;

	c = (char *)strchr(*str, ' ');
	if(c == NULL) {
		d = *str;
		*str = *str+strlen(*str);
		return d;
	}
	*c = 0;
	d = *str;
	*str = c+1;
	return d;
}

static void prompt(void)
{
	printf("\e[92;1mLitex Simple CPU\e[0m> ");
}

static int strfc(char token[],char arg[],int arr[]) {
    int arg_len=0;
    int no_of_args=0;
    char new_value[100];
    int new_len=0;
    new_value[0]='\0';
    while(token[arg_len]!='\0'&&arg[arg_len]!='\0'&&token[arg_len]!=' ') {
        if(token[arg_len]!=arg[arg_len])
            return 0;
        arg_len++;
    }
    arg_len++;
    while(token[arg_len]!='\0') {
        if((token[arg_len]>'9'||token[arg_len]<'0')&&token[arg_len]!=' ') {
            //printf("Problem with %c",token[arg_len]);
	    arr[0]=-1;
            return 1;
        }
        if(token[arg_len]==' ') {
            //printf("String is %s\n",new_value);
            arr[no_of_args+1]=atoi(new_value);
            no_of_args++;
            new_len=0;
            new_value[0]='\0';
        } else {
            new_value[new_len]=token[arg_len];
            new_value[new_len+1]='\0';
            new_len++;
        }
        arg_len++;
    }
    arr[no_of_args+1]=atoi(new_value);
    no_of_args++;
    arr[0]=no_of_args;
    //printf("Total args %d\n",no_of_args);
    return 1;
}

/*-----------------------------------------------------------------------*/
/* Help                                                                  */
/*-----------------------------------------------------------------------*/

static void help(void)
{
	puts("\n Simple CPU Help Commands \n");
	puts("Available commands:");
	puts("help		- Show this command");
	puts("reboot		- Reboot CPU");
	puts("add		- Addition of {num1} {num2} {num3} ...");
	puts("mul		- Multiplication of {num1} {num2} {num3} ...");
	puts("sub		- Subtraction from {num1} of {num2} {num3} ...");
	puts("div 		- Division from {num1} of {num2} {num3} ...");
}

/*-----------------------------------------------------------------------*/
/* Commands                                                              */
/*-----------------------------------------------------------------------*/

static void reboot_cmd(void)
{
	ctrl_reset_write(1);
}

extern void addn(int arr[]);
extern void muln(int arr[]);
extern void subn(int arr[]);
extern void divn(int arr[]);


/*-----------------------------------------------------------------------*/
/* Console service / Main                                                */
/*-----------------------------------------------------------------------*/

static void console_service(void)
{
	char *str;
	char *token;
	int arr[100];

	str = readstr();
	if(str == NULL) return;
	token = get_token(&str);
	if(strcmp(token, "help") == 0)
		help();
	else if(strcmp(token, "reboot") == 0)
		reboot_cmd();
	else if(strfc(token,"add",arr)) {
		if(arr[0]<1) {
			printf(" ! invalid arguments\n");
		} else {
			addn(arr);
		}
	}
        else if(strfc(token,"sub",arr)) {
                if(arr[0]<1) {
                        printf(" ! invalid arguments\n");
                } else {
                        subn(arr);
                }
        }
        else if(strfc(token,"mul",arr)) {
                if(arr[0]<1) {
                        printf(" ! invalid arguments\n");
                } else {
                        muln(arr);
                }
        }
        else if(strfc(token,"div",arr)) {
                if(arr[0]<2) {
                        printf(" ! invalid arguments\n");
                } else {
                        divn(arr);
                }
        }
	prompt();
}

int main(void)
{
#ifdef CONFIG_CPU_HAS_INTERRUPT
	irq_setmask(0);
	irq_setie(1);
#endif
	uart_init();

	help();
	prompt();

	while(1) {
		console_service();
	}

	return 0;
}
