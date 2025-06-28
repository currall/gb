#include <stdint.h>

typedef struct {

    uint8_t     ROMName[16];
    uint16_t    NewLicensee;
    uint8_t     SGBFeatures;
    uint8_t     CartridgeType;
    uint8_t     ROMSize;
    uint8_t     RAMSize;
    uint8_t     CountryCode;
    uint8_t     Licensee;

} Header;