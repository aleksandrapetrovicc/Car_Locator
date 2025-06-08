/// @file CALCM.c
/// @brief Main file used for calculator module used for handling data operations
/// @author Aleksandra Petrovic

#include "CALCM.h"
#include "SIM.h"

// Used as buffer where calculated coordinates will be written into
static volatile t_CALCM_Cooridnates t_Cooridnates = {0u};

/// @brief Function used to convert longitude and latitude from characters to numbers
///
/// @pre MSGM state machine reads and processes data read from GPS module
/// @post Bearing calculation
/// @param uint8_t *aTempBuffer, double f_LowLimit, double f_HighLimit
/// @return double f_Num
///
/// @globals None
///
/// @InOutCorelation Function converts characters to double number values.
/// @callsequence
///   @startuml "v_ConvertToNumbers.png"
///     title "Sequence diagram for function v_ConvertToNumbers"
///     -> CALCM: v_ConvertToNumbers(uint8_t *u_TempBuffer, double f_LowLimit, double f_HighLimit)
///     CALCM++
///       loop Used to skip zero values.
///         rnote over CALCM: Increment index until non zero value is reached.
///       end
///       loop Searches for dot character because of the message format.
///         rnote over CALCM: Length counter increments for each passing trough the loop to determine the length of whole number.
///       end
///       loop Goes through the temporary buffer and checks if read elements are number characters using ASCII table.
///         opt if elements are inside the range from '0' to '9'
///           rnote over CALCM: Correct elements are converted into number values.
///           loop Until u_Length reaches 1 decrease its value
///           rnote over CALCM: Multiply each element with correct multiplication of 10 and add it to value that will be returned.
///           end
///         else else
///           rnote over CALCM: Incorrect elements are deleted by writing 0 values to their places.
///       end
///       rnote over CALCM: Dot characters is written into temporary buffer and index is incremented to skip to fractioned part.
///       loop Goes through the remaining elements to read fraction part of the coordinates.
///         opt If read values are corresponding with ASCII values for digits, convert them to numbers.
///           rnote over CALCM: Derive each element with correct multiplication of 10 and add it to value that will be returned.
///         else else If incorrect data occurred, all written data gets deleted.
///         end
///       end
///       opt if number is not inside the correct range
///         loop Go through elements of temporary buffer
///           rnote over CALCM: Write 0s into each buffer element.
///          end
///       end
///     <- CALCM:// Returns a double value of coordinates.//
///     CALCM--
///   @enduml

static double v_ConvertToNumbers(uint8_t *u_TempBuffer, double f_LowLimit, double f_HighLimit);

