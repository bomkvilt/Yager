#ifndef COMMON_HIT_TIMER_HPP
#define COMMON_HIT_TIMER_HPP


class HitTimer
{
private:
	size_t hits;
	size_t required;

public:
	HitTimer(size_t required = 0);

	size_t GetHits() const;	//!< returns number of hits
	size_t GetNorm() const;	//!< returns count of hits required for an activation
	void SetNorm(size_t required);	//!< sets count of activation hits

	/** Perform a number of hits
	 * @count	- number of hits
	 * @return	- (hits += count)  > required ?
	 */
	bool Hit(size_t count = 1);
};


#endif //!COMMON_HIT_TIMER_HPP
