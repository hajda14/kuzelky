#include <SPI.h>
#include <UTFT.h>
#include <SD.h>
#include <pitches.h>
#include <Ubuntu.c>
#include <arial_bold.c>
/*Touch IO*/
#define DCLK     6
#define CS       5  
#define DIN      4 
#define DOUT     3
#define IRQ      2 
boolean endgame = false;
const char* screen = "splash";
const char* oldtextbox = "";
char* textboxtext;
int nastole = 0;
int log1 = A6;
int kuzelka1 = A9;
int kuzelka2 = A7;
int kuzelka3 = A10;
int kuzelka4 = A12;
int kuzelka5 = A8;
int kuzelka6 = A11;
int kuzelka7 = A13;
int kuzelka8 = A15;
int kuzelka9 = A14;
int vyherce = 0;
int vyherce2 = 0;
int vyherce3 = 0;
int kolo =-1;
const char* hraci[] = {"hrac1","Legie","hrac3","hrac4","hrac5","hrac6","hrac7","hrac8","hrac9","hrac10"}; 
int hrac[10][10] = {{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0}};
int pozicepismakol=0;
int posun = 240;
int ulozenejposun = 0;
int aktualnihrac = 1;
int tlac = 11;
int Dlog1 = 12;
int soucet = 0;
int minulejsoucet =0;
int pocethracu=1;
int velikosttabulky=0;
boolean lockkuzelka1 =false;
boolean lockkuzelka2 =false;
boolean lockkuzelka3 =false;
boolean lockkuzelka4 =false;
boolean lockkuzelka5 =false;
boolean lockkuzelka6 =false;
boolean lockkuzelka7 =false;
boolean lockkuzelka8 =false;
boolean lockkuzelka9 =false;

char buf[12]; 
char buf1[12];
char bufL[12];

/* touch*/
struct pix_
{
	unsigned int x;
	unsigned int y;
};
struct pix_ Tp_pix;	 //当前触控坐标的取样值  

void spistar()           //SPI Start
{
  digitalWrite(CS,HIGH);
  digitalWrite(DCLK,HIGH);
  digitalWrite(DIN,HIGH);
  digitalWrite(DCLK,HIGH);
 
}

void WriteCharTo7843(unsigned char num)          //SPI Write Data
{
  unsigned char count=0;
  unsigned char temp;
  unsigned nop;
  temp=num;
  digitalWrite(DCLK,LOW);
  for(count=0;count<8;count++)
  {
    if(temp&0x80)
      digitalWrite(DIN,HIGH);
    else
      digitalWrite(DIN,LOW);
 
    temp=temp<<1; 
 
    digitalWrite(DCLK,LOW);                
    nop++;
    nop++;
    digitalWrite(DCLK,HIGH);
    nop++;
    nop++;
  }
}
 
//**********************************************************
unsigned int ReadFromCharFrom7843()             //SPI Read Data
{ 
  unsigned nop;
  unsigned char count=0;
  unsigned int Num=0;
  for(count=0;count<12;count++)
  {
    Num<<=1;
    digitalWrite(DCLK,HIGH);//DCLK=1; _nop_();_nop_();_nop_();                
    nop++;
    digitalWrite(DCLK,LOW);//DCLK=0; _nop_();_nop_();_nop_();
    nop++;
    if(digitalRead(DOUT)) Num++;
  }
  return(Num);
}
 
//void AD7843(void) 
struct pix_ AD7843()  
{
  struct pix_ pix;
  digitalWrite(CS,LOW);                    
  WriteCharTo7843(0x90); 
  digitalWrite(DCLK,HIGH);
  digitalWrite(DCLK,LOW); 
  pix.y=ReadFromCharFrom7843();
  WriteCharTo7843(0xD0);   
  digitalWrite(DCLK,HIGH);
  digitalWrite(DCLK,LOW);
  pix.x=ReadFromCharFrom7843();
  digitalWrite(CS,HIGH);
  return pix;
}

 unsigned char Makpix(struct pix_ pix1,struct pix_ pix2) //Filtering
{
    unsigned char l=0;
    int x=pix1.x>pix2.x?pix1.x-pix2.x:pix2.x-pix1.x;
    int y=pix1.y>pix2.y?pix1.y-pix2.y:pix2.y-pix1.y;
    if(x<10&&y<10)	
    {
    	l=1;
    	Tp_pix.x=pix1.x;
    	Tp_pix.y=pix1.y;
    }
    return l;
}

unsigned char Getpix() 
{
	struct pix_ pix1;
	struct pix_ pix2; 
	pix1=AD7843();
	pix2=AD7843();
 	return	Makpix(pix1,pix2);	
}

 /* </touch> */

int melody[] = {
  NOTE_C1, NOTE_E1,NOTE_C1, NOTE_E1, NOTE_F1,0, NOTE_E1, NOTE_D1, NOTE_C1, NOTE_B0};
int noteDurations[] = {
  4, 4, 4, 4,4,4,4,4,4,4};  


