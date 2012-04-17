/*
 * NonStrictMarking.cpp
 *
 *  Created on: 29/02/2012
 *      Author: MathiasGS
 */

#include "NonStrictMarking.hpp"
#include <iostream>

using namespace std;

namespace VerifyTAPN {
namespace DiscreteVerification {

NonStrictMarking::NonStrictMarking() {
	// TODO Auto-generated constructor stub

}

NonStrictMarking::NonStrictMarking(const std::vector<int>& v){
	int prevPlaceId = -1;
	for(std::vector<int>::const_iterator iter = v.begin(); iter != v.end(); iter++){
		if(*iter == prevPlaceId){
			Place& p = places.back();
			if(p.tokens.size() == 0){
				Token t(0,1);
				p.tokens.push_back(t);
			}else{
				p.tokens.begin()->add(1);
			}
		}else{
			Place p(*iter);
			Token t(0,1);
			p.tokens.push_back(t);
			places.push_back(p);
		}
		prevPlaceId = *iter;
	}
}

NonStrictMarking::NonStrictMarking(const NonStrictMarking& nsm) {
	for(PlaceList::const_iterator it = nsm.places.begin(); it != nsm.places.end(); it++){
		places.push_back(Place(*it));
	}
}

unsigned int NonStrictMarking::size(){
	int count = 0;
	for(PlaceList::const_iterator iter = places.begin(); iter != places.end(); iter++){
		for(TokenList::const_iterator it = iter->tokens.begin(); it != iter->tokens.end(); it++){
			count += it->getCount();
		}
	}
	return count;
}

//int NonStrictMarking::NumberOfTokensInPlace(const Place& place) const{
//	int count = 0;
//	for(TokenList::const_iterator it = place.tokens.begin(); it != place.tokens.end(); it++){
//		count = count + it->getCount();
//	}
//	return count;
//}

int NonStrictMarking::NumberOfTokensInPlace(int placeId) const{
	int count = 0;
	for(PlaceList::const_iterator iter = places.begin(); iter != places.end(); iter++){
		if(iter->id == placeId){
			for(TokenList::const_iterator it = iter->tokens.begin(); it != iter->tokens.end(); it++){
				count = count + it->getCount();
			}
		}
	}
	return count;
}

const TokenList& NonStrictMarking::GetTokenList(int placeId){
	for(PlaceList::const_iterator iter = places.begin(); iter != places.end(); iter++){
		if(iter->id == placeId) return iter->tokens;
	}
	return emptyTokenList;
}

bool NonStrictMarking::RemoveToken(int placeId, int age){
	for(PlaceList::iterator pit = places.begin(); pit != places.end(); pit++){
		if(pit->id == placeId){
			for(TokenList::iterator tit = pit->tokens.begin(); tit != pit->tokens.end(); tit++){
				if(tit->getAge() == age){
					return RemoveToken(*pit, *tit);
				}
			}
		}
	}
	return false;
}

void NonStrictMarking::RemoveRangeOfTokens(Place& place, TokenList::iterator begin, TokenList::iterator end){
	place.tokens.erase(begin, end);
}

bool NonStrictMarking::RemoveToken(Place& place, Token& token){
	if(token.getCount() > 1){
		token.remove(1);
		return true;
	}else{
		for(TokenList::iterator iter = place.tokens.begin(); iter != place.tokens.end(); iter++){
			if(iter->getAge() == token.getAge()){
				place.tokens.erase(iter);
				if(place.tokens.size() == 0){
					for(PlaceList::iterator it = places.begin(); it != places.end(); it++){
						if(it->id == place.id){
							places.erase(it);
							return true;
						}
					}
				}
				return true;
			}
		}
	}
	return false;
}

void NonStrictMarking::AddTokenInPlace(int placeId, int age){
	for(PlaceList::iterator pit = places.begin(); pit != places.end(); pit++){
		if(pit->id == placeId){
			for(TokenList::iterator tit = pit->tokens.begin(); tit != pit->tokens.end(); tit++){
				if(tit->getAge() == age){
					Token t(age, 1);
					AddTokenInPlace(*pit, t);
					return;
				}
			}
			Token t(age,1);
			AddTokenInPlace(*pit, t);
			return;
		}
	}
	Token t(age,1);
	Place p(placeId);
	AddTokenInPlace(p,t);

	// Insert place
	bool inserted = false;
	for(PlaceList::iterator it = places.begin(); it != places.end(); it++){
		if(it->id > placeId){
			places.insert(it, p);
			inserted = true;
			break;
		}
	}
	if(!inserted){
		places.push_back(p);
	}
}

void NonStrictMarking::AddTokenInPlace(int placeid, Token& token){
	for(PlaceList::iterator pit = places.begin(); pit != places.end(); pit++){
		if(pit->id == placeid){
			AddTokenInPlace(*pit, token);
			return;
		}
	}

	Place p(placeid);
	AddTokenInPlace(p,token);

	// Insert place
	bool inserted = false;
	for(PlaceList::iterator it = places.begin(); it != places.end(); it++){
		if(it->id > placeid){
			places.insert(it, p);
			inserted = true;
			break;
		}
	}
	if(!inserted){
		places.push_back(p);
	}
}

void NonStrictMarking::AddTokenInPlace(Place& place, Token& token){
	if(token.getCount() == 0) return;
	for(TokenList::iterator iter = place.tokens.begin(); iter != place.tokens.end(); iter++){
		if(iter->getAge() == token.getAge()){
			iter->add(token.getCount());
			return;
		}
	}
	// Insert token
	bool inserted = false;
	for(TokenList::iterator it = place.tokens.begin(); it != place.tokens.end(); it++){
		if(it->getAge() > token.getAge()){
			place.tokens.insert(it, token);
			inserted = true;
			break;
		}
	}
	if(!inserted){
		place.tokens.push_back(token);
	}
}

void NonStrictMarking::incrementAge(){
	for(PlaceList::iterator iter = places.begin(); iter != places.end(); iter++){
		iter->incrementAge();
	}
}

NonStrictMarking::~NonStrictMarking() {
	// TODO: Should we destruct something here? (places)
}

bool NonStrictMarking::equals(const NonStrictMarking &m1) const{
	if(m1.places.size() == 0) return false;
	if(m1.places.size() != places.size())	return false;

	PlaceList::const_iterator p_iter = m1.places.begin();
	for(PlaceList::const_iterator iter = places.begin(); iter != places.end(); iter++, p_iter++){
		if(iter->id != p_iter->id)	return false;
		if(iter->tokens.size() != p_iter->tokens.size())	return false;
		TokenList::const_iterator pt_iter = p_iter->tokens.begin();
		for(TokenList::const_iterator t_iter = iter->tokens.begin(); t_iter != iter->tokens.end(); t_iter++, pt_iter++){
			if(!t_iter->equals(*pt_iter))	return false;
		}
	}

	return true;
}

std::ostream& operator<<(std::ostream& out, NonStrictMarking& x ) {
	out << "-";
	for(PlaceList::iterator iter = x.places.begin(); iter != x.places.end(); iter++){
		out << "place " << iter->id << " has tokens (age, count): ";
		for(TokenList::iterator it = iter->tokens.begin(); it != iter->tokens.end(); it++){
			out << "(" << it->getAge() << ", " << it->getCount() << ") ";
		}
		if(iter != x.places.end()-1){
			out << endl;
		}
	}

	return out;
}

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
