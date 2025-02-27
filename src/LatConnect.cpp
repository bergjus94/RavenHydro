/*----------------------------------------------------------------
  Raven Library Source Code
  Copyright (c) 2008-2023 the Raven Development Team
  ------------------------------------------------------------------
  Simple blowing snow redistribution algorithm
  ----------------------------------------------------------------*/

#include "HydroProcessABC.h"
#include "LatConnect.h"


///////////////////////////////////////////////////////////////////
/// \brief Constructor for CLatConnect
//
CLatConnect::CLatConnect(int hru, int connected_hru, double w)
  : source_hru_id(hru), connected_hru_id(connected_hru), weight(w)
{
  if (source_hru_id < 0) {
    std::cerr << "CLatConnect constructor:: Invalid HRU ID: " << source_hru_id << std::endl;
    ExitGracefully("CLatConnect constructor:: Invalid HRU ID", BAD_DATA);
  }
  if (connected_hru_id < 0) {
    std::cerr << "CLatConnect constructor:: Invalid Connected HRU ID: " << connected_hru_id << std::endl;
    ExitGracefully("CLatConnect constructor:: Invalid Connected HRU ID", BAD_DATA);
  }
  if (weight < 0.0) {
    std::cerr << "CLatConnect constructor:: Invalid weight: " << weight << std::endl;
    ExitGracefully("CLatConnect constructor:: Invalid weight", BAD_DATA);
  }
}

///////////////////////////////////////////////////////////////////
/// \brief Destructor for CLatConnect
//
CLatConnect::~CLatConnect()
{
  // Destructor logic if needed
}

///////////////////////////////////////////////////////////////////
/// \brief Returns the HRU ID
/// \return Integer HRU ID
//
int CLatConnect::GetHRUID() const
{
  return source_hru_id;
}

///////////////////////////////////////////////////////////////////
/// \brief Returns the connected HRU ID
/// \return Integer connected HRU ID
//
int CLatConnect::GetConnectedHRUID() const
{
  return connected_hru_id;
}

///////////////////////////////////////////////////////////////////
/// \brief Returns the weight
/// \return Double weight
//
double CLatConnect::GetWeight() const
{
  return weight;
}