const char* znaky[] = {"Q", "W", "E","R","T","Z","U","I","O","P","A","S","D","F","G","H","J","K","L","Y","X","C","V","B","N","M","DEL"}; 

File myFile;
int maxx=480;
int maxy=272;
extern uint8_t Ubuntu[];
extern uint8_t BigFont[];
extern uint8_t SmallFont[];
extern uint8_t arial_bold[];
int k = 45;

UTFT myGLCD(ITDB43,38,39,40,41);   

void setup()
{
  pinMode(kuzelka1, INPUT);
   pinMode(kuzelka2, INPUT);
    pinMode(kuzelka3, INPUT);
     pinMode(kuzelka4, INPUT);;
      pinMode(kuzelka5, INPUT);
       pinMode(kuzelka6, INPUT);
        pinMode(kuzelka7, INPUT);
         pinMode(kuzelka8, INPUT);
          pinMode(kuzelka9, INPUT);
          pinMode(tlac, INPUT);
  pinMode(log1,OUTPUT);
  pinMode(Dlog1,OUTPUT);
  digitalWrite(Dlog1, HIGH);
  digitalWrite(log1, HIGH);
   for(int p=22;p<42;p++)
  {
    pinMode(p,OUTPUT);
  }
  for(int p=2; p<7;p++)
      pinMode(p,OUTPUT);
  pinMode(DOUT,INPUT);
  pinMode(IRQ,INPUT);
  
 spistar();
Serial.begin(9600);
  if (!SD.begin(53)) {
   tone(13, 800,500);
  }
  else
  {
    tone(13, 400,200);
    delay(500);
    tone(13, 400,200);
    myGLCD.setColor(255,0,0);
    myGLCD.print("SD miss",CENTER,1);
   
  }

  randomSeed(analogRead(0));
  //setup Touch

  
  // Setup the LCD
  myGLCD.InitLCD();
  myGLCD.setFont(BigFont);
  int buf[478];
  int x, x2;
  int y, y2;

  myGLCD.clrScr();
  SDBMP(190,30,"SSIER.BMP");
  myGLCD.print("SSIER",CENTER,190);
  myGLCD.print("Roznov pod Radhostem",CENTER,210);
  delay(3000);
  menu();

}
void loop()
{
 waitclick();	
}























void SDBMP(int x,int y,const char* file)
{
  int i,a,b,ix,iy,modra,zelena,cervena,barva,sirka,vyska,data;


  i=b= 0;
  ix=x;
  iy=0;  
  myFile = SD.open(file);
  if (myFile) {
    while (myFile.available()) {
      i++;
      data=myFile.read();
      if(i==18){
        sirka=data;
      }  
      if(i==19){
        sirka=sirka*256+data;
      }


      if(i==22){
        vyska=data;
        iy=vyska-1+y;
      }
      if(i==23){
        vyska=vyska*256+data;
        iy=vyska-1+y;
      }


      if(i>54)
      {
        i--;
        b++;
        if(b==1){
          modra=data;
        }
        else if(b==2){
          zelena=data;
        }
        else
        {
          myGLCD.setColor(data,zelena,modra);
          myGLCD.drawPixel(ix,iy);
          ix++;
          if(ix==sirka+x){
            iy--;
            ix=x;
          }
          //Serial.print(i);
          //Serial.print(". : ");
          //Serial.println(barva);              
          b=0;
        }
      }
      /*
    i++;
       Serial.print(i);
       Serial.print(". : ");
       Serial.println(myFile.read());
       */
    }
    // close the file:
    myFile.close();
  }
}

void menu()
{
  screen = "menu";
  myGLCD.clrScr();
  myGLCD.fillScr(0,100,0);
  myGLCD.drawCircle((maxx/3)*2+50,(maxy/3)*2,25);
  myGLCD.drawLine((maxx/3)*2+50,(maxy/3)*2-25,(maxx/3)*2+50,25);
  myGLCD.drawRoundRect(60,25,(maxx/3)*2+50,30);
  myGLCD.drawRect(60,25,65,(maxy/3)*2+50);
  myGLCD.drawRoundRect(60,(maxy/3)*2+50,(maxx/3)*2+120,(maxy/3)*2+80);
  myGLCD.setBackColor(60,100,0);
  myGLCD.drawLine(350,229,362,229);
  myGLCD.drawLine(350,229,350,210);
  myGLCD.drawLine(362,229,362,210);
  myGLCD.drawLine(350,210,353,206);
  myGLCD.drawLine(362,210,359,206);
  myGLCD.drawCircle(356,197,6);
  myGLCD.setColor(0,0,0);
  myGLCD.print("!! Kuzelky !!",CENTER,6);
  button(maxx/2-80,60,160,30,"HRA");
  button(maxx/2-80,110,160,30,"nastav");
  button(maxx/2-80,160,160,30,"skore");



 

}


