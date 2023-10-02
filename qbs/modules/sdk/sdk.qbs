import qbs.File
import qbs.FileInfo
import qbs.Probes

Module {
    Depends { name: "cpp" }

    Probes.PathProbe {
        id: sdkProbe
        // Inputs.
        candidatePaths: [toolchainInstallPath]
        // Outputs.
        property string includesPath;
        property string libsPath;
        configure: {
            for (var i in candidatePaths) {
                var rootPath = candidatePaths[i];
                if (!File.exists(rootPath))
                    continue;
                includesPath = FileInfo.joinPaths(rootPath, "SDK");
                libsPath = FileInfo.joinPaths(rootPath, "lib");
                found = true;
                return;
            }
        }
    }

    validate: {
        if (!sdkProbe.found)
            throw "SDK sub-folder not found in the toolchain root directory"
    }

    property string toolchainInstallPath: cpp.toolchainInstallPath

    cpp.systemIncludePaths: [sdkProbe.includesPath, "C:\\ARM\\SDK", "/opt/arm/SDK"]
    cpp.libraryPaths: [sdkProbe.libsPath, "C:\\ARM\\lib", "/opt/arm/lib"]
    cpp.linkerFlags: ["-nolocals", "-reloc"]
}
