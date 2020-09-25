const int pwm = 13;
long tempo;
int cont = 0;
long RPM = 700;
double aux;
const int fase = 12;
long RPM_ant = 0;
unsigned char vetor[5];
int Qtd = 0;
int i = 0;
void setup() 
{
  pinMode(pwm, OUTPUT);
  pinMode(fase, OUTPUT);
  // put your setup code here, to run once:
  aux = RPM/60;
  aux = 1/aux;
  aux = aux / 120;
  aux = aux * 1000000;
  tempo = aux;
  Serial.begin(9600);
  digitalWrite(pwm, LOW);
  digitalWrite(fase, LOW);
}

void loop() 
{
 
  if(Serial.available())
  {
       
       vetor[i] = Serial.read();
       i++;
       
       if(i == 5)
       {
          RPM = ((vetor[0] - '0')*1000) + ((vetor[1] - '0')*100)+ ((vetor[2] - '0')*10)+ (vetor[3] - '0');
          i = 0;
       }
  }
     /*
    switch(i)
    {
      case 1:
        RPM = vetor[0] - 48;
        break;
      case 2:
        RPM = ((vetor[0] - '0')*10) + (vetor[1] - '0');
        break;
      case 3:
        RPM = ((vetor[0] - '0')*100) + ((vetor[1] - '0')*10)+ (vetor[2] - '0');
        break;
        case 4:
        RPM = ((vetor[0] - '0')*1000) + ((vetor[1] - '0')*100)+ ((vetor[2] - '0')*10)+ (vetor[3] - '0');
        break;
    }
    */

  if(RPM != RPM_ant)
  {
    aux = RPM/60;
    aux = 1/aux;
    aux = aux / 120;
    aux = aux * 1000000;
    tempo = aux;
    RPM_ant = RPM;
  Serial.print("Rotacao: ");
  Serial.println(RPM);
  }
  cont ++;
  if(cont == 59)
    digitalWrite(fase, !fase);
  if(cont < 59)
    digitalWrite(pwm, HIGH);
  delayMicroseconds(tempo);
  digitalWrite(pwm, LOW);
  delayMicroseconds(tempo);
  if(cont == 60)
    cont = 0;
}
