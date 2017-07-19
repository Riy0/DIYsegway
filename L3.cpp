#include "L3.h"

L3GD20::L3GD20(int8_t cs, int8_t miso, int8_t mosi, int8_t clk) {
  _cs = cs;
  _miso = miso;
  _mosi = mosi;
  _clk = clk;
}

L3GD20::L3GD20(void) {
  // user i2c
  _cs = _mosi = _miso = _clk = -1;
}

bool L3GD20::begin(L3GD20Range r, byte addr) {
  if(_cs == -1) {
    Wire.begin();
  } else {
    pinMode(_cs, OUTPUT);
    pinMode(_clk, OUTPUT);
    pinMode(_mosi, OUTPUT);
    pinMode(_miso, INPUT);
    digitalWrite(_cs, HIGH);
  }

  address = addr;
  range = r;

  // Ask if it is L3G20
  uint8_t id = read8(L3GD20_REGISTER_WHO_AM_I);
  if(id != L3GD20_ID) {
    return false;
  }

  // Enabling GYRO
  write8(L3GD20_REGISTER_CTRL_REG1, 0x0F);

  // Adjust resolution range
  switch(range) {
    case L3GD20_RANGE_250DPS:
      write8(L3GD20_REGISTER_CTRL_REG4, 0x00);
      break;
    case L3GD20_RANGE_500DPS:
      write8(L3GD20_REGISTER_CTRL_REG4, 0x10);
      break;
    case L3GD20_RANGE_2000DPS:
      write8(L3GD20_REGISTER_CTRL_REG4, 0x20);
      break;
    default:
      break;
  }

  return true;   
}

void L3GD20::read() {
  uint8_t xhi, xlo, ylo, yhi, zlo, zhi;

  if(_cs == -1) {
    // i2c
    Wire.beginTransmission(address);
    Wire.write(L3GD20_REGISTER_OUT_X_L | 0x80);
    Wire.endTransmission();
    Wire.requestFrom(address, (byte)6);
    while(Wire.available() < 6) ;
    xlo = Wire.read();
    xhi = Wire.read();
    ylo = Wire.read();
    yhi = Wire.read();
    zlo = Wire.read();
    zhi = Wire.read();
  } else {
    // spi
    digitalWrite(_clk, HIGH);
    digitalWrite(_cs, LOW);
    SPIxfer(L3GD20_REGISTER_OUT_X_L | 0x80 | 0x40);
    delay(20);
    xlo = SPIxfer(0xFF);
    xhi = SPIxfer(0xFF);
    ylo = SPIxfer(0xFF);
    yhi = SPIxfer(0xFF);
    zlo = SPIxfer(0xFF);
    zhi = SPIxfer(0xFF);
    digitalWrite(_cs, HIGH);
  }

  data.x = (int16_t)(xlo | (xhi << 8));
  data.y = (int16_t)(ylo | (yhi << 8));
  data.z = (int16_t)(zlo | (zhi << 8));   
  switch(range) {     
    case L3GD20_RANGE_250DPS:       
      data.x *= L3GD20_SENSITIVITY_250DPS;       
      data.y *= L3GD20_SENSITIVITY_250DPS;       
      data.z *= L3GD20_SENSITIVITY_250DPS;       
      break;     
    case L3GD20_RANGE_500DPS:       
      data.x *= L3GD20_SENSITIVITY_500DPS;       
      data.y *= L3GD20_SENSITIVITY_500DPS;       
      data.z *= L3GD20_SENSITIVITY_500DPS;       
      break;     
    case L3GD20_RANGE_2000DPS:       
      data.x *= L3GD20_SENSITIVITY_2000DPS;       
      data.y *= L3GD20_SENSITIVITY_2000DPS;       
      data.z *= L3GD20_SENSITIVITY_2000DPS;       
      break;   
  } 
} 

void L3GD20::write8(L3GD20Register reg, byte value) {   
  if(_cs == -1) {     
    // i2c
    Wire.beginTransmission(address);     
    Wire.write((byte)reg);     
    Wire.write(value);     
    Wire.endTransmission();   
  } else {     
    // spi     
    digitalWrite(_clk, HIGH);     
    digitalWrite(_cs, LOW);     
    SPIxfer(reg);     
    SPIxfer(value);     
    digitalWrite(_cs, HIGH);   
  } 
} 

byte L3GD20::read8(L3GD20Register reg) {   
  byte value;   
  if(_cs == -1) {     
    // i2c     
    Wire.beginTransmission(address);     
    Wire.write((byte)reg);     
    Wire.endTransmission();     
    Wire.requestFrom(address, (byte)1);     
    value = Wire.read();     
    Wire.endTransmission();   
  } else {     
    // spi     
    digitalWrite(_clk, HIGH);     
    digitalWrite(_cs, LOW);     
    SPIxfer((uint8_t)reg | 0x80);     
    value = SPIxfer(0xFF);     
    digitalWrite(_cs, HIGH);   
  }   
  return value; 
} 

uint8_t L3GD20::SPIxfer(uint8_t x) {   
  uint8_t value = 0;   
  for(int i = 7; i >= 0; i--) {
    digitalWrite(_clk, LOW);
    if(x & (1<<i)) {
      digitalWrite(_mosi, HIGH);
    } else {
      digitalWrite(_mosi, LOW);
    }
    digitalWrite(_clk, HIGH);
    if(digitalRead(_miso)) {
      value |= (1<<i);
    }
  }
  return value;
}
