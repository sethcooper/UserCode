#include "DataFormats/Common/interface/Wrapper.h"
#include "DataFormats/Common/interface/AssociationMap.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

namespace { namespace {
      edm::helpers::KeyVal<edm::RefProd<std::vector<reco::Track> >,edm::RefProd<edm::SortedCollection<EcalRecHit,edm::StrictWeakOrdering<EcalRecHit> > > > dummy3;
      edm::AssociationMap<edm::OneToOne<reco::TrackCollection, EcalRecHitCollection, unsigned int> >  dummy2;
      edm::Wrapper<edm::AssociationMap<edm::OneToOne<reco::TrackCollection, EcalRecHitCollection, unsigned int> > > dummy1;
}}
