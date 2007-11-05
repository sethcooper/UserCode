// Define bozo cluster object
// 3x3 cluster

#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"

class BozoCluster
{
  public:
   explicit BozoCluster() {energy_=-1;}
   explicit BozoCluster(EcalRecHit seed);
   explicit BozoCluster(EcalRecHit seed, double energy);
   
   EcalRecHit getSeed() {return seed_;}
   double getEnergy() {return energy_;}
   
  private:
   EcalRecHit seed_;
   double energy_;
};

BozoCluster::BozoCluster(EcalRecHit seed)
{
  seed_ = seed;
}

BozoCluster::BozoCluster(EcalRecHit seed, double energy)
{
  seed_ = seed;
  energy_ = energy;
}

