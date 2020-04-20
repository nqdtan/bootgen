/******************************************************************************
* Copyright 2015-2020 Xilinx, Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
******************************************************************************/

#pragma once

#ifndef _BIFOPTIONS_H_
#define _BIFOPTIONS_H_

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include "options.h"
#include "imageheadertable-versal.h"

/* Forward Class References */
class Options;


/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/
#define SHUTTER_VAL                 0x01000020
#define DEFAULT_PMCDATA_LOADADDR    0xF2000000
#define DEFAULT_ID_CODE_S80         0x04ca8093


typedef struct
{
    Encryption::Type encrypt;
    KeySource::Type encrKeySource;
    std::string encrKeyFile;
    std::vector<uint32_t> encrBlocks;
    uint32_t defEncrBlockSize;
    Authentication::Type authenticate;
    std::string ppk;
    std::string psk;
    std::string spk;
    std::string ssk;
    std::string spkSignature;
    std::string presign;
    uint32_t revokeId;
    Checksum::Type checksum;
    DpaCM::Type dpaCM;
    PufHdLoc::Type pufHdLoc;
} MetaHdrInfo;
/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class PartitionBifOptions
{
public:
    PartitionBifOptions();

    void SetArchType(Arch::Type type);
    void SetEncryptionBlocks(uint32_t size, uint32_t num = 1);
    void SetDefaultEncryptionBlockSize(uint32_t blk);
    void SetDestCpu(DestinationCPU::Type);
    void SetDestDevice(DestinationDevice::Type);
    void SetAuthType(Authentication::Type);
    void SetEncryptType(Encryption::Type);
    void SetChecksumType(Checksum::Type);
    void SetOwnerType(PartitionOwner::Type);
    void SetPartitionType(PartitionType::Type);
    void SetDpaCM(DpaCM::Type);
    void SetExceptionLevel(ExceptionLevel::Type);
    void SetSpkId(uint32_t);
    void SetTrustZone(TrustZone::Type);
    void SetEarlyHandoff(bool);
    void SetHivec(bool);
    void SetRevokeId(uint32_t);
    void SetSlrNum(uint8_t);
    void SetUdfDataFile(std::string filename);
    void SetEncryptionKeySource(KeySource::Type type);
    void SetAuthBlockAttr(size_t blocksizeattr);
    void SetPufHdLocation(PufHdLoc::Type);

    std::string GetUdfDataFile(void);
    std::vector<uint32_t>& GetEncryptionBlocks(void);
    uint32_t GetDefaultEncryptionBlockSize(void);
    KeySource::Type GetEncryptionKeySource(void);
    DpaCM::Type GetDpaCM(void);
    uint32_t GetRevokeId(void);
    PufHdLoc::Type GetPufHdLocation(void);

    void Dump(void)
    {
        LOG_INFO("Filename: %s", filename.c_str());
        if (alignment.IsSet())
        {
            LOG_INFO(" Alignment: %x", alignment.Value());
        }
        if (offset.IsSet())
        {
            LOG_INFO(" Offset: %x", offset.Value());
        }
        if (reserve.IsSet())
        {
            LOG_INFO(" Reserve: %x", reserve.Value());
        }
        if (load.IsSet())
        {
            LOG_INFO(" Load: %x", load.Value());
        }
    }
//private:
    std::string aesKeyFile;
    std::string spkFile;
    std::string sskFile;
    std::string ppkFile;
    std::string pskFile;
    std::string spkSignatureFile;
    bool bootImage;
    bool pmuFwImage;
    bool pmcData;
    std::string presignFile;
    std::string udfDataFile;
    Encryption::Type encryptType;
    Authentication::Type authType;
    Checksum::Type checksumType;
    PartitionOwner::Type ownerType;
    PartitionType::Type partitionType;
    DestinationCPU::Type destCPUType;
    DestinationDevice::Type destDeviceType;
    ExceptionLevel::Type exceptionLevel;
    TrustZone::Type trustzone;
    DpaCM::Type dpaCM;
    PufHdLoc::Type pufHdLoc;
    uint8_t slrNum;

    //no default declared
    Override<int> alignment;
    Override<Binary::Length_t> offset;
    Override<Binary::Length_t> reserve;
    Override<Binary::Address_t> load;
    Override<Binary::Address_t> startup;
    Override<Binary::Address_t> pmcDataLoadAddress;

    bool bigEndian;
    bool a32Mode;
    uint32_t partitionId;
    bool boot;
    bool user;
    bool Static;
    bool Protected;
    bool multiboot;
    bool noautostart;
    bool early_handoff;
    bool bootloader;
    bool hivec;
    KeySource::Type keySrc;
    size_t authblockattr;
    size_t pid;
    std::vector<uint32_t> blocks;
    uint32_t defBlockSize;
    uint32_t revokeId;

    //no default declared
    uint32_t blockSize;
    uint32_t spkSelect;
    bool spkSelLocal;
    uint32_t spkId;
    bool spkIdLocal;
    int  fileType;
    std::string filename;

    Arch::Type arch;
};

