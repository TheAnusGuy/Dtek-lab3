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
int seconds = 0;
int minutes = 0;
int hours = 0;
int segment_number_values[] = {64, 121, 36, 48, 25, 18, 2, 120, 0, 16};
int timeoutcount = 0;

/* Below is the function that will be called when an interrupt is triggered. */
void handle_interrupt(unsigned cause) 
{}

/* Add your code here for initializing interrupts. */
// 100 ms = 0.1 s.
// 30 MHz = 30,000,000 Hz.
// Period = 30,000,000 * 0.1 = 3,000,000 cycles.
void labinit(void){
  volatile int *timer_status = (volatile int*) 0x04000020;
  volatile int *timer_control = (volatile int*) 0x04000024;
  volatile int *timer_periodl = (volatile int*) 0x04000028;
  volatile int *timer_periodh = (volatile int*) 0x0400002C;

   // Set timer period 3,000,000 cycles = 0x2DC6C0 in hexadecimal
    *timer_periodl = 0xC6C0;   // Set the lower half of the period 
    *timer_periodh = 0x2D;        // Set the upper half of the period 

    *timer_control = 0x4;        // Set only the START bit to 1 (binary 0100)
    *timer_status = 0;
}

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
  volatile int *button = (volatile int*) 0x040000d0;
  int value = *button;

  return value;
}

void update_time(){
    seconds++;

    if (seconds == 60){
        seconds = 0;
        minutes++;
    }

    if (minutes == 60){
        minutes = 0;
        hours++;
    }

    if (hours == 24){
        hours = 0;
    }
}

void digit_converter(int display, int digit){
    if (digit >= 0 && digit <= 9) {
        int value = segment_number_values[digit];  // Get the corresponding value from the array
        set_displays(display, value);       // Set the value on the correct display
    }
}

void display_time(){

  int hours_1D = hours / 10;  
  int hours_2D = hours % 10;  
  digit_converter(6, hours_1D);  
  digit_converter(5, hours_2D);  

  int minutes_1D = minutes / 10;  
  int minutes_2D = minutes % 10;  
  digit_converter(4, minutes_1D);    
  digit_converter(3, minutes_2D);    

  int seconds_1D = seconds / 10;  
  int seconds_2D = seconds % 10;  
  digit_converter(2, seconds_1D);    
  digit_converter(1, seconds_2D);    
}

/* Your code goes into main as well as any needed functions. */
int main() {
  // Call labinit()
  labinit();

  print("The toggles are: ");
  print_dec(get_sw());
  print("\n");

  print("The buttons status is: ");
  print_dec(get_btn());
  print("\n");

  for (int led_value = 0; led_value <= 15; led_value++){
    set_leds(led_value); 
    delay( 1000 );
    }
    print("The LEDs have reached their goal are finished. \n");

  // Enter a forever loop
  while (1) {

    volatile int *timer_status = (volatile int *) 0x04000020;
    volatile int *timer_control = (volatile int*) 0x04000024;

    int button_value = get_btn();
    if (button_value != 0){
      int switch_value = get_sw();
      int mostimportant2 = (switch_value >> 8) & 0x3;
      int leastimportant6 = switch_value & 0x3F;

      switch (mostimportant2){
      case 0b01:
        seconds = leastimportant6;
        break;
      case 0b10:
        minutes = leastimportant6;
        break;
        case 0b11:
        hours = leastimportant6;
        break;
      default:
        break;
      }
    }
    
    if (*timer_status & 0x1){
      *timer_status = 0x0;
      *timer_control = 0x4;

      timeoutcount++;

      if (timeoutcount % 10 == 0){
        display_time();
        update_time();

        time2string( textstring, mytime ); // Converts mytime to string
        display_string( textstring ); //Print out the string 'textstring'
        tick( &mytime );     // Ticks the clock once
      }
    }
  }
}