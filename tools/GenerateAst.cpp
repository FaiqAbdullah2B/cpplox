#include <iostream>
#include <fstream>
#include <filesystem>
#include <string_view>
#include <vector>
#include <string>

namespace fs = std::filesystem;
void defineType(
    std::ofstream& writer, std::string_view baseName,
    std::string_view className, std::string_view fieldList
);

constexpr std::string_view trim(std::string_view str) {
    constexpr std::string_view WHITESPACE = " \n\r\t\f\v";
    
    // Find the first character that ISN'T whitespace
    size_t start = str.find_first_not_of(WHITESPACE);
    if (start == std::string_view::npos) {
        return ""; // The whole string is just spaces
    }

    size_t end = str.find_last_not_of(WHITESPACE);
    return str.substr(start, end - start + 1);
}

void defineAst(
    std::string_view outputDir, std::string_view baseName, const std::vector<std::string>& types
) {
    std::string fileName = std::string(baseName) + ".h";
    fs::path outPath = fs::path(outputDir) / fileName;

    std::ofstream writer(outPath);
    if (!writer.is_open()) {
        std::cerr << "Could not open output file " << outPath << "\n";
        exit(65); // bad input data
    }

    writer << "#pragma once\n";
    writer << "\n";
    writer << "#include <vector>\n";
    writer << "#include <memory>\n";
    writer << "\n" << "#include \"Token.h\"\n"; 
    writer << "\n";


    writer << "namespace lox {\n"; 
    writer << "\n";
    // The Base Abstract Class
    writer << "struct " << baseName << " {\n";
    writer << "    virtual ~" << baseName << "() = default;\n"; 
    
    for (std::string_view type : types)  {
        size_t colonPos = type.find(':');

        if (colonPos == std::string_view::npos) {
            continue; 
        }
        
        std::string_view className = trim(type.substr(0, colonPos));
        std::string_view fields = trim(type.substr(colonPos + 1));

        defineType(writer, baseName, className, fields);
    }
    
    // Closing the class
    writer << "};\n";
    writer << "\n";
    writer << "} // namespace lox\n";
}

void defineType(
    std::ofstream& writer, std::string_view baseName,
    std::string_view className, std::string_view fieldList
) {

}

int main (int argc, char* argv[]){
    if (argc!=2) {
        std::cout << "Usage: " << argv[0] << " <output_directory>\n";
        return 64; // EX_USAGE
    }
    
    std::string outputDir = argv[1];

    defineAst(outputDir, "Expr", {
        "Binary   : std::unique_ptr<Expr> left, Token operator, std::unique_ptr<Expr> right",
        "Grouping : std::unique_ptr<Expr> expression",
        "Literal  : LiteralType value",
        "Unary    : Token operator, std::unique_ptr<Expr> right"
    });
}