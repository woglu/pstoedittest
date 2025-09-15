

namespace GenericInts {
	enum TypeMapperTypeEnum { char_t, short_t, int_t, long_t, uchar_t, ushort_t, uint_t, ulong_t, notavailable_t }; 

//need a macro here because C++ does not allow a function call to be used as template instantiation parameter
#define SizeToTypeMapper_gettype(size) (\
	(size)==8*sizeof(signed char) ?	GenericInts::char_t: \
	(size)==8*sizeof(signed short)?	GenericInts::short_t: \
	(size)==8*sizeof(signed int)  ?	GenericInts::int_t: \
	(size)==8*sizeof(signed long) ?	GenericInts::long_t: \
	GenericInts::notavailable_t	)

template <GenericInts::TypeMapperTypeEnum t> struct TypeMapper;
//
// the following template specialisations map from the type enum to the concrete type
//
template<> struct TypeMapper<GenericInts::char_t >	{ typedef signed   char	type;};
template<> struct TypeMapper<GenericInts::short_t>	{ typedef signed   short	type;};
template<> struct TypeMapper<GenericInts::int_t  >	{ typedef signed   int	type;};
template<> struct TypeMapper<GenericInts::long_t >	{ typedef signed   long	type;};
template<> struct TypeMapper<GenericInts::uchar_t >	{ typedef unsigned char	type;};
template<> struct TypeMapper<GenericInts::ushort_t>	{ typedef unsigned short	type;};
template<> struct TypeMapper<GenericInts::uint_t  >	{ typedef unsigned int	type;};
template<> struct TypeMapper<GenericInts::ulong_t >	{ typedef unsigned long	type;};

template <unsigned int size> struct Int {
	typedef typename TypeMapper< SizeToTypeMapper_gettype(size) >::type  signedtype;
	typedef typename TypeMapper< (GenericInts::TypeMapperTypeEnum) (SizeToTypeMapper_gettype(size) + (GenericInts::uchar_t - GenericInts::char_t)) >::type  unsignedtype;
};

}
