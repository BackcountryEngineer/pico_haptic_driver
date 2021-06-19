#include <stdio.h>
#include "drv2605.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c1

bool drv2605_init() {
  i2c_init(I2C_PORT, 100 * 1000);
  gpio_set_function(2, GPIO_FUNC_I2C);
  gpio_set_function(3, GPIO_FUNC_I2C);

  //For some reason i2c doesn't start working until you probe it a few times
  // while (i2c_read_blocking(I2C_PORT, DRV2605_ADDR, NULL, 1, true) == PICO_ERROR_GENERIC);

  //Check chip ID is 3 or 7 (DRV2605 or DRV2605L).
  uint8_t status = read_reg(DRV2605_REG_STATUS);
  uint8_t id = (status >> 5) & 0x07;
  if (id != 3 && id != 7) {
    return false;
  }

  if (!write_reg(DRV2605_REG_MODE, 0x00) || // out of standby
      !write_reg(DRV2605_REG_RTPIN, 0x00) || // no real-time-playback
      !write_reg(DRV2605_REG_WAVESEQ1, 1) || // strong click
      !write_reg(DRV2605_REG_WAVESEQ2, 0) || // end sequence
      !write_reg(DRV2605_REG_OVERDRIVE, 0) || // no overdrive
      !write_reg(DRV2605_REG_SUSTAINPOS, 0) ||
      !write_reg(DRV2605_REG_SUSTAINNEG, 0) ||
      !write_reg(DRV2605_REG_BREAK, 0) ||
      !write_reg(DRV2605_REG_AUDIOMAX, 0x64) || // ERM open loop
      !write_reg(DRV2605_REG_FEEDBACK, read_reg(DRV2605_REG_FEEDBACK) & 0x7F) || // turn off N_ERM_LRA
      !write_reg(DRV2605_REG_CONTROL3, read_reg(DRV2605_REG_CONTROL3) | 0x20)) { // turn on ERM_OPEN_LOOP

    return false;
  }

  return true;
}

void set_waveform(uint8_t slot, uint8_t w) {
  write_reg(DRV2605_REG_WAVESEQ1 + slot, w);
}

void select_library(uint8_t lib) {
  write_reg(DRV2605_REG_LIBRARY, lib);
}

void go() { 
  write_reg(DRV2605_REG_GO, 1); 
}

void stop() { 
  write_reg(DRV2605_REG_GO, 0); 
}

void set_mode(uint8_t mode) {
  write_reg(DRV2605_REG_MODE, mode);
}

void set_realtimeValue(uint8_t rtp) {
  write_reg(DRV2605_REG_RTPIN, rtp);
}

uint8_t read_reg(uint8_t reg) {
  uint8_t read = -1;
  if (i2c_write_blocking(I2C_PORT, DRV2605_ADDR, &reg, 1, true)) {
    if (i2c_read_blocking(I2C_PORT, DRV2605_ADDR, &read, 1, false) == PICO_ERROR_GENERIC) {
      printf("I2C read reg failed, is the device connected properly?\n");
      return read;
    }
  }
  printf("Read %02x from reg %02x\n", read, reg);
  return read;
}

bool write_reg(uint8_t reg, uint8_t val) {
  uint8_t data[2];
  data[1] = reg;
  data[2] = val;
  if (i2c_write_blocking(I2C_PORT, DRV2605_ADDR, data, 2, false) == PICO_ERROR_GENERIC) {
    printf("I2C write failed, is the device connected properly?\n");
    return false;
  }
  printf("Wrote %02x to %02x\n", val, reg);
  return true;
}

void use_ERM() {
  write_reg(DRV2605_REG_FEEDBACK, read_reg(DRV2605_REG_FEEDBACK) & 0x7F);
}

void use_LRA() {
  write_reg(DRV2605_REG_FEEDBACK, read_reg(DRV2605_REG_FEEDBACK) | 0x80);
}