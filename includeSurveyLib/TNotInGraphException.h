#ifndef T_NOT_IN_GRAPH_EXCEPTION_89670E6B_13FA_4ec4_A91E_C7E925AAF765
#define T_NOT_IN_GRAPH_EXCEPTION_89670E6B_13FA_4ec4_A91E_C7E925AAF765

#include <stdexcept>

struct TNotInGraphException : std::runtime_error
{
	explicit TNotInGraphException(const std::string & error_message)
		: std::runtime_error(error_message) {}
	//char const* what() const throw();
};

#endif
