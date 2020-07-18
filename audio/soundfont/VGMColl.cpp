/*
 * VGMTrans (c) 2002-2019
 * Licensed under the zlib license,
 * refer to the included LICENSE.txt file
 */

#include <common/debug.h>
#include "VGMColl.h"
//#include "VGMSeq.h"
#include "VGMInstrSet.h"
#include "VGMSampColl.h"
#include "VGMSamp.h"
#include "VGMRgn.h"
//#include "ScaleConversion.h"
//#include "Root.h"

using namespace std;

//DECLARE_MENU(VGMColl)

double ConvertLogScaleValToAtten(double percent) {
	if (percent == 0)
		return 100.0;  // assume 0 is -100.0db attenuation
	double atten = 20 * log10(percent) * 2;
	return MIN(-atten, 100.0);
}

// Convert a percent of volume value to it's attenuation in decibels.
//  ex: ConvertPercentVolToAttenDB_SF2(0.5) returns -(-6.02db) = half perceived loudness
double ConvertPercentAmplitudeToAttenDB_SF2(double percent) {
	if (percent == 0)
		return 100.0;  // assume 0 is -100.0db attenuation
	double atten = 20 * log10(percent);
	return MIN(-atten, 100.0);
}

VGMColl::VGMColl(Common::String theName) : VGMItem(), name(std::move(theName)), seq(nullptr) {}

void VGMColl::RemoveFileAssocs() {
    for (auto set : instrsets) {
        set->RemoveCollAssoc(this);
    }
    for (auto samp : sampcolls) {
        samp->RemoveCollAssoc(this);
    }
    for (auto file : miscfiles) {
        file->RemoveCollAssoc(this);
    }
}

const Common::String &VGMColl::GetName() const {
    return name;
}

void VGMColl::SetName(const Common::String *newName) {
    name = *newName;
}

void VGMColl::AddInstrSet(VGMInstrSet *theInstrSet) {
    if (theInstrSet != nullptr) {
        theInstrSet->AddCollAssoc(this);
        instrsets.push_back(theInstrSet);
    }
}

void VGMColl::AddSampColl(VGMSampColl *theSampColl) {
    if (theSampColl != nullptr) {
        theSampColl->AddCollAssoc(this);
        sampcolls.push_back(theSampColl);
    }
}

void VGMColl::AddMiscFile(VGMFile *theMiscFile) {
    if (theMiscFile != nullptr) {
        theMiscFile->AddCollAssoc(this);
        miscfiles.push_back(theMiscFile);
    }
}

bool VGMColl::Load() {
    if (!LoadMain())
        return false;
//    pRoot->AddVGMColl(this);
    return true;
}

void VGMColl::UnpackSampColl(SynthFile &synthfile, VGMSampColl *sampColl,
                             Common::Array<VGMSamp *> &finalSamps) {
    assert(sampColl != nullptr);

    size_t nSamples = sampColl->samples.size();
    for (size_t i = 0; i < nSamples; i++) {
        VGMSamp *samp = sampColl->samples[i];

        uint32_t bufSize;
        if (samp->ulUncompressedSize)
            bufSize = samp->ulUncompressedSize;
        else
            bufSize = (uint32_t)ceil((double)samp->dataLength * samp->GetCompressionRatio());

        uint8_t *uncompSampBuf =
            new uint8_t[bufSize];  // create a new memory space for the uncompressed wave
        samp->ConvertToStdWave(uncompSampBuf);  // and uncompress into that space

        uint16_t blockAlign = samp->bps / 8 * samp->channels;
        SynthWave *wave =
            synthfile.AddWave(1, samp->channels, samp->rate, samp->rate * blockAlign, blockAlign,
                              samp->bps, bufSize, uncompSampBuf, (samp->name));
        finalSamps.push_back(samp);

        // If we don't have any loop information, then don't create a sampInfo structure for the
        // Wave
        if (samp->loop.loopStatus == -1) {
            debug("No loop information for %s - some parameters might be incorrect",
                    samp->sampName.c_str());
            return;
        }

        SynthSampInfo *sampInfo = wave->AddSampInfo();
        if (samp->bPSXLoopInfoPrioritizing) {
            if (samp->loop.loopStart != 0 || samp->loop.loopLength != 0)
                sampInfo->SetLoopInfo(samp->loop, samp);
        } else
            sampInfo->SetLoopInfo(samp->loop, samp);

        double attenuation = (samp->volume != -1) ? ConvertLogScaleValToAtten(samp->volume) : 0;
        uint8_t unityKey = (samp->unityKey != -1) ? samp->unityKey : 0x3C;
        short fineTune = samp->fineTune;
        sampInfo->SetPitchInfo(unityKey, fineTune, attenuation);
    }
}

