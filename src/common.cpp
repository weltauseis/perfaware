#include "common.h"
#include <cmath>
#include <numbers>

#define EARTH_RADIUS 6372.8

f64 square(f64 x)
{
    return x * x;
}

f64 rad_from_deg(f64 degrees)
{
    return (std::numbers::pi / 360.) * degrees * 2.;
}

f64 reference_haversine(f64 x0, f64 y0, f64 x1, f64 y1)
{
    f64 lat1 = y0;
    f64 lat2 = y1;
    f64 lon1 = x0;
    f64 lon2 = x1;

    f64 dlat = rad_from_deg(lat2 - lat1);
    f64 dlon = rad_from_deg(lon2 - lon1);
    lat1 = rad_from_deg(lat1);
    lat2 = rad_from_deg(lat2);

    f64 a = square(sin(dlat / 2.0)) + cos(lat1) * cos(lat2) * square(sin(dlon / 2));
    f64 c = 2.0 * asin(sqrt(a));

    return EARTH_RADIUS * c;
}

