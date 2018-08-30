import qbs

CppApplication {
    name: "bmi"
    files: ["bmi.cpp"]
    cpp.cxxLanguageVersion: "c++17"
    cpp.cxxStandardLibrary: "libc++"
}

