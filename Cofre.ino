// Desenvolvido por Gustavo Melo
// Email: gustavo.pereira96@edu.pucrs.br

#include <Key.h>
#include <Keypad.h>

bool portaAberta = false;
int senha = 123456;
bool trancaDestr = false;
bool violado = false;
const int MAX_TENTATIVAS = 3;
int temporizador = 60;
char tecla;
const byte LINHAS = 4;
const byte COLUNAS = 3;
int senhaDigitada = 0;
int vzRestante = 52; //4 em ASCII
bool tranca = false;
bool senhaOK = false;


#define LED_TRANCA_DEST 31
#define SINAL_PORTA_ABERTA 21
#define BUZZER 18
#define LED_PORTA_FECHADA 24

#define RS  8
#define EN  9

#define D7  7
#define D6  6
#define D5  5
#define D4  4

char matriz_teclas[LINHAS][COLUNAS] = 
{
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};


byte PinosLinhas[LINHAS] = {A8, A9, A10, A11};

byte PinosColunas[COLUNAS] = {A12, A13, A14};



Keypad teclado = Keypad( makeKeymap(matriz_teclas), PinosLinhas,
                   PinosColunas, LINHAS, COLUNAS);
void lcd_config()
{

    pinMode(RS, OUTPUT);
    pinMode(EN, OUTPUT);
    pinMode(D7, OUTPUT);
    pinMode(D6, OUTPUT);
    pinMode(D5, OUTPUT);
    pinMode(D4, OUTPUT);
  
  // 0x02
  lcd_write(0,0x02);
  
  // 0x28
    lcd_write(0,0x28);

    // 0x0f
    lcd_write(0,0x0e);
  
    // 0x06
    lcd_write(0,0x06);    
  
    // 0x01
    lcd_write(0,0x01);

}


void lcd_enable()
{
    digitalWrite(EN,0);  
    delay(10);
    digitalWrite(EN,1);
    delay(10);
    digitalWrite(EN,0);  
    delay(10);  
}  

void lcd_write(int tipo, int dado)
{
  int b7, b6, b5, b4, b3, b2, b1, b0 = 0;
  
    if (tipo == 0)
    {
      digitalWrite(RS,0);
    }
    else
    {
      digitalWrite(RS,1);
    }
      
  
    b7 = (0x80 & dado) >> 7;
    b6 = (0x40 & dado) >> 6;
    b5 = (0x20 & dado) >> 5;
    b4 = (0x10 & dado) >> 4;
    b3 = (0x08 & dado) >> 3;
    b2 = (0x04 & dado) >> 2;
    b1 = (0x02 & dado) >> 1;
    b0 = (0x01 & dado) >> 0;
  
  digitalWrite(D7, b7);
  digitalWrite(D6, b6);
  digitalWrite(D5, b5);
  digitalWrite(D4, b4);
  
  lcd_enable();
  
  digitalWrite(D7, b3);
  digitalWrite(D6, b2);
  digitalWrite(D5, b1);
  digitalWrite(D4, b0);
  
  lcd_enable();
  
} 
String readVal()
{
    interrupts(); 
    String myString = "";
    tecla = teclado.getKey();
    while (tecla != '#')
    {
      tecla = teclado.getKey();
      if ((tecla != NO_KEY) && (tecla != '#')) 
      {
          myString.concat(tecla);
          lcd_write(1,'*');
      }
    }
  if(tecla=='#')
  {return(myString);}
 
}
void trocaSenha(bool tranca)
{
    char teclaTrocaSenha = teclado.getKey();
    if(teclaTrocaSenha == '*')
    {
      lcd_write(0,0x01); //limpa tela
      char msgDigiteSenha[] = "Nova senha: ";
      for(int i=0; i<sizeof(msgDigiteSenha)-1; i++)
      {
        lcd_write(1, msgDigiteSenha[i]);
      }
      lcd_write(0, 0xc0);
      senha = readVal().toInt();
      lcd_write(0,0x01);
      char msgSenhaOK[] = "Nova Senha OK";
      for(int i=0; i<sizeof(msgSenhaOK)-1; i++)
      {
        lcd_write(1, msgSenhaOK[i]);
      }
      delay(3000);
      if(tranca)
      {
        trancaDestravada();
      }
      else
      {
        metPortaAberta();
      }
    }
}
void trancaDestravada()
{
    lcd_write(0,0x01); //limpa tela
  trancaDestr = true;
  digitalWrite(LED_TRANCA_DEST, trancaDestr);
    tranca = trancaDestr;
    char msg[] = "Tranca Destr.";
    for(int i=0; i<sizeof(msg)-1; i++)
    {
      lcd_write(1, msg[i]);
    }
  lcd_write(0, 0xc0);
    char msgTrocaSenha[] = "Ap * troca senha";
    for(int i=0; i<sizeof(msgTrocaSenha)-1; i++)
    {
      lcd_write(1, msgTrocaSenha[i]);
    }
}