void oneplayer(const char* text)
{
  screen="oneplayer";
  myGLCD.clrScr();
  myGLCD.fillScr(0,100,0);
  
myGLCD.setColor(225,225,225);
myGLCD.fillRoundRect(maxx/2+90,55,maxx/2+120,145); 
myGLCD.setColor(0,0,0);
myGLCD.drawRoundRect(maxx/2+90,55,maxx/2+120,145);
myGLCD.drawRoundRect(maxx/2+89,54,maxx/2+121,146);
myGLCD.setColor(25,25,220);
myGLCD.fillRoundRect(maxx/2+90,55,maxx/2+120,85);                              myGLCD.fillRoundRect(maxx/2+90,115,maxx/2+120,145);
myGLCD.setColor(0,0,0);
myGLCD.drawRoundRect(maxx/2+90,55,maxx/2+120,85);                              myGLCD.drawRoundRect(maxx/2+90,115,maxx/2+120,145);
myGLCD.fillRect(maxx/2+92,68,maxx/2+118,72);                                   myGLCD.fillRect(maxx/2+92,128,maxx/2+118,132);
myGLCD.fillRect(maxx/2+103,57,maxx/2+107,82);        

itoa(pocethracu,bufL,10);
myGLCD.setBackColor(255,255,255);
myGLCD.print("  ",maxx/2+90,89);
myGLCD.print(bufL,maxx/2+90,89);
myGLCD.drawRoundRect(maxx/2+90,55,maxx/2+120,145);
myGLCD.drawRoundRect(maxx/2+89,54,maxx/2+121,146);

  textbox(maxx/2-80,90,160,20,text);
  button(maxx/4+maxx/4+20,160,80,30,"HRA!");
  button(maxx/4,160,80,30,"BACK");


  


}

void oneplayerkey()
{
  screen="oneplayerkey";
  textboxtext = "";
  myGLCD.clrScr();
  myGLCD.fillScr(0,100,0);
  textbox(maxx/2-80,50,160,20,"");
  button(maxx-110,50,80,30,"OK");
  button(30,50,80,30,"BACK");
  keyboard();
 


}

void obnovtabulku()
{

         if (pocethracu==1){velikosttabulky=31+pocethracu*31;posun=maxx-184;posunkol(velikosttabulky+10);}
    else if (pocethracu==2){velikosttabulky=31+pocethracu*31;posun=maxx-184;posunkol(velikosttabulky+10);} 
    else if (pocethracu==3){velikosttabulky=31+pocethracu*31;posun=maxx-184;posunkol(velikosttabulky+10);} 
    else if (pocethracu==4){velikosttabulky=31+pocethracu*31;posun=maxx-184;posunkol(velikosttabulky+10);} 
    else if (pocethracu==5){velikosttabulky=31+pocethracu*31;posun=maxx-velikosttabulky;posunkol(velikosttabulky+10);} 
    else if (pocethracu==6){velikosttabulky=31+pocethracu*31;posun=maxx-velikosttabulky;posunkol(velikosttabulky+10);} 
    else if (pocethracu==7){velikosttabulky=31+pocethracu*31;posun=maxx-velikosttabulky;posunkol(velikosttabulky+10);} 
    else if (pocethracu==8){velikosttabulky=31+pocethracu*31;posun=maxx-velikosttabulky;posunkol(velikosttabulky+10);} 
    else if (pocethracu==9){velikosttabulky=31+pocethracu*31;posun=maxx-velikosttabulky;posunkol(velikosttabulky+10);} 
   else if (pocethracu==10){velikosttabulky=31+pocethracu*31;posun=maxx-velikosttabulky;posunkol(velikosttabulky+10);}

else  {velikosttabulky=78;posunkol(78);posun=maxx-184;pocethracu=1;}
    for(int p=50;p<270;p=p+20)
        {
    myGLCD.drawLine(0,p,velikosttabulky,p);
        }
 
 for (int o=0;o<velikosttabulky+31;o=o+31)
{
myGLCD.drawLine(o,50,o,272);
} 
}

void skore()
{
  screen="skore";
  myGLCD.clrScr();
  myGLCD.fillScr(0,100,0);
  myGLCD.setColor(0,0,255);
  myGLCD.fillRect(maxx/2-60,30,maxx/2+60,maxy/2);
  myGLCD.fillRect(maxx/2-60,80,maxx/2-120,maxy/2);
  button(maxx/2-40,210,80,30,"BACK");
}

