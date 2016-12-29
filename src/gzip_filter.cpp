#include <zlib/gzip_filter.hpp>
#include <zlib.h>
#include <stdexcept>
#include <assert.h>

struct gzip_filter::impl
{
	bool compress;
	z_stream z;

	void close()
	{
		if (compress)
			deflateEnd(&z);
		else
			inflateEnd(&z);
	}
};

gzip_filter::gzip_filter(bool compress)
	: pimpl_(new impl{ compress })
{
	pimpl_->z.zalloc = nullptr;
	pimpl_->z.zfree = nullptr;
	pimpl_->z.opaque = nullptr;
	pimpl_->z.next_in = nullptr;

	int r;
	if (compress)
		r = deflateInit2(&pimpl_->z, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 15 + 16, 8, Z_DEFAULT_STRATEGY);
	else
		r = inflateInit2(&pimpl_->z, 15 + 16);

	if (r < 0)
	{
		delete pimpl_;
		throw std::runtime_error("cannot initialize zlib");
	}
}

gzip_filter::gzip_filter(gzip_filter && o)
	: pimpl_(o.pimpl_)
{
	o.pimpl_ = nullptr;
}

gzip_filter::~gzip_filter()
{
	if (pimpl_)
	{
		pimpl_->close();
		delete pimpl_;
	}
}

gzip_filter & gzip_filter::operator=(gzip_filter && o)
{
	std::swap(pimpl_, o.pimpl_);
	return *this;
}

std::pair<size_t, size_t> gzip_filter::process(char const * inbuf, size_t inlen, char * outbuf, size_t outlen)
{
	assert(pimpl_ != nullptr);

	pimpl_->z.next_in = (Bytef *)inbuf;
	pimpl_->z.avail_in = inlen;

	pimpl_->z.next_out = (Bytef *)outbuf;
	pimpl_->z.avail_out = outlen;

	int r;
	if (pimpl_->compress)
		r = deflate(&pimpl_->z, Z_NO_FLUSH);
	else
		r = inflate(&pimpl_->z, Z_NO_FLUSH);

	if (r < 0)
		throw std::runtime_error("zlib error");

	return std::make_pair(pimpl_->z.next_in - (Bytef *)inbuf, pimpl_->z.next_out - (Bytef *)outbuf);
}

size_t gzip_filter::finish(char * outbuf, size_t outlen)
{
	if (pimpl_ == nullptr)
		return 0;

	pimpl_->z.next_in = nullptr;
	pimpl_->z.avail_in = 0;

	pimpl_->z.next_out = (Bytef *)outbuf;
	pimpl_->z.avail_out = outlen;

	size_t ret = 0;
	while (ret == 0)
	{
		int r;
		if (pimpl_->compress)
			r = deflate(&pimpl_->z, Z_FINISH);
		else
			r = inflate(&pimpl_->z, Z_FINISH);

		if (r < 0)
			throw std::runtime_error("zlib error");

		ret = pimpl_->z.next_out - (Bytef *)outbuf;

		if (r == Z_STREAM_END)
		{
			pimpl_->close();
			delete pimpl_;
			pimpl_ = nullptr;
			break;
		}
	}

	return ret;
}
