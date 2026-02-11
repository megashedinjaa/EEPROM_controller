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

int currentMode = 0;

void setup() {
  Serial.begin(9600);
  Serial.println();

  for(int i = 0; i < ADDR_BITS; i++) pinMode(addressPins[i], OUTPUT);
  for(int i = 0; i < DATA_BITS; i++) pinMode(dataPins[i], INPUT);

  pinMode(CE, OUTPUT);
  pinMode(OE, OUTPUT);
  pinMode(WE, OUTPUT);

  digitalWrite(CE, HIGH);
  digitalWrite(OE, HIGH);
  digitalWrite(WE, HIGH);
}

void loop() {
  Serial.println("Insert operation mode (1-READ or 2-WRITE or 3-SELECTOR).");
  Serial.read(); // buffer clear //
  while(!Serial.available());
  String r = Serial.readString(); r.trim();

  if (r == "1" || r == "read" || r == "r" || r == "rd")
  {
    currentMode = 1;
    printMode(currentMode);
    Serial.println("Insert decimal address to read from.");
    while(!Serial.available());
    int address = Serial.parseInt(), ad = address;
    int binAddr[ADDR_BITS]; ///
    for(int i = 0; i < ADDR_BITS; i++)
    {
      binAddr[ADDR_BITS-i-1] = ad % 2;
      ad /= 2;
    }
    printMode(currentMode);
    Serial.print("Reading data from binary address ");
    for(int i = 0; i < ADDR_BITS; i++) Serial.print(binAddr[i]);
    Serial.println("...");

    long readData = readRom(binAddr);

    printMode(currentMode);
    Serial.print("Successfully read ");
    Serial.print(readData);
    Serial.println(" from the specified address.");
  }
  else if (r == "2" || r == "write" || r == "w" || r == "wr")
  {
    currentMode = 2;
    writeRom();
  }
  else if (r == "3" || r == "address" || r == "selector")
  {
    currentMode = 3;
    selector();
  }
  else
  {
    currentMode = 0;
    Serial.print("Error: Mode ");
    Serial.print(r);
    Serial.println(" not recognized.");
  }

  Serial.println("End of loop.\n\n");
}

int writeRom() {
  for(int i = 0; i < DATA_BITS; i++) pinMode(dataPins[i], OUTPUT); // prepares to write //

  printMode(currentMode);
  Serial.println("Insert decimal address to write to.");
  while(!Serial.available());
  int address = Serial.parseInt(), ad = address;
  int binAddr[ADDR_BITS]; ///
  for(int i = 0; i < ADDR_BITS; i++)
  {
    binAddr[ADDR_BITS-i-1] = ad % 2;
    ad /= 2;
  }

  printMode(currentMode);
  Serial.print("Insert binary data to write on binary address ");
  for(int i = 0; i < ADDR_BITS; i++) Serial.print(binAddr[i]);
  Serial.println(" (OPR SEL CE).");
  Serial.read();
  while(!Serial.available());
  String data = Serial.readString(); data.trim();
  int binData[DATA_BITS]; ///
  Serial.print("binData = ");
  for(int i = 0; i < DATA_BITS; i++)
  {
    binData[i] = data[i] - 48; // ascii math //
    Serial.print(binData[i]);
  }
  Serial.println();

  printMode(currentMode);
  Serial.print("\nPrinting ");
  for(int i = 0; i < DATA_BITS; i++) Serial.print(binData[i]);
  Serial.print(" on address ");
  for(int i = 0; i < ADDR_BITS; i++) Serial.print(binAddr[i]);

  Serial.println(".\n\nProceed? (y/n)");
  while(!Serial.available());
  String yn = Serial.readString(); yn.trim();
  if(yn == "y" || yn == "Y")
  {
    printMode(currentMode);
    Serial.println("proceeding...");
    for(int i = 0; i < ADDR_BITS; i++)
      digitalWrite(addressPins[i], binAddr[i] == 1 ? HIGH : LOW); // not just binAddr[i] for safety reasons //
    for(int i = 0; i < DATA_BITS; i++)
      digitalWrite(dataPins[i], binData[i] == 1 ? HIGH : LOW);
    digitalWrite(OE, HIGH);
    digitalWrite(WE, LOW);
    digitalWrite(CE, LOW);
    delay(1);
    digitalWrite(WE, HIGH);
    digitalWrite(CE, HIGH);
    delay(10); // chip needs a bit of time to write //
    for(int i = 0; i < DATA_BITS; i++) pinMode(dataPins[i], INPUT);
    delay(10);

    /// check if writing succeeded ///

    long check = readRom(binAddr);

    if(check == data.toInt())
    {
      printMode(currentMode);
      Serial.println("Success!");
      return 0;
    }
    else
    {
      printMode(currentMode);
      Serial.println("Error: written value does not match checked value!");
      printMode(currentMode);
      Serial.print("Wrote ");
      Serial.print(data);
      Serial.print("; received ");
      Serial.print(check);
      Serial.println("!");
      return -1;
    }

  }
  else
  {
    printMode(currentMode);
    Serial.println("Write operation aborted.");
  }

}

long readRom(int address[]) {
  for(int i = 0; i < DATA_BITS; i++) pinMode(dataPins[i], INPUT); // prepares to read //

  for(int i = 0; i < ADDR_BITS; i++)
    digitalWrite(addressPins[i], address[i] == 1 ? HIGH : LOW);

  digitalWrite(WE, HIGH);
  digitalWrite(OE, LOW);
  digitalWrite(CE, LOW);

  delay(10);

  long readData = 0;

  for(int i = 0; i < DATA_BITS; i++)
  {
    readData *= 10;
    readData += digitalRead(dataPins[i]);
  }

  return readData;
}

void selector() {
  printMode(currentMode);
  Serial.println("Insert decimal address to select.");
  while(Serial.available()) Serial.read();
  while(!Serial.available());
  int address = Serial.parseInt(), ad = address;
  int binAddr[ADDR_BITS]; ///
  for(int i = 0; i < ADDR_BITS; i++)
  {
    binAddr[ADDR_BITS-i-1] = ad % 2;
    ad /= 2;
  }

  Serial.print("Selecting binary address ");
  for(int i = 0; i < ADDR_BITS; i++) Serial.print(binAddr[i]);
  Serial.println(".");

  for(int i = 0; i < DATA_BITS; i++) pinMode(dataPins[i], INPUT); // prepares to read //

  for(int i = 0; i < ADDR_BITS; i++)
    digitalWrite(addressPins[i], binAddr[i] == 1 ? HIGH : LOW);

  digitalWrite(WE, HIGH);
  digitalWrite(OE, LOW);
  digitalWrite(CE, LOW);
  delay(10);
}

void printMode(int mode) {
  Serial.print("<");
  String s = "CONSOLE";
  switch(mode)
  {
    case 0:
    s = "CONSOLE";
    break;

    case 1:
    s = "READ";
    break;

    case 2:
    s = "WRITE";
    break;

    case 3:
    s = "SELECTOR";
    break;

    default:
    s = "MODE " + String(mode);
    break;
  }
  Serial.print(s);
  Serial.print("> ");
}





