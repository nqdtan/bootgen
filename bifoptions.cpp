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

/*
-------------------------------------------------------------------------------
************************************************************ HEADER FILES *****
-------------------------------------------------------------------------------
*/
#include <fstream>
#include <algorithm>
#include <string>
#include "bifoptions.h"
#include "bifscanner.h"

/* Forward Class Declaration */
class BootImage;
static bool delay_load_warning_given;
static bool delay_handoff_warning_given;
/*
-------------------------------------------------------------------------------
*************************************************************** FUNCTIONS *****
-------------------------------------------------------------------------------
*/
/******************************************************************************/
BifOptions::BifOptions(Arch::Type architecture, const char* name)
    : groupname("")
    , regInitFile("")
    , udfBhFile("")
    , ppkFile("")
    , pskFile("")
    , headerSignatureFile("")
    , bhKeyFile("")
    , pufHelperFile("")
    , bhKekIVFile("")
    , bbramKekIVFile("")
    , efuseKekIVFile("")
    , efuseUserKek0IVFile("")
    , efuseUserKek1IVFile("")
    , familyKeyFile("")
    , keySourceEncryption(KeySource::None)
    , bootDevice(BootDevice::DEFAULT)
    , bootDeviceAddress(0)
    , bhAuthEnable(BhRsa::BhRsaDisable)
    , pufHdLoc(PufHdLoc::PUFinEFuse)
    , pufHdinBHEnable(false)
    , authOnly(AuthOnly::None)
    , pufMode(PufMode::PUF12K)
    , optKey(OptKey::None)
    , shutterVal(SHUTTER_VAL)
    , dpaCM(DpaCM::DpaCMDisable)
    , ppkSelect(0)
    , spkSelect(1)
    , spkId(0x00000000)
    , headerAuthParam(false)
    , headerAuthType(Authentication::None)
    , createHeaderAC(false)
    , doHeaderEncryption(false)
    , splitMode(SplitMode::None)
    , splitFmt(File::Unknown)
    , xipMode(false)
    , pmcCdoLoadAddress(DEFAULT_PMCDATA_LOADADDR)
    , pmcdataSize(0)
    , totalpmcdataSize(0)
    , pmcDataBuffer(NULL)
    , pdiId(0)
    , parentId(0)
    , idCode(0)
    , extendedIdCode(0)
    , bypassIdCode(false)
    , smapWidth(32)
    , core(Core::A53Singlex64)
    , isPPKSelectGlobal(false)
    , isSPKSelectGlobal(false)
    , isSpkIdGlobal(false)
    , slrBootCnt(0)
    , slrConfigCnt(0)
{
    arch = architecture;
    SetGroupName(name);
    delay_handoff_warning_given = false;
    delay_load_warning_given = false;
    metaHdrAttributes.encrypt = Encryption::None;
    metaHdrAttributes.encrKeySource = KeySource::None;
    metaHdrAttributes.encrKeyFile = "";
    metaHdrAttributes.authenticate = Authentication::None;
    metaHdrAttributes.ppk = metaHdrAttributes.spk = metaHdrAttributes.psk = metaHdrAttributes.ssk = "";
    metaHdrAttributes.presign = metaHdrAttributes.spkSignature = "";
    metaHdrAttributes.revokeId = 0;
    metaHdrAttributes.checksum = Checksum::None;
    metaHdrAttributes.dpaCM = DpaCM::DpaCMDisable;
    metaHdrAttributes.pufHdLoc = PufHdLoc::PUFinEFuse;
}

/******************************************************************************/
PartitionBifOptions::PartitionBifOptions()
    : aesKeyFile("")
    , ppkFile("")
    , pskFile("")
    , spkFile("")
    , sskFile("")
    , spkSignatureFile("")
    , bootImage(false)
    , pmuFwImage(false)
    , pmcData(false)
    , presignFile("")
    , udfDataFile("")
    , encryptType(Encryption::None)
    , authType(Authentication::None)
    , checksumType(Checksum::None)
    , ownerType(PartitionOwner::FSBL)
    , partitionType(PartitionType::RESERVED)
    , destCPUType(DestinationCPU::NONE)
    , destDeviceType(DestinationDevice::DEST_DEV_PS)
    , exceptionLevel(ExceptionLevel::EL3)
    , trustzone(TrustZone::NonSecure)
    , bootloader(false)
    , early_handoff(false)
    , hivec(false)
    , authblockattr(0)
    , pid(0)
    , blocks(0)
    , defBlockSize(0)
    , spkSelect(1)
    , spkSelLocal(false)
    , spkId(0x00000000)
    , spkIdLocal(false)
    , fileType(0)
    , filename("")
    , arch (Arch::ZYNQ)
    , partitionId(0)
    , keySrc(KeySource::None)
    , boot(false)
    , user(false)
    , Static(false)
    , Protected(false)
    , multiboot(false)
    , noautostart(false)
    , blockSize(0)
    , bigEndian(false)
    , a32Mode(false)
    , dpaCM(DpaCM::DpaCMDisable)
    , revokeId(0x00000000)
    , slrNum(0xFF)
    , pufHdLoc(PufHdLoc::PUFinEFuse)
{ }

