# Introduction 
This NuGet package contains the XAudio 2.9 redistributable for Windows 7 SP1, Windows 8, Windows 8.1 and Windows 10.

# Documentation
Please refer to the [on-line developer guide](https://aka.ms/XAudio2Redist) for information about how to use this NuGet package.

# Version history

## Version 1.2.1
- Updated XAudio2Redist.h header to make IXAudio2::RegisterForCallbacks and IXAudio2::UnregisterForCallbacks callable from C code.

## Version 1.2.0
- Fixed bug in the IXAudio2Voice::SetOutputMatrix function that caused panning of stereo signals to be wrong when the PC suppports AVX instructions.

## Version 1.1.1
- Defined a USING_XAUDIO2_REDIST preprocessor symbol, which can be used in "#ifdef USING_XAUDIO2_REDIST" statements, if there is a need to do things differently when the NuGet package is installed.

## Version 1.1.0
- Fixed bug that causes CreateMasteringVoice to fail on Windows 8 and 8.1 when a Virtual Audio Client is requested. As a consequence of this fix, the Virtual Audio Client feature only functions on Windows 10.

## Version 1.0.4
- Always link with Release version of xaudio2_9redist.lib even if the MSVC project is configured for a Debug build.

## Version 1.0.3
- Always deploy Release version of xaudio2_9redist.dll even if the MSVC project is configured for a Debug build. The Debug version of xaudio2_9redist.dll is still included in the NuGet package, but if you have a special reason to use it you will have to manually modify the project to copy it to the output directory.
- Fixed Debug-only assert failure in WMA decoder.

## Version 1.0.2
- Optimize xapobaseredist.lib for speed.
- Enable full debug info in xaudio2_9redist.lib.
- Include new library called XapoBaseRedist_md.lib, which is a version of XapoBaseRedist.lib that depends on the dynamic runtime library. Use this if you get a warning about libcmt.lib causing a conflict.
  Set an environment variable or property in the vcxproj file called XAPODynamicCRT to 1, to automatically make the project link with XapoBaseRedist_md.lib.

## Version 1.0.1

- Fixed bug where the CreateAudioVolumeMeter API was invoking CreateAudioReverb instead.
- Added xaudio2_9redist.pdb to the NuGet package.
- NuGet package now includes both Release and Debug versions of the DLLs and LIBs.
- Disabled Whole Program Optimization flag in Release version of XapoBaseRedist.lib for better compatibility with older compilers.

## Version 1.0.0

Initial release version of the NuGet package. There are no differences between this version and the previous pre-release version of the package,
other than the addition of this Readme file.
