#ifndef __DISTANCEDATUM_H
#define __DISTANCEDATUM_H

class DistanceDatum
{
   int missing;
   double value;

   friend class DistancesBlock;

public:
   DistanceDatum();
   ~DistanceDatum();
};

#endif