/******************************************************************************/
void BifOptions::SetHeaderAC(bool flag)
{
    createHeaderAC = flag;
}

/******************************************************************************/
void BifOptions::SetHeaderEncryption(bool flag)
{
    doHeaderEncryption = flag;
}

/******************************************************************************/
void BifOptions::Add(PartitionBifOptions* currentPartitionBifOptions, ImageBifOptions* currentImageBifOptions)
{
    //filespec->Dump();
    switch (currentPartitionBifOptions->fileType)
    {
    case BIF::BisonParser::token::INIT:
        SetRegInitFileName(currentPartitionBifOptions->filename);
        break;

    case BIF::BisonParser::token::UDF_BH:
        SetUdfBHFileName(currentPartitionBifOptions->filename);
        break;

    case BIF::BisonParser::token::AES_KEY_FILE:
        SetAESKeyFileName(currentPartitionBifOptions->filename);
        break;

    case BIF::BisonParser::token::FAMILY_KEY:
        SetFamilyKeyFileName(currentPartitionBifOptions->filename);
        break;

    case BIF::BisonParser::token::PPK_FILE:
        SetPPKFileName(currentPartitionBifOptions->filename);
        break;

    case BIF::BisonParser::token::PSK_FILE:
        SetPSKFileName(currentPartitionBifOptions->filename);
        break;

    case BIF::BisonParser::token::SPK_FILE:
        SetSPKFileName(currentPartitionBifOptions->filename);
        break;

    case BIF::BisonParser::token::SSK_FILE:
        SetSSKFileName(currentPartitionBifOptions->filename);
        break;

    case BIF::BisonParser::token::SPK_SIGNATURE_FILE:
        SetSPKSignFileName(currentPartitionBifOptions->filename);
        break;

    case BIF::BisonParser::token::BH_SIGNATURE_FILE:
        SetBHSignFileName(currentPartitionBifOptions->filename);
        break;

    case BIF::BisonParser::token::HEADER_SIGNATURE_FILE:
        SetHeaderSignFileName(currentPartitionBifOptions->filename);
        break;

    case BIF::BisonParser::token::BH_KEY_FILE:
        SetBHKeyFileName(currentPartitionBifOptions->filename);
        break;

    case BIF::BisonParser::token::PUF_HELPER_FILE:
        SetPUFHelperFileName(currentPartitionBifOptions->filename);
        break;

    case BIF::BisonParser::token::BH_KEY_IV:
        SetBHKekIVFileName(currentPartitionBifOptions->filename);
        break;

    case BIF::BisonParser::token::PMUFW_IMAGE:
        SetPmufwImageFileName(currentPartitionBifOptions);
        break;

    case BIF::BisonParser::token::PMCDATA:
        SetPmcdataFile(currentPartitionBifOptions->filename);
        if ((currentPartitionBifOptions)->load.Value() != 0)
        {
            pmcCdoLoadAddress = (currentPartitionBifOptions)->load.Value();
        }
        if ((currentPartitionBifOptions)->aesKeyFile != "")
        {
            pmcDataAesFile = (currentPartitionBifOptions)->aesKeyFile;
        }
        //If no key file found in partition specific attributes - Generate aeskeyfile with partition_name.nky
        else
        {
            pmcDataAesFile = StringUtils::RemoveExtension(StringUtils::BaseName((currentPartitionBifOptions)->filename)) + ".nky";
        }
        currentPartitionBifOptions->pmcData = true;
        break;

    case 0:
        if (currentImageBifOptions != NULL)
        {
            if (currentImageBifOptions->GetImageType() == PartitionType::SLR_BOOT)
            {
                slrBootCnt++;
                currentPartitionBifOptions->partitionType = PartitionType::SLR_BOOT;
            }
            if (currentImageBifOptions->GetImageType() == PartitionType::SLR_CONFIG)
            {
                slrConfigCnt++;
                currentPartitionBifOptions->partitionType = PartitionType::SLR_CONFIG;
            }
            if (currentPartitionBifOptions->partitionType == PartitionType::RESERVED)
            {
                currentPartitionBifOptions->partitionType = currentImageBifOptions->GetImageType();
            }
            currentImageBifOptions->partitionBifOptionsList.push_back(currentPartitionBifOptions);
        }
        partitionBifOptionList.push_back(currentPartitionBifOptions);
    }
}


