import qbs.File
import qbs.FileInfo
import qbs.ModUtils
import qbs.PathTools
import qbs.Probes
import qbs.Utilities
import "tcc.js" as Tcc

Module {
    condition: qbs.toolchain && qbs.toolchain.contains("tcc")

    Probes.BinaryProbe {
        id: compilerPathProbe
        condition: !toolchainInstallPath
        names: ["tcc"]
    }

    version: compilerVersion

    property string compilerVersion: [compilerVersionMajor, compilerVersionMinor, compilerVersionPatch].join(".")

    // Версия компилятора ADS1.2 [Build 848].
    property int compilerVersionMajor: 1
    property int compilerVersionMinor: 2
    property int compilerVersionPatch: 848

    property pathList includePaths
    property pathList systemIncludePaths
    property pathList libraryPaths

    property stringList staticLibraries
    property stringList linkerFlags

    property string endianness: "big"
    property string toolchainInstallPath: compilerPathProbe.found ? compilerPathProbe.path : undefined

    property string architecture: qbs.architecture

    // Work-around for QtCreator which expects these properties to exist.
    property string cCompilerName: compilerName
    property string cxxCompilerName: compilerName

    property string compilerExtension: FileInfo.executableSuffix()
    property string compilerName: "tcc" + compilerExtension
    property string compilerPath: FileInfo.joinPaths(toolchainInstallPath, compilerName)

    property string linkerName: "armlink" + compilerExtension
    property string linkerPath: FileInfo.joinPaths(toolchainInstallPath, linkerName)

    property string executablePrefix: ""
    property string staticLibrarySuffix: ".o"
    property string executableSuffix: ".elf"
    property string objectSuffix: ".o"

    property string defineFlag: "-D"
    property string includeFlag: "-I"

    property stringList defines
	property string optimization

    validate: {
        var validator = new ModUtils.PropertyValidator("cxx");
        validator.setRequiredProperty("architecture", architecture,
                                      "you might want to re-run 'qbs-setup-toolchains'");
        validator.addCustomValidator("architecture", architecture, function (value) {
            return !architecture || architecture === Utilities.canonicalArchitecture(architecture);
        }, "'" + architecture + "' is invalid. You must use the canonical name '" +
        Utilities.canonicalArchitecture(architecture) + "'");
        validator.setRequiredProperty("endianness", endianness);
        validator.setRequiredProperty("compilerVersion", compilerVersion);
        validator.setRequiredProperty("compilerVersionMajor", compilerVersionMajor);
        validator.setRequiredProperty("compilerVersionMinor", compilerVersionMinor);
        validator.setRequiredProperty("compilerVersionPatch", compilerVersionPatch);
        validator.addVersionValidator("compilerVersion", compilerVersion, 3, 3);
        validator.addRangeValidator("compilerVersionMajor", compilerVersionMajor, 1);
        validator.addRangeValidator("compilerVersionMinor", compilerVersionMinor, 0);
        validator.addRangeValidator("compilerVersionPatch", compilerVersionPatch, 0);
        validator.validate();
    }

    FileTagger {
        patterns: ["*.c"]
        fileTags: ["c"]
    }

    FileTagger {
        patterns: ["*.cpp"]
        fileTags: ["cpp"]
    }

    Rule {
        id: compiler
        inputs: ["cpp", "c"]
        outputFileTags: ["obj"]
        outputArtifacts: [{
                fileTags: ["obj"],
                filePath: FileInfo.joinPaths(Utilities.getHash(input.baseDir),
                                             input.fileName + input.cpp.objectSuffix)
            }];
        prepare: Tcc.prepareCompiler.apply(Tcc, arguments)
    }

    Rule {
        id: linker
        multiplex: true
        inputs: ["obj"]
        outputFileTags: ["application"]
        outputArtifacts: [{
                fileTags: ["application"],
                filePath: FileInfo.joinPaths(product.destinationDirectory,
                                             PathTools.applicationFilePath(product))
            }];
        prepare: Tcc.prepareLinker.apply(Tcc, arguments)
    }
}