/******************************************************************************/
class ImageBifOptions
{
public:
    ImageBifOptions()
        : imageId(0)
        , imageName("")
        , delayHandoff(false)
        , delayLoad(false)
        , memCopyAddr(0xFFFFFFFFFFFFFFFF)
        , imageType(PartitionType::RESERVED)
    {
        partitionBifOptionsList.clear();
    }

    void SetDelayHandoff(bool flag);
    void SetDelayLoad(bool flag);
    
    void SetImageId(uint32_t id)
    {
        imageId = id;
    }
    void SetImageName(std::string name)
    {
        imageName = name;
    }

    void SetMemCopyAddress(uint64_t addr)
    {
        memCopyAddr = addr;
    }
    void SetImageType(PartitionType::Type type)
    {
        imageType = type;
    }

    uint32_t GetImageId(void)
    {
        return imageId;
    }
    std::string GetImageName(void)
    {
        return imageName;
    }
    bool GetDelayHandoff(void)
    {
        return delayHandoff;
    }
    bool GetDelayLoad(void)
    {
        return delayLoad;
    }
    uint64_t GetMemCopyAddress(void)
    {
        return memCopyAddr;
    }
    PartitionType::Type GetImageType(void)
    {
        return imageType;
    }

    std::list<PartitionBifOptions*> partitionBifOptionsList;

private:
    uint32_t imageId;
    std::string imageName;
    bool delayHandoff;
    bool delayLoad;
    uint64_t memCopyAddr;
    PartitionType::Type imageType;
};

/******************************************************************************/
class BifOptions
{
public:
    BifOptions(Arch::Type architecture, const char* name);

    void Add(PartitionBifOptions* currentPartitionBifOptions, ImageBifOptions* currentImageBifOptions = NULL);
    void AddFiles(int type, std::string filename);

    void SetRegInitFileName(std::string filename);
    void SetUdfBHFileName(std::string filename);
    void SetPmufwImageFileName(PartitionBifOptions* currentPartitionBifOptions);
    void SetPPKFileName(std::string filename);
    void SetPSKFileName(std::string filename);
    void SetSPKFileName(std::string filename);
    void SetSSKFileName(std::string filename);
    void SetSPKSignFileName(std::string filename);
    void SetBHSignFileName(std::string filename);
    void SetHeaderSignFileName(std::string filename);
    void SetBHKeyFileName(std::string filename);
    void SetPUFHelperFileName(std::string filename);
    void SetBHKekIVFileName(std::string filename);
    void SetBbramKekIVFileName(std::string filename);
    void SetEfuseKekIVFileName(std::string filename);
    void SetEfuseUserKek0IVFileName(std::string filename);
    void SetEfuseUserKek1IVFileName(std::string filename);
    void SetFamilyKeyFileName(std::string filename);
    void SetAESKeyFileName(std::string filename);
    void SetEncryptionKeySource(KeySource::Type type);
    void SetBootDevice(BootDevice::Type type);
    void SetBootDeviceAddress(uint32_t address);
    void SetBootVectorArray(uint32_t vector);
    void SetXipMode();
    void SetFsblFilename(std::string);
    void SetTotalPmcFwSize(uint32_t);
    void SetPmcFwSize(uint32_t);
    void SetPPKSelection(uint32_t ppkSelection);
    void SetSPKSelection(uint32_t spkSelection);
    void SetSpkId(uint32_t spkId);
    void SetHeaderAuthentication();
    void SetSplitMode(SplitMode::Type type);
    void SetSplitFmt(File::Type type);
    void SetPmcdataFile(const std::string & filename);
    void SetPdiId(uint32_t id);
    void SetGroupName(std::string name);
    void SetParentId(uint32_t id);
    void SetBhRsa(BhRsa::Type value);
    void SetPufHdLoc(PufHdLoc::Type value);
    void SetDpaCM(DpaCM::Type value);
    void SetHeaderAC(bool flag);
    void SetHeaderEncryption(bool flag);
    void SetTotalpmcdataSize(uint32_t size);
    void SetAuthOnly(AuthOnly::Type type);
    void SetOptKey(OptKey::Type type);
    void SetPufMode(PufMode::Type type);
    void SetShutterValue(uint32_t value);
    void InsertEncryptionBlock(uint32_t size);
    void SetCore(Core::Type type);
    void SetMetaHeaderEncryptionKeySource(KeySource::Type type);
    void SetPufHdinBHFlag();