/******************************************************************************/
void BifOptions::AddFiles(int type, std::string filename)
{
    //filespec->Dump();
    switch (type)
    {
        case BIF::BisonParser::token::INIT: 
            SetRegInitFileName(filename);
            break;

        case BIF::BisonParser::token::UDF_BH:
            SetUdfBHFileName(filename);
            break;

        case BIF::BisonParser::token::AES_KEY_FILE:
            SetAESKeyFileName(filename);
            break;

        case BIF::BisonParser::token::FAMILY_KEY:
            SetFamilyKeyFileName(filename);
            break;

        case BIF::BisonParser::token::PPK_FILE:
            SetPPKFileName(filename);
            break;

        case BIF::BisonParser::token::PSK_FILE:
            SetPSKFileName(filename);
            break;

        case BIF::BisonParser::token::SPK_FILE:
            SetSPKFileName(filename);
            break;

        case BIF::BisonParser::token::SSK_FILE:
            SetSSKFileName(filename);
            break;

        case BIF::BisonParser::token::SPK_SIGNATURE_FILE:
            SetSPKSignFileName(filename);
            break;

        case BIF::BisonParser::token::BH_SIGNATURE_FILE:
            SetBHSignFileName(filename);
            break;

        case BIF::BisonParser::token::HEADER_SIGNATURE_FILE:
            SetHeaderSignFileName(filename);
            break;

        case BIF::BisonParser::token::BH_KEY_FILE:
            SetBHKeyFileName(filename);
            break;

        case BIF::BisonParser::token::PUF_HELPER_FILE:
            SetPUFHelperFileName(filename);
            break;

        case BIF::BisonParser::token::BH_KEK_IV:
            SetBHKekIVFileName(filename);
            break;

        case BIF::BisonParser::token::BBRAM_KEK_IV:
            SetBbramKekIVFileName(filename);
            break;

        case BIF::BisonParser::token::EFUSE_KEK_IV:
            SetEfuseKekIVFileName(filename);
            break;

        case BIF::BisonParser::token::EFUSE_USER_KEK0_IV:
            SetEfuseUserKek0IVFileName(filename);
            break;

        case BIF::BisonParser::token::EFUSE_USER_KEK1_IV:
            SetEfuseUserKek1IVFileName(filename);
            break;
    }
}

/******************************************************************************/
void BifOptions::SetRegInitFileName(std::string filename)
{
    std::ifstream f(filename.c_str());
    if (!f) 
    {
        LOG_ERROR("Cannot read file - %s", regInitFile.c_str());
    }
    regInitFile = filename;
    LOG_TRACE("Setting Register initialization file as %s", regInitFile.c_str());
}

/******************************************************************************/
std::string BifOptions::GetRegInitFileName()
{
    return regInitFile;
}

/******************************************************************************/
std::string BifOptions::GetGroupName()
{
    return groupname;
}

/******************************************************************************/
std::string BifOptions::GetAESKeyFileName(void)
{
    return aesKeyFile;
}

/******************************************************************************/
void BifOptions::SetUdfBHFileName(std::string filename)
{
    std::ifstream f(filename.c_str());
    if (!f)
    {
        LOG_ERROR("Cannot read file - %s", regInitFile.c_str());
    }
    udfBhFile = filename;
    LOG_TRACE("Setting UDF of BH as %s", udfBhFile.c_str());
}

/******************************************************************************/
void BifOptions::SetPmufwImageFileName(PartitionBifOptions* currentPartitionBifOptions)
{
    if (arch != Arch::ZYNQMP)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'[pmufw_image]' is supported only in ZYNQMP architecture");
    }
    std::ifstream f(currentPartitionBifOptions->filename.c_str());
    if (!f)
    {
        LOG_ERROR("Cannot read file - %s", currentPartitionBifOptions->filename.c_str());
    }

    if (currentPartitionBifOptions->authType != Authentication::None)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\tBif option 'authentication' is not supported with [pmufw_image].\n\t\tpmufw will be signed along with bootloader, if authentication is enabled for bootloader.");
    }
    if (currentPartitionBifOptions->encryptType != Encryption::None)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\tBif option 'encryption' is not supported with [pmufw_image].\n\t\tpmufw will be encrypted if encryption is enabled for bootloader.");
    }
    if (currentPartitionBifOptions->checksumType != Checksum::None)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\tBif option 'checksum' is not supported with [pmufw_image].\n\t\tpmufw will be checksummed if checksum is enabled for bootloader.");
    }
    if (currentPartitionBifOptions->destCPUType != DestinationCPU::NONE)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\tBif option 'destination_cpu' is not supported with [pmufw_image].");
    }

    pmuFwImageFile = currentPartitionBifOptions->filename;
    LOG_TRACE("Setting PMU FW Image file as %s", pmuFwImageFile.c_str());
}


/******************************************************************************/
void BifOptions::SetPmcdataFile(const std::string& filename)
{
    if (arch != Arch::VERSAL)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t[pmcdata] supported only in VERSAL architecture");
    }
    pmcdataFile = filename;
}

/******************************************************************************/
void BifOptions::SetGroupName(std::string name)
{
   groupname = name;
}

/******************************************************************************/
void BifOptions::SetPPKFileName(std::string filename)
{
    std::ifstream f(filename.c_str());
    ppkFile = filename;
}

/******************************************************************************/
void BifOptions::SetPSKFileName(std::string filename)
{
    std::ifstream f(filename.c_str());
    pskFile = filename;
}

/******************************************************************************/
void BifOptions::SetSPKFileName(std::string filename)
{
    std::ifstream f(filename.c_str());
    spkFile = filename;
}

/******************************************************************************/
void BifOptions::SetSSKFileName(std::string filename)
{
    std::ifstream f(filename.c_str());
    sskFile = filename;
}

/******************************************************************************/
void BifOptions::SetSPKSignFileName(std::string filename)
{
    std::ifstream f(filename.c_str());
    if (!f)
    {
        LOG_ERROR("Cannot read file - %s", filename.c_str());
    }
    spkSignatureFile = filename;
}