static double v_ConvertToNumbers(uint8_t *u_TempBuffer, double f_LowLimit, double f_HighLimit)
{
  double f_Num = 0;
  // Used for going through u_TempBuffer array
  uint8_t u_Index = 0u;
  // Used to store whole number part
  uint8_t u_Buffer[COORDINATES_LENGTH];
  // Variable used to determine length of whole number part
  uint8_t u_Length = 0u;
  uint8_t u_TmpCntr = 0u;

  // Until non zero value is reached or index reaches the last element, keep going through the array
  while((u_TempBuffer[u_Index] == 0) && (u_Index != COORDINATES_LENGTH))
  {
    u_Index++;
  }
  // Index of first non zero character
  uint8_t u_Start = u_Index;

  // . character represents the end of whole digit part of the coordinate
  while(u_TempBuffer[u_Index] != '.' && (u_Index != COORDINATES_LENGTH))
  {
	u_Buffer[u_TmpCntr] = u_TempBuffer[u_Index];
	u_TmpCntr++;
	u_Index++;
	u_Length++;
  }
  // Write a . character into a buffer
  u_Buffer[u_TmpCntr] = u_TempBuffer[u_Index];
  // Increment u_TmpCntr so u_FractionStart gets its right value
  u_TmpCntr++;
  // Index of last whole number digit
  uint8_t u_FractionStart = u_TmpCntr;
  // Used to store length value so when it decreases it can be read again
  uint8_t u_LenTmp = u_Length;
  // Number of elements for fraction part of coordinates
  uint8_t u_NumofRemainigElements = COORDINATES_LENGTH - u_Length - 1;
  uint8_t u_Cnt = 0u;
  // Start from first non zero character and go through all the elements before '.' character
  while(u_Start < u_Index && u_Cnt < u_TmpCntr - 1)
  {
	double u_Tmp = (double)u_Buffer[u_Cnt];
	// ASCII range for 0-9 digits
	if(u_Tmp >= '0' && u_Tmp <= '9')
	{
      // ASCII value for 0, used to convert character to number
	  u_Tmp -= '0';
	  while(u_Length != 1)
	  {
	    // u_Length is used to realize 10^u_Length
	    u_Tmp *= 10;
		u_Length--;
	  }
	  // Derivation with 100 needs to be done because of the format of the number that is read from GPS module
	  u_Tmp /= DERIVATION_CONST;
	  // Add u_Tmp number to f_Num which will be returned
	  f_Num += u_Tmp;
	}
	else
	{
      // In case of incorrect data format clear all the elements
	  u_Length = 0u;
	  for(uint8_t u_Count = 0; u_Count < u_FractionStart; u_Count++)
	  {
		u_Buffer[u_Count] = 0;
	  }
	}
	u_Start++;
	// u_LenTmp needs to be decremented so next passing through loop will provide lesser length for 10^u_Length
	u_LenTmp--;
	u_Length = u_LenTmp;
	u_Cnt++;
  }

  u_Cnt = 0u;
  // When array reaches '.' character, skip to fraction part
  u_Index++;
  // Values need to be derived by position compared to . character multiplied by derivation format constant
  double u_Deriv = 10 * DERIVATION_CONST;
  // Loop goes through remaining elements in longitude/latitude buffer
  while((u_Cnt < u_NumofRemainigElements) && (u_TempBuffer[u_Index] != 0))
  {
	double u_Tmp = (double)u_TempBuffer[u_Index];
	if(u_Tmp >= '0' && u_Tmp <= '9')
	{
	  u_Tmp -= '0';
	  u_Tmp /= u_Deriv;
	  // Add each digit of fractioned part to the number
	  f_Num += u_Tmp;
	}
	// Multiply derivation variable so each next element gets derived 10 times more
	u_Deriv *= 10;
	u_Index++;
	u_Cnt++;
  }
  // Check if calculated number is inside the range for longitude/latitude
  if(!(f_Num >= f_LowLimit && f_Num <= f_HighLimit))
  {
	// Clear everything if non number values appear
	f_Num = 0;
	for(uint8_t u_Count = 0u; u_Count < u_Index; u_Count++)
	{
	  u_Buffer[u_Count] = 0u;
	}
  }
  return f_Num;
}

/// @brief Function used for getting coordinates from a buffer
///
/// @pre Getter must have the coordinates ready
/// @post Coordinates are converted to number values and properly stored into correct elements of the structure
/// @param None
///
/// @return t_CALCM_Cooridnates * t_Cooridnates
///
/// @globals static volatile t_CALCM_Cooridnates t_Cooridnates
///
/// @InOutCorelation Function reads coordinates from SIM800L getter and then converts them in correct format.
/// @callsequence
///   @startuml "p_SetCoordinates.png"
///     title "Sequence diagram for function p_SetCoordinates"
///     -> CALCM: p_SetCoordinates()
///     CALCM++
///       SIM -> CALCM: SIM_v_ReceiveCoordinates()
///       loop Goes through read elements until latitude direction is reached.
///         rnote over CALCM: Write elements in temporary buffer.
///       end
///       CALCM -> CALCM: v_ConvertToNumbers(u_TmpLat, LATITUDE_LOW_RANGE, LATITUDE_HIGH_RANGE)
///       opt if latitude is North
///         rnote over CALCM: Set latitude to North
///       else else
///         rnote over CALCM: Set latitude to South
///       end
///       loop Goes through read elements until longitude direction is reached.
///         rnote over CALCM: Write elements in temporary buffer.
///       end
///       CALCM -> CALCM: v_ConvertToNumbers(u_TmpLon, LONGITUDE_LOW_RANGE, LONGITUDE_HIGH_RANGE)
///       opt if longitude is West
///         rnote over CALCM: Set longitude to West
///       else else
///         rnote over CALCM: Set longitude to East
///       end
///     <- CALCM:// Returns t_CALCM_Cooridnates * value of a structure with coordinates converted into usable values.//
///     CALCM--
///   @enduml

