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
                                       redist_method method,
                                       CModel *pModel)
    : CLateralExchangeProcessABC(LAT_REDISTRIBUTE, pModel),
      _max_snow_height(max_snow_height),
      _method(method)
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
void CmvLatRedistribute::GetLateralExchange(const double *const *state_vars,
                                            const CHydroUnit *const *pHRUs,
                                            const optStruct &Options,
                                            const time_struct &tt,
                                            double *exchange_rates) const
{
    const double PI = 3.141592653589793;
    const double TAN_80_DEGREES = tan(80.0 * PI / 180.0);
    const double RAD_TO_DEG = 180.0 / PI;
    const double _k_param = 0.05; // Bernhardt & Schulz (2010) SnowSlide threshold method parameter

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
        double slope_rad = pHRUs[fromHRU-1]->GetSlope();
        double snowSWE = state_vars[fromHRU-1][_iRedistributeFrom];

        double snowMoved = 0.0;

        if (_method == CONTINUOUS_REDIST)
        {
            // Original continuous method
            double snowTransport = std::min(0.5, slope_rad / TAN_80_DEGREES) * std::min(1.0, snowSWE / _max_snow_height);
            snowMoved = snowTransport * snowSWE;
        }
        else if (_method == THRESHOLD_REDIST)
        {
            // Bernhardt & Schulz (2010) SnowSlide threshold method
            const double SNOWSLIDE_LIMIT_ANGLE = 60.0; // Maximum angle in degrees
            double slope_deg = slope_rad * RAD_TO_DEG;

            // Calculate threshold snow height based on slope angle
            double threshold_snow_height = _max_snow_height * exp(-_k_param * slope_deg);

            // Ensure no accumulation above SNOWSLIDE_LIMIT_ANGLE
            if (slope_deg > SNOWSLIDE_LIMIT_ANGLE) {
            threshold_snow_height = 0.0;
            }

            // Calculate excess snow to be redistributed
            if (snowSWE > threshold_snow_height) {
            snowMoved = snowSWE - threshold_snow_height;
            }
        }

        // Calculate the exchange rate considering area
        exchange_rates[q] = (snowMoved * weight * areaFrom) / Options.timestep; // [mm-km2/d]

        // Debug info - uncomment when needed
        /*
        std::cout << "Connection " << q << ": fromHRU=" << fromHRU << " toHRU=" << toHRU 
        << " method=" << (_method == CONTINUOUS_REDIST ? "CONTINUOUS" : "THRESHOLD")
        << " snowSWE=" << snowSWE << " snowMoved=" << snowMoved 
        << " exchange_rate=" << exchange_rates[q] << std::endl;
        */
    }
}
