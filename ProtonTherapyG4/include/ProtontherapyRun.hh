#ifndef ProtontherapyRun_h
#define ProtontherapyRun_h 1

#include "G4Run.hh"
#include "G4Event.hh"

#include "G4THitsMap.hh"
#include <vector>

using namespace std;

class ProtontherapyRun : public G4Run {

public:
  // constructor and destructor.
  //  vector of multifunctionaldetector name has to given to constructor.
    ProtontherapyRun();
    ProtontherapyRun(const vector<G4String> mfdName);
    virtual ~ProtontherapyRun();

public:
    // virtual method from G4Run. 
    // The method is overriden in this class for scoring.
    virtual void RecordEvent(const G4Event*);

    // Access methods for scoring information.
    // - Number of HitsMap for this RUN. 
    //   This is equal to number of collections.
    G4int GetNumberOfHitsMap() const {return fRunMap.size();}
    // - Get HitsMap of this RUN.
    //   by sequential number, by multifucntional name and collection name,
    //   and by collection name with full path.
    G4THitsMap<G4double>* GetHitsMap(G4int i) const {return fRunMap[i];}
    G4THitsMap<G4double>* GetHitsMap(const G4String& detName, 
                                    const G4String& colName) const;
    G4THitsMap<G4double>* GetHitsMap(const G4String& fullName) const;

    void ConstructMFD(const vector<G4String>&);

    virtual void Merge(const G4Run*);

private:
  vector<G4String> fCollName;
  vector<G4int> fCollID;
  vector<G4THitsMap<G4double>*> fRunMap;

};

//==========================================================================
//          Generic Functions to help with merge
//==========================================================================
template <typename T>
inline void Copy(vector<T>& main, const vector<T>& data)
{
    for(unsigned i = main.size(); i < data.size(); ++i) {
        main.push_back(data.at(i));
    }
}
//==========================================================================
template <typename T>
inline unsigned Copy(vector<T*>& main, const vector<T*>& data)
{
    unsigned size_diff = data.size() - main.size();
    for(unsigned i = main.size(); i < data.size(); ++i) {
        main.push_back(new T(*data.at(i)));
    }
    return size_diff;
}
//==========================================================================
template <typename T>
inline void Print(const vector<T>& data)
{
    G4cout << G4endl;
    for(unsigned i = 0; i < data.size(); ++i) {
        G4cout << "\t\t" << i << " \t\t " << data.at(i) << G4endl;
    }
    G4cout << G4endl;
}
//==========================================================================

#endif