/******************************************************************************/
void BifOptions::SetBHSignFileName(std::string filename)
{
    std::ifstream f(filename.c_str());
    if (!f)
    {
        LOG_ERROR("Cannot read file - %s", filename.c_str());
    }
    bhSignatureFile = filename;
}

/******************************************************************************/
void BifOptions::SetHeaderSignFileName(std::string filename)
{
    std::ifstream f(filename.c_str());
    if (!f)
    {
        LOG_ERROR("Cannot read file - %s", filename.c_str());
    }
    headerSignatureFile = filename;
}

/******************************************************************************/
void BifOptions::SetBHKeyFileName(std::string filename)
{
    if (arch == Arch::ZYNQ)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'[bh_keyfile]' not supported in ZYNQ architecture");
    }

    std::ifstream f(filename.c_str());
    if (!f)
    {
        LOG_ERROR("Cannot read file - %s", filename.c_str());
    }

    bhKeyFile = filename;
    LOG_TRACE("Setting BH Key file as %s", bhKeyFile.c_str());
}

/******************************************************************************/
void BifOptions::SetTotalpmcdataSize(uint32_t size)
{
    totalpmcdataSize = size;
}

/******************************************************************************/
void BifOptions::SetOptKey(OptKey::Type type)
{
    optKey = type;
}

/******************************************************************************/
void BifOptions::SetPufMode(PufMode::Type type)
{
    pufMode = type;
}

/******************************************************************************/
void BifOptions::SetShutterValue(uint32_t value)
{
    shutterVal = value;
}

/******************************************************************************/
void BifOptions::SetPUFHelperFileName(std::string filename)
{
    if (arch == Arch::ZYNQ)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'[puf_file]' not supported in ZYNQ architecture");
    }

    std::ifstream f(filename.c_str());
    if (!f)
    {
        LOG_ERROR("Cannot read file - %s", filename.c_str());
    }

    pufHelperFile = filename;
    LOG_TRACE("Setting PUF Helper Data file as %s", pufHelperFile.c_str());
}

/******************************************************************************/
void BifOptions::SetBHKekIVFileName(std::string filename)
{
    if (arch == Arch::ZYNQ)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'bh_key_iv/bh_kek_iv' not supported in ZYNQ architecture");
    }

    std::ifstream f(filename.c_str());
    if (!f)
    {
        LOG_ERROR("Cannot read file - %s", filename.c_str());
    }

    bhKekIVFile = filename;
    LOG_TRACE("Setting BH Kek IV file as %s", bhKekIVFile.c_str());
}

/******************************************************************************/
void BifOptions::SetBbramKekIVFileName(std::string filename)
{
    if (arch != Arch::VERSAL)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'bbram_kek_iv' supported only in VERSAL architecture");
    }

    std::ifstream f(filename.c_str());
    if (!f)
    {
        LOG_ERROR("Cannot read file - %s", filename.c_str());
    }

    bbramKekIVFile = filename;
    LOG_TRACE("Setting Bbram Kek IV file as %s", bbramKekIVFile.c_str());
}

/******************************************************************************/
void BifOptions::SetEfuseKekIVFileName(std::string filename)
{
    if (arch != Arch::VERSAL)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'efuse_kek_iv' is supported only in VERSAL architecture");
    }

    std::ifstream f(filename.c_str());
    if (!f)
    {
        LOG_ERROR("Cannot read file - %s", filename.c_str());
    }

    efuseKekIVFile = filename;
    LOG_TRACE("Setting Efuse Kek IV file as %s", efuseKekIVFile.c_str());
}

/******************************************************************************/
void BifOptions::SetEfuseUserKek0IVFileName(std::string filename)
{
    if (arch != Arch::VERSAL)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'efuse_user_kek0_iv' is supported only in VERSAL architecture");
    }

    std::ifstream f(filename.c_str());
    if (!f)
    {
        LOG_ERROR("Cannot read file - %s", filename.c_str());
    }

    efuseUserKek0IVFile = filename;
    LOG_TRACE("Setting Efuse User Kek0 IV file as %s", efuseUserKek0IVFile.c_str());
}

/******************************************************************************/
void BifOptions::SetEfuseUserKek1IVFileName(std::string filename)
{
    if (arch != Arch::VERSAL)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'efuse_user_kek1_iv' is supported only in VERSAl architecture");
    }

    std::ifstream f(filename.c_str());
    if (!f)
    {
        LOG_ERROR("Cannot read file - %s", filename.c_str());
    }

    efuseUserKek1IVFile = filename;
    LOG_TRACE("Setting Efuse User Kek1 IV file as %s", efuseUserKek1IVFile.c_str());
}

/******************************************************************************/
void BifOptions::SetFamilyKeyFileName(std::string filename)
{
    if (arch == Arch::ZYNQ)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'[familykey]' not supported in ZYNQ architecture");
    }

    std::ifstream f(filename.c_str());
    if (!f)
    {
        LOG_ERROR("Cannot read file - %s", filename.c_str());
    }

    familyKeyFile = filename;
    LOG_TRACE("Setting Family Key file as %s", familyKeyFile.c_str());
}

