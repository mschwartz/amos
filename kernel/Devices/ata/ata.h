#ifndef AMOS_ATADEVICE_ATA_H
#define AMOS_ATADEVICE_ATA_H

#include <Types.h>

// TODO: make these #defines into const
#define IDE_ATA 0x00
#define IDE_ATAPI 0x01
// #define ATA_PRIMARY 0x1F0
// #define ATA_SECONDARY 0x170
#define ATA_MASTER 0x00
#define ATA_SLAVE 0x10

// STATUS
#define ATA_SR_BSY 0x80  // Busy
#define ATA_SR_DRDY 0x40 // Drive ready
#define ATA_SR_DF 0x20   // Drive write fault
#define ATA_SR_DSC 0x10  // Drive seek complete
#define ATA_SR_DRQ 0x08  // Data request ready
#define ATA_SR_CORR 0x04 // Corrected data
#define ATA_SR_IDX 0x02  // Index
#define ATA_SR_ERR 0x01  // Error

// ERRORS
#define ATA_ER_BBK 0x80   // Bad block
#define ATA_ER_UNC 0x40   // Uncorrectable data
#define ATA_ER_MC 0x20    // Media changed
#define ATA_ER_IDNF 0x10  // ID mark not found
#define ATA_ER_MCR 0x08   // Media change request
#define ATA_ER_ABRT 0x04  // Command aborted
#define ATA_ER_TK0NF 0x02 // Track 0 not found
#define ATA_ER_AMNF 0x01  // No address mark

// COMMANDS
#define ATA_CMD_READ_PIO 0x20
#define ATA_CMD_READ_PIO_EXT 0x24
#define ATA_CMD_READ_DMA 0xC8
#define ATA_CMD_READ_DMA_EXT 0x25
#define ATA_CMD_WRITE_PIO 0x30
#define ATA_CMD_WRITE_PIO_EXT 0x34
#define ATA_CMD_WRITE_DMA 0xCA
#define ATA_CMD_WRITE_DMA_EXT 0x35
#define ATA_CMD_CACHE_FLUSH 0xE7
#define ATA_CMD_CACHE_FLUSH_EXT 0xEA
#define ATA_CMD_PACKET 0xA0
#define ATA_CMD_IDENTIFY_PACKET 0xA1
#define ATA_CMD_IDENTIFY 0xEC

#define ATAPI_CMD_READ 0xA8
#define ATAPI_CMD_EJECT 0x1B

// ATA IDENTIFY 512 BYTE BUFFER OFFSETS
#define ATA_IDENT_DEVICETYPE 0
#define ATA_IDENT_CYLINDERS 2
#define ATA_IDENT_HEADS 6
#define ATA_IDENT_SECTORS 12
#define ATA_IDENT_SERIAL 20
#define ATA_IDENT_MODEL 54
#define ATA_IDENT_LBA48 83
#define ATA_IDENT_CAPABILITIES 98
#define ATA_IDENT_FIELDVALID 106
#define ATA_IDENT_MAX_LBA 120
#define ATA_IDENT_COMMANDSETS 164
#define ATA_IDENT_MAX_LBA_EXT 200

// CHANNEL ATA REGISTER OFFSETS
#define ATA_REG_DATA 0x00
#define ATA_REG_ERROR 0x01
#define ATA_REG_FEATURES 0x01
#define ATA_REG_SECCOUNT0 0x02
#define ATA_REG_LBA0 0x03
#define ATA_REG_LBA1 0x04
#define ATA_REG_LBA2 0x05
#define ATA_REG_HDDEVSEL 0x06
#define ATA_REG_COMMAND 0x07
#define ATA_REG_STATUS 0x07
#define ATA_REG_SECCOUNT1 0x08
#define ATA_REG_LBA3 0x09
#define ATA_REG_LBA4 0x0A
#define ATA_REG_LBA5 0x0B
#define ATA_REG_CONTROL 0x0C
#define ATA_REG_ALTSTATUS 0x0C
#define ATA_REG_DEVADDRESS 0x0D

// CHANNELS
#define ATA_PRIMARY 0x00
#define ATA_SECONDARY 0x01
// DIRECTIONS
#define ATA_READ 0x00
#define ATA_WRITE 0x01

// one of these for master and slave
typedef struct {
  TUint16 mBasePort;    // I/O Base.
  TUint16 mControlBase; // Control Base
  TUint16 mDmaPort;     // Bus Master IDE
  TUint8 nIEN;          // nIEN (No Interrupt);
} TIdeChannelRegisters;

typedef struct _tag_TIdeDevice {
  TUint8 Reserved;      // 0 (Empty) or 1 (This Drive really exists).
  TUint8 Channel;       // 0 (Primary Channel) or 1 (Secondary Channel).
  TUint8 Drive;         // 0 (Master Drive) or 1 (Slave Drive).
  TUint16 Type;         // 0: ATA, 1:ATAPI.
  TUint16 Signature;    // Drive Signature
  TUint16 Capabilities; // Features.
  TUint32 CommandSets;  // Command Sets Supported.
  TUint32 Size;         // Size in Sectors.
  TUint8 Model[41];     // Model in string.
  TBool mLba48;         // ETrue if device supports LBA48 addressing
  void Dump() {
    if (Reserved == 1) {
      dlog("  Channel(%s) Drive(%s) Type(%s) Size(%d) LBA48(%s) Model(%s)\n",
        Channel ? "Secondary" : "Primary",
        Drive ? "Slave" : "Master",
        Type ? "ATAPI" : "ATA",
        Size,
        mLba48 ? "YES" : "NO",
        Model);
    }
    else {
      dlog("  %s %s %s\n",
        Channel ? "Secondary" : "Primary",
        Drive ? "Slave" : "Master",
        "Not Present");
    }
  }
} TIdeDevice;

typedef struct {
  TUint16 mPad0[27];
  TUint16 mModel[20];
  TUint16 mPad1[13];
  TUint32 mLbaSectors;
  TUint16 mPad2[21];
  TUint16 mLba48;
  TUint16 mPad3[16];
  TUint64 mLba48Sectors;
  TUint16 mPad4[152];
} PACKED TAtaIdentity;

#endif
