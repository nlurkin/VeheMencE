/*
 *  VeheMencE: a simple library for VME access
 *  Copyright (C) 2019 Universite catholique de Louvain (UCLouvain), Belgium
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "CaenetBridge.h"

CaenetBridge::CaenetBridge(VmeController *cont, uint32_t bridgeAdd, uint8_t interrupt):VmeBoard(cont,bridgeAdd,cvA24_S_DATA,cvD16,true),interrupt_(interrupt) {}

void CaenetBridge::reset() {
  uint16_t data = 0x0000;
  LOG_TRACE("sending reset command " + int_to_hex(baseAddress()+0x6));
  writeData(baseAddress()+0x6,&data);
}

bool CaenetBridge::validStatus() {
  uint16_t data = 0x0000;
  LOG_TRACE("checking the status at " + int_to_hex(baseAddress()+0x2));
  readData(baseAddress()+0x2,&data);
  return !(data&0x1);
}

void CaenetBridge::write(const std::vector<uint32_t>& data) {
  // buffers data
  for(auto d : data) {
    LOG_TRACE("writing data " + int_to_hex(d) + " to " + int_to_hex(baseAddress()));
    writeData(baseAddress(),&d);
    if (!validStatus()) throw CAENETexception(0XFFF0);
  }
  // start transmission
  uint32_t tmp = 0x0000;
  LOG_TRACE("start the transmission");
  writeData(baseAddress()+0x4,&tmp);
  if (!validStatus()) throw CAENETexception(0XFFF0);
}

std::tuple<uint32_t, std::vector<uint32_t> > CaenetBridge::readResponse() {
  uint32_t errorCode = 0;
  std::vector<uint32_t> data;
  uint32_t tmp = 0;
  // first wait for the response
  if (interrupt_) { // wait for interrupt if possible
    LOG_TRACE("waiting for interrupt")
    controller()->IRQWait(1<<(interrupt_-1),500);
    controller()->IRQDisable(1<<(interrupt_-1));
  }
  // polling. First word read is the error code (0 for success)
  do {
    readData(baseAddress(),&errorCode);
  } while (!validStatus());
  LOG_TRACE("valid data received. Will be returned as error code.");
  // then read data
  while (validStatus()) {
    readData(baseAddress(),&tmp);
    LOG_TRACE("received data: " + int_to_hex(tmp));
    data.push_back(tmp);
  }
  // last word was garbage
  data.pop_back();
  return make_tuple(errorCode,data);
}
