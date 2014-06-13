/*
 * NonStrictSearch.cpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#include "TimeDartReachabilitySearch.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

bool TimeDartReachabilitySearch::verify(){
	// no need to check if trace is set, just as expensive as setting the generated by
	initialMarking.setGeneratedBy(NULL);

	if(addToPW(&initialMarking, NULL, INT_MAX)){
		return true;
	}

	//Main loop
	while(pwList->hasWaitingStates()){
		TimeDartBase& dart = *pwList->getNextUnexplored();
		exploredMarkings++;

		int passed = dart.getPassed();
		dart.setPassed(dart.getWaiting());
		tapn.getTransitions();
                this->tmpdart = NULL;
                if(options.getTrace() == VerificationOptions::SOME_TRACE){
                    this->tmpdart = ((ReachabilityTraceableDart*)&dart)->trace;
                }
		for(TimedTransition::Vector::const_iterator transition_iter = tapn.getTransitions().begin();
				transition_iter != tapn.getTransitions().end(); transition_iter++){
			TimedTransition& transition = **transition_iter;
			pair<int,int> calculatedStart = calculateStart(transition, dart.getBase());
			if(calculatedStart.first == -1){	// Transition cannot be enabled in marking
				continue;
			}
			int start = max(dart.getWaiting(), calculatedStart.first);
			int end = min(passed-1, calculatedStart.second);

			if(start <= end){

				if(transition.hasUntimedPostset()){
					NonStrictMarkingBase Mpp(*dart.getBase());
					Mpp.incrementAge(start);

                                        this->tmpupper = start;
                                        if(successorGenerator.generateAndInsertSuccessors(Mpp, transition)){
                                            return true;
                                        }
				}else{
					int stop = min(max(start, calculateStop(transition, dart.getBase())), end);
					for(int n = start; n <= stop; n++){
						NonStrictMarkingBase Mpp(*dart.getBase());
						Mpp.incrementAge(n);
                                                this->tmpupper = n;
                                                if(successorGenerator.generateAndInsertSuccessors(Mpp, transition)){
                                                    return true;
                                                }
					}
				}
			}
		}
                deleteBase(dart.getBase());
	}

	return false;
}

bool TimeDartReachabilitySearch::addToPW(NonStrictMarkingBase* marking, WaitingDart* parent, int upper){
        int start = 0;
        if(options.getTrace() == VerificationOptions::SOME_TRACE){
            start = marking->getYoungest();
        }
	int maxDelay = marking->cut();

	unsigned int size = marking->size();

	pwList->setMaxNumTokensIfGreater(size);

	if(size > options.getKBound()) {
                delete marking;
		return false;
	}
        
        int youngest = marking->makeBase();
        
	if(pwList->add( marking, youngest, parent, upper, start)){


                if(maxDelay != std::numeric_limits<int>::max())
                    maxDelay += youngest;
                if(maxDelay > tapn.getMaxConstant()){
                    maxDelay = tapn.getMaxConstant() + 1;
                }
                
                if(this->options.getTrace() == VerificationOptions::FASTEST_TRACE){
				   int totalDelay = (parent && parent) ? parent->meta->totalDelay : 0;
				   if(marking->getGeneratedBy() == NULL) ++totalDelay;
				   marking->meta->totalDelay = totalDelay;
                }

		QueryVisitor<NonStrictMarkingBase> checker(*marking, tapn, maxDelay);
                AST::BoolResult context;
		query->accept(checker, context);
		if(context.value) {
                        if (options.getTrace()) {
                            lastMarking = pwList->getLast();
                        }
			return true;
		} else {
                        deleteBase(marking);
			return false;
		}
	}
        deleteBase(marking);
	return false;
}

void TimeDartReachabilitySearch::printStats(){
	std::cout << "  discovered markings:\t" << pwList->discoveredMarkings << std::endl;
	std::cout << "  explored markings:\t" << exploredMarkings << std::endl;
	std::cout << "  stored markings:\t" << pwList->size() << std::endl;
}

TimeDartReachabilitySearch::~TimeDartReachabilitySearch() {

}

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
