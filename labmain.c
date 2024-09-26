/* main.c

   This file written 2024 by Artur Podobas and Pedro Antunes

   For copyright and licensing, see file COPYING */


/* Below functions are external and found in other files. */

extern void print(const char*);
extern void print_dec(unsigned int);
extern void display_string(char*);
extern void time2string(char*,int);
extern void tick(int*);
extern void delay(int);
extern int nextprime( int );

int mytime = 0x5957;
char textstring[] = "text, more text, and even more text!";

/* Below is the function that will be called when an interrupt is triggered. */
void handle_interrupt(unsigned cause) 
{}

/* Add your code here for initializing interrupts. */
void labinit(void)
{}

void set_leds(int led_mask){
  volatile int *leds = (volatile int*) 0x04000000;
  *leds = led_mask & 0x3FF;
}

void set_displays(int display_number, int value){
  volatile int *segment = (volatile int*) 0x04000050;
  switch (display_number){
    case 1:
      *segment = value;
      break;
    case 2:
      segment = (volatile int*) 0x04000060;
      *segment = value;
      break;
    case 3:
      segment = (volatile int*) 0x04000070;
      *segment = value;
      break;
    case 4:
      segment = (volatile int*) 0x04000080;
      *segment = value;
      break;
    case 5:
      segment = (volatile int*) 0x04000090;
      *segment = value;
      break;
    case 6:
      segment = (volatile int*) 0x040000a0;
      *segment = value;
      break;
    default:
      print("Faulty display number, choose a number between 1-6.");
      break;
  }
}

int get_sw(void){
  volatile int *toggles = (volatile int*) 0x04000010;
  int value = *toggles;

  return value;
}

int get_btn(void){
  
}

/* Your code goes into main as well as any needed functions. */
int main() {
  // Call labinit()
  labinit();

  int led_value = 0;
  
  set_displays(2, 25);

  print("The toggles are: ");
  print_dec(get_sw());
  print("\n");

  // Enter a forever loop
  while (1) {
    
    set_leds(led_value);
    if (led_value == 0xF){  
      print("The program is finished.");
      break;
    }

    time2string( textstring, mytime ); // Converts mytime to string
    display_string( textstring ); //Print out the string 'textstring'
    delay( 1000 );          // Delays 1 sec (adjust this value)
    tick( &mytime );     // Ticks the clock once

    led_value++;
  }
}