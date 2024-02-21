// Here's a summary:
//Detta är Wordle för nummer som ger dig 5 gissningar att gissa rätt kod innan du förlorar och den byter kod. Om du gissar rätt kod på rätt position så printas det nummret på tredje raden, om du har rätt nummer på fel position printar den nummret på tredje raden och stjärna på fjärde och om du har fel nummer, printas det inte. # för att göra sin gissning och 'D' för att ta bort det du skrivit och gissa en ny kod.

#include "Adafruit_Keypad.h" //Inkluderar keypad biblioteket

// define your specific keypad here via PID
#define KEYPAD_PID3844 //Inklluderar keypaden som rätt typ
// define your pins here
// can ignore ones that don't apply
#define R1    2 //Detta sätter rätt sladdar till rätt position på keyboarden och gör att varje nummer på keypaden görs om till en nummer string i koden. vänster är positioner på keypaden och höger är position på arduinon.
#define R2    3
#define R3    4
#define R4    5
#define C1    6
#define C2    7
#define C3    8
#define C4    9
// leave this import after the above configuration
#include "keypad_config.h" //Mer keypad bibliotek
#include "U8glib.h" //drar in oled skärmen


U8GLIB_SSD1306_128X64 oled(U8G_I2C_OPT_NO_ACK); //Skapar rätt typ av oled skärm med rätt mått.

//initialize an instance of class NewKeypad
Adafruit_Keypad customKeypad = Adafruit_Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS); //Gör att custom keypad blir variabel för den riktiga keypaden.


String saved = ""; //Initierar global string variabel. Saved är vad användaren testar som kod.
String lastSaved = "AAAA"; //Initierar global string variabel. Lastsaved är användaren förra svar. 4 bokstäver för att förhindra buggar.
String code = ""; //Initierar global string variabel. Code är den randomizade koden som man ska gissa.
String correctCode = "0000"; //Initierar global string variabel. correctCode är jämnförelsen mellan användarens svar och den riktiga koden. 0 = fel nummer, fel position. 1 = Rätt nummer rätt position. 2 = rätt nummer fel position.
boolean locked = false; //Initierar global boolean variabel. Används för att titta om variabeln 'code' har skapats än.
int numTries = 0; //Initierar global intager variabel. Antal gissningar från användaren.


void setup() { //I setup skapar jag sakerna som är en del av koden men behövs inte vara med mer än i början.
  Serial.begin(9600); //Rätt serial skärm
  customKeypad.begin(); //customKeypad funktion som börjarkeypaden, 
  oled.setFont(u8g_font_helvB10); //Rätt font på oled skärmen.
  randomSeed(analogRead(0)); //Inkluderar random.
}

void loop() { //Min main loop som koden alltid loopar igenom. 


  if (locked == false) { //If sats som tittar om en kod har skapats än. Om inte går den in i makeCode och gör en kod och efter loopar den igen och går in i else satsen.
    Serial.println("test");
    makeCode(); //Funktion som skapar kod.
  }
  else { //Else sats som visar saker på oled skärmen samt låter användaren gissa en kod med keypaden.
    showCode(correctCode); //Displayar för oled skärmen
    getCode(); //Användaren gissar en kod.
  }
}



void getCode() { //En funktion som låter användaren trycka på kaypaden för att välja ett nytt nummer tills man har valt 4. Sedan när man trycker på '#' sparas ditt val och den går vidare i koden.
    
    customKeypad.tick(); //Inbyggd funktion som gör att keypaden alltid checkas för nya intryckta knappar.
    
    while (customKeypad.available() && locked == true) { //När man kan trycka på keypaden och en kod har blivit vald går den in i while loopen.
     
      keypadEvent e = customKeypad.read(); //e blir en variabel för den inbyggda funktionen customKeypad.read som letar efter någon knapp man har tryckt på på keypaden.
      Serial.println((char)e.bit.KEY);

      if (e.bit.EVENT == KEY_JUST_PRESSED) { //If sats: Om en knapp har blivir intryckt.

        if ((String((char)e.bit.KEY)) == "D") { //Om en string verision av knappen är 'D' blir saved = "" eftersom saved är vad användarens gissning sparas som och om den blir = "" betyder det att den blir tom igen och man kan skapa en ny kod.
          saved = "";
        }  
        else if ((String((char)e.bit.KEY)) == "#") { // Annars om man tryckte på '#' sparas koden och man får + 1 försökk och den går in in funktionen saveCode.
          numTries+=1; //Försök += 1
          saveCode(); //Funktion som checkar om koden du gissade på är rätt.
        }
        else if (saved.length() != 4) { //Annars om saved längden inte är 4 så inktementeras stringen saved med nummret som man tryckte på eftersom koden max kan bli 4 karaktärer kommer den aldrig bli mer än 4 nu.
          Serial.println(" pressed");
          saved += ((char)e.bit.KEY);     //Saved += nummret på keypaden,
      }
     }
     delay(50); //Delay så att man inte råkar t.ex dubbelklicka.
   }
}