void kuzelky (boolean a,boolean b,boolean c,boolean d,boolean e,boolean f,boolean g,boolean h,boolean ch)
{
  int k = posun/6;
  int r= k/2;
if (posun == ulozenejposun)
{
}
else{ 
  ulozenejposun=posun;
  int k = ulozenejposun/6;
  int r= k/2;
  
  myGLCD.setColor(0,0,0);
 myGLCD.drawCircle(maxx-(3*k),(1*k),r );    myGLCD.drawCircle(maxx-(3*k),(1*k),r-1 );   myGLCD.drawCircle(maxx-(3*k),(1*k),r-2 );
 myGLCD.drawCircle(maxx-(4*k),(2*k),r );    myGLCD.drawCircle(maxx-(4*k),(2*k),r-1 );   myGLCD.drawCircle(maxx-(4*k),(2*k),r-2 );
 myGLCD.drawCircle(maxx-(5*k),(3*k),r );    myGLCD.drawCircle(maxx-(5*k),(3*k),r-1 );   myGLCD.drawCircle(maxx-(5*k),(3*k),r-2 );
 myGLCD.drawCircle(maxx-(2*k),(2*k),r );    myGLCD.drawCircle(maxx-(2*k),(2*k),r-1 );   myGLCD.drawCircle(maxx-(2*k),(2*k),r-2 );
 myGLCD.drawCircle(maxx-(3*k),(3*k),r );    myGLCD.drawCircle(maxx-(3*k),(3*k),r-1 );   myGLCD.drawCircle(maxx-(3*k),(3*k),r-2 );
 myGLCD.drawCircle(maxx-(4*k),(4*k),r );    myGLCD.drawCircle(maxx-(4*k),(4*k),r-1 );   myGLCD.drawCircle(maxx-(4*k),(4*k),r-2 );
 myGLCD.drawCircle(maxx-(1*k),(3*k),r );    myGLCD.drawCircle(maxx-(1*k),(3*k),r-1 );   myGLCD.drawCircle(maxx-(1*k),(3*k),r-2 );
 myGLCD.drawCircle(maxx-(2*k),(4*k),r );    myGLCD.drawCircle(maxx-(2*k),(4*k),r-1 );   myGLCD.drawCircle(maxx-(2*k),(4*k),r-2 );
 myGLCD.drawCircle(maxx-(3*k),(5*k),r );    myGLCD.drawCircle(maxx-(3*k),(5*k),r-1 );   myGLCD.drawCircle(maxx-(3*k),(5*k),r-2 );

}
 
  if (a == 1) {myGLCD.setColor(0,0,255);}else if(lockkuzelka1==true){myGLCD.setColor(255,0,0);}     else{myGLCD.setColor(0,200,0);}   myGLCD.fillCircle(maxx-(3*k),(1*k),r-3 );
  if (b == 1) {myGLCD.setColor(0,0,255);}else if(lockkuzelka2==true){myGLCD.setColor(255,0,0);}     else{myGLCD.setColor(0,200,0);}   myGLCD.fillCircle(maxx-(4*k),(2*k),r-3 );
  if (c == 1) {myGLCD.setColor(0,0,255);}else if(lockkuzelka3==true){myGLCD.setColor(255,0,0);}     else{myGLCD.setColor(0,200,0);}   myGLCD.fillCircle(maxx-(5*k),(3*k),r-3 );
  if (d == 1) {myGLCD.setColor(0,0,255);}else if(lockkuzelka4==true){myGLCD.setColor(255,0,0);}     else{myGLCD.setColor(0,200,0);}   myGLCD.fillCircle(maxx-(2*k),(2*k),r-3 );
  if (e == 1) {myGLCD.setColor(0,0,255);}else if(lockkuzelka5==true){myGLCD.setColor(255,0,0);}     else{myGLCD.setColor(0,200,0);}   myGLCD.fillCircle(maxx-(3*k),(3*k),r-3 );
  if (f == 1) {myGLCD.setColor(0,0,255);}else if(lockkuzelka6==true){myGLCD.setColor(255,0,0);}     else{myGLCD.setColor(0,200,0);}   myGLCD.fillCircle(maxx-(4*k),(4*k),r-3 );
  if (g == 1) {myGLCD.setColor(0,0,255);}else if(lockkuzelka7==true){myGLCD.setColor(255,0,0);}     else{myGLCD.setColor(0,200,0);}   myGLCD.fillCircle(maxx-(1*k),(3*k),r-3 );
  if (h == 1) {myGLCD.setColor(0,0,255);}else if(lockkuzelka8==true){myGLCD.setColor(255,0,0);}     else{myGLCD.setColor(0,200,0);}   myGLCD.fillCircle(maxx-(2*k),(4*k),r-3 );
  if (ch== 1) {myGLCD.setColor(0,0,255);}else if(lockkuzelka9==true){myGLCD.setColor(255,0,0);}     else{myGLCD.setColor(0,200,0);}   myGLCD.fillCircle(maxx-(3*k),(5*k),r-3 );
 
}

void posunkol(int x)//160  +33  +66   
{
   myGLCD.setColor(0,0,0);
 myGLCD.drawRoundRect(x,230,x+100,260);

             myGLCD.drawLine(x+33,230,x+33,260);
             myGLCD.drawLine(x+66,230,x+66,260);
                                                    myGLCD.setColor(228,185,80);
             myGLCD.fillRoundRect(x+1,231,x+99,259);
                                                    myGLCD.setColor(0,0,0);
             myGLCD.drawLine(x+33,230,x+33,260);
             myGLCD.drawLine(x+66,230,x+66,260);
             
             pozicepismakol=x+10;
}

