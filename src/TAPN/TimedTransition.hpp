#ifndef VERIFYTAPN_TAPN_TIMEDTRANSITION_HPP_
#define VERIFYTAPN_TAPN_TIMEDTRANSITION_HPP_

#include <string>
#include <vector>
#include "boost/shared_ptr.hpp"

namespace VerifyTAPN {
	namespace TAPN {

		class TimedTransition
		{
		public: // typedefs
			typedef std::vector< boost::shared_ptr<TimedTransition> > Vector;
		public:
			TimedTransition(const std::string& name) : name(name) { };
			virtual ~TimedTransition() { /* empty */ }

		public: // inspectors
			const std::string& GetName() const;
			void Print(std::ostream&) const;

		private: // data
			const std::string name;
		};

		inline std::ostream& operator<<(std::ostream& out, const TimedTransition& transition)
		{
			transition.Print(out);
			return out;
		}
	}
}

#endif /* VERIFYTAPN_TAPN_TIMEDTRANSITION_HPP_ */