static volatile t_CALCM_Cooridnates * p_SetCoordinates();

static volatile t_CALCM_Cooridnates * p_SetCoordinates()
{
  uint8_t * u_Coordinates = SIM_p_ReceiveCoordinates();
  uint8_t u_Cnt = 0;
  uint8_t u_Count = 0;
  uint8_t u_TmpLon[COORDINATES_LENGTH] = {0};
  uint8_t u_TmpLat[COORDINATES_LENGTH] = {0};

  // Go through elements that represent the latitude part of the coordinates
  while(u_Coordinates[u_Cnt] != ',' && u_Cnt < COORDINATES_LENGTH)
  {
	u_TmpLat[u_Cnt] = u_Coordinates[u_Cnt];
	u_Cnt++;
  }
  u_Cnt++;

  // Convert the latitude part into double numbers for further calculations
  t_Cooridnates.d_Latitude = v_ConvertToNumbers(u_TmpLat, LATITUDE_LOW_RANGE, LATITUDE_HIGH_RANGE);
  // Check if latitude direction is North or South
  if(u_Coordinates[u_Cnt] == 'N')
  {
    t_Cooridnates.t_LatitudeDirection = CALCM_NORTH_SIDE;
  }
  else if(u_Coordinates[u_Cnt] == 'S')
  {
  	t_Cooridnates.t_LatitudeDirection = CALCM_SOUTH_SIDE;
  }

  // Increment u_Cnt to get to longitude part
  u_Cnt+= 2;

  // Go through elements that represent the longitude part of the coordinates
  while(u_Coordinates[u_Cnt] != ',' && u_Cnt < COORDINATES_LENGTH)
  {
  	u_TmpLon[u_Count] = u_Coordinates[u_Cnt];
  	u_Cnt++;
  	u_Count++;
  }

  u_Cnt++;

  // Convert longitude part into double numbers for further calculations
  t_Cooridnates.d_Longitude = v_ConvertToNumbers(u_TmpLon, LONGITUDE_LOW_RANGE, LONGITUDE_HIGH_RANGE);
  // Check if longitude direction is East or West
  if(u_Coordinates[u_Cnt] == 'E')
  {
    t_Cooridnates.t_LongitudeDirection = CALCM_EAST_SIDE;
  }
  else if(u_Coordinates[u_Cnt] == 'W')
  {
    t_Cooridnates.t_LongitudeDirection = CALCM_WEST_SIDE;
  }
  return &t_Cooridnates;
}

/// @brief Function used for calculating the absolute value of the distance between two points.
///
/// @pre Values for each coordinate must be known
/// @post Further calculation based on the absolute distance can be performed
/// @param double f_CarLongitude, double f_StartingLongitude
///
/// @return double f_DeltaL
///
/// @globals None
///
/// @InOutCorelation Function calculates the absolute value of the distance between two points.
/// @callsequence
///   @startuml "f_CalculateDistance.png"
///     title "Sequence diagram for function f_CalculateDistance"
///     -> CALCM: f_CalculateDistance(double f_CarLongitude, double f_StartingLongitude)
///     CALCM++
///       opt Check if car longitude is positive.
///         opt Check if our current longitude is positive.
///           opt If car longitude is greater than current longitude.
///             rnote over CALCM: Subtract current longitude from car longitude.
///           else else If current longitude is greater than car longitude.
///             rnote over CALCM: Subtract car longitude from current longitude.
///           end
///         else else If current longitude is negative.
///           rnote over CALCM: Add car longitude to current longitude multiplied by negative value of 1.
///         end
///       else else If car longitude is negative.
///         rnote over CALCM: Car longitude is multiplied by negative value of 1 and saved into temporary variable.
///         opt If current longitude is positive.
///           rnote over CALCM: Add current longitude to value in temporary variable.
///         else else If current longitude is negative.
///           rnote over CALCM: Current longitude is multiplied by negative value of 1. and saved into temporary variable.
///           opt If car longitude multiplied by negative value of 1 is greater than current longitude multiplied by negative value of 1.
///             rnote over CALCM: Subtract current longitude multiplied by negative value of 1 form car longitude multiplied by negative value of 1.
///           else else If current longitude multiplied by negative value of 1 is greater than car longitude multiplied by negative value of 1.
///             rnote over CALCM: Subtract car longitude multiplied by negative value of 1 from current longitude multiplied by negative value of 1.
///           end
///         end
///       end
///     <- CALCM: //Returns double value of absolute value of distance.
///     CALCM--
///   @enduml