void onegame (const char* text)
{
  screen="onegame";
  myGLCD.clrScr();
  myGLCD.fillScr(228,185,80);
  myGLCD.setColor(51,35,7);
  myGLCD.setBackColor(228,185,80);
  myGLCD.setFont(SmallFont); 
  for (int j =31;j<pocethracu*31+62;j=j+31){   myGLCD.drawLine(j,51,j+31,0); }
  int w = -1;
  for (int t = 46;t<pocethracu*31+46;t=t+31)
  {
  w++;  
  myGLCD.print(hraci[w],t,40,-57);
  myGLCD.print(hraci[w],t+1,40,-57);
  myGLCD.print(hraci[w],t+1,39,-57);
  }
  
  myGLCD.setFont(arial_bold);
     obnovtabulku();
            
  myGLCD.print("1",15,60-6,0);
  myGLCD.print("2",15,80-6,0);
  myGLCD.print("3",15,100-6,0);
  myGLCD.print("4",15,120-6,0);
  myGLCD.print("5",15,140-6,0);
  myGLCD.print("6",15,160-6,0);
  myGLCD.print("7",15,180-6,0);
  myGLCD.print("8",15,200-6,0);
  myGLCD.print("9",15,220-6,0);
  myGLCD.print("10",0,240-6,0);
  myGLCD.print("=",10,260-6,0);
  myGLCD.drawLine(31,50,31,272);
 
 
  myGLCD.setBackColor(0,200,0);
  myGLCD.print(" ",43,54,0);
  myGLCD.setBackColor(228,185,80);

  while(endgame==false)
{ 
       if (digitalRead(tlac) ==1)

{
    for(int k=60;k<260;k=k+20)
    {
        if (kolo==9){endgame=true;winscreen();break;}else{kolo++;}
      
             for(int l = 31;l<velikosttabulky;l=l+31)
               { 
         if (pocethracu==1){velikosttabulky=31+pocethracu*31;posun=maxx-184;posunkol(velikosttabulky+10);}
    else if (pocethracu==2){velikosttabulky=31+pocethracu*31;posun=maxx-184;posunkol(velikosttabulky+10);}
    else if (pocethracu==3){velikosttabulky=31+pocethracu*31;posun=maxx-184;posunkol(velikosttabulky+10);} 
    else if (pocethracu==4){velikosttabulky=31+pocethracu*31;posun=maxx-184;posunkol(velikosttabulky+10);} 
    else if (pocethracu==5){velikosttabulky=31+pocethracu*31;posun=maxx-velikosttabulky;posunkol(velikosttabulky+10);} 
    else if (pocethracu==6){velikosttabulky=31+pocethracu*31;posun=maxx-velikosttabulky;posunkol(velikosttabulky+10);} 
    else if (pocethracu==7){velikosttabulky=31+pocethracu*31;posun=maxx-velikosttabulky;posunkol(velikosttabulky+10);} 
    else if (pocethracu==8){velikosttabulky=31+pocethracu*31;posun=maxx-velikosttabulky;posunkol(velikosttabulky+10);} 
    else if (pocethracu==9){velikosttabulky=31+pocethracu*31;posun=maxx-velikosttabulky;posunkol(velikosttabulky+10);} 
   else if (pocethracu==10){velikosttabulky=31+pocethracu*31;posun=maxx-velikosttabulky;posunkol(velikosttabulky+10);} 
                 int soucet_hodu = 0;
                 minulejsoucet =0;
                 nastole = 9;
                     for(int p = 187;p<280;p=p+31)
                     {
                               myGLCD.setBackColor(0,200,0);
                               myGLCD.print(" ",l+3,k-6,0);
                               myGLCD.setBackColor(228,185,80);
                                  while(digitalRead(tlac)==0)
                                  { kuzelky(stavkuzelky(1),stavkuzelky(2),stavkuzelky(3),stavkuzelky(4),stavkuzelky(5),stavkuzelky(6),stavkuzelky(7),stavkuzelky(8),stavkuzelky(9));
}
                                    kuzelky(stavkuzelky(1),stavkuzelky(2),stavkuzelky(3),stavkuzelky(4),stavkuzelky(5),stavkuzelky(6),stavkuzelky(7),stavkuzelky(8),stavkuzelky(9));
                                      soucet = stavkuzelky(1)+stavkuzelky(2)+stavkuzelky(3)+stavkuzelky(4)+stavkuzelky(5)+stavkuzelky(6)+stavkuzelky(7)+stavkuzelky(8)+stavkuzelky(9);
                                      soucet =9-soucet;
                                      if (minulejsoucet ==9){
                                      minulejsoucet=0;
                                      }
                                      soucet = soucet-minulejsoucet;
                                     
                                      for(int c = 1;c<10;c++)
                                      {
                                       if (stavkuzelky(c)==0) {zmenstavkuzelek(true,c);}
                                      }
                                      
                                       if (soucet+minulejsoucet ==9)
                                      {
                                        for(int c = 1;c<10;c++)
                                      {
                                       zmenstavkuzelek(false,c);
                                      }
                                      
                                      }
                                      itoa(soucet,buf,10);
                                      myGLCD.setColor(0,0,0);
                                      myGLCD.print(buf,p+pozicepismakol-180,234,0);  
                                      soucet_hodu = soucet_hodu + soucet;
                                      itoa(soucet_hodu,buf1,10);
                                      minulejsoucet=minulejsoucet+soucet;
                                delay(400);
                                
                              }
                    myGLCD.print(buf1,l,k-6,0); 
                    hrac[aktualnihrac][kolo]=soucet_hodu;
                    char bufp[12];
                    char bufg[12];
                     itoa(hrac[aktualnihrac][kolo],bufp,10);
                     itoa(aktualnihrac,bufg,10);
                     Serial.print(bufg);
                     Serial.print(" : ");
                     Serial.println(bufp);
                     
                    obnovtabulku();
                    zmenahrace(l,k);    
               }
   }
}
 if (endgame=false){ kuzelky(stavkuzelky(1),stavkuzelky(2),stavkuzelky(3),stavkuzelky(4),stavkuzelky(5),stavkuzelky(6),stavkuzelky(7),stavkuzelky(8),stavkuzelky(9));}
 else{}
} 

}

