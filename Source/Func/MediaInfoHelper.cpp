/*

    Image Uploader -  free application for uploading images/files to the Internet

    Copyright 2007-2015 Sergey Svistunov (zenden2k@gmail.com)

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

*/

#include "MediaInfoHelper.h"

#include "Core/CommonDefs.h"
#include "Core/3rdpart/MediaInfoDLL.h"
#include "LangClass.h"
#include "WinUtils.h"

namespace MediaInfoHelper {

void AddStr(CString &Str, const CString& Str2)
{
    if (Str2.GetLength() > 0) Str += Str2;
}

void AddStr(CString &Str, const CString& Str2, const CString& StrPostfix, const CString& StrPrefix = CString(_T("")))
{
    if (Str2.GetLength() > 0) Str += StrPrefix + Str2 + StrPostfix;
}

inline CString& replace(CString &text, CString s, CString d)
{
    text.Replace(s, d);
    return text;
}

#define VIDEO(a) MI.Get(Stream_Video, 0, _T(a), Info_Text, Info_Name)
#define AUDIO(n, a) MI.Get(Stream_Audio, n, _T(a), Info_Text, Info_Name)

std::string timestampToStr(int64_t duration, int64_t units) {
    int hours, mins, secs, us;
    secs = static_cast<int>(duration / units);
    us = static_cast<int>(duration % units);
    mins = secs / 60;
    secs %= 60;
    hours = mins / 60;
    mins %= 60;
    char buffer[100];
    sprintf(buffer, "%02d:%02d:%02d", hours, mins, secs/*, (int)((100 * us) / units)*/);
    return buffer;
}

bool FindMediaInfoDllPath() {
    *MediaInfoDllPath = 0;

    CString MediaDll = WinUtils::GetAppFolder() + _T("\\Modules\\MediaInfo.dll");
    if (WinUtils::FileExists(MediaDll)) {
        lstrcpy(MediaInfoDllPath, MediaDll);
        return true;
    }
    else {
        TCHAR Buffer[MAX_PATH];
        HKEY ExtKey;
        Buffer[0] = 0;
        RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\KLCodecPack"), 0,/* REG_OPTION_NON_VOLATILE, */KEY_QUERY_VALUE, &ExtKey/* NULL*/);
        TCHAR ClassName[MAX_PATH] = _T("\0");
        DWORD BufSize = sizeof(ClassName) / sizeof(TCHAR);
        DWORD Type = REG_SZ;
        RegQueryValueEx(ExtKey, _T("installdir"), 0, &Type, reinterpret_cast<LPBYTE>(&ClassName), &BufSize);
        RegCloseKey(ExtKey);
        CString MediaDll2 = CString(ClassName) + _T("\\Tools\\MediaInfo.dll");
        if (WinUtils::FileExists(MediaDll2)) {
            lstrcpy(MediaInfoDllPath, MediaDll2);
            return true;
        }
    }
    return false;
}

bool IsMediaInfoAvailable() {
    return *MediaInfoDllPath != _T('\0');
}

bool GetMediaFileInfo(LPCWSTR FileName, CString &Buffer)
{
    using namespace MediaInfoDLL;
    MediaInfo MI;

    if (!MI.IsReady()) {
        Buffer = TR("Unable to load library MediaInfo.dll!");
        return FALSE;
    }

    MI.Open(FileName);

    CString Result;
    CString VideoFormat, VideoVersion, VideoCodec, VideoWidth, VideoHeight, VideoAspectRatio, VideoFrameRate, VideoBitrate, VideoNominalBitrate, VideoBitsperPixel;
    CString AudioFormat, AudioFormatProfile, AudioSampleRate, AudioChannels, AudioBitrate, AudioBitrateMode, AudioLanguage;

    int count = MI.Count_Get(Stream_Audio); //Count of audio streams in file
    int VideoCount = MI.Count_Get(Stream_Video); //Count of video streams in file
    int SubsCount = MI.Count_Get(Stream_Text);

    Result += CString(TR("Filename: ")) + WinUtils::myExtractFileName(FileName);
    Result += _T("\r\n");
    CString fileSize = MI.Get(Stream_General, 0, _T("FileSize/String"), Info_Text, Info_Name);;
    fileSize.Replace(_T("iB"), _T("B")); // MiB --> MB
    Result += CString(TR("Filesize: ")) + fileSize;
    Result += _T("\r\n");
    CString Duration;
    CString DurationStr = MI.Get(Stream_General, 0, _T("Duration"), Info_Text, Info_Name);
    if (!DurationStr.IsEmpty()) {
        uint64_t duration = IuCoreUtils::stringToInt64(W2U(DurationStr));
        Duration = U2W(timestampToStr(duration, 1000));
    } else {
        Duration = MI.Get(Stream_General, 0, _T("Duration/String"), Info_Text, Info_Name);
    }

    AddStr(Result, Duration, CString(_T("\r\n")), CString(TR("Duration: ")));

    if (count + VideoCount > 1) // if file contains more than one audio/video stream 
    {
        AddStr(Result, MI.Get(Stream_General, 0, _T("OverallBitRate/String"), Info_Text, Info_Name),
            CString(_T("\r\n")), CString(TR("Overall bitrate: ")));
    }

    if (VideoCount) // If file contains one or more video streams
    {
        // The next piece of code gets information only from the first video stream
        // e.g. there is no support for multiple video streams
        CString VideoTotal = TR("Video: ");
        VideoFormat = VIDEO("Format");
        VideoVersion = VIDEO("Format_Version");

        AddStr(VideoTotal, VideoFormat);
        replace(VideoVersion, CString(_T("Version ")), CString(_T("")));

        if (VideoVersion.GetLength()) {
            AddStr(VideoTotal, CString(_T(" ")), VideoVersion);
        }

        VideoCodec = VIDEO("CodecID/Hint");
        if (VideoCodec.GetLength())
            AddStr(VideoTotal, CString(_T(", ")), VideoCodec);
        AddStr(VideoTotal, CString(_T(", ")));
        AddStr(VideoTotal, VIDEO("Width"));
        AddStr(VideoTotal, CString(_T("x")));
        AddStr(VideoTotal, VIDEO("Height"));
        replace(VideoTotal, CString(_T("MPEG Video")), CString(_T("MPEG")));
        CString DisplayRatio;
        CString temp;
        replace(VideoTotal, CString(_T("MPEG-4 Visual")), CString(_T("MPEG4")));
        DisplayRatio = VIDEO("DisplayAspectRatio/String");
        if (DisplayRatio.GetLength()) {
            AddStr(VideoTotal, CString(_T(" (")));
            AddStr(VideoTotal, DisplayRatio);
            AddStr(VideoTotal, CString(_T(")")));
        }

        VideoFrameRate = VIDEO("FrameRate");
        if (VideoFrameRate.GetLength()) {
            AddStr(VideoTotal, CString(_T(", ")));
            AddStr(VideoTotal, VideoFrameRate);
            AddStr(VideoTotal, CString(_T(" fps")));
        }

        VideoNominalBitrate = VIDEO("BitRate_Nominal/String");
        VideoBitrate = VIDEO("BitRate/String");

        if (VideoNominalBitrate.GetLength()) // Nominal bitrate value has higher priority than Actual bitrate
        {
            AddStr(VideoTotal, CString(_T(", ")));
            AddStr(VideoTotal, VideoNominalBitrate);
        } else if (VideoBitrate.GetLength()) {
            AddStr(VideoTotal, CString(_T(", ")));
            AddStr(VideoTotal, VideoBitrate);
        }

        VideoBitsperPixel = VIDEO("Bits-(Pixel*Frame)");
        if (VideoBitsperPixel.GetLength()) {
            AddStr(VideoTotal, CString(_T(" (")));
            AddStr(VideoTotal, VideoBitsperPixel);
            AddStr(VideoTotal, CString(_T(" bit/pixel)")));
        }

        Result += VideoTotal;
        Result += _T("\r\n");
    }  // End of getting information about video stream

    CString CountString = MI.Get(Stream_Audio, 0, _T("StreamCount"), Info_Text, Info_Name);

    for (int i = 0; i < count; i++) {
        CString AudioTotal;
        TCHAR buf[256];
        wsprintf(buf, CString(TR("Audio")) + _T(" #%d: "), i + 1);

        if (count > 1)
            AudioTotal = buf;
        else
            AudioTotal += CString(TR("Audio")) + _T(": ");

        AudioFormat = AUDIO(i, "Format");
        AudioFormatProfile = AUDIO(i, "Format_Profile");
        AudioSampleRate = AUDIO(i, "SamplingRate/String");
        AudioChannels = AUDIO(i, "Channel(s)");
        AudioBitrate = AUDIO(i, "BitRate/String");
        AudioBitrateMode = AUDIO(i, "BitRate_Mode");
        AudioLanguage = AUDIO(i, "Language/String");

        AddStr(AudioTotal, AudioFormat);
        AddStr(AudioTotal, AudioFormatProfile, CString(_T("")), CString(_T(" ")));
        AddStr(AudioTotal, AudioSampleRate, CString(_T("")), CString(_T(", ")));
        AddStr(AudioTotal, AudioChannels, CString(_T(" ch")), CString(_T(", ")));
        AddStr(AudioTotal, AudioBitrate, CString(_T("")), CString(_T(", ")));
        AddStr(AudioTotal, AudioBitrateMode, CString(_T("")), CString(_T(", ")));
        AddStr(AudioTotal, AudioLanguage, CString(_T(")")), CString(_T(" (")));
        AudioTotal += _T("\r\n");
        replace(AudioTotal, CString(_T("MPEG Audio Layer ")), CString(_T("MP")));
        replace(AudioTotal, CString(_T(" ,")), CString(_T("")));
        replace(AudioTotal, CString(_T(",,")), CString(_T(",")));
        Result += AudioTotal;
    } // End of getting information about audio streams

    if (SubsCount > 0) {
        CString SubsTotal;

        for (int i = 0; i < SubsCount; i++) {
            CString mode = MI.Get(Stream_Text, i, _T("Language_More"), Info_Text, Info_Name);
            AddStr(SubsTotal, MI.Get(Stream_Text, i, _T("Language/String"), Info_Text, Info_Name), CString(_T("")), CString(i ? _T(", ") : _T("")));

            if (mode == _T("Forced")) AddStr(SubsTotal, CString(_T(" (forced)")));
        }

        if (SubsTotal.GetLength()) {
            SubsTotal = CString(TR("Subtitles: ")) + SubsTotal;
            Result += SubsTotal;
            Result += _T("\r\n");
        }
    }

    MI.Close();
    Buffer = Result;
    return TRUE;
}

}