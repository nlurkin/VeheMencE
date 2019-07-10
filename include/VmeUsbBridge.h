#ifndef __UsbVmeBridge
#define __UsbVmeBridge

#include <iostream>
#include <exception>

#include "CAENVMElib.h"
#include "CAENVMEoslib.h"
#include "CAENVMEtypes.h"

#include "VmeController.h"

/**
 * \brief Definition of a USB/VME controller.
 * 
 * This class implements the virtual functions of a VmeController object.
 * 
 * It has been built in order to use a V1718 bridge.
 * 
 */

class V1718Pulser
{
  public:
    V1718Pulser(int32_t handle, CVPulserSelect id);
    ~V1718Pulser() {}

    void setPeriod(unsigned char period) { this->period_ = period; this->configured_ = false; }
    unsigned char getPeriod() const { return this->period_; }
    void setWidth(unsigned char width) {this->width_ = width; this->configured_ = false; }
    unsigned char  getWidth() const { return this->width_; }
    void setUnits(CVTimeUnits units) { this->units_ = units; this->configured_ = false; }
    CVTimeUnits getUnits() const { return this->units_; }
    void setPulseNo(unsigned char no) { this->pulseNo_ = no; this->configured_ = false; }
    unsigned char getPulseNo() const { return this->pulseNo_; }
    void setStartSource(CVIOSources source) { this->start_ = source; this->configured_ = false; }
    CVIOSources getStartSource() const { return this->start_; }
    void setStopSource(CVIOSources source) { this->reset_ = source; this->configured_ = false; }
    CVIOSources getStopSource() const { return this->reset_; }

    void configure() const;
    void update();
    
    void start() const;
    void stop() const;

  private:
    int32_t BHandle_;
    CVPulserSelect pulserId_;
    unsigned char period_;
    unsigned char width_;
    CVTimeUnits units_;
    unsigned char pulseNo_;
    CVIOSources start_;
    CVIOSources reset_;
    bool configured_;
};

class V1718Scaler
{
  public:
    V1718Scaler(int32_t handle);
    ~V1718Scaler() {}
  
    void setLimit(short limit) { this->limit_ = limit; this->configured_=false; }
    short getLimit() const { return this->limit_; }
    void setAutoReset(short autoReset) { this->autoReset_ = autoReset; this->configured_=false; }
    short getAutoReset() const { return this->autoReset_; }
    void setHitSource(CVIOSources source) {this->hit_ = source; this->configured_=false; }
    CVIOSources getHitSource() const { return this->hit_; }
    void setGateSource(CVIOSources source) {this->gate_ = source; this->configured_=false; }
    CVIOSources getGateSource() const { return this->gate_; }
    void setResetSource(CVIOSources source) { this->reset_ = source; this->configured_=false; }
    CVIOSources getResetSource() const { return this->reset_; }
    
    void configure();
    void update();
    
    void resetCount();
    void enableGate();
    void disableGate();
    unsigned short count() const;
  
  private:
    int32_t BHandle_;
    short limit_;
    short autoReset_;
    CVIOSources hit_;
    CVIOSources gate_;
    CVIOSources reset_;
    bool configured_;
};

class CAENVMEexception: public exception
{
  public:
    CAENVMEexception(const CAENVME_API& errorcode) : errorcode_(errorcode) {}
    ~CAENVMEexception() {}

    virtual const char* what() const throw()
    {
      return CAENVME_DecodeError(errorcode_);
    }
  private:
    CAENVME_API errorcode_;
};


class UsbController: public vmeController{
public:
     UsbController(int verbose=3);
     /**< 
      * \brief Class constructor.
      * 
      * This constructor will create the object "BHandle" and store it. It also will set a few default modes and check 
      * if the link to the VME is ok.
      * 
      */
     
     ~UsbController();///< Liberates the USB controller and "BHandle    
     void setMode(const CVAddressModifier AM, const CVDataWidth DW);
     UsbController* mode(const CVAddressModifier AM, const CVDataWidth DW);
     CVAddressModifier getAM() const;
     CVDataWidth getDW() const;
     void setAM(CVAddressModifier AM);
     void setDW(CVDataWidth DW);

     int writeData(const long unsigned int address,void* data) const;
     int readData (const long unsigned int address,void* data) const;
     /* more RW methods here */
     //TODO

     /* Pulser */
     V1718Pulser& getPulser(CVPulserSelect); 
     
     /* Scaler */
     V1718Scaler& getScaler();
     
     /* Input and output registers */
     //TODO
     
     /* Read display */
     CVDisplay readDisplay() const;
     
     /* Behavior */
     //TODO
     
     /* System reset */
     void systemReset() const;
     
     /* Interupts */
     void IRQEnable(uint32_t mask) const;
     void IRQDisable(uint32_t mask) const;
     void IRQWait(uint32_t mask, uint32_t timeout_ms) const;
     unsigned char IRQCheck() const;
     uint16_t IACK(CVIRQLevels Level) const;
     
private:
  std::string firmwareVersion_;
  CVAddressModifier AM_;
  CVDataWidth DW_;
  CVAddressModifier AMtmp_;
  CVDataWidth DWtmp_;
  int32_t BHandle_;
  /**<
   * 
   * \brief Communication identifier.
   * 
   * This object is primordial in any communication with the VME bridge. 
   * It is used every single time DATA has to be transfered from the 
   * host computer to a VME Board.
   * 
   * It is created with the class and will die with the class. Long live BHandle!
   * 
   */
  CVBoardTypes board_;

  V1718Pulser pulserA_;
  V1718Pulser pulserB_;
  V1718Scaler scaler_;
};


#endif
