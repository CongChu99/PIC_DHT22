#include <16F877A.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP                       
#use delay(clock = 20000000)
#include <lcd.c>
#use fast_io(c)
// Connection pin between PIC16F877A and DHT22 sensor
#BIT Data_Pin = 0x06.0                       // Pin mapped to PORTD.0
#BIT Data_Pin_Direction = 0x86.0             // Pin direction mapped to TRISD.0
#use rs232(baud=38400,UART1,parity=N,bits=8)

char message1[] = "Temp = 00.0 C";
char message2[] = "RH   = 00.0 %";
char message3[] = "Tem1 = 00.0 C";
char message4[] = "Tem2 = 00.0 C";
char message5[] = "H1   = 00.0 %";
char message6[] = "H2   = 00.0 %";
short Time_out ;
unsigned int8 T_byte1, T_byte2, RH_byte1, RH_byte2, CheckSum ;
unsigned int16 Temp, RH,T1 = 0,T2 = 300;
unsigned int16 RH1 = 0, RH2 = 500;
int set = 0;
void nut()
{
   if(input(pin_d3)==0)
   {
      while(input(pin_d3)==0)
      {
      }
      set++;
      if(set>5){set=1;}
   }
}
void start_signal(){
  Data_Pin_Direction = 0;              // Configure connection pin as output
  Data_Pin = 0;                        // Connection pin output low
  delay_ms(25);
  Data_Pin = 1;                        // Connection pin output high
  delay_us(30);
  Data_Pin_Direction = 1;              // Configure connection pin as input
}
short check_response(){
  delay_us(40);
  if(!Data_Pin){                     // Read and test if connection pin is low
    delay_us(80);
    if(Data_Pin){                    // Read and test if connection pin is high
      delay_us(50);
      return 1;}
    }
}
unsigned int8 Read_Data(){
  unsigned int8 i, k, _data = 0;     // k is used to count 1 bit reading duration
  if(Time_out)
    break;
  for(i = 0; i < 8; i++){     k = 0;     while(!Data_Pin){ // Wait until pin goes high       k++;       if (k > 100) {Time_out = 1; break;}
      delay_us(1);}
    delay_us(30);
    if(!Data_Pin)
      bit_clear(_data, (7 - i));               // Clear bit (7 - i)
    else{
      bit_set(_data, (7 - i));                 // Set bit (7 - i)
      while(Data_Pin){                         // Wait until pin goes low
      k++;
      if (k > 100) {Time_out = 1; break;}
      delay_us(1);}
    }
  }
  return _data;
}

void DHT22(){
    delay_ms(1000);
    Time_out = 0;
    Start_signal();
    if(check_response()){                    // If there is response from sensor
      RH_byte1 = Read_Data();                 // read RH byte1
      RH_byte2 = Read_Data();                 // read RH byte2
      T_byte1 = Read_Data();                  // read T byte1
      T_byte2 = Read_Data();                  // read T byte2
      Checksum = Read_Data();                 // read checksum
      if(Time_out){                           // If reading takes long time
        lcd_putc('\f');                       // LCD clear
        lcd_gotoxy(5, 1);                     // Go to column 5 row 1
        lcd_putc("Time out!");
      }
      else{
       if(CheckSum == ((RH_Byte1 + RH_Byte2 + T_Byte1 + T_Byte2) & 0xFF)){
        RH = RH_byte1;
        RH = (RH << 8) | RH_byte2;
        Temp = T_byte1;
        Temp = (Temp << 8) | T_byte2;         if (Temp > 0X8000){
         message1[6] = '-';
         Temp = Temp & 0X7FFF; }
        else
         message1[6] = ' ';
        message1[7]  = (Temp / 100) % 10  + 48;
        message1[8]  = (Temp / 10) % 10  + 48;
        message1[10] = Temp % 10  + 48;
        message2[7]  = (RH / 100) % 10 + 48;
        message2[8]  = (RH / 10) % 10 + 48;
        message2[10] = RH % 10 + 48;
        message1[11] = 223;                   // Degree symbol
        lcd_putc('\f');                       // LCD clear    
        lcd_gotoxy(1, 1);                     // Go to column 1 row 1
        printf(lcd_putc,message1);           // Display message1
        lcd_gotoxy(1, 2);                     // Go to column 1 row 2
        printf(lcd_putc, message2);           // Display message2
       }
        else {
          lcd_putc('\f');                     // LCD clear
          lcd_gotoxy(1, 1);                   // Go to column 1 row 1
          lcd_putc("Checksum Error!");
        }
      }
    }
    else {
      lcd_putc('\f');             // LCD clear
      lcd_gotoxy(3, 1);           // Go to column 3 row 1
      lcd_putc("No response");
      lcd_gotoxy(1, 2);           // Go to column 1 row 2
      lcd_putc("from the sensor");
    }
  }