SF2File *VGMColl::CreateSF2File() {
    SynthFile *synthfile = CreateSynthFile();
    if (!synthfile) {
        debug("SF2 conversion for '%s' aborted", name.c_str());
        return nullptr;
    }

    SF2File *sf2file = new SF2File(synthfile);
    delete synthfile;
    return sf2file;
}

SynthFile *VGMColl::CreateSynthFile() {
    if (instrsets.empty()) {
        debug("%s has no instruments", name.c_str());
        return nullptr;
    }

    /* FIXME: shared_ptr eventually */
    SynthFile *synthfile = new SynthFile("SynthFile");

    Common::Array<VGMSamp *> finalSamps;
    Common::Array<VGMSampColl *> finalSampColls;

    /* Grab samples either from the local sampcolls or from the instrument sets */
    if (!sampcolls.empty()) {
        for (int sam = 0; sam < sampcolls.size(); sam++) {
            finalSampColls.push_back(sampcolls[sam]);
            UnpackSampColl(*synthfile, sampcolls[sam], finalSamps);
        }
    } else {
        for (int i = 0; i < instrsets.size(); i++) {
            auto instrset_sampcoll = instrsets[i]->sampColl;
            if (instrset_sampcoll) {
                finalSampColls.push_back(instrset_sampcoll);
                UnpackSampColl(*synthfile, instrset_sampcoll, finalSamps);
            }
        }
    }

    if (finalSamps.empty()) {
        debug("No sample collection present for '%s'", name.c_str());
        delete synthfile;
        return nullptr;
    }

    for (size_t inst = 0; inst < instrsets.size(); inst++) {
        VGMInstrSet *set = instrsets[inst];
        size_t nInstrs = set->aInstrs.size();
        for (size_t i = 0; i < nInstrs; i++) {
            VGMInstr *vgminstr = set->aInstrs[i];
            size_t nRgns = vgminstr->aRgns.size();
            if (nRgns == 0)  // do not write an instrument if it has no regions
                continue;
            SynthInstr *newInstr = synthfile->AddInstr(vgminstr->bank, vgminstr->instrNum);
            for (uint32_t j = 0; j < nRgns; j++) {
                VGMRgn *rgn = vgminstr->aRgns[j];
                //				if (rgn->sampNum+1 > sampColl->samples.size())
                ////does thereferenced sample exist? 					continue;

                // Determine the SampColl associated with this rgn.  If there's an explicit pointer
                // to it, use that.
                VGMSampColl *sampColl = rgn->sampCollPtr;
                if (!sampColl) {
                    // If rgn is of an InstrSet with an embedded SampColl, use that SampColl.
                    if (((VGMInstrSet *)rgn->vgmfile)->sampColl)
                        sampColl = ((VGMInstrSet *)rgn->vgmfile)->sampColl;

                    // If that does not exist, assume the first SampColl
                    else
                        sampColl = finalSampColls[0];
                }

                // Determine the sample number within the rgn's associated SampColl
                size_t realSampNum;
                // If a sample offset is provided, then find the sample number based on this offset.
                // see sampOffset declaration in header file for more info.
                if (rgn->sampOffset != -1) {
                    bool bFoundIt = false;
                    for (uint32_t s = 0; s < sampColl->samples.size(); s++) {  // for every sample
                        if (rgn->sampOffset == sampColl->samples[s]->dwOffset - sampColl->dwOffset -
                                                   sampColl->sampDataOffset) {
                            realSampNum = s;

                            // samples[m]->loop.loopStart =
                            // parInstrSet->aInstrs[i]->aRgns[k]->loop.loopStart;
                            // samples[m]->loop.loopLength = (samples[m]->dataLength) -
                            // (parInstrSet->aInstrs[i]->aRgns[k]->loop.loopStart);
                            // //[aInstrs[i]->aRegions[k]->sample_num]->dwUncompSize/2) -
                            // ((aInstrs[i]->aRegions[k]->loop_point*28)/16); //to end of sample
                            bFoundIt = true;
                            break;
                        }
                    }
                    if (!bFoundIt) {
                        debug("Failed matching region to a sample with offset %X (Instrset "
                                "%d, Instr %d, Region %d)",
                                rgn->sampOffset, inst, i, j);
                        realSampNum = 0;
                    }
                }
                // Otherwise, the sample number should be explicitly defined in the rgn.
                else
                    realSampNum = rgn->sampNum;

                // Determine the sampCollNum (index into our finalSampColls vector)
                auto sampCollNum = finalSampColls.size();
                for (size_t i = 0; i < finalSampColls.size(); i++) {
                    if (finalSampColls[i] == sampColl)
                        sampCollNum = i;
                }
                if (sampCollNum == finalSampColls.size()) {
                    debug("SampColl does not exist");
                    return nullptr;
                }
                //   now we add the number of samples from the preceding SampColls to the value to
                //   get the real sampNum in the final DLS file.
                for (uint32_t k = 0; k < sampCollNum; k++)
                    realSampNum += finalSampColls[k]->samples.size();

                SynthRgn *newRgn = newInstr->AddRgn();
                newRgn->SetRanges(rgn->keyLow, rgn->keyHigh, rgn->velLow, rgn->velHigh);
                newRgn->SetWaveLinkInfo(0, 0, 1, (uint32_t)realSampNum);

                if (realSampNum >= finalSamps.size()) {
                    debug("Sample %d does not exist", realSampNum);
                    realSampNum = finalSamps.size() - 1;
                }

                VGMSamp *samp = finalSamps[realSampNum];  // sampColl->samples[rgn->sampNum];
                SynthSampInfo *sampInfo = newRgn->AddSampInfo();

                // This is a really loopy way of determining the loop information, pardon the pun.
                // However, it works. There might be a way to simplify this, but I don't want to
                // test out whether another method breaks anything just yet Use the sample's
                // loopStatus to determine if a loop occurs.  If it does, see if the sample provides
                // loop info (gathered during ADPCM > PCM conversion.  If the sample doesn't provide
                // loop offset info, then use the region's loop info.
                if (samp->bPSXLoopInfoPrioritizing) {
                    if (samp->loop.loopStatus != -1) {
                        if (samp->loop.loopStart != 0 || samp->loop.loopLength != 0)
                            sampInfo->SetLoopInfo(samp->loop, samp);
                        else {
                            rgn->loop.loopStatus = samp->loop.loopStatus;
                            sampInfo->SetLoopInfo(rgn->loop, samp);
                        }
                    } else {
                        delete synthfile;
                        error("argh"); //TODO
                    }
                }
                // The normal method: First, we check if the rgn has loop info defined.
                // If it doesn't, then use the sample's loop info.
                else if (rgn->loop.loopStatus == -1) {
                    if (samp->loop.loopStatus != -1)
                        sampInfo->SetLoopInfo(samp->loop, samp);
                    else {
                        delete synthfile;
                        error("argh2"); //TODO
                    }
                } else
                    sampInfo->SetLoopInfo(rgn->loop, samp);

                int8_t realUnityKey = -1;
                if (rgn->unityKey == -1)
                    realUnityKey = samp->unityKey;
                else
                    realUnityKey = rgn->unityKey;
                if (realUnityKey == -1)
                    realUnityKey = 0x3C;

                short realFineTune;
                if (rgn->fineTune == 0)
                    realFineTune = samp->fineTune;
                else
                    realFineTune = rgn->fineTune;

                double attenuation;
                if (rgn->volume != -1)
                    attenuation = ConvertLogScaleValToAtten(rgn->volume);
                else if (samp->volume != -1)
                    attenuation = ConvertLogScaleValToAtten(samp->volume);
                else
                    attenuation = 0;

                double sustainLevAttenDb;
                if (rgn->sustain_level == -1)
                    sustainLevAttenDb = 0.0;
                else
                    sustainLevAttenDb = ConvertPercentAmplitudeToAttenDB_SF2(rgn->sustain_level);

                SynthArt *newArt = newRgn->AddArt();
                newArt->AddPan(rgn->pan);
                newArt->AddADSR(rgn->attack_time, (Transform)rgn->attack_transform, rgn->decay_time,
                                sustainLevAttenDb, rgn->sustain_time, rgn->release_time,
                                (Transform)rgn->release_transform);

                sampInfo->SetPitchInfo(realUnityKey, realFineTune, attenuation);
            }
        }
    }
    return synthfile;
}
