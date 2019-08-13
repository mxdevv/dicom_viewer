#ifndef __STRING_SWITCH_CPP__
#define __STRING_SWITCH_CPP__

namespace _string_switch {

constexpr unsigned long pow(unsigned long a, unsigned long n)
{
	unsigned long r = 1;
	while(n)
		if (n & 1) {
			r *= a;
			--n;
		} else {
			a *= a;
			n >>= 1;
		}
	return r;
}

constexpr unsigned long hash(const char* chs)
{
	unsigned long hash = 0;
	for(unsigned long i = 0; chs[i] != '\0'; i++)
		hash += (unsigned long)chs[i] * pow(31, i);
	return hash;
}

} // _string_switch

#define sswitch(str) switch(_string_switch::hash(str))
#define scase(str) case _string_switch::hash(str):

#endif
