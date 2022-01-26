#include <locale.h>

#include <type_traits>

static_assert(
	std::is_same<decltype(::uselocale(std::declval<locale_t>())), locale_t>::value, "no compatible uselocale found");

int main()
{
	return 1;
}
