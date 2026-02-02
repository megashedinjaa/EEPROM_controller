// pinos //
#define ADDR0 A0
#define ADDR1 A1
#define ADDR2 A2
#define ADDR3 A3
#define ADDR4 A4
#define ADDR5 A5

#define I0 2
#define I1 3
#define I2 4
#define I3 5
#define I4 6
#define I5 7
#define I6 8
#define I7 9

// ativos ao nível baixo //
#define CE 10
#define OE 11
#define WE 12

// constantes //
#define ADDR_BITS 6
#define DATA_BITS 8

int addressPins[] = {ADDR5, ADDR4, ADDR3, ADDR2, ADDR1, ADDR0};
int dataPins[] = {I7, I6, I5, I4, I3, I2, I1, I0};

void setup() {
  Serial.begin(9600);
  Serial.println();

  for(int i = 0; i < ADDR_BITS; i++) pinMode(addressPins[i], OUTPUT);
  for(int i = 0; i < DATA_BITS; i++) pinMode(dataPins[i], OUTPUT);
  pinMode(CE, OUTPUT);
  pinMode(OE, OUTPUT);
  pinMode(WE, OUTPUT);
  digitalWrite(CE, HIGH);
  digitalWrite(OE, HIGH);
  digitalWrite(WE, HIGH);

}

void loop() {
  writeRom();

  Serial.println("End of loop.");
}

void writeRom() {
  Serial.println("Insert address (decimal).");
  while(!Serial.available());
  int address = Serial.parseInt(), ad = address;
  int binAddr[ADDR_BITS]; ///
  for(int i = 0; i < ADDR_BITS; i++)
  {
    binAddr[ADDR_BITS-i-1] = ad % 2;
    ad /= 2;
  }
  Serial.print("Insert data to write on binary address ");
  for(int i = 0; i < ADDR_BITS; i++) Serial.print(binAddr[i]);
  Serial.println(" (OPR SEL CE).");
  Serial.read();
  while(!Serial.available());
  String data = Serial.readString(); data.trim();
  int binData[DATA_BITS]; ///
  for(int i = 0; i < DATA_BITS; i++)
  {
    debug(data);
    binData[i] = data[i] - 48; // ascii math //
  }
  Serial.print("\nPrinting ");
  for(int i = 0; i < DATA_BITS; i++) Serial.print(binData[i]);
  Serial.print(" on address ");
  for(int i = 0; i < ADDR_BITS; i++) Serial.print(binAddr[i]);

  Serial.println(".\n\nProceed? (y/n)");
  String yn = Serial.readString();
  yn.trim();
  if(yn == "y" || yn == "Y")
  {
    for(int i = 0; i < ADDR_BITS; i++)
    {

    }
  }

}

void debug(String value) {
  Serial.println(value);
}