void metViolado()
{
      digitalWrite(BUZZER, 1);
        lcd_write(0,0x01);
        char msgViolado[] = "Violado! Senha?";
        for(int i=0; i<sizeof(msgViolado)-1; i++)
        {
          lcd_write(1, msgViolado[i]);
        }
}
void metPortaAberta()
{
    portaAberta = digitalRead(SINAL_PORTA_ABERTA);
    digitalWrite(LED_PORTA_FECHADA, portaAberta);
    if(senhaOK != true)
    {
      metViolado();
        lcd_write(0, 0xc0);
        int senhaDesbloqueio = -1;
        senhaDesbloqueio = readVal().toInt();
        if(senhaDesbloqueio != senha)
        {metPortaAberta();}
        trancaDestr = true;
    digitalWrite(LED_TRANCA_DEST, trancaDestr);
        digitalWrite(BUZZER, 0);
        lcd_write(0,0x01);
        char msg[] = "Feche a porta";
        for(int i=0; i<sizeof(msg)-1; i++)
        {
          lcd_write(1, msg[i]);
        }
        while(portaAberta)
        {
          portaAberta = digitalRead(SINAL_PORTA_ABERTA);
          digitalWrite(LED_PORTA_FECHADA, portaAberta);
          delay(10);
          
        }
        loop();
    }
    else
    {
      lcd_write(0,0x01);
      char msgPortaAberta[] = "Porta Aberta";
      for(int i=0; i<sizeof(msgPortaAberta)-1; i++)
      {
        lcd_write(1, msgPortaAberta[i]);
      }
      lcd_write(0, 0xc0);
      char msgTrocaSenha[] = "Ap * troca senha";
      for(int i=0; i<sizeof(msgTrocaSenha)-1; i++)
      {
        lcd_write(1, msgTrocaSenha[i]);
      }
      tranca = false;
    }
}
void senhaCorreta()
{
    vzRestante = 52;
    trancaDestravada();
    while(portaAberta == false)
    {
        trocaSenha(tranca);
        Serial.println(portaAberta);
      portaAberta = digitalRead(SINAL_PORTA_ABERTA);
        digitalWrite(LED_PORTA_FECHADA, portaAberta);
        delay(10);
    }
  //metPortaAberta();
    portaAberta = digitalRead(SINAL_PORTA_ABERTA);
    while(portaAberta == true)
    {
        trocaSenha(tranca);
    portaAberta = digitalRead(SINAL_PORTA_ABERTA);
      delay(10);
    }
    trancaDestr = false;
    senhaOK = false;
}
void senhaIncorreta()
{
    lcd_write(0,0x01); //limpa tela
    trancaDestr = false;
  digitalWrite(LED_TRANCA_DEST, trancaDestr);
    char msg[] = "Senha Incorreta";
    for(int i=0; i<sizeof(msg)-1; i++)
    {
      lcd_write(1, msg[i]);
    }
    lcd_write(0, 0xc0);
    vzRestante--;
    if(vzRestante == 48) //0 em ASCII
    {
      int timer = 53; //5 em ASCII
      lcd_write(0,0x01);
      char msgBloqueado[] = "Bloqueado por";
      for(int i=0; i<sizeof(msgBloqueado)-1; i++)
      {
        lcd_write(1, msgBloqueado[i]);
      }
      lcd_write(0, 0xc0);
      do{
        lcd_write(1, timer);
        char msgSegundos[] = " Segundos";
        for(int i=0; i<sizeof(msgSegundos)-1; i++)
        {
          lcd_write(1, msgSegundos[i]);
        }
        lcd_write(0, 0xc0);
        delay(1000);
        timer--;
      }while(timer > 48);
      vzRestante = 52;
    }
    else
    {
      char msgVzRestante[] = "Vz Restante: ";
      for(int i=0; i<sizeof(msgVzRestante)-1; i++)
      {
        lcd_write(1, msgVzRestante[i]);
      }
      lcd_write(1, vzRestante);
      delay(5000);
    }
}

void msgPadrao(){
    digitalWrite(LED_PORTA_FECHADA, portaAberta);
    lcd_write(0,0x01); //limpa tela
  trancaDestr = false;
  digitalWrite(LED_TRANCA_DEST, trancaDestr);
    char msg[] = "Aguarda Senha:";
    for(int i=0; i<sizeof(msg)-1; i++)
    {
      lcd_write(1, msg[i]);
    }
      lcd_write(0, 0xc0);//desloca cursor baixo primeira coluna
    
}

void funcaoInterrup()
{
  metPortaAberta();
}

void setup()
{
    Serial.begin(9600);
    lcd_config();
    pinMode(LED_TRANCA_DEST, OUTPUT);
    pinMode(SINAL_PORTA_ABERTA, INPUT);
    attachInterrupt(digitalPinToInterrupt(SINAL_PORTA_ABERTA), funcaoInterrup, RISING);
    pinMode(BUZZER, OUTPUT);
    pinMode(LED_PORTA_FECHADA, OUTPUT);
}

void loop()
{
  msgPadrao();
  senhaDigitada = readVal().toInt();
  if(senhaDigitada == senha)
  {
    senhaOK = true;
  senhaCorreta();
  }
  else
  {
  senhaIncorreta();
    loop();
  }
}