void main(){
  lcd_init();                                 // Initialize LCD module
  lcd_putc('\f');                             // LCD clear 
  lcd_gotoxy(5,1);
   lcd_putc("Nhom 10");


  while(TRUE){
  nut();
      // T2
   message4[7]  = (T2 / 100) % 10  + 48;
   message4[8]  = (T2 / 10) % 10  + 48;
   message4[10] = T2 % 10  + 48;
   message4[11] = 223; 
   // T1
   message3[7]  = (T1 / 100) % 10  + 48;
   message3[8]  = (T1 / 10) % 10  + 48;
   message3[10] = T1 % 10  + 48;
   message3[11] = 223;
   // RH1
   message5[7]  = (RH1 / 100) % 10 + 48;
   message5[8]  = (RH1 / 10) % 10 + 48;
   message5[10] = RH1 % 10 + 48;
   //RH2
   message6[7]  = (RH2 / 100) % 10 + 48;
   message6[8]  = (RH2 / 10) % 10 + 48;
   message6[10] = RH2 % 10 + 48;
   switch(set){
   case 1 :
    {
    do{
      if(input(pin_a0)==0)
      {
         while(input(pin_a0)==0)
         {
         }
         T1=T1+100;
         
      }else if (input(pin_a1)==0)
       {
         while(input(pin_a1)==0)
         {
         }
         T1=T1+10;
       } else if(input(pin_a2)==0)
       {
         while(input(pin_a2)==0)
         {
         }
         T1++;
       }
       else if(input(pin_a3)==0)
      {
         while(input(pin_a3)==0)
         {
         }
         T1=T1-100;
         
      }else if (input(pin_a4)==0)
       {
         while(input(pin_a4)==0)
         {
         }
         T1=T1-10;
       } else if(input(pin_a5)==0)
       {
         while(input(pin_a5)==0)
         {
         }
         T1--;
       }
        lcd_gotoxy(1, 1);                     // Go to column 1 row 1
        printf(lcd_putc,message3);           // Display message1
                lcd_gotoxy(1, 2);                     // Go to column 1 row 1
        printf(lcd_putc,message4);           // Display message1
        }
     while(T1>300);
        break;
    }
    case 2:
    {
    do{
      if(input(pin_a0)==0)
      {
         while(input(pin_a0)==0)
         {
         }
         T2=T2+100;
      }else if (input(pin_a1)==0)
       {
         while(input(pin_a1)==0)
         {
         }
         T2=T2+10;
       } else if(input(pin_a2)==0)
       {
         while(input(pin_a2)==0)
         {
         }
         T2++;
       }
       else if(input(pin_a3)==0)
      {
         while(input(pin_a3)==0)
         {
         }
         T2=T2-100;
         
      }else if (input(pin_a4)==0)
       {
         while(input(pin_a4)==0)
         {
         }
         T2=T2-10;
       } else if(input(pin_a5)==0)
       {
         while(input(pin_a5)==0)
         {
         }
         T2--;
       }
        lcd_gotoxy(1, 1);                     // Go to column 1 row 1
        printf(lcd_putc,message3);           // Display message1
        lcd_gotoxy(1, 2);                     // Go to column 1 row 1
        printf(lcd_putc,message4);           // Display message1
        }while(T2<301);
        break;
    }
    case 3 :
     {
     do{
      if(input(pin_a0)==0)
      {
         while(input(pin_a0)==0)
         {
         }
         RH1=RH1+100;
      }else if (input(pin_a1)==0)
       {
         while(input(pin_a1)==0)
         {
         }
         RH1=RH1+10;
       } else if(input(pin_a2)==0)
       {
         while(input(pin_a2)==0)
         {
         }
         RH1++;
       }
              else if(input(pin_a3)==0)
      {
         while(input(pin_a3)==0)
         {
         }
         RH1=RH1-100;
         
      }else if (input(pin_a4)==0)
       {
         while(input(pin_a4)==0)
         {
         }
         RH1=RH1-10;
       } else if(input(pin_a5)==0)
       {
         while(input(pin_a5)==0)
         {
         }
         RH1--;
         }
        lcd_gotoxy(1, 1);                     // Go to column 1 row 1
        printf(lcd_putc,message5);           // Display message1
        lcd_gotoxy(1, 2);                     // Go to column 1 row 1
        printf(lcd_putc,message6);           // Display message1
        }while(RH1>500);
        break;
    }
    case 4 :
        {
    do{
      if(input(pin_a0)==0)
      {
         while(input(pin_a0)==0)
         {
         }
         RH2=RH2+100;
      }else if (input(pin_a1)==0)
       {
         while(input(pin_a1)==0)
         {
         }
         RH2=RH2+10;
       } else if(input(pin_a2)==0)
       {
         while(input(pin_a2)==0)
         {
         }
         RH2++;
       }
       else if(input(pin_a3)==0)
      {
         while(input(pin_a3)==0)
         {
         }
         RH2=RH2-100;
         
      }else if (input(pin_a4)==0)
       {
         while(input(pin_a4)==0)
         {
         }
         RH2=RH2-10;
       } else if(input(pin_a5)==0)
       {
         while(input(pin_a5)==0)
         {
         }
         RH2--;
       }
        lcd_gotoxy(1, 1);                     // Go to column 1 row 1
        printf(lcd_putc,message5);           // Display message1
        lcd_gotoxy(1, 2);                     // Go to column 1 row 1
        printf(lcd_putc,message6);           // Display message1
        }while(RH2<501);
        break;        
    }
    case 5 :
    {
      DHT22();
      puts(message1);
      puts(message2);
      if(RH<RH1)
      {
        output_high(pin_b3);
        output_low(pin_b4);
      }
      else if(RH>RH2)
      {
        output_high(pin_b4);
        output_low(pin_b3);
      }
      else 
      {
         output_low(pin_b4);
         output_low(pin_b3);
      }
      if(T1>Temp)
      {
         output_high(pin_b1);
         output_low(pin_b2);
      } else if(Temp>T2)
      {
         output_high(pin_b2);
         output_low(pin_b1);
       } 
       else
       {
         output_low(pin_b1);
         output_low(pin_b2);
       }
    }   
}
}
}
  