String getLine3(String correctCode) { //Funktion som returnerar en string och tar in en string som argument som i funktionen kallas för 'correctCode'. Denna funktionen ska printa line3 av oled skärmen beroende på om det var rätt nummer på fel position, rätt nummer på rätt poition eller fel nummer.
  String line3 = ""; //Lokal string som heter line3. Detta är vad som returneras och är vad som ska printas på oled skärmen.
  for (int i=0; i<4; i++) { //For loop som loopar 4 gånger.
    if (correctCode.charAt(i) != '0') { // if sats som tittar om karaktären på correct code vid index i som loopar är = 0. Om det inte är det ska ett nummer läggas till, annars läggs bara mellanslag till för att det ska se snyggt ut på oled skärmen.
      line3 += lastSaved[i]; //line3 += bokstaven på rätt position av din förra gissning när det inte är ett fel nummer.
      line3 += "    "; //+ mellanslag för att det ska se snyggt ut på oled skärmen.
    }
    else { //Annars bara space eftersom det var fel nummer.
      line3 += "      "; //spaces.
    }
  }
  return line3;//Returnerar stringen av nummer och mellanslag som skapades.
}

void showCode(String correctCode) { //Detta printar ut 4 linjer som ändras beroende på gissningar. Den har lite logik för att kunna printa line2 rätt men annars använder den correct code som skickas in. Behöver bara en updateOled rad.

  String line1 = "Gissa koden";// gör det lättare att förstå när jag döper den till line1.
 

  String line2 = ""; //Line 2 är "" i början och ändras i funktionen.
  for (int i=0; i<4; i++) { //for loop som körs 4 gånger
    if ((i+1)<=saved.length()) { //den börjar på 0 och därför gör man om (i+1) <=saved.length vilket kommer ändras eftersom den ska byta ut '_' till nummer medans man skriver på oled skärmen. Den börjar inte på 1 eftersom man villlägga till saved[0] först och loopar inte från 0 till 3 eftersom den bara loopar 3 gånger då. Villkor testen äär därför nödvändbara.
      line2+= saved[i]; //Linje 2 på oled skärmen inkrementeras med vad användaren väljer för nummer.
      line2+= "    "; //Mellanslag för utseende
    }
    else {
      line2+= "_    "; //Om man inte har valt mer än tex 2 nummer ska det fortfarande vara 2 '_' kvar på skärmen.
    }
  }
  String line3 = getLine3(correctCode); //line 3 deklareras och är lika med vad funktionen 'getLine3' returnerar.
  updateOled(line1,"  " + line2, "  " + line3, correctCode);  //Updaterar oled med alla rader som skapats och correctCode vilket skickas in i funktionen.
  
}

void makeCode() { //Funktion som skapar en random code och sedan sätter locked till true eftersom en kod har skapats.
  randomCode();
  locked = true;
}

String randomCode() { //Funktion som skapar och 

  for (int i = 0; i < 4; i++) { //For loop som loopar 4 gånger för strängen 'code' ska ha 4 nummer
    code += String((random(0, 9))); //det läggs till 4 random nummer mellan 0 och 9.
    Serial.println(code);
  }
  
  Serial.println("code");
  Serial.println(code);
  
  return code; //returnerar koden
  
}

