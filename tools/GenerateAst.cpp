#include <iostream>
#include <fstream>
#include <filesystem>
#include <string_view>
#include <vector>
#include <string>

namespace fs = std::filesystem;

// Forward declaration
void defineType(
    std::ofstream& writer, std::string_view baseName,
    std::string_view className, std::string_view fieldList, bool isImplementation
);

constexpr std::string_view trim(std::string_view str) {
    constexpr std::string_view WHITESPACE = " \n\r\t\f\v";
    size_t start = str.find_first_not_of(WHITESPACE);
    if (start == std::string_view::npos) return "";
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
        exit(65);
    }

    writer << "#pragma once\n\n";
    writer << "#include <vector>\n";
    writer << "#include <memory>\n";
    writer << "#include <variant>\n";
    writer << "#include \"Token.h\"\n\n"; // <--- This brings in your global LiteralType

    writer << "namespace lox {\n\n";

    // 1. Forward Declarations
    writer << "struct " << baseName << ";\n";
    
    // --- CHANGE: REMOVED LiteralType REDEFINITION ---
    // We assume Token.h provides it. 
    // If Token.h is global, this will use ::LiteralType automatically.
    
    std::vector<std::pair<std::string, std::string>> classInfo;
    for (const std::string& type : types)  {
        size_t colonPos = type.find(':');
        if (colonPos == std::string::npos) continue;
        
        std::string className = std::string(trim(type.substr(0, colonPos)));
        std::string fields = std::string(trim(type.substr(colonPos + 1)));
        
        defineType(writer, baseName, className, fields, false);
        classInfo.push_back({className, fields});
    }

    // 2. Define the full Expr variant wrapper
    writer << "struct " << baseName << " {\n";
    writer << "    using Variant = std::variant<";
    for (size_t i = 0; i < classInfo.size(); ++i) {
        writer << classInfo[i].first << (i < classInfo.size() - 1 ? ", " : "");
    }
    writer << ">;\n\n";
    writer << "    Variant value;\n\n";
    writer << "    template <typename T> " << baseName << "(T&& val) : value(std::forward<T>(val)) {}\n";
    writer << "};\n\n";

    // 3. Write Definitions
    for (const auto& info : classInfo) {
        defineType(writer, baseName, info.first, info.second, true);
    }

    writer << "} // namespace lox\n";
}

void defineType(
    std::ofstream& writer, std::string_view baseName,
    std::string_view className, std::string_view fieldList, bool isImplementation
) { 
    // --- 1. Parse Fields ---
    std::vector<std::string> fields;
    size_t start = 0;
    while (start < fieldList.length()) {
        size_t comma = fieldList.find(',', start);
        if (comma == std::string_view::npos) {
            fields.emplace_back(trim(fieldList.substr(start)));
            break;
        }
        fields.emplace_back(trim(fieldList.substr(start, comma - start)));
        start = comma + 1;
    }

    if (!isImplementation) {
        // --- Write the Struct Header (Declaration) ---
        writer << "struct " << className << " {\n";

        for (const auto& field : fields) {
            writer << "    " << field << ";\n";
        }
        writer << "\n";
        
        // Constructor Declaration
        writer << "    " << className << "(" << fieldList << ");\n";
        
        // Destructor Declaration (Requires Expr to be complete later)
        writer << "    ~" << className << "();\n"; 

        // !!! THE FIX: Explicitly default the Move Constructor !!!
        // This resurrects the ability to move these objects into the variant.
        writer << "    " << className << "(" << className << "&&) = default;\n";

        writer << "};\n\n";

    } else {
        // --- Write the Code (Definition) ---
        
        // Constructor Definition
        writer << "inline " << className << "::" << className << "(" << fieldList << ")\n";
        
        // Member Initializer List
        if (!fields.empty()) {
            writer << "    : ";
            for (size_t i = 0; i < fields.size(); i++) {
                std::string_view field = fields[i];
                size_t spacePos = field.find_last_of(' ');
                std::string_view name = field.substr(spacePos + 1);

                writer << name << "(std::move(" << name << "))";
                if (i < fields.size() - 1) writer << ", ";
            }
            writer << " {}\n\n";
        } else {
            writer << "    {}\n\n";
        }

        // Destructor Definition
        writer << "inline " << className << "::~" << className << "() = default;\n\n";
    }
}

int main (int argc, char* argv[]){
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <output_directory>\n";
        return 64;
    }
    
    std::string outputDir = argv[1];

    defineAst(outputDir, "Expr", {
        "Binary   : std::unique_ptr<Expr> left, Token loxperator, std::unique_ptr<Expr> right",
        "Grouping : std::unique_ptr<Expr> expression",
        "Literal  : LiteralType value",
        "Unary    : Token loxperator, std::unique_ptr<Expr> right",
        "Ternary  : std::unique_ptr<Expr> left, Token op1, std::unique_ptr<Expr> middle, Token op2, std::unique_ptr<Expr> right"
    });
    
    return 0;
}