void zmenahrace(int l, int k)
{
                                      for(int c = 1;c<10;c++)
                                      {
                                       zmenstavkuzelek(false,c);
                                      }
                                    /*  while(stavkuzelky(1)+stavkuzelky(2)+stavkuzelky(3)+stavkuzelky(4)+stavkuzelky(5)+stavkuzelky(6)+stavkuzelky(7)+stavkuzelky(8)+stavkuzelky(9)!=9)
                                      
                                  { 
                                    kuzelky(stavkuzelky(1),stavkuzelky(2),stavkuzelky(3),stavkuzelky(4),stavkuzelky(5),stavkuzelky(6),stavkuzelky(7),stavkuzelky(8),stavkuzelky(9));
                                    delay(500);
                                    tone(13, 250,500);
                                    kuzelky(1,1,1,1,1,1,1,1,1);
                                    
                                    delay(500);
                              
                                  }
                                  noTone(13);
                                   tone(13, 900,400);
                                   noTone(13);
                                   tone(13, 500,400);   
                                   */
                                   if (aktualnihrac==pocethracu)
                                   {
                                   aktualnihrac=1;
                                   }
                                   else
                                   {
                                     aktualnihrac++;
                                   }
                                   
                                   
                                          
}

int sum(int khrac)
{
  int vysledek=0;
  for (int z=0;z<10;z++)
{
vysledek = vysledek + hrac[khrac][z];
}
return vysledek;
}


int maxi(int u)
{
  int vysledek=0;
   int vysledek2=0;
    int vysledek3=0;
  

        for (int z=0;z<10;z++)
          {
            if (sum(z)>vysledek) {vysledek=sum(z);vyherce=z-1;}
          }
          
        for (int z=0;z<10;z++)
          {
            if (sum(z)>vysledek2 && sum(z)!=vysledek) {vysledek2=sum(z);vyherce2=z-1;}
          }  
        for (int z=0;z<10;z++)
          {
            if (sum(z)>vysledek3 && sum(z)!=vysledek && sum(z)!=vysledek2) {vysledek3=sum(z);vyherce3=z-1;}
          }  
                          
if (u == 1){return vysledek;}
if (u == 2){return vysledek2;}
if (u == 3){return vysledek3;}

}

void nastaveni()
{
myGLCD.clrScr();
myGLCD.fillScr(50,100,80);

}

void winscreen()
{
  char buft[12];
 screen="winscreen";
  myGLCD.clrScr();
  myGLCD.fillScr(0,100,0);
 
 int shrac1 = sum(1); int shrac2 = sum(2); int shrac3 = sum(3); 
 int shrac4 = sum(4); int shrac5 = sum(5); int shrac6 = sum(6); 
 int shrac7 = sum(7); int shrac8 = sum(8); int shrac9 = sum(9); 
 int shrac10 = sum(10); 
 
 
               itoa(shrac1,buft,10);
                     Serial.println(buft);
               itoa(shrac2,buft,10);
                     Serial.println(buft);
               itoa(shrac3,buft,10);
                     Serial.println(buft);
                     
               itoa(maxi(1),buft,10);
                     Serial.print("vyherce: ");
                     Serial.print(hraci[vyherce]);
                     Serial.print(" skore:");
                     Serial.println(buft);
                     
                     myGLCD.setColor(0,0,100);
                     myGLCD.fillRect(maxx/2-60,30,maxx/2+60,maxy/2);
                     myGLCD.fillRect(maxx/2-60,80,maxx/2-180,maxy/2);
                     myGLCD.fillRect(maxx/2+60,108,maxx/2+180,maxy/2);
                     
                     
                     myGLCD.print(hraci[vyherce], maxx/2-(strlen(hraci[vyherce])*16)/2,11,0);
                     myGLCD.print(buft,maxx/2-(strlen(buft)*16)/2,34,0);
                     
                     if (pocethracu>1)  {
                      myGLCD.print(hraci[vyherce2], maxx/2-120-(strlen(hraci[vyherce2])*16)/2,63,0);
                      itoa(maxi(2),buft,10);
                     myGLCD.print(buft,maxx/2-120-(strlen(buft)*16)/2,84,0);
                                        }
                     
                     if (pocethracu>2)  {
                     myGLCD.print(hraci[vyherce3], maxx/2+120-(strlen(hraci[vyherce3])*16)/2,63,0);
                      itoa(maxi(3),buft,10);
                     myGLCD.print(buft,maxx/2+120-(strlen(buft)*16)/2,84,0);
                                         }
  
            while(1)
           {} 

                                          
}




