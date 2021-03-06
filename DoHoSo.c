// Ten project: Dong ho so hien thi LCD su dung PIC16F877A
// Ngay tao: 15/11/16
// Nguoi tao: HuyenLTM

#include <16f877A.h> //khai bao thu vien PIC
#device *=16 adc=10 //khai bao su dung adc 10 bit
#fuses NOWDT,HS,NOPUT,NOPROTECT,NODEBUG,NOBROWNOUT,NOLVP,NOCPD,NOWRT //cau hinh cho PIC
#use delay(clock=20000000) //khai bao su dung thach anh 20MHz
#define LCD_ENABLE_PIN PIN_D3 // chan E noi voi D3
#define LCD_RS_PIN PIN_D1    // RS noi voi D1
#define LCD_RW_PIN PIN_D2
#define LCD_Data4 PIN_D4    //D4 noi voi D4
#define LCD_Data5 PIN_D5
#define LCD_Data6 PIN_D6
#define LCD_Data7 PIN_D7
#include <LCD.c> //khai bao thu vien LCD
#use i2c(Master,Slow,SDA=PIN_C4,SCL=PIN_C3) //thiet lap i2c o che do Master, toc do truyen du lieu cham, SDA la chan C4, SCL la chan C3
int set=0; //khai bao bien set la so nguyen
int mod=0; //khai bao bien mod la so nguyen
byte second,minute,hour,day,month,year; //khai bao dang so nguyen 8 bit.
   byte BCD2DEC(byte BCD) //ham chuyen tu BCD sang thap phan
{
      byte a; //khai bao bien a la so nguyen 8 bit
      byte b;
      byte DEC;
      a=BCD/16; //chia lay phan nguyen cua ket qua
      b=BCD%16; //chia lay phan du cua ket qua
      DEC=(a*10+b); 
      return DEC; //tra ve ket qua cua ham la gia tri cua DEC
}
   byte DEC2BCD(byte DEC) //ham chuyen tu so thap phan sang BCD
{
      byte a;
      byte b;
      byte BCD;
      a=DEC/10;
      b=DEC%10;
      BCD=(a*16)+b;
      return BCD; //tra ve ket qua cua ham la gia tri cua BCD
}
void display(int8 DEC) //ham hien thi 1 so len LCD
{
     lcd_putc(DEC/10 + 48);
     lcd_putc(DEC%10 + 48);
}
void write_data(byte address,byte data) //ham ghi du lieu vao DS1307
{
   i2c_start(); //tao tin hieu start cho ham
   i2c_write(0xd0);// gui 7 bit dia chi (+ 1 bit R/W=0 - Mode write) den DS1307
   i2c_write(address);// con tro chi den dia chi
   i2c_write(data);// ghi du lieu vao dia chi da chi dinh
   i2c_stop(); //tao tin hieu stop cho ham
}
byte read_data(byte address) //ham doc du lieu tu DS1307
{
   byte data;
   i2c_start();
   i2c_write(0xd0);//gui 7 bit dia chi ( + 1 bit R/W=0 - Mode write) den DS1307
   i2c_write(address);// con tro dua den dia chi
   i2c_stop();
   i2c_start();
   i2c_write(0xd1);// gui 7 bit dia chi (+ 1 bit R/W=1 - Mode read) den DS1307
   data=i2c_read(0); // doc du lieu dong thoi gui not-ACK
   i2c_stop();
   return(data); //tra ve ket qua cua ham la gia tri data.
}
void update_ds1307() //cap nhat thoi gian
{
         write_data(0, DEC2BCD(second)); // cap nhat giay
         delay_ms(5); //tao thoi gian tre
         write_data(1, DEC2BCD(minute)); // cap nhat phut
         delay_ms(5);
         write_data(2, DEC2BCD(hour)); // cap nhat gio
         delay_ms(5);
         write_data(4, DEC2BCD(day)); //cap nhat ngay
         delay_ms(5);
         write_data(5, DEC2BCD(month)); // cap nhat thang
         delay_ms(5);
         write_data(6, DEC2BCD(year)); // cap nhat nam
         delay_ms(5);
} 
#int_RB //ngat khi co bat cu thay doi nao tu chan B4-B7
void set_time()
{
if (input(PIN_B4)==0) // chinh thoi gian
   {
      if (set==0)// bat dau chinh thoi gian
      {
      set=1;
      mod=1;
      }
      else // update DS1307
      {
      set=0;
      mod=0;
  
      }
   }
else if (input(PIN_B5)==0)// thoi gian can chinh
     {
        mod++;
        if (mod==7) mod=1;
       
     }
else if (input(PIN_B6)==0)// tang thoi gian
   {
   if (set==1)
   switch(mod)
     {
        case 1:
          second++;
          if (second==60)
          second=0;
          break;
        case 2:
          minute++;
          if (minute==60)
          minute=0;
          break;
        case 3:
          hour++;
          if (hour==24)
          hour=0;
          break;
        case 4:
           day++;
           if((day==31)&&((month==4)||(month==6)||(month==9)||(month==11)))
              day=1;
           else if((month==2)&&(year%4==0)&&(year==30))
              day=1;
           else if ((month==2)&&(year%4!=0)&&(day==29))
              day=1;
           else if (day==32)
              day=1;
           break;
       case 5:
          month++;
          if (month==13)
          month=1;
          break;
       case 6:
          year++;
          if (year==100)
          year=0;
          break;
     }
   }
else if (input(PIN_B7)==0)// giam thoi gian
    {
      switch(mod)
         {
           case 1:
              if (second==0)
              second=60;
              second--;
              break;
           case 2:
              if (minute==0)
              minute=60;
              minute--;
              break;
           case 3:
              if (hour==0)
              hour=24;
              hour--;
              break;
           case 4:
               day--;
               if((day==0)&&((month==4)||(month==6)||(month==9)||(month==11)))
               day=30;
               else if((month==2)&&(year%4==0)&&(day==0))
               day=29;
               else if ((month==2)&&(year%4!=0)&&(day==0))
               day=28;
               else if (day==0)
               day=31;
               break;
           case 5:
              month--;
              if (month==0)
              month=12;
              break;
           case 6:
              year--;
              if (year==0)
              year=99;
              break;
         }
    }
else return;    
}      
void main()
{
   enable_interrupts(INT_RB); //cho phep ngat khi co bat ki thay doi nao tren chan B4-B7
   enable_interrupts(global); //cho phep ngat toan cuc
   LCD_init();// khoi tao LCD
   LCD_gotoxy(1,1); // chon vi tri hien thi tren LCD
   LCD_putc("Time"); //hien thi noi dung len LCD
   LCD_gotoxy(1,2);
   LCD_putc("Date");
   While (TRUE)
    {
      if (set == 0) // lay thoi gian
       {
          second=BCD2DEC(read_data(0)); 
          delay_us(100);
          minute=BCD2DEC(read_data(1));
          delay_us(100);
          hour=BCD2DEC(read_data(2));
          delay_us(100);
          day=BCD2DEC(read_data(4));
          delay_us(100);
          month=BCD2DEC(read_data(5));
          delay_us(100);
          year=BCD2DEC(read_data(6));
          delay_us(100);
      }
      if (set==1) //hien thi thoi gian da cap nhat
        update_ds1307(); 
        LCD_gotoxy(13,1);
        display(second);
        LCD_gotoxy(10,1);
        display(minute);
        LCD_putc(":");
        LCD_gotoxy(7,1);
        display(hour);
        LCD_putc(":");
        LCD_gotoxy(7,2);
        display(day);
        LCD_putc(":");
        LCD_gotoxy(10,2);
        display(month);
        LCD_putc(":");
        LCD_gotoxy(13,2);
        display(year);
        switch(mod) // hien thi nhap nhay khi chinh thoi gian 
        {
      case 1: //chinh giay
          LCD_gotoxy(13,1);
          LCD_putc("  ");
          delay_ms(1);
          break;
      case 2: // chinh phut
          LCD_gotoxy(10,1);
          LCD_putc("  ");
          delay_ms(1);
          break;
      case 3: // chinh gio
          LCD_gotoxy(7,1);
          LCD_putc("  ");
          delay_ms(1);
          break;
      case 4: // chinh ngay
          LCD_gotoxy(7,2);
          LCD_putc("  ");
          delay_ms(1);
          break;
      case 5: // chinh thang
          LCD_gotoxy(10,2);
          LCD_putc("  ");
          delay_ms(1);
          break;
      case 6: // chinh nam
          LCD_gotoxy(13,2);
          LCD_putc("  ");
          delay_ms(1);
          break;    
      }    
    }
}

