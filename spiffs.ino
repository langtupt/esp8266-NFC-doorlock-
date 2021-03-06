
bool openspiffs(){

    datafile = SPIFFS.open(carddatafile, "r+b");        // exsisting file opened
    if (!datafile){
          datafile = SPIFFS.open(carddatafile, "w+b");  // create new file
    }
  return datafile;
}

void closespiffs(){

    if (datafile){
          datafile.close();
    }
}


void writestringdata(uint16_t whatbyte, int howlong, String databuffer){
    byte s[maxnamelength+1];
    datafile.seek(DATASTART+whatbyte, SeekSet);
    
    databuffer.getBytes(s, maxnamelength);     
    datafile.write(s, howlong);
    datafile.flush();
}

String readstringdata(uint16_t whatbyte, int howlong){
  char s[maxnamelength+1];
  byte s2[maxnamelength+1];
  
  datafile.seek(DATASTART+whatbyte, SeekSet);
  datafile.read(s2, howlong);
  for ( int i = 0; i < howlong; i++)
    s[i] = char(s2[i]);
  return String(s);
}


uint16_t read16data(uint16_t whatbyte){
  uint16_t out;
  uint8_t high;
  uint8_t low;
  
  datafile.seek(DATASTART+whatbyte, SeekSet);
  datafile.read(&high, 1);
  datafile.read(&low, 1);

  out = high << 8;
  out += low;
  return out;
 }

void write16data(uint16_t whatbyte, uint16_t databuffer){
  uint8_t high;
  uint8_t low;
  high = highByte(databuffer);
  low = lowByte(databuffer);
  datafile.seek(DATASTART+whatbyte, SeekSet);
  datafile.write(&high, 1);
  datafile.write(&low, 1);  
  datafile.flush();

 }
 
void getdatafromfile(){
  if (openspiffs()){
  rowIndex = read16data(0);                                                   //read the card count from fram 
  uint16_t buffer_offset = recordlength;                                    //start at record 1
    Blynk.virtualWrite(vcount, rowIndex);       // write the rowIndex to the server
    
  for (int i = 1; i <= rowIndex; i++){                                             // read all the card data
//  read16data(buffer_offset, tempindex);                                       // skip the index
    buffer_offset += 2;
    cardHolder[i] = readstringdata(buffer_offset, maxnamelength);
    buffer_offset += maxnamelength;
    cardId[i] = readstringdata(buffer_offset, maxuidlength);
    buffer_offset += maxuidlength;
    accessFlags[i] = read16data(buffer_offset);
    buffer_offset += 2;
    yield();
  }
  closespiffs();
  }
}

void dumpdatafromfile(){
  String ch;
  String cId;
  int af;
  int ri;
  int tempIndex;
  uint16_t buffer_offset = recordlength;                                    //start at record 1

    if ( openspiffs()){
    tempIndex = read16data(0);                                                   //read the card count from fram address 0
    terminal.println("index "+String(tempIndex));
//  for (int i = 1; i <= tempIndex; i++){                                             // read all the card data
    ri = read16data(buffer_offset);
    buffer_offset += 2;
    ch = readstringdata(buffer_offset, maxnamelength);
    buffer_offset += maxnamelength;
    cId = readstringdata(buffer_offset, maxuidlength);
    buffer_offset += maxuidlength;
    af = read16data(buffer_offset);
    buffer_offset += 2;
    terminal.println("index: " + String(ri) + " holder:  " + ch + " ID:  " + cId + " flags: " + String(af));
    yield();
//  }
  terminal.flush();
  closespiffs();
    }
}