/******************************************************************************/
void BifOptions::SetAESKeyFileName(std::string filename)
{
    std::ifstream f(filename.c_str());
    aesKeyFile = filename;
}

/******************************************************************************/
void BifOptions::SetBootVectorArray(uint32_t vector)
{
    static bool warningGiven = false;
    if (!warningGiven)
    {
        LOG_TRACE("Setting bootvector array");
    }
    warningGiven = true;

    bootvectors.push_back(vector);
    if (bootvectors.size() > 8)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t[bootvectors] cannot take more than 8 vector addresses");
    }
}

/******************************************************************************/
void BifOptions::SetXipMode()
{
    xipMode = true;
}

/******************************************************************************/
bool BifOptions::GetXipMode(void)
{
    return xipMode;
}

/******************************************************************************/
void BifOptions::SetSplitMode(SplitMode::Type type)
{
    LOG_TRACE("Setting Split Mode as %d", type );
    splitMode = type;
}

/******************************************************************************/
void BifOptions::SetSplitFmt(File::Type type)
{
    LOG_TRACE("Setting Split Format as %d", type);
    splitFmt = type;
}

/******************************************************************************/
void BifOptions::SetEncryptionKeySource(KeySource::Type type)
{
    if (arch == Arch::ZYNQ)
    {
        if ((type != KeySource::BbramRedKey) && (type != KeySource::EfuseRedKey) && (type != KeySource::None))
        {
            LOG_DEBUG(DEBUG_STAMP, "Key Source = %d", type);
            LOG_ERROR("BIF attribute error !!!\n\t\tUnknown option for 'keysrc_encryption' in BIF file\n\t\tZynq Architecture supports 'efuse' & 'bbram' key sources");
        }
    }
    keySourceEncryption = type;
}

/******************************************************************************/
void BifOptions::SetBootDevice(BootDevice::Type type)
{
    LOG_TRACE("Setting Boot Device as %d", type);
    bootDevice = type;
}

/******************************************************************************/
void BifOptions::SetBootDeviceAddress(uint32_t address)
{
    bootDeviceAddress = address;
}

/******************************************************************************/
OptKey::Type BifOptions::GetAesOptKeyFlag(void)
{
    return optKey;
}

/******************************************************************************/
void BifOptions::SetMetaHeaderEncryptionKeySource(KeySource::Type type)
{
    metaHdrAttributes.encrKeySource = type;
}

/******************************************************************************/
void BifOptions::SetPufHdinBHFlag()
{
    pufHdinBHEnable = true;
}

/******************************************************************************/
void PartitionBifOptions::SetEncryptionKeySource(KeySource::Type type)
{
    keySrc = type;
}

/******************************************************************************/
void PartitionBifOptions::SetArchType(Arch::Type type)
{
    arch = type;
}

/******************************************************************************/
void PartitionBifOptions::SetEncryptionBlocks(uint32_t size, uint32_t num)
{
    if (arch == Arch::ZYNQ)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'blocks' not supported in ZYNQ architecture");
    }
    if (arch == Arch::VERSAL)
    {
        if (size < 64)
        {
            LOG_ERROR("BIF attribute error !!!\n\t\tThe minimum block size allowed is 4 AES encryption blocks (i.e., 64 bytes)");
        }
    }
    if (num == 0)
    {
        SetDefaultEncryptionBlockSize(size);
    }
    else if (defBlockSize == 0)
    {
        for (uint32_t i = 0; i < num; i++)
        {
            /* The size of each block is considered in bytes */
            if ((size & 0x3) != 0)
            {
                LOG_DEBUG(DEBUG_STAMP, "Encryption Block Size Error - Block Size - %d", size);
                LOG_ERROR("BIF attribute 'blocks' must specify sizes which are multiples of 4, for word alignment.");
            }
            blocks.push_back(size);
        }
    }
    else
    {
        LOG_ERROR("Cannot choose block size after choosing a default block size.");
    }
}

/******************************************************************************/
void PartitionBifOptions::SetDefaultEncryptionBlockSize(uint32_t blk)
{
    defBlockSize = blk;
}

/******************************************************************************/
void BifOptions::InsertEncryptionBlock(uint32_t blk)
{
    blocks.push_back(blk);
}

/******************************************************************************/
void PartitionBifOptions::SetDestCpu(DestinationCPU::Type type)
{
    if (arch == Arch::ZYNQ)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'destination_cpu' not supported in ZYNQ architecture");
    }
    destCPUType = type;
}

/******************************************************************************/
void PartitionBifOptions::SetDestDevice(DestinationDevice::Type type)
{
    if (arch == Arch::ZYNQ)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'destination_device' not supported in ZYNQ architecture");
    }

    if (type == DestinationDevice::DEST_DEV_PMU)
    {
        LOG_ERROR("BIF attribute 'destination_device=pmufw' is not supported.\
                    Please use 'destination_cpu=pmu'");
    }

    if (type == DestinationDevice::DEST_DEV_XIP)
    {
        LOG_ERROR("BIF attribute 'destination_device=xip' is not supported.\
                     Please use the attribute 'xip_mode'. \
                     For more info, see 'bootgen -bif_help xip_mode'");
    }

    destDeviceType = type;
}