    std::string GetGroupName(void);
    std::string GetAESKeyFileName(void);
    std::string GetRegInitFileName();
    OptKey::Type GetAesOptKeyFlag(void);
    std::vector<uint32_t>& GetEncryptionBlocksList(void);
    BhRsa::Type GetBhRsa(void);
    DpaCM::Type GetDpaCM(void);
    BootDevice::Type GetBootDevice(void);
    uint32_t GetBootDeviceAddress(void);
    uint32_t GetShutterValue(void);
    std::string GetHeaderSignatureFile(void);
    PufMode::Type GetPufMode(void);
    PufHdLoc::Type GetPufHdLoc(void);
    OptKey::Type GetOptKey(void);
    void SetIdCode(uint32_t id);
    void SetExtendedIdCode(uint32_t id);
    void SetBypassIdcodeFlag(bool flag);
    AuthOnly::Type GetAuthOnly(void);
    uint32_t GetPpkSelection(void);
    uint32_t GetSpkSelection(void);
    uint32_t GetSmapWidth(void);
    std::string GetPPKFileName(void);
    std::string GetPSKFileName(void);
    std::string GetSPKFileName(void);
    std::string GetSSKFileName(void);
    uint32_t GetSpkId(void);
    bool GetHeaderAC(void);
    bool GetHeaderEncyption(void);
    bool GetPpkSelectionGlobal(void);
    bool GetSpkSelectionGlobal(void);
    bool GetSpkIdGlobal(void);
    bool GetXipMode(void);
    std::string GetBhKeyFile(void);
    std::string GetPufHelperFile(void);
    std::string GetPmuFwImageFile(void);
    std::string GetBHKekIVFile(void);
    std::string GetBbramKekIVFile(void);
    std::string GetEfuseKekIVFile(void);
    std::string GetEfuseUserKek0IVFile(void);
    std::string GetEfuseUserKek1IVFile(void);
    std::string GetUdfBhFile(void);
    SplitMode::Type GetSplitMode(void);
    uint32_t GetIdCode(void);
    uint32_t GetExtendedIdCode(void);
    bool GetBypassIdcodeFlag(void);
    File::Type GetSplitFormat(void);
    std::vector<uint32_t>& GetBootVectorArray(void);
    std::string GetSPKSignFileName(void);
    std::string GetBHSignFileName(void);
    uint32_t GetPmcCdoLoadAddress(void);
    std::string GetPmcdataFile(void);
    std::string GetPmcDataAesFile(void);
    uint32_t GetTotalPmcFwSize(void);
    uint32_t GetPmcFwSize(void);
    uint8_t* GetPmcDataBuffer(void);
    uint32_t GetTotalpmcdataSize(void);
    uint32_t GetPdiId(void);
    uint32_t GetParentId(void);
    KeySource::Type GetEncryptionKeySource(void);
    bool GetPufHdinBHFlag(void);

    std::string GetFamilyKeyFileName();
    std::list<std::string> aie_elfs;
    std::list<ImageBifOptions*> imageBifOptionList;
    std::list<PartitionBifOptions*> partitionBifOptionList;
    Core::Type GetCore(void);
    
    //Versal
    Binary::Address_t pmcCdoLoadAddress;
    std::string pmcDataAesFile;
    uint32_t idCode;
    uint32_t extendedIdCode;
    bool bypassIdCode;
    uint32_t pmcdataSize;
    uint32_t totalpmcdataSize;
    uint8_t* pmcDataBuffer;
    uint32_t smapWidth;
    MetaHdrInfo metaHdrAttributes;
    uint32_t slrBootCnt;
    uint32_t slrConfigCnt;

private:
    std::string regInitFile;
    std::string udfBhFile;
    std::string pmuFwImageFile;
    std::string pmcdataFile;
    std::string fsblFilename;
    std::string ppkFile;
    std::string pskFile;
    std::string spkFile;
    std::string sskFile;
    std::string spkSignatureFile;
    std::string bhSignatureFile;
    std::string headerSignatureFile;
    std::string bhKeyFile;
    std::string pufHelperFile;
    std::string bhKekIVFile;
    std::string bbramKekIVFile;
    std::string efuseKekIVFile;
    std::string efuseUserKek0IVFile;
    std::string efuseUserKek1IVFile;
    std::string familyKeyFile;
    std::string aesKeyFile;
    KeySource::Type keySourceEncryption;
    BootDevice::Type bootDevice;
    uint32_t bootDeviceAddress;
    std::vector<uint32_t> bootvectors;
    BhRsa::Type bhAuthEnable;
    PufHdLoc::Type pufHdLoc;
    bool pufHdinBHEnable;
    AuthOnly::Type authOnly;
    PufMode::Type pufMode;
    OptKey::Type optKey;
    uint32_t shutterVal;
    DpaCM::Type dpaCM;
    bool xipMode;
    uint32_t ppkSelect;
    bool isPPKSelectGlobal;
    uint32_t spkSelect;
    bool isSPKSelectGlobal;
    uint32_t spkId;
    bool isSpkIdGlobal;
    bool headerAuthParam;
    Authentication::Type headerAuthType;
    bool createHeaderAC;
    bool doHeaderEncryption;
    SplitMode::Type splitMode;
    File::Type splitFmt;
    std::string groupname;
    Arch::Type arch;
    std::vector<uint32_t> blocks;
    Core::Type core;
    // Versal
    uint32_t pdiId;
    uint32_t parentId;
};
#endif