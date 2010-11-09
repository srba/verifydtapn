#include "SearchStrategy.hpp"
#include "../TAPN/TimedArcPetriNet.hpp"

namespace VerifyTAPN
{
	DFS::DFS(
		const VerifyTAPN::TAPN::TimedArcPetriNet& tapn,
		SymMarking* initialMarking,
		const AST::Query* query,
		const VerificationOptions& options
	) : tapn(tapn), initialMarking(initialMarking), checker(query), options(options)
	{
		pwList = new PWList(new StackWaitingList);

		maxConstantsArray = new int[options.GetKBound()+1];
		for(int i = 0; i < options.GetKBound()+1; ++i)
		{
			maxConstantsArray[i] = tapn.MaxConstant();
		}
	};

	bool DFS::Verify()
	{
		initialMarking->Delay();

		if(options.GetSymmetry())
			initialMarking->Canonicalize();

		pwList->Add(*initialMarking);
		if(CheckQuery(*initialMarking)) return checker.IsEF(); // return true if EF query (proof found), or false if AG query (counter example found)

		while(pwList->HasWaitingStates()){
			SymMarking& next = pwList->GetNextUnexplored();

			typedef std::vector<SymMarking*> SuccessorVector;
			SuccessorVector successors;

			next.GenerateDiscreteTransitionSuccessors(tapn, options.GetKBound(), successors);

			for(SuccessorVector::iterator iter = successors.begin(); iter != successors.end(); ++iter)
			{
				SymMarking& succ = **iter;
				succ.Delay();
				succ.Extrapolate(maxConstantsArray);

				if(options.GetSymmetry())
					succ.Canonicalize();

				bool added = pwList->Add(succ);
				if(!added) delete *iter;
				else {
					if(CheckQuery(succ)) return checker.IsEF();
				}
			}

			//PrintDiagnostics(successors.size());
		}

		return checker.IsAG(); // return true if AG query (no counter example found), false if EF query (no proof found)
	}

	bool DFS::CheckQuery(const SymMarking& marking) const
	{
		bool satisfied = checker.IsExpressionSatisfied(marking);
		return (satisfied && checker.IsEF()) || (!satisfied && checker.IsAG());
	}

	Stats DFS::GetStats() const
	{
		return pwList->GetStats();
	}

	void DFS::PrintDiagnostics(size_t successors) const
	{
		pwList->Print();
		std::cout << ", successors: " << successors;
		std::cout << std::endl;
	}
}