/******************************************************************************/
void PartitionBifOptions::SetAuthType(Authentication::Type type)
{
    authType = type;
}

/******************************************************************************/
void PartitionBifOptions::SetEncryptType(Encryption::Type type)
{
    encryptType = type;
}

/******************************************************************************/
void PartitionBifOptions::SetChecksumType(Checksum::Type type)
{
    if (type == Checksum::SHA2)
    {
        LOG_ERROR("BIF cksum BIF attribute error !!!\n\t\t'checksum=sha2' is not supported.");
    }
    if ((arch == Arch::ZYNQ) && (type == Checksum::SHA3))
    {
        
        LOG_ERROR("BIF attribute error !!!\n\t\t'checksum=sha3' not supported in ZYNQ architecture");
    }
    if ((arch == Arch::ZYNQMP) && (type == Checksum::MD5))
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'checksum=md5' not supported in ZYNQMP architecture");
    }
    checksumType = type;
}

/******************************************************************************/
void PartitionBifOptions::SetOwnerType(PartitionOwner::Type type)
{
    ownerType = type;
}

/******************************************************************************/
void PartitionBifOptions::SetPartitionType(PartitionType::Type type)
{
    partitionType = type;
}

/******************************************************************************/
void PartitionBifOptions::SetDpaCM(DpaCM::Type value)
{
    dpaCM = value;
}

/******************************************************************************/
void PartitionBifOptions::SetExceptionLevel(ExceptionLevel::Type type)
{
    if (arch == Arch::ZYNQ)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'exception_level' not supported in ZYNQ architecture");
    }
    exceptionLevel = type;
}

/******************************************************************************/
void PartitionBifOptions::SetSpkId(uint32_t id)
{
    if (arch != Arch::ZYNQMP)
    {
        //LOG_ERROR("BIF attribute error !!!\n\t\t'spk_id' is supported only in ZYNQMP architecture");
    }
    spkId = id;
    spkIdLocal = true;
}

/******************************************************************************/
void BifOptions::SetFsblFilename(std::string filename)
{
    fsblFilename = filename;
}

/******************************************************************************/
void PartitionBifOptions::SetTrustZone(TrustZone::Type type)
{
    if (arch == Arch::ZYNQ)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'trustzone' not supported in ZYNQ architecture");
    }
    trustzone = type;
}

/******************************************************************************/
void PartitionBifOptions::SetEarlyHandoff(bool flag)
{
    if (arch == Arch::ZYNQ)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'early_handoff' not supported in ZYNQ architecture");
    }
    if (bootloader == true)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'early_handoff' not supported for bootloader partitions");
    }
    early_handoff = flag;
}

/******************************************************************************/
void PartitionBifOptions::SetHivec(bool flag)
{
    if (arch == Arch::ZYNQ)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'hivec' not supported in ZYNQ architecture");
    }
    hivec = flag;
}

/******************************************************************************/
void PartitionBifOptions::SetRevokeId(uint32_t id)
{
    if (revokeId > 0xFF)
    {
        LOG_ERROR("revoke_id can only take values from 0x0 to 0xFF.");
    }
    revokeId = id;
}

/******************************************************************************/
void PartitionBifOptions::SetSlrNum(uint8_t id)
{
    slrNum = id;
}

/******************************************************************************/
std::vector<uint32_t>& PartitionBifOptions::GetEncryptionBlocks(void)
{
    return blocks;
}

/******************************************************************************/
std::vector<uint32_t>& BifOptions::GetEncryptionBlocksList(void)
{
    return blocks;
}

/******************************************************************************/
void PartitionBifOptions::SetAuthBlockAttr(size_t authBlockAttr)
{
    if (authBlockAttr != 4 && authBlockAttr != 8 && authBlockAttr != 16 && authBlockAttr != 32 && authBlockAttr != 64)
    {
        LOG_ERROR("'-authblock' option supports only 4,8,16,32,64 sizes (taken in MB)");
    }
    if (arch != Arch::ZYNQMP)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'-authblock' option supported only for ZYNQMP architecture '-arch zynqmp'");
    }
    authblockattr = authBlockAttr;
}

/******************************************************************************/
void PartitionBifOptions::SetPufHdLocation(PufHdLoc::Type type)
{
    pufHdLoc = type;
}

/******************************************************************************/
uint32_t PartitionBifOptions::GetDefaultEncryptionBlockSize(void)
{
    return defBlockSize;
}

/******************************************************************************/
KeySource::Type PartitionBifOptions::GetEncryptionKeySource(void)
{
    return keySrc;
}

/******************************************************************************/
DpaCM::Type PartitionBifOptions::GetDpaCM(void)
{
    return dpaCM;
}

/******************************************************************************/
uint32_t PartitionBifOptions::GetRevokeId(void)
{
    return revokeId;
}

/******************************************************************************/
PufHdLoc::Type PartitionBifOptions::GetPufHdLocation(void)
{
    return pufHdLoc;
}