static double f_CalculateDistance(double f_CarLongitude, double f_StartingLongitude);

static double f_CalculateDistance(double f_CarLongitude, double f_StartingLongitude)
{
  double f_DeltaL;
  // Check if car longitude coordinate is positive
  if(f_CarLongitude >= 0)
  {
	// Check if current longitude coordinate is positive
    if(f_StartingLongitude >= 0)
    {
      // Check if car longitude coordinate is greater than current longitude coordinate
      if(f_CarLongitude > f_StartingLongitude)
      {
    	// Calculate absolute distance between coordinates when both values are positive
        f_DeltaL = f_CarLongitude - f_StartingLongitude;
       }
      else
      {
        // Calculate absolute distance between coordinates when both values are positive
        f_DeltaL = f_StartingLongitude - f_CarLongitude;
      }
    }
    else
    {
      // Calculate absolute distance between coordinates when car longitude is positive and current longitude is negative
      f_DeltaL = f_CarLongitude + f_StartingLongitude * (-1);
    }
  }
  else
  {
    // Temporary variable used when value of car longitude is negative
    double f_CarLongitude_tmp = f_CarLongitude * (-1);
    // Check if current longitude is positive
    if(f_StartingLongitude >= 0)
    {
      // Calculate absolute distance between coordinates when current longitude is positive and car longitude is negative
      f_DeltaL = f_CarLongitude_tmp + f_StartingLongitude;
    }
    else
    {
      // Temporary variable used when value of current longitude is negative
      double f_StartingLongitude_tmp = f_StartingLongitude * (-1);
      if(f_CarLongitude_tmp > f_StartingLongitude_tmp)
      {
        // Calculate absolute distance between coordinates when both values are negative
        f_DeltaL = f_CarLongitude_tmp - f_StartingLongitude_tmp;
      }
      else
      {
        // Calculate absolute distance between coordinates when both values are negative
        f_DeltaL = f_StartingLongitude_tmp - f_CarLongitude_tmp;
      }
    }
  }
  // Returns absolute value of distance
  return f_DeltaL;
}

uint16_t CALCM_u_CalculateBearing()
{
  // Starting latitude and latitude are 0 in our project (pointing N)
  double f_StartingLatitude = 0;

  volatile t_CALCM_Cooridnates * p_GetCoordinates = p_SetCoordinates();

  // Read car latitude gotten from GPS module
  double f_CarLatitude = p_GetCoordinates -> d_Latitude;
  // South side of latitude has negative value
  if(p_GetCoordinates -> t_LatitudeDirection == CALCM_SOUTH_SIDE)
  {
    f_CarLatitude *= (-1);
  }

  // Read car longitude gotten from GPS module
  double f_CarLongitude = p_GetCoordinates -> d_Longitude;
  // West side of longitude has negative value
  if(p_GetCoordinates -> t_LongitudeDirection == CALCM_WEST_SIDE)
  {
    f_CarLongitude *= (-1);
  }

  // Calculate the absolute value of distance
  double f_Distance = f_CalculateDistance(f_CarLongitude, f_StartingLatitude);

  // Converting coordinates to radians because math.h functions use radians
  double f_CarLatitude_rad = RADIANS_CONVERTOR(f_CarLatitude);
  double f_StartingLatitude_rad = RADIANS_CONVERTOR(f_StartingLatitude);
  double f_Distance_rad = RADIANS_CONVERTOR(f_Distance);

  double f_x = cos(f_StartingLatitude_rad) * sin(f_Distance_rad);
  double f_y = cos(f_CarLatitude_rad) * sin(f_StartingLatitude_rad) - sin(f_CarLatitude_rad) * cos(f_StartingLatitude_rad) * cos(f_Distance_rad);

  double f_Bearing = atan2(f_x, f_y);

  // Convert bearing to degrees
  f_Bearing = DEGREES_CONVERTOR(f_Bearing);

  uint16_t u_Bearing = (uint16_t)f_Bearing;
  // Check if bearing is in correct range
  if(u_Bearing > FULL_CIRCLE)
  {
    u_Bearing %= FULL_CIRCLE;
  }
  return u_Bearing;
}
