var File = require("qbs.File");
var FileInfo = require("qbs.FileInfo");

function compilerFlags(project, product, input, outputs, explicitlyDependsOn) {
    var args = ["-c"];

    // Input.
    args.push(input.filePath);

    // Output.
    args.push("-o", outputs.obj[0].filePath);

    // Defines.
    var defines = ["__P2K__"].uniqueConcat(input.cpp.defines || []);
    args = args.concat(defines.map(function(define) { return input.cpp.defineFlag + define; }));

    // Includes.
    var includes = (input.cpp.includePaths || []).uniqueConcat(input.cpp.systemIncludePaths || []).uniqueConcat(input.cpp.distributionIncludePaths || []);
    args = args.concat(includes.map(function(include) { return input.cpp.includeFlag + include; }));

    // Optimization level flags.
    switch (input.cpp.optimization) {
    case "None":
        break;
    case "O0":
        args.push("-O0");
        break;
    case "O1":
        args.push("-O1");
        break;
    case "O2":
        args.push("-O2");
        break;
    default:
        break;
    }

    // Endianness.
    var endianness = input.cpp.endianness;
    if (endianness === "big")
        args.push("-bigend");

    args.push("-apcs", "/interwork");
    return args;
}

function linkerFlags(project, product, inputs, outputs) {
    var args = [];

    // Flags.
    args = args.concat(product.cpp.linkerFlags);

    // Input objects.
    args = args.concat((inputs.obj || []).map(function(obj) { return obj.filePath; }));

    // Input library relocation.
    var firstStaticLib = product.cpp.staticLibraries[0];
    args.push("-first", firstStaticLib + product.cpp.staticLibrarySuffix + "(Lib)");

    // Input libraries.
    var libraryPaths = product.cpp.libraryPaths;
    var libs = (product.cpp.staticLibraries || []).map(function(library) {
        library += product.cpp.staticLibrarySuffix;
        for (var i in libraryPaths) {
            var file = FileInfo.joinPaths(libraryPaths[i], library);
            if (File.exists(file))
                return file;
        }
        return library;
    });
    args = args.concat(libs);

    args.push("-o", outputs.application[0].filePath);
    return args;
}

function prepareCompiler(project, product, inputs, outputs, input, output, explicitlyDependsOn) {
    var cmds = [];
    var args = compilerFlags(project, product, input, outputs, explicitlyDependsOn);
    var compilerPath = product.cpp.compilerPath;
    var cmd = new Command(compilerPath, args);
    cmd.description = "compiling " + input.fileName;
    cmd.highlight = "compiler";
    cmd.jobPool = "compiler";
    cmds.push(cmd);
    return cmds;
}

function prepareLinker(project, product, inputs, outputs, input, output) {
    var cmds = [];
    var args = linkerFlags(project, product, inputs, outputs);
    var linkerPath = product.cpp.linkerPath;
    var cmd = new Command(linkerPath, args);
    cmd.description = "linking " + outputs.application[0].fileName;
    cmd.highlight = "linker";
    cmd.jobPool = "linker";
    cmds.push(cmd);
    return cmds;
}
