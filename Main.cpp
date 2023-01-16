#include <iostream>
#include <fstream>
#include <Windows.h>

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: LargeAddressifier <file>" << std::endl;
        return EXIT_SUCCESS;
    }

    std::string inFilename = argv[1];
    std::cout << "Opening file \"" << inFilename << "\"..." << std::endl;

    std::ifstream inFile(inFilename, std::ios::binary);
    if (!inFile.is_open())
    {
        std::cout << "ERROR: Could not open file \"" << inFilename << "\"" << std::endl;
        return EXIT_FAILURE;
    }

    IMAGE_DOS_HEADER dosHeader = { 0 };
    inFile.read(reinterpret_cast<char*>(&dosHeader), sizeof(dosHeader));

    if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE)
    {
        std::cout << "ERROR: Invalid file: Invalid DOS header magic." << std::endl;
        return EXIT_FAILURE;
    }

    inFile.seekg(dosHeader.e_lfanew);

    IMAGE_NT_HEADERS ntHeaders = { 0 };
    inFile.read(reinterpret_cast<char*>(&ntHeaders), sizeof(ntHeaders));

    if (ntHeaders.Signature != IMAGE_NT_SIGNATURE)
    {
        std::cout << "ERROR: Invalid file: Invalid NT header magic." << std::endl;
        return EXIT_FAILURE;
    }

    IMAGE_FILE_HEADER& coffHeader = ntHeaders.FileHeader;

    if ((coffHeader.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE) == 0)
    {
        std::cout << "ERROR: Invalid file: The file must be executable." << std::endl;
        return EXIT_FAILURE;
    }

    if ((coffHeader.Characteristics & IMAGE_FILE_DLL) == IMAGE_FILE_DLL)
    {
        std::cout << "ERROR: Invalid file: The executable cannot be a DLL." << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "The file is a valid executable application." << std::endl;

    bool isLargeAddressAware = (coffHeader.Characteristics & IMAGE_FILE_LARGE_ADDRESS_AWARE) == IMAGE_FILE_LARGE_ADDRESS_AWARE;
    std::cout << "The application is " << (isLargeAddressAware ? "already" : "not") << " aware of large addresses." << std::endl;
    if (isLargeAddressAware)
        return EXIT_SUCCESS;

    coffHeader.Characteristics |= IMAGE_FILE_LARGE_ADDRESS_AWARE;

    std::string outFilename = inFilename;
    size_t extensionPos = outFilename.find_last_of('.');
    if (extensionPos != std::string::npos)
        outFilename.insert(extensionPos, ".new");
    else
        outFilename += ".new";

    std::cout << "Writing file \"" << outFilename << "\"..." << std::endl;

    std::ofstream outFile(outFilename, std::ios::binary);
    if (!outFile.is_open())
    {
        std::cout << "ERROR: Cannot write to file \"" << outFilename << "\"" << std::endl;
        return EXIT_FAILURE;
    }

    inFile.seekg(0);

    outFile << inFile.rdbuf();
    outFile.seekp(dosHeader.e_lfanew);
    outFile.write(reinterpret_cast<char*>(&ntHeaders), sizeof(ntHeaders));
}
