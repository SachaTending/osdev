#include "stdint.h"

typedef enum
{
    OPL_REGISTER_PORT = 0,
    OPL_DATA_PORT = 1,
    OPL_REGISTER_PORT_OPL3 = 2
} opl_port_t;

class OPL
{
private:
    bool init_stage_reg = true;
public:
    uint16_t port_base;
    OPL(uint16_t port_base);
    OPL();

    void Detect();

    void WritePort(opl_port_t port, uint32_t value);
    uint32_t ReadPort(opl_port_t port);

    void WriteReg(int reg, int val);
};

#define OPL_REG_WAVEFORM_ENABLE   0x01
#define OPL_REG_TIMER1            0x02
#define OPL_REG_TIMER2            0x03
#define OPL_REG_TIMER_CTRL        0x04
#define OPL_REG_FM_MODE           0x08
#define OPL_REG_NEW               0x105

// Operator registers (21 of each):

#define OPL_REGS_TREMOLO          0x20
#define OPL_REGS_LEVEL            0x40
#define OPL_REGS_ATTACK           0x60
#define OPL_REGS_SUSTAIN          0x80
#define OPL_REGS_WAVEFORM         0xE0

// Voice registers (9 of each):

#define OPL_REGS_FREQ_1           0xA0
#define OPL_REGS_FREQ_2           0xB0
#define OPL_REGS_FEEDBACK         0xC0