/*----------------------------------------------------------------
  Raven Library Source Code
  Copyright (c) 2008-2023 the Raven Development Team
  ----------------------------------------------------------------*/
#ifndef LATCONNECT_H
#define LATCONNECT_H

#include "RavenInclude.h"
#include "ModelABC.h"
#include "Model.h"
#include "HydroUnits.h"
#include "HydroProcessABC.h"
#include <vector>
class CModel;

///////////////////////////////////////////////////////////////////
/// \brief Class to store lateral connection information
//
class CLatConnect
{
public:
  int source_hru_id; // HRU_ID of the source HRU
  int connected_hru_id; // HRU_ID of the connected HRU
  double weight; // Weight of the connection

  // Constructor
  CLatConnect(int hru, int connected_hru, double w);

  // Destructor
  ~CLatConnect();

  // Accessors
  int GetNumLatConnections() const;
  int GetHRUID() const;
  int GetConnectedHRUID() const;
  double GetWeight() const;

  // Static methods to interact with the collection
  static int GetNumLatConnections(const CModel *pModel);
  static CLatConnect* GetLatConnection(const CModel *pModel, int index);
  static bool CheckConnectionWeights(const CLatConnect* const connections[], 
    const int nConnections,
    const CModel* pModel,
    const double tolerance = 0.05);
};


#endif // LATCONNECT_H