int stavkuzelky(int kuzel)
{
   switch (kuzel) {
case 1:
      if (lockkuzelka1 == true){return 0;}else{return digitalRead(kuzelka1);}
      break;
case 2:
      if (lockkuzelka2 == true){return 0;}else{return digitalRead(kuzelka2);}
      break;
case 3:
      if (lockkuzelka3 == true){return 0;}else{return digitalRead(kuzelka3);}
      break;
case 4:
      if (lockkuzelka4 == true){return 0;}else{return digitalRead(kuzelka4);}
      break;
case 5:
      if (lockkuzelka5 == true){return 0;}else{return digitalRead(kuzelka5);}
      break;
case 6:
      if (lockkuzelka6 == true){return 0;}else{return digitalRead(kuzelka6);}
      break;
case 7:
      if (lockkuzelka7 == true){return 0;}else{return digitalRead(kuzelka7);}
      break;
case 8:
      if (lockkuzelka8 == true){return 0;}else{return digitalRead(kuzelka8);}
      break;
case 9:
      if (lockkuzelka9 == true){return 0;}else{return digitalRead(kuzelka9);}
      break;
}
}

int zmenstavkuzelek(boolean zamkni,int kuzel)
{
   switch (kuzel) {
case 1:
      if (zamkni == true){lockkuzelka1=true;}else{lockkuzelka1=false;}
      break;
case 2:
      if (zamkni == true){lockkuzelka2=true;}else{lockkuzelka2=false;}
      break;
case 3:
      if (zamkni == true){lockkuzelka3=true;}else{lockkuzelka3=false;}
      break;
case 4:
      if (zamkni == true){lockkuzelka4=true;}else{lockkuzelka4=false;}
      break;
case 5:
      if (zamkni == true){lockkuzelka5=true;}else{lockkuzelka5=false;}
      break;
case 6:
      if (zamkni == true){lockkuzelka6=true;}else{lockkuzelka6=false;}
      break;
case 7:
      if (zamkni == true){lockkuzelka7=true;}else{lockkuzelka7=false;}
      break;
case 8:
      if (zamkni == true){lockkuzelka8=true;}else{lockkuzelka8=false;}
      break;
case 9:
      if (zamkni == true){lockkuzelka9=true;}else{lockkuzelka9=false;}
      break;
}

}

void button (int x,int y,int width,int height,const char* text)
{
  myGLCD.setColor(0,0,100);
  myGLCD.fillRoundRect(x,y,x+width,y+height);
  myGLCD.setColor(0,0,0);
  myGLCD.drawRoundRect(x,y,x+width,y+height);
  myGLCD.setBackColor(0,0,100);
  myGLCD.print(text,x+width/5-2,y+1);
}

void textbox (int x,int y,int width,int height,const char* text)
{
  myGLCD.setColor(255,255,255);
  myGLCD.fillRect(x,y,x+width,y+height);
  myGLCD.setColor(0,0,0);
  myGLCD.drawRect(x,y,x+width,y+height);
  myGLCD.setBackColor(255,255,255);
  myGLCD.print(text,x+width/5-2,y+1);
}



char* spoj(const char *s1, const char *s2)                                                          
{                                                                                                                       
    char *s = (char*)malloc(sizeof(char)*(strlen(s1)
                                         +strlen(s2)+1));        char *c = (char*)s1, *cc = s;                                                                                       
    for ( ; *c != 0; ++c, ++cc) *cc = *c;                    
    for (c = (char*)s2; *c != 0; ++c, ++cc) *cc = *c; 
    *cc = 0;                                                                                                            
    return s;
}

const char* getkeyboard(unsigned int lx,unsigned int ly)
{  
const char* znak;  
if ((ly>163)&&(ly<199)) 
{
znak = znaky[((lx-(lx % 48))/48)];
}
else if((ly>198)&&(ly<235))
{
znak = znaky[((lx-(lx % 53))/53)+10];
}
else if(ly>234)
{
znak = znaky[((lx-(lx % 60))/60)+19];
}
delay(120);
return znak;
}

