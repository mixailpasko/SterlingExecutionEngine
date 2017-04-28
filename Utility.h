#ifndef Common_Utility_Header
#define Common_Utility_Header

#include <iostream>

#define IMPLEMENT_SET_METHOD( type , name )	\
private:	type name;	\
public:		void set_##name(  type value ) {  name = value; }

#define IMPLEMENT_GET_METHOD( type , name )	\
public:		 type get_##name() 	{ return name; }

#define IMPLEMENT_SET_GET_METHOD( type , name ) \
	IMPLEMENT_SET_METHOD( type , name );	\
	IMPLEMENT_GET_METHOD( type , name );



namespace Framework
{
	namespace Utility
	{
		class CommnUtility
		{
		   static std::string base64_chars;
		   bool is_base64(unsigned char c);
		public:
		 std::string  base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len); //base64_encode(reinterpret_cast<const unsigned char*>(ser.c_str()), ser.length());
		 std::string base64_decode(std::string const& encoded_string);
		};

	}//end namespace ExecutionEngine
}//end namespace Framework
#endif 