/******************************************************************************/
void BifOptions::SetTotalPmcFwSize(uint32_t size)
{
    totalpmcdataSize = size;
}

/******************************************************************************/
void BifOptions::SetPmcFwSize(uint32_t size)
{
    pmcdataSize = size;
}

/******************************************************************************/
void BifOptions::SetPPKSelection(uint32_t ppkSelection)
{
    if ((ppkSelection == 0) | (ppkSelection == 1))
    {
        ppkSelect = ppkSelection;
        isPPKSelectGlobal = true;
        LOG_TRACE("Setting PPK Selection in Auth Certificate as %d", ppkSelect);
    }
    else
    {
        LOG_DEBUG(DEBUG_STAMP, "PPK Select - %d", ppkSelect);
        LOG_ERROR("BIF attribute error !!!\n\t\t'ppk_select' can either be 0 or 1");
    }
}

/******************************************************************************/
void BifOptions::SetSPKSelection(uint32_t spkSelection)
{
    spkSelect = spkSelection;
    isSPKSelectGlobal = true;
    LOG_TRACE("Setting SPK Selection in Auth Certificate as %d", spkSelect);
}

/******************************************************************************/
void BifOptions::SetSpkId(uint32_t id)
{
    spkId = id;
    isSpkIdGlobal = true;
    LOG_TRACE("Setting SPK ID in Auth Certificate as 0x%x", id);
}

/******************************************************************************/
void BifOptions::SetHeaderAuthentication()
{
    headerAuthParam = true;
    SetHeaderAC(true);
    LOG_TRACE("Setting Header Authentication");
}

/******************************************************************************/
std::string BifOptions::GetFamilyKeyFileName()
{
    return familyKeyFile;
}

/******************************************************************************/
BhRsa::Type BifOptions::GetBhRsa(void)
{
    return bhAuthEnable;
}

/******************************************************************************/
BootDevice::Type BifOptions::GetBootDevice(void)
{
    return bootDevice;
}

/******************************************************************************/
uint32_t BifOptions::GetBootDeviceAddress(void)
{
    return bootDeviceAddress;
}

/******************************************************************************/
uint32_t BifOptions::GetSmapWidth(void)
{
    return smapWidth;
}

/******************************************************************************/
uint32_t BifOptions::GetPmcCdoLoadAddress(void)
{
    return pmcCdoLoadAddress;
}

/******************************************************************************/
uint32_t BifOptions::GetPmcFwSize(void)
{
    return pmcdataSize;
}

/******************************************************************************/
uint32_t BifOptions::GetTotalPmcFwSize(void)
{
    return totalpmcdataSize;
}

/******************************************************************************/
uint32_t BifOptions::GetPdiId(void)
{
    return pdiId;
}

/******************************************************************************/
uint32_t BifOptions::GetParentId(void)
{
    return parentId;
}

/******************************************************************************/
KeySource::Type BifOptions::GetEncryptionKeySource(void)
{
    return keySourceEncryption;
}

/******************************************************************************/
bool BifOptions::GetPufHdinBHFlag(void)
{
    return pufHdinBHEnable;
}

/******************************************************************************/
uint8_t* BifOptions::GetPmcDataBuffer(void)
{
    return pmcDataBuffer;
}

/******************************************************************************/
uint32_t BifOptions::GetTotalpmcdataSize(void)
{
    return totalpmcdataSize;
}

/******************************************************************************/
std::string BifOptions::GetPmcdataFile(void)
{
    return pmcdataFile;
}

std::string BifOptions::GetPmcDataAesFile(void)
{
    return pmcDataAesFile;
}

/******************************************************************************/
PufMode::Type BifOptions::GetPufMode(void)
{
    return pufMode;
}

/******************************************************************************/
PufHdLoc::Type BifOptions::GetPufHdLoc(void)
{
    return pufHdLoc;
}

/******************************************************************************/
OptKey::Type BifOptions::GetOptKey(void)
{
    return optKey;
}

/******************************************************************************/
AuthOnly::Type BifOptions::GetAuthOnly(void)
{
    return authOnly;
}

/******************************************************************************/
uint32_t BifOptions::GetPpkSelection(void)
{
    return ppkSelect;
}

/******************************************************************************/
bool BifOptions::GetPpkSelectionGlobal()
{
    return isPPKSelectGlobal;
}

/******************************************************************************/
std::string BifOptions::GetPPKFileName(void)
{
    return ppkFile;
}
/******************************************************************************/
std::string BifOptions::GetPSKFileName(void)
{
    return pskFile;
}

/******************************************************************************/
uint32_t BifOptions::GetSpkSelection(void)
{
    return spkSelect;
}

/******************************************************************************/
bool BifOptions::GetSpkSelectionGlobal()
{
    return isSPKSelectGlobal;
}

/******************************************************************************/
std::string BifOptions::GetSPKFileName(void)
{
    return spkFile;
}

/******************************************************************************/
std::string BifOptions::GetSSKFileName(void)
{
    return sskFile;
}

/******************************************************************************/
uint32_t BifOptions::GetSpkId(void)
{
    return spkId;
}

/******************************************************************************/
bool BifOptions::GetSpkIdGlobal()
{
    return isSpkIdGlobal;
}

