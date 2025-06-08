/// @file CALCM.h
/// @brief Header file used for calculator module used for handling data operations
/// @author Aleksandra Petrovic

#ifndef CALCM_H_
#define CALCM_H_

#include "math.h"
#include "MSGM.h"

/// Function that converts degrees to radians
#define RADIANS_CONVERTOR(x) (x * 0.0174532925)
/// Function that converts radians to degrees
#define DEGREES_CONVERTOR(x) (x * 57.2957795)

/// Defines value of full circle in degrees
#define FULL_CIRCLE 360
/// Lowest value longitude can have
#define LONGITUDE_LOW_RANGE -180
/// Greatest value longitude can have
#define LONGITUDE_HIGH_RANGE 180
/// Lowest value latitude can have
#define LATITUDE_LOW_RANGE -90
/// Greatest value latitude can have
#define LATITUDE_HIGH_RANGE 90
/// Derivation constant used to correct format of data read from GPS module
#define DERIVATION_CONST 100

/// Enum used to define sides of the world
typedef enum
{
  CALCM_NORTH_SIDE = 0u,							///< North side
  CALCM_WEST_SIDE  = 1u,							///< West side
  CALCM_EAST_SIDE  = 2u,							///< East side
  CALCM_SOUTH_SIDE = 3u								///< South side
} t_CALCM_WorldDirection;

/// Structure used for coordinates in format that is needed for further calculations
typedef struct
{
  double d_Latitude;								///< Value of latitude coordinate
  double d_Longitude;								///< Value of longitude coordinate
  t_CALCM_WorldDirection t_LatitudeDirection;		///< Direction for a latitude coordinate
  t_CALCM_WorldDirection t_LongitudeDirection;		///< Direction for a longitude coordinate
} t_CALCM_Cooridnates;

/// @brief Function used to calculate bearing based on parsed coordinates
///
/// @pre None
/// @post Correct LED will be turned on
/// @param None
/// @return uint16_t u_Bearing
///
/// @globals None
///
/// @InOutCorelation Function calculates bearing based on coordinates.
/// @callsequence
///   @startuml "CALCM_u_CalculateBearing.png"
///     title "Sequence diagram for function CALCM_u_CalculateBearing"
///     -> CALCM: CALCM_u_CalculateBearing()
///     CALCM++
///       CALCM -> CALCM: CALCM_p_SetCoordinates()
///       opt if latitude is South side
///         rnote over CALCM: Multiply latitude by -1 (South side of latitude has negative value)
///       end
///       opt if longitude is West side
///         rnote over CALCM: Multiply longitude by -1 (West side of longitude has negative value)
///       end
///       math.h -> CALCM: Uses sin(), cos() and tan2() functions from math.h library
///       CALCM -> CALCM: f_CalculateDistance(f_CarLongitude, f_StartingLatitude)
///       rnote over CALCM: Read values from MSGM must be converted to radians for further calculations.
///       CALCM -> CALCM: RADIANS_CONVERTOR()
///       rnote over CALCM: After the calculations are done, value of bearing must be converted to degrees.
///       CALCM -> CALCM: DEGREES_CONVERTOR(f_Bearing_dbl)
///       opt if check if the bearing is less than a full circle
///         rnote over CALCM: Perform modulo derivation so bearing doesn't exceed the maximum value.
///       end
///     <- CALCM:// Returns a uint16_t value of direction.//
///     CALCM--
///   @enduml

uint16_t CALCM_u_CalculateBearing(void);

#endif /* CALCM_H_ */
