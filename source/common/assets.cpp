#include "common/runtime.h"
#include "common/assets.h"

#include <unistd.h>
#include <dirent.h>

void Assets::Initialize(const std::string & path)
{
    Location location = Assets::GetLocation(path);
    
    switch(location)
    {
        case Location::SDMC:
            directory = "game";
            break;
        case Location::EXTERNAL:
            romfsMountFromFsdev(path.c_str(), 0, "romfs");
            break;
        default:
        case Location::ROMFS:
            break;
    }

    chdir(directory.c_str());
}

Location Assets::GetLocation(const std::string & path)
{
    if (path.empty())
        return Location::SDMC;

    size_t extPosition = path.find_last_of(".");
    bool isROMFSGame = false;

    if (extPosition != std::string::npos)
        isROMFSGame = (path.substr(extPosition) == ".lpx");

    if (!isROMFSGame)
    {
        struct stat pathInfo;

        stat("game", &pathInfo);

        if (S_ISDIR(pathInfo.st_mode))
            return Location::SDMC;
    }

    // couldn't grab game folder
    Result rc = romfsInit();

    // load our external ROMFS game
    if (rc == 0 && isROMFSGame)
        return Location::EXTERNAL;

    // load no game / fused
    return Location::ROMFS;
}