void saveCode() { //Funktion som checkar om du har rätt kod, 5 eller mer försök gjort eller inte har rätt kod och gör olika saker i de olika if satserna.

  if (code == saved) { //Om 'code' == 'saved' har användare gissat helt rätt kod och då vinner man.
    updateOled("Ratt Kod", "Antal forsok: " + String(numTries), "  " + String(saved[0]) + "    " + String(saved[1]) + "    " + String(saved[2]) + "    " + String(saved[3]) //Updaterar oled med ett vinst meddelande.
    , "");
    locked = false; //Resetar alla variablar. Eftersom man bara gör detta på 2 ställen tänkte jag det var onödigt att skapa en helt egen funktion för detta.
    code = "";    //reset
    correctCode = "0000"; //reset
    numTries = 0; //Reset
    lastSaved = "AAAA"; //Reset 
    delay(2000); //Delay för vinstmeddelandet.

  }
  else if (numTries > 5) { //Om man får mer än 5 försök förlorar man
    updateOled("Lose", "Lose", String(numTries) + " Tries", "ratt kod: " + String(code)); //Fel meddelande när man förlorar
    locked = false; //Resetar allt här också
    code = "";    
    correctCode = "0000";
    numTries = 0;
    lastSaved = "AAAA";
    delay(2000);

  }
  else { //Om man inte har rätt kod och använt mindre än 5 försök startar checken av din kod emot den riktiga koden.
    lastSaved = saved; //LastSaved ändras inte på något annat ställe än här och därför gör man det till saved för att komma ihåg den förra gissningen.
    correctCode = checkCode(); //'correctCode' är vad som returneras vid funktionen 'checkCode'
    showCode(correctCode); //Showcode eftersom det ska printa nya värden på skärmen efter en gissning för att se hur rätt man har.
  }

  Serial.println("locked");
  Serial.println(locked);
  //correctCode = saved;
  saved = ""; //Saved = "" eftersom man efter denna funktionen går tillbaka till loopen och ska skapa en ny gissning.

  delay(1000);
}
/* Return a string where
// 0 = not found
// 1 = found at correct position
// 2 = found at wrong position */
String checkCode() { //Funktion som tittar om man har rätt eller fel gissning. 0 = fel nummer, 1 = rätt nummer rätt position, 2 = rätt nummer del position
  String tmpCode = "0000"; //Lokal string som ändras till 1 eller 2 om man har rätt gissning.
  String tmpCodePos = ""; //lokal string som
  String empty = " "; //Mellanslag om det inte är rätt nummer gissning.

  for (int i=0; i<4; i++) { //for loop med variabeln i. Loopar 4 gånger
    for (int y=0; y<4; y++) { //for loop med variabel y. Loopar 4 gånger.
      if (lastSaved[i] == code[y]) { //tittar om någon karaktär på någon position av koden är lika med alla 4 positioner av din förra gissning en efter en.
        if (i==y) { //om y och i är samma betyder det att det är på rätt position och då kan position 'i' av tmpCode ändras till 1
          tmpCode.setCharAt(i, '1'); //Funktion som sätter in '1' på rätt position.
          break; //break eftersom en positionen av lastsaved (i) redan är avklarad och kan gå till nästa position av i.
        }
        else { //Om det inte är samma är det rätt nummer på fel position och då kan '2' sättas in på rätt position av i
          if (lastSaved[y] != code[y]) { //Om 'y' positionen av lastSaved och code är samma så har den positionen redan blivit tagen och då kan den skippa den så att samma nummer inte pekar på 2 positioner.
            tmpCode.setCharAt(i, '2'); //Lägger till 2 vid position 'i' av tmpCode.
          }
        }
      }
    }
  }
  //Serial.print(tmpCode);
  return tmpCode; //Returnerar tmpCode vilket nu är en string av 0, 1 eller 2 och har en längd av 4
}


void updateOled(String text1, String text2, String text3, String text4) { // printar värderna på oled skärmen.
  oled.firstPage(); //Innbyggd funktion som gör redo skärmen för printade värden.
  do {
    oled.drawStr(0, 15, text1.c_str()); //Skriver rad 1 på rätt x och y position
    oled.drawStr(0, 35, text2.c_str()); //Skriver rad 2 på rätt x och y position
    oled.drawStr(0, 55, text3.c_str()); //Skriver rad 3 på rätt x och y position
    if (text4[0] == '2') { //Om rad fyras första position är 2 ska den printa nummret med en stjärna under eftersom användaren ska veta att det är rätt nummer på fel position, Samma med resten av alla positioner.
      oled.drawStr(9, 68, "*"); //Printar på rätt position
    }
    if (text4[1] == '2') {
      oled.drawStr(33, 68, "*");
    }
    if (text4[2] == '2') {
      oled.drawStr(57, 68, "*");
    }
    if (text4[3] == '2') {
      oled.drawStr(80, 68, "*");
    }
  } while (oled.nextPage()); //Clearar skärmen
}
