#include "GameLevelLoader.hpp"
#include "GameLevels.hpp"


inline bool carOccupiesCell(const CarsStates& car, Grid2D cell)
{
    // First cell the car occupies
    if (car.grid2d == cell)
        return true;

    // Second cell depends on orientation
    if (PosVehicules::OrientationRULESpreset.at(car.orientation) == OrientationRULES::LEFT_RIGHT)
    {
        // Car extends to the right: also occupies (x+1, y)
        Grid2D secondCell = {.x = static_cast<uint8_t>(car.grid2d.x + 1), .y = car.grid2d.y};
        return secondCell == cell;
    }
    else
    {
        // Car extends downward: also occupies (x, y+1)
        Grid2D secondCell = {.x = car.grid2d.x, .y = static_cast<uint8_t>(car.grid2d.y + 1)};
        return secondCell == cell;
    }
}

void GameLevelLoader::LoadLevel(int level)
{
    lev_data = Levels.at(level);

    std::for_each(lev_data.begin(), lev_data.end(), [](CarsStates& n){
        if (n.true_car == 1)
        {
            n.basepose = PosVehicules::BasePoses.at(n.orientation);
            PosVehicules::LoadVehicule_Mesh(n.ptrMesh, n.carID, n.orientation);
            PosVehicules::LoadVehicule_Texture(&n.texGLptr, n.tex);
        }

    });
}

bool GameLevelLoader::CollisionCheck(Grid2D grid2d, size_t excludeIndex)
{
    

    const CarsStates& movingCar = lev_data.at(excludeIndex);

    // Get both cells the moving car will occupy at the new position
    Grid2D cell1 = grid2d;
    Grid2D cell2;
    if (PosVehicules::OrientationRULESpreset.at(movingCar.orientation) == OrientationRULES::LEFT_RIGHT)
    {
        cell2 = {.x = static_cast<uint8_t>(grid2d.x + 1), .y = grid2d.y};
    }
    else
    {
        cell2 = {.x = grid2d.x, .y = static_cast<uint8_t>(grid2d.y + 1)};
    }

    size_t i = 0;
    auto it = std::find_if(lev_data.begin(), lev_data.end(), [&](const CarsStates& car){
        size_t currentIndex = i++;
        if (currentIndex == excludeIndex || car.true_car == 0)
            return false;

        // Check if either cell of the moving car collides with any cell of this car
        return carOccupiesCell(car, cell1) || carOccupiesCell(car, cell2);
    });

    return it != lev_data.end();
}