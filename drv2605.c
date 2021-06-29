#include <stdio.h>
#include "drv2605.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c1

bool drv2605_init() {
  i2c_init(I2C_PORT, 100 * 1000);
  gpio_set_function(2, GPIO_FUNC_I2C);
  gpio_set_function(3, GPIO_FUNC_I2C);

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

  select_library(1);
  set_mode(DRV2605_MODE_INTTRIG);

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
      return read;
    }
  }
  return read;
}

bool write_reg(uint8_t reg, uint8_t val) {
  uint8_t data[2];
  data[0] = reg;
  data[1] = val;
  if (i2c_write_blocking(I2C_PORT, DRV2605_ADDR, data, 2, false) == PICO_ERROR_GENERIC) {
    return false;
  }
  return true;
}

void use_ERM() {
  write_reg(DRV2605_REG_FEEDBACK, read_reg(DRV2605_REG_FEEDBACK) & 0x7F);
}

void use_LRA() {
  write_reg(DRV2605_REG_FEEDBACK, read_reg(DRV2605_REG_FEEDBACK) | 0x80);
}

const char* identify_effect(uint8_t effect) {
  if (effect == 1) {
    return ("1 − Strong Click - 100%%");
  }
  if (effect == 2) {
    return ("2 − Strong Click - 60%%");
  }
  if (effect == 3) {
    return ("3 − Strong Click - 30%%");
  }
  if (effect == 4) {
    return ("4 − Sharp Click - 100%%");
  }
  if (effect == 5) {
    return ("5 − Sharp Click - 60%%");
  }
  if (effect == 6) {
    return ("6 − Sharp Click - 30%%");
  }
  if (effect == 7) {
    return ("7 − Soft Bump - 100%%");
  }
  if (effect == 8) {
    return ("8 − Soft Bump - 60%%");
  }
  if (effect == 9) {
    return ("9 − Soft Bump - 30%%");
  }
  if (effect == 10) {
    return ("10 − Double Click - 100%%");
  }
  if (effect == 11) {
    return ("11 − Double Click - 60%%");
  }
  if (effect == 12) {
    return ("12 − Triple Click - 100%%");
  }
  if (effect == 13) {
    return ("13 − Soft Fuzz - 60%%");
  }
  if (effect == 14) {
    return ("14 − Strong Buzz - 100%%");
  }
  if (effect == 15) {
    return ("15 − 750 ms Alert 100%%");
  }
  if (effect == 16) {
    return ("16 − 1000 ms Alert 100%%");
  }
  if (effect == 17) {
    return ("17 − Strong Click 1 - 100%%");
  }
  if (effect == 18) {
    return ("18 − Strong Click 2 - 80%%");
  }
  if (effect == 19) {
    return ("19 − Strong Click 3 - 60%%");
  }
  if (effect == 20) {
    return ("20 − Strong Click 4 - 30%%");
  }
  if (effect == 21) {
    return ("21 − Medium Click 1 - 100%%");
  }
  if (effect == 22) {
    return ("22 − Medium Click 2 - 80%%");
  }
  if (effect == 23) {
    return ("23 − Medium Click 3 - 60%%");
  }
  if (effect == 24) {
    return ("24 − Sharp Tick 1 - 100%%");
  }
  if (effect == 25) {
    return ("25 − Sharp Tick 2 - 80%%");
  }
  if (effect == 26) {
    return ("26 − Sharp Tick 3 – 60%%");
  }
  if (effect == 27) {
    return ("27 − Short Double Click Strong 1 – 100%%");
  }
  if (effect == 28) {
    return ("28 − Short Double Click Strong 2 – 80%%");
  }
  if (effect == 29) {
    return ("29 − Short Double Click Strong 3 – 60%%");
  }
  if (effect == 30) {
    return ("30 − Short Double Click Strong 4 – 30%%");
  }
  if (effect == 31) {
    return ("31 − Short Double Click Medium 1 – 100%%");
  }
  if (effect == 32) {
    return ("32 − Short Double Click Medium 2 – 80%%");
  }
  if (effect == 33) {
    return ("33 − Short Double Click Medium 3 – 60%%");
  }
  if (effect == 34) {
    return ("34 − Short Double Sharp Tick 1 – 100%%");
  }
  if (effect == 35) {
    return ("35 − Short Double Sharp Tick 2 – 80%%");
  }
  if (effect == 36) {
    return ("36 − Short Double Sharp Tick 3 – 60%%");
  }
  if (effect == 37) {
    return ("37 − Long Double Sharp Click Strong 1 – 100%%");
  }
  if (effect == 38) {
    return ("38 − Long Double Sharp Click Strong 2 – 80%%");
  }
  if (effect == 39) {
    return ("39 − Long Double Sharp Click Strong 3 – 60%%");
  }
  if (effect == 40) {
    return ("40 − Long Double Sharp Click Strong 4 – 30%%");
  }
  if (effect == 41) {
    return ("41 − Long Double Sharp Click Medium 1 – 100%%");
  }
  if (effect == 42) {
    return ("42 − Long Double Sharp Click Medium 2 – 80%%");
  }
  if (effect == 43) {
    return ("43 − Long Double Sharp Click Medium 3 – 60%%");
  }
  if (effect == 44) {
    return ("44 − Long Double Sharp Tick 1 – 100%%");
  }
  if (effect == 45) {
    return ("45 − Long Double Sharp Tick 2 – 80%%");
  }
  if (effect == 46) {
    return ("46 − Long Double Sharp Tick 3 – 60%%");
  }
  if (effect == 47) {
    return ("47 − Buzz 1 – 100%%");
  }
  if (effect == 48) {
    return ("48 − Buzz 2 – 80%%");
  }
  if (effect == 49) {
    return ("49 − Buzz 3 – 60%%");
  }
  if (effect == 50) {
    return ("50 − Buzz 4 – 40%%");
  }
  if (effect == 51) {
    return ("51 − Buzz 5 – 20%%");
  }
  if (effect == 52) {
    return ("52 − Pulsing Strong 1 – 100%%");
  }
  if (effect == 53) {
    return ("53 − Pulsing Strong 2 – 60%%");
  }
  if (effect == 54) {
    return ("54 − Pulsing Medium 1 – 100%%");
  }
  if (effect == 55) {
    return ("55 − Pulsing Medium 2 – 60%%");
  }
  if (effect == 56) {
    return ("56 − Pulsing Sharp 1 – 100%%");
  }
  if (effect == 57) {
    return ("57 − Pulsing Sharp 2 – 60%%");
  }
  if (effect == 58) {
    return ("58 − Transition Click 1 – 100%%");
  }
  if (effect == 59) {
    return ("59 − Transition Click 2 – 80%%");
  }
  if (effect == 60) {
    return ("60 − Transition Click 3 – 60%%");
  }
  if (effect == 61) {
    return ("61 − Transition Click 4 – 40%%");
  }
  if (effect == 62) {
    return ("62 − Transition Click 5 – 20%%");
  }
  if (effect == 63) {
    return ("63 − Transition Click 6 – 10%%");
  }
  if (effect == 64) {
    return ("64 − Transition Hum 1 – 100%%");
  }
  if (effect == 65) {
    return ("65 − Transition Hum 2 – 80%%");
  }
  if (effect == 66) {
    return ("66 − Transition Hum 3 – 60%%");
  }
  if (effect == 67) {
    return ("67 − Transition Hum 4 – 40%%");
  }
  if (effect == 68) {
    return ("68 − Transition Hum 5 – 20%%");
  }
  if (effect == 69) {
    return ("69 − Transition Hum 6 – 10%%");
  }
  if (effect == 70) {
    return ("70 − Transition Ramp Down Long Smooth 1 – 100 to 0%%");
  }
  if (effect == 71) {
    return ("71 − Transition Ramp Down Long Smooth 2 – 100 to 0%%");
  }
  if (effect == 72) {
    return ("72 − Transition Ramp Down Medium Smooth 1 – 100 to 0%%");
  }
  if (effect == 73) {
    return ("73 − Transition Ramp Down Medium Smooth 2 – 100 to 0%%");
  }
  if (effect == 74) {
    return ("74 − Transition Ramp Down Short Smooth 1 – 100 to 0%%");
  }
  if (effect == 75) {
    return ("75 − Transition Ramp Down Short Smooth 2 – 100 to 0%%");
  }
  if (effect == 76) {
    return ("76 − Transition Ramp Down Long Sharp 1 – 100 to 0%%");
  }
  if (effect == 77) {
    return ("77 − Transition Ramp Down Long Sharp 2 – 100 to 0%%");
  }
  if (effect == 78) {
    return ("78 − Transition Ramp Down Medium Sharp 1 – 100 to 0%%");
  }
  if (effect == 79) {
    return ("79 − Transition Ramp Down Medium Sharp 2 – 100 to 0%%");
  }
  if (effect == 80) {
    return ("80 − Transition Ramp Down Short Sharp 1 – 100 to 0%%");
  }
  if (effect == 81) {
    return ("81 − Transition Ramp Down Short Sharp 2 – 100 to 0%%");
  }
  if (effect == 82) {
    return ("82 − Transition Ramp Up Long Smooth 1 – 0 to 100%%");
  }
  if (effect == 83) {
    return ("83 − Transition Ramp Up Long Smooth 2 – 0 to 100%%");
  }
  if (effect == 84) {
    return ("84 − Transition Ramp Up Medium Smooth 1 – 0 to 100%%");
  }
  if (effect == 85) {
    return ("85 − Transition Ramp Up Medium Smooth 2 – 0 to 100%%");
  }
  if (effect == 86) {
    return ("86 − Transition Ramp Up Short Smooth 1 – 0 to 100%%");
  }
  if (effect == 87) {
    return ("87 − Transition Ramp Up Short Smooth 2 – 0 to 100%%");
  }
  if (effect == 88) {
    return ("88 − Transition Ramp Up Long Sharp 1 – 0 to 100%%");
  }
  if (effect == 89) {
    return ("89 − Transition Ramp Up Long Sharp 2 – 0 to 100%%");
  }
  if (effect == 90) {
    return ("90 − Transition Ramp Up Medium Sharp 1 – 0 to 100%%");
  }
  if (effect == 91) {
    return ("91 − Transition Ramp Up Medium Sharp 2 – 0 to 100%%");
  }
  if (effect == 92) {
    return ("92 − Transition Ramp Up Short Sharp 1 – 0 to 100%%");
  }
  if (effect == 93) {
    return ("93 − Transition Ramp Up Short Sharp 2 – 0 to 100%%");
  }
  if (effect == 94) {
    return ("94 − Transition Ramp Down Long Smooth 1 – 50 to 0%%");
  }
  if (effect == 95) {
    return ("95 − Transition Ramp Down Long Smooth 2 – 50 to 0%%");
  }
  if (effect == 96) {
    return ("96 − Transition Ramp Down Medium Smooth 1 – 50 to 0%%");
  }
  if (effect == 97) {
    return ("97 − Transition Ramp Down Medium Smooth 2 – 50 to 0%%");
  }
  if (effect == 98) {
    return ("98 − Transition Ramp Down Short Smooth 1 – 50 to 0%%");
  }
  if (effect == 99) {
    return ("99 − Transition Ramp Down Short Smooth 2 – 50 to 0%%");
  }
  if (effect == 100) {
    return ("100 − Transition Ramp Down Long Sharp 1 – 50 to 0%%");
  }
  if (effect == 101) {
    return ("101 − Transition Ramp Down Long Sharp 2 – 50 to 0%%");
  }
  if (effect == 102) {
    return ("102 − Transition Ramp Down Medium Sharp 1 – 50 to 0%%");
  }
  if (effect == 103) {
    return ("103 − Transition Ramp Down Medium Sharp 2 – 50 to 0%%");
  }
  if (effect == 104) {
    return ("104 − Transition Ramp Down Short Sharp 1 – 50 to 0%%");
  }
  if (effect == 105) {
    return ("105 − Transition Ramp Down Short Sharp 2 – 50 to 0%%");
  }
  if (effect == 106) {
    return ("106 − Transition Ramp Up Long Smooth 1 – 0 to 50%%");
  }
  if (effect == 107) {
    return ("107 − Transition Ramp Up Long Smooth 2 – 0 to 50%%");
  }
  if (effect == 108) {
    return ("108 − Transition Ramp Up Medium Smooth 1 – 0 to 50%%");
  }
  if (effect == 109) {
    return ("109 − Transition Ramp Up Medium Smooth 2 – 0 to 50%%");
  }
  if (effect == 110) {
    return ("110 − Transition Ramp Up Short Smooth 1 – 0 to 50%%");
  }
  if (effect == 111) {
    return ("111 − Transition Ramp Up Short Smooth 2 – 0 to 50%%");
  }
  if (effect == 112) {
    return ("112 − Transition Ramp Up Long Sharp 1 – 0 to 50%%");
  }
  if (effect == 113) {
    return ("113 − Transition Ramp Up Long Sharp 2 – 0 to 50%%");
  }
  if (effect == 114) {
    return ("114 − Transition Ramp Up Medium Sharp 1 – 0 to 50%%");
  }
  if (effect == 115) {
    return ("115 − Transition Ramp Up Medium Sharp 2 – 0 to 50%%");
  }
  if (effect == 116) {
    return ("116 − Transition Ramp Up Short Sharp 1 – 0 to 50%%");
  }
  if (effect == 117) {
    return ("117 − Transition Ramp Up Short Sharp 2 – 0 to 50%%");
  }
  if (effect == 118) {
    return ("118 − Long buzz for programmatic stopping – 100%%");
  }
  if (effect == 119) {
    return ("119 − Smooth Hum 1 (No kick or brake pulse) – 50%%");
  }
  if (effect == 120) {
    return ("120 − Smooth Hum 2 (No kick or brake pulse) – 40%%");
  }
  if (effect == 121) {
    return ("121 − Smooth Hum 3 (No kick or brake pulse) – 30%%");
  }
  if (effect == 122) {
    return ("122 − Smooth Hum 4 (No kick or brake pulse) – 20%%");
  }
  if (effect == 123) {
    return ("123 − Smooth Hum 5 (No kick or brake pulse) – 10%%");
  }
}