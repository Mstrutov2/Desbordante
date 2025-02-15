//
// Created by Ilya Vologin
// https://github.com/cupertank
//

#pragma once

#include <map>
#include <type_traits>
#include <boost/dynamic_bitset.hpp>
#include "ColumnLayoutRelationData.h"
#include "Vertical.h"
#include "ConfidenceInterval.h"
#include "custom/CustomRandom.h"

using std::enable_if, std::is_base_of, boost::dynamic_bitset, std::map;

//abstract base class for Agree Set Sample implementations (trie <- not used, list)
class AgreeSetSample {
public:

    virtual unsigned long long getNumAgreeSupersets(std::shared_ptr<Vertical> agreement) = 0;
    virtual unsigned long long getNumAgreeSupersets(std::shared_ptr<Vertical> agreement, std::shared_ptr<Vertical> disagreement) = 0;
    virtual std::shared_ptr<std::vector<unsigned long long>> getNumAgreeSupersetsExt(std::shared_ptr<Vertical> agreement, std::shared_ptr<Vertical> disagreement);

    double estimateAgreements(std::shared_ptr<Vertical> agreement);
    ConfidenceInterval estimateAgreements(std::shared_ptr<Vertical> agreement, double confidence);
    ConfidenceInterval estimateMixed(std::shared_ptr<Vertical> agreement, std::shared_ptr<Vertical> disagreement, double confidence);

    double getSamplingRatio() { return sampleSize / static_cast<double>(populationSize); }
    bool isExact() { return populationSize == sampleSize; }

    virtual ~AgreeSetSample() = default;

protected:
    shared_ptr<ColumnLayoutRelationData> relationData;
    shared_ptr<Vertical> focus;
    unsigned int sampleSize;
    unsigned long long populationSize;
    AgreeSetSample(shared_ptr<ColumnLayoutRelationData> relationData, shared_ptr<Vertical> focus, unsigned int sampleSize, unsigned long long populationSize);

    //template<typename T, typename enable_if<is_base_of<AgreeSetSample, T>::value>::type* = nullptr> //- TODO: SFINAE???
    template<typename T>
    static shared_ptr<T> createFor(shared_ptr<ColumnLayoutRelationData> relationData, int sampleSize);

    //template<typename T, typename enable_if<is_base_of<AgreeSetSample, T>::value>::type* = nullptr> // - TODO: SFINAE???
    template<typename T>
    static shared_ptr<T> createFocusedFor(shared_ptr<ColumnLayoutRelationData> relation,
                                          shared_ptr<Vertical> restrictionVertical,
                                          shared_ptr<PositionListIndex> restrictionPli,
                                          unsigned int sampleSize, CustomRandom& random);
private:
    static double stdDevSmoothing;

    double ratioToRelationRatio(double ratio) { return ratio * populationSize / relationData->getNumTuplePairs(); }
    double observationsToRelationRatio(double numObservations) { return ratioToRelationRatio(numObservations / sampleSize); }
    static double calculateNonNegativeFraction(double a, double b);

    ConfidenceInterval estimateGivenNumHits(unsigned long long numHits, double confidence);
    // Inverse cumulative distribution function (aka the probit function)
    double probitFunction(double quantile) const;
};

//include template implementation
#include "AgreeSetSample_impl.h"
