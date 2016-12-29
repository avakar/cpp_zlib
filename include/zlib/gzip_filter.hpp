#ifndef ZLIB_GZIP_FILTER_HPP
#define ZLIB_GZIP_FILTER_HPP

#include <utility>

struct gzip_filter final
{
	explicit gzip_filter(bool compress);
	gzip_filter(gzip_filter && o);
	~gzip_filter();
	gzip_filter & operator=(gzip_filter && o);

	std::pair<size_t, size_t> process(char const * inbuf, size_t inlen, char * outbuf, size_t outlen);
	size_t finish(char * outbuf, size_t outlen);

private:
	struct impl;
	impl * pimpl_;
};

#endif // ZLIB_GZIP_FILTER_HPP