/******************************************************************************/
bool BifOptions::GetHeaderAC()
{
    return createHeaderAC;
}

/******************************************************************************/
bool BifOptions::GetHeaderEncyption()
{
    return doHeaderEncryption;
}

/******************************************************************************/
std::string BifOptions::GetBhKeyFile(void)
{
    return bhKeyFile;
}

/******************************************************************************/
uint32_t BifOptions::GetShutterValue(void)
{
    return shutterVal;
}

/******************************************************************************/
DpaCM::Type BifOptions::GetDpaCM(void)
{
    return dpaCM;
}

/******************************************************************************/
std::string BifOptions::GetHeaderSignatureFile(void)
{
    return headerSignatureFile;
}

/******************************************************************************/
std::string BifOptions::GetPufHelperFile(void)
{
    return pufHelperFile;
}

/******************************************************************************/
std::string BifOptions::GetPmuFwImageFile(void)
{
    return pmuFwImageFile;
}

/******************************************************************************/
std::string BifOptions::GetBHKekIVFile(void)
{
    return bhKekIVFile;
}

/******************************************************************************/
std::string BifOptions::GetBbramKekIVFile(void)
{
    return bbramKekIVFile;
}

/******************************************************************************/
std::string BifOptions::GetEfuseKekIVFile(void)
{
    return efuseKekIVFile;
}

/******************************************************************************/
std::string BifOptions::GetEfuseUserKek0IVFile(void)
{
    return efuseUserKek0IVFile;
}

/******************************************************************************/
std::string BifOptions::GetEfuseUserKek1IVFile(void)
{
    return efuseUserKek1IVFile;
}

/******************************************************************************/
std::string BifOptions::GetUdfBhFile(void)
{
    return udfBhFile;
}

/******************************************************************************/
SplitMode::Type BifOptions::GetSplitMode(void)
{
    return splitMode;
}

/******************************************************************************/
File::Type BifOptions::GetSplitFormat(void)
{
    return splitFmt;
}

/******************************************************************************/
std::vector<uint32_t>& BifOptions::GetBootVectorArray(void)
{
    return bootvectors;
}

/******************************************************************************/
std::string BifOptions::GetSPKSignFileName(void)
{
    return spkSignatureFile;
}

/******************************************************************************/
std::string BifOptions::GetBHSignFileName(void)
{
    return bhSignatureFile;
}

/******************************************************************************/
void BifOptions::SetAuthOnly(AuthOnly::Type type)
{
    authOnly = type;
}

/******************************************************************************/
void BifOptions::SetPdiId(uint32_t id)
{
    pdiId = id;
}

/******************************************************************************/
void BifOptions::SetIdCode(uint32_t id)
{
    idCode = id;
}

/******************************************************************************/
void BifOptions::SetExtendedIdCode(uint32_t id)
{
    if (id > 0x3F)
    {
        LOG_ERROR("Invalid extended ID code. Maximum length of extended_id_code is 6-bits");
    }
    extendedIdCode = id;
}

/******************************************************************************/
void BifOptions::SetBypassIdcodeFlag(bool flag)
{
    bypassIdCode = flag;
}

/******************************************************************************/
void BifOptions::SetParentId(uint32_t id)
{
    parentId = id;
}

/******************************************************************************/
void BifOptions::SetBhRsa(BhRsa::Type value)
{
    bhAuthEnable = value;
}

/******************************************************************************/
void BifOptions::SetPufHdLoc(PufHdLoc::Type value)
{
    pufHdLoc= value;
}

/******************************************************************************/
void BifOptions::SetCore(Core::Type type)
{
    core = type;
}

/******************************************************************************/
Core::Type BifOptions::GetCore(void)
{
    return core;
}

/******************************************************************************/
void BifOptions::SetDpaCM(DpaCM::Type value)
{
    dpaCM = value;
}

/******************************************************************************/
uint32_t BifOptions::GetIdCode(void)
{
    return idCode;
}

/******************************************************************************/
uint32_t BifOptions::GetExtendedIdCode(void)
{
    return extendedIdCode;
}

/******************************************************************************/
bool BifOptions::GetBypassIdcodeFlag(void)
{
    return bypassIdCode;
}

/******************************************************************************/
void PartitionBifOptions::SetUdfDataFile(std::string filename)
{
    if (arch == Arch::VERSAL)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'udf_data' is not supported in VERSAL architecture");
    }
    udfDataFile = filename;
}

/******************************************************************************/
std::string PartitionBifOptions::GetUdfDataFile(void)
{
    return udfDataFile;
}

/******************************************************************************/
void ImageBifOptions::SetDelayHandoff(bool flag)
{
    if (!delay_handoff_warning_given)
    {
        delay_handoff_warning_given = true;
        LOG_WARNING("delay_handoff is specified, this may cause some issues if not handled properly");
    }
    delayHandoff = flag;
}

/******************************************************************************/
void ImageBifOptions::SetDelayLoad(bool flag)
{
    if (!delay_load_warning_given)
    {
        delay_load_warning_given = true;
        LOG_WARNING("delay_load is specified, this may cause some issues if not handled properly");
    }
    delayLoad = flag;
}