void keyboard ()
{

  myGLCD.setColor(37,40,41);
  myGLCD.fillRect(0,163,479,271);
  myGLCD.setColor(0,0,0);
  myGLCD.drawLine(0,163,479,163);
  myGLCD.drawLine(0,199,479,199);
  myGLCD.drawLine(0,235,479,235);
  for(int i=1;i<10;i++){
    myGLCD.drawLine(i*48,163,i*48,199);
  }
  for(int i=1;i<9;i++){
    myGLCD.drawLine(i*53,199,i*53,235);
  }
  for(int i=1;i<8;i++){
    myGLCD.drawLine(i*60,235,i*60,271);
  }
  myGLCD.setBackColor(37,40,41);
  myGLCD.setColor(255,255,255);  
  for(int i=0;i<10;i++){myGLCD.print(znaky[i],i*48+15,173);}
  for(int i=10;i<19;i++){myGLCD.print(znaky[i],(i-10)*53+17,209);}
  for(int i=19;i<26;i++){myGLCD.print(znaky[i],(i-19)*60+21,245);}
  myGLCD.print(znaky[26],(26-19)*60+11,245);
 /* myGLCD.drawLine(428,253,433,250);
  myGLCD.drawLine(428,253,433,255);
  myGLCD.fillRect(433,251,463,254);
  myGLCD.fillRect(458,240,463,254);*/

}

void waitclick ()
{
 unsigned int lx,ly;
                int pacy=random(0, 7);
		while(digitalRead(IRQ)==0)
		{
			if(Getpix()==1)
			{
				lx=480-((Tp_pix.x-380)*10/69);
				if(lx>479) lx=479;
				ly=272-((Tp_pix.y-600)*10/111);
                                if(ly>270) ly=270; 
                              lx = map(Tp_pix.x, 3887, 173, 0, 479);
                              ly = map(Tp_pix.y, 3692, 213, 0, 271);
                              if(lx>479) lx=479;
                              if(ly>271) ly=271;
                            //  myGLCD.drawCircle(lx,ly,5);
                                // Serial.println(lx);
                                // Serial.println(ly);
                                 
				
	                     
                              
                              if (screen == "menu")
                              {// button(maxx/2-80,110,160,30,"nastav");
                                if ((lx>maxx/2-80)&& (lx<maxx/2-80+160)&& (ly>60)&&(ly<90)){oneplayer("ahoj");}
                                if ((lx>maxx/2-80)&& (lx<maxx/2-80+160)&& (ly>160)&&(ly<190)){skore();}
                                if ((lx>maxx/2-80)&& (lx<maxx/2-80+190)&& (ly>110)&&(ly<140)){nastaveni();}
                              } 
                              if (screen == "oneplayer")
                              //(maxx/2+90,55,maxx/2+120,85) pismo
                              //myGLCD.fillRoundRect(maxx/2+90,55,maxx/2+120,85);+   myGLCD.fillRoundRect(maxx/2+90,115,maxx/2+120,145);-
                              {
                              if ((lx>maxx/2+90)&& (lx<maxx/2+120)&& (ly>55)&&(ly<85)) {if (pocethracu==10){}else{pocethracu++;}itoa(pocethracu,bufL,10);myGLCD.setBackColor(255,255,255);myGLCD.print("  ",maxx/2+90,89);myGLCD.print(bufL,maxx/2+90,89);myGLCD.drawRoundRect(maxx/2+90,55,maxx/2+120,145);myGLCD.drawRoundRect(maxx/2+89,54,maxx/2+121,146);delay(200);}   //+
                              if ((lx>maxx/2+90)&& (lx<maxx/2+120)&& (ly>90)&&(ly<145)){if (pocethracu==1){}else{pocethracu--;}itoa(pocethracu,bufL,10);myGLCD.setBackColor(255,255,255);myGLCD.print("  ",maxx/2+90,89);myGLCD.print(bufL,maxx/2+90,89);myGLCD.drawRoundRect(maxx/2+90,55,maxx/2+120,145);myGLCD.drawRoundRect(maxx/2+89,54,maxx/2+121,146);delay(200);}  //-
                              
                              if ((lx>maxx/4)&& (lx<maxx/4+80)&& (ly>160)&&(ly<190)){menu();}
                              if ((lx>maxx/2-80)&& (lx<maxx/2-80+160)&& (ly>90)&&(ly<110)){oneplayerkey();}
                               // maxx/4+maxx/4+20,160,80,30
                              if ((lx>maxx/4+maxx/4+20)&& (lx<maxx/4+maxx/4+100)&& (ly>160)&&(ly<190)){hraci[0]=textboxtext;onegame(textboxtext);}
                               //
                              }   
                              if (screen == "oneplayerkey")
                               // button(maxx-110,50,80,30,"OK");
                               // button(30,50,80,30,"BACK");
                                                            {
                                if ((lx>maxx-110)&& (lx<maxx-30)&& (ly>50)&&(ly<80)){oneplayer(textboxtext);}
                                
                                if ((lx>30)&& (lx<110)&& (ly>50)&&(ly<80)){oneplayer("");}
                                
                                if ((ly>163)&& (ly<271)){
                                  if (getkeyboard(lx,ly)=="DEL")
                                  {
                                   textboxtext[strlen(textboxtext)-1]='\0';
                                  }
                                  else{
                                  textboxtext = spoj(textboxtext,getkeyboard(lx,ly));
                                  }
                                  textbox(maxx/2-80,50,160,20,textboxtext);
                                }
                              }         
                             
                              if (screen == "skore")
                              {
                                if ((lx>maxx/2-40)&& (lx<maxx/2+40)&& (ly>210)&&(ly<240)){menu();}
                              }                            
		   	}
                   
		}	
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
