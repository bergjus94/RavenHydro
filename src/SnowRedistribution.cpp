/*----------------------------------------------------------------
Raven Library Source Code
Copyright (c) 2008-2017 the Raven Development Team
------------------------------------------------------------------
LatRedistribute (redistribute snow based on slope and snow SWE)
----------------------------------------------------------------*/
#include "RavenInclude.h"
#include "HydroProcessABC.h"
#include "LatConnect.h"
#include <cmath>

/*****************************************************************
   Constructor/Destructor
------------------------------------------------------------------
*****************************************************************/

CmvLatRedistribute::CmvLatRedistribute(int sv_ind,
                                       double max_snow_height,
                                       CModel *pModel)
    : CLateralExchangeProcessABC(LAT_REDISTRIBUTE, pModel),
      _max_snow_height(max_snow_height)
{
    _iRedistributeFrom = sv_ind;
    _iRedistributeTo = sv_ind;

    DynamicSpecifyConnections(0); // purely lateral flow, no vertical

    // check for valid SV index
    ExitGracefullyIf(sv_ind == DOESNT_EXIST, "CmvLatRedistribute::unrecognized state variable specified in :SnowRedistribute command", BAD_DATA_WARN);
}

//////////////////////////////////////////////////////////////////
/// \brief Implementation of the default destructor
//
CmvLatRedistribute::~CmvLatRedistribute() {}

//////////////////////////////////////////////////////////////////
/// \brief Initialization (prior to solution)
//
void CmvLatRedistribute::Initialize()
{

    std::cout << "CmvLatRedistribute::Initialize called" << std::endl;

    // Calculate the number of lateral connections
    int numLatConnections = _pModel->GetNumLatConnections();

    // Allocate memory for the arrays
    _kFrom = new int[numLatConnections];
    _kTo = new int[numLatConnections];
    _iFromLat = new int[numLatConnections];
    _iToLat = new int[numLatConnections];

    // Initialize the arrays using the methods from CLatConnect
    for (int q = 0; q < numLatConnections; q++) {
        CLatConnect* connection = _pModel->GetLatConnection(q);
        _kFrom[q] = connection->GetHRUID()-1;
        _kTo[q] = connection->GetConnectedHRUID()-1;
        _iFromLat[q] = _iRedistributeFrom; // Assuming _iRedistributeFrom is defined
        _iToLat[q] = _iRedistributeTo; // Assuming _iRedistributeTo is defined
    }
}

//////////////////////////////////////////////////////////////////
/// \brief Sets reference to participating state variables
/// \param se_type [in] Model of soil evaporation used
/// \param *aSV [out] Array of state variable types needed by soil evaporation algorithm
/// \param *aLev [out] Array of level of multilevel state variables (or DOESNT_EXIST, if single level)
/// \param &nSV [out] Number of state variables required by soil evaporation algorithm (size of aSV[] and aLev[] arrays)
//
void CmvLatRedistribute::GetParticipatingStateVarList(sv_type *aSV, int *aLev, int &nSV)
{
    nSV = 0;
    // user specified 'from' & 'to' compartment, Levels - not known before construction
}

void CmvLatRedistribute::GetParticipatingParamList(string *aP, class_type *aPC, int &nP) const
{
    nP = 0;
}

//////////////////////////////////////////////////////////////////
/// \brief returns lateral exchange rates (mm/d) between from and to HRU/SV combinations
/// \param **state_vars [in] 2D array of current state variables [nHRUs][nSVs]
/// \param **pHRUs [in] array of pointers to HRUs
/// \param &Options [in] Global model options information
/// \param &tt [in] Specified point at time at which this accessing takes place
/// \param *exchange_rates [out] Rate of loss from "from" compartment [mm-km2/day]
//
void CmvLatRedistribute::GetLateralExchange(const double *const *state_vars, // array of all SVs for all HRUs, [k][i]
                                            const CHydroUnit *const *pHRUs,
                                            const optStruct &Options,
                                            const time_struct &tt,
                                            double *exchange_rates) const
{
    // Add a print statement to verify if this function is being called
    //std::cout << "CmvLatRedistribute::GetLateralExchange called" << std::endl;

    const double PI = 3.141592653589793;
    //const double MAX_SNOW_HEIGHT = 15000.0; // in mm
    const double TAN_80_DEGREES = tan(80.0 * PI / 180.0);

    for (int q = 0; q < _pModel->GetNumLatConnections(); q++)
    {
        CLatConnect *connection = _pModel->GetLatConnection(q);
        int fromHRU = connection->GetHRUID();
        int toHRU = connection->GetConnectedHRUID();
        double weight = connection->GetWeight();

        // Get the areas of both HRUs
        double areaFrom = pHRUs[fromHRU-1]->GetArea();
        double areaTo = pHRUs[toHRU-1]->GetArea();

        // Get slope and SWE from source HRU
        double slope = pHRUs[fromHRU-1]->GetSlope();
        double snowSWE = state_vars[fromHRU-1][_iRedistributeFrom];

        double snowTransport = std::min(0.5, slope / TAN_80_DEGREES) * std::min(1.0, snowSWE / _max_snow_height);
        double snowMoved = snowTransport * snowSWE;

        // Calculate the exchange rate considering area
        // The exchange rate is the amount of snow moved from the source HRU
        // The units are [mm-km2/d] - volume per time
        exchange_rates[q] = (snowMoved * weight * areaFrom) / Options.timestep; // [mm-km2/d]

        // Debug info
        //std::cout << "Connection " << q << ": fromHRU=" << fromHRU << " toHRU=" << toHRU 
        //          << " slope=" << slope << " snowSWE=" << snowSWE 
        //          << " snowTransport=" << snowTransport << " snowMoved=" << snowMoved 
        //          << " areaFrom=" << areaFrom << " areaTo=" << areaTo 
        //          << " exchange_rate=" << exchange_rates[q] << std::endl;
    }
}


