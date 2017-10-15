#include "stdafx.h"
#include "GZipCompress.h"
#include <zlib/zlib.h>
#include <assert.h>


namespace mray
{
namespace Compress
{
	const int GZipCompress::m_defaultChunk = 16384;

	GZipCompress::GZipCompress()
	{
		m_bytes.resize(m_defaultChunk);
		m_outbytes.resize(m_defaultChunk);
	}
	GZipCompress::~GZipCompress()
	{
	}
	bool GZipCompress::ReadChunk(Chunk* input)
	{
		if (!input || input->eof())
			return false;
		inputSize = input->read(&m_bytes[0], m_defaultChunk);
		return true;
	}
	bool GZipCompress::isLossless()
	{
		return true;
	}

	int GZipCompress::compress(Chunk* input, Chunk* output)
	{
		z_stream m_zstrm;
		m_zstrm.zalloc = Z_NULL;
		m_zstrm.zfree = Z_NULL;
		m_zstrm.opaque = Z_NULL;
		deflateInit(&m_zstrm, 9);


		int flush = Z_NO_FLUSH;
		int ret = 0;
		unsigned int have = 0;
		bool flagStatus = true;
		int total = 0;
		do
		{
			ReadChunk(input);

			if (input->eof()) {
				flagStatus = false;
				flush = Z_FINISH;
			}

			m_zstrm.avail_in = inputSize;
			m_zstrm.next_in = &m_bytes[0];

			/* run deflate() on input until output buffer not full, finish
			compression if all of source has been read in */
			do {
				m_zstrm.avail_out = m_defaultChunk;
				m_zstrm.next_out = &m_outbytes[0];
				ret = deflate(&m_zstrm, flush);    /* no bad return value */
				assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
				have = m_defaultChunk - m_zstrm.avail_out;
				total += have;
				if (output->write(&m_outbytes[0], have) != have)
				{
					flagStatus = false;
					break;
				}
			} while (m_zstrm.avail_out == 0);
		} while (flagStatus);

		deflateEnd(&m_zstrm);

		return total;
	}
	int GZipCompress::decompress(Chunk* input, Chunk* output)
	{
		z_stream m_zstrm;
		m_zstrm.zalloc = Z_NULL;
		m_zstrm.zfree = Z_NULL;
		m_zstrm.opaque = Z_NULL;
		inflateInit(&m_zstrm);


		int ret = 0;
		ret = 0;
		unsigned int have = 0;
		bool flagStatus = true;
		int total = 0;
		do
		{
			ReadChunk(input);

			if (input->eof()) {
				flagStatus = false;
			}

			m_zstrm.avail_in = inputSize;
			m_zstrm.next_in = &m_bytes[0];

			/* run deflate() on input until output buffer not full, finish
			compression if all of source has been read in */
			do {
				m_zstrm.avail_out = m_defaultChunk;
				m_zstrm.next_out = &m_outbytes[0];
				ret = inflate(&m_zstrm, Z_NO_FLUSH);    /* no bad return value */
				assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
				have = m_defaultChunk - m_zstrm.avail_out;
				total += have;
				if (output->write(&m_outbytes[0], have) != have)
				{
					flagStatus = false;
					break;
				}
			} while (m_zstrm.avail_out == 0);
		} while (flagStatus);

		inflateEnd(&m_zstrm);

		return total;
	